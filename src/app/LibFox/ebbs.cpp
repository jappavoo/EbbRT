#include "src/ebb/ebb.hpp"
#include "ebbs.hpp"
#include "ebb/SharedRoot.hpp"
#include <cstring>

#define TRACE   printf("%s: called from: %s\n", __func__, (char *)data)

ebbrt::EbbRoot*
ebbrt::fox::RDData::ConstructRoot()
{
  return new SharedRoot<RDData>;
}

void
ebbrt::fox::RDData::set(const void * data, size_t len) 
{ 
  buf = malloc(len);
  memcpy(buf, data, len);
  buf_len = len;
}


void *
ebbrt::fox::RDData::get(size_t *len)
{ 
  void *data = buf;
  *len = buf_len;
  return data;
}


ebbrt::EbbRoot*
ebbrt::fox::TaskQ::ConstructRoot()
{
  return new SharedRoot<TaskQ>;
}

int
ebbrt::fox::TaskQ::enque(const void * data, size_t len) 
{ 
  char *p;
  p = (char *)malloc(len);
  memcpy(p, data, len);
  myqueue.emplace(p, len);
  return 0;
}

void *
ebbrt::fox::TaskQ::deque(size_t *len) 
{
  void *data;
  struct el &hd = myqueue.front();
  // note, data allocated at start, returns pointer freed by 
  // callee
  data = hd.ptr;
  *len = hd.len;
  myqueue.pop();
  return data; 
}


ebbrt::EbbRoot*
ebbrt::fox::TaskSync::ConstructRoot()
{
  return new SharedRoot<TaskSync>;
}

void
ebbrt::fox::TaskSync::enter(const void * data) 
{ 
  TRACE;
}

ebbrt::EbbRoot*
ebbrt::fox::RWData::ConstructRoot()
{
  return new SharedRoot<RWData>;
}

void
ebbrt::fox::RWData::add(const void * data, size_t len)
{
  TRACE;
  char *p;
  p = (char *)malloc(len);
  memcpy(p, data, len);
  myqueue.emplace(p, len);
  return ;
}

// FIXME: this should return all the data from all the adds
void *
ebbrt::fox::RWData::gather(size_t *len)
{
  struct el &hd = myqueue.front();
  void *data;

  // TRACE;
  // note, data allocated at start, returns pointer freed by 
  // callee
  data = hd.ptr;
  *len = hd.len;
  myqueue.pop();
  return data; 
}


