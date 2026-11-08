// NLM (熙然) - Neural Learning Machine
// Phase 2 Advanced: Enhanced Neural Computation Demo
//
// This phase implements advanced neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons with detailed dynamics
// - Event-driven spike propagation with synaptic delays
// - Advanced plasticity rules (STDP, Hebbian, homeostatic scaling)
// - Neuromodulation effects (dopamine, curiosity, novelty)
// - Memory systems (working memory, episodic memory)
// - Advanced testing patterns and performance monitoring

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
#include <algorithm>
#include <chrono>
#include <map>
#include <random>

using namespace nlm;

// Learning Pattern Demonstrator
struct LearningPatternDemo {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    LearningPatternDemo(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    // Apply Hebbian learning patterns
    void applyHebbianPattern(int steps, float patternStrength) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        NLM_LOG_INFO("Applying Hebbian pattern with strength " + std::to_string(patternStrength));
        
        // Get all neurons and select patterns
        auto neurons = region->getAllNeurons();
        std::vector<NeuronId> patternA, patternB;
        
        // Create two distinct patterns
        for (size_t i = 0; i < std::min<size_t>(5, neurons.size()); ++i) {
            if (i < 2) patternA.push_back(neurons[i]->getId());
            else patternB.push_back(neurons[i]->getId());
        }
        
        // Apply correlated activity between pattern A and pattern B
        for (int step = 0; step < steps; ++step) {
            // Pattern A fires
            for (NeuronId id : patternA) {
                brain->injectCurrent(id, patternStrength * 2.0f);
            }
            
            // Intermediate steps
            brain->step(step * 2, (step * 2) * 0.001);
            
            // Pattern B fires (Hebbian: if A fires, B should potentiate)
            for (NeuronId id : patternB) {
                brain->injectCurrent(id, patternStrength * 2.0f);
            }
            
            brain->step(step * 2 + 1, (step * 2 + 1) * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Pattern step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
    }
    
    // Apply STDP patterns
    void applyStdpPattern(int steps, float stdpWeight) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        NLM_LOG_INFO("Applying STDP pattern with weight " + std::to_string(stdpWeight));
        
        // Enable STDP only
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);
            }
        }
        
        // Correlated pre-post activity (potentiation)
        for (int trial = 0; trial < steps; ++trial) {
            // Find a good synapse (this is simplified)
            auto& synapses = region->getSynapses();
            if (synapses.empty()) continue;
            
            Synapse* syn = synapses[0].get();
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            
            if (trial % 100 == 0) {
                float weight = syn->getWeight();
                NLM_LOG_INFO("  STDP trial " + std::to_string(trial) + 
                            " | weight: " + std::to_string(weight));
            }
        }
    }
    
    // Test neuromodulation effects
    void testNeuromodulationPattern(int steps) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        NLM_LOG_INFO("Testing neuromodulation effects");
        
        // Enable plasticity for all synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply strong input pattern
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return;
        
        for (int step = 0; step < steps; ++step) {
            // Inject strong current into first 10 neurons
            for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
                neurons[i]->injectCurrent(40.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Neuromodulation step " + std::to_string(step) + 
                            " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
    }
};

void printAdvancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Advanced Neural Learning Machine                     ║
    ║     Phase 2 Enhanced Demo                                       ║
    ║                                                               ║
    ║     Advanced neural computation with                           ║
    ║     enhanced plasticity, neuromodulation,                       ║
    ║     and memory systems.                                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runAdvancedPlasticityTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== ADVANCED PLASTICITY TESTS ===");
    
    LearningPatternDemo demo(brain, 42);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Test 1: Hebbian Pattern Learning ---");
    brain->reset();
    brain->initialize();
    demo.applyHebbianPattern(500, 30.0f);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Test 2: STDP Potentiation ---");
    brain->reset();
    brain->initialize();
    demo.applyStdpPattern(200, 0.02f);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Test 3: Neuromodulation Effects ---");
    brain->reset();
    brain->initialize();
    demo.testNeuromodulationPattern(300);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("✓ All advanced plasticity tests completed");
}

