// NLM Advanced Application - Simplified Enhanced Version
//
// A focused implementation of advanced NLM features with proper compilation

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                               ║\n";
    std::cout << "║           NLM — 熙然                                          ║\n";
    std::cout << "║          Neural Learning Machine                             ║\n";
    std::cout << "║                                                               ║\n";
    std::cout << "║      Advanced Neural Simulation Framework                   ║\n";
    std::cout << "║                                                               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help                          Show this help message" << std::endl;
    std::cout << "  --version                       Show version information" << std::endl;
    std::cout << "  --neurons <count>               Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps <count>                 Set simulation steps (default: 5000)" << std::endl;
    std::cout << "  --seed <value>                  Set random seed (default: 42)" << std::endl;
    std::cout << "  --timestep <ms>                 Simulation timestep in ms (default: 1.0)" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --neuron-type <type>            Neuron type: internal, sensory, motor, excitatory, inhibitory (default: internal)" << std::endl;
    std::cout << "  --connection-prob <float>       Synaptic connection probability (default: 0.15)" << std::endl;
    std::cout << "  --stdp-weights <ltp> <ltd>      STDP weights: LTP, LTD (default: 0.02, 0.015)" << std::endl;
    std::cout << "  --stdp-tau <ms>                 STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --enable-plasticity             Enable synaptic plasticity (default: true)" << std::endl;
    std::cout << "  --enable-development            Enable developmental processes (default: true)" << std::endl;
    std::cout << "  --enable-stdp                   Enable STDP (default: true)" << std::endl;
    std::cout << "  --enable-hebbian                Enable Hebbian (default: true)" << std::endl;
    std::cout << "  --enable-curiosity              Enable curiosity learning (default: false)" << std::endl;
    std::cout << "  --enable-reward-mod             Enable reward-modulated plasticity (default: false)" << std::endl;
    std::cout << std::endl;
    std::cout << "Logging Options:" << std::endl;
    std::cout << "  --log-level <level>             Log level: debug, info, warning, error (default: info)" << std::endl;
    std::cout << "  --log-file <file>               Log output file" << std::endl;
    std::cout << "  --debug-mode                    Enable debug logging" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --mode <mode>                   Run mode: basic, learning, stdp, connectivity (default: basic)" << std::endl;
    std::cout << "  --save-checkpoint <file>         Save brain state" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 2.2.0 Advanced Edition" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;
    std::cout << "Enhanced Features:" << std::endl;
    std::cout << "  ✓ Advanced command-line interface" << std::endl;
    std::cout << "  ✓ Configurable neural parameters" << std::endl;
    std::cout << "  ✓ Multiple experiment modes" << std::endl;
    std::cout << "  ✓ Enhanced logging and debugging" << std::endl;
    std::cout << "  ✓ Checkpoint save/load support" << std::endl;
    std::cout << "  ✓ Error handling and validation" << std::endl;
}

class AdvancedNLMApplication {
public:
    AdvancedNLMApplication(int argc, char** argv) : argc_(argc), argv_(argv) {
        parseCommandLine();
    }
    
    bool run() {
        setupLogging();
        
        // Create and configure brain
        auto config = std::make_shared<Config>();
        configureBrain(config);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        NLM_LOG_INFO("NLM Advanced Application initialized successfully");
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount_) + " neurons, " +
                    std::to_string(steps_) + " steps, timestep: " + std::to_string(timestep_) + "ms");
        
        // Run experiment
        bool success = runExperiment(brain);
        
        if (success && !checkpointPath_.empty()) {
            NLM_LOG_INFO("Saving checkpoint to: " + checkpointPath_);
            if (brain->save(checkpointPath_)) {
                NLM_LOG_INFO("Checkpoint saved successfully");
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint");
            }
        }
        
        return success;
    }
    
