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
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	common.CreateGPUInstance()
	defer common.DestroyGPUInstance()
	d := ultralightDetector(modelPath)
	defer d.Destroy()
	detect(d, imgPath, "ultralight-pose3.jpg")
}

func ultralightDetector(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "ultralight-pose")
	d := detecter.NewUltralight()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func detect(d detecter.Detecter, imgPath string, filename string) {
	inPath := filepath.Join(imgPath, filename)
	img, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	rois, err := d.ExtractKeypoints(common.NewImage(img))
	if err != nil {
		log.Fatalln(err)
	}

	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("pose-%s", filename))

	drawer := posedrawer.New()

	out := drawer.Draw(img, rois, true)

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
