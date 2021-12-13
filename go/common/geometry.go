package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import (
	"unsafe"
)

// Rectangle represents a Rectangle
type Rectangle struct {
	X      float64
	Y      float64
	Width  float64
	Height float64
}

// Rect returns a Retancle
func Rect(x, y, w, h float64) Rectangle {
	return Rectangle{
		X:      x,
		Y:      y,
		Width:  w,
		Height: h,
	}
}

// MaxX returns right x
func (r Rectangle) MaxX() float64 {
	return r.X + r.Width
}

// MaxY returns bottom y
func (r Rectangle) MaxY() float64 {
	return r.Y + r.Height
}

// CRect returns C.Rect
func (r Rectangle) CRect(w float64, h float64) *C.Rect {
	v := (*C.Rect)(C.malloc(C.sizeof_Rect))
	v.x = C.int(r.X * w)
	v.y = C.int(r.Y * h)
	v.width = C.int(r.Width * w)
	v.height = C.int(r.Height * h)
	return v
}

// GoRect convert C.Rect to go type
func GoRect(c *C.Rect, w float64, h float64) Rectangle {
	return Rect(
		float64(c.x)/w,
		float64(c.y)/h,
		float64(c.width)/w,
		float64(c.height)/h,
	)
}

var ZR = Rectangle{}

var FullRect = Rect(0, 0, 1, 1)

// Point represents a Point
type Point struct {
	X float64
	Y float64
}

// Pt returns a New Point
func Pt(x, y float64) Point {
	return Point{x, y}
}

var ZP = Point{}

// GoPoint2f conver C.Point2f to Point
func GoPoint2f(c *C.Point2f, w float64, h float64) Point {
	return Pt(
		float64(c.x)/w,
		float64(c.y)/h,
	)
}

// NewCPoint2fVector retruns C.Point2fVector pointer
func NewCPoint2fVector() *C.Point2fVector {
	return (*C.Point2fVector)(C.malloc(C.sizeof_Point2f))
}

// GoPoint2fVector convert C.Point2fVector to []Point
func GoPoint2fVector(cVector *C.Point2fVector, w float64, h float64) []Point {
	if cVector == nil {
		return nil
	}
	l := int(cVector.length)
	ret := make([]Point, 0, l)
	ptr := unsafe.Pointer(cVector.points)
	for i := 0; i < l; i++ {
		cPoint2f := (*C.Point2f)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_Point2f*C.int(i))))
		ret = append(ret, GoPoint2f(cPoint2f, w, h))
	}
	return ret
}

// FreeCPoint2fVector release C.Point2fVector memory
func FreeCPoint2fVector(c *C.Point2fVector) {
	C.FreePoint2fVector(c)
	C.free(unsafe.Pointer(c))
}

// Point3d represents a 3dPoint
type Point3d struct {
	X float64
	Y float64
	Z float64
}

// Pt3d returns a New Point3d
func Pt3d(x, y, z float64) Point3d {
	return Point3d{x, y, z}
}

var ZP3d = Point3d{}

// GoPoint3d conver C.Point3d to Point3d
func GoPoint3d(c *C.Point3d) Point3d {
	return Pt3d(
		float64(c.x),
		float64(c.y),
		float64(c.z),
	)
}

// NewCPoint3dVector retruns C.Point3dVector pointer
func NewCPoint3dVector() *C.Point3dVector {
	return (*C.Point3dVector)(C.malloc(C.sizeof_Point3d))
}

// GoPoint3dVector convert C.Point3dVector to []Point3d
func GoPoint3dVector(cVector *C.Point3dVector) []Point3d {
	if cVector == nil {
		return nil
	}
	l := int(cVector.length)
	ret := make([]Point3d, 0, l)
	ptr := unsafe.Pointer(cVector.points)
	for i := 0; i < l; i++ {
		cPoint3d := (*C.Point3d)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_Point3d*C.int(i))))
		ret = append(ret, GoPoint3d(cPoint3d))
	}
	return ret
}

// FreeCPoint3dVector release C.Point3dVector memory
func FreeCPoint3dVector(c *C.Point3dVector) {
	C.FreePoint3dVector(c)
	C.free(unsafe.Pointer(c))
}
