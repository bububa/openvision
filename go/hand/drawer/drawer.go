package drawer

import (
	"image"

	"github.com/llgcode/draw2d/draw2dimg"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/hand"
)

// Drawer represents a hand drawer
type Drawer struct {
	// BorderColor represents hand rect border color
	BorderColor string
	// BorderStrokeWidth represents hand rect stroke width
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
func (d *Drawer) Draw(img image.Image, rois []hand.ROI, drawBorder bool) image.Image {
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
		l := len(roi.Keypoints)
		if l == 0 {
			continue
		}
		// draw lines
		gc.SetLineWidth(d.BorderStrokeWidth)
		for idx := range roi.Keypoints[:l-1] {
			var (
				p0        common.Point
				p1        common.Point
				poseColor = PoseColors[idx/4]
			)
			gc.SetStrokeColor(common.ColorFromHex(poseColor))
			if idx == 5 || idx == 9 || idx == 13 || idx == 17 {
				p0 = roi.Keypoints[0]
				p1 = roi.Keypoints[idx]
				gc.BeginPath()
				gc.MoveTo(p0.X*imgW, p0.Y*imgH)
				gc.LineTo(p1.X*imgW, p1.Y*imgH)
				gc.Close()
				gc.Stroke()
			} else if idx == 4 || idx == 8 || idx == 12 || idx == 16 {
				continue
			}
			p0 = roi.Keypoints[idx]
			p1 = roi.Keypoints[idx+1]
			gc.BeginPath()
			gc.MoveTo(p0.X*imgW, p0.Y*imgH)
			gc.LineTo(p1.X*imgW, p1.Y*imgH)
			gc.Close()
			gc.Stroke()
		}

		// draw keypoints
		for idx, pt := range roi.Keypoints {
			colorIdx := idx / 4
			if idx == 4 || idx == 8 || idx == 12 || idx == 16 || idx >= 20 {
				colorIdx--
			}
			poseColor := PoseColors[colorIdx]
			common.DrawCircle(gc, common.Pt(pt.X*imgW, pt.Y*imgH), d.KeypointRadius, poseColor, "", d.KeypointStrokeWidth)
		}
	}
	return out
}
