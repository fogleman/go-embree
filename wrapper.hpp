#ifdef __cplusplus
extern "C" {
#endif

void init();
void *createMesh(int numTriangles, int numPoints, float *points, int *indexes);
void intersectMesh(void *mesh, float ox, float oy, float oz, float dx, float dy, float dz, float *t, int *index);

#ifdef __cplusplus
}
#endif
