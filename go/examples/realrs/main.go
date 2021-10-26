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
	"github.com/bububa/openvision/go/realsr"
	"github.com/bububa/openvision/go/realsr/realesrgan"
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	common.CreateGPUInstance()
	defer common.DestroyGPUInstance()
	d := real_esrgan(modelPath)
	defer d.Destroy()
	process(d, imgPath, "realsr-1.jpg")
}

func real_esrgan(modelPath string) realsr.Processer {
	gpuCount := common.GetGPUCount()
	log.Printf("gpu: %d\n", gpuCount)
	modelPath = filepath.Join(modelPath, "realesrgan/x4-jpeg")
	d := realesrgan.NewRealERSGAN(1, false)
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func process(d realsr.Processer, imgPath string, filename string) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	out, err := d.Process(img, 4)
	if err != nil {
		log.Fatalln(err)
	}
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("realrs-%s", filename))

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
