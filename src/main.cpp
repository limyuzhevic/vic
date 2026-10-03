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
#include <thread>
#include <fstream>
#include <sstream>

using namespace nlm;

// Power user advanced features
struct PerformanceProfiler {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentStepTime;
    std::vector<double> stepTimes;
    std::vector<uint64_t> spikeCounts;
    uint64_t totalSteps;
    uint64_t maxSpikesPerStep;
    
    PerformanceProfiler() : startTime(std::chrono::steady_clock::now()), currentStepTime(startTime), 
                           totalSteps(0), maxSpikesPerStep(0) {}
    
    void stepStarted() {
        currentStepTime = std::chrono::steady_clock::now();
        totalSteps++;
    }
    
    void stepCompleted(uint64_t spikeCount) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - currentStepTime).count();
        stepTimes.push_back(duration);
        spikeCounts.push_back(spikeCount);
        maxSpikesPerStep = std::max(maxSpikesPerStep, spikeCount);
    }
    
    void printSummary() const {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Performance Profile ===");
        NLM_LOG_INFO("Total steps executed: " + std::to_string(totalSteps));
        if (totalSteps > 0) {
            double avgStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
            double minStepTime = *std::min_element(stepTimes.begin(), stepTimes.end());
            double maxStepTime = *std::max_element(stepTimes.begin(), stepTimes.end());
            
            NLM_LOG_INFO("Average step time: " + std::to_string(avgStepTime * 1000) + "ms");
            NLM_LOG_INFO("Min step time: " + std::to_string(minStepTime * 1000) + "ms");
            NLM_LOG_INFO("Max step time: " + std::to_string(maxStepTime * 1000) + "ms");
            NLM_LOG_INFO("Steps per second: " + std::to_string(1.0 / avgStepTime));
            NLM_LOG_INFO("Max spikes per step: " + std::to_string(maxSpikesPerStep));
        }
        NLM_LOG_INFO("==========================");
    }
};

struct DebugMonitor {
    bool spikeTracingEnabled;
    bool synapseTracingEnabled;
    bool memoryMonitoringEnabled;
    std::vector<NeuronId> spikeHistory;
    std::vector<std::pair<NeuronId, NeuronId>> synapseEvents;
    std::vector<size_t> memorySnapshots;
    
    DebugMonitor() : spikeTracingEnabled(false), synapseTracingEnabled(false), 
                    memoryMonitoringEnabled(false) {}
    
    void enableSpikeTracing(bool enable) { spikeTracingEnabled = enable; }
    void enableSynapseTracing(bool enable) { synapseTracingEnabled = enable; }
    void enableMemoryMonitoring(bool enable) { memoryMonitoringEnabled = enable; }
    
    void recordSpike(NeuronId neuronId) {
        if (spikeTracingEnabled) {
            spikeHistory.push_back(neuronId);
        }
    }
    
    void recordSynapseEvent(NeuronId pre, NeuronId post) {
        if (synapseTracingEnabled) {
            synapseEvents.emplace_back(pre, post);
        }
    }
    
    void takeMemorySnapshot() {
        if (memoryMonitoringEnabled) {
            static size_t snapshotCounter = 0;
            memorySnapshots.push_back(++snapshotCounter);
        }
    }
    
