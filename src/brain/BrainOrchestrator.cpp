#include "BrainOrchestrator.hpp"
#include "Brain.hpp"
#include "Config.hpp"
#include "SimulationClock.hpp"
#include "SystemInitializer.hpp"
#include <memory>

namespace nlm {

// Implementation struct
class BrainOrchestrator::Impl {
public:
    std::shared_ptr<Config> config;
    std::shared_ptr<SimulationClock> clock;
    std::shared_ptr<Brain> brain;
    
    Impl(std::shared_ptr<Config> cfg, std::shared_ptr<Brain> b)
        : config(cfg), brain(b) {
        clock = std::make_shared<SimulationClock>(0.001);
    }
};

// Constructor
BrainOrchestrator::BrainOrchestrator(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(config, brain)) {
}

BrainOrchestrator::~BrainOrchestrator() = default;

// Core simulation methods
bool BrainOrchestrator::initialize() {
    return SystemInitializer::initializeAllSystems(pImpl->brain);
}

void BrainOrchestrator::step(SimulationStep step, Timestamp time) {
    pImpl->brain->step(step, time);
}

void BrainOrchestrator::receiveSensoryInput(const SensoryInput& input) {
    pImpl->brain->receiveSensoryInput(input);
}

std::unique_ptr<Action> BrainOrchestrator::produceAction() {
    return pImpl->brain->produceAction();
}

// Development and neuromodulation
void BrainOrchestrator::updateDevelopment(double timestep) {
    pImpl->brain->develop();
}

void BrainOrchestrator::applyNeuromodulation(const Neuromodulator& signal) {
    pImpl->brain->applyNeuromodulation(signal);
}

void BrainOrchestrator::reset() {
    pImpl->brain->reset();
}

// Accessors
std::shared_ptr<Brain> BrainOrchestrator::getBrain() const {
    return pImpl->brain;
}

float BrainOrchestrator::getExcitationInhibitionRatio() const {
    return pImpl->brain->getExcitationInhibitionRatio();
}

float BrainOrchestrator::getAverageFiringRate() const {
    return pImpl->brain->getAverageFiringRate();
}

} // namespace nlm

