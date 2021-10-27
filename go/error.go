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
	DetectPoseError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "detect pose failed",
		}
	}
	RealsrError = func(code int) Error {
		return Error{
			Code:    code,
			Message: "super-resolution process error",
		}
	}
)
