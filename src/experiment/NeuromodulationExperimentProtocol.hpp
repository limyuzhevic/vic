#pragma once

#include "ExperimentProtocol.hpp"
#include "../neuromodulation/NeuromodulationSystem.hpp"

namespace nlm {

class NeuromodulationExperimentProtocol : public ExperimentProtocol {
public:
    virtual ~NeuromodulationExperimentProtocol() = default;
    
    // Neuromodulation-specific protocol methods
    virtual bool configureNeuromodulation(NeuromodulationSystem& neuromodulation) = 0;
    virtual bool configureLearning(class LearningAlgorithm& learning) = 0;
    virtual bool validateNeuromodulationConfig(const NeuromodulationSystem& neuromodulation) const = 0;
    
    // Neuromodulation-specific experiment execution
    virtual bool executeNeuromodulationExperiment(ExperimentContext& context) = 0;
    virtual bool analyzeModulationEffects(const ExperimentResults& results) = 0;
    virtual bool generateModulationReport(ExperimentReport& report) = 0;
};

} // namespace nlm