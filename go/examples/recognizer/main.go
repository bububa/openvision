package main

import (
	"bytes"
	"image"
	"image/jpeg"
	"log"
	"os"
	"os/user"
	"path/filepath"
	"strings"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face/detecter"
	"github.com/bububa/openvision/go/face/recognizer"
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	common.CreateGPUInstance()
	defer common.DestroyGPUInstance()
	d := retinaface(modelPath)
	defer d.Destroy()
	m := mobilefacenet(modelPath)
	defer m.Destroy()
	extract_features(d, m, imgPath, "4.jpg")
}

func retinaface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "fd")
	d := detecter.NewRetinaFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func mobilefacenet(modelPath string) recognizer.Recognizer {
	modelPath = filepath.Join(modelPath, "mobilefacenet")
	d := recognizer.NewMobilefacenet()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func extract_features(d detecter.Detecter, r recognizer.Recognizer, imgPath string, filename string) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	faces, err := d.DetectFace(img)
	if err != nil {
		log.Fatalln(err)
	}
	for _, face := range faces {
		rect := face.Rect
		log.Printf("x:%f, y:%f, w:%f, h:%f\n", rect.X*img.WidthF64(), rect.Y*img.HeightF64(), rect.Width*img.WidthF64(), rect.Height*img.HeightF64())
		features, err := r.ExtractFeatures(img, rect)
		if err != nil {
			log.Fatalln(err)
		}
		log.Println(len(features))
		log.Printf("feature: %+v\n", features)
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
