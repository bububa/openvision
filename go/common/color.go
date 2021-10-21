package common

import (
	"fmt"
	"image/color"
	"strings"
)

// ColorFromHex get color from hex
func ColorFromHex(hexColor string) color.RGBA {
	r, g, b, a := parseHexColor(hexColor)
	return color.RGBA{uint8(r), uint8(g), uint8(b), uint8(a)}
}

// parseHexColor parse color hex string to rgba value
func parseHexColor(x string) (r, g, b, a uint32) {
	if !strings.HasPrefix(x, "#") {
		return color.Transparent.RGBA()
	}
	x = strings.TrimPrefix(x, "#")
	a = 255
	if len(x) == 3 {
		format := "%1x%1x%1x"
		fmt.Sscanf(x, format, &r, &g, &b)
		r |= r << 4
		g |= g << 4
		b |= b << 4
	}
	if len(x) == 6 {
		format := "%02x%02x%02x"
		fmt.Sscanf(x, format, &r, &g, &b)
	}
	if len(x) == 8 {
		format := "%02x%02x%02x%02x"
		fmt.Sscanf(x, format, &r, &g, &b, &a)
	}
	return
}

const (
	Green = "#64DD17"
	Pink  = "#E91E63"
	Red   = "#FF1744"
)
