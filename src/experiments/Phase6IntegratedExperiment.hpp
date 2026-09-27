#pragma once

#include "ExperimentEnhanced.hpp"

namespace nlm {

// Phase 6 Integrated Experiment - Comprehensive integration test
class Phase6IntegratedExperiment : public Experiment {
public:
    Phase6IntegratedExperiment(const std::string& name = "Phase6_Integrated");
    bool run();
};

} // namespace nlm

