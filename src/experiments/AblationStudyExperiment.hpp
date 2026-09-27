#pragma once

#include "ExperimentEnhanced.hpp"

namespace nlm {

// Ablation Study Experiment
class AblationStudyExperiment : public Experiment {
public:
    AblationStudyExperiment(const std::string& name = "Ablation_Study");
    bool run();
};

} // namespace nlm
