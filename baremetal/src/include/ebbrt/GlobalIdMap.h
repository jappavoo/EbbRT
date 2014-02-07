//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef BAREMETAL_SRC_INCLUDE_EBBRT_GLOBALIDMAP_H_
#define BAREMETAL_SRC_INCLUDE_EBBRT_GLOBALIDMAP_H_

#include <ebbrt/CacheAligned.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/Net.h>
#include <ebbrt/StaticIds.h>

namespace ebbrt {
class GlobalIdMap : public CacheAligned {
 public:
  static void Init();
  static GlobalIdMap& HandleFault(EbbId id);
  void Connect(uint32_t address, uint16_t port);

 private:
  NetworkManager::TcpPcb tcp_;
};

constexpr auto global_id_map = EbbRef<GlobalIdMap>(kGlobalIdMapId);
}  // namespace ebbrt

#endif  // BAREMETAL_SRC_INCLUDE_EBBRT_GLOBALIDMAP_H_