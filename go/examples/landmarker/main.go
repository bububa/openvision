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
	"github.com/bububa/openvision/go/face/detecter"
	facedrawer "github.com/bububa/openvision/go/face/drawer"
	"github.com/bububa/openvision/go/face/landmarker"
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
	d := retinaface(modelPath)
	defer d.Destroy()
	common.SetEstimatorThreads(d, cpuCores)
	for idx, m := range []landmarker.Landmarker{
		insightface(modelPath),
		zq(modelPath),
		scrfd(modelPath),
	} {
		defer m.Destroy()
		common.SetEstimatorThreads(m, cpuCores)
		extract_keypoints(d, m, imgPath, "4.jpg", idx)
	}
}

func retinaface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "fd")
	d := detecter.NewRetinaFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func insightface(modelPath string) landmarker.Landmarker {
	modelPath = filepath.Join(modelPath, "insightface")
	d := landmarker.NewInsightface()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func zq(modelPath string) landmarker.Landmarker {
	modelPath = filepath.Join(modelPath, "zq")
	d := landmarker.NewZq()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func scrfd(modelPath string) landmarker.Landmarker {
	modelPath = filepath.Join(modelPath, "scrfd/landmarker")
	d := landmarker.NewScrfd()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func extract_keypoints(d detecter.Detecter, m landmarker.Landmarker, imgPath string, filename string, idx int) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	faces, err := d.Detect(img)
	if err != nil {
		log.Fatalln(err)
	}
	drawer := facedrawer.New(
		facedrawer.WithKeypointColor(common.Red),
		facedrawer.WithKeypointRadius(1),
		facedrawer.WithKeypointStrokeWidth(0),
	)
	var keypoints []common.Point
	for _, face := range faces {
		rect := face.Rect
		points, err := m.ExtractKeypoints(img, rect)
		if err != nil {
			log.Fatalln(err)
		}
		keypoints = append(keypoints, points...)
	}
	out := drawer.DrawLandmark(imgLoaded, keypoints)
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("landmarker-%d-%s", idx, filename))
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
