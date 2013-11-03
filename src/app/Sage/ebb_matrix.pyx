from cython.operator cimport dereference as deref

cdef extern from "ebb/EventManager/Future.hpp" namespace "ebbrt":
    cdef cppclass Future[T]:
        pass

cdef extern from "app/Sage/ebb_matrix_helper.hpp":
    void activate_context()
    void deactivate_context()
    void wait_for_future_void "wait_for_future<void>"(Future[void]*)
    double wait_for_future_double "wait_for_future<double>"(Future[double]*)

cdef extern from "inttypes.h":
    ctypedef unsigned int uint32_t

cdef extern from "lrt/EbbId.hpp" namespace "ebbrt::lrt::trans":
    ctypedef uint32_t EbbId

cdef extern from "lrt/EbbRef.hpp" namespace "ebbrt::lrt::trans":
    cdef cppclass EbbRef[T]:
        EbbRef()
        EbbRef(EbbId id)
        T* operator*()
        EbbId operator()

cdef extern from "lrt/EbbRoot.hpp" namespace "ebbrt::lrt::trans":
    cdef cppclass EbbRoot

cdef extern from "ebb/EbbManager/EbbManager.hpp" namespace "ebbrt":
    cdef cppclass EbbManager:
        EbbId AllocateId()
        void Bind(EbbRoot* (*factory)(), EbbId id)
    EbbRef[EbbManager] ebb_manager

cdef extern from "app/Sage/Matrix.hpp" namespace "ebbrt":
    cdef cppclass Matrix:
        Future[void] Randomize()
        Future[double] Get(int, int)
        Future[void] Set(int, int, double)
        Future[double] Sum()
        void Destroy()

cdef extern from "app/Sage/LocalMatrix.hpp" namespace "ebbrt":
    cdef cppclass LocalMatrix:
        Future[void] Randomize()
        Future[double] Get(int, int)
        Future[void] Set(int, int, double)
        Future[double] Sum()

cdef extern from "app/Sage/Matrix.hpp" namespace "ebbrt::Matrix":
    EbbRoot* ConstructRoot()
    void SetSize(EbbId, int)

cdef extern from "app/Sage/LocalMatrix.hpp" namespace "ebbrt::LocalMatrix":
    EbbRoot* LocalConstructRoot "ebbrt::LocalMatrix::ConstructRoot" ()
    void LocalSetSize "ebbrt::LocalMatrix::SetSize" (EbbId, int)

cdef extern from "ebbrt.hpp" namespace "ebbrt":
    cdef cppclass EbbRT:
        EbbRT()
    cdef cppclass Context:
        Context(EbbRT& instance)
        void Activate()
        void Deactivate()

cdef class EbbMatrix:
    cdef EbbRef[Matrix] matrix
    cdef int size
    def __cinit__(self, size):
        activate_context()
        cdef EbbManager* manager = deref(ebb_manager)
        self.matrix = <EbbRef[Matrix]>manager.AllocateId()
        manager.Bind(ConstructRoot, <EbbId>self.matrix)
        SetSize(<EbbId>self.matrix, size)
        deactivate_context()
    def __init__(self, size):
        self.size = size
    def randomize(self):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Randomize()
        wait_for_future_void(&fut)
        deactivate_context()
    def sum(self):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Sum()
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        return ret
    def __getitem__(self, pos):
        row,column = pos
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Get(row, column)
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        return ret
    def __setitem__(self, pos, value):
        row,column = pos
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Set(row, column, value)
        wait_for_future_void(&fut)
        deactivate_context()
    def __iter__(self):
        for row in range(self.size):
            for column in range(self.size):
                yield self.__getitem__((row, column))
    def __dealloc__(self):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        ref.Destroy()
        deactivate_context()

cdef class LocalEbbMatrix:
    cdef EbbRef[LocalMatrix] matrix
    cdef int size
    def __cinit__(self, size):
        activate_context()
        cdef EbbManager* manager = deref(ebb_manager)
        self.matrix = <EbbRef[LocalMatrix]>manager.AllocateId()
        manager.Bind(LocalConstructRoot, <EbbId>self.matrix)
        LocalSetSize(<EbbId>self.matrix, size)
        deactivate_context()
    def __init__(self, size):
        self.size = size
    def randomize(self):
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Randomize()
        wait_for_future_void(&fut)
        deactivate_context()
    def sum(self):
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Sum()
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        return ret
    def __getitem__(self, pos):
        row,column = pos
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Get(row, column)
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        return ret
    def __setitem__(self, pos, value):
        row,column = pos
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Set(row, column, value)
        wait_for_future_void(&fut)
        deactivate_context()
    def __iter__(self):
        for row in range(self.size):
            for column in range(self.size):
                yield self.__getitem__((row, column))

cimport sage.matrix.matrix_double_dense

from sage.rings.real_double import RDF
from sage.matrix.matrix_dense import Matrix_dense

cimport numpy as cnumpy

numpy=None
scipy=None

