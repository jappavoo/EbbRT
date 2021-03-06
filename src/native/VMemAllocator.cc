//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include "VMemAllocator.h"

#include <cinttypes>
#include <cxxabi.h>
#include <mutex>

#include "../Align.h"
#include "Cpu.h"
#include "LocalIdMap.h"

#include "PMem.h"

namespace {

uintptr_t ReadCr2() {
  uintptr_t cr2;
  asm volatile("mov %%cr2, %[cr2]" : [cr2] "=r"(cr2));
  return cr2;
}
}

/* initialize VMemAllocator */
void ebbrt::VMemAllocator::Init() {
  auto rep = new VMemAllocator;
  local_id_map->Insert(
      std::make_pair(kVMemAllocatorId, boost::any(std::ref(*rep))));
}

ebbrt::VMemAllocator& ebbrt::VMemAllocator::HandleFault(EbbId id) {
  kassert(id == kVMemAllocatorId);
  LocalIdMap::ConstAccessor accessor;
  auto found = local_id_map->Find(accessor, id);
  kassert(found);
  auto ref =
      boost::any_cast<std::reference_wrapper<VMemAllocator>>(accessor->second);
  EbbRef<VMemAllocator>::CacheRef(id, ref.get());
  return ref;
}

/* construct VMemAllocator */
ebbrt::VMemAllocator::VMemAllocator() {
  regions_.emplace(std::piecewise_construct,
                   std::forward_as_tuple(Pfn::Up(kVMemRangeStart)),
                   std::forward_as_tuple(Pfn::Down(kVMemRangeEnd)));
}

ebbrt::Pfn
ebbrt::VMemAllocator::Alloc(size_t npages,
                            std::unique_ptr<PageFaultHandler> pf_handler) {

  std::lock_guard<SpinLock> lock(lock_);

  for (auto it = regions_.begin(); it != regions_.end(); ++it) {
    /* it->first: Pfn start_addr */
    const auto& begin = it->first;
    /* it->second: Region, it->second.end(): Pfn end_addr  */
    auto end = it->second.end();

    /* if Region is not free or not large enough, try another Region */
    if (!it->second.IsFree() || end - begin < npages) {
      continue;
    }
    /* if Region is free and large enough, allocate full or partial Region */
    auto ret = end - npages;
    if (ret == begin) {
      it->second.set_page_fault_handler(std::move(pf_handler));
      it->second.set_allocated(true);
    } else {
      /* new region starts at ret and ends at ret + npages */
      it->second.set_end(end - npages);
      auto p =
          regions_.emplace(std::piecewise_construct, std::forward_as_tuple(ret),
                           std::forward_as_tuple(ret + npages));
      kassert(p.second);
      p.first->second.set_page_fault_handler(std::move(pf_handler));
      p.first->second.set_allocated(true);
    }

    kprintf("Allocated %llx - %llx\n", ret.ToAddr(),
            (ret + npages).ToAddr() - 1);
    return ret;
  }
  kabort("%s: unable to allocate %llu virtual pages\n", __PRETTY_FUNCTION__,
         npages);
}

ebbrt::Pfn
ebbrt::VMemAllocator::AllocRange(size_t npages, uintptr_t vmem_start,
                                 std::unique_ptr<PageFaultHandler> pf_handler) {

  if (!npages || !vmem_start || pf_handler == nullptr) {
    kprintf("%s:\nUnable to reserve virtual pages; Incomplete range/handler.\n",
            __PRETTY_FUNCTION__);
    return Pfn::None();
  }

  std::lock_guard<SpinLock> lock(lock_);
  uintptr_t vmem_end = vmem_start + (pmem::kPageSize * npages) - 1;

  /* Check requested range is valid */
  if (vmem_start < kVMemRangeStart || vmem_end > kVMemRangeEnd ||
      vmem_start >= vmem_end) {
    kprintf("Unable to reserve virtual pages; Requested address is outside the "
            "virtual range\n");
    return Pfn::None();
  }
  if (vmem_start & (pmem::kPageSize - 1)) {
    kprintf("Unable to reserve virtual pages; Requested address is not page "
            "aligned\n");
    return Pfn::None();
  }

  kprintf("\nVMem Range Requested:\t 0x%llx - 0x%llx\n", vmem_start, vmem_end);

  /* find requested range within regions_ */
  for (auto it = regions_.begin(); it != regions_.end(); ++it) {
    /* it->first: Pfn start_addr */
    const auto& begin = it->first;
    /* it->second: Region, it->second.end(): Pfn end_addr  */
    auto end = it->second.end();
    kprintf("Checking range from 0x%llx to 0x%llx\n", begin.ToAddr(),
            end.ToAddr());

    /* Requested range is outside this Region, try another Region*/
    if (begin.ToAddr() > vmem_end) {
      continue;
    }

    /* Check if Region is free and reserve it*/
    if (begin.ToAddr() <= vmem_start && end.ToAddr() >= vmem_end) {
      if (!it->second.IsFree()) {
        kprintf("Unable to reserve virtual pages; Region not free\n");
        return Pfn::None();
      }

      auto vmem_start_vfn = Pfn(vmem_start >> pmem::kPageShift);
      auto vmem_end_vfn = Pfn(vmem_start_vfn + npages + 1);

      if (begin.ToAddr() == vmem_start) {
        it->second.set_end(vmem_end_vfn);
        it->second.set_page_fault_handler(std::move(pf_handler));
        it->second.set_allocated(true);
      } else {
        kassert(begin.ToAddr() < vmem_start);
        it->second.set_end(Pfn::Down(vmem_start));  // XXX(jmcadden): off by 1?

        /* Create new Region for requested range */
        auto p = regions_.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(vmem_start_vfn),
                                  std::forward_as_tuple(vmem_end_vfn));
        kassert(p.second);
        p.first->second.set_page_fault_handler(std::move(pf_handler));
        p.first->second.set_allocated(true);
      }
      kprintf("VMem Range Reserved:\t %llx - %llx\n\n", vmem_start_vfn.ToAddr(),
              vmem_end_vfn.ToAddr() - 1);
      /* Create a Region for the extra */
      if (vmem_end_vfn < end) {
        regions_.emplace(std::piecewise_construct,
                         std::forward_as_tuple(vmem_end_vfn),
                         std::forward_as_tuple(end));
      }
      return vmem_start_vfn;
    }
  }
  kprintf("Unable to reserve virtual pages; No suitable Region found\n");
  return Pfn::None();
}

