#include "ExperimentRunner.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    std::string resultsFile;
    bool enableVerboseLogging;
    
    Impl() : globalSeed(42), resultsFile(""), enableVerboseLogging(false) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment, brain, or environment parameters");
        return false;
    }
    
    try {
        experiment->setStartStep(0);
        
        // Reset environment and brain
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        // Run simulation loop with progress tracking
        for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
            // Brain step
            brain->step(step);
            
            // Record data
            experiment->recordStep(
                step,
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getFiringNeuronCount(),
                brain->getAverageFiringRate()
            );
            
            // Get action from brain
            auto action = brain->produceAction();
            
            // Environment step
            observation = environment->step(*action);
            
            // Send observation to brain
            brain->receiveSensoryInput(*observation);
            
            // Record reward
            float reward = environment->getLastReward();
            experiment->recordMetric("total_reward", 
                experiment->getMetric("total_reward") + reward);
            
            // Log progress for long runs
            if (pImpl->enableVerboseLogging && step % 1000 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                            " | Synapses: " + std::to_string(brain->getTotalSynapseCount()) +
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Reward: " + std::to_string(reward));
            }
        }
        
        experiment->setEndStep(maxSteps);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Experiment execution failed: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Experiment execution failed with unknown exception");
        return false;
    }
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    // Report batch start
    NLM_LOG_INFO("=== Starting Batch Experiment Run ===");
    NLM_LOG_INFO("  Number of experiments: " + std::to_string(experiments.size()));
    NLM_LOG_INFO("  Max steps per experiment: " + std::to_string(maxSteps));
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& exp = experiments[i];
        
        NLM_LOG_INFO("\n--- Running experiment " + std::to_string(i + 1) + "/" + 
                    std::to_string(experiments.size()) + ": " + exp->getName() + " ---");
        
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
            NLM_LOG_INFO("✓ Experiment completed successfully");
        } else {
            NLM_LOG_ERROR("✗ Experiment failed");
        }
        
        // Brief pause between experiments for stability
        if (i < experiments.size() - 1) {
            NLM_LOG_INFO("Waiting before next experiment...");
        }
    }
    
    NLM_LOG_INFO("\n=== Batch Run Complete ===");
    NLM_LOG_INFO("  Successful experiments: " + std::to_string(results.size()));
    NLM_LOG_INFO("  Failed experiments: " + std::to_string(experiments.size() - results.size()));
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    NLM_LOG_INFO("Clearing all experiments");
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
    NLM_LOG_INFO("Global seed set to: " + std::to_string(seed));
}

void ExperimentRunner::setResultsFile(const std::string& filepath) {
    pImpl->resultsFile = filepath;
    NLM_LOG_INFO("Results will be saved to: " + filepath);
}

void ExperimentRunner::enableVerboseLogging(bool enable) {
    pImpl->enableVerboseLogging = enable;
    if (enable) {
        NLM_LOG_INFO("Verbose logging enabled");
    } else {
        NLM_LOG_INFO("Verbose logging disabled");
    }
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    if (pImpl->experiments.empty()) {
        NLM_LOG_WARNING("No experiments to save");
        return false;
    }
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open results file for writing: " + filepath);
            return false;
        }
        
        file << "=== NLM Experiment Results ===" << std::endl;
        file << "Generated: " << getCurrentTimestamp() << std::endl;
        file << "Total Experiments: " + std::to_string(pImpl->experiments.size()) << std::endl;
        file << "" << std::endl;
        
        for (const auto& exp : pImpl->experiments) {
            file << "--- Experiment: " + exp->getName() + " ---" << std::endl;
            file << "  Seed: " + std::to_string(exp->getSeed()) << std::endl;
            file << "  Start Step: " + std::to_string(exp->getStartStep()) << std::endl;
            file << "  End Step: " + std::to_string(exp->getEndStep()) << std::endl;
            file << "  Total Reward: " + std::to_string(exp->getMetric("total_reward")) << std::endl;
            
            // Get and save all metrics
            const auto& metrics = exp->getMetrics();
            for (const auto& metric : metrics) {
                if (metric.first != "total_reward") {  // Already printed
                    file << "  " + metric.first + ": " + std::to_string(metric.second) << std::endl;
                }
            }
            file << "" << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Results saved successfully to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save results: " + std::string(e.what()));
        return false;
    }
}

std::string ExperimentRunner::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

uint64_t ExperimentRunner::getExperimentCount() const {
    return pImpl->experiments.size();
}

std::vector<std::string> ExperimentRunner::getExperimentNames() const {
    std::vector<std::string> names;
    for (const auto& exp : pImpl->experiments) {
        names.push_back(exp->getName());
    }
    return names;
}

} // namespace nlm
