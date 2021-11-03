package drawer

import (
	"github.com/bububa/openvision/go/common"
)

const (
	// DefaultBorderColor default drawer border color
	DefaultBorderColor = common.Green
	// DefaultBorderStrokeWidth default drawer border stroke width
	DefaultBorderStrokeWidth = 3
	// DefaultKeypointRadius default drawer keypoint radius
	DefaultKeypointRadius = 2
	// DefaultKeypointStrokeWidth default drawer keypoint stroke width
	DefaultKeypointStrokeWidth = 2
)

// CocoPart coco part define
type CocoPart = int

const (
	// CocoPartNose nose
	CocoPartNose CocoPart = iota
	// CocoPartLEye left eye
	CocoPartLEye
	// CocoPartREye right eye
	CocoPartREye
	// CocoPartLEar left ear
	CocoPartLEar
	// CocoPartREar right ear
	CocoPartREar
	// CocoPartLShoulder left sholder
	CocoPartLShoulder
	// CocoPartRShoulder right sholder
	CocoPartRShoulder
	// CocoPartLElbow left elbow
	CocoPartLElbow
	// CocoPartRElbow right elbow
	CocoPartRElbow
	// CocoPartLWrist left wrist
	CocoPartLWrist
	// CocoPartRWrist right wrist
	CocoPartRWrist
	// CocoPartLHip left hip
	CocoPartLHip
	// CocoPartRHip right hip
	CocoPartRHip
	// CocoPartLKnee left knee
	CocoPartLKnee
	// CocoPartRKnee right knee
	CocoPartRKnee
	// CocoPartRAnkle right ankle
	CocoPartRAnkle
	// CocoPartLAnkle left ankle
	CocoPartLAnkle
	// CocoPartNeck neck
	CocoPartNeck
	// CocoPartBackground background
	CocoPartBackground
)

var (
	// CocoPair represents joints pair
	CocoPair = [16][2]CocoPart{
		{0, 1}, {1, 3}, {0, 2}, {2, 4}, {5, 6}, {5, 7}, {7, 9}, {6, 8}, {8, 10}, {5, 11}, {6, 12}, {11, 12}, {11, 13}, {12, 14}, {13, 15}, {14, 16},
	}

	// OpenPosePair represents joints pair for openpose
	OpenPosePair = [17][2]int{
		{1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6},
		{6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11},
		{11, 12}, {12, 13}, {1, 0}, {0, 14}, {14, 16},
		{0, 15}, {15, 17},
	}

	// CocoColors represents color for coco parts
	CocoColors = [19]string{
		"#ff0000", "#ff5500", "#ffaa00", "#ffff00",
		"#aaff00", "#55ff00", "#00ff00", "#00ff55", "#00ffaa",
		"#00ffff", "#00aaff", "#0055ff",
		"#0000ff", "#aa00ff", "#ff00ff",
		"#ff00aa", "#ff0055", "#ff55aa", "#aa0055",
	}
)
