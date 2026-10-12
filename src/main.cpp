// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Comprehensive Brain System
//
// This is the main executable for NLM Phase 6, implementing:
// - Complete integrated brain architecture with all systems
// - Agent-based interaction with the world
// - Advanced learning and development capabilities
// - Comprehensive testing and benchmarking

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBrainFactory.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include <algorithm>

using namespace nlm;

// ===== COMMAND LINE ARGUMENTS =====
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string experiment = "all";  // all, basic, plasticity, stdp, phase2, phase3, phase4, phase6
    std::string agentProfile = "default";
    std::string benchmarkMode = "off";  // off, speed, accuracy, memory, learning
    bool verbose = false;
    bool saveCheckpoint = false;
    std::string checkpointFile = "brain_checkpoint.nlm";
    int numRuns = 1;
    double timeLimit = 0.0;  // seconds (0 = unlimited)
    bool help = false;
};

// ===== UTILITY FUNCTIONS =====
void printUsage(const char* programName) {
    std::cout << R"(
NLM — 熙然 Neural Learning Machine
Usage: "" << programName << " [OPTIONS]

Comprehensive Brain Simulation Framework - Phase 6 Final Integration

OPTIONS:
  --config <file>                Configuration file (default: configs/default.cfg)
  --experiment <name>            Experiment mode:
                                 all      - Run all available experiments (default)
                                 basic    - Basic connectivity test
                                 plasticity - Plasticity learning experiment
                                 stdp      - STDP verification
                                 phase2    - Phase 2: Real Neural Computation
                                 phase3    - Phase 3: World Interaction
                                 phase4    - Phase 4: Cognitive Development
                                 phase6    - Phase 6: Final Integration
  --agent-profile <profile>      Agent profile (default, explorer, worker, social, conservative, aggressive, adaptive)
  --benchmark <mode>             Benchmarking mode (off, speed, accuracy, memory, learning)
  --verbose                      Enable verbose logging
  --save-checkpoint <file>       Save brain state to checkpoint file
  --runs <count>                 Number of experiment runs (default: 1)
  --time-limit <seconds>         Maximum runtime per experiment (0 = unlimited)
  --help                         Display this help message

EXAMPLES:
  "" << programName << " --experiment phase6 --agent-profile explorer --benchmark speed"
  "" << programName << " --experiment all --verbose --save-checkpoint brain.nlm"
  "" << programName << " --experiment plasticity --benchmark accuracy --runs 5"

DESIGNED FOR:
  • Research and development of artificial neural systems
  • Development of brain-inspired architectures
  • Study of learning and development processes
  )" << std::endl;
}

CommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--experiment" && i + 1 < argc) {
            args.experiment = argv[++i];
        } else if (arg == "--agent-profile" && i + 1 < argc) {
            args.agentProfile = argv[++i];
        } else if (arg == "--benchmark" && i + 1 < argc) {
            args.benchmarkMode = argv[++i];
        } else if (arg == "--save-checkpoint" && i + 1 < argc) {
            args.saveCheckpoint = true;
            args.checkpointFile = argv[++i];
        } else if (arg == "--runs" && i + 1 < argc) {
            args.numRuns = std::atoi(argv[++i]);
        } else if (arg == "--time-limit" && i + 1 < argc) {
            args.timeLimit = std::atof(argv[++i]);
        }
    }
    
    return args;
}

AgentProfile AgentProfileFromString(const std::string& profileName) {
    std::string lowerName = profileName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName == "default") return AgentProfile::Default;
    if (lowerName == "explorer") return AgentProfile::Explorer;
    if (lowerName == "worker") return AgentProfile::Worker;
    if (lowerName == "social") return AgentProfile::Social;
    if (lowerName == "conservative") return AgentProfile::Conservative;
    if (lowerName == "aggressive") return AgentProfile::Aggressive;
    if (lowerName == "adaptive") return AgentProfile::Adaptive;
    
    // Default to Default if not found
    NLM_LOG_WARNING("Unknown agent profile '" + profileName + "', using default");
    return AgentProfile::Default;
}

