package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"

// GetGPUCount get gpu number
func GetGPUCount() int {
	count := C.get_gpu_count()
	return int(count)
}

// CreateGPUInstance create gpu instance
func CreateGPUInstance() int {
	i := C.create_gpu_instance()
	return int(i)
}

// DestroyGPUInstance destory gpu instance
func DestroyGPUInstance() {
	C.destroy_gpu_instance()
}

// GetBigCPUCount get cpu number
func GetBigCPUCount() int {
	return int(C.get_big_cpu_count())
}

// SetOMPThreads set omp thread number
func SetOMPThreads(n int) {
	C.set_omp_num_threads(C.int(n))
}