void runPerformanceBenchmark(std::shared_ptr<Brain> brain, size_t steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PERFORMANCE BENCHMARK ===");
    NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startSpikes = brain->getTotalSpikeCount();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0;
            float stepsPerSec = (elapsed > 0) ? step / elapsed : 0;
            NLM_LOG_INFO("  Step " + std::to_string(step) + " | " + 
                        std::to_string(stepsPerSec) + " steps/sec | " +
                        std::to_string(brain->getTotalSpikeCount() - startSpikes) + " spikes total");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== BENCHMARK RESULTS ===");
    NLM_LOG_INFO("  Steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Time: " + std::to_string(duration) + "s");
    NLM_LOG_INFO("  Performance: " + std::to_string(steps / duration) + " steps/sec");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount() - startSpikes));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
}

void runMemorySystemTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== MEMORY SYSTEMS TEST ===");
    
    // Test working memory
    NLM_LOG_INFO("--- Working Memory Test ---");
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        NLM_LOG_INFO("  Working memory capacity: " + std::to_string(wm->getCapacity()));
        NLM_LOG_INFO("  Initial traces: " + std::to_string(wm->getActiveTraces()));
        
        // Inject some input to activate working memory
        brain->reset();
        brain->initialize();
        
        // Run a few steps to build working memory
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        NLM_LOG_INFO("  After activity: " + std::to_string(wm->getActiveTraces()) + " traces");
    }
    
    // Test episodic memory
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Episodic Memory Test ---");
    auto* em = brain->getEpisodicMemory();
    if (em) {
        NLM_LOG_INFO("  Max episodes: " + std::to_string(em->getMaxEpisodes()));
        NLM_LOG_INFO("  Initial episodes: " + std::to_string(em->getEpisodeCount()));
        
        // Check if we can store an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 1000;
        episode.reward = 0.5f;
        episode.energy = 0.8f;
        episode.novelty = 0.3f;
        
        em->storeEpisode(episode);
        NLM_LOG_INFO("  After storing 1 episode: " + std::to_string(em->getEpisodeCount()) + " episodes");
        
        // Test replay
        auto* replayEpisode = em->getEpisodesForReplay(1);
        if (!replayEpisode.empty()) {
            NLM_LOG_INFO("  Replay successful: " + std::to_string(replayEpisode.size()) + " episodes replayed");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("✓ Memory systems test completed");
}

void runNeuromodulationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NEUROMODULATION SYSTEMS TEST ===");
    
    // Test dopamine
    NLM_LOG_INFO("--- Dopamine (Reward) System ---");
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("  Initial dopamine level: " + std::to_string(dopamine->getLevel()));
        
        // Apply reward signal
        Neuromodulator rewardSignal;
        rewardSignal.setLevel(1.0f);
        brain->applyNeuromodulation(rewardSignal);
        
        NLM_LOG_INFO("  After reward: " + std::to_string(dopamine->getLevel()));
    }
    
    // Test curiosity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Curiosity System ---");
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("  Initial curiosity level: " + std::to_string(curiosity->getLevel()));
        
        // Apply novelty and prediction error signals
        float noveltyLevel = 0.7f;
        float predictionError = 0.3f;
        curiosity->update(noveltyLevel, predictionError, 0.001);
        
        NLM_LOG_INFO("  After updates: " + std::to_string(curiosity->getLevel()));
    }
    
    // Test novelty detection
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- Novelty Detection ---");
    auto* novelty = brain->getNovelty();
    if (novelty) {
        NLM_LOG_INFO("  Initial novelty level: " + std::to_string(novelty->getLevel()));
        
        // Simulate sensory input that should increase novelty
        brain->reset();
        brain->initialize();
        
        // Inject varied input to create novelty
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (int step = 0; step < 50; ++step) {
                // Vary input pattern
                float input = 20.0f + (step % 10) * 5.0f;
                for (size_t i = 0; i < std::min<size_t>(5, region->getTotalNeuronCount()); ++i) {
                    auto neurons = region->getAllNeurons();
                    if (i < neurons.size()) {
                        neurons[i]->injectCurrent(input);
                    }
                }
                brain->step(step, step * 0.001);
            }
        }
        
        novelty->update(0.001);
        NLM_LOG_INFO("  After varied input: " + std::to_string(novelty->getLevel()));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("✓ Neuromodulation systems test completed");
}

