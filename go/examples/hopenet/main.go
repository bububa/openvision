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
	"github.com/bububa/openvision/go/face/hopenet"
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
	h := processer(modelPath)
	defer h.Destroy()
	common.SetEstimatorThreads(h, cpuCores)
	for _, fn := range []string{"robocop.jpg", "terminator.jpg"} {
		process(d, h, imgPath, fn)
	}
}

func processer(modelPath string) *hopenet.Hopenet {
	gpuCount := common.GetGPUCount()
	log.Printf("gpu: %d\n", gpuCount)
	modelPath = filepath.Join(modelPath, "hopenet")
	d := hopenet.NewHopenet()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func retinaface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "fd")
	d := detecter.NewRetinaFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func process(d detecter.Detecter, h *hopenet.Hopenet, imgPath string, filename string) {
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
	drawer := facedrawer.New(facedrawer.WithBorderStrokeWidth(4))
	for _, face := range faces {
		pose, err := h.Detect(img, face.Rect)
		if err != nil {
			log.Fatalln(err)
		}
		log.Printf("pose: %+v\n", pose)
		out := drawer.DrawHeadPose(imgLoaded, pose, face.Rect)
		outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("hopenet-%s", filename))

		if err := saveImage(out, outPath); err != nil {
			log.Fatalln(err)
		}
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
