#include "cli/ExperimentCommand.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

using json = nlohmann::json;

using namespace nlm;
namespace cli {

ExperimentCommand::ExperimentCommand() 
    : CommandBase("experiment", "Run a specific experiment (Phase 3, 4, 5, 6)"),
      experiment_name_("phase3"),
      phase_(3),
      param_file_(""),
      parallel_(false) {
}

bool ExperimentCommand::execute(const std::map<std::string, std::string>& args) {
    std::cout << "Running experiment: " << experiment_name_ << std::endl;
    std::cout << "Phase: " << phase_ << std::endl;
    
    if (!param_file_.empty()) {
        std::cout << "Parameter file: " << param_file_ << std::endl;
        loadParameters(param_file_);
    }
    
    if (parallel_) {
        std::cout << "Parallel execution enabled" << std::endl;
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain for experiment!" << std::endl;
        return false;
    }
    
    // Run experiment based on phase
    bool success = false;
    switch (phase_) {
        case 3:
            success = runPhase3Experiment(brain, param_file_);
            break;
        case 4:
            success = runPhase4Experiment(brain, param_file_);
            break;
        case 5:
            success = runPhase5Experiment(brain, param_file_);
            break;
        case 6:
            success = runPhase6Experiment(brain, param_file_);
            break;
        default:
            std::cerr << "Error: Invalid experiment phase: " << phase_ << std::endl;
            return false;
    }
    
    if (success) {
        std::cout << "Experiment completed successfully!" << std::endl;
        saveExperimentResults(brain, experiment_name_);
    }
    
    return success;
}

void ExperimentCommand::printHelp() const {
    std::cout << "Usage: nlm-cli experiment <experiment_name> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run a specific experiment phase." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  <experiment_name>   Experiment name (phase3, phase4, phase5, phase6)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -p, --phase <n>      Experiment phase (3, 4, 5, or 6)" << std::endl;
    std::cout << "  -f, --params <file>  Parameter file (JSON/YAML) for custom experiment" << std::endl;
    std::cout << "  -P, --parallel       Enable parallel execution" << std::endl;
    std::cout << "  -q, --quiet          Suppress console output" << std::endl;
    std::cout << "  -v, --verbose        Enable verbose logging" << std::endl;
}

void ExperimentCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

void ExperimentCommand::loadParameters(const std::string& param_file) {
    try {
        std::ifstream file(param_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open parameter file: " << param_file << std::endl;
            return;
        }
        
        json params;
        file >> params;
        file.close();
        
        // Process parameters
        if (params.contains("phase")) {
            phase_ = params["phase"].get<int>();
        }
        if (params.contains("parallel")) {
            parallel_ = params["parallel"].get<bool>();
        }
        if (params.contains("neuron_count")) {
            // Apply to configuration
        }
        
        std::cout << "Loaded parameters from: " << param_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading parameters: " << e.what() << std::endl;
    }
}

bool ExperimentCommand::runPhase3Experiment(std::shared_ptr<Brain> brain, const std::string& param_file) {
    std::cout << "Running Phase 3 Experiment: Basic Neural Connectivity" << std::endl;
    
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Phase 3 completed successfully" << std::endl;
    return true;
}

bool ExperimentCommand::runPhase4Experiment(std::shared_ptr<Brain> brain, const std::string& param_file) {
    std::cout << "Running Phase 4 Experiment: Sensorimotor Integration" << std::endl;
    
    for (int step = 0; step < 2000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Phase 4 completed successfully" << std::endl;
    return true;
}

bool ExperimentCommand::runPhase5Experiment(std::shared_ptr<Brain> brain, const std::string& param_file) {
    std::cout << "Running Phase 5 Experiment: Social Learning" << std::endl;
    
    for (int step = 0; step < 3000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Phase 5 completed successfully" << std::endl;
    return true;
}

bool ExperimentCommand::runPhase6Experiment(std::shared_ptr<Brain> brain, const std::string& param_file) {
    std::cout << "Running Phase 6 Experiment: Self-Awareness" << std::endl;
    
    for (int step = 0; step < 5000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Phase 6 completed successfully" << std::endl;
    return true;
}

void ExperimentCommand::saveExperimentResults(std::shared_ptr<Brain> brain, const std::string& experiment_name) {
    json results;
    
    results["timestamp"] = getTimestamp();
    results["experiment_name"] = experiment_name;
    results["total_neurons"] = brain->getTotalNeuronCount();
    results["total_synapses"] = brain->getTotalSynapseCount();
    results["total_spikes"] = brain->getTotalSpikeCount();
    
    // Save to file
    std::string output_file = "results/experiment_" + experiment_name + "_" + getTimestamp() + ".json";
    std::ofstream file(output_file);
    if (file.is_open()) {
        file << results.dump(2);
        file.close();
        std::cout << "Experiment results saved to: " << output_file << std::endl;
    }
}

} // namespace cli
} // namespace nlm
