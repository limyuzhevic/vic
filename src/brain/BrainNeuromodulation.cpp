// Brain Neuromodulation System Implementation - Contains neuromodulation system management
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

// Neuromodulation system management
namespace NeuromodulationManagement {

    // Neuromodulator initialization
    void initializeNeuromodulators(Brain::Impl* impl);
    void updateNeuromodulatorLevels(Brain::Impl* impl, const TimestepDuration& timestep);
    void applyNeuromodulatorEffects(Brain::Impl* impl, SimulationStep currentStep);
    
    // Dopamine system management
    void updateDopamineSystem(Brain::Impl* impl, const TimestepDuration& timestep);
    void applyDopamineEffects(Brain::Impl* impl);
    void manageDopaminePlasticity(Brain::Impl* impl);
    
    // Curiosity system management
    void updateCuriositySystem(Brain::Impl* impl, const TimestepDuration& timestep);
    void applyCuriosityEffects(Brain::Impl* impl);
    
    // Novelty detection
    void updateNoveltyDetection(Brain::Impl* impl, const TimestepDuration& timestep);
    
    // Prediction error system
    void updatePredictionErrorSystem(Brain::Impl* impl);
    void applyPredictionErrorEffects(Brain::Impl* impl);
    
    // Cross-neuromodulation integration
    void integrateNeuromodulators(Brain::Impl* impl);
    void synchronizeNeuromodulation(Brain::Impl* impl);
    
    // Neuromodulation statistics
    void updateNeuromodulationStatistics(Brain::Impl* impl);
    std::string getNeuromodulationStatisticsReport(const Brain::Impl* impl);
    
    // Neuromodulator-based plasticity
    void applyNeuromodulatorPlasticity(Brain::Impl* impl, SimulationStep currentStep);
    void manageNeuromodulatorEligibilityTraces(Brain::Impl* impl);
}

} // namespace nlm