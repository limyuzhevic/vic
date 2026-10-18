#pragma once

#include "core/CommandLineParser.hpp"
#include "experiments/LearningExperiment.hpp"
#include "tests/TestModules.hpp"
#include <memory>
#include <iostream>
#include <fstream>

namespace nlm {

class ExperimentRunner {
public:
    ExperimentRunner(std::shared_ptr<Config> config);
    
    // Run specific experiment by name
    bool runExperiment(const std::string& experimentName);
    
    // Run all experiments
    bool runAllExperiments();
    
    // Get available experiment names
    static std::vector<std::string> getAvailableExperiments();
    
    // Set brain for experiments
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    bool verbose_;
};

} // namespace nlm