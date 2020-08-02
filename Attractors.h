#pragma once
#include <random>
#include <math.h>
#include <glm.hpp>
#include <string>

namespace Attr {

	enum AttrType { LORENZ, THOMAS, AIZAWA, DADRAS, CHEN, HALVORSEN, RABINOVICH, THREESCROLL, CUSTOM, _LAST_ENTRY };

	class Attractor {
	private:
		glm::vec3 point;
		float step;
		AttrType state;
		glm::vec3(Attractor::*funcP) (glm::vec3);

		glm::vec3 computeLorenzAttr(glm::vec3 point);
		glm::vec3 computeThomasAttr(glm::vec3 point);
		glm::vec3 computeAizawaAttr(glm::vec3 point);
		glm::vec3 computeDadrasAttr(glm::vec3 point);
		glm::vec3 computeChenAttr(glm::vec3 point);
		glm::vec3 computeHalvorsenAttr(glm::vec3 point);
		glm::vec3 computeRabinovichAttr(glm::vec3 point);
		glm::vec3 computeThreeScrollAttr(glm::vec3 point);

	public:
		Attractor(AttrType type, float step = 0.1);
		glm::vec3 compute();
		glm::vec3 compute(glm::vec3 point);

		void setType(AttrType type);

		inline void setStepsize(float size) { step = size; };
		inline glm::vec3 getPoint() { return point; };
		inline void setPoint(glm::vec3 point) { this->point = point; };


	};

}





