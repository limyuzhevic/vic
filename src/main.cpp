// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Clean, organized main.cpp with enhanced experiment framework

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase2Experiment.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <stdexcept>
#include <algorithm>

using namespace nlm;

// Command line argument parsing
class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv) : argc(argc), argv(argv) {}
    
    bool parse() {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg == "--version") {
                versionRequested = true;
            } else if (arg == "--list") {
                listRequested = true;
            } else if (arg.find("--config=") == 0) {
                configFile = arg.substr(9);
            } else if (std::find(experiments.begin(), experiments.end(), arg) != experiments.end()) {
                selectedExperiments.push_back(arg);
            }
        }
        return true;
    }
    
    bool isHelpRequested() const { return helpRequested; }
    bool isVersionRequested() const { return versionRequested; }
    bool isListRequested() const { return listRequested; }
    const std::string& getConfigFile() const { return configFile; }
    const std::vector<std::string>& getSelectedExperiments() const { return selectedExperiments; }
    
    void printHelp() const {
        std::cout << "NLM Experiment Framework - Command Line Interface" << std::endl;
        std::cout << "===============================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [OPTIONS] [EXPERIMENT_NAME...]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl;
        std::cout << "  --list                  List available experiments" << std::endl;
        std::cout << "  --config FILE           Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << std::endl;
        std::cout << "Available experiments:" << std::endl;
        std::cout << "  - connectivity            (Basic neural connectivity test)" << std::endl;
        std::cout << "  - plasticity              (Plasticity learning experiment)" << std::endl;
        std::cout << "  - stdp                    (STDP verification)" << std::endl;
        std::cout << std::endl;
        std::cout << "Example:" << std::endl;
        std::cout << "  nlm --list                           List all experiments" << std::endl;
        std::cout << "  nlm connectivity                     Run connectivity experiment" << std::endl;
    }
    
private:
    int argc;
    char** argv;
    bool helpRequested = false;
    bool versionRequested = false;
    bool listRequested = false;
    std::string configFile = "configs/default.cfg";
    std::vector<std::string> experiments = {"connectivity", "plasticity", "stdp"};
    std::vector<std::string> selectedExperiments;
};

int main(int argc, char** argv) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Parse command line arguments
        CommandLineParser parser(argc, argv);
        if (!parser.parse()) {
            std::cerr << "Error parsing command line arguments!" << std::endl;
            return 1;
        }
        
        // Handle help/version/list flags
        if (parser.isHelpRequested()) {
            parser.printHelp();
            return 0;
        }
        if (parser.isVersionRequested()) {
            std::cout << "NLM Phase 2: Real Neural Computation - Version 2.0" << std::endl;
            return 0;
        }
        if (parser.isListRequested()) {
            std::cout << "Available experiments:" << std::endl;
            std::cout << "  - connectivity            (Basic neural connectivity test)" << std::endl;
            std::cout << "  - plasticity              (Plasticity learning experiment)" << std::endl;
            std::cout << "  - stdp                    (STDP verification)" << std::endl;
            return 0;
        }
        
        // Initialize system
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Log startup information
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Implementing:");
        NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
        NLM_LOG_INFO("  - Event-driven spike propagation with delays");
        NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
        NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("");
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Determine which experiments to run
        std::vector<std::string> experimentsToRun;
        
        if (parser.getSelectedExperiments().empty()) {
            experimentsToRun = {"connectivity", "plasticity", "stdp"};
            NLM_LOG_INFO("Running all experiments...");
        } else {
            experimentsToRun = parser.getSelectedExperiments();
            NLM_LOG_INFO("Running selected experiments: ");
            for (const auto& exp : experimentsToRun) {
                NLM_LOG_INFO("  - " + exp);
            }
        }
        
        // Run each experiment using the new experiment framework
        int successCount = 0;
        int failureCount = 0;
        
        for (const auto& expName : experimentsToRun) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Running Experiment: " + expName + " ===");
            
            try {
                std::shared_ptr<Experiment> experiment;
                
                if (expName == "connectivity") {
                    experiment = std::make_shared<ConnectivityExperiment>();
                } else if (expName == "plasticity") {
                    experiment = std::make_shared<PlasticityExperiment>();
                } else if (expName == "stdp") {
                    experiment = std::make_shared<StdpExperiment>();
                } else {
                    NLM_LOG_ERROR("Unknown experiment: " + expName);
                    failureCount++;
                    continue;
                }
                
                // Create experiment runner and run
                ExperimentRunner runner;
                auto environment = std::make_shared<Environment>("TestEnvironment", 10, 10);
                
                if (runner.runExperiment(experiment, brain, environment, 1000)) {
                    successCount++;
                    NLM_LOG_INFO("✓ Experiment completed successfully");
                } else {
                    failureCount++;
                    NLM_LOG_ERROR("✗ Experiment failed");
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error running experiment " + expName + ": " + e.what());
                failureCount++;
            }
        }
        
        // Final status and summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Experiment Summary ===");
        NLM_LOG_INFO("Successful experiments: " + std::to_string(successCount));
        NLM_LOG_INFO("Failed experiments: " + std::to_string(failureCount));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime.count()) + "s");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        
        return (failureCount > 0) ? 1 : 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}