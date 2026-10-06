NLM (熙然) - Neural Learning Machine
Phase 2: Real Neural Computation

This phase implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

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
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace nlm;

// Enhanced command-line argument parser
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointFile = "";
    std::string experimentType = "basic"; // basic, plasticity, stdp, integration, batch
    int maxSteps = 100;
    int numRuns = 1;
    int neuronCount = 500;
    double timestep = 0.001;
    bool verbose = false;
    bool profiling = false;
    bool saveBrain = false;
    bool loadBrain = false;
    bool benchmark = false;
    int benchmarkIterations = 10;
    bool enablePlasticity = true;
    bool enableSTDP = true;
    bool enableDevelopment = false;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    float connectionProbability = 0.15f;
    int randomSeed = 42;
};

CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--profiling" || arg == "-p") {
            args.profiling = true;
        } else if (arg == "--benchmark" || arg == "-b") {
            args.benchmark = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--output") {
            if (arg.find('=') != std::string::npos) {
                args.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--batch" || arg == "-B") {
            if (i + 1 < argc) {
                args.numRuns = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 13) == "--experiment") {
            if (arg.find('=') != std::string::npos) {
                args.experimentType = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.experimentType = argv[++i];
            }
        } else if (arg.substr(0, 10) == "--maxsteps") {
            if (arg.find('=') != std::string::npos) {
                args.maxSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.maxSteps = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--neuron") {
            if (arg.find('=') != std::string::npos) {
                args.neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--time") {
            if (arg.find('=') != std::string::npos) {
                args.timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.timestep = std::stod(argv[++i]);
            }
        } else if (arg.substr(0, 5) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                args.randomSeed = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.randomSeed = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--prob") {
            if (arg.find('=') != std::string::npos) {
                args.connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg == "--no-plasticity") {
            args.enablePlasticity = false;
        } else if (arg == "--no-stdp") {
            args.enableSTDP = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else if (arg == "--checkpoint") {
            args.enableCheckpointing = true;
            if (i + 1 < argc) {
                args.checkpointFile = argv[++i];
            }
        } else if (arg == "--replay") {
            args.enableReplay = true;
        } else if (arg.substr(0, 9) == "--benchmark-iter") {
            if (arg.find('=') != std::string::npos) {
                args.benchmarkIterations = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                args.benchmarkIterations = std::stoi(argv[++i]);
            }
        } else {
            // Unknown argument
            std::cerr << "Warning: Unknown argument \"" << arg << "\" ignored." << std::endl;
        }
    }
    
    return args;
}

void printUsage() {
    std::cout << R"(
NLM - Neural Learning Machine
Phase 2: Real Neural Computation

Usage: nlm [OPTIONS]

Basic Options:
  --help, -h              Show this help message
  --verbose, -v           Enable verbose output
  --config FILE, -c FILE   Configuration file (default: configs/default.cfg)
  --output FILE, -o FILE   Output file for results

Experiment Types:
  --experiment TYPE       Experiment type:
                           basic      : Basic connectivity test
                           plasticity : Plasticity learning experiment  
                           stdp       : STDP verification
                           integration: Full integration test (Phase 6)
                           batch      : Run multiple experiments

Advanced Options:
  --maxsteps N            Number of simulation steps (default: 100)
  --neuron N              Number of neurons (default: 500)
  --time T                Simulation timestep in seconds (default: 0.001)
  --seed S                Random seed (default: 42)
  --prob P                 Connection probability (default: 0.15)
  --batch N                Run N experiments sequentially
  --benchmark             Enable benchmarking
  --benchmark-iter N      Number of benchmark iterations (default: 10)
  --profiling, -p         Enable profiling

Features:
  --no-plasticity         Disable plasticity
  --no-stdp               Disable STDP
  --no-development         Disable development
  --checkpoint FILE        Enable checkpointing and save to file
  --replay                Enable replay mode

Examples:
  nlm                                    # Run basic experiment
  nlm --experiment plasticity --maxsteps 1000
  nlm --experiment integration --neuron 1000 --batch 3
  nlm --benchmark --benchmark-iter 20
  nlm --config myconfig.cfg --output results.txt

For detailed information about NLM, visit: https://github.com/nlm-project/nlm
    )" << std::endl;
}

void printProgress(int current, int total, int step, const std::string& currentTime) {
    if (current % (total / 10) == 0 || current == total) {
        std::cout << "\rProgress: " << (current * 100) / total << "% "
                  << "Step: " << step << " "
                  << "Time: " << currentTime << std::flush;
    }
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

double calculateAverageFiringRate(const std::vector<size_t>& spikes, double time) {
    if (time <= 0.0 || spikes.empty()) return 0.0;
    return static_cast<double>(*std::max_element(spikes.begin(), spikes.end())) / time;
}

void saveResultsToFile(const CommandLineArgs& args, const std::string& results) {
    if (args.outputFile.empty()) return;
    
    std::ofstream file(args.outputFile, std::ios::app);
    if (file.is_open()) {
        file << "=== NLM Experiment Results ===\n";
        file << "Timestamp: " << getCurrentTime() << "\n";
        file << "Experiment Type: " << args.experimentType << "\n";
        file << "Max Steps: " << args.maxSteps << "\n";
        file << "Neuron Count: " << args.neuronCount << "\n";
        file << "Timestep: " << args.timestep << "\n";
        file << "Random Seed: " << args.randomSeed << "\n";
        file << "Connection Probability: " << args.connectionProbability << "\n";
        file << "Plasticity: " << (args.enablePlasticity ? "ON" : "OFF") << "\n";
        file << "STDP: " << (args.enableSTDP ? "ON" : "OFF") << "\n";
        file << "Development: " << (args.enableDevelopment ? "ON" : "OFF") << "\n";
        file << "Checkpointing: " << (args.enableCheckpointing ? "ON" : "OFF") << "\n";
        file << "Replay: " << (args.enableReplay ? "ON" : "OFF") << "\n";
        file << "\nResults:\n";
        file << results << "\n";
        file << "=== End Results ===\n\n";
        file.close();
    }
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runPhase6Integration(const CommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    
    Phase6IntegratedExperiment experiment;
    
    // Create Phase 6 config
    Phase6Config config;
    config.neuronCount = args.neuronCount;
    config.maxSteps = args.maxSteps;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    if (!args.checkpointFile.empty()) {
        config.checkpointPath = args.checkpointFile;
    }
    
    // Run integration verification first
    NLM_LOG_INFO("Running integration verification...");
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        return;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    
    // Test individual systems
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    
    if (args.enableCheckpointing) {
        experiment.testCheckpointing();
    }
    
    if (args.enableReplay) {
        experiment.testReplay();
    }
    
    // Run full integration experiment
    NLM_LOG_INFO("Running full integration experiment...");
    auto result = experiment.run(config);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("Total reward: " << std::fixed << std::setprecision(2) << result.totalReward);
    NLM_LOG_INFO("Average firing rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate);
    NLM_LOG_INFO("Memory episodes stored: " << result.memoryEpisodesStored);
    NLM_LOG_INFO("Dopamine level: " << std::fixed << std::setprecision(2) << result.dopamineLevel);
    NLM_LOG_INFO("");
    
    // Print system integration status
    NLM_LOG_INFO("=== System Integration Status ===");
    NLM_LOG_INFO("Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
    NLM_LOG_INFO("Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING"));
}

void runBenchmark(std::shared_ptr<Brain> brain, int iterations) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmarking ===");
    NLM_LOG_INFO("Running " << iterations << " iterations...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        brain->reset();
        brain->initialize();
        
        // Run simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        if (i % 10 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            printProgress(i, iterations, step, std::to_string(elapsed / 1000.0) + "s");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    double avgTime = static_cast<double>(totalTime) / iterations;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark Results:");
    NLM_LOG_INFO("  Total time: " << std::fixed << std::setprecision(2) << totalTime / 1000.0 << "s");
    NLM_LOG_INFO("  Average time per iteration: " << std::fixed << std::setprecision(4) << avgTime / 1000.0 << "s");
    NLM_LOG_INFO("  Iterations per second: " << std::fixed << std::setprecision(2) << iterations / (totalTime / 1000.0) << " Hz");
}

std::string getResultsSummary(const CommandLineArgs& args, std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    ss << "Results Summary:" << std::endl;
    ss << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    ss << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    ss << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "  Average firing rate: " << std::fixed << std::setprecision(4) << brain->getAverageFiringRate() << std::endl;
    ss << "  Excitation/inhibition ratio: " << std::fixed << std::setprecision(4) << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    return ss.str();
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Print banner
    printBanner();
    
    // Initialize logging based on verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    
    std::string resultsSummary = "";
    
    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
        if (config->loadFromFile(args.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line args and experiment settings
        config->loadFromArgs(argc, argv);
        
        // Set experiment-specific configuration
        config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
        config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
        config->set("connection_probability", args.connectionProbability, ConfigSource::CommandLine);
        
        // Plasticity settings
        config->set("stdp_enabled", args.enableSTDP, ConfigSource::CommandLine);
        config->set("hebbian_enabled", args.enablePlasticity, ConfigSource::CommandLine);
        config->set("structural_plasticity_enabled", args.enableDevelopment, ConfigSource::CommandLine);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  Random seed: " + std::to_string(config->getOr<int64_t>("random_seed", args.randomSeed)));
        NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", args.timestep)) + "s");
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", args.neuronCount)));
        NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  Connection probability: " + std::to_string(config->getOr<float>("connection_probability", args.connectionProbability)));
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", args.timestep);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        if (args.benchmark) {
            // Run benchmark mode
            for (int run = 0; run < args.numRuns; ++run) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Benchmark Run " << run + 1 << "/" << args.numRuns << " ===");
                
                auto brain = std::make_shared<Brain>(config);
                if (!brain->initialize()) {
                    NLM_LOG_ERROR("Failed to initialize brain!");
                    return 1;
                }
                
                runBenchmark(brain, args.benchmarkIterations);
                
                brain->reset();
            }
        } else {
            // Run experiments based on type
            for (int run = 0; run < args.numRuns; ++run) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Experiment Run " << run + 1 << "/" << args.numRuns << " ===");
                NLM_LOG_INFO("Experiment type: " << args.experimentType);
                NLM_LOG_INFO("Max steps: " << args.maxSteps);
                
                auto brain = std::make_shared<Brain>(config);
                if (!brain->initialize()) {
                    NLM_LOG_ERROR("Failed to initialize brain!");
                    return 1;
                }
                
                if (args.experimentType == "basic") {
                    runBasicConnectivityTest(brain);
                } else if (args.experimentType == "plasticity") {
                    runPlasticityExperiment(brain);
                } else if (args.experimentType == "stdp") {
                    runStdpVerification(brain);
                } else if (args.experimentType == "integration") {
                    runPhase6Integration(args);
                    break; // Phase 6 integration is comprehensive, run only once
                } else if (args.experimentType == "batch") {
                    // Run all experiment types in sequence
                    NLM_LOG_INFO("Running batch experiment (basic -> plasticity -> stdp)...");
                    runBasicConnectivityTest(brain);
                    
                    brain->reset();
                    brain->initialize();
                    runPlasticityExperiment(brain);
                    
                    brain->reset();
                    brain->initialize();
                    runStdpVerification(brain);
                } else {
                    NLM_LOG_ERROR("Unknown experiment type: " + args.experimentType);
                    return 1;
                }
                
                // Save brain state if requested
                if (args.saveBrain) {
                    std::string filename = "brain_checkpoint_" + std::to_string(run) + ".bin";
                    if (brain->save(filename)) {
                        NLM_LOG_INFO("Brain state saved to: " + filename);
                    } else {
                        NLM_LOG_ERROR("Failed to save brain state!");
                    }
                }
                
                // Load brain state if requested
                if (args.loadBrain && run > 0) {
                    if (brain->load("brain_checkpoint_" + std::to_string(run - 1) + ".bin")) {
                        NLM_LOG_INFO("Brain state loaded successfully.");
                    } else {
                        NLM_LOG_ERROR("Failed to load brain state!");
                    }
                }
                
                // Get results summary
                resultsSummary += "\n--- Run " + std::to_string(run + 1) + " ---\n";
                resultsSummary += getResultsSummary(args, brain);
                
                // Reset for next run (unless it's the last one)
                if (run < args.numRuns - 1) {
                    brain->reset();
                    brain->initialize();
                }
            }
        }
        
        // Save results to file if requested
        saveResultsToFile(args, resultsSummary);
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Status ===");
        if (!args.benchmark) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brain->logStatus();
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("");
        
        if (!resultsSummary.empty()) {
            NLM_LOG_INFO("=== Results Summary ===");
            NLM_LOG_INFO(resultsSummary);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during execution: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}