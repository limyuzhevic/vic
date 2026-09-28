#include "ExperimentFramework.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

std::vector<std::unique_ptr<Experiment>> ExperimentFactory::getAllExperiments() {
    static std::vector<std::unique_ptr<Experiment>> experiments = registerExperiments();
    return experiments;
}

std::unique_ptr<Experiment> ExperimentFactory::createExperiment(const std::string& name) {
    auto experiments = getAllExperiments();
    for (auto& exp : experiments) {
        if (exp->getName() == name) {
            return std::move(exp);
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<Experiment>> ExperimentFactory::registerExperiments() {
    std::vector<std::unique_ptr<Experiment>> experiments;
    
    // Register built-in experiments here
    // examples:
    // experiments.push_back(std::make_unique<BasicConnectivityExperiment>());
    // experiments.push_back(std::make_unique<PlasticityExperiment>());
    // experiments.push_back(std::make_unique<STDPExperiment>());
    
    return experiments;
}

std::string ExperimentUtils::formatDuration(double seconds) {
    std::stringstream ss;
    if (seconds < 1.0) {
        ss << std::fixed << std::setprecision(3) << seconds * 1000.0 << " ms";
    } else if (seconds < 60.0) {
        ss << std::fixed << std::setprecision(3) << seconds << " s";
    } else {
        int minutes = static_cast<int>(seconds / 60.0);
        double remainingSeconds = seconds - minutes * 60.0;
        ss << minutes << "." << std::fixed << std::setprecision(3) << remainingSeconds << " min";
    }
    return ss.str();
}

std::shared_ptr<Config> ExperimentUtils::createDefaultConfig(const std::string& experimentName) {
    auto config = std::make_shared<Config>();
    
    // Set default values based on experiment type
    if (experimentName == "basic_connectivity") {
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
    } else if (experimentName == "plasticity") {
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 123);
        config->set("plasticity_learning_rate", 0.01f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
    } else if (experimentName == "stdp") {
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 456);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
    }
    else {
        // General defaults
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
    }
    
    return config;
}

std::vector<ExperimentResult> ExperimentUtils::runExperiments(
    const std::vector<std::unique_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Config> config) {
    
    std::vector<ExperimentResult> results;
    
    for (const auto& experiment : experiments) {
        try {
            experiment->setup(config);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            ExperimentResult result(experiment->getName(), experiment->getDescription());
            
            // Run the experiment
            result = experiment->run(brain);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            result.executionTime = duration;
            
            experiment->cleanup();
            
            results.push_back(std::move(result));
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
            ExperimentResult result(experiment->getName(), experiment->getDescription());
            result.success = false;
            result.output = "Exception: " + std::string(e.what());
            results.push_back(std::move(result));
        }
    }
    
    return results;
}

bool ExperimentUtils::saveResults(const std::string& filepath, const std::vector<ExperimentResult>& results) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for saving results: " + filepath);
            return false;
        }
        
        // Write results in a simple text format
        file << "NLM Experiment Results" << std::endl;
        file << "=======================" << std::endl;
        file << "Generated: " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << "Experiment: " << result.name << std::endl;
            file << "Description: " << result.description << std::endl;
            file << "Status: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
            file << "Execution Time: " << formatDuration(result.executionTime) << std::endl;
            file << "Output:" << std::endl;
            file << result.output << std::endl;
            file << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save results: " + std::string(e.what()));
        return false;
    }
}

std::vector<ExperimentResult> ExperimentUtils::loadResults(const std::string& filepath) {
    std::vector<ExperimentResult> results;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for loading results: " + filepath);
            return results;
        }
        
        // Simple parsing - in practice, you'd want a more robust format
        std::string line;
        while (std::getline(file, line)) {
            // Basic parsing logic
            if (line.find("Experiment: ") == 0) {
                // Create a new result (simplified for example)
                ExperimentResult result("unknown", "unknown");
                results.push_back(result);
            }
        }
        
        file.close();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load results: " + std::string(e.what()));
    }
    
    return results;
}

} // namespace nlm
