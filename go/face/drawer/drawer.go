package drawer

import (
	"image"
	"log"

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

// DrawHeadPose draw head pose
func (d *Drawer) DrawHeadPose(img image.Image, pose face.HeadPose, rect common.Rectangle) image.Image {
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())
	rect.X *= imgW
	rect.Y *= imgH
	rect.Width *= imgW
	rect.Height *= imgH
	xOffset := rect.X + rect.Width/2
	yOffset := rect.Y + rect.Height/2
	dist := rect.Height / 2
	axises := EulrToAxisPoints(pose.Yaw, pose.Pitch, pose.Roll, dist)
	log.Printf("%+v\n", axises)
	out := image.NewRGBA(img.Bounds())
	gc := draw2dimg.NewGraphicContext(out)
	gc.DrawImage(img)
	gc.SetLineWidth(d.BorderStrokeWidth)
	gc.SetStrokeColor(common.ColorFromHex(common.Red))
	gc.BeginPath()
	gc.MoveTo(xOffset, yOffset)
	gc.LineTo(axises[0][0]+xOffset, axises[0][1]+yOffset)
	gc.Stroke()
	gc.Close()
	gc.SetStrokeColor(common.ColorFromHex(common.Green))
	gc.BeginPath()
	gc.MoveTo(xOffset, yOffset)
	gc.LineTo(axises[1][0]+xOffset, axises[1][1]+yOffset)
	gc.Stroke()
	gc.Close()
	gc.SetStrokeColor(common.ColorFromHex(common.Blue))
	gc.BeginPath()
	gc.MoveTo(xOffset, yOffset)
	gc.LineTo(axises[2][0]+xOffset, axises[2][1]+yOffset)
	gc.Stroke()
	gc.Close()
	return out
}
