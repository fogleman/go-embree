package embree

// #cgo CXXFLAGS: -D__forceinline=inline -DTASKING_TBB -D__TARGET_AVX2__ -D__TARGET_AVX__ -D__TARGET_SSE42__ -fPIC -std=c++11 -fno-strict-aliasing -Wno-narrowing -Wno-deprecated-register -fvisibility-inlines-hidden -fvisibility=hidden -mmacosx-version-min=10.7 -stdlib=libc++ -DNDEBUG -O3 -mavx
// #cgo LDFLAGS: -lembree
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

func index(triangles []Triangle) ([]Vector, []int) {
	lookup := make(map[Vector]int)
	var points []Vector
	for _, t := range triangles {
		if _, ok := lookup[t.A]; !ok {
			lookup[t.A] = len(points)
			points = append(points, t.A)
		}
		if _, ok := lookup[t.B]; !ok {
			lookup[t.B] = len(points)
			points = append(points, t.B)
		}
		if _, ok := lookup[t.C]; !ok {
			lookup[t.C] = len(points)
			points = append(points, t.C)
		}
	}
	indexes := make([]int, len(triangles)*3)
	for i, t := range triangles {
		indexes[i*3+0] = lookup[t.A]
		indexes[i*3+1] = lookup[t.B]
		indexes[i*3+2] = lookup[t.C]
	}
	return points, indexes
}

func NewMesh(triangles []Triangle) *Mesh {
	points, indexes := index(triangles)
	cpoints := make([]C.float, len(points)*3)
	for i, p := range points {
		cpoints[i*3+0] = C.float(p.X)
		cpoints[i*3+1] = C.float(p.Y)
		cpoints[i*3+2] = C.float(p.Z)
	}
	cindexes := make([]C.int, len(indexes))
	for i, x := range indexes {
		cindexes[i] = C.int(x)
	}
	return &Mesh{C.createMesh(C.int(len(triangles)), C.int(len(points)), &cpoints[0], &cindexes[0])}
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
