#include "NLMExperimentRunner.hpp"
#include "src/experiments/ExperimentFramework.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {

bool NLMExperimentRunner::run(int argc, char** argv) {
    try {
        // Parse command line arguments
        CommandArgs args = parseCommandLine(argc, argv);
        
        if (args.help) {
            printHelp();
            return true;
        }
        
        // Load configuration
        auto config = loadConfig(args.configFile);
        
        // Initialize brain
        std::cout << "Initializing NLM brain...\n";
        auto brain = initializeBrain(config);
        
        std::cout << "Brain initialized with " 
                  << brain->getTotalNeuronCount() << " neurons and "
                  << brain->getTotalSynapseCount() << " synapses\n";
        
        std::vector<ExperimentResult> allResults;
        
        if (args.runAll) {
            // Run all available experiments
            std::cout << "Running all available experiments...\n";
            auto experiments = ExperimentFactory::getAllExperiments();
            allResults = ExperimentUtils::runExperiments(experiments, brain, config);
        } else if (!args.experimentName.empty()) {
            // Run specific experiment
            std::cout << "Running experiment: " << args.experimentName << "...\n";
            auto experiment = ExperimentFactory::createExperiment(args.experimentName);
            if (!experiment) {
                std::cerr << "Error: Experiment not found: " << args.experimentName << "\n";
                return false;
            }
            allResults.push_back(runSingleExperiment(experiment, brain, config));
        } else {
            std::cerr << "Error: No experiment specified. Use --help for options.\n";
            return false;
        }
        
        // Display results
        displayResults(allResults);
        
        // Save results if requested
        if (!args.outputFile.empty()) {
            std::cout << "Saving results to: " << args.outputFile << "\n";
            if (!saveResults(args.outputFile, allResults)) {
                std::cerr << "Warning: Failed to save results.\n";
            }
        }
        
        // Print summary
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "EXPERIMENT RUN SUMMARY\n";
        std::cout << std::string(70, '=') << "\n";
        
        int successCount = 0;
        double totalTime = 0.0;
        for (const auto& result : allResults) {
            totalTime += result.executionTime;
            if (result.success) successCount++;
        }
        
        std::cout << "Experiments run: " << allResults.size() << "\n";
        std::cout << "Successful: " << successCount << "\n";
        std::cout << "Failed: " << (allResults.size() - successCount) << "\n";
        std::cout << "Total execution time: " << ExperimentUtils::formatDuration(totalTime) << "\n";
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error running experiments: " << e.what() << "\n";
        return false;
    }
}

void NLMExperimentRunner::printHelp() const {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "NLM EXPERIMENT RUNNER - HELP\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Usage:\n";
    std::cout << "  nlm_experiment [OPTIONS]\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --list                    List all available experiments\n";
    std::cout << "  --all                     Run all experiments\n";
    std::cout << "  --experiment NAME         Run specific experiment by name\n";
    std::cout << "  --config FILE             Load configuration from file\n";
    std::cout << "  --output FILE             Save results to file\n";
    std::cout << "  --verbose                 Enable verbose output\n";
    std::cout << "  --trials N                Number of trials (default: 5)\n";
    std::cout << "  --steps N                 Steps per episode (default: 200)\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  nlm_experiment --help\n";
    std::cout << "  nlm_experiment --list\n";
    std::cout << "  nlm_experiment --all --output results.txt\n";
    std::cout << "  nlm_experiment --experiment phase3_demo --config my_config.cfg\n";
    std::cout << std::endl;
}

void NLMExperimentRunner::printExperimentList() const {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "AVAILABLE EXPERIMENTS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    auto experiments = ExperimentFactory::getAllExperiments();
    
    if (experiments.empty()) {
        std::cout << "No experiments available.\n";
        return;
    }
    
    int count = 1;
    for (const auto& exp : experiments) {
        std::cout << count++ << ". " << exp->getName() << "\n";
        std::cout << "   " << exp->getDescription() << "\n\n";
    }
}