// ===== EXPERIMENT IMPLEMENTATIONS =====
void runPhase2Experiment(std::shared_ptr<Brain> brain, bool verbose) {
    printExperimentHeader("=== NLM Phase 2: Real Neural Computation ===", 1);
    
    if (verbose) {
        std::cout << "Testing real spiking neural computation with:" << std::endl;
        std::cout << "  • Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
        std::cout << "  • Event-driven spike propagation with delays" << std::endl;
        std::cout << "  • STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    }
    
    // Test 1: Basic connectivity
    std::cout << "\nTest 1: Basic Neural Connectivity" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "ERROR: No region found!" << std::endl;
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "ERROR: No neurons found!" << std::endl;
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    std::cout << "Injecting current into 10 neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "✓ SUCCESS: Spikes propagate through network" << std::endl;
    } else {
        std::cout << "⚠ WARNING: No spikes generated - checking neuron parameters" << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "  Neuron " << i 
                     << " V=" << neurons[i]->getMembranePotential()
                     << " thresh=" << neurons[i]->getThreshold()
                     << std::endl;
        }
    }
    
    // Test 2: Plasticity learning
    std::cout << "\nTest 2: Plasticity Learning Experiment" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    // Record initial state
    size_t initialSynapseCount = brain->getTotalSynapseCount();
    std::cout << "Initial synapse count: " << initialSynapseCount << std::endl;
    
    // Enable plasticity
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Hebbian and STDP
        }
    }
    
    // Apply learning input
    std::cout << "Applying learning input patterns (500 steps)..." << std::endl;
    for (SimulationStep step = 0; step < 500; ++step) {
        // Inject sensory input
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step 
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Firing: " << brain->getFiringNeuronCount()
                     << std::endl;
        }
    }
    
    size_t finalSynapseCount = brain->getTotalSynapseCount();
    std::cout << "Final synapse count: " << finalSynapseCount << std::endl;
    std::cout << "Synapse change: " << (finalSynapseCount - initialSynapseCount) << std::endl;
    
    // Test 3: STDP verification
    std::cout << "\nTest 3: STDP Verification" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    if (synapses.size() < 5) {
        std::cout << "Not enough synapses for STDP test" << std::endl;
        return;
    }
    
    std::cout << "Testing STDP on 5 synapses..." << std::endl;
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // STDP only
        std::cout << "  Synapse " << i << " before: " << beforeWeights[i] << std::endl;
    }
    
    // Apply correlated pre->post activity (LTP)
    std::cout << "Applying correlated pre->post activity (LTP)..." << std::endl;
    for (int trial = 0; trial < 25; ++trial) {
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
    
    // Check weight changes
    std::cout << "After correlated activity:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "  Synapse " << i 
                 << " after: " << synapses[i]->getWeight()
                 << " (Δ=" << delta << ")" << std::endl;
    }
    
    if (totalDelta > 0.001f) {
        std::cout << "✓ SUCCESS: STDP working - pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "⚠ WARNING: STDP reversed - check parameters" << std::endl;
    } else {
        std::cout << "⚠ WARNING: No change - STDP may not be triggering" << std::endl;
    }
    
    std::cout << "\n✓ Phase 2 Experiment Complete" << std::endl;
}

void runAgentExperiment(std::shared_ptr<Brain> brain, const std::string& profile, bool verbose) {
    printExperimentHeader("=== Agent Integration Experiment ===", 1);
    
    if (verbose) {
        std::cout << "Creating agent with profile: " << profile << std::endl;
    }
    
    // Create world for agent interaction
    SimpleWorld world(32, 32);  // 32x32 grid world
    
    // Create agent using factory
    std::unique_ptr<AgentBrain> agent = AgentBrainFactory::createAgent(brain, world, AgentProfileFromString(profile));
    
    if (!agent) {
        std::cout << "ERROR: Failed to create agent!" << std::endl;
        return;
    }
    
    std::cout << "Agent created successfully" << std::endl;
    std::cout << "  Motor neurons: " << agent->getMotorOutputSize() << std::endl;
    std::cout << "  Sensory input size: " << agent->getSensoryInputSize() << std::endl;
    std::cout << "  Developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    
    // Simulate agent-environment interaction
    std::cout << "\nSimulating agent-environment interaction..." << std::endl;
    
    for (int step = 0; step < 100; ++step) {
        // Create sensory percept
        SensoryPercept percept;
        
        // Simple random perception
        for (size_t i = 0; i < 256; ++i) percept.addVision(i % 256 * 0.01f);
        for (size_t i = 0; i < 8; ++i) percept.addTouch(i * 0.1f);
        for (size_t i = 0; i < 4; ++i) percept.addInternal((i - 2) * 0.1f);
        for (size_t i = 0; i < 6; ++i) percept.addProprioception((i - 3) * 0.1f);
        
        // Process input
        agent->processSensoryInput(percept);
        
        // Decode action
        MotorCommand action = agent->decodeMotorCommand();
        
        // Apply reward-based learning
        float reward = 0.1f;  // Simple reward
        agent->applyRewardModulation(reward, 0.05f);  // Small prediction error
        
        // Update development
        agent->updateDevelopment(0.001);
        
        // Log progress
        if (step % 20 == 0) {
            std::cout << "  Step " << step 
                     << " | Action: " << static_cast<int>(action)
                     << " | Dopamine: " << agent->getNeuromodulationLevel()
                     << " | Curiosity: " << agent->getCuriosityLevel()
                     << " | Novelty: " << agent->getNoveltyLevel()
                     << std::endl;
        }
    }
    
    std::cout << "\n✓ Agent Integration Experiment Complete" << std::endl;
}

