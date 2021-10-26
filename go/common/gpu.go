package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"

func GetGPUCount() int {
	count := C.get_gpu_count()
	return int(count)
}

func CreateGPUInstance() int {
	i := C.create_gpu_instance()
	return int(i)
}

func DestroyGPUInstance() {
	C.destroy_gpu_instance()
}
