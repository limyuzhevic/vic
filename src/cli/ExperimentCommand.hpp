#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

namespace nlm {

class ExperimentCommand : public SimpleCLICommand {
public:
    ExperimentCommand() : SimpleCLICommand(
        "experiment",
        "Run neural simulation experiments",
        "nlm experiment run <experiment_name> [--steps <steps>] [--params <params>] [--output <output>] [--verbose]",
        "Run neural simulation experiments with configurable parameters.\n\n"
        "Options:\n"
        "  --steps <steps>      Number of simulation steps (default: 1000)\n"
        "  --params <params>    Parameters file (JSON format)\n"
        "  --output <output>    Output file for results\n"
        "  --verbose            Verbose output\n"
        "  --parallel           Run experiments in parallel\n"
        "  --continue           Continue from previous experiment\n\n"
        "Available experiments:\n"
        "  - phase3            Phase 3: Neural network exploration\n"
        "  - phase4            Phase 4: Advanced neuroplasticity\n"
        "  - phase5            Phase 5: Cognitive development\n"
        "  - phase6            Phase 6: Integrated learning\n\n"
        "Examples:\n"
        "  nlm experiment run phase3 --steps 5000\n"
        "  nlm experiment run phase4 --params experiment_params.json\n"
        "  nlm experiment run phase5 --output results.json --verbose"
    ) {
        options = {
            {"steps", "", "Number of simulation steps", OptionType::OptionalValue, false, "1000"},
            {"params", "", "Parameters file (JSON format)", OptionType::OptionalValue},
            {"output", "", "Output file for results", OptionType::OptionalValue},
            {"verbose", "", "Verbose output", OptionType::Flag},
            {"parallel", "", "Run experiments in parallel", OptionType::Flag},
            {"continue", "", "Continue from previous experiment", OptionType::Flag}
        };
        
        parameters = {
            {"experiment_name", "Name of experiment to run", true, "string", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string experimentName = getRequiredParameter(args, 0);
        
        // Get options
        std::string stepsStr = getOptionValue(args, "steps");
        std::string paramsFile = getOptionValue(args, "params");
        std::string outputFile = getOptionValue(args, "output");
        bool verbose = hasOption(args, "verbose");
        bool parallel = hasOption(args, "parallel");
        bool continueExperiment = hasOption(args, "continue");
        
        // Parse parameters
        int steps = 1000;
        if (!stepsStr.empty()) {
            try {
                steps = std::stoi(stepsStr);
            } catch (...) {
                errorHandler.addError("Invalid steps value: " + stepsStr);
                return errorHandler.exitWithError();
            }
        }
        
        // Load experiment parameters
        auto params = loadExperimentParams(paramsFile);
        
        // Get experiment runner
        auto& experimentRunner = ExperimentRunner::getGlobalExperimentRunner();
        
        // Create brain
        auto config = getConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            errorHandler.addError("Failed to initialize brain");
            return errorHandler.exitWithError();
        }
        
        // Get experiment
        auto experiment = experimentRunner.getExperiment(experimentName);
        if (!experiment) {
            errorHandler.addError("Experiment not found: " + experimentName);
            return errorHandler.exitWithError();
        }
        
        CLIFormatters::printSuccess("Running experiment: " + experimentName);
        
        // Create environment
        auto environment = std::make_shared<Environment>();
        environment->initialize(config);
        
        // Run experiment
        if (verbose) {
            CLIFormatters::printInfo("Experiment parameters:");
            for (const auto& param : params) {
                CLIFormatters::printInfo("  " + param.first + ": " + param.second);
            }
        }
        
        bool success = experimentRunner.runExperiment(experiment, brain, environment, steps);
        
        if (!success) {
            errorHandler.addError("Experiment failed");
            return errorHandler.exitWithError();
        }
        
        // Save results if requested
        if (!outputFile.empty()) {
            saveExperimentResults(brain, experiment, outputFile);
        }
        
        // Display experiment results
        displayExperimentResults(brain, experiment, verbose);
        
        CLIFormatters::printSuccess("Experiment completed successfully");
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        // Check if experiment name is provided
        std::string experimentName = args[0];
        
        // Check if experiment exists
        auto& experimentRunner = ExperimentRunner::getGlobalExperimentRunner();
        auto experiment = experimentRunner.getExperiment(experimentName);
        if (!experiment) {
            return false;
        }
        
        return true;
    }
    
private:
    std::map<std::string, std::string> loadExperimentParams(const std::string& paramsFile) {
        std::map<std::string, std::string> params;
        
        if (paramsFile.empty()) {
            return params;
        }
        
        std::ifstream file(paramsFile, std::ios::in);
        if (!file.is_open()) {
            CLIFormatters::printWarning("Cannot open parameters file: " + paramsFile);
            return params;
        }
        
        // Parse JSON parameters (simplified)
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Simple JSON parsing (for demonstration)
        // This should be replaced with a proper JSON parser
        size_t pos = content.find('"');
        while (pos != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = content.find('"', keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find colon after key
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            std::string valueStr;
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            } else {
                // Number or boolean value
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart, valueEnd - valueStart);
            }
            
            params[key] = valueStr;
            
            pos = content.find('"', keyEnd + 1);
        }
        
        return params;
    }
    
    void saveExperimentResults(std::shared_ptr<Brain> brain, std::shared_ptr<Experiment> experiment,
                               const std::string& filepath) {
        std::ofstream file(filepath, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filepath);
        }
        
        file << "Experiment Results" << std::endl;
        file << "==================" << std::endl << std::endl;
        
        file << "Experiment: " << experiment->getName() << std::endl;
        file << "Brain Statistics:" << std::endl;
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        
        file.close();
        CLIFormatters::printInfo("Experiment results saved to: " + filepath);
    }
    
    void displayExperimentResults(std::shared_ptr<Brain> brain, std::shared_ptr<Experiment> experiment,
                                   bool verbose) {
        std::cout << "Experiment Results:" << std::endl;
        std::cout << "==================" << std::endl << std::endl;
        
        std::cout << "Experiment: " << experiment->getName() << std::endl;
        std::cout << "Description: " << experiment->getDescription() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        
        if (verbose) {
            std::cout << "\nDetailed Metrics:" << std::endl;
            // Display additional metrics
            std::cout << "  Average Firing Rate: " << calculateFiringRate(brain) << "%" << std::endl;
            std::cout << "  Learning Progress: " << calculateLearningProgress(brain) << std::endl;
            std::cout << "  Network Connectivity: " << calculateConnectivity(brain) << std::endl;
        }
    }
    
    double calculateFiringRate(std::shared_ptr<Brain> brain) {
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        if (totalNeurons == 0) return 0.0;
        return (static_cast<double>(firingNeurons) / totalNeurons) * 100.0;
    }
    
    double calculateLearningProgress(std::shared_ptr<Brain> brain) {
        // Calculate learning progress based on synaptic changes
        // This is a simplified implementation
        return 50.0; // Placeholder
    }
    
    double calculateConnectivity(std::shared_ptr<Brain> brain) {
        // Calculate network connectivity
        // This is a simplified implementation
        size_t totalSynapses = brain->getTotalSynapseCount();
        if (totalSynapses == 0) return 0.0;
        return 75.0; // Placeholder
    }
    
    // Helper methods
    std::shared_ptr<Config> getConfig() {
        // Get the global configuration
        // This should be implemented based on how configuration is managed
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
};

} // namespace nlm
