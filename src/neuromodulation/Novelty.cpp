// Minimal implementation of Novelty class
#include "Novelty.hpp"
#include <vector>

namespace nlm {

class Novelty::Impl {
public:
    float level;
    std::vector<float> history;
    
    Impl() : level(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl()) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* /* brain */) {}

float Novelty::getLevel() const { return pImpl->level; }

void Novelty::setLevel(float level) { pImpl->level = level; }

void Novelty::detectNovelty(const class Observation& /* observation */, 
                           const class Observation& /* previousObservation */) {}

void Novelty::detectNovelty(const std::vector<float>& /* currentPattern */,
                           const std::vector<float>& /* previousPattern */) {}

void Novelty::update(TimestepDuration /* dt */) {}

const std::vector<float>& Novelty::getHistory() const { return pImpl->history; }

void Novelty::clearHistory() { pImpl->history.clear(); }

} // namespace nlm