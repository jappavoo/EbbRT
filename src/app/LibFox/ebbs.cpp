#include "src/ebb/ebb.hpp"
#include "ebbs.hpp"
#include "ebb/SharedRoot.hpp"

#define TRACE   printf("%s:%d:%s: called from: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, (char *)data)


ebbrt::EbbRoot*
ebbrt::fox::Hash::ConstructRoot()
{
  return new SharedRoot<RDData>;
}


void 
ebbrt::fox::Hash::set(const char *key, EbbRef<ebbrt::fox::Object> o)
{
  char data[] = "HASH_SET";
  map[key] = o;
  TRACE;
}

ebbrt::lrt::trans::EbbRef<ebbrt::fox::Object> 
ebbrt::fox::Hash::get(const char * key)
{
  char data[] = "HASH_GET";
  TRACE;
  return map[key];
}

ebbrt::EbbRoot*
ebbrt::fox::RDData::ConstructRoot()
{
  return new SharedRoot<RDData>;
}

void
ebbrt::fox::RDData::set(void * data) 
{ 
  TRACE;
}


void
ebbrt::fox::RDData::get(void * data)
{ 
  TRACE;
}


ebbrt::EbbRoot*
ebbrt::fox::TaskQ::ConstructRoot()
{
  return new SharedRoot<TaskQ>;
}

int
ebbrt::fox::TaskQ::enque(void * data) 
{ 
  TRACE;
  return 0;
}

void *
ebbrt::fox::TaskQ::deque(void * data) 
{
  TRACE;
  return NULL; 
}


ebbrt::EbbRoot*
ebbrt::fox::TaskSync::ConstructRoot()
{
  return new SharedRoot<TaskSync>;
}

void
ebbrt::fox::TaskSync::enter(void * data) 
{ 
  TRACE;
}

ebbrt::EbbRoot*
ebbrt::fox::RWData::ConstructRoot()
{
  return new SharedRoot<RWData>;
}

void
ebbrt::fox::RWData::add(void * data)
{
  TRACE;
}

void
ebbrt::fox::RWData::gather(void * data)
{
  TRACE;
}


