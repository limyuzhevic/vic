#include "BrainPlasticity.hpp"

namespace nlm {

void BrainPlasticity::applyPlasticityRules(Timestamp currentTime) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainPlasticity::applyStructuralPlasticity(SimulationStep currentStep) {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

} // namespace nlm