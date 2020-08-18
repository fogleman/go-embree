package embree

import (
	"fmt"
	"testing"
)

func TestIntersect(t *testing.T) {
	triangles := []Triangle{
		Triangle{Vector{0, 0, 0}, Vector{1, 0, 0}, Vector{1, 1, 0}},
	}

	mesh := NewMesh(triangles)

	org := Vector{0, 0, -1}
	dir := Vector{0, 0, 1}
	ray := Ray{org, dir}

	hit := mesh.Intersect(ray)
	fmt.Println(hit)
}
