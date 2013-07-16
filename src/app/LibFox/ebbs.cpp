#include "src/ebb/ebb.hpp"
#include "ebbs.hpp"
#include "ebb/SharedRoot.hpp"

ebbrt::EbbRoot*
ebbrt::fox::RDData::ConstructRoot()
{
  return new SharedRoot<RDData>;
}

void
ebbrt::fox::RDData::set(void * dat, int len) 
{ 
}

void
ebbrt::fox::RDData::get(void ** dat, int * len) 
{ 
}


ebbrt::EbbRoot*
ebbrt::fox::TaskQ::ConstructRoot()
{
  return new SharedRoot<TaskQ>;
}

int
ebbrt::fox::TaskQ::enque(void * data) 
{ return 0; }

void *
ebbrt::fox::TaskQ::deque() 
{ return NULL; }


ebbrt::EbbRoot*
ebbrt::fox::TaskSync::ConstructRoot()
{
  return new SharedRoot<TaskSync>;
}

void
ebbrt::fox::TaskSync::enter() 
{ }

ebbrt::EbbRoot*
ebbrt::fox::RWData::ConstructRoot()
{
  return new SharedRoot<RWData>;
}

void
ebbrt::fox::RWData::add(void *d)
{
}

void
ebbrt::fox::RWData::gather(void **data)
{
}


