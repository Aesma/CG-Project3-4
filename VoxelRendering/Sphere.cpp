#include "sphere.h"
Sphere::Sphere(Vec3f center, float radius, Material* material)
{
	this->center = center;
	this->radius = radius;
	this->material = material;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
	bool result = false;

	Vec3f direction = r.getDirection();
	Vec3f origin = r.getOrigin();

	float a = direction.Dot3(direction);
	float b = 2.0f * direction.Dot3(origin - (center));
	float c = (origin - (center)).Length() * (origin - (center)).Length() - radius * radius;
	float delta = b * b - 4 * a * c;
	float t = INT_MAX * 1.0f;
	if (delta >= 0)
	{
		float d = sqrt(delta);
		float t1 = (-b - d) / (a * 2.0f);
		float t2 = (-b + d) / (a * 2.0f);
		if (t1 >= tmin)
		{
			t = t1;
			result = true;
		}
		else if (t2 >= tmin)
		{
			t = t2;
			result = true;
		}
		if (t < h.getT())
		{
			Vec3f normal = origin + direction * t - center;
			normal.Normalize();
			h.set(t, material, normal, r);
		}
	}
	return result;
}

void Sphere::paint()
{
	material->glSetMaterial();
	glPushMatrix();
	glTranslatef(center.x(), center.y(), center.z());
	glutSolidSphere(radius, 100, 100);
	glPopMatrix();
}

BoundingBox* Sphere::getBoundingBox(){
	boundingBox = new BoundingBox(Vec3f(center.x() - radius, center.y() - radius, center.z() - radius), Vec3f(center.x() + radius, center.y() + radius, center.z() + radius));

	return boundingBox;
}

void Sphere::insertIntoGrid(Grid *g, Matrix *m)
{
	if (m != NULL){
		Object3D::insertIntoGrid(g, m);
		return;
	}
	Vec3f v = g->getGird();
	BoundingBox *bb = g->getBoundingBox();
	Vec3f min = bb->getMin();
	Vec3f max = Vec3f(bb->getMax().x() + FLT_EPSILON, bb->getMax().y() + FLT_EPSILON, bb->getMax().z() + FLT_EPSILON);
	int x = v.x();
	int y = v.y();
	int z = v.z();
	Vec3f size = max - min;
	float grid_x = size.x() / x;
	float grid_y = size.y() / y;
	float grid_z = size.z() / z;
	Vec3f cen = center - min;
	Vec3f _voxel;
	for (int _i = 0; _i < x; _i++) 
	{
		float _x1 = (_i + 0.5f)*grid_x;
		for (int _j = 0; _j < y; _j++) 
		{
			float _y1 = (_j + 0.5f)*grid_y;
			for (int _k = 0; _k < z; _k++)
			{
				float _z1 = (_k + 0.5f)*grid_z;
				_voxel.Set(_x1, _y1, _z1);
				if ((_voxel - cen).Length() <= radius){
					g->insertIntoThis((_i * y + _j) * z + _k, true, this);
				}
			}
		}
	}
}

Sphere::~Sphere()
{
	
}