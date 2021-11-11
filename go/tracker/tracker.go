package tracker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/tracker/tracker.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Tracker represents Object Tracker interface
type Tracker interface {
	common.Estimator
	Track(img *common.Image, rect common.Rectangle) (common.Rectangle, error)
}

// Track returns object tracked rect
func Track(d Tracker, img *common.Image, rect common.Rectangle) (common.Rectangle, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	rectC := rect.CRect(imgWidth, imgHeight)
	defer C.free(unsafe.Pointer(rectC))
	errCode := C.track_object(
		(C.ITracker)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(rectC)))
	if errCode != 0 {
		return common.ZR, openvision.TrackerError(int(errCode))
	}
	return common.GoRect(rectC, imgWidth, imgHeight), nil
}
