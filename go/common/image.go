package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import (
	"bytes"
	"image"
	"image/color"
	"io"
	"unsafe"

	"github.com/llgcode/draw2d/draw2dimg"
	"github.com/llgcode/draw2d/draw2dkit"
)

// Image image with buffer cache
type Image struct {
	image.Image
	buffer *bytes.Buffer
}

// NewImage returns a new Image
func NewImage(img image.Image) *Image {
	buf := new(bytes.Buffer)
	if img == nil {
		return &Image{buffer: buf}
	}
	Image2RGB(buf, img)
	return &Image{
		Image:  img,
		buffer: buf,
	}
}

func (i *Image) Reset() {
	i.Image = nil
	if i.buffer != nil {
		i.buffer.Reset()
	}
}

// Write write bytes to buffer
func (i *Image) Write(b []byte) {
	if i.buffer == nil {
		return
	}
	i.buffer.Write(b)
}

// Bytes returns image bytes in rgb
func (i Image) Bytes() []byte {
	if i.buffer == nil {
		return nil
	}
	return i.buffer.Bytes()
}

// RGBABytes returns image bytes in rgba
func (i Image) RGBABytes() []byte {
	buf := new(bytes.Buffer)
	Image2RGBA(buf, i.Image)
	return buf.Bytes()
}

// Width returns image width
func (i Image) Width() int {
	return i.Bounds().Dx()
}

// Height returns image height
func (i Image) Height() int {
	return i.Bounds().Dy()
}

// WidthF64 returns image width in float64
func (i Image) WidthF64() float64 {
	return float64(i.Bounds().Dx())
}

// HeightF64 returns image height in float64
func (i Image) HeightF64() float64 {
	return float64(i.Bounds().Dy())
}

// NewCImage returns new C.Image
func NewCImage() *C.Image {
	ret := (*C.Image)(C.malloc(C.sizeof_Image))
	return ret
}

// FreeCImage free C.Image
func FreeCImage(c *C.Image) {
	C.FreeImage(c)
	C.free(unsafe.Pointer(c))
}

// GoImage returns Image from C.Image
func GoImage(c *C.Image, out *Image) {
	w := int(c.width)
	h := int(c.height)
	channels := int(c.channels)
	data := C.GoBytes(unsafe.Pointer(c.data), C.int(w*h*channels)*C.sizeof_uchar)
	NewImageFromBytes(data, w, h, channels, out)
}

// NewImageFromBytes returns Image by []byte
func NewImageFromBytes(data []byte, w int, h int, channels int, out *Image) {
	img := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			pos := (y*w + x) * channels
			var alpha byte = 255
			if channels == 4 {
				alpha = data[pos+3]
			}
			img.SetRGBA(x, y, color.RGBA{uint8(data[pos]), uint8(data[pos+1]), uint8(data[pos+2]), uint8(alpha)})
			out.Write([]byte{byte(data[pos]), byte(data[pos+1]), byte(data[pos+2]), byte(alpha)})
		}
	}
	out.Image = img
}

// Image2RGB write image rgbdata to buffer
func Image2RGB(buf io.Writer, img image.Image) {
	bounds := img.Bounds()
	for j := bounds.Min.Y; j < bounds.Max.Y; j++ {
		for i := bounds.Min.X; i < bounds.Max.X; i++ {
			r, g, b, _ := img.At(i, j).RGBA()
			buf.Write([]byte{byte(r >> 8), byte(g >> 8), byte(b >> 8)})
		}
	}
}

// Image2RGBA write image rgbdata to buffer
func Image2RGBA(buf io.Writer, img image.Image) {
	bounds := img.Bounds()
	for j := bounds.Min.Y; j < bounds.Max.Y; j++ {
		for i := bounds.Min.X; i < bounds.Max.X; i++ {
			r, g, b, a := img.At(i, j).RGBA()
			buf.Write([]byte{byte(r >> 8), byte(g >> 8), byte(b >> 8), byte(a >> 8)})
		}
	}
}

// Image2BGR write image bgrdata to buffer
func Image2BGR(buf io.Writer, img image.Image) {
	bounds := img.Bounds()
	for j := bounds.Min.Y; j < bounds.Max.Y; j++ {
		for i := bounds.Min.X; i < bounds.Max.X; i++ {
			r, g, b, _ := img.At(i, j).RGBA()
			buf.Write([]byte{byte(b >> 8), byte(g >> 8), byte(r >> 8)})
		}
	}
}

// DrawRectangle draw rectangle on image
func DrawRectangle(gc *draw2dimg.GraphicContext, rect Rectangle, borderColor string, bgColor string, strokeWidth float64) {
	gc.SetStrokeColor(ColorFromHex(borderColor))
	if bgColor != "" {
		gc.SetFillColor(ColorFromHex(bgColor))
	}
	gc.SetLineWidth(strokeWidth)
	draw2dkit.Rectangle(gc, rect.X, rect.Y, rect.MaxX(), rect.MaxY())
	if strokeWidth == 0 || bgColor != "" {
		if bgColor == "" {
			gc.SetFillColor(ColorFromHex(borderColor))
		}
		gc.FillStroke()
	} else {
		gc.Stroke()
	}
}

// DrawCircle draw circle on image
func DrawCircle(gc *draw2dimg.GraphicContext, pt Point, r float64, borderColor string, bgColor string, strokeWidth float64) {
	gc.SetStrokeColor(ColorFromHex(borderColor))
	if bgColor != "" {
		gc.SetFillColor(ColorFromHex(bgColor))
	}
	gc.SetLineWidth(strokeWidth)
	draw2dkit.Circle(gc, pt.X, pt.Y, r)
	if strokeWidth == 0 || bgColor != "" {
		if bgColor == "" {
			gc.SetFillColor(ColorFromHex(borderColor))
		}
		gc.FillStroke()
	} else {
		gc.Stroke()
	}
}

// DrawLabel draw label text to image
func DrawLabel(gc *draw2dimg.GraphicContext, font *Font, label string, pt Point, txtColor string, bgColor string, scale float64) {
	if font == nil || font.Cache == nil || font.Data == nil {
		return
	}
	gc.FontCache = font.Cache
	gc.SetFontData(*font.Data)
	gc.SetFontSize(font.Size * scale)
	var (
		x       = float64(pt.X)
		y       = float64(pt.Y)
		padding = 2.0 * scale
	)
	left, top, right, bottom := gc.GetStringBounds(label)
	height := bottom - top
	width := right - left
	if bgColor != "" {
		gc.SetFillColor(ColorFromHex(bgColor))
		draw2dkit.Rectangle(gc, x, y, x+width+padding*2, y+height+padding*2)
		gc.Fill()
	}
	gc.SetFillColor(ColorFromHex(txtColor))
	gc.FillStringAt(label, x-left+padding, y-top+padding)
}

// DrawLabelInWidth draw label text to image in width restrict
func DrawLabelInWidth(gc *draw2dimg.GraphicContext, font *Font, label string, pt Point, txtColor string, bgColor string, boundWidth float64) {
	if font == nil || font.Cache == nil || font.Data == nil {
		return
	}
	gc.FontCache = font.Cache
	gc.SetFontData(*font.Data)
	gc.SetFontSize(font.Size)
	left, _, right, _ := gc.GetStringBounds(label)
	padding := 2.0
	width := right - left
	fontWidth := width + padding*2
	scale := boundWidth / fontWidth
	DrawLabelInWidth(gc, font, label, pt, txtColor, bgColor, scale)
}
