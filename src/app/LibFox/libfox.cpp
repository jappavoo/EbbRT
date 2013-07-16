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
#include "ebbs.hpp"

#define TRACE printf("%s:%d:%s\n", __FILE__, __LINE__,__func__)

struct fox_st {
  ebbrt::EbbRT instance;
  ebbrt::Context context{instance};
};

extern "C"
int
fox_new(fox_ptr* fhand_ptr, int nprocs, int procid)
{
  struct fox_st *fh = new fox_st;

  TRACE;
  fh->context.Activate();  
  ebbrt::fox::theRDData->set((void *)__func__);
  *fhand_ptr = fh;
  return 0;
}

extern "C"
int
fox_free(fox_ptr fhand)
{
  TRACE;
  return 0;
}

extern "C"
int
fox_flush(fox_ptr fhand, int term)
{
  TRACE;
  return 0;
}

extern "C"
int
fox_server_add(fox_ptr fhand, const char *hostlist)
{
  TRACE;
  return 0;
}

extern "C"
int
fox_set(fox_ptr fhand,
        const char *key, size_t key_sz,
        const char *value, size_t value_sz)
{
  TRACE;
  assert(0);
  return 0;
}

extern "C"
int
fox_get(fox_ptr fhand,
        const char *key, size_t key_sz,
        char **pvalue, size_t *pvalue_sz)
{
  TRACE;
  assert(0);
  return 0;
}

extern "C"
int
fox_delete(fox_ptr fhand, const char* key, size_t key_sz)
{
  TRACE;
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
  TRACE;
  assert(strcmp(key,STR_TASK_SYNC));
  ebbrt::fox::theTaskSync->enter((void *)__func__);
  return 0;
}

extern "C"
int
fox_sync_get(fox_ptr fhand, unsigned delta,
             const char *key, size_t key_sz,
             char **pvalue, size_t *pvalue_sz)
{
  //FIXME: no semaphore stuff
  TRACE;
  assert(strcmp(key,STR_TASK_SYNC)==0);
  ebbrt::fox::theTaskSync->enter((void *)__func__);
  return 0;
}

extern "C"
int
fox_broad_set(fox_ptr fhand,
              const char *key, size_t key_sz,
              const char *value, size_t value_sz)
{
  //FIXME: no broadcast stuff
  TRACE;
  assert(strcmp(key,STR_RDDATA)==0);
  ebbrt::fox::theRDData->set((void *)__func__);
  return 0;
}

extern "C"
int
fox_broad_get(fox_ptr fhand,
              const char *key, size_t key_sz,
              char **pvalue, size_t *pvalue_sz)
{
  //FIXME: no broadcast stuff
  TRACE;
  assert(strcmp(key,STR_RDDATA)==0);
  ebbrt::fox::theRDData->get((void *)__func__);
  return 0;
}

extern "C"
int
fox_queue_set(fox_ptr fhand,
              const char *key, size_t key_sz,
              const char *value, size_t value_sz)
{
  TRACE;
  assert(strcmp(key,STR_TASKS)==0);
  ebbrt::fox::theTaskQ->enque((void *)__func__);
  return 0;
}

extern "C"
int
fox_queue_get(fox_ptr fhand,
              const char *key, size_t key_sz,
              char **pvalue, size_t *pvalue_sz)
{
  TRACE;
  assert(strcmp(key,STR_TASKS)==0);
  ebbrt::fox::theTaskQ->deque((void *)__func__);
  return 0;
}

extern "C"
int
fox_broad_queue_set(fox_ptr fhand,
                    const char *key, size_t key_sz,
                    const char *value, size_t value_sz)
{
  TRACE;
  assert(strcmp(key,STR_TASKS)==0);
  ebbrt::fox::theTaskQ->enque((void *)__func__);
  return 0;
}

extern "C"
int
fox_dist_queue_set(fox_ptr fhand,
                   const char *key, size_t key_sz,
                   const char *value, size_t value_sz)
{
  TRACE;
  assert(strcmp(key,STR_TASKS)==0);
  ebbrt::fox::theTaskQ->enque((void *)__func__);
  return 0;
}

extern "C"
int
fox_dist_queue_get(fox_ptr fhand,
                   const char *key, size_t key_sz,
                   char **pvalue, size_t *pvalue_sz)
{
  TRACE;
  assert(strcmp(key,STR_TASKS)==0);
  ebbrt::fox::theTaskQ->deque((void *)__func__);
  return 0;
}

extern "C"
int
fox_reduce_set(fox_ptr fhand,
               const char *key, size_t key_sz,
               const char *value, size_t value_sz)
{
  TRACE;
  assert(strcmp(key,STR_RWDATA)==0);
  ebbrt::fox::theRWData->add((void *)__func__);
  return 0;
}

extern "C"
int
fox_reduce_get(fox_ptr fhand,
               const char *key, size_t key_sz,
               char *pvalue, size_t pvalue_sz,
               void (*reduce)(void *out, void *in))
{
  TRACE;
  assert(strcmp(key,STR_RWDATA)==0);
  ebbrt::fox::theRWData->gather((void *)__func__);
  return 0;
}

extern "C"
int
fox_collect(fox_ptr fhand,
            const char *key, size_t key_sz,
            int root, int opt)
{
  TRACE;
  assert(strcmp(key,STR_RWDATA)==0);
  ebbrt::fox::theRWData->gather((void *)__func__);
  return 0;
}
