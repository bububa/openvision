package realesrgan

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/realsr/realesrgan.h"
*/
import "C"
import (
	"image"
	"image/color"
	"log"
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// RealESRGAN represents RealESRGAN processer
type RealESRGAN struct {
	d C.RealEsrgan
}

// NewRealERSGAN returns a new RealESRGAN
func NewRealERSGAN(gpuid int, ttaMode bool) *RealESRGAN {
	return &RealESRGAN{
		d: C.new_realesrgan(C.int(gpuid), C.bool(ttaMode)),
	}
}

// Destroy a detecter
func (d *RealESRGAN) Destroy() {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(d.d)))
}

// LoadModel load processer model
func (d *RealESRGAN) LoadModel(modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(d.d)), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Process run super-resolution process
func (d *RealESRGAN) Process(img *common.Image, scale int) (image.Image, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	COutput := common.NewCBytes()
	defer common.FreeCBytes(COutput)
	// defer C.free(unsafe.Pointer(COutput))
	errCode := C.realesrgan_process(
		d.d,
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		C.int(scale),
		(*C.Bytes)(unsafe.Pointer(COutput)),
	)
	if errCode != 0 {
		return nil, openvision.RealsrError(int(errCode))
	}
	outData := common.GoBytes(COutput)
	log.Printf("w:%d, h:%d, l:%d\n", int(imgWidth)*scale, int(imgHeight)*scale, len(outData))
	outW := int(imgWidth) * scale
	outH := int(imgHeight) * scale
	outImg := image.NewRGBA(image.Rect(0, 0, int(imgWidth)*scale, int(imgHeight)*scale))
	for y := 0; y < outH; y++ {
		for x := 0; x < outW; x++ {
			pt := y*outW*3 + x
			log.Printf("r:%d, g:%d, b:%d\n", outData[pt], outData[pt+1], outData[pt+2])
			outImg.SetRGBA(x, y, color.RGBA{outData[pt], outData[pt+1], outData[pt+2], 255})
		}
	}
	return outImg, nil
}
