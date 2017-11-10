#ifndef OBJECT3D_H
#define OBJECT3D_H

#define MIN(a,b,c) (a<b?(a<c?a:c):(b<c?b:c))
#define MAX(a,b,c) (a>b?(a>c?a:c):(b>c?b:c))

#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>

#include "hit.h"
#include "material.h"
#include "ray.h"
#include "boundingbox.h"
#include "object3dvector.h"

class Grid;

class Object3D
{
public:
	Object3D(){ material = NULL; };
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
	virtual void paint() = 0;
	virtual BoundingBox * getBoundingBox(){ boundingBox = new BoundingBox(Vec3f(INT_MAX, INT_MAX, INT_MAX), Vec3f(INT_MIN, INT_MIN, INT_MIN)); return boundingBox; };
	virtual void insertIntoGrid(Grid *g, Matrix *m){};
	~Object3D(){};
public:
	bool triangle = false;
	Vec3f a;
	Vec3f b;
	Vec3f c;
	Material *material;
	BoundingBox *boundingBox;
};

#endif