cdef class Matrix_ebblocal_real_double_dense(sage.matrix.matrix_double_dense.Matrix_double_dense):
    cdef EbbRef[LocalMatrix] matrix

    def __cinit__(self, parent, entries, copy, coerce):
        global numpy
        if numpy is None:
            import numpy
        self._numpy_dtype = numpy.dtype('float64')
        self._numpy_dtypeint = cnumpy.NPY_DOUBLE
        self._python_dtype = float
        # TODO: Make RealDoubleElement instead of RDF for speed
        self._sage_dtype = RDF
        # this is kludge for the moment we preallocate a numpy array for interfacing
        # to local computation if needed.
        # self.__create_matrix__()

        activate_context()
        cdef EbbManager* manager = deref(ebb_manager)
        self.matrix = <EbbRef[LocalMatrix]>manager.AllocateId()
        manager.Bind(LocalConstructRoot, <EbbId>self.matrix)
        LocalSetSize(<EbbId>self.matrix, self._ncols)
        deactivate_context()

        return

    def __init__(self, parent, entries, copy, coerce):
       return

    cdef set_unsafe(self, Py_ssize_t i, Py_ssize_t j, object value):
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Set(i, j, value)
        wait_for_future_void(&fut)
        deactivate_context()

    cdef set_unsafe_double(self, Py_ssize_t i, Py_ssize_t j, double value):
        """
        Set the (i,j) entry to value without any type checking or
        bound checking.

        This currently isn't faster than calling self.set_unsafe; should
        we speed it up or is it just a convenience function that has the
        right headers?
        """
        self.set_unsafe(i,j,value)

    cdef get_unsafe(self, Py_ssize_t i, Py_ssize_t j):
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Get(i,j)
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        return self._sage_dtype(ret)

    cdef double get_unsafe_double(self, Py_ssize_t i, Py_ssize_t j):
        """
        Get the (i,j) entry without any type checking or bound checking.

        This currently isn't faster than calling self.get_unsafe; should
        we speed it up or is it just a convenience function that has the
        right headers?
        """
        return self.get_unsafe(i,j)

    def __dealloc__(self):
        pass

    def change_ring(self, ring):
        print "ebb: change_ring called"
        print ring
        return self

    def numpy(self, dtype=None):
        import numpy as np
        print "ebb: numpy: this is a kludge right now need to GATHER values into our local numpy cache"
        print dtype

        if dtype is None or self._numpy_dtype == np.dtype(dtype):
            return self._matrix_numpy.copy()
        else:
            return Matrix_dense.numpy(self, dtype=dtype)

    def randomize(self, density=1, nonzero=False, *args, **kwds):
        activate_context()
        cdef LocalMatrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Randomize()
        wait_for_future_void(&fut)
        deactivate_context()

cdef class Matrix_ebbremote_real_double_dense(sage.matrix.matrix_double_dense.Matrix_double_dense):
    cdef EbbRef[Matrix] matrix
    cdef int            accessed
    def __cinit__(self, parent, entries, copy, coerce):
        global numpy
        if numpy is None:
            import numpy
        self._numpy_dtype = numpy.dtype('float64')
        self._numpy_dtypeint = cnumpy.NPY_DOUBLE
        self._python_dtype = float
        # TODO: Make RealDoubleElement instead of RDF for speed
        self._sage_dtype = RDF
        # this is kludge for the moment we preallocate a numpy array for interfacing
        # to local computation if needed.
        # self.__create_matrix__()

        activate_context()
        cdef EbbManager* manager = deref(ebb_manager)
        self.matrix = <EbbRef[Matrix]>manager.AllocateId()
        manager.Bind(ConstructRoot, <EbbId>self.matrix)
        SetSize(<EbbId>self.matrix, self._ncols)
        deactivate_context()
        self.accessed = 0
        return

    def __init__(self, parent, entries, copy, coerce):
       return

    cdef set_unsafe(self, Py_ssize_t i, Py_ssize_t j, object value):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Set(i, j, value)
        wait_for_future_void(&fut)
        deactivate_context()
        self.accessed = 1

    cdef set_unsafe_double(self, Py_ssize_t i, Py_ssize_t j, double value):
        """
        Set the (i,j) entry to value without any type checking or
        bound checking.

        This currently isn't faster than calling self.set_unsafe; should
        we speed it up or is it just a convenience function that has the
        right headers?
        """
        self.set_unsafe(i,j,value)

    cdef get_unsafe(self, Py_ssize_t i, Py_ssize_t j):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[double] fut = ref.Get(i,j)
        cdef double ret = wait_for_future_double(&fut)
        deactivate_context()
        self.accessed = 1
        return self._sage_dtype(ret)

    cdef double get_unsafe_double(self, Py_ssize_t i, Py_ssize_t j):
        """
        Get the (i,j) entry without any type checking or bound checking.

        This currently isn't faster than calling self.get_unsafe; should
        we speed it up or is it just a convenience function that has the
        right headers?
        """
        return self.get_unsafe(i,j)

    def __dealloc__(self):
        cdef Matrix* ref
        if self.accessed == 1:
          activate_context()
          ref = deref(self.matrix)
          ref.Destroy()
          deactivate_context()

    def change_ring(self, ring):
        print "ebb: change_ring called"
        print ring
        return self

    def numpy(self, dtype=None):
        import numpy as np
        print "ebb: numpy: this is a kludge right now need to GATHER values into our local numpy cache"
        print dtype

        if dtype is None or self._numpy_dtype == np.dtype(dtype):
            return self._matrix_numpy.copy()
        else:
            return Matrix_dense.numpy(self, dtype=dtype)

    def randomize(self, density=1, nonzero=False, *args, **kwds):
        activate_context()
        cdef Matrix* ref = deref(self.matrix)
        cdef Future[void] fut = ref.Randomize()
        wait_for_future_void(&fut)
        deactivate_context()
        self.accessed = 1
