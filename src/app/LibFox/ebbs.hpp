#ifndef __FOX_EBBS_H__
#define __FOX_EBBS_H__


#define STR_RDDATA    "RD"
#define STR_RWDATA    "WR"
#define STR_TASKS     "TK"
#define STR_TASK_SYNC "TKS"
#define STR_HASH      "HASH"

#include <unordered_map>
#include <queue>


namespace ebbrt {
  // JAHACK: not sure if we have one but 
  // adding here has a hack till we
  // talk about it
  const EbbId NULLID = 0;
  
  namespace fox {
    class Object;

    class  Value {
      char *bytes_;
      int len_;
      friend Object;
    public:
      Value() : bytes_(NULL), len_(0) {}
      char *bytes() { return bytes_; }
      int   len() { return len_; }
    };

    class Object : public EbbRep {
      Value val;
    public:
      //      Object(const char *str);
      virtual Value value() { return val; }
    };

    class Dictionary : public Object {
    public:
      virtual EbbRef<Object> get(const char *key) = 0;
      virtual void set(const char *key, EbbRef<Object> obj) = 0;
    };

    class ScatterData : public Object {
    public:
      virtual void set(const void * data, size_t len) = 0;
      virtual void *get(size_t *len) = 0;
    };

    // push/pop queue
    class Queue : public Object {
    public:
      virtual int enque(const void * data, size_t len) = 0;
      virtual void *deque(size_t *len) = 0;
    };

    // FIXME:barrier, 
    // does a sync_get for number of processors
    // sets don't block...
    class Sync : public Object {
    public:
      virtual void enter(const void * data) = 0;
    };

    class GatherData : public Object {
    public:
      virtual void add(const void * data, size_t len)=0;
      virtual void *gather(size_t *len)=0;
    };

    const EbbRef<ScatterData> theRDData =
      EbbRef<ScatterData>(lrt::trans::find_static_ebb_id(STR_RDDATA));

    const EbbRef<Queue> theTaskQ =
      EbbRef<Queue>(lrt::trans::find_static_ebb_id(STR_TASKS));

    const EbbRef<Sync> theTaskSync =
      EbbRef<Sync>(lrt::trans::find_static_ebb_id(STR_TASK_SYNC));

    const EbbRef<GatherData> theRWData =
      EbbRef<GatherData>(lrt::trans::find_static_ebb_id(STR_RWDATA));

    const EbbRef<Dictionary> theHash = 
      EbbRef<Dictionary>(lrt::trans::find_static_ebb_id(STR_HASH));
  }
}

namespace ebbrt {
  namespace fox {
    class Hash : public Dictionary {
      std::unordered_map<std::string, EbbRef<Object>> map;
    public:
      static EbbRoot * ConstructRoot();
      virtual EbbRef<Object> get(const char *key) override;
      virtual void set(const char *key, EbbRef<Object> obj) override;
    };

    class RDData : public ScatterData {
      void *buf;
      size_t buf_len;
    public:
      static EbbRoot * ConstructRoot();
      virtual void set(const void * data, size_t len) override;
      virtual void *get(size_t *len) override;
    };

    class TaskQ : public Queue  {
      struct el {
	char *ptr;
	size_t len;
	
	el(char *p, size_t l):ptr(p),len(l){}
      };
      std::queue<el> myqueue;

    public:
      static EbbRoot * ConstructRoot();
      virtual int enque(const void * data, size_t len) override;
      virtual void * deque(size_t *len) override;
    };

    class TaskSync : public Sync  {
    public:
      static EbbRoot * ConstructRoot();
      virtual void enter(const void * data) override;
    };

    class RWData : public GatherData {
      // for now, bullshit, returns back one element, the gather should return 
      // all the data, do we want to do it by returning stuff until nothing left
      // also, we need to know how many nodes will put data in to get this right. 
      struct el {
	char *ptr;
	size_t len;
	
	el(char *p, size_t l):ptr(p),len(l){}
      };
      std::queue<el> myqueue;
    public:
      static EbbRoot * ConstructRoot();
      virtual void add(const void * data, size_t len) override;
      virtual void *gather(size_t *len) override;
    };
  }
}


#endif
