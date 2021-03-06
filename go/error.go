package openvision

// Error customed error
type Error struct {
	// Code .
	Code int
	// Message .
	Message string
}

// Error represents error interface
func (e Error) Error() string {
	return e.Message
}

var (
	LoadModelError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "load model failed",
		}
	}
	DetectFaceError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "detect face failed",
		}
	}
	FaceLandmarkError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "face landmark failed",
		}
	}
	RecognizeFaceError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "recognize face failed",
		}
	}
	AlignFaceError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "align face failed",
		}
	}
	TrackFaceError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "track face failed",
		}
	}
	DetectHeadPoseError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "detect head pose failed",
		}
	}
	HairMattingError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "hair matting failed",
		}
	}
	DetectHandError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "detect hand failed",
		}
	}
	DetectPoseError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "detect pose failed",
		}
	}
	TrackerError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "object tracker error",
		}
	}
	CounterError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "object counter error",
		}
	}
	RealsrError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "super-resolution process error",
		}
	}
	TrainingError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "training process failed",
		}
	}
	ClassifyError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "classify process failed",
		}
	}
)