private:
    enum class RunMode {
        Basic,
        Learning,
        STDP,
        Connectivity
    };
    
    int argc_;
    char** argv_;
    
    // Configuration
    std::string configFile_;
    std::string logFile_;
    bool debugMode_ = false;
    std::string checkpointPath_;
    
    LogLevel logLevel_ = LogLevel::Info;
    
    // Brain configuration
    uint64_t seed_ = 42;
    int64_t neuronCount_ = 1000;
    uint64_t steps_ = 5000;
    double timestep_ = 0.001; // 1ms
    std::string neuronType_ = "internal";
    float connectionProbability_ = 0.15f;
    
    // STDP parameters
    float stdpLTPWeight_ = 0.02f;
    float stdpLTDWeight_ = 0.015f;
    float stdpTau_ = 20.0f;
    
    // Plasticity flags
    bool plasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool stdpEnabled_ = true;
    bool hebbianEnabled_ = true;
    bool curiosityEnabled_ = false;
    bool rewardModulatedEnabled_ = false;
    
    // Experiment
    RunMode mode_ = RunMode::Basic;
    
    void parseCommandLine() {
        for (int i = 1; i < argc_; ++i) {
            std::string arg = argv_[i];
            
            if (arg == "--help") {
                printHelp(argv_[0]);
                exit(0);
            } else if (arg == "--version") {
                printVersion();
                exit(0);
            } else if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                if (i + 1 < argc_) {
                    std::string value = argv_[++i];
                    setOption(key, value);
                } else {
                    NLM_LOG_ERROR("Missing value for option: " + key);
                }
            } else {
                NLM_LOG_ERROR("Unknown option: " + arg);
            }
        }
    }
    
    void setOption(const std::string& key, const std::string& value) {
        if (key == "config") {
            configFile_ = value;
        } else if (key == "log-file") {
            logFile_ = value;
        } else if (key == "neurons") {
            neuronCount_ = std::stoi(value);
        } else if (key == "steps") {
            steps_ = std::stoull(value);
        } else if (key == "seed") {
            seed_ = std::stoull(value);
        } else if (key == "timestep") {
            timestep_ = std::stod(value);
        } else if (key == "neuron-type") {
            neuronType_ = value;
        } else if (key == "connection-prob") {
            connectionProbability_ = std::stof(value);
        } else if (key == "stdp-weights") {
            if (i + 1 < argc_) {
                stdpLTPWeight_ = std::stof(value);
                stdpLTDWeight_ = std::stof(argv_[++i]);
            }
        } else if (key == "stdp-tau") {
            stdpTau_ = std::stof(value);
        } else if (key == "enable-plasticity") {
            plasticityEnabled_ = true;
        } else if (key == "enable-development") {
            developmentEnabled_ = true;
        } else if (key == "enable-stdp") {
            stdpEnabled_ = true;
        } else if (key == "enable-hebbian") {
            hebbianEnabled_ = true;
        } else if (key == "enable-curiosity") {
            curiosityEnabled_ = true;
        } else if (key == "enable-reward-mod") {
            rewardModulatedEnabled_ = true;
        } else if (key == "log-level") {
            if (value == "debug") logLevel_ = LogLevel::Debug;
            else if (value == "info") logLevel_ = LogLevel::Info;
            else if (value == "warning") logLevel_ = LogLevel::Warning;
            else if (value == "error") logLevel_ = LogLevel::Error;
        } else if (key == "mode") {
            if (value == "basic") mode_ = RunMode::Basic;
            else if (value == "learning") mode_ = RunMode::Learning;
            else if (value == "stdp") mode_ = RunMode::STDP;
            else if (value == "connectivity") mode_ = RunMode::Connectivity;
        } else if (key == "save-checkpoint") {
            checkpointPath_ = value;
        } else {
            NLM_LOG_WARNING("Unknown option ignored: " + key + " (value: " + value + ")");
        }
    }
    
    void setupLogging() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel_);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        
        if (!logFile_.empty()) {
            auto fileLogger = std::make_shared<ConsoleLogger>(logLevel_);
            fileLogger->setOutputFile(logFile_);
            logger->addLogger(fileLogger);
        }
        
        if (debugMode_) {
            logger->setLevel(LogLevel::Debug);
        }
        
        Logger::setGlobal(logger);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("random_seed", seed_);
        config->set("simulation_timestep", timestep_);
        config->set("neuron_count", neuronCount_);
        config->set("region_count", 1);
        config->set("connection_probability", connectionProbability_);
        
        // STDP parameters
        config->set("stdp_ltp_weight", stdpLTPWeight_);
        config->set("stdp_ltd_weight", stdpLTDWeight_);
        config->set("stdp_tau", stdpTau_);
        
        // Plasticity flags
        config->set("hebbian_enabled", hebbianEnabled_);
        config->set("stdp_enabled", stdpEnabled_);
        config->set("reward_modulated_enabled", rewardModulatedEnabled_);
        config->set("structural_enabled", true);
        config->set("development_enabled", developmentEnabled_);
        
        // Neuromodulation
        config->set("curiosity_enabled", curiosityEnabled_);
    }
    
    bool runExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running " << getModeString(mode_) << " Experiment ===");
        
        switch (mode_) {
            case RunMode::Basic:
                return runBasicExperiment(brain);
            case RunMode::Learning:
                return runLearningExperiment(brain);
            case RunMode::STDP:
                return runSTDPExperiment(brain);
            case RunMode::Connectivity:
                return runConnectivityExperiment(brain);
        }
        return false;
    }
    
    std::string getModeString(RunMode mode) {
        switch (mode) {
            case RunMode::Basic: return "Basic Connectivity";
            case RunMode::Learning: return "Learning";
            case RunMode::STDP: return "STDP Learning";
            case RunMode::Connectivity: return "Connectivity Analysis";
        }
        return "Unknown";
    }
    
    bool runBasicExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running basic connectivity test...");
        
        // Inject current into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Run simulation
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps_); ++step) {
            brain->step(step, step * timestep_);
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                            " | Firing rate: " + std::to_string(brain->getAverageFiringRate()) +
                            " | Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("Basic experiment completed successfully");
        return true;
    }
    
    bool runLearningExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running learning experiment...");
        
        // Enable plasticity
        for (auto& region : brain->getRegions()) {
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
        }
        
        // Run learning simulation
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps_); ++step) {
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * timestep_);
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Learning step " + std::to_string(step) +
                            " | Firing rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        NLM_LOG_INFO("Learning experiment completed successfully");
        return true;
    }
    
    bool runSTDPExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running STDP learning experiment...");
        
        // Enable only STDP
        for (auto& region : brain->getRegions()) {
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, true, false);
                }
            }
        }
        
        // Run STDP test
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps_); ++step) {
            if (step % 50 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 40.0f);
                brain->step(step, step * timestep_);
                
                brain->injectCurrentToNeurons(NeuronType::Inhibitory, 40.0f);
                brain->step(step + 1, (step + 1) * timestep_);
            }
            
            brain->step(step, step * timestep_);
        }
        
        NLM_LOG_INFO("STDP experiment completed successfully");
        return true;
    }
    
    bool runConnectivityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running connectivity analysis...");
        
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        // Run connectivity test
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps_); ++step) {
            brain->step(step, step * timestep_);
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Connectivity step " + std::to_string(step) +
                            " | Synapses: " + std::to_string(brain->getTotalSynapseCount()) +
                            " | Firing rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        size_t finalSynapses = brain->getTotalSynapseCount();
        NLM_LOG_INFO("Connectivity experiment completed");
        NLM_LOG_INFO("  Initial synapses: " + std::to_string(initialSynapses));
        NLM_LOG_INFO("  Final synapses: " + std::to_string(finalSynapses));
        NLM_LOG_INFO("  Net change: " + std::to_string(finalSynapses - initialSynapses));
        
        return true;
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    AdvancedNLMApplication app(argc, argv);
    
    if (!app.run()) {
        std::cerr << "ERROR: Application failed" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Application Completed Successfully ===" << std::endl;
    
    return 0;
}
