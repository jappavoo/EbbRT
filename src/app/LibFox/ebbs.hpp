#ifndef __FOX_EBBS_H__
#define __FOX_EBBS_H__

#define STR_RDDATA    "RD"
#define STR_RWDATA    "RW"
#define STR_TASKS     "TK"
#define STR_TASK_SYNC "TKS"
#define STR_HASH      "HASH"

namespace ebbrt {
  namespace fox {
    class Dictionary : public EbbRep {
    public:
      virtual void get(void * data) = 0;
      virtual void set(void * data) = 0;
    }
    class ScatterData : public EbbRep {
    public:
      virtual void set(void * data) = 0;
      virtual void get(void * data) = 0;
    };

    class Queue : public EbbRep {
    public:
      virtual int enque(void * data) = 0;
      virtual void * deque(void * data) = 0;
    };

    class Sync : public EbbRep {
    public:
      virtual void enter(void * data) = 0;
    };

    class GatherData : public EbbRep {
    public:
      virtual void add(void * data) = 0;
      virtual void gather(void * data) = 0;
    };
#if 0
    const EbbRef<ScatterData> theRDData =
      EbbRef<ScatterData>(lrt::trans::find_static_ebb_id(STR_RDDATA));

    const EbbRef<Queue> theTaskQ =
      EbbRef<Queue>(lrt::trans::find_static_ebb_id(STR_TASKS));

    const EbbRef<Sync> theTaskSync =
      EbbRef<Sync>(lrt::trans::find_static_ebb_id(STR_TASK_SYNC));

    const EbbRef<GatherData> theRWData =
      EbbRef<GatherData>(lrt::trans::find_static_ebb_id(STR_RWDATA));
#endif
    const EbbRef<Dictionary> theHash = 
      EbbRef<Dictonary>(lrt::trans::find_static_ebb_id(STR_HASH));
  }
}

namespace ebbrt {
  namespace fox {
    class Hash : public Dictionary {
    public:
      static EbbRoot * ConstructRoot();
      virtual void set(void * data) override;
      virtual void get(void * data) override;
    };

    class RDData : public ScatterData {
    public:
      static EbbRoot * ConstructRoot();
      virtual void set(void * data) override;
      virtual void get(void * data) override;
    };

    class TaskQ : public Queue  {
    public:
      static EbbRoot * ConstructRoot();
      virtual int enque(void * data) override;
      virtual void * deque(void * data) override;
    };

    class TaskSync : public Sync  {
    public:
      static EbbRoot * ConstructRoot();
      virtual void enter(void * data) override;
    };

    class RWData : public GatherData {
    public:
      static EbbRoot * ConstructRoot();
      virtual void add(void * data) override;
      virtual void gather(void * data) override;
    };
  }
}


#endif
