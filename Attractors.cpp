#include "Attractors.h"

using namespace Attr;

	glm::vec3 Attractor::computeLorenzAttr(glm::vec3 point) {   //Computes the Differntial Equation
		float a = 10, b = 28, c = 8 / 3.;
		float x = point.x + step * (a * (point.y - point.x));
		float y = point.y + step * (point.x * (b - point.z) - point.y);
		float z = point.z + step * (point.x * point.y - c * point.z);
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeThomasAttr(glm::vec3 point) {
		float b = 0.208186f;
		float x = point.x + step * (sin(point.y) - b * point.x);
		float y = point.y + step * (sin(point.z) - b * point.y);
		float z = point.z + step * (sin(point.x) - b * point.z);
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeAizawaAttr(glm::vec3 point) {
		float a = 0.95, b = 0.7, c = 0.6, d = 3.5, e = 0.25, f = 0.1;
		float x = point.x + step * ((point.z - b)*point.x - d * point.y);
		float y = point.y + step * (d*point.x + (point.z - b)*point.y);
		float z = point.z + step * (c + a * point.z - (pow(point.z, 3) / 3.) - (pow(point.x, 2) + pow(point.y, 2))*(1 + e * point.z) + (f*point.z*pow(point.x, 3)));
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeDadrasAttr(glm::vec3 point) {
		float a = 3, b = 2.7, c = 1.7, d = 2, e = 9;
		float x = point.x + step * (point.y - a * point.x + b * point.y*point.z);
		float y = point.y + step * (c*point.y - point.x * point.z + point.z);
		float z = point.z + step * (d*point.x*point.y - e * point.z);
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeChenAttr(glm::vec3 point) {
		float a = 5, b = -10, c = -0.38;
		float x = point.x + step * (a * point.x - point.y * point.z);
		float y = point.y + step * (b * point.y + point.x * point.z);
		float z = point.z + step * (c* point.z + (point.x*point.y) / 3.);
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeHalvorsenAttr(glm::vec3 point) {
		float a = 1.89;
		float x = point.x + step * (-a * point.x - 4 * point.y - 4 * point.z - pow(point.y, 2));
		float y = point.y + step * (-a * point.y - 4 * point.z - 4 * point.x - pow(point.z, 2));
		float z = point.z + step * (-a * point.z - 4 * point.x - 4 * point.y - pow(point.x, 2));
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeRabinovichAttr(glm::vec3 point) {
		float a = 0.14, b = 0.10;
		float x = point.x + step * (point.y*(point.z - 1 + pow(point.x, 2)) + b * point.x);
		float y = point.y + step * (point.x*(3 * point.z + 1 - pow(point.x, 2)) + b * point.y);
		float z = point.z + step * (-2 * point.z * (a + point.x*point.y));
		return glm::vec3(x, y, z);
	}

	glm::vec3 Attractor::computeThreeScrollAttr(glm::vec3 point) {
		float a = 32.48, b = 45.84, c = 1.18, d = 0.13, e = 0.57, f = 14.7;
		float x = point.x + step * (a*(point.y - point.x) + d * point.x*point.y);
		float y = point.y + step * (b * point.x - point.x*point.z + f * point.y);
		float z = point.z + step * (c*point.z + point.x*point.y - e * pow(point.x, 2));
		return glm::vec3(x, y, z);
	}

	void Attractor::setType(AttrType type)
	{
		state = type;
		switch (type) {  //maybe use lookup table instead...
		case LORENZ:
			point = glm::vec3(1.1, 1.0, 1.0);
			funcP = &Attractor::computeLorenzAttr;
			break;
		case THOMAS:
			point = glm::vec3(1.1, 1.1, -0.01);
			funcP = &Attractor::computeThomasAttr;
			break;
		case AIZAWA:
			point = glm::vec3(0.1, 1.0, 0.01);
			funcP = &Attractor::computeAizawaAttr;
			break;
		case DADRAS:
			point = glm::vec3(1.1, 2.1, -2.0);
			funcP = &Attractor::computeDadrasAttr;
			break;
		case CHEN:
			point = glm::vec3(5.0, 10.0, 10.0);
			funcP = &Attractor::computeChenAttr;
			break;
		case HALVORSEN:
			point = glm::vec3(-1.48, -1.51, 2.04);
			funcP = &Attractor::computeHalvorsenAttr;
			break;
		case RABINOVICH:
			point = glm::vec3(-1.0, 0.0, 0.5);
			funcP = &Attractor::computeRabinovichAttr;
			break;
		case THREESCROLL:
			point = glm::vec3(-0.29, -0.25, -0.59);
			funcP = &Attractor::computeThreeScrollAttr;
			break;
		default: //Lorenz
			point = glm::vec3(1.0, 1.0, 1.0);
			funcP = &Attractor::computeLorenzAttr;
			break;
		}
	}

	Attractor::Attractor(AttrType type, float step)
	{
		this->step = step;
		setType(type);

	}
	glm::vec3 Attractor::compute()
	{
		point = (this->*funcP)(point);
		return point;
	}

	glm::vec3 Attractor::compute(glm::vec3 point)
	{
		return (this->*funcP)(point);
	}

