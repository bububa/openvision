// +build vulkan

package common

/*
#cgo CXXFLAGS:   --std=c++11 -fopenmp
#cgo CFLAGS: -DMIRROR_VULKAN=ON -DMIRROR_OPENMP=ON
#cgo CPPFLAGS:   -I ${SRCDIR}/../../include -I /usr/local/include
#cgo LDFLAGS: -lstdc++ -lncnn -lomp -lopenvision -lglslang -lvulkan -lSPIRV -lOGLCompiler -lMachineIndependent -lGenericCodeGen -lOSDependent
#cgo LDFLAGS: -L /usr/local/lib -L ${SRCDIR}/../../lib
*/
import "C"
