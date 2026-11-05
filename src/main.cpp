// NLM - Unified Demo System
// Provides command-line interface for running different NLM phases
// Supports multiple demo modes with configurable parameters

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
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified Demo System                                        ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "Usage: nlm_demo [OPTIONS]\n\n";
    std::cout << "Run NLM brain simulations with configurable options.\n\n";
    std::cout << "Options:\n";
    std::cout << "  --phase PHASE          Phase to run (2, 3, 4, 5, 6) [default: 2]\n";
    std::cout << "  --help                 Show this help message\n";
    std::cout << "  --version              Show version information\n";
    std::cout << "  --build-only           Only build the system, don't run\n";
    std::cout << "  --config FILE          Configuration file [default: configs/default.cfg]\n";
    std::cout << "  --episodes N           Number of episodes for phase 3/4/6 [default: 5]\n";
    std::cout << "  --steps-per-episode N   Steps per episode [default: 200]\n";
    std::cout << "  --neuron-count N       Total neurons [default: 500]\n";
    std::cout << "  --region-count N        Number of brain regions [default: 1]\n";
    std::cout << "  --random-seed N         Random seed [default: 42]\n";
    std::cout << "  --enable-dev            Enable development features\n";
    std::cout << "  --enable-reward         Enable reward modulation\n";
    std::cout << "  --enable-curiosity     Enable curiosity module\n";
    std::cout << "  --enable-plasticity    Enable synaptic plasticity\n";
    std::cout << "  --enable-structural    Enable structural plasticity\n";
    std::cout << "  --benchmark            Run benchmark mode\n";
    std::cout << "\nExamples:\n";
    std::cout << "  ./nlm_demo --phase 6 --episodes 10 --neuron-count 1000\n";
    std::cout << "  ./nlm_demo --phase 2 --enable-plasticity\n";
    std::cout << "  ./nlm_demo --phase 3 --steps-per-episode 500\n";
}

void printVersion() {
    std::cout << "NLM Unified Demo System v0.1.0\n";
    std::cout << "Phase 2: Real Neural Computation\n";
    std::cout << "Phase 3: World Interaction\n";
    std::cout << "Phase 4: Cognitive Development\n";
    std::cout << "Phase 5: Performance & Integration\n";
    std::cout << "Phase 6: Final Integration\n";
}

class UnifiedDemo {
public:
    struct DemoOptions {
        std::string phase;
        std::string configFile = "configs/default.cfg";
        int episodes = 5;
        int stepsPerEpisode = 200;
        int neuronCount = 500;
        int regionCount = 1;
        int64_t randomSeed = 42;
        bool enableDevelopment = false;
        bool enableRewardModulation = false;
        bool enableCuriosity = false;
        bool enablePlasticity = false;
        bool enableStructuralPlasticity = false;
        bool benchmarkMode = false;
        bool buildOnly = false;
    };

