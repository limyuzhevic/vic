// Experiment Tool Implementation
//
// This file implements the exp-info CLI tool for displaying experiment details.

#pragma once

#include "tools/experiment_tool.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

class ExpInfoTool : public CLITool {
public:
    ExpInfoTool() = default;
    ~ExpInfoTool() = default;
    
    // Tool information
    std::string getName() const override {
        return "exp-info";
    }
    
    std::string getDescription() const override {
        return "Show experiment details and metadata";
    }
    
    std::string getUsage() const override {
        return "nlm-cli exp-info <experiment_name> [options]";
    }
    
    CommandType getType() const override {
        return CommandType::EXPERIMENT;
    }
    
    std::string getCategory() const override {
        return "Experiment Management";
    }
    
    // Tool execution
    ExecutionResult execute(const std::vector<std::string>& args, Config& config) override {
        if (args.empty()) {
            printError("Error: Experiment name is required");
            printInfo("Usage: nlm-cli exp-info <experiment_name>");
            return ExecutionResult::VALIDATION_ERROR;
        }
        
        std::string experimentName = args[0];
        
        printInfo("=== Experiment Information ===");
        printInfo("Experiment name: " + experimentName);
        printInfo("Timestamp: " + getCurrentTimestamp());
        
        // Display configuration-based experiment details
        printInfo("\n=== Experiment Configuration ===");
        
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        int64_t regionCount = config.getOr<int64_t>("region_count", 1);
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        int64_t maxSteps = config.getOr<int64_t>("max_simulation_steps", 10000);
        
        printInfo("Neurons: " + std::to_string(neuronCount));
        printInfo("Regions: " + std::to_string(regionCount));
        printInfo("Time step: " + std::to_string(timestep) + "s");
        printInfo("Max simulation steps: " + std::to_string(maxSteps));
        
        // Calculate estimated runtime
        double estimatedTime = maxSteps * timestep / 1.0; // Assuming 1 second per step for estimation
        printInfo("Estimated runtime: ~" + formatTime(estimatedTime) + " seconds");
        
        // Display plasticity parameters
        printInfo("\n=== Plasticity Parameters ===");
        
        float stdpLTP = config.getOr<float>("stdp_ltp_weight", 0.02f);
        float stdpLTD = config.getOr<float>("stdp_ltd_weight", 0.015f);
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        
        printInfo("STDP LTP weight: " + std::to_string(stdpLTP));
        printInfo("STDP LTD weight: " + std::to_string(stdpLTD));
        printInfo("Synaptogenesis rate: " + std::to_string(synaptogenesisRate));
        printInfo("Pruning rate: " + std::to_string(pruningRate));
        
        // Display learning potential
        printInfo("\n=== Learning Potential Analysis ===");
        
        bool enableValidation = config.getOr<bool>("enable_validation", false);
        bool recordNeurons = config.getOr<bool>("record_neurons", false);
        bool recordSynapses = config.getOr<bool>("record_synapses", false);
        bool recordMemories = config.getOr<bool>("record_memories", false);
        
        int learningFeatures = (enableValidation ? 1 : 0) + 
                             (recordNeurons ? 1 : 0) +
                             (recordSynapses ? 1 : 0) +
                             (recordMemories ? 1 : 0);
        
        printInfo("Validation enabled: " + std::string(enableValidation ? "Yes" : "No"));
        printInfo("Record neurons: " + std::string(recordNeurons ? "Yes" : "No"));
        printInfo("Record synapses: " + std::string(recordSynapses ? "Yes" : "No"));
        printInfo("Record memories: " + std::string(recordMemories ? "Yes" : "No"));
        printInfo("Learning features count: " + std::to_string(learningFeatures) + "/4");
        
        if (learningFeatures >= 3) {
            printSuccess("High learning potential experiment configured");
        } else if (learningFeatures >= 2) {
            printInfo("Moderate learning potential experiment configured");
        } else {
            printWarning("Low learning potential experiment - consider increasing recording features");
        }
        
        // Display preset information
        std::string preset = config.getOr<std::string>("preset", "custom");
        if (preset != "custom") {
            printInfo("\nPreset: " + preset);
        }
        
        printSuccess("Experiment information displayed successfully");
        return ExecutionResult::SUCCESS;
    }
    
    // Tool-specific help
    void displayHelp() const override {
        std::cout << "=== exp-info Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: Show experiment details and metadata" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm-cli exp-info <experiment_name> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "This tool displays information about a neural simulation experiment:" << std::endl;
        std::cout << "  - Experiment name and timestamp" << std::endl;
        std::cout << "  - Configuration details (neurons, regions, timestep)" << std::endl;
        std::cout << "  - Plasticity parameters (STDP, synaptogenesis, pruning)" << std::endl;
        std::cout << "  - Learning potential analysis" << std::endl;
        std::cout << "  - Preset configuration information" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-cli exp-info learning_experiment" << std::endl;
        std::cout << "  nlm-cli exp-info my_exp --config configs/experiment.json" << std::endl;
        std::cout << "  nlm-cli exp-info test_experiment --verbose" << std::endl;
        std::cout << std::endl;
        std::cout << "Category: Experiment Management" << std::endl;
    }
    
    // Tool-specific validation
    bool validateArgs(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        return true;
    }
};

} // namespace nlm