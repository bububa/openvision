package drawer

import (
	"image"
	"image/color"

	"github.com/llgcode/draw2d/draw2dimg"
	"github.com/llgcode/draw2d/draw2dkit"

	"github.com/bububa/openvision/go/common"
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
	// KeypointColor represents keypoint color
	KeypointColor string
	// LabelColor string
	LabelColor string
	// Font
	Font *common.Font
}

// New returns a new Drawer
func New(options ...Option) *Drawer {
	d := &Drawer{
		BorderColor:         DefaultBorderColor,
		BorderStrokeWidth:   DefaultBorderStrokeWidth,
		KeypointStrokeWidth: DefaultKeypointStrokeWidth,
		KeypointRadius:      DefaultKeypointRadius,
		KeypointColor:       DefaultKeypointColor,
		LabelColor:          DefaultLabelColor,
	}
	for _, opt := range options {
		opt.apply(d)
	}
	return d
}

// Draw draw rois
func (d *Drawer) Draw(img image.Image, rois []common.ObjectInfo, drawBorder bool) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	for _, roi := range rois {
		rect := common.Rect(
			roi.Rect.X*imgW,
			roi.Rect.Y*imgH,
			roi.Rect.Width*imgW,
			roi.Rect.Height*imgH,
		)
		borderColor := d.BorderColor
		if drawBorder {
			// draw rect
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
				p0 = roi.Keypoints[0].Point
				p1 = roi.Keypoints[idx].Point
				gc.BeginPath()
				gc.MoveTo(p0.X*imgW, p0.Y*imgH)
				gc.LineTo(p1.X*imgW, p1.Y*imgH)
				gc.Close()
				gc.Stroke()
			} else if idx == 4 || idx == 8 || idx == 12 || idx == 16 {
				continue
			}
			p0 = roi.Keypoints[idx].Point
			p1 = roi.Keypoints[idx+1].Point
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
			common.DrawCircle(gc, common.Pt(pt.Point.X*imgW, pt.Point.Y*imgH), d.KeypointRadius, poseColor, "", d.KeypointStrokeWidth)
		}
		// draw name
		if roi.Name != "" {
			common.DrawLabelInWidth(gc, d.Font, roi.Name, common.Pt(rect.X, rect.MaxY()), d.LabelColor, borderColor, rect.Width)
		}
	}
	return out
}

// DrawPalm draw PalmObject
func (d *Drawer) DrawPalm(img image.Image, rois []common.PalmObject) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	for _, roi := range rois {
		gc.SetLineWidth(d.BorderStrokeWidth)
		gc.SetStrokeColor(common.ColorFromHex(d.BorderColor))
		gc.BeginPath()
		for idx, pt := range roi.Rect {
			gc.MoveTo(pt.X*imgW, pt.Y*imgH)
			if idx == len(roi.Rect)-1 {
				gc.LineTo(roi.Rect[0].X*imgW, roi.Rect[0].Y*imgH)
			} else {
				gc.LineTo(roi.Rect[idx+1].X*imgW, roi.Rect[idx+1].Y*imgH)
			}
		}
		gc.Close()
		gc.Stroke()

		l := len(roi.Skeleton)
		if l == 0 {
			continue
		}
		// draw skeleton
		for idx := range roi.Skeleton[:l-1] {
			var (
				p0        common.Point
				p1        common.Point
				poseColor = PoseColors[idx/4]
			)
			gc.SetStrokeColor(common.ColorFromHex(poseColor))
			if idx == 5 || idx == 9 || idx == 13 || idx == 17 {
				p0 = roi.Skeleton[0]
				p1 = roi.Skeleton[idx]
				gc.BeginPath()
				gc.MoveTo(p0.X*imgW, p0.Y*imgH)
				gc.LineTo(p1.X*imgW, p1.Y*imgH)
				gc.Close()
				gc.Stroke()
			} else if idx == 4 || idx == 8 || idx == 12 || idx == 16 {
				continue
			}
			p0 = roi.Skeleton[idx]
			p1 = roi.Skeleton[idx+1]
			gc.BeginPath()
			gc.MoveTo(p0.X*imgW, p0.Y*imgH)
			gc.LineTo(p1.X*imgW, p1.Y*imgH)
			gc.Close()
			gc.Stroke()
		}
		for _, pt := range roi.Landmarks {
			common.DrawCircle(gc, common.Pt(pt.X*imgW, pt.Y*imgH), d.KeypointRadius, d.KeypointColor, "", d.KeypointStrokeWidth)
		}
	}
	return out
}

// DrawPalm3D draw 3d PalmObject
func (d *Drawer) DrawPalm3D(roi common.PalmObject, size float64, bg string) image.Image {
	out := image.NewRGBA(image.Rect(0, 0, int(size), int(size)))
	gc := draw2dimg.NewGraphicContext(out)
	l := len(roi.Skeleton3d)
	if l == 0 {
		return out
	}
	if bg != "" {
		bgColor := common.ColorFromHex(bg)
		gc.SetFillColor(bgColor)
		draw2dkit.Rectangle(gc, 0, 0, size, size)
		gc.Fill()
		gc.SetFillColor(color.Transparent)
	}
	// draw skeleton3d
	for idx := range roi.Skeleton3d[:l-1] {
		var (
			p0        common.Point3d
			p1        common.Point3d
			poseColor = PoseColors[idx/4]
		)
		gc.SetStrokeColor(common.ColorFromHex(poseColor))
		if idx == 5 || idx == 9 || idx == 13 || idx == 17 {
			p0 = roi.Skeleton3d[0]
			p1 = roi.Skeleton3d[idx]
			gc.BeginPath()
			gc.MoveTo(p0.X*size, p0.Y*size)
			gc.LineTo(p1.X*size, p1.Y*size)
			gc.Close()
			gc.Stroke()
		} else if idx == 4 || idx == 8 || idx == 12 || idx == 16 {
			continue
		}
		p0 = roi.Skeleton3d[idx]
		p1 = roi.Skeleton3d[idx+1]
		gc.BeginPath()
		gc.MoveTo(p0.X*size, p0.Y*size)
		gc.LineTo(p1.X*size, p1.Y*size)
		gc.Close()
		gc.Stroke()
	}
	return out
}
