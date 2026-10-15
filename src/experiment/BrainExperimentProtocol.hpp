#pragma once

#include "ExperimentProtocol.hpp"
#include "../brain/Brain.hpp"
#include "../core/ExtensibleBrain.hpp"
#include "../neuromodulation/NeuromodulationSystem.hpp"

namespace nlm {

class BrainExperimentProtocol : public ExperimentProtocol {
public:
    virtual ~BrainExperimentProtocol() = default;
    
    // Brain-specific protocol methods
    virtual bool configureBrain(ExtensibleBrain& brain) = 0;
    virtual bool configureNeuromodulation(NeuromodulationSystem& neuromodulation) = 0;
    virtual bool configureLearning(class LearningAlgorithm& learning) = 0;
    virtual bool validateBrainConfiguration(const ExtensibleBrain& brain) const = 0;
    
    // Experiment execution specific to brain simulations
    virtual bool executeBrainSimulation(ExperimentContext& context) = 0;
    virtual bool analyzeBrainDynamics(const ExperimentResults& results) = 0;
    virtual bool generateBrainReport(ExperimentReport& report) = 0;
};

} // namespace nlm