// ===== BENCHMARKING FUNCTIONS =====
void runBenchmark(const CommandLineArgs& args, std::shared_ptr<Brain> brain) {
    printExperimentHeader("=== NLM Performance Benchmarking ===", 1);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Basic performance metrics
    size_t initialSpikes = brain->getTotalSpikeCount();
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Run benchmark steps
    size_t steps = 1000;
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Simple benchmark metrics
        if (step % 200 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            std::cout << "  Step " << step 
                     << " | Time: " << duration.count() << "ms"
                     << " | Spikes/sec: " << brain->getTotalSpikeCount() - initialSpikes
                     << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\nBenchmark Results:" << std::endl;
    std::cout << "  Total simulation time: " << totalDuration.count() << "ms" << std::endl;
    std::cout << "  Steps per second: " << (steps * 1000.0 / totalDuration.count()) << std::endl;
    std::cout << "  Total spikes generated: " << brain->getTotalSpikeCount() - initialSpikes << std::endl;
    std::cout << "  Average firing rate: " << (brain->getTotalSpikeCount() - initialSpikes) / (steps * 1.0) << " spikes/step" << std::endl;
    
    std::cout << "\n✓ Benchmarking Complete" << std::endl;
}

// ===== MAIN FUNCTION =====
int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    if (args.help) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM Neural Learning Machine - Phase 6 Final Integration" << std::endl;
    std::cout << "=======================================================" << std::endl;
    
    if (args.verbose) {
        std::cout << "Command line arguments parsed:" << std::endl;
        std::cout << "  Config file: " << args.configFile << std::endl;
        std::cout << "  Experiment: " << args.experiment << std::endl;
        std::cout << "  Agent profile: " << args.agentProfile << std::endl;
        std::cout << "  Benchmark mode: " << args.benchmarkMode << std::endl;
        std::cout << "  Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
        std::cout << "  Save checkpoint: " << (args.saveCheckpoint ? "Yes" : "No") << std::endl;
        std::cout << "  Number of runs: " << args.numRuns << std::endl;
        std::cout << "  Time limit: " << args.timeLimit << " seconds" << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set experiment-specific configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiments
    for (int run = 1; run <= args.numRuns; ++run) {
        if (args.numRuns > 1) {
            std::cout << "\n" << std::string(80, '=') << std::endl;
            std::cout << "EXPERIMENT RUN " << run << " OF " << args.numRuns << std::endl;
            std::cout << std::string(80, '=') << std::endl;
        }
        
        // Reset brain for each run
        brain->reset();
        brain->initialize();
        
        // Run specified experiment
        if (args.experiment == "all" || args.experiment == "phase2") {
            runPhase2Experiment(brain, args.verbose);
        } else if (args.experiment == "all" || args.experiment == "agent") {
            runAgentExperiment(brain, args.agentProfile, args.verbose);
        } else if (args.experiment == "all" || args.experiment == "benchmark") {
            runBenchmark(args, brain);
        } else {
            std::cout << "ERROR: Unknown experiment mode: " << args.experiment << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
        // Save checkpoint if requested
        if (args.saveCheckpoint && run == args.numRuns) {
            NLM_LOG_INFO("Saving checkpoint to: " + args.checkpointFile);
            if (!brain->save(args.checkpointFile)) {
                NLM_LOG_ERROR("Failed to save checkpoint!");
            }
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Experiments Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Summary:")
    NLM_LOG_INFO("  Runs completed: " + std::to_string(args.numRuns));
    NLM_LOG_INFO("  Final brain status:")
    brain->logStatus();
    
    std::cout << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "NLM Phase 6 Final Integration Complete!" << std::endl;
    std::cout << "============================================" << std::endl;
    
    return 0;
}

(End of file - total 1502 lines)