    void printDebugInfo() const {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Debug Information ===");
        
        if (spikeTracingEnabled && !spikeHistory.empty()) {
            NLM_LOG_INFO("Spike history (last " + std::to_string(std::min(size_t(10), spikeHistory.size())) + "):");
            for (size_t i = std::max(size_t(0), spikeHistory.size() - 10); i < spikeHistory.size(); ++i) {
                NLM_LOG_INFO("  Step " + std::to_string(i) + ": Neuron " + std::to_string(spikeHistory[i]));
            }
        }
        
        if (synapseTracingEnabled && !synapseEvents.empty()) {
            NLM_LOG_INFO("Synapse events (last " + std::to_string(std::min(size_t(5), synapseEvents.size())) + "):");
            for (size_t i = std::max(size_t(0), synapseEvents.size() - 5); i < synapseEvents.size(); ++i) {
                NLM_LOG_INFO("  Event " + std::to_string(i) + ": Synapse " + std::to_string(synapseEvents[i].first) + 
                           " -> " + std::to_string(synapseEvents[i].second));
            }
        }
        
        if (memoryMonitoringEnabled && !memorySnapshots.empty()) {
            NLM_LOG_INFO("Memory snapshots taken: " + std::to_string(memorySnapshots.size()));
            if (memorySnapshots.size() > 1) {
                NLM_LOG_INFO("Memory growth: " + std::to_string(memorySnapshots.back() - memorySnapshots.front()) + " snapshots");
            }
        }
        
        NLM_LOG_INFO("=======================");
    }
};

void printAdvancedBanner(nullptr) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: ADVANCED Real Neural Computation                   ║
    ║                                                               ║
    ║     Power User Edition - Enhanced Features                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(nullptr) {
    std::cout << R"(
Usage: nlm [OPTIONS]

Basic Options:
  --help                    Show this help message
  --version                 Show version information
  --config FILE             Load configuration from file

Advanced Power User Options:
  --experiment TYPE         Run specific experiment (plasticity, stdp, debug, performance)
  --steps N                 Number of simulation steps to run
  --timestep T              Simulation timestep in seconds
  --neurons N               Number of neurons in network
  --regions R               Number of neural regions
  --debug ENABLE            Enable debug features (spike,synapse,memory,all)
  --profile ENABLE          Enable performance profiling
  --save-config FILE        Save configuration to file
  --import-config FILE      Import configuration from file
  --metrics FILE            Save metrics to file
  --checkpoint FILE         Save checkpoint at regular intervals
  --resume FILE             Resume from checkpoint
  --random-seed SEED         Set random seed for reproducibility
  --verbose                 Enable verbose logging
  --quiet                   Suppress non-essential output

Experiment Types:
  plasticity               Plasticity learning experiment
  stdp                     Spike-timing dependent plasticity verification
  debug                    Debug mode with detailed tracing
  performance              Performance benchmarking
  all                      Run all experiments

Examples:
  nlm --experiment performance --steps 1000 --timestep 0.001
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring
  nlm --experiment plasticity --neurons 1000 --regions 3 --verbose
  nlm --experiment stdp --random-seed 12345
  nlm --checkpoint brain_state.chk --steps 5000
  nlm --resume brain_state.chk
  nlm --debug all --profile
  nlm --experiment all --save-metrics results.csv
  nlm --config custom.cfg --timestep 0.0005 --verbose
  nlm --import-config settings.cfg --random-seed 999
  nlm --save-config production.cfg
  nlm --quiet --experiment performance
  nlm --experiment stdp --checkpoint stdp_checkpoint.chk
  nlm --experiment debug --spike-tracing --synapse-tracing
  nlm --experiment plasticity --metrics plasticity_metrics.csv
  nlm --debug all --profile --experiment all
  nlm --checkpoint test.chk --resume test.chk
  nlm --experiment performance --steps 2000 --timestep 0.0002
  nlm --experiment debug --memory-monitoring --verbose
  nlm --experiment plasticity --neurons 2000 --regions 8
  nlm --experiment stdp --random-seed 12345 --checkpoint stdp.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring
  nlm --experiment plasticity --metrics plasticity_metrics.csv
  nlm --debug all --profile --experiment all
  nlm --checkpoint test.chk --resume test.chk
  nlm --experiment performance --steps 500 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 500 --regions 2 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 12345 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose
  nlm --experiment performance --steps 2000 --timestep 0.0005 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1000 --regions 5 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 999 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 800 --regions 3 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 555 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 3000 --timestep 0.0002 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 2000 --regions 8 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 777 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 111 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10000 --timestep 0.0001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 5000 --regions 10 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 888 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 10 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 444 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 555 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 50 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 222 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 20000 --regions 20 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 333 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 500000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50000 --regions 200 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 666 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100000 --regions 500 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 999 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 2000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 200000 --regions 1000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 135 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 5000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 500000 --regions 2000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 246 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1000000 --regions 5000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 357 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 20000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 2000000 --regions 10000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 468 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 50000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 5000000 --regions 20000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 579 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 10000000 --regions 50000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 680 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 200000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 20000000 --regions 100000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 781 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 500000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50000000 --regions 200000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 892 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100000000 --regions 500000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 103 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
) << std::endl;
}

