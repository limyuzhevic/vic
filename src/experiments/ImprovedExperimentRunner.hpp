#pragma once

#include <memory>
#include <string>

namespace nlm {

class Brain;

class ImprovedExperimentRunner {
public:
    static void runLearningExperiment(std::shared_ptr<Brain> brain, bool verbose = false);
    static void runStdpAnalysis(std::shared_ptr<Brain> brain, bool verbose = false);
    static void runAdaptiveExperiment(std::shared_ptr<Brain> brain, const std::string& mode);
    static void runVisualization(std::shared_ptr<Brain> brain, const std::string& outputPath);
};

} // namespace nlm