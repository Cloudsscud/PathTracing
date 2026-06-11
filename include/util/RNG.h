#pragma once

#include <random>

class RNG {
private:
	mutable std::mt19937 m_gen;
	mutable std::uniform_real_distribution<float> m_uniform{ 0, 1 };
public:
	RNG() : RNG(0) {};
	RNG(size_t seed) {setSeed(seed);};
	void setSeed(size_t seed) { m_gen.seed(seed); };
	float uniform() const { return m_uniform(m_gen); };	// 0-1
};