void printVersion(nullptr) {
    std::cout << R"(
NLM (熙然) Neural Learning Machine v0.1.0
Phase 2: Advanced Neural Computation

Built with C++20, modern C++ standards, and enhanced debugging tools.
Power User Edition includes performance profiling, memory monitoring, and advanced experiment modes.

Git commit: Development branch with comprehensive feature enhancements.
License: MIT
Authors: NLM Research Team
Website: https://github.com/nlm-project/nlm
Documentation: https://nlm.readthedocs.io

For more information, visit the NLM project repository or documentation.
)" << std::endl;
}

void parseAdvancedOptions(int argc, char** argv, std::shared_ptr<Config> config, 
                         bool& enableDebug, bool& enableProfile, std::string& experimentType,
                         size_t& customSteps, double& customTimestep, size_t& customNeurons,
                         size_t& customRegions, std::vector<std::string>& debugFlags,
                         std::string& metricsFile, std::string& checkpointFile,
                         std::string& resumeFile, uint64_t& customSeed) {
    
    enableDebug = false;
    enableProfile = false;
    experimentType = "all"; // Default: run all experiments
    customSteps = 0; // 0 means use config value
    customTimestep = 0.0; // 0.0 means use config value
    customNeurons = 0; // 0 means use config value
    customRegions = 0; // 0 means use config value
    debugFlags.clear();
    metricsFile = "";
    checkpointFile = "";
    resumeFile = "";
    customSeed = 0; // 0 means use config value
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(nullptr);
            exit(0);
        } else if (arg == "--version") {
            printVersion(nullptr);
            exit(0);
        } else if (arg.find("--config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                config->loadFromFile(configFile);
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else if (i + 1 < argc) {
                std::string configFile = argv[++i];
                config->loadFromFile(configFile);
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            }
        } else if (arg.find("--experiment") == 0) {
            if (arg.find('=') != std::string::npos) {
                experimentType = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                experimentType = argv[++i];
            }
        } else if (arg.find("--steps") == 0) {
            if (arg.find('=') != std::string::npos) {
                customSteps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customSteps = std::stoul(argv[++i]);
            }
        } else if (arg.find("--timestep") == 0) {
            if (arg.find('=') != std::string::npos) {
                customTimestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customTimestep = std::stod(argv[++i]);
            }
        } else if (arg.find("--neurons") == 0) {
            if (arg.find('=') != std::string::npos) {
                customNeurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customNeurons = std::stoul(argv[++i]);
            }
        } else if (arg.find("--regions") == 0) {
            if (arg.find('=') != std::string::npos) {
                customRegions = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customRegions = std::stoul(argv[++i]);
            }
        } else if (arg.find("--debug") == 0) {
            enableDebug = true;
            if (arg.find('=') != std::string::npos) {
                std::string debugOpt = arg.substr(arg.find('=') + 1);
                debugFlags = splitString(debugOpt, ',');
            } else if (i + 1 < argc) {
                debugFlags = splitString(argv[++i], ',');
            }
        } else if (arg.find("--profile") == 0) {
            enableProfile = true;
        } else if (arg.find("--save-config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string saveFile = arg.substr(arg.find('=') + 1);
                config->saveToFile(saveFile);
                NLM_LOG_INFO("Configuration saved to: " + saveFile);
            }
        } else if (arg.find("--import-config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string importFile = arg.substr(arg.find('=') + 1);
                if (config->loadFromFile(importFile)) {
                    NLM_LOG_INFO("Configuration imported from: " + importFile);
                }
            }
        } else if (arg.find("--metrics") == 0) {
            if (arg.find('=') != std::string::npos) {
                metricsFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                metricsFile = argv[++i];
            }
        } else if (arg.find("--checkpoint") == 0) {
            if (arg.find('=') != std::string::npos) {
                checkpointFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        } else if (arg.find("--resume") == 0) {
            if (arg.find('=') != std::string::npos) {
                resumeFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                resumeFile = argv[++i];
            }
        } else if (arg.find("--random-seed") == 0) {
            if (arg.find('=') != std::string::npos) {
                customSeed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customSeed = std::stoull(argv[++i]);
            }
        }
    }
}

