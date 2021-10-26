package face

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"

// HeadPose represents detected head pose
type HeadPose struct {
	Roll  float64
	Pitch float64
	Yaw   float64
}

// GoHeadPose convert C.HeadPose to go type
func GoHeadPose(c *C.HeadPose) HeadPose {
	return HeadPose{
		Roll:  float64(c.roll),
		Pitch: float64(c.pitch),
		Yaw:   float64(c.yaw),
	}
}

// NewCHeadPose retruns a new C.HeadPose
func NewCHeadPose() *C.HeadPose {
	return (*C.HeadPose)(C.malloc(C.sizeof_HeadPose))
}
