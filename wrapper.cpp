#include <embree3/rtcore.h>

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
    RTCScene scene = rtcNewScene(device);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    Vertex *vertexBuf = (Vertex *)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
        sizeof(Vertex), numTriangles * 3);
    for (int i = 0; i < numTriangles; i++) {
        vertexBuf[i*3+0] = Vertex{data[i*9+0], data[i*9+1], data[i*9+2]};
        vertexBuf[i*3+1] = Vertex{data[i*9+3], data[i*9+4], data[i*9+5]};
        vertexBuf[i*3+2] = Vertex{data[i*9+6], data[i*9+7], data[i*9+8]};
    }

    Triangle *triangleBuf = (Triangle *)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
        sizeof(Triangle), numTriangles);
    for (int i = 0; i < numTriangles; i++) {
        triangleBuf[i] = Triangle{i*3+0, i*3+1, i*3+2};
    }

    rtcCommitGeometry(geom);
    rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(scene);

    return scene;
}

void intersectMesh(void *mesh, float ox, float oy, float oz, float dx, float dy, float dz, float *t, int *index) {
    RTCScene scene = (RTCScene)mesh;

    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    RTCRayHit r;
    r.ray.org_x = ox; r.ray.org_y = oy; r.ray.org_z = oz;
    r.ray.dir_x = dx; r.ray.dir_y = dy; r.ray.dir_z = dz;
    r.ray.tnear = 0;
    r.ray.tfar = 1.0 / 0.0;
    r.ray.mask = -1;
    r.ray.flags = 0;
    r.ray.time = 0;
    r.ray.id = 0;

    r.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    r.hit.primID = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(scene, &context, &r);

    *t = r.ray.tfar;
    *index = r.hit.primID;
}