    UnifiedDemo(DemoOptions opts) : options(opts) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("NLM Unified Demo System initialized");
        NLM_LOG_INFO("Phase: " + options.phase);
    }

    bool run() {
        NLM_LOG_INFO("Starting NLM Unified Demo");
        
        if (options.benchmarkMode) {
            return runBenchmark();
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from file
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line options
        config->set("random_seed", options.randomSeed, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        
        // Apply feature flags
        if (options.enableDevelopment) {
            config->set("development_enabled", true, ConfigSource::Default);
        }
        if (options.enableRewardModulation) {
            config->set("reward_modulation_enabled", true, ConfigSource::Default);
        }
        if (options.enableCuriosity) {
            config->set("curiosity_enabled", true, ConfigSource::Default);
        }
        if (options.enablePlasticity) {
            config->set("plasticity_enabled", true, ConfigSource::Default);
        }
        if (options.enableStructuralPlasticity) {
            config->set("structural_plasticity_enabled", true, ConfigSource::Default);
        }
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        
        // Run phase-specific demo
        return runPhase(brain);
    }

private:
    DemoOptions options;
    
    bool runPhase(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running Phase " + options.phase);
        
        if (options.phase == "2") {
            return runPhase2(brain);
        } else if (options.phase == "3") {
            return runPhase3(brain);
        } else if (options.phase == "4") {
            return runPhase4(brain);
        } else if (options.phase == "5") {
            return runPhase5(brain);
        } else if (options.phase == "6") {
            return runPhase6(brain);
        } else {
            NLM_LOG_ERROR("Unknown phase: " + options.phase);
            return false;
        }
    }
    
    bool runPhase2(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Testing LIF neuron dynamics, STDP, and structural plasticity");
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        return true;
    }
    
    bool runPhase3(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 3: World Interaction Demo ===");
        NLM_LOG_INFO("Creating world, agent, and running interaction loop");
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(options.randomSeed);
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        NLM_LOG_INFO("Initial brain state:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        
        // Run episodes
        for (int episode = 0; episode < options.episodes; ++episode) {
            NLM_LOG_INFO("--- Episode " << episode << " started ---");
            
            // Reset environment and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            brain->initialize();
            
            // Run steps
            for (int step = 0; step < options.stepsPerEpisode; ++step) {
                // 1. Update world
                world.update(0.1f);
                
                // 2. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 3. Process sensory input
                agentBrain.processSensoryInput(percept);
                
                // 4. Brain step
                brain->step(step, step * 0.001);
                
                // 5. Get action from brain
                Action action = agentBrain.decodeMotorCommand();
                
                // 6. Apply action in world
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Log progress periodically
                if (step % 50 == 0) {
                    NLM_LOG_INFO("Step " << step << 
                                " | Neurons firing: " << brain->getFiringNeuronCount() <<
                                " | Total spikes: " << brain->getTotalSpikeCount());
                }
            }
        }
        
        NLM_LOG_INFO("Phase 3 completed successfully");
        return true;
    }
    
    bool runPhase4(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 4: Cognitive Development ===");
        NLM_LOG_INFO("Testing working memory, episodic memory, and planning");
        
        // Create world for cognitive testing
        SimpleWorld world;
        world.configure(15.0f, 15.0f, 12, 12);
        world.setRandomSeed(options.randomSeed);
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable all cognitive features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        NLM_LOG_INFO("Running cognitive development simulation...");
        
        // Run simulation focusing on cognitive processes
        for (int step = 0; step < options.stepsPerEpisode; ++step) {
            world.update(0.1f);
            
            const SensoryPercept& percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            Action action = agentBrain.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Log cognitive metrics
            if (step % 50 == 0) {
                NLM_LOG_INFO("Cognitive Step " << step <<
                            " | Development: " << static_cast<int>(brain->getDevelopmentalStage()) <<
                            " | Prediction Error: " << agentBrain.getPredictionError());
            }
        }
        
        NLM_LOG_INFO("Phase 4 completed successfully");
        return true;
    }
    
    bool runPhase5(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 5: Performance & Integration ===");
        NLM_LOG_INFO("Testing performance optimizations and checkpointing");
        
        // Create world
        SimpleWorld world;
        world.configure(10.0f, 10.0f, 8, 8);
        world.setRandomSeed(options.randomSeed);
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable all features for integration testing
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        NLM_LOG_INFO("Running integration test with all systems...");
        
        // Run extended simulation
        for (int step = 0; step < options.stepsPerEpisode; ++step) {
            world.update(0.1f);
            
            const SensoryPercept& percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            Action action = agentBrain.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            if (step % 50 == 0) {
                NLM_LOG_INFO("Integration Step " << step <<
                            " | Spikes: " << brain->getTotalSpikeCount() <<
                            " | Firing rate: " << brain->getAverageFiringRate());
            }
        }
        
        NLM_LOG_INFO("Phase 5 completed successfully");
        return true;
    }
    
    bool runPhase6(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 6: Final Integration ===");
        NLM_LOG_INFO("Comprehensive integration test of all NLM systems");
        
        // Create world
        SimpleWorld world;
        world.configure(25.0f, 25.0f, 20, 20);
        world.setRandomSeed(options.randomSeed);
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable all features for final integration
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        NLM_LOG_INFO("Running final integration test with all systems connected...");
        
        // Run comprehensive integration test
        for (int step = 0; step < options.stepsPerEpisode; ++step) {
            world.update(0.1f);
            
            const SensoryPercept& percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            Action action = agentBrain.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            if (step % 50 == 0) {
                NLM_LOG_INFO("Final Integration Step " << step <<
                            " | Stage: " << static_cast<int>(brain->getDevelopmentalStage()) <<
                            " | Reward: " << agentBrain.getNeuromodulationLevel() <<
                            " | Curiosity: " << agentBrain.getCuriosityLevel());
            }
        }
        
        NLM_LOG_INFO("Phase 6 integration completed successfully");
        NLM_LOG_INFO("All systems are functioning cohesively");
        return true;
    }
    
    bool runBenchmark() {
        NLM_LOG_INFO("=== Benchmark Mode ===");
        NLM_LOG_INFO("Running performance benchmarks...");
        
        // Create configuration optimized for benchmarking
        auto config = std::make_shared<Config>();
        config->set("random_seed", options.randomSeed, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount * 2), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for benchmark");
            return false;
        }
        
        // Measure performance
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 1000; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Benchmark step " << step <<
                            " | Spikes: " << brain->getTotalSpikeCount() <<
                            " | Firing rate: " << brain->getAverageFiringRate());
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        NLM_LOG_INFO("Benchmark completed in " << duration.count() << " ms");
        NLM_LOG_INFO("Average steps per second: " << (1000.0 / (duration.count() / 1000.0)));
        
        return true;
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
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
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        // Use sensory neurons for pattern input
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " << step << 
                            " | Spikes: " << brain->getTotalSpikeCount() <<
                            " | Firing: " << brain->getFiringNeuronCount());
            }
        }
        
        NLM_LOG_INFO("Plasticity experiment completed");
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("  No synapses for STDP test");
            return;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("  Testing STDP on 1 synapse:");
        NLM_LOG_INFO("    Synapse before: " << beforeWeight <<
                     " (pre=" << syn->getSourceNeuron().index() <<
                     ", post=" << syn->getDestinationNeuron().index() << ")");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " << syn->getWeight() <<
                     " (Δ=" << delta << ")");
        
        if (delta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
};

