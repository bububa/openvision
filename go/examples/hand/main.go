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
	"github.com/bububa/openvision/go/hand/detecter"
	handdrawer "github.com/bububa/openvision/go/hand/drawer"
	"github.com/bububa/openvision/go/hand/pose"
	"github.com/bububa/openvision/go/hand/pose3d"
	"github.com/llgcode/draw2d"
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
	// estimator := handpose(modelPath)
	// defer estimator.Destroy()
	// common.SetEstimatorThreads(estimator, cpuCores)
	// for idx, d := range []detecter.Detecter{
	// 	yolox(modelPath),
	// 	nanodet(modelPath),
	// } {
	// 	defer d.Destroy()
	// 	common.SetEstimatorThreads(d, cpuCores)
	// 	detect(d, estimator, imgPath, "hand2.jpg", idx)
	// }
	d3d := mediapipe(modelPath)
	detect3d(d3d, imgPath, fontPath, "hand1.jpg")
	detect3d(d3d, imgPath, fontPath, "hand2.jpg")
}

func yolox(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "yolox-hand/relu")
	d := detecter.NewYolox()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func nanodet(modelPath string) detecter.Detecter {
	modelPath = filepath.Join(modelPath, "nanodet-hand")
	d := detecter.NewNanodet()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func handpose(modelPath string) pose.Estimator {
	modelPath = filepath.Join(modelPath, "handpose")
	d := pose.NewHandPoseEstimator()
	if err := d.LoadModel(modelPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func mediapipe(modelPath string) *pose3d.Mediapipe {
	palmPath := filepath.Join(modelPath, "mediapipe/palm/full")
	handPath := filepath.Join(modelPath, "mediapipe/hand/full")
	d := pose3d.NewMediapipe()
	if err := d.LoadModel(palmPath, handPath); err != nil {
		log.Fatalln(err)
	}
	return d
}

func detect(d detecter.Detecter, e pose.Estimator, imgPath string, filename string, idx int) {
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
	log.Printf("%+v\n", rois)
	for idx, roi := range rois {
		keypoints, err := e.Detect(img, roi.Rect)
		if err != nil {
			log.Fatalln(err)
			continue
		}
		pts := make([]common.Keypoint, 0, len(keypoints))
		for _, pt := range keypoints {
			pts = append(pts, common.Keypoint{
				Point: pt,
			})
		}
		rois[idx].Keypoints = pts
		log.Printf("keypoints: %d\n", len(keypoints))
	}

	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("%d-hand-%s", idx, filename))

	drawer := handdrawer.New()

	out := drawer.Draw(img, rois, true)

	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}
}

func detect3d(d *pose3d.Mediapipe, imgPath string, fontPath string, filename string) {
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
	// log.Printf("%+v\n", rois)
	fnt := load_font(fontPath)
	drawer := handdrawer.New(handdrawer.WithFont(fnt))
	outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("pose3d-hand-%s", filename))
	out := drawer.DrawPalm(img, rois)

	if err := saveImage(out, outPath); err != nil {
		log.Fatalln(err)
	}

	for idx, roi := range rois {
		outPath := filepath.Join(imgPath, "./results", fmt.Sprintf("pose3d-palm3d-%d-%s", idx, filename))
		out := drawer.DrawPalm3D(roi, 400, "#442519")

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
