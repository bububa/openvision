package drawer

// Option represents Drawer option interface
type Option interface {
	apply(*Drawer)
}

type optionFunc func(d *Drawer)

func (fn optionFunc) apply(d *Drawer) {
	fn(d)
}

// WithBorderColor set Drawer BorderColor
func WithBorderColor(color string) Option {
	return optionFunc(func(d *Drawer) {
		d.BorderColor = color
	})
}

// WithKeypointColor set Drawer KeypointColor
func WithKeypointColor(color string) Option {
	return optionFunc(func(d *Drawer) {
		d.KeypointColor = color
	})
}

// WithBorderStrokeWidth set Drawer BorderStrokeWidth
func WithBorderStrokeWidth(w float64) Option {
	return optionFunc(func(d *Drawer) {
		d.BorderStrokeWidth = w
	})
}

// WithKeypointRadius set Drawer KeypointRadius
func WithKeypointRadius(r float64) Option {
	return optionFunc(func(d *Drawer) {
		d.KeypointRadius = r
	})
}

// WithKeypointStrokeWidth set Drawer KeypointStrokeWidth
func WithKeypointStrokeWidth(w float64) Option {
	return optionFunc(func(d *Drawer) {
		d.KeypointStrokeWidth = w
	})
}

// WithInvalidBorderColor set Drawer InvalidBorderColor
func WithInvalidBorderColor(color string) Option {
	return optionFunc(func(d *Drawer) {
		d.InvalidBorderColor = color
	})
}

// WithMaskColor set Drawer MaskColor
func WithMaskColor(color string) Option {
	return optionFunc(func(d *Drawer) {
		d.MaskColor = color
	})
}
