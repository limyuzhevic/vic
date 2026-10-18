#include "ExperimentRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

ExperimentRunner::ExperimentRunner(std::shared_ptr<Config> config)
    : config_(config), verbose_(false) {
}

bool ExperimentRunner::runExperiment(const std::string& experimentName) {
    if (!brain_) {
        NLM_LOG_ERROR("No brain available for experiment: " + experimentName);
        return false;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Experiment: " + experimentName + " ===");
    NLM_LOG_INFO("");
    
    try {
        if (experimentName == "basic") {
            runBasicConnectivityTest(brain_);
            return true;
        } else if (experimentName == "plasticity") {
            runPlasticityExperiment(brain_);
            return true;
        } else if (experimentName == "stdp") {
            runStdpVerification(brain_);
            return true;
        } else {
            NLM_LOG_ERROR("Unknown experiment: " + experimentName);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Experiment " + experimentName + " failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::runAllExperiments() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running All Experiments ===");
    NLM_LOG_INFO("");
    
    bool allSuccess = true;
    
    // Run basic connectivity test
    if (!runExperiment("basic")) {
        allSuccess = false;
        NLM_LOG_WARN("Basic connectivity test failed");
    }
    
    // Reset brain between experiments
    brain_->reset();
    brain_->initialize();
    
    // Run plasticity experiment
    if (!runExperiment("plasticity")) {
        allSuccess = false;
        NLM_LOG_WARN("Plasticity experiment failed");
    }
    
    // Reset brain between experiments
    brain_->reset();
    brain_->initialize();
    
    // Run STDP verification
    if (!runExperiment("stdp")) {
        allSuccess = false;
        NLM_LOG_WARN("STDP verification failed");
    }
    
    return allSuccess;
}

std::vector<std::string> ExperimentRunner::getAvailableExperiments() {
    return {"basic", "plasticity", "stdp", "all"};
}

} // namespace nlm