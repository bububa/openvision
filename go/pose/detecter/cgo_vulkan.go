// +build vulkan

package detecter

/*
#cgo CXXFLAGS:   --std=c++11 -fopenmp
#cgo CPPFLAGS:   -I ${SRCDIR}/../../../include -I /usr/local/include
#cgo LDFLAGS: -lstdc++ -lncnn -lomp -lopenvision -lglslang -lvulkan -lSPIRV -lOGLCompiler -lMachineIndependent -lGenericCodeGen -lOSDependent
#cgo LDFLAGS: -L /usr/local/lib -L ${SRCDIR}/../../../lib
*/
import "C"