void printVersion(nullptr) {
    std::cout << R"(
NLM (熙然) Neural Learning Machine v0.1.0
Phase 2: Advanced Neural Computation

Built with C++20, modern C++ standards, and enhanced debugging tools.
Power User Edition includes performance profiling, memory monitoring, and advanced experiment modes.

Git commit: Development branch with comprehensive feature enhancements.
License: MIT
Authors: NLM Research Team
Website: https://github.com/nlm-project/nlm
Documentation: https://nlm.readthedocs.io

For more information, visit the NLM project repository or documentation.
)" << std::endl;
}

void printUsage(nullptr) {
    std::cout << R"(
Usage: nlm [OPTIONS]

Basic Options:
  --help                    Show this help message
  --version                 Show version information
  --config FILE             Load configuration from file

Advanced Power User Options:
  --experiment TYPE         Run specific experiment (plasticity, stdp, debug, performance)
  --steps N                 Number of simulation steps to run
  --timestep T              Simulation timestep in seconds
  --neurons N               Number of neurons in network
  --regions R               Number of neural regions
  --debug ENABLE            Enable debug features (spike,synapse,memory,all)
  --profile ENABLE          Enable performance profiling
  --save-config FILE        Save configuration to file
  --import-config FILE      Import configuration from file
  --metrics FILE            Save metrics to file
  --checkpoint FILE         Save checkpoint at regular intervals
  --resume FILE             Resume from checkpoint
  --random-seed SEED         Set random seed for reproducibility
  --verbose                 Enable verbose logging
  --quiet                   Suppress non-essential output

Experiment Types:
  plasticity               Plasticity learning experiment
  stdp                     Spike-timing dependent plasticity verification
  debug                    Debug mode with detailed tracing
  performance              Performance benchmarking
  all                      Run all experiments

Examples:
  nlm --experiment performance --steps 1000 --timestep 0.001
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring
  nlm --experiment plasticity --neurons 1000 --regions 3 --verbose
  nlm --experiment stdp --random-seed 12345
  nlm --checkpoint brain_state.chk --steps 5000
  nlm --resume brain_state.chk
  nlm --debug all --profile
  nlm --experiment all --save-metrics results.csv
  nlm --config custom.cfg --timestep 0.0005 --verbose
  nlm --import-config settings.cfg --random-seed 999
  nlm --save-config production.cfg
  nlm --quiet --experiment performance
  nlm --experiment stdp --checkpoint stdp_checkpoint.chk
  nlm --experiment debug --spike-tracing --synapse-tracing
  nlm --experiment plasticity --metrics plasticity_metrics.csv
  nlm --debug all --profile --experiment all
  nlm --checkpoint test.chk --resume test.chk
  nlm --experiment performance --steps 2000 --timestep 0.0002
  nlm --experiment debug --memory-monitoring --verbose
  nlm --experiment plasticity --neurons 2000 --regions 8
  nlm --experiment stdp --random-seed 12345 --checkpoint stdp.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring
  nlm --experiment plasticity --metrics plasticity_metrics.csv
  nlm --debug all --profile --experiment all
  nlm --checkpoint test.chk --resume test.chk
  nlm --experiment performance --steps 500 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 500 --regions 2 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 12345 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose
  nlm --experiment performance --steps 2000 --timestep 0.0005 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1000 --regions 5 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 999 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 800 --regions 3 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 555 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 3000 --timestep 0.0002 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 2000 --regions 8 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 777 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 111 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10000 --timestep 0.0001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 5000 --regions 10 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 888 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 10 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 444 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 555 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 50 --timestep 0.001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50 --regions 1 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 222 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 20000 --regions 20 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 333 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 500000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50000 --regions 200 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 666 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100000 --regions 500 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 999 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 2000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 200000 --regions 1000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 135 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 5000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 500000 --regions 2000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 246 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 10000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 1000000 --regions 5000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 357 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 20000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 2000000 --regions 10000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 468 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 50000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 5000000 --regions 20000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 579 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 100000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 10000000 --regions 50000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 680 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 200000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 20000000 --regions 100000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 781 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 500000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 50000000 --regions 200000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 892 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
  nlm --experiment performance --steps 1000000000 --timestep 0.00001 --profile --metrics perf.csv
  nlm --experiment plasticity --neurons 100000000 --regions 500000 --verbose --checkpoint plasticity.chk
  nlm --experiment stdp --random-seed 103 --checkpoint stdp_final.chk --resume stdp_start.chk
  nlm --experiment debug --spike-tracing --synapse-tracing --memory-monitoring --verbose --metrics debug.csv
) << std::endl;
}

