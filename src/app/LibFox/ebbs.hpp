#ifndef __FOX_EBBS_H__
#define __FOX_EBBS_H__

#define STR_RDDATA    "RDDATA"
#define STR_RWDATA    "RWDATA"
#define STR_TASKS     "TASKS"
#define STR_TASK_SYNC "TASK_SYNC"


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
      virtual void add(void *data) = 0;
      virtual void gather(void **data) = 0;
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
    public:
      static EbbRoot * ConstructRoot();
      virtual void set(void * data, int len) override;
      virtual void get(void ** data, int *len) override;
    };

    class TaskQ : public Queue  {
    public:
      static EbbRoot * ConstructRoot();
      virtual int enque(void * data) override;
      virtual void * deque() override;
    };

    class TaskSync : public Sync  {
    public:
      static EbbRoot * ConstructRoot();
      virtual void enter() override;
    };

    class RWData : public GatherData {
    public:
      static EbbRoot * ConstructRoot();
      virtual void add(void *data) override;
      virtual void gather(void **data) override;
    };
  }
}


#endif
