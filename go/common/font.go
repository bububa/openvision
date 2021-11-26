package common

import (
	"errors"

	"github.com/golang/freetype/truetype"
	"github.com/llgcode/draw2d"
)

// Font font info
type Font struct {
	// Cache FontCache
	Cache draw2d.FontCache
	// Size font size
	Size float64 `json:"size,omitempty"`
	// Data font setting
	Data *draw2d.FontData `json:"data,omitempty"`
	// Font
	Font *truetype.Font `json:"-"`
}

// Load font from font cache
func (f *Font) Load(cache draw2d.FontCache) error {
	if f.Font != nil {
		return nil
	}
	if f.Data == nil {
		return nil
	}
	if cache == nil {
		return errors.New("missing font cache")
	}
	ft, err := cache.Load(*f.Data)
	if err != nil {
		return err
	}
	f.Cache = cache
	f.Font = ft
	return nil
}

// NewFontCache load font cache
func NewFontCache(fontFolder string) *draw2d.SyncFolderFontCache {
	return draw2d.NewSyncFolderFontCache(fontFolder)
}
