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

#include <cstdlib>
#include <cstring>
#include <cassert>

extern "C" {
#include "src/app/LibFox/libfox.h"
}

#include "src/ebb/ebb.hpp"

namespace ebbrt {
  namespace fox {
    class ScatterData : public EbbRep {
    public:
      virtual void set(void * data, int len) = 0;
      virtual void get(void ** data, int *len) = 0;
    };

    class Queue : public EbbRep {
    public:
      virtual int enque(void * data) = 0;
      virtual void * deque() = 0;
    };

    class Sync : public EbbRep {
    public:
      virtual void enter() = 0;
    };

    class GatherData : public EbbRep {
    public:
      virtual void add() = 0;
      virtual void gather() = 0;
    };

    const EbbRef<ScatterData> theRDData =
      EbbRef<ScatterData>(lrt::trans::find_static_ebb_id("RDDATA"));

    const EbbRef<Queue> theTaskQ =
      EbbRef<Queue>(lrt::trans::find_static_ebb_id("TASKS"));

    const EbbRef<Sync> theTaskSync =
      EbbRef<Sync>(lrt::trans::find_static_ebb_id("TASK_SYNC"));

    const EbbRef<GatherData> theRWData =
      EbbRef<GatherData>(lrt::trans::find_static_ebb_id("RWDATA"));

  }
}

// in the middle 
#if 0
namespace ebbrt {
  namespace fox {
    class RDData : public ScatterData {
    public:
      virtual void set(void * data, int len);
      virtual void get(void ** data, int *len);
    };

    class Queue : public EbbRep {
    public:
      virtual int enque(void * data);
      virtual void * deque();
    };

    class Sync : public EbbRep {
    public:
      virtual void enter() = 0;
    };

    class GatherData : public EbbRep {
    public:
      virtual void add() = 0;
      virtual void gather() = 0;
    };
  }
}
#endif

struct fox_st {
};

extern "C"
int
fox_new(fox_ptr* fhand_ptr, int nprocs, int procid)
{
  return 0;
}

extern "C"
int
fox_free(fox_ptr fhand)
{
  return 0;
}

extern "C"
int
fox_flush(fox_ptr fhand, int term)
{
  return 0;
}

extern "C"
int
fox_server_add(fox_ptr fhand, const char *hostlist)
{
  return 0;
}

extern "C"
int
fox_set(fox_ptr fhand,
        const char *key, size_t key_sz,
        const char *value, size_t value_sz)
{
  return 0;
}

extern "C"
int
fox_get(fox_ptr fhand,
        const char *key, size_t key_sz,
        char **pvalue, size_t *pvalue_sz)
{
  return 0;
}

extern "C"
int
fox_delete(fox_ptr fhand, const char* key, size_t key_sz)
{
  assert(0);
  return 0;
}

extern "C"
int
fox_sync_set(fox_ptr fhand, unsigned delta,
             const char* key, size_t key_sz,
             const char* value, size_t value_sz)
{
  //FIXME: no semaphore stuff
  return fox_set(fhand, key, key_sz, value, value_sz);
}

extern "C"
int
fox_sync_get(fox_ptr fhand, unsigned delta,
             const char *key, size_t key_sz,
             char **pvalue, size_t *pvalue_sz)
{
  //FIXME: no semaphore stuff
  return 0;
}

extern "C"
int
fox_broad_set(fox_ptr fhand,
              const char *key, size_t key_sz,
              const char *value, size_t value_sz)
{
  //FIXME: no broadcast stuff
  return 0;
}

extern "C"
int
fox_broad_get(fox_ptr fhand,
              const char *key, size_t key_sz,
              char **pvalue, size_t *pvalue_sz)
{
  //FIXME: no broadcast stuff
  return 0;
}

extern "C"
int
fox_queue_set(fox_ptr fhand,
              const char *key, size_t key_sz,
              const char *value, size_t value_sz)
{
  return 0;
}

extern "C"
int
fox_queue_get(fox_ptr fhand,
              const char *key, size_t key_sz,
              char **pvalue, size_t *pvalue_sz)
{
  return 0;
}

extern "C"
int
fox_broad_queue_set(fox_ptr fhand,
                    const char *key, size_t key_sz,
                    const char *value, size_t value_sz)
{
  return 0;
}

extern "C"
int
fox_dist_queue_set(fox_ptr fhand,
                   const char *key, size_t key_sz,
                   const char *value, size_t value_sz)
{
  return 0;
}

extern "C"
int
fox_dist_queue_get(fox_ptr fhand,
                   const char *key, size_t key_sz,
                   char **pvalue, size_t *pvalue_sz)
{
  return 0;
}

extern "C"
int
fox_reduce_set(fox_ptr fhand,
               const char *key, size_t key_sz,
               const char *value, size_t value_sz)
{
  return 0;
}

extern "C"
int
fox_reduce_get(fox_ptr fhand,
               const char *key, size_t key_sz,
               char *pvalue, size_t pvalue_sz,
               void (*reduce)(void *out, void *in))
{
  return 0;
}

extern "C"
int
fox_collect(fox_ptr fhand,
            const char *key, size_t key_sz,
            int root, int opt)
{
  return 0;
}
