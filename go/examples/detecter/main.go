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
	"strconv"
	"strings"

	"github.com/llgcode/draw2d"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face/detecter"
	"github.com/bububa/openvision/go/face/drawer"
	facedrawer "github.com/bububa/openvision/go/face/drawer"
)

func main() {
	wd, _ := os.Getwd()
	dataPath := cleanPath(wd, "~/go/src/github.com/bububa/openvision/data")
	imgPath := filepath.Join(dataPath, "./images")
	modelPath := filepath.Join(dataPath, "./models")
	fontPath := filepath.Join(dataPath, "./font")
	common.CreateGPUInstance()
	defer common.DestroyGPUInstance()
	cpuCores := common.GetBigCPUCount()
	common.SetOMPThreads(cpuCores)
	log.Printf("CPU big cores:%d\n", cpuCores)
	test_detect(imgPath, modelPath, fontPath, cpuCores)
	test_mask(imgPath, modelPath, fontPath, cpuCores)
}

func load_font(fontPath string) *common.Font {
	fontCache := common.NewFontCache(fontPath)
	fnt := &common.Font{
		Size: 9,
		Data: &draw2d.FontData{
			Name: "NotoSansCJKsc",
			//Name:   "Roboto",
			Family: draw2d.FontFamilySans,
			Style:  draw2d.FontStyleNormal,
		},
	}
	if err := fnt.Load(fontCache); err != nil {
		log.Fatalln(err)
	}
	return fnt
}
func test_detect(imgPath string, modelPath string, fontPath string, threads int) {
	fnt := load_font(fontPath)
	drawer := facedrawer.New(drawer.WithFont(fnt))
	for idx, d := range []detecter.Detecter{
		retinaface(modelPath),
		centerface(modelPath),
		mtcnn(modelPath),
		yoloface(modelPath),
		scrfd(modelPath),
	} {
		common.SetEstimatorThreads(d, threads)
		detect(d, drawer, imgPath, idx, "4.jpg")
		d.Destroy()
	}
}

func test_mask(imgPath string, modelPath string, fontPath string, threads int) {
	fnt := load_font(fontPath)
	drawer := facedrawer.New(
		facedrawer.WithBorderColor(common.Red),
		facedrawer.WithMaskColor(common.Green),
		facedrawer.WithFont(fnt),
	)
	d := anticonv(modelPath)
	common.SetEstimatorThreads(d, threads)
	defer d.Destroy()
	detect(d, drawer, imgPath, 0, "mask3.jpg")
}

func retinaface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "fd")
	d := detecter.NewRetinaFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func mtcnn(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "mtcnn")
	d := detecter.NewMtcnn()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func yoloface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "yoloface/v505")
	d := detecter.NewYoloFace()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func scrfd(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "scrfd/scrfd1g")
	d := detecter.NewScrfd()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func centerface(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "centerface")
	d := detecter.NewCenterface()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func anticonv(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "mask")
	d := detecter.NewAnticonv()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func detect(d detecter.Detecter, drawer *facedrawer.Drawer, imgPath string, idx int, filename string) {
	inPath := filepath.Join(imgPath, filename)
	img, err := loadImage(inPath)
	if err != nil {
		log.Fatalln("load image failed,", err)
	}
	faces, err := d.Detect(common.NewImage(img))
	if err != nil {
		log.Fatalln(err)
	}

	for idx, face := range faces {
		faces[idx].Label = strconv.FormatFloat(float64(face.Score), 'f', 4, 64)
	}

	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("%d-%s", idx, filename))
	out := drawer.Draw(img, faces)

	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}

	log.Printf("faces: %+v\n", faces)
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
