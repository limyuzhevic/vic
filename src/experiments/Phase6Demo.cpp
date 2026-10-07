#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/NeuralAttention.hpp"

using namespace nlm;

void printHelp() {
    printHeader("NLM Phase 6 Demo - Help");
    
    std::cout << "Usage: nlm_phase6_demo [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --neurons=N        Number of neurons (default: 500)\n";
    std::cout << "  --steps=N          Maximum simulation steps (default: 1000)\n";
    std::cout << "  --regions=N        Number of brain regions (default: 1)\n";
    std::cout << "  --conn_prob=VAL    Connection probability (default: 0.15)\n";
    std::cout << "  --dev[elopment]    Enable development system\n";
    std::cout << "  --checkpoint=FILE   Checkpoint file path (enable checkpointing)\n";
    std::cout << "  --config=FILE      Load configuration from file\n";
    std::cout << "  --verbose          Enable verbose output\n";
    std::cout << "  --help             Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  nlm_phase6_demo --neurons=1000 --steps=5000\n";
    std::cout << "  nlm_phase6_demo --dev --checkpoint=./checkpoint.bin\n";
    std::cout << "  nlm_phase6_demo --config=./myconfig.json --verbose\n\n";
    std::cout << "Format options:\n";
    std::cout << "  Config files can be in JSON, YAML, or simple key=value format.\n";
}

void printConfig(const Config& config) {
    std::cout << "Configuration:\n";
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        auto valOpt = config.get<std::string>(key);
        if (valOpt) {
            std::cout << "  " << std::left << std::setw(25) << key << ": " << *valOpt << "\n";
        }
    }
}

bool parseArgs(int argc, char* argv[], Phase6Config& config, bool& verbose) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    if (key == "neurons" || key == "neuron_count") {
                        config.neuronCount = std::stoul(value);
                        // Validate neuron count
                        if (config.neuronCount == 0) {
                            std::cout << "Error: Neuron count must be greater than 0" << std::endl;
                            return false;
                        }
                    } else if (key == "steps" || key == "max_steps") {
                        config.maxSteps = std::stoul(value);
                        // Validate step count
                        if (config.maxSteps == 0) {
                            std::cout << "Error: Step count must be greater than 0" << std::endl;
                            return false;
                        }
                    } else if (key == "regions" || key == "region_count") {
                        config.regionCount = std::stoul(value);
                        // Validate region count
                        if (config.regionCount == 0) {
                            std::cout << "Error: Region count must be greater than 0" << std::endl;
                            return false;
                        }
                    } else if (key == "conn_prob" || key == "connection_probability") {
                        config.connectionProbability = std::stof(value);
                        // Validate connection probability
                        if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
                            std::cout << "Error: Connection probability must be between 0.0 and 1.0" << std::endl;
                            return false;
                        }
                    } else if (key == "dev" || key == "development") {
                        config.enableDevelopment = true;
                    } else if (key == "checkpoint" || key == "checkpoint_path") {
                        config.checkpointPath = value;
                        config.enableCheckpointing = !value.empty();
                    } else if (key == "verbose" || key == "verbose") {
                        verbose = true;
                    }
                } catch (const std::exception& e) {
                    std::cout << "Error parsing argument " << arg << ": " << e.what() << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}

bool loadConfigFile(const std::string& filepath, Config& config) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    if (config.loadFromFile(filepath)) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
        NLM_LOG_INFO("Falling back to default configuration...");
        return false;
    }
}

void generateDefaultConfig(const std::string& filepath) {
    NLM_LOG_INFO("Generating default configuration file: " + filepath);
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000u, ConfigSource::Default);
    config->set("region_count", 1u, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("reward_discount_factor", 0.99f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001f, ConfigSource::Default);
    config->set("max_simulation_steps", 10000u, ConfigSource::Default);
    config->set("log_level", std::string("INFO"), ConfigSource::Default);
    config->set("log_to_file", false, ConfigSource::Default);
    config->set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    
    if (config->saveToFile(filepath)) {
        NLM_LOG_INFO("Default configuration saved to: " + filepath);
        std::cout << "Default configuration generated: " << filepath << std::endl;
    } else {
        NLM_LOG_ERROR("Failed to save default configuration to: " + filepath);
        std::cout << "Error: Failed to generate default configuration" << std::endl;
    }
}
    
    // Create and run Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    printSection("Phase 6 Integration Experiment");
    auto result = experiment.run(config);
    
    printSection("Integration Results");
    printResult("Total Reward", result.totalReward, 1000.0f, "steps");
    printResult("Average Firing Rate", result.avgFiringRate, 100.0f, "Hz");
    printResult("Episodes Stored", result.memoryEpisodesStored, 100.0f, "episodes");
    printResult("Novelty Level", result.noveltyLevel, 1.0f);
    printResult("Curiosity Level", result.curiosityLevel, 1.0f);
    printResult("Dopamine Level", result.dopamineLevel, 1.0f);
    printResult("Wall Clock Time", result.totalWallClockTime, 300.0f, "s");
    
    printSection("Integration Verification");
    printResult("Working Memory", result.memoryWorkingMemoryIntegrated ? 1.0f : 0.0f);
    printResult("Episodic Memory", result.memoryEpisodicMemoryIntegrated ? 1.0f : 0.0f);
    printResult("Neuromodulation", result.neuromodulationIntegrated ? 1.0f : 0.0f);
    printResult("Prediction System", result.predictionIntegrated ? 1.0f : 0.0f);
    printResult("Development System", result.developmentIntegrated ? 1.0f : 0.0f);
    printResult("Checkpointing", result.checkpointingWorks ? 1.0f : 0.0f);
    
    printSection("Overall Assessment");
    bool success = result.memoryWorkingMemoryIntegrated && 
                   result.memoryEpisodicMemoryIntegrated &&
                   result.neuromodulationIntegrated &&
                   result.predictionIntegrated &&
                   result.developmentIntegrated;
    
    if (success) {
        std::cout << "✓ Phase 6 INTEGRATION SUCCESSFUL\n";
        std::cout << "All brain systems are properly connected and functional.\n";
        std::cout << "The NLM artificial brain is now a coherent, integrated system.\n";
    } else {
        std::cout << "✗ Phase 6 INTEGRATION INCOMPLETE\n";
        std::cout << "Some systems are not properly integrated.\n";
    }
    
    printSection("Additional Verification Tests");
    
    // Run additional verification tests
    bool verificationSuccess = true;
    
    verificationSuccess &= experiment.verifyIntegration();
    verificationSuccess &= experiment.testMemoryIntegration();
    verificationSuccess &= experiment.testNeuromodulationIntegration();
    verificationSuccess &= experiment.testCheckpointing();
    verificationSuccess &= experiment.testReplay();
    
    if (verificationSuccess) {
        std::cout << "✓ All verification tests PASSED\n";
    } else {
        std::cout << "✗ Some verification tests FAILED\n";
        success = false;
    }
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "Phase 6 Demo Complete\n";
    std::cout << "Press Enter to exit...\n";
    
    std::string dummy;
    std::getline(std::cin, dummy);
    
    return success ? 0 : 1;
}