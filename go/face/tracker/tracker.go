package tracker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/common.h"
#include "openvision/face/tracker.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Tracker represents Tracker
type Tracker struct {
	d C.IFaceTracker
}

// NewTracker returns a new Tracker
func NewTracker() *Tracker {
	return &Tracker{
		d: C.new_tracker(),
	}
}

// Destroy destroy C.ITracker
func (t *Tracker) Destroy() {
	C.destroy_tracker(t.d)
}

// Track track faces
func (t *Tracker) Track(img *common.Image, faces []face.FaceInfo) ([]face.TrackedFaceInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	l := len(faces)
	currFaces := make([]face.FaceInfo, 0, l)
	for _, f := range faces {
		f.Rect.X *= imgWidth
		f.Rect.Y *= imgHeight
		f.Rect.Width *= imgWidth
		f.Rect.Height *= imgHeight
		for i := 0; i < 5; i++ {
			f.Keypoints[i].X *= imgWidth
			f.Keypoints[i].Y *= imgHeight
		}
		currFaces = append(currFaces, f)
	}
	CCurrFaces := face.NewCFaceInfoVectorFromFaces(currFaces)
	defer face.FreeCFaceInfoVector(CCurrFaces)
	CTrackedFaces := face.NewCTrackedFaceInfoVector()
	defer face.FreeCTrackedFaceInfoVector(CTrackedFaces)
	errCode := C.track(
		t.d,
		(*C.FaceInfoVector)(unsafe.Pointer(CCurrFaces)),
		(*C.TrackedFaceInfoVector)(unsafe.Pointer(CTrackedFaces)),
	)
	if errCode != 0 {
		return nil, openvision.TrackFaceError(int(errCode))
	}
	trackedFaces := face.GoTrackedFaceInfoVector(CTrackedFaces, imgWidth, imgHeight)
	return trackedFaces, nil
}
