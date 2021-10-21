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
	"github.com/bububa/openvision/go/face"
	"github.com/bububa/openvision/go/face/detecter"
	facedrawer "github.com/bububa/openvision/go/face/drawer"
	"github.com/bububa/openvision/go/face/tracker"
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	d := retinaface(modelPath)
	defer d.Destroy()
	t := tracker.NewTracker()
	defer t.Destroy()
	track(d, t, imgPath, "4.jpg")
}

func retinaface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "fd")
	d := detecter.NewRetinaFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func track(d detecter.Detecter, t *tracker.Tracker, imgPath string, filename string) {
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
	trackedFaces, err := t.Track(img, faces)
	if err != nil {
		log.Fatalln(err)
	}
	infos := make([]face.FaceInfo, 0, len(trackedFaces))
	for _, f := range trackedFaces {
		infos = append(infos, f.Face)
	}
	drawer := facedrawer.New()
	out := drawer.Draw(img, infos)
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("track-%s", filename))

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
