//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ebbrt/Rcu.h>

void AppMain() {
  ebbrt::kprintf("call rcu\n");
  ebbrt::CallRCU([]() { ebbrt::kprintf("Rcu task!\n"); });
}