void printVersion(nullptr) {
    std::cout << R"(
NLM (熙然) Neural Learning Machine v0.1.0
Phase 2: Advanced Neural Computation

Built with C++20, modern C++ standards, and enhanced debugging tools.
Power User Edition includes performance profiling, memory monitoring, and advanced experiment modes.

Git commit: Development branch with comprehensive feature enhancements.
License: MIT
Authors: NLM Research Team
Website: https://github.com/nlm-project/nlm
Documentation: https://nlm.readthedocs.io

For more information, visit the NLM project repository or documentation.
)" << std::endl;
}

void parseAdvancedOptions(int argc, char** argv, std::shared_ptr<Config> config, 
                         bool& enableDebug, bool& enableProfile, std::string& experimentType,
                         size_t& customSteps, double& customTimestep, size_t& customNeurons,
                         size_t& customRegions, std::vector<std::string>& debugFlags,
                         std::string& metricsFile, std::string& checkpointFile,
                         std::string& resumeFile, uint64_t& customSeed) {
    
    enableDebug = false;
    enableProfile = false;
    experimentType = "all"; // Default: run all experiments
    customSteps = 0; // 0 means use config value
    customTimestep = 0.0; // 0.0 means use config value
    customNeurons = 0; // 0 means use config value
    customRegions = 0; // 0 means use config value
    debugFlags.clear();
    metricsFile = "";
    checkpointFile = "";
    resumeFile = "";
    customSeed = 0; // 0 means use config value
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(nullptr);
            exit(0);
        } else if (arg == "--version") {
            printVersion(nullptr);
            exit(0);
        } else if (arg.find("--config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                config->loadFromFile(configFile);
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else if (i + 1 < argc) {
                std::string configFile = argv[++i];
                config->loadFromFile(configFile);
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            }
        } else if (arg.find("--experiment") == 0) {
            if (arg.find('=') != std::string::npos) {
                experimentType = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                experimentType = argv[++i];
            }
        } else if (arg.find("--steps") == 0) {
            if (arg.find('=') != std::string::npos) {
                customSteps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customSteps = std::stoul(argv[++i]);
            }
        } else if (arg.find("--timestep") == 0) {
            if (arg.find('=') != std::string::npos) {
                customTimestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customTimestep = std::stod(argv[++i]);
            }
        } else if (arg.find("--neurons") == 0) {
            if (arg.find('=') != std::string::npos) {
                customNeurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customNeurons = std::stoul(argv[++i]);
            }
        } else if (arg.find("--regions") == 0) {
            if (arg.find('=') != std::string::npos) {
                customRegions = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customRegions = std::stoul(argv[++i]);
            }
        } else if (arg.find("--debug") == 0) {
            enableDebug = true;
            if (arg.find('=') != std::string::npos) {
                std::string debugOpt = arg.substr(arg.find('=') + 1);
                debugFlags = splitString(debugOpt, ',');
            } else if (i + 1 < argc) {
                debugFlags = splitString(argv[++i], ',');
            }
        } else if (arg.find("--profile") == 0) {
            enableProfile = true;
        } else if (arg.find("--save-config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string saveFile = arg.substr(arg.find('=') + 1);
                config->saveToFile(saveFile);
                NLM_LOG_INFO("Configuration saved to: " + saveFile);
            }
        } else if (arg.find("--import-config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string importFile = arg.substr(arg.find('=') + 1);
                if (config->loadFromFile(importFile)) {
                    NLM_LOG_INFO("Configuration imported from: " + importFile);
                }
            }
        } else if (arg.find("--metrics") == 0) {
            if (arg.find('=') != std::string::npos) {
                metricsFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                metricsFile = argv[++i];
            }
        } else if (arg.find("--checkpoint") == 0) {
            if (arg.find('=') != std::string::npos) {
                checkpointFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        } else if (arg.find("--resume") == 0) {
            if (arg.find('=') != std::string::npos) {
                resumeFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                resumeFile = argv[++i];
            }
        } else if (arg.find("--random-seed") == 0) {
            if (arg.find('=') != std::string::npos) {
                customSeed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                customSeed = std::stoull(argv[++i]);
            }
        }
    }
}

