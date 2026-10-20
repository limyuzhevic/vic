// Brain development system implementation - development system integration
#include "Brain.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

// Brain development system implementation - development system integration

// Development system accessors
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

} // namespace nlm