#ifndef __FOX_EBBS_H__
#define __FOX_EBBS_H__

#define STR_RDDATA    "RD"
#define STR_RWDATA    "WR"
#define STR_TASKS     "TK"
#define STR_TASK_SYNC "TKS"

#include <queue>

namespace ebbrt {
  namespace fox {
    class ScatterData : public EbbRep {
    public:
      virtual void set(const void * data, size_t len) = 0;
      virtual void *get(size_t *len) = 0;
    };

    // push/pop queue
    class Queue : public EbbRep {
    public:
      virtual int enque(const void * data, size_t len) = 0;
      virtual void *deque(size_t *len) = 0;
    };

    // FIXME:barrier, 
    // does a sync_get for number of processors
    // sets don't block...
    class Sync : public EbbRep {
    public:
      virtual void enter(const void * data) = 0;
    };

    class GatherData : public EbbRep {
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

  }
}

namespace ebbrt {
  namespace fox {
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
