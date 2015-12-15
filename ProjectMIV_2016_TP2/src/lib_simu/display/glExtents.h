
#ifndef GL_EXTENTS_H
#define GL_EXTENTS_H

#include "AllMath.h"

#include "MyMath.h"

//new useful functions...

void glexSolidBox(float size_x, float size_y, float size_z);
void glexSolidBox2(float size_x, float size_y, float size_z);
void glexSolidSphere(double radius, int slices = 10, int stacks = 10);

void glexInitShadowing();
void glexDrawShadow(float darkness = 0.5f);

void glexBeginFBShadowing();
void glexEndFBShadowing();
void glexSetShadowTrans(float* mat);
void glexSetShadowLightDir(Maths::Vector3& light_dir);
void glexDrawTriangleShadow(Maths::Vector3& p1, Maths::Vector3& p2, Maths::Vector3& p3);
void glexBeginFrontShadowing();
void glexBeginBackShadowing();
void glexSolidBoxShadow(float size_x, float size_y, float size_z);
void glexSolidSphereShadow(float size_x, int slices = 10, int stacks = 10);

void glexDefaultLightAndColor();
void glexSetLightPos();

void glexCameraFromAnglesZoom(float angle_x, float angle_y, float zoom, float c_x, float c_y, float c_z);

void glexDrawString(int x, int y,const char *text);

void glexDrawNumber(int x, int y, double num);
void glexDrawNumber(int x, int y, float num);
void glexDrawNumber(int x, int y, int num);
void glexDrawNumber(int x, int y, long num);

void glexSetVSyncState(bool enable);

#endif