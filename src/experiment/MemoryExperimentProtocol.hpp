#pragma once

#include "ExperimentProtocol.hpp"
#include "../memory/MemorySystem.hpp"

namespace nlm {

class MemoryExperimentProtocol : public ExperimentProtocol {
public:
    virtual ~MemoryExperimentProtocol() = default;
    
    // Memory-specific protocol methods
    virtual bool configureMemorySystem(MemorySystem& memory) = 0;
    virtual bool configurePlasticity(class PlasticityAlgorithm& plasticity) = 0;
    virtual bool validateMemoryConfiguration(const MemorySystem& memory) const = 0;
    
    // Memory-specific experiment execution
    virtual bool executeMemoryExperiment(ExperimentContext& context) = 0;
    virtual bool analyzeMemoryTrajectories(const ExperimentResults& results) = 0;
    virtual bool generateMemoryReport(ExperimentReport& report) = 0;
};

} // namespace nlm