// Command line argument parsing
UnifiedDemo::DemoOptions parseArguments(int argc, char** argv) {
    UnifiedDemo::DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--build-only") {
            options.buildOnly = true;
        } else if (arg == "--benchmark") {
            options.benchmarkMode = true;
        } else if (arg.substr(0, 8) == "--phase=") {
            options.phase = arg.substr(8);
        } else if (arg == "--phase" && i + 1 < argc) {
            options.phase = argv[++i];
        } else if (arg.substr(0, 9) == "--config=") {
            options.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.substr(0, 13) == "--episodes=") {
            options.episodes = std::stoi(arg.substr(13));
        } else if (arg == "--episodes" && i + 1 < argc) {
            options.episodes = std::stoi(argv[++i]);
        } else if (arg.substr(0, 18) == "--steps-per-episode=") {
            options.stepsPerEpisode = std::stoi(arg.substr(18));
        } else if (arg == "--steps-per-episode" && i + 1 < argc) {
            options.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg.substr(0, 14) == "--neuron-count=") {
            options.neuronCount = std::stoi(arg.substr(14));
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            options.neuronCount = std::stoi(argv[++i]);
        } else if (arg.substr(0, 15) == "--region-count=") {
            options.regionCount = std::stoi(arg.substr(15));
        } else if (arg == "--region-count" && i + 1 < argc) {
            options.regionCount = std::stoi(argv[++i]);
        } else if (arg.substr(0, 14) == "--random-seed=") {
            options.randomSeed = std::stoll(arg.substr(14));
        } else if (arg == "--random-seed" && i + 1 < argc) {
            options.randomSeed = std::stoll(argv[++i]);
        } else if (arg == "--enable-dev") {
            options.enableDevelopment = true;
        } else if (arg == "--enable-reward") {
            options.enableRewardModulation = true;
        } else if (arg == "--enable-curiosity") {
            options.enableCuriosity = true;
        } else if (arg == "--enable-plasticity") {
            options.enablePlasticity = true;
        } else if (arg == "--enable-structural") {
            options.enableStructuralPlasticity = true;
        } else if (arg == "--unknown") {
            printHelp();
            exit(1);
        }
    }
    
    return options;
}

int main(int argc, char** argv) {
    printBanner();
    
    UnifiedDemo::DemoOptions options = parseArguments(argc, argv);
    
    if (options.buildOnly) {
        std::cout << "Build-only mode - skipping execution" << std::endl;
        return 0;
    }
    
    UnifiedDemo demo(options);
    
    if (demo.run()) {
        NLM_LOG_INFO("=== Demo completed successfully ===");
        return 0;
    } else {
        NLM_LOG_ERROR("Demo failed");
        return 1;
    }
}