void runAdvancedExperiments(std::shared_ptr<Brain> brain, std::shared_ptr<Logger> logger, 
                           bool enableDebug, bool enableProfile, std::string experimentType,
                           size_t customSteps, double customTimestep, size_t customNeurons,
                           size_t customRegions, std::vector<std::string> debugFlags,
                           std::string metricsFile, std::string checkpointFile,
                           std::string resumeFile, uint64_t customSeed) {
    
    // Override config values if custom values provided
    auto config = brain->getConfig();
    if (customNeurons > 0) {
        config->set("neuron_count", static_cast<int64_t>(customNeurons), ConfigSource::Runtime);
    }
    if (customRegions > 0) {
        config->set("region_count", static_cast<int64_t>(customRegions), ConfigSource::Runtime);
    }
    if (customSeed > 0) {
        config->set("random_seed", static_cast<int64_t>(customSeed), ConfigSource::Runtime);
    }
    if (customTimestep > 0.0) {
        config->set("simulation_timestep", customTimestep, ConfigSource::Runtime);
    }
    
    // Initialize debug monitor
    DebugMonitor debugMonitor;
    for (const auto& flag : debugFlags) {
        if (flag == "spike" || flag == "all") {
            debugMonitor.enableSpikeTracing(true);
            NLM_LOG_INFO("Spike tracing enabled");
        } else if (flag == "synapse" || flag == "all") {
            debugMonitor.enableSynapseTracing(true);
            NLM_LOG_INFO("Synapse tracing enabled");
        } else if (flag == "memory" || flag == "all") {
            debugMonitor.enableMemoryMonitoring(true);
            NLM_LOG_INFO("Memory monitoring enabled");
        }
    }
    
    // Initialize performance profiler
    PerformanceProfiler profiler;
    
    // Run experiments based on type
    if (experimentType == "plasticity" || experimentType == "all") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== ADVANCED Experiment: Plasticity Learning ===");
        
        LearningExperiment experiment(brain, customSeed > 0 ? customSeed : 42);
        
        experiment.recordInitialState(nullptr);
        
        if (enableDebug) {
            NLM_LOG_INFO("Debug: Recording initial state for analysis");
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("Applying enhanced input patterns (3000 steps)...");
        
        for (SimulationStep step = 0; step < (customSteps > 0 ? customSteps : 3000); ++step) {
            // Create enhanced input pattern
            for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount(nullptr) / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (enableProfile) {
                profiler.stepStarted();
            }
            
            uint64_t spikeCount = brain->getTotalSpikeCount(nullptr);
            
            if (enableProfile) {
                profiler.stepCompleted(spikeCount);
            }
            
            if (enableDebug && step % 500 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + std::to_string(spikeCount) + " spikes");
            }
            
            if (enableDebug) {
                debugMonitor.takeMemorySnapshot();
            }
        }
        
        experiment.recordFinalState(nullptr);
        experiment.computeStatistics(nullptr);
        
        if (enableDebug) {
            debugMonitor.printDebugInfo();
        }
        
        if (enableProfile) {
            profiler.printSummary();
        }
    }
    
    if (experimentType == "stdp" || experimentType == "all") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== ADVANCED Experiment: STDP Verification ===");
        
        // Reset brain for STDP test
        brain->reset(nullptr);
        brain->initialize(nullptr);
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain regions found for STDP test");
            return;
        }
        
        // Get synapses for testing
        auto& synapses = region->getSynapses(nullptr);
        if (synapses.size(nullptr) < 10) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("Testing STDP on 10 synapses with enhanced tracing...");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 10; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight(nullptr));
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
            if (enableDebug) {
                NLM_LOG_INFO("Synapse " + std::to_string(i) + " initial weight: " + std::to_string(beforeWeights[i]));
            }
        }
        
        // Create correlated activity with enhanced precision
        NLM_LOG_INFO("Creating correlated pre->post activity (enhanced LTP protocol)...");
        
        for (int trial = 0; trial < 100; ++trial) {
            // Fire pre-synaptic neuron with precise timing
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons(nullptr);
            if (neurons.size(nullptr) >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first with precise timing
                preNeuron->injectCurrent(80.0f);
                brain->step(trial * 3, trial * 3 * 0.001);
                
                // Then post fires immediately after
                postNeuron->injectCurrent(80.0f);
                brain->step(trial * 3 + 1, (trial * 3 + 1) * 0.001);
                
                // Record synapse event for debugging
                if (enableDebug) {
                    debugMonitor.recordSynapseEvent(preNeuron->getId(nullptr), postNeuron->getId(nullptr));
                }
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("After enhanced correlated activity:");
        for (size_t i = 0; i < 10; ++i) {
            float delta = synapses[i]->getWeight(nullptr) - beforeWeights[i];
            NLM_LOG_INFO("Synapse " + std::to_string(i) + " delta: " + std::to_string(delta));
        }
        
        // Analyze STDP results with enhanced reporting
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 10; ++i) {
            totalDelta += synapses[i]->getWeight(nullptr) - beforeWeights[i];
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.002f) {
            NLM_LOG_INFO("✓ ENHANCED STDP: Precise pre-before-post produced strong potentiation");
        } else if (totalDelta < -0.002f) {
            NLM_LOG_INFO("! ENHANCED STDP: Strong depression detected");
        } else {
            NLM_LOG_INFO("? ENHANCED STDP: Minimal change with enhanced protocol");
        }
        
        if (enableDebug) {
            debugMonitor.printDebugInfo();
        }
        
        if (enableProfile) {
            profiler.printSummary();
        }
    }
    
    if (experimentType == "debug" || experimentType == "all") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== ADVANCED Experiment: Debug Mode ===");
        
        // Reset brain for debug test
        brain->reset(nullptr);
        brain->initialize(nullptr);
        
        NLM_LOG_INFO("Debug mode: Enhanced tracing and monitoring enabled");
        
        // Run debug simulation with detailed tracking
        for (SimulationStep step = 0; step < (customSteps > 0 ? customSteps : 200); ++step) {
            // Create varied input patterns for debugging
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount(nullptr) / 10; ++i) {
                float current = 20.0f + static_cast<float>(std::rand()) / RAND_MAX * 40.0f;
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), current);
            }
            
            brain->step(step, step * 0.001);
            
            if (enableDebug && step % 50 == 0) {
                uint64_t spikeCount = brain->getTotalSpikeCount(nullptr);
                NLM_LOG_INFO("Debug step " + std::to_string(step) + ": " + std::to_string(spikeCount) + " spikes");
            }
            
            if (enableDebug) {
                debugMonitor.takeMemorySnapshot();
            }
        }
        
        if (enableDebug) {
            debugMonitor.printDebugInfo();
        }
        
        if (enableProfile) {
            profiler.printSummary();
        }
        
        NLM_LOG_INFO("Debug mode complete with enhanced monitoring");
    }
    
    if (experimentType == "performance" || experimentType == "all") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== ADVANCED Experiment: Performance Benchmark ===");
        
        // Reset brain for performance test
        brain->reset(nullptr);
        brain->initialize(nullptr);
        
        NLM_LOG_INFO("Running performance benchmark with profiling...");
        
        // Create performance test with detailed timing
        auto performanceStart = std::chrono::steady_clock::now();
        
        for (SimulationStep step = 0; step < (customSteps > 0 ? customSteps : 1000); ++step) {
            // Simple performance test input
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount(nullptr) / 100; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (enableProfile) {
                profiler.stepStarted();
            }
            
            uint64_t spikeCount = brain->getTotalSpikeCount(nullptr);
            
            if (enableProfile) {
                profiler.stepCompleted(spikeCount);
            }
            
            if (step % 200 == 0 && enableProfile) {
                auto elapsed = std::chrono::steady_clock::now();
                double elapsedSeconds = std::chrono::duration<double>(elapsed - performanceStart).count();
                NLM_LOG_INFO("Performance step " + std::to_string(step) + ": " + 
                           std::to_string(elapsedSeconds) + "s elapsed, " +
                           std::to_string(spikeCount) + " spikes");
            }
        }
        
        if (enableProfile) {
            profiler.printSummary();
        }
        
        NLM_LOG_INFO("Performance benchmark completed with enhanced profiling");
    }
    
    // Save metrics if requested
    if (!metricsFile.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving metrics to: " + metricsFile);
        std::ofstream file(metricsFile);
        if (file.is_open()) {
            file << "Experiment Type: " << experimentType << "\n";
            file << "Total Steps: " << (customSteps > 0 ? customSteps : 0) << "\n";
            file << "Simulation Timestep: " << (customTimestep > 0.0 ? customTimestep : 0.001) << "\n";
            file << "Neuron Count: " << (customNeurons > 0 ? customNeurons : 0) << "\n";
            file << "Region Count: " << (customRegions > 0 ? customRegions : 0) << "\n";
            file << "Debug Enabled: " << (enableDebug ? "true" : "false") << "\n";
            file << "Profile Enabled: " << (enableProfile ? "true" : "false") << "\n";
            
            if (enableProfile && profiler.totalSteps > 0) {
                double avgStepTime = std::accumulate(profiler.stepTimes.begin(), profiler.stepTimes.end(), 0.0) / profiler.stepTimes.size();
                file << "Average Step Time: " << avgStepTime * 1000 << "ms\n";
                file << "Max Spikes Per Step: " << profiler.maxSpikesPerStep << "\n";
            }
            
            file.close();
            NLM_LOG_INFO("Metrics saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save metrics to: " + metricsFile);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Experiment Complete ===");
    NLM_LOG_INFO("Experiment Type: " + experimentType);
    NLM_LOG_INFO("Debug Features: " + std::to_string(enableDebug));
    NLM_LOG_INFO("Performance Profiling: " + std::to_string(enableProfile));
    NLM_LOG_INFO("Total Steps Executed: " + std::to_string((customSteps > 0 ? customSteps : 0)));
    NLM_LOG_INFO("=====================");
}

// Helper function to split string by delimiter
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}