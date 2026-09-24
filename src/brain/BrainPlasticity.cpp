// Brain Plasticity System Implementation - Contains plasticity rules management
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

// Plasticity system management
namespace PlasticityManagement {

    // STDP system management
    void configureSTDPSystem(Brain::Impl* impl);
    void updateSTDP(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
    void manageSTDPWeights(Brain::Impl* impl);
    
    // Hebbian learning management
    void configureHebbianSystem(Brain::Impl* impl);
    void updateHebbianLearning(Brain::Impl* impl, SimulationStep currentStep);
    void manageHebbianWeights(Brain::Impl* impl);
    
    // Structural plasticity management
    void configureStructuralPlasticity(Brain::Impl* impl);
    void updateStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep, std::unique_ptr<RandomGenerator>& rng);
    void manageStructuralChanges(Brain::Impl* impl);
    
    // Cross-plasticity integration
    void integratePlasticityRules(Brain::Impl* impl, SimulationStep currentStep);
    void synchronizePlasticitySystems(Brain::Impl* impl);
    
    // Plasticity statistics
    void updatePlasticityStatistics(Brain::Impl* impl);
    std::string getPlasticityStatisticsReport(const Brain::Impl* impl);
    
    // Plasticity optimization
    void optimizePlasticityRules(Brain::Impl* impl);
    void managePlasticityResources(Brain::Impl* impl);
}

} // namespace nlm