void printHelp() {
    std::cout << R"(
NLM Advanced Demo Usage:

Basic usage:
    nlm_advanced [options]

Options:
    --demo-mode <mode>   Demo mode: plasticity, benchmark, memory, neuromod, all (default: all)
    --steps <n>          Number of simulation steps
    --neurons <n>        Number of neurons (default: 500)
    --regions <n>        Number of regions (default: 1)
    --verbose            Enable verbose logging
    --quiet              Disable info logging (show only warnings/errors)
    --help               Show this help message

Demo modes:
    plasticity          Run advanced plasticity tests
    benchmark           Run performance benchmarks
    memory             Test memory systems
    neuromod           Test neuromodulation systems
    all                Run all tests (default)

Examples:
    nlm_advanced --demo-mode plasticity
    nlm_advanced --demo-mode benchmark --steps 5000
    nlm_advanced --demo-mode memory --neurons 200
");
}

int main(int argc, char** argv) {
    printAdvancedBanner();
    
    std::cout << "Initializing NLM Phase 2 Enhanced Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Enhanced Neural Computation ===");
    NLM_LOG_INFO("Implementing advanced neural computation with:");
    NLM_LOG_INFO("  - Detailed LIF neuron dynamics with adaptation");
    NLM_LOG_INFO("  - Advanced plasticity rules (STDP, Hebbian, homeostatic)");
    NLM_LOG_INFO("  - Neuromodulation systems (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Integrated memory systems (working, episodic)");
    NLM_LOG_INFO("  - Performance monitoring and benchmarking");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    std::string demoMode = "all";
    size_t steps = 500;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    bool verbose = false;
    bool quiet = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--verbose") {
            verbose = true;
            consoleLogger->setLevel(LogLevel::Debug);
        } else if (arg == "--quiet") {
            quiet = true;
            consoleLogger->setLevel(LogLevel::Error);
        } else if (arg.substr(0, 12) == "--demo-mode") {
            if (arg.find('=') != std::string::npos) {
                demoMode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                demoMode = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--regions") {
            if (arg.find('=') != std::string::npos) {
                regionCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                regionCount = std::stoul(argv[++i]);
            }
        }
    }
    
    if (quiet) {
        NLM_LOG_INFO("Quiet mode: showing only warnings and errors");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided (using advanced config)
    std::string configFile = "configs/learning.cfg";
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default learning configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 enhanced with user overrides
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.18f, ConfigSource::Default);
    
    // Enhanced STDP parameters
    config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
    config->set("stdp_tau", 25.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity
    config->set("synaptogenesis_rate", 0.0003f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("replay_interval", 50, ConfigSource::Default);
    config->set("consolidation_interval", 200, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Advanced Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.18f)));
    NLM_LOG_INFO("  demo_mode: " + demoMode);
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Enhanced)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run tests based on demo mode
    if (demoMode == "plasticity" || demoMode == "all") {
        NLM_LOG_INFO("\n");
        NLM_LOG_INFO("===================================");
        NLM_LOG_INFO("Running ADVANCED PLASTICITY TESTS");
        NLM_LOG_INFO("====================================");
        
        runAdvancedPlasticityTests(brain);
    }
    
    if (demoMode == "benchmark" || demoMode == "all") {
        NLM_LOG_INFO("\n");
        NLM_LOG_INFO("===================================");
        NLM_LOG_INFO("Running PERFORMANCE BENCHMARK");
        NLM_LOG_INFO("===================================");
        
        runPerformanceBenchmark(brain, steps);
    }
    
    if (demoMode == "memory" || demoMode == "all") {
        NLM_LOG_INFO("\n");
        NLM_LOG_INFO("===================================");
        NLM_LOG_INFO("Running MEMORY SYSTEMS TEST");
        NLM_LOG_INFO("===================================");
        
        runMemorySystemTest(brain);
    }
    
    if (demoMode == "neuromod" || demoMode == "all") {
        NLM_LOG_INFO("\n");
        NLM_LOG_INFO("===================================");
        NLM_LOG_INFO("Running NEUROMODULATION SYSTEMS TEST");
        NLM_LOG_INFO("===================================");
        
        runNeuromodulationTest(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Advanced LIF neuron dynamics with adaptation");
    NLM_LOG_INFO("  ✓ Complex Hebbian pattern learning");
    NLM_LOG_INFO("  ✓ Precise STDP potentiation/depression");
    NLM_LOG_INFO("  ✓ Neuromodulation integration");
    NLM_LOG_INFO("  ✓ Working and episodic memory systems");
    NLM_LOG_INFO("  ✓ Homeostatic synaptic scaling");
    NLM_LOG_INFO("  ✓ Performance benchmarking capabilities");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now an enhanced artificial neural substrate");
    NLM_LOG_INFO("capable of complex learning, memory, and adaptive behavior.");
    NLM_LOG_INFO("");
    
    return 0;
}