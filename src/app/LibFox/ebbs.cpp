#include "src/ebb/ebb.hpp"
#include "ebbs.hpp"
#include "ebb/SharedRoot.hpp"
#include <cstring>

#define TRACE   printf("> %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


ebbrt::EbbRoot*
ebbrt::fox::Hash::ConstructRoot()
{
  return new SharedRoot<Hash>;
}


ebbrt::fox::Hash::Hash()
{
  //FIXME:  Not really sure I like public constructors when then 
  //        reps really should only be constructed by the root

  // for completelness we put ourselves in the dictionary
  set(STR_HASH, static_cast<EbbRef<Object>>(theHash));

  // put the rest of the well known instances in the dictionary
  set(STR_RDDATA, static_cast<EbbRef<Object>>(theRDData));
  set(STR_TASKS, static_cast<EbbRef<Object>>(theTaskQ));
  set(STR_TASK_SYNC, static_cast<EbbRef<Object>>(theTaskSync));
  set(STR_RWDATA, static_cast<EbbRef<Object>>(theRDData)); 
}

void 
ebbrt::fox::Hash::set(const char *key, EbbRef<ebbrt::fox::Object> o)
{
  map[key] = o;
  TRACE;
}

ebbrt::lrt::trans::EbbRef<ebbrt::fox::Object> 
ebbrt::fox::Hash::get(const char * key)
{
  TRACE;
  std::unordered_map<std::string,EbbRef<Object>>::const_iterator got = map.find(key);
  if ( got == map.end() )
    return EbbRef<Object>(NULLID);
  else
    return got->second;
}

ebbrt::EbbRoot*
ebbrt::fox::RDData::ConstructRoot()
{
  return new SharedRoot<RDData>;
}

ebbrt::EbbRef<ebbrt::fox::RDData>
ebbrt::fox::RDData::Create()
{
  EbbRef<RDData> ref = EbbRef<RDData>(ebb_manager->AllocateId());
  ebb_manager->Bind(ConstructRoot, ref);
  return ref;
}

void
ebbrt::fox::RDData::set(const void * data, size_t len) 
{ 
  assert(val_.bytes_ == 0 && val_.len_ == 0);
  val_.bytes_ = malloc(len);
  memcpy(val_.bytes_, data, len);
  val_.len_ = len;
}


void *
ebbrt::fox::RDData::get(size_t *len)
{ 
  *len = val_.len_;
  return val_.bytes_;
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


