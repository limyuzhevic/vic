// Brain.cpp - Main brain implementation
// This file now only contains the core initialization and main step methods
// All other functionality is implemented in BrainCore.cpp, BrainIntegration.cpp, and BrainMemory.cpp

#include "Brain.hpp"
#include <memory>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    // Cleanup is handled in BrainMemory.cpp
    delete pImpl;
    pImpl = nullptr;
}

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    // Initialize core brain components (neurons, regions, basic connectivity)
    return initializeCoreBrain();
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     */
    
    // Update all systems using the integration module
    updateAllSystems(currentStep, currentTime);
}

} // namespace nlm
