#ifndef __FOX_EBBS_H__
#define __FOX_EBBS_H__


#define STR_RDDATA    "RD"
#define STR_RWDATA    "RW"
#define STR_TASKS     "TK"
#define STR_TASK_SYNC "TKS"
#define STR_HASH      "HASH"

namespace ebbrt {
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
      virtual void set(void * data) = 0;
      virtual void get(void * data) = 0;
    };

    class Queue : public Object {
    public:
      virtual int enque(void * data) = 0;
      virtual void * deque(void * data) = 0;
    };

    class Sync : public Object {
    public:
      virtual void enter(void * data) = 0;
    };

    class GatherData : public Object {
    public:
      virtual void add(void * data) = 0;
      virtual void gather(void * data) = 0;
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
