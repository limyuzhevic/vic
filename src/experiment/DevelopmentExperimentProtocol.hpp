#pragma once

#include "ExperimentProtocol.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

class DevelopmentExperimentProtocol : public ExperimentProtocol {
public:
    virtual ~DevelopmentExperimentProtocol() = default;
    
    // Development-specific protocol methods
    virtual bool configureDevelopmentSystem(DevelopmentSystem& development) = 0;
    virtual bool configureEvolution(class EvolutionaryAlgorithm& evolution) = 0;
    virtual bool validateDevelopmentConfiguration(const DevelopmentSystem& development) const = 0;
    
    // Development-specific experiment execution
    virtual bool executeDevelopmentExperiment(ExperimentContext& context) = 0;
    virtual bool analyzeDevelopmentTrajectories(const ExperimentResults& results) = 0;
    virtual bool generateDevelopmentReport(ExperimentReport& report) = 0;
};

} // namespace nlm