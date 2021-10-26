package realsr

import (
	"image"

	"github.com/bububa/openvision/go/common"
)

// Processer super-resolution processer interface
type Processer interface {
	LoadModel(modelPath string) error
	Destroy()
	Process(img *common.Image, scale int) (image.Image, error)
}
