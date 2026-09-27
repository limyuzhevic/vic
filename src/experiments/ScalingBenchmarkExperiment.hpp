#pragma once

#include "ExperimentEnhanced.hpp"

namespace nlm {

// Scaling Benchmark Experiment
class ScalingBenchmarkExperiment : public Experiment {
public:
    ScalingBenchmarkExperiment(const std::string& name = "Scaling_Benchmark");
    bool run();
};

// Ablation Study Experiment
class AblationStudyExperiment : public Experiment {
public:
    AblationStudyExperiment(const std::string& name = "Ablation_Study");
    bool run();
};

} // namespace nlm
