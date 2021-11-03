package drawer

import (
	"image"

	"github.com/llgcode/draw2d/draw2dimg"

	"github.com/bububa/openvision/go/common"
)

// Drawer represents a pose drawer
type Drawer struct {
	// BorderColor represents face rect border color
	BorderColor string
	// BorderStrokeWidth represents face rect stroke width
	BorderStrokeWidth float64
	// KeypointStrokeWidth represents keypoints stroke width
	KeypointStrokeWidth float64
	// KeypointRadius represents keypoints circle radius
	KeypointRadius float64
}

// New returns a new Drawer
func New(options ...Option) *Drawer {
	d := &Drawer{
		BorderColor:         DefaultBorderColor,
		BorderStrokeWidth:   DefaultBorderStrokeWidth,
		KeypointStrokeWidth: DefaultKeypointStrokeWidth,
		KeypointRadius:      DefaultKeypointRadius,
	}
	for _, opt := range options {
		opt.apply(d)
	}
	return d
}

// Draw draw rois
func (d *Drawer) Draw(img image.Image, rois []common.ObjectInfo, drawBorder bool, openPose bool) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	for _, roi := range rois {
		if drawBorder {
			// draw rect
			rect := common.Rect(
				roi.Rect.X*imgW,
				roi.Rect.Y*imgH,
				roi.Rect.Width*imgW,
				roi.Rect.Height*imgH,
			)
			borderColor := d.BorderColor
			common.DrawRectangle(gc, rect, borderColor, "", d.BorderStrokeWidth)
		}

		if len(roi.Keypoints) == 0 {
			return out
		}
		// draw joins
		pairs := CocoPair[:]
		if openPose {
			pairs = OpenPosePair[:]
		}
		for idx, pair := range pairs {
			p0 := roi.Keypoints[pair[0]]
			p1 := roi.Keypoints[pair[1]]
			if p0.Score < 0.2 || p1.Score < 0.2 {
				continue
			}
			if p0.Point.X < 0 || p0.Point.Y < 0 || p1.Point.Y < 0 || p1.Point.X < 0 {
				continue
			}
			cocoColor := CocoColors[idx]
			gc.SetLineWidth(d.BorderStrokeWidth)
			gc.SetStrokeColor(common.ColorFromHex(cocoColor))
			gc.BeginPath()
			gc.MoveTo(p0.Point.X*imgW, p0.Point.Y*imgH)
			gc.LineTo(p1.Point.X*imgW, p1.Point.Y*imgH)
			gc.Close()
			gc.Stroke()
		}

		// draw keypoints
		for idx, pt := range roi.Keypoints {
			if pt.Score < 0.2 {
				continue
			}
			if pt.Point.X < 0 || pt.Point.Y < 0 {
				continue
			}
			cocoColor := CocoColors[idx]
			common.DrawCircle(gc, common.Pt(pt.Point.X*imgW, pt.Point.Y*imgH), d.KeypointRadius, cocoColor, "", d.KeypointStrokeWidth)
		}
	}
	return out
}
