#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

#include <xmmintrin.h>
#include <pmmintrin.h>

#include "wrapper.hpp"

RTCDevice device;

struct Vertex {
    float x, y, z, a;
};

struct Triangle {
    int v0, v1, v2;
};

// rtcDeleteGeometry(scene, geomID);
// rtcDeleteScene(scene);
// rtcDeleteDevice(device);

void init() {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    device = rtcNewDevice(NULL);
}

void *createMesh(int numTriangles, float *data) {
    RTCScene scene = rtcDeviceNewScene(device, RTC_SCENE_STATIC, RTC_INTERSECT1);
    unsigned geomID = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, numTriangles, numTriangles * 3, 1);

    Vertex *vertices = (Vertex *)rtcMapBuffer(scene, geomID, RTC_VERTEX_BUFFER);
    for (int i = 0; i < numTriangles; i++) {
        vertices[i*3+0] = Vertex{data[i*9+0], data[i*9+1], data[i*9+2]};
        vertices[i*3+1] = Vertex{data[i*9+3], data[i*9+4], data[i*9+5]};
        vertices[i*3+2] = Vertex{data[i*9+6], data[i*9+7], data[i*9+8]};
    }
    rtcUnmapBuffer(scene, geomID, RTC_VERTEX_BUFFER);

    Triangle *triangles = (Triangle *)rtcMapBuffer(scene, geomID, RTC_INDEX_BUFFER);
    for (int i = 0; i < numTriangles; i++) {
        triangles[i] = Triangle{i*3+0, i*3+1, i*3+2};
    }
    rtcUnmapBuffer(scene, geomID, RTC_INDEX_BUFFER);

    rtcCommit(scene);
    return scene;
}

void intersectMesh(void *mesh, float ox, float oy, float oz, float dx, float dy, float dz, float *t, int *index) {
    RTCScene scene = (RTCScene)mesh;

    RTCRay ray;
    ray.org[0] = ox; ray.org[1] = oy; ray.org[2] = oz;
    ray.dir[0] = dx; ray.dir[1] = dy; ray.dir[2] = dz;
    ray.tnear = 0;
    ray.tfar = 1.0 / 0.0;
    ray.geomID = RTC_INVALID_GEOMETRY_ID;
    ray.primID = RTC_INVALID_GEOMETRY_ID;
    ray.mask = -1;
    ray.time = 0;

    rtcIntersect(scene, ray);

    *t = ray.tfar;
    *index = ray.primID;
}
