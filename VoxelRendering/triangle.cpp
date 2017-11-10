#include "triangle.h"

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->material = m;
	Vec3f::Cross3(normal, b - a, c - a);
	this->ca = a - c;
	this->cb = b - c;
	this->triangle = true;
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
	bool result = false;
	Vec3f l = r.getOrigin() - c;
	Vec3f k = -1.0f * r.getDirection();
	float det = k.Dot3(normal);
	if (det != 0)
	{
		float dett = l.Dot3(normal);
		Vec3f tmp1 = Vec3f();
		Vec3f tmp2 = Vec3f();
		Vec3f::Cross3(tmp1, l, cb);
		Vec3f::Cross3(tmp2, ca, l);
		float detu = k.Dot3(tmp1);
		float detv = k.Dot3(tmp2);

		float t = dett / det;
		float u = detu / det;
		float v = detv / det;

		if (t > tmin && u >= 0 && v >= 0 && 1 - u - v >= 0)
		{
			if (t < h.getT())
			{
				result = true;
				Vec3f n = normal;
				n.Normalize();
				h.set(t, material, n, r);
			}
		}
	}

	return result;
}

void Triangle::paint()
{
	material->glSetMaterial();
	glBegin(GL_TRIANGLES);
	glNormal3f(normal.x(), normal.y(), normal.z());
	glVertex3f(a.x(), a.y(), a.z());
	glVertex3f(b.x(), b.y(), b.z());
	glVertex3f(c.x(), c.y(), c.z());
	glEnd();
}

BoundingBox* Triangle::getBoundingBox()
{
	boundingBox = new BoundingBox(Vec3f(MIN(a.x(), b.x(), c.x()), MIN(a.y(), b.y(), c.y()), MIN(a.z(), b.z(), c.z())), Vec3f(MAX(a.x(), b.x(), c.x()), MAX(a.y(), b.y(), c.y()), MAX(a.z(), b.z(), c.z())));
	return boundingBox;
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
{
	Vec3f m_min = boundingBox->getMin();
	Vec3f m_max = boundingBox->getMax();
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

	int _start_i = (fabs(m_min.x() - min.x())) * (1 / grid_x);
	int _start_j = (fabs(m_min.y() - min.y())) * (1 / grid_y);
	int _start_k = (fabs(m_min.z() - min.z())) * (1 / grid_z);
	int _end_i = (fabs(m_max.x() - min.x()))*(1 / grid_x);
	int _end_j = (fabs(m_max.y() - min.y()))*(1 / grid_y);
	int _end_k = (fabs(m_max.z() - min.z()))*(1 / grid_z);

	if (_start_i == _end_i) _end_i++;
	if (_start_j == _end_j) _end_j++;
	if (_start_k == _end_k) _end_k++;
	if (_start_i > _end_i) swap(_start_i, _end_i);
	if (_start_j > _end_j) swap(_start_j, _end_j);
	if (_start_k > _end_k) swap(_start_k, _end_k);

	assert(_end_i <= x && _end_j <= y && _end_k <= z);

	for (int _i = _start_i; _i < _end_i; _i++)
	{
		for (int _j = _start_j; _j < _end_j; _j++) 
		{
			for (int _k = _start_k; _k < _end_k; _k++) 
			{
				g->insertIntoThis((_i * y + _j) * z + _k, true, this);
			}
		}
	}
}