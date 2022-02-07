package drawer

import (
	"github.com/bububa/openvision/go/common"
)

const (
	// DefaultBorderColor default drawer border color
	DefaultBorderColor = common.Green
	// DefaultKeypointColor default drawer keypoint color
	DefaultKeypointColor = common.Pink
	// DefaultBorderStrokeWidth default drawer border stroke width
	DefaultBorderStrokeWidth = 3
	// DefaultKeypointRadius default drawer keypoint radius
	DefaultKeypointRadius = 3
	// DefaultKeypointStrokeWidth default drawer keypoint stroke width
	DefaultKeypointStrokeWidth = 1
	// DefaultLabelColor default label color
	DefaultLabelColor = common.White
)

var (
	// PoseColors represents color for hand pose
	PoseColors = [6]string{
		"#0ad7ff", "#ff7337", "#05ff37", "#190fff", "#ff0f37", "#aa00ff",
	}
)