std::string NLMExperimentRunner::getExperimentSummary(const ExperimentResult& result) const {
    std::stringstream ss;
    ss << "Experiment: " << result.name << "\n";
    ss << "Description: " << result.description << "\n";
    ss << "Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
    ss << "Execution Time: " << ExperimentUtils::formatDuration(result.executionTime) << "\n";
    
    if (!result.output.empty()) {
        ss << "Output:\n" << result.output << "\n";
    }
    
    return ss.str();
}

NLMExperimentRunner::CommandArgs NLMExperimentRunner::parseCommandLine(int argc, char** argv) {
    CommandArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--list") {
            // List experiments and exit
            printExperimentList();
            exit(0);
        } else if (arg == "--all") {
            args.runAll = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg.find("--experiment=") == 0) {
            args.experimentName = arg.substr(13);
        } else if (arg == "--experiment" && i + 1 < argc) {
            args.experimentName = argv[++i];
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg.find("--trials=") == 0) {
            args.numTrials = std::stoi(arg.substr(9));
        } else if (arg == "--trials" && i + 1 < argc) {
            args.numTrials = std::stoi(argv[++i]);
        } else if (arg.find("--steps=") == 0) {
            args.stepsPerEpisode = std::stoi(arg.substr(8));
        } else if (arg == "--steps" && i + 1 < argc) {
            args.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg[0] != '-') {
            // Try to determine experiment from filename or name
            args.experimentName = arg;
        }
    }
    
    return args;
}

std::shared_ptr<Config> NLMExperimentRunner::loadConfig(const std::string& configFile) const {
    auto config = std::make_shared<Config>();
    
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << "\n";
        if (!config->loadFromFile(configFile)) {
            std::cout << "Warning: Failed to load configuration file. Using defaults.\n";
        }
    } else {
        std::cout << "Using default configuration.\n";
    }
    
    return config;
}

std::shared_ptr<Brain> NLMExperimentRunner::initializeBrain(std::shared_ptr<Config> config) const {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    return brain;
}

ExperimentResult NLMExperimentRunner::runSingleExperiment(
    std::shared_ptr<Experiment> experiment,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Config> config) const {
    
    std::cout << "Running experiment: " << experiment->getName() << "...\n";
    
    // Setup experiment
    experiment->setup(config);
    
    // Run experiment
    auto startTime = std::chrono::high_resolution_clock::now();
    ExperimentResult result(experiment->getName(), experiment->getDescription());
    
    try {
        result = experiment->run(brain);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        result.executionTime = duration;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        result.executionTime = duration;
        result.success = false;
        result.output = "Exception: " + std::string(e.what());
        NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
    }
    
    // Cleanup
    experiment->cleanup();
    
    return result;
}

void NLMExperimentRunner::displayResults(const std::vector<ExperimentResult>& results) const {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "EXPERIMENT RESULTS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        std::cout << "Experiment " << (i + 1) << "/" << results.size() << ":\n";
        std::cout << "  Name: " << result.name << "\n";
        std::cout << "  Description: " << result.description << "\n";
        std::cout << "  Status: " << (result.success ? "✓ SUCCESS" : "✗ FAILED") << "\n";
        std::cout << "  Execution Time: " << ExperimentUtils::formatDuration(result.executionTime) << "\n";
        
        if (!result.output.empty()) {
            std::cout << "  Output:\n";
            std::cout << result.output << "\n";
        }
        
        std::cout << "\n";
    }
}

bool NLMExperimentRunner::saveResults(const std::string& filepath, 
                                     const std::vector<ExperimentResult>& results) const {
    try {
        return ExperimentUtils::saveResults(filepath, results);
    } catch (const std::exception& e) {
        std::cerr << "Error saving results: " << e.what() << "\n";
        return false;
    }
}

} // namespace nlm
