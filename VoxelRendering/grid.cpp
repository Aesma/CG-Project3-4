#include "grid.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
{
	this->boundingBox = bb;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	m_is_voxel_opaque.assign(nx * ny * nz, false);
	object3ds.resize(nx*ny*nz);
}

Vec3f Grid::getGird(){
	return Vec3f(nx, ny, nz);
}

BoundingBox * Grid::getBoundingBox()
{
	return boundingBox;
}

void Grid::insertIntoThis(int i, bool v, Object3D *obj)
{
	m_is_voxel_opaque[i] = v;
	object3ds[i] = obj;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
	bool result = false;
	MarchingInfo mi;
	initializeRayMarch(mi, r, tmin);
	if (mi.tmin < h.getT()) {
		while (mi.i < nx && mi.j < ny && mi.k < nz &&
			mi.i >= 0 && mi.j >= 0 && mi.k >= 0){
			if (m_is_voxel_opaque[(mi.i * ny + mi.j) * nz + mi.k]) {
				if (object3ds[(mi.i * ny + mi.j) * nz + mi.k]->material == NULL)
					object3ds[(mi.i * ny + mi.j) * nz + mi.k]->material = material;
				h.set(mi.tmin, object3ds[(mi.i * ny + mi.j) * nz + mi.k]->material, mi.normal, r);
				result = true;
				break;
			}
			mi.nextCell();
		}
	}

	return result;
}

void Grid::paint()
{
	material->glSetMaterial();
	boundingBox->paint();
	Vec3f size = boundingBox->getMax() - boundingBox->getMin();
	float grid_x = size.x() / nx;
	float grid_y = size.y() / ny;
	float grid_z = size.z() / nz;
	Vec3f center = boundingBox->getMin();
	glPushMatrix();
	glTranslatef(center.x(), center.y(), center.z());
	for (int i = 0; i < nx*ny*nz; i++)
	{
		if (m_is_voxel_opaque[i]){
			int kk = i % nz;
			int jj = ((i - kk) / nz) % ny;
			int ii = (((i - kk) / nz) - jj) / ny;
			glBegin(GL_QUADS);
			glVertex3f(ii*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, kk*grid_z);
			glVertex3f(ii*grid_x, jj*grid_y, kk*grid_z);

			glVertex3f(ii*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f(ii*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glVertex3f(ii*grid_x, (jj + 1)*grid_y, kk*grid_z);
			glVertex3f(ii*grid_x, jj*grid_y, kk*grid_z);

			glVertex3f(ii*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, kk*grid_z);
			glVertex3f(ii*grid_x, (jj + 1)*grid_y, kk*grid_z);

			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, kk*grid_z);
			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, kk*grid_z);

			glVertex3f(ii*grid_x, jj*grid_y, kk*grid_z);
			glVertex3f(ii*grid_x, (jj + 1)*grid_y, kk*grid_z);
			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, kk*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, kk*grid_z);

			glVertex3f(ii*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, jj*grid_y, (kk + 1)*grid_z);
			glVertex3f((ii + 1)*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glVertex3f(ii*grid_x, (jj + 1)*grid_y, (kk + 1)*grid_z);
			glEnd();
		}
	}
	glPopMatrix();
}

void Grid::initializeRayMarch(MarchingInfo &mi,
	const Ray &r, float tmin) const
{
	const Vec3f D = r.getDirection();
	const Vec3f O = r.getOrigin();

	float _tnear = tmin;
	float _tfar = INFINITY;
	Vec3f _n(0.0f, 0.0f, 0.0f);

	assert(_tnear >= 0);

	// x axy
	float _dx = D.x();
	float _t1x = (boundingBox->getMin().x() - O.x()) / _dx;
	float _t2x = (boundingBox->getMax().x() - O.x()) / _dx;
	if (_t1x > _t2x) { float _tmp = _t1x; _t1x = _t2x; _t2x = _tmp; }

	// y axy
	float _dy = D.y();
	float _t1y = (boundingBox->getMin().y() - O.y()) / _dy;
	float _t2y = (boundingBox->getMax().y() - O.y()) / _dy;
	if (_t1y > _t2y) { float _tmp = _t1y; _t1y = _t2y; _t2y = _tmp; }

	// z axy
	float _dz = D.z();
	float _t1z = (boundingBox->getMin().z() - O.z()) / _dz;
	float _t2z = (boundingBox->getMax().z() - O.z()) / _dz;
	if (_t1z > _t2z) { float _tmp = _t1z; _t1z = _t2z; _t2z = _tmp; }

	_tnear = (_t1x > _tnear) ? _t1x : _tnear;
	_tnear = (_t1y > _tnear) ? _t1y : _tnear;
	_tnear = (_t1z > _tnear) ? _t1z : _tnear;

	_tfar = (_t2x < _tfar) ? _t2x : _tfar;
	_tfar = (_t2y < _tfar) ? _t2y : _tfar;
	_tfar = (_t2z < _tfar) ? _t2z : _tfar;

	if (_tfar < _tnear) return; // missed

	mi.sign_x = _dx>0 ? 1 : -1;
	mi.sign_y = _dy>0 ? 1 : -1;
	mi.sign_z = _dz>0 ? 1 : -1;

	if (_tnear == _t1x) _n += Vec3f(-1.0f, 0.0f, 0.0f) * mi.sign_x;
	if (_tnear == _t1y) _n += Vec3f(0.0f, -1.0f, 0.0f) * mi.sign_y;
	if (_tnear == _t1z) _n += Vec3f(0.0f, 0.0f, -1.0f) * mi.sign_z;
	_n.Normalize();

	Vec3f min = boundingBox->getMin();
	Vec3f max = Vec3f(boundingBox->getMax().x() + FLT_EPSILON, boundingBox->getMax().y() + FLT_EPSILON, boundingBox->getMax().z() + FLT_EPSILON);
	Vec3f size = max - min;
	float grid_x = size.x() / nx;
	float grid_y = size.y() / ny;
	float grid_z = size.z() / nz;

	mi.tmin = _tnear;
	mi.dt_x = fabs(grid_x / _dx);
	mi.dt_y = fabs(grid_y / _dy);
	mi.dt_z = fabs(grid_z / _dz);
	if (_t1x > -INFINITY) {
		while (_t1x <= _tnear){ _t1x += mi.dt_x; }
		mi.tnext_x = _t1x;
	}
	if (_t1y > -INFINITY) {
		while (_t1y <= _tnear){ _t1y += mi.dt_y; }
		mi.tnext_y = _t1y;
	}
	if (_t1z> -INFINITY) {
		while (_t1z <= _tnear){ _t1z += mi.dt_z; }
		mi.tnext_z = _t1z;
	}
	Vec3f _rp = O + D * _tnear - boundingBox->getMin();
	_rp.Divide(grid_x, grid_y, grid_z);
	mi.i = static_cast<int>(_rp.x()); if (mi.sign_x < 0 && mi.i == nx) mi.i--;
	mi.j = static_cast<int>(_rp.y()); if (mi.sign_y < 0 && mi.j == ny) mi.j--;
	mi.k = static_cast<int>(_rp.z()); if (mi.sign_z < 0 && mi.k == nz) mi.k--;
	mi.normal = _n;
}