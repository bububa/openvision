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
	"github.com/bububa/openvision/go/styletransfer"
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
	for _, modelName := range []string{"celeba", "facepaintv1", "facepaintv2"} {
		transfer := animegan2(modelPath, modelName)
		defer transfer.Destroy()
		common.SetEstimatorThreads(transfer, cpuCores)
		transform(transfer, imgPath, "transfer1.jpg", modelName)
	}
}

func animegan2(modelPath string, modelName string) styletransfer.StyleTransfer {
	modelPath = filepath.Join(modelPath, "animegan2", modelName)
	d := styletransfer.NewAnimeGan2()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func transform(transfer styletransfer.StyleTransfer, imgPath string, filename string, modelName string) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	out, err := transfer.Transform(img)
	if err != nil {
		log.Fatalln(err)
	}
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("%s-%s", modelName, filename))
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
