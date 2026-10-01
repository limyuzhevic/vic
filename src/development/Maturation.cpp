#include "Maturation.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Maturation::Impl {
    class Brain* brain;
    float baselinePlasticity;
    float developmentRate;
    std::vector<float> ageFactors;
    
    Impl() : brain(nullptr), baselinePlasticity(1.0f), developmentRate(0.001f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

void Maturation::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Maturation system initialized");
}

void Maturation::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Age-dependent plasticity modulation
    // Plasticity decreases as brain matures (from 1.0 to 0.1)
    pImpl->baselinePlasticity = std::max(0.1f, 1.0f - pImpl->developmentRate * dt);
    
    // Update developmental factors
    if (pImpl->ageFactors.empty()) {
        pImpl->ageFactors.reserve(100);
        for (int i = 0; i < 100; ++i) {
            pImpl->ageFactors.push_back(1.0f - i * 0.01f); // Linear decay
        }
    }
    
    NLM_LOG_DEBUG("Maturation update: baseline plasticity = " + 
                  std::to_string(pImpl->baselinePlasticity));
}

float Maturation::getBaselinePlasticity() const {
    return pImpl->baselinePlasticity;
}

void Maturation::setDevelopmentRate(float rate) {
    pImpl->developmentRate = std::max(0.0f, std::min(rate, 0.01f));
}

float Maturation::getDevelopmentRate() const {
    return pImpl->developmentRate;
}

void Maturation::reset() {
    pImpl->baselinePlasticity = 1.0f;
    pImpl->ageFactors.clear();
}

} // namespace nlm
