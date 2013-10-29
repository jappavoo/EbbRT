/*
  EbbRT: Distributed, Elastic, Runtime
  Copyright (C) 2013 SESA Group, Boston University

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef EBBRT_EBB_SHAREDROOT_HPP
#define EBBRT_EBB_SHAREDROOT_HPP

#include "arch/args.hpp"
#include "ebb/ebb.hpp"
#include "ebb/EbbManager/EbbManager.hpp"
#include "sync/spinlock.hpp"

namespace ebbrt {
  template<int ...>
  struct seq {};

  template <int N, int ...S>
  struct gens : gens<N-1, N-1, S...> {};

  template <int ...S>
  struct gens<0, S...> {
    typedef seq<S...> type;
  };

  template <typename T, typename... CtorArgs>
  /**
   * @brief Shared Ebb Root
   * A single ebb representaive is shared between all locations
   */
  class SharedRoot : public EbbRoot {
  public:
    SharedRoot(CtorArgs... args) : rep_{nullptr}, tup_{std::move(args)...}
    {
    }
    /**
     * @brief Handle the construction of the rep. This gets called on a
     * miss to cache the local representative.
     *
     * @param args Ebb call arguments
     * @param fnum Ebb call function number
     * @param fret Return value address
     * @param id   Ebb Id
     *
     * @return
     */
    bool PreCall(Args* args, ptrdiff_t fnum,
                 lrt::trans::FuncRet* fret, EbbId id) override
    {
      if (rep_ == nullptr) {
        lock_.Lock();
        if (rep_ == nullptr) {
          call_new(id, typename gens<sizeof...(CtorArgs)>::type());
        }
        lock_.Unlock();
      }
      ebb_manager->CacheRep(id, rep_);

      args->this_pointer() = reinterpret_cast<uintptr_t>(rep_);

      // rep is a pointer to pointer to array 256 of pointer to
      // function returning void
      void (*(**rep)[256])() = reinterpret_cast<void (*(**)[256])()>(rep_);
      fret->func = (**rep)[fnum];
      return true;
    }
    /**
     * @brief Ebb construction post-call.
     *
     * @param ret ebb calls return
     *
     * @return Return ebb call return value
     */
    void* PostCall(void* ret) override
    {
      return ret;
    }
  private:
    template<int ...S>
    void call_new(EbbId id, seq<S...>) {
      rep_ = new T(id, std::get<S>(tup_)...);
    }

    /**
     * @brief Our single shared representative
     */
    T* rep_;
    std::tuple<CtorArgs...> tup_;
    Spinlock lock_;
  };
}

#endif
