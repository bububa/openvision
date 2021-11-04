package main

import (
	"bytes"
	"fmt"
	"image"
	"image/jpeg"
	"log"
	"os"
	"os/user"
	"path/filepath"
	"strings"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/pose/detecter"
	posedrawer "github.com/bububa/openvision/go/pose/drawer"
	"github.com/bububa/openvision/go/pose/estimator"
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	common.CreateGPUInstance()
	defer common.DestroyGPUInstance()
	cpuCores := common.GetBigCPUCount()
	common.SetOMPThreads(cpuCores)
	log.Printf("CPU big cores:%d\n", cpuCores)
	for did, d := range []detecter.Detecter{
		ultralightDetector(modelPath),
		openposeDetector(modelPath),
	} {
		defer d.Destroy()
		common.SetEstimatorThreads(d, cpuCores)
		for mid, m := range []estimator.Estimator{
			ultralightEstimator(modelPath),
			moveNetEstimator(modelPath, estimator.MoveNetType_Lightning),
			moveNetEstimator(modelPath, estimator.MoveNetType_Thunder),
		} {
			defer m.Destroy()
			common.SetEstimatorThreads(d, cpuCores)
			detect(d, m, imgPath, "ultralight-pose.jpg", did, mid)
		}
	}
}

func ultralightDetector(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "ultralight-pose/roi")
	d := detecter.NewUltralight()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func openposeDetector(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "openpose")
	d := detecter.NewOpenPose()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func ultralightEstimator(modelPath string) estimator.Estimator {
	modelPath = filepath.Join(modelPath, "ultralight-pose/pose")
	d := estimator.NewUltralight()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func moveNetEstimator(modelPath string, modelType estimator.MoveNetType) estimator.Estimator {
	if modelType == estimator.MoveNetType_Lightning {
		modelPath = filepath.Join(modelPath, "movenet/lightning")
	} else {
		modelPath = filepath.Join(modelPath, "movenet/thunder")
	}
	d := estimator.NewMoveNet(modelType)
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func detect(d detecter.Detecter, m estimator.Estimator, imgPath string, filename string, did int, mid int) {
	inPath := filepath.Join(imgPath, filename)
	imgSrc, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgSrc)
	rois, err := d.Detect(img)
	if err != nil {
		log.Fatalln(err)
	}
	if !d.HasKeypoints() {
		for idx, roi := range rois {
			keypoints, err := m.ExtractKeypoints(img, roi.Rect)
			if err != nil {
				log.Fatalln(err)
			}
			rois[idx].Keypoints = keypoints
		}
	}
	var useOpenPose bool
	if did == 1 {
		useOpenPose = true
	}

	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("pose-%d-%d-%s", did, mid, filename))

	drawer := posedrawer.New()

	out := drawer.Draw(img, rois, true, useOpenPose)

	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}

}

func loadImage(filePath string) (image.Image, error) {
	fn, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer fn.Close()
	img, _, err := image.Decode(fn)
	if err != nil {
		return nil, err
	}
	return img, nil
}

func saveImage(img image.Image, filePath string) error {
	buf := new(bytes.Buffer)
	if err := jpeg.Encode(buf, img, nil); err != nil {
		return err
	}
	fn, err := os.Create(filePath)
	if err != nil {
		return err
	}
	defer fn.Close()
	fn.Write(buf.Bytes())
	return nil
}

func cleanPath(wd string, path string) string {
	usr, _ := user.Current()
	dir := usr.HomeDir
	if path == "~" {
		return dir
	} else if strings.HasPrefix(path, "~/") {
		return filepath.Join(dir, path[2:])
	}
	return filepath.Join(wd, path)
}
