package embree

// #cgo CXXFLAGS: -D__forceinline=inline -DTASKING_TBB -D__TARGET_AVX2__ -D__TARGET_AVX__ -D__TARGET_SSE42__ -fPIC -std=c++11 -fno-strict-aliasing -Wno-narrowing -Wno-deprecated-register -fvisibility-inlines-hidden -fvisibility=hidden -DNDEBUG -O3 -mavx -I/opt/local/include
// #cgo LDFLAGS: -lembree -L/opt/local/lib
// #include "wrapper.hpp"
import "C"
import "unsafe"

func init() {
	C.init()
}

type Vector struct {
	X, Y, Z float64
}

type Triangle struct {
	A, B, C Vector
}

type Mesh struct {
	Handle unsafe.Pointer
}

type Ray struct {
	Org, Dir Vector
}

type Hit struct {
	T     float64
	Index int
}

func NewMesh(triangles []Triangle) *Mesh {
	n := len(triangles)
	data := make([]C.float, n*3*3)
	for i, t := range triangles {
		data[i*9+0] = C.float(t.A.X)
		data[i*9+1] = C.float(t.A.Y)
		data[i*9+2] = C.float(t.A.Z)
		data[i*9+3] = C.float(t.B.X)
		data[i*9+4] = C.float(t.B.Y)
		data[i*9+5] = C.float(t.B.Z)
		data[i*9+6] = C.float(t.C.X)
		data[i*9+7] = C.float(t.C.Y)
		data[i*9+8] = C.float(t.C.Z)
	}
	return &Mesh{C.createMesh(C.int(n), &data[0])}
}

func (m *Mesh) Intersect(ray Ray) Hit {
	ox := C.float(ray.Org.X)
	oy := C.float(ray.Org.Y)
	oz := C.float(ray.Org.Z)
	dx := C.float(ray.Dir.X)
	dy := C.float(ray.Dir.Y)
	dz := C.float(ray.Dir.Z)
	t := C.float(-1)
	index := C.int(-1)
	C.intersectMesh(m.Handle, ox, oy, oz, dx, dy, dz, &t, &index)
	return Hit{float64(t), int(index)}
}
