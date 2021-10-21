package common

/*
#cgo CXXFLAGS:   --std=c++11 -fopenmp
#cgo CPPFLAGS:   -I ${SRCDIR}/../../include -I /usr/local/include
#cgo LDFLAGS: -lstdc++ -lncnn -lomp -lopenvision
#cgo LDFLAGS: -L /usr/local/lib -L ${SRCDIR}/../../lib
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import "unsafe"

// NewCFloatVector returns C.FloatVector pointer
func NewCFloatVector() *C.FloatVector {
	return (*C.FloatVector)(C.malloc(C.sizeof_FloatVector))
}

// GoFloatVector convert C.FloatVector to []float64
func GoFloatVector(cVector *C.FloatVector) []float64 {
	l := int(cVector.length)
	ret := make([]float64, 0, l)
	ptr := unsafe.Pointer(cVector.values)
	for i := 0; i < l; i++ {
		v := (*float32)(unsafe.Pointer(uintptr(ptr) + uintptr(C.int(i)*C.sizeof_float)))
		ret = append(ret, float64(*v))
	}
	return ret
}

// FreeCFloatVector release C.FloatVector memory
func FreeCFloatVector(c *C.FloatVector) {
	C.FreeFloatVector(c)
	C.free(unsafe.Pointer(c))
}
