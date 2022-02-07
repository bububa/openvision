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
	"github.com/bububa/openvision/go/pose/segmentor"
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
	for idx, seg := range []segmentor.Segmentor{
		deeplabv3(modelPath),
		erdnet(modelPath),
	} {
		defer seg.Destroy()
		common.SetEstimatorThreads(seg, cpuCores)
		matting(seg, imgPath, "ultralight-pose.jpg", idx)
		//merge(seg, imgPath, "ultralight-pose.jpg", "bg2.jpg", idx)
	}
	seg := rvm(modelPath, 512, 512)
	defer seg.Destroy()
	common.SetEstimatorThreads(seg, cpuCores)
	for i := 1; i < 4; i++ {
		filename := fmt.Sprintf("slice/%d.jpeg", i)
		videomatting(seg, imgPath, filename, i)
	}
}

func deeplabv3(modelPath string) segmentor.Segmentor {
	modelPath = filepath.Join(modelPath, "deeplabv3plus")
	d := segmentor.NewDeeplabv3plus()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func erdnet(modelPath string) segmentor.Segmentor {
	modelPath = filepath.Join(modelPath, "erdnet")
	d := segmentor.NewERDNet()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func rvm(modelPath string, w int, h int) segmentor.Segmentor {
	modelPath = filepath.Join(modelPath, fmt.Sprintf("rvm/mobilenetv3/%dx%d", w, h))
	d := segmentor.NewRVM(w, h)
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func videomatting(seg segmentor.Segmentor, imgPath string, filename string, idx int) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	out := common.NewImage(nil)
	if err := seg.Matting(img, out); err != nil {
		log.Fatalln(err)
	}
	outPath := filepath.Join(imgPath, "./results/videomatting", fmt.Sprintf("%d.jpeg", idx))
	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}

}

func matting(seg segmentor.Segmentor, imgPath string, filename string, idx int) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	out := common.NewImage(nil)
	if err := seg.Matting(img, out); err != nil {
		log.Fatalln(err)
	}
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("poseseg-matting-%d-%s", idx, filename))
	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}

}

func merge(seg segmentor.Segmentor, imgPath string, filename string, bgFilename string, idx int) {
	inPath := filepath.Join(imgPath, filename)
	imgLoaded, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	img := common.NewImage(imgLoaded)
	bgPath := filepath.Join(imgPath, bgFilename)
	bgLoaded, err := loadImage(bgPath)
	if err != nil {
		log.Fatalln("load bg image failed,", err)
	}
	bg := common.NewImage(bgLoaded)
	out := common.NewImage(nil)
	if err := seg.Merge(img, bg, out); err != nil {
		log.Fatalln(err)
	}
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("poseseg-merge-%d-%s", idx, filename))
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
