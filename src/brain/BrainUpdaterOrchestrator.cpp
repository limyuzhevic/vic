#include "BrainUpdaterOrchestrator.hpp"
#include <iostream>

namespace nlm {

BrainUpdaterOrchestrator::BrainUpdaterOrchestrator() {
    // Add updaters in execution order
    addUpdater(std::make_unique<NeuronSystemUpdater>());
    addUpdater(std::make_unique<MemorySystemUpdater>());
    addUpdater(std::make_unique<NeuromodulationUpdater>());
}

BrainUpdaterOrchestrator::~BrainUpdaterOrchestrator() = default;

void BrainUpdaterOrchestrator::addUpdater(std::unique_ptr<BrainSystemUpdater> updater) {
    if (updater) {
        updaters_.push_back(std::move(updater));
    }
}

void BrainUpdaterOrchestrator::update(Brain& brain, SimulationStep currentStep,
                                      Timestamp currentTime, TimestepDuration timestep) {
    // Execute updaters in registration order
    for (auto& updater : updaters_) {
        try {
            if (updater->shouldUpdate(brain, currentStep)) {
                updater->update(brain, currentStep, currentTime, timestep);
            }
        } catch (const std::exception& e) {
            // Log error but continue with other updaters
            std::cerr << "Error in updater " << updater->getName() 
                     << ": " << e.what() << std::endl;
        }
    }
}

std::vector<std::string> BrainUpdaterOrchestrator::getUpdaterNames() const {
    std::vector<std::string> names;
    for (const auto& updater : updaters_) {
        names.push_back(updater->getName());
    }
    return names;
}

void BrainUpdaterOrchestrator::clearUpdaters() {
    updaters_.clear();
}

} // namespace nlm
