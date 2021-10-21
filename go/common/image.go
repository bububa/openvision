package common

import (
	"bytes"
	"image"
	"io"
	"sync"

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
	Image2RGB(buf, img)
	return &Image{
		Image:  img,
		buffer: buf,
	}
}

// Bytes returns image bytes in rgb
func (i Image) Bytes() []byte {
	if i.buffer == nil {
		return nil
	}
	return i.buffer.Bytes()
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

// Crop returns cropped image bytes in Rectangle
func (i Image) Crop(rect Rectangle) []byte {
	imgW := i.WidthF64()
	imgH := i.HeightF64()
	imgWInt := i.Width()
	imgHInt := i.Height()
	cropWidth := int(rect.Width)
	if rect.MaxX() > imgW {
		cropWidth = imgWInt
	}
	cropHeight := int(rect.Height)
	if rect.MaxY() > imgH {
		cropHeight = imgHInt
	}
	xOffset := int(rect.X)
	if rect.X < 1e-15 {
		xOffset = 0
	}
	yOffset := int(rect.Y)
	if rect.Y < 1e-15 {
		yOffset = 0
	}
	imgData := i.Bytes()
	ret := make([]byte, 0, cropWidth*cropHeight*3)
	pool := &sync.Pool{
		New: func() interface{} {
			return make([]byte, cropWidth*3)
		},
	}
	for y := 0; y < cropHeight; y++ {
		srcCur := ((y+yOffset)*imgWInt + xOffset) * 3
		dist := pool.Get().([]byte)
		dist = dist[:0]
		copy(dist, imgData[srcCur:cropWidth*3])
		pool.Put(dist)
		ret = append(ret, dist...)
	}
	return ret
}

// Image2RGB write image rgbdata to buffer
func Image2RGB(buf io.Writer, img image.Image) {
	bounds := img.Bounds()
	for j := bounds.Min.Y; j < bounds.Max.Y; j++ {
		for i := bounds.Min.X; i < bounds.Max.X; i++ {
			r, g, b, _ := img.At(i, j).RGBA()
			buf.Write([]byte{byte(b >> 8), byte(g >> 8), byte(r >> 8)})
		}
	}
}

// Image2BGR write image bgrdata to buffer
func Image2BGR(buf io.Writer, img image.Image) {
	bounds := img.Bounds()
	for j := bounds.Min.Y; j < bounds.Max.Y; j++ {
		for i := bounds.Min.X; i < bounds.Max.X; i++ {
			r, g, b, _ := img.At(i, j).RGBA()
			buf.Write([]byte{byte(r >> 8), byte(g >> 8), byte(b >> 8)})
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