ebbrt::Pfn
ebbrt::VMemAllocator::Alloc(size_t npages, size_t pages_align,
                            std::unique_ptr<PageFaultHandler> pf_handler) {
  std::lock_guard<SpinLock> lock(lock_);
  for (auto it = regions_.begin(); it != regions_.end(); ++it) {
    const auto& begin = it->first;
    auto end = it->second.end();
    if (!it->second.IsFree() || end - begin < npages)
      continue;

    auto ret = Pfn(align::Down(end.val() - npages, pages_align));

    if (ret < begin)
      continue;

    auto endsize = (end - ret) - npages;

    if (ret == begin) {
      it->second.set_page_fault_handler(std::move(pf_handler));
      it->second.set_allocated(true);
    } else {
      it->second.set_end(ret);
      auto p =
          regions_.emplace(std::piecewise_construct, std::forward_as_tuple(ret),
                           std::forward_as_tuple(ret + npages));
      kassert(p.second);
      p.first->second.set_page_fault_handler(std::move(pf_handler));
      p.first->second.set_allocated(true);
    }

    if (endsize > 0) {
      regions_.emplace(std::piecewise_construct,
                       std::forward_as_tuple(ret + npages),
                       std::forward_as_tuple(end));
    }

    return ret;
  }
  kabort("%s: unable to allocate %llu virtual pages\n", __PRETTY_FUNCTION__,
         npages);
}

void ebbrt::VMemAllocator::HandlePageFault(idt::ExceptionFrame* ef) {
  std::lock_guard<SpinLock> lock(lock_);
  auto fault_addr = ReadCr2();

  if (fault_addr >= trans::kVMemStart) {
    trans::HandleFault(ef, fault_addr);
  } else {
    auto it = regions_.lower_bound(Pfn::Down(fault_addr));
    if (it == regions_.end() || it->second.end() < Pfn::Up(fault_addr)) {
      kprintf_force("Page fault for address %llx, no handler for it\n",
                    fault_addr);
      kprintf_force("SS: %#018" PRIx64 " RSP: %#018" PRIx64 "\n", ef->ss,
                    ef->rsp);
      kprintf_force("FLAGS: %#018" PRIx64 "\n",
                    ef->rflags);  // TODO(Dschatz): print out actual meaning
      kprintf_force("CS: %#018" PRIx64 " RIP: %#018" PRIx64 "\n", ef->cs,
                    ef->rip);
      kprintf_force("Error Code: %" PRIx64 "\n", ef->error_code);
      kprintf_force("RAX: %#018" PRIx64 " RBX: %#018" PRIx64 "\n", ef->rax,
                    ef->rbx);
      kprintf_force("RCX: %#018" PRIx64 " RDX: %#018" PRIx64 "\n", ef->rcx,
                    ef->rdx);
      kprintf_force("RSI: %#018" PRIx64 " RDI: %#018" PRIx64 "\n", ef->rsi,
                    ef->rdi);
      kprintf_force("RBP: %#018" PRIx64 " R8:  %#018" PRIx64 "\n", ef->rbp,
                    ef->r8);
      kprintf_force("R9:  %#018" PRIx64 " R10: %#018" PRIx64 "\n", ef->r9,
                    ef->r10);
      kprintf_force("R11: %#018" PRIx64 " R12: %#018" PRIx64 "\n", ef->r11,
                    ef->r12);
      kprintf_force("R13: %#018" PRIx64 " R14: %#018" PRIx64 "\n", ef->r13,
                    ef->r14);
      kprintf_force("R15: %#018" PRIx64 "\n", ef->r15);
      // TODO(dschatz): FPU
      kabort();
    }
    it->second.HandleFault(ef, fault_addr);
  }
}

extern "C" void ebbrt::idt::PageFaultException(ExceptionFrame* ef) {
  vmem_allocator->HandlePageFault(ef);
}

