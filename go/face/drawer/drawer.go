package face

import (
	"image"

	"github.com/llgcode/draw2d/draw2dimg"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Drawer represents a face drawer
type Drawer struct {
	// BorderColor represents face rect border color
	BorderColor string
	// KeypointColor represents keypoint color
	KeypointColor string
	// BorderStrokeWidth represents face rect stroke width
	BorderStrokeWidth float64
	// KeypointRadius represents keypoints circle radius
	KeypointRadius float64
	// KeypointStrokeWidth represents keypoints stroke width
	KeypointStrokeWidth float64
	// MaskColor represents border color which mask is true
	MaskColor string
	// InvalidBorderColor
	InvalidBorderColor string
}

// New returns a new Drawer
func New(options ...Option) *Drawer {
	d := &Drawer{
		BorderColor:         DefaultBorderColor,
		BorderStrokeWidth:   DefaultBorderStrokeWidth,
		KeypointColor:       DefaultKeypointColor,
		KeypointStrokeWidth: DefaultKeypointStrokeWidth,
		KeypointRadius:      DefaultKeypointRadius,
		InvalidBorderColor:  DefaultInvalidBorderColor,
		MaskColor:           DefaultBorderColor,
	}
	for _, opt := range options {
		opt.apply(d)
	}
	return d
}

// Draw draw faces
func (d *Drawer) Draw(img image.Image, faces []face.FaceInfo) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	for _, face := range faces {
		// draw rect
		rect := common.Rect(
			face.Rect.X*imgW,
			face.Rect.Y*imgH,
			face.Rect.Width*imgW,
			face.Rect.Height*imgH,
		)
		borderColor := d.BorderColor
		if face.Mask {
			borderColor = d.MaskColor
		}
		common.DrawRectangle(gc, rect, borderColor, "", d.BorderStrokeWidth)
		// draw keypoints
		for _, pt := range face.Keypoints {
			common.DrawCircle(gc, common.Pt(pt.X*imgW, pt.Y*imgH), d.KeypointRadius, d.KeypointColor, "", d.KeypointStrokeWidth)
		}
	}
	return out
}

// DrawLandmark draw landmark
func (d *Drawer) DrawLandmark(img image.Image, points []common.Point) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	for _, pt := range points {
		common.DrawCircle(gc, common.Pt(pt.X*imgW, pt.Y*imgH), d.KeypointRadius, d.KeypointColor, "", d.KeypointStrokeWidth)
	}
	return out
}
