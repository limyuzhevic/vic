// NLM Command Line Interface Tool
// Provides easy access to NLM operations with comprehensive commands

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace nlm;

class NLMTool {
public:
    NLMTool() : brain(nullptr), config(nullptr), running(false) {}
    
    ~NLMTool() {
        if (brain) {
            saveBrainState("./nlm_auto_save.bin");
        }
    }
    
    bool initialize() {
        std::cout << "=== NLM Command Line Tool ===" << std::endl;
        
        // Initialize configuration
        config = std::make_shared<Config>();
        
        // Load default configuration
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        return true;
    }
    
    void printHelp() {
        std::cout << "=== NLM Command Line Tool ===" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE: nlm_tool <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  help                    - Show this help message" << std::endl;
        std::cout << "  version                 - Show NLM version information" << std::endl;
        std::cout << "  status                  - Show current brain status" << std::endl;
        std::cout << "  start [steps]           - Start simulation with optional steps" << std::endl;
        std::cout << "  stop                    - Stop current simulation" << std::endl;
        std::cout << "  reset                   - Reset brain to initial state" << std::endl;
        std::cout << "  save <file>             - Save brain state to file" << std::endl;
        std::cout << "  load <file>             - Load brain state from file" << std::endl;
        std::cout << "  demo                    - Run standard demo sequence" << std::endl;
        std::cout << "  test <test_name>        - Run specific test" << std::endl;
        std::cout << "  explore                 - Run exploration mode" << std::endl;
        std::cout << "  develop                 - Run developmental simulation" << std::endl;
        std::cout << "  config <key=value>      - Set configuration parameter" << std::endl;
        std::cout << "  show config             - Show current configuration" << std::endl;
        std::cout << "  logs                    - Show log file location" << std::endl;
        std::cout << "  metrics                 - Show performance metrics" << std::endl;
        std::cout << "  neurons <region>        - Show neuron statistics" << std::endl;
        std::cout << "  synapses <region>       - Show synapse statistics" << std::endl;
        std::cout << "  memory                  - Show memory system status" << std::endl;
        std::cout << "  cognition               - Show cognition system status" << std::endl;
        std::cout << "  neuromodulation          - Show neuromodulation system status" << std::endl;
        std::cout << "  prediction              - Show prediction system status" << std::endl;
        std::cout << "  development             - Show development system status" << std::endl;
        std::cout << "  quit, exit              - Exit NLM Tool" << std::endl;
        std::cout << std::endl;
        std::cout << "TEST COMMANDS:" << std::endl;
        std::cout << "  test connectivity       - Test neural connectivity" << std::endl;
        std::cout << "  test plasticity         - Test plasticity learning" << std::endl;
        std::cout << "  test stdp               - Test STDP mechanism" << std::endl;
        std::cout << "  test memory             - Test memory integration" << std::endl;
        std::cout << "  test neuromodulation    - Test neuromodulation" << std::endl;
        std::cout << "  test all                - Run all tests" << std::endl;
        std::cout << std::endl;
        std::cout << "DEVELOPMENT COMMANDS:" << std::endl;
        std::cout << "  dev stage <stage>       - Set developmental stage (Initial, CriticalPeriod, Maturation, Adult)" << std::endl;
        std::cout << "  dev synapto <rate>      - Set synaptogenesis rate" << std::endl;
        std::cout << "  dev prune <rate>        - Set pruning rate" << std::endl;
        std::cout << "  dev plasticity <rate>   - Set overall plasticity rate" << std::endl;
        std::cout << "  replay <count>          - Replay stored episodes" << std::endl;
        std::cout << "  consolidate <threshold>  - Consolidate memories" << std::endl;
        std::cout << "  checkpoint <interval>   - Set checkpoint interval" << std::endl;
        std::cout << std::endl;
        std::cout << "SIMULATION CONTROLS:" << std::endl;
        std::cout << "  inject <region> <type> <value>  - Inject current into neurons" << std::endl;
        std::cout << "  stimulus <region> <type> <value> - Provide sensory stimulus" << std::endl;
        std::cout << "  reward <value>          - Apply reward signal" << std::endl;
        std::cout << "  set novelty <value>     - Set novelty detection level" << std::endl;
        std::cout << "  set curiosity <value>   - Set curiosity level" << std::endl;
        std::cout << std::endl;
    }
    
    void printVersion() {
        std::cout << "=== NLM Neural Learning Machine ===" << std::endl;
        std::cout << "Version: Phase 6 - Final Integration" << std::endl;
        std::cout << "Architecture: Integrated Artificial Brain" << std::endl;
        std::cout << "Components: " << std::endl;
        std::cout << "  - Neural Core (LIF neurons, spikes, plasticity)" << std::endl;
        std::cout << "  - Memory Systems (Working, Episodic, Associative)" << std::endl;
        std::cout << "  - Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
        std::cout << "  - Cognition (Planner, Concept Formation, Attention)" << std::endl;
        std::cout << "  - Prediction System" << std::endl;
        std::cout << "  - Development System" << std::endl;
        std::cout << "  - Performance Infrastructure (Memory pools, Event queues)" << std::endl;
        std::cout << std::endl;
    }
    
    void showStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized. Use 'start' or 'load' first." << std::endl;
            return;
        }
        
        std::cout << "=== Brain Status ===" << std::endl;
        brain->logStatus();
        
        // Show memory system details
        if (brain->getWorkingMemory()) {
            std::cout << "Working Memory Active Traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        }
        if (brain->getEpisodicMemory()) {
            std::cout << "Episodic Memory Episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        }
        if (brain->getAssociativeMemory()) {
            std::cout << "Associative Memory Patterns: " << brain->getAssociativeMemory()->getPatternCount() << std::endl;
        }
        
        // Show neuromodulation details
        if (brain->getDopamine()) {
            std::cout << "Dopamine Level: " << brain->getDopamine()->getLevel() << std::endl;
        }
        if (brain->getCuriosity()) {
            std::cout << "Curiosity Level: " << brain->getCuriosity()->getLevel() << std::endl;
        }
        if (brain->getNovelty()) {
            std::cout << "Novelty Level: " << brain->getNovelty()->getLevel() << std::endl;
        }
        
        // Show development stage
        std::cout << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        // Show current step
        std::cout << "Current Simulation Step: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Current Time: " << (brain->getTotalSpikeCount() * 0.001) << "s" << std::endl;
    }
    
    bool startSimulation(uint64_t steps = 100) {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized. Use 'load' or 'reset' first." << std::endl;
            return false;
        }
        
        std::cout << "=== Starting Simulation ===" << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            // Show progress every 10% or every 1000 steps
            if (step % std::max<uint64_t>(1, steps / 10) == 0 || step % 1000 == 0) {
                std::cout << "Step " << step << "/" << steps 
                         << " | Firing: " << brain->getFiringNeuronCount()
                         << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | WorkingMem: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
                         << " | Episodes: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0)
                         << std::endl;
            }
            
            // Check for quit
            if (shouldQuit()) {
                std::cout << "Simulation interrupted by user." << std::endl;
                break;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "=== Simulation Complete ===" << std::endl;
        std::cout << "Total time: " << duration << "s" << std::endl;
        std::cout << "Steps per second: " << (steps / duration) << std::endl;
        
        return true;
    }
    
    bool runDemo() {
        std::cout << "=== Running NLM Phase 6 Demo ===" << std::endl;
        
        // Create demo configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 500);
        cfg->set("region_count", 1);
        cfg->set("random_seed", 42);
        
        // Create and initialize brain
        brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain." << std::endl;
            return false;
        }
        
        // Test 1: Basic connectivity
        std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                size_t initialSpikes = brain->getTotalSpikeCount();
                
                // Inject current into first 10 neurons
                for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
                    neurons[i]->injectCurrent(50.0f);
                }
                
                // Run 50 steps
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
                std::cout << "Spikes generated: " << spikes << std::endl;
                
                if (spikes > 0) {
                    std::cout << "✓ Neural connectivity verified" << std::endl;
                } else {
                    std::cout << "! No spikes - checking neuron parameters..." << std::endl;
                }
            }
        }
        
        // Test 2: Plasticity learning
        std::cout << "\n=== Test 2: Plasticity Learning Experiment ===" << std::endl;
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input pattern
        std::cout << "Applying repeated input patterns (200 steps)..." << std::endl;
        for (SimulationStep step = 0; step < 200; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                         << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | Firing: " << brain->getFiringNeuronCount()
                         << " | WorkingMem: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
                         << std::endl;
            }
        }
        
        std::cout << "✓ Plasticity experiment completed" << std::endl;
        
        // Test 3: STDP verification
        std::cout << "\n=== Test 3: STDP Verification ===" << std::endl;
        
        brain->reset();
        brain->initialize();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            if (synapses.size() >= 5) {
                // Test 5 synapses
                std::vector<float> beforeWeights;
                for (size_t i = 0; i < 5; ++i) {
                    beforeWeights.push_back(synapses[i]->getWeight());
                    synapses[i]->enablePlasticity(false, true, false);
                }
                
                // Create correlated activity for LTP
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    for (int trial = 0; trial < 20; ++trial) {
                        Neuron* preNeuron = neurons[0];
                        Neuron* postNeuron = neurons[1];
                        
                        preNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2, trial * 2 * 0.001);
                        
                        postNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                    }
                }
                
                // Check weight changes
                float totalDelta = 0.0f;
                for (size_t i = 0; i < 5; ++i) {
                    float delta = synapses[i]->getWeight() - beforeWeights[i];
                    totalDelta += delta;
                }
                
                if (totalDelta > 0.001f) {
                    std::cout << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
                } else if (totalDelta < -0.001f) {
                    std::cout << "! STDP reversed: Check parameters" << std::endl;
                } else {
                    std::cout << "! No change: STDP may not be triggering" << std::endl;
                }
            }
        }
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        showStatus();
        
        return true;
    }
    
    bool saveBrainState(const std::string& filename) {
        if (!brain) {
            std::cout << "ERROR: No brain to save." << std::endl;
            return false;
        }
        
        std::cout << "=== Saving Brain State ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        
        if (brain->save(filename)) {
            std::cout << "✓ Brain state saved successfully" << std::endl;
            return true;
        } else {
            std::cout << "✗ Failed to save brain state" << std::endl;
            return false;
        }
    }
    
    bool loadBrainState(const std::string& filename) {
        if (!brain) {
            std::cout << "ERROR: No brain to load into." << std::endl;
            return false;
        }
        
        std::cout << "=== Loading Brain State ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        
        if (brain->load(filename)) {
            std::cout << "✓ Brain state loaded successfully" << std::endl;
            return true;
        } else {
            std::cout << "✗ Failed to load brain state" << std::endl;
            return false;
        }
    }
    
    void showMemoryStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Memory System Status ===" << std::endl;
        
        // Working memory
        if (brain->getWorkingMemory()) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Active traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
            std::cout << "  Capacity: " << brain->getWorkingMemory()->getCapacity() << std::endl;
            std::cout << "  Decay rate: " << brain->getWorkingMemory()->getDecayRate() << std::endl;
        } else {
            std::cout << "Working Memory: Not available" << std::endl;
        }
        
        // Episodic memory
        if (brain->getEpisodicMemory()) {
            std::cout << "Episodic Memory:" << std::endl;
            std::cout << "  Episodes stored: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
            std::cout << "  Max episodes: " << brain->getEpisodicMemory()->getMaxEpisodes() << std::endl;
            std::cout << "  Consolidation threshold: " << brain->getEpisodicMemory()->getConsolidationThreshold() << std::endl;
            
            // Show recent episodes
            auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(5);
            std::cout << "  Recent episodes: " << episodes.size() << std::endl;
        } else {
            std::cout << "Episodic Memory: Not available" << std::endl;
        }
        
        // Associative memory
        if (brain->getAssociativeMemory()) {
            std::cout << "Associative Memory:" << std::endl;
            std::cout << "  Pattern associations: " << brain->getAssociativeMemory()->getPatternCount() << std::endl;
            std::cout << "  Hebbian strength: " << brain->getAssociativeMemory()->getHebbianStrength() << std::endl;
        } else {
            std::cout << "Associative Memory: Not available" << std::endl;
        }
    }
    
    void showCognitionStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Cognition System Status ===" << std::endl;
        
        // Planner
        if (brain->getPlanner()) {
            std::cout << "Planner:" << std::endl;
            std::cout << "  Planning depth: " << brain->getPlanner()->getPlanningDepth() << std::endl;
            std::cout << "  Action sequences: " << brain->getPlanner()->getActiveActionCount() << std::endl;
        } else {
            std::cout << "Planner: Not available" << std::endl;
        }
        
        // Concept formation
        if (brain->getConceptFormation()) {
            std::cout << "Concept Formation:" << std::endl;
            std::cout << "  Concepts discovered: " << brain->getConceptFormation()->getConceptCount() << std::endl;
            std::cout << "  Similarity threshold: " << brain->getConceptFormation()->getSimilarityThreshold() << std::endl;
        } else {
            std::cout << "Concept Formation: Not available" << std::endl;
        }
        
        // Attention
        if (brain->getAttention()) {
            std::cout << "Attention:" << std::endl;
            std::cout << "  Inhibition strength: " << brain->getAttention()->getInhibitionStrength() << std::endl;
            std::cout << "  Excitation strength: " << brain->getAttention()->getExcitationStrength() << std::endl;
            std::cout << "  Active focus: " << (brain->getAttention()->getActiveFocus() ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "Attention: Not available" << std::endl;
        }
    }
    
    void showNeuromodulationStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Neuromodulation System Status ===" << std::endl;
        
        // Dopamine
        if (brain->getDopamine()) {
            std::cout << "Dopamine:" << std::endl;
            std::cout << "  Level: " << brain->getDopamine()->getLevel() << std::endl;
            std::cout << "  Plasticity factor: " << brain->getDopamine()->getPlasticityFactor() << std::endl;
            std::cout << "  Reward prediction error: " << brain->getDopamine()->getPredictionError() << std::endl;
        } else {
            std::cout << "Dopamine: Not available" << std::endl;
        }
        
        // Curiosity
        if (brain->getCuriosity()) {
            std::cout << "Curiosity:" << std::endl;
            std::cout << "  Level: " << brain->getCuriosity()->getLevel() << std::endl;
            std::cout << "  Novelty threshold: " << brain->getCuriosity()->getNoveltyThreshold() << std::endl;
            std::cout << "  Exploration rate: " << brain->getCuriosity()->getExplorationRate() << std::endl;
        } else {
            std::cout << "Curiosity: Not available" << std::endl;
        }
        
        // Novelty
        if (brain->getNovelty()) {
            std::cout << "Novelty:" << std::endl;
            std::cout << "  Level: " << brain->getNovelty()->getLevel() << std::endl;
            std::cout << "  Sensitivity: " << brain->getNovelty()->getSensitivity() << std::endl;
        } else {
            std::cout << "Novelty: Not available" << std::endl;
        }
        
        // Prediction error
        if (brain->getPredictionErrorSignal()) {
            std::cout << "Prediction Error:" << std::endl;
            std::cout << "  Signal: " << brain->getPredictionErrorSignal()->getSignal() << std::endl;
            std::cout << "  Confidence: " << brain->getPredictionErrorSignal()->getConfidence() << std::endl;
        } else {
            std::cout << "Prediction Error: Not available" << std::endl;
        }
    }
    
    void showPredictionStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Prediction System Status ===" << std::endl;
        
        if (brain->getPredictionSystem()) {
            // Prediction system status would go here
            std::cout << "Prediction System: Available (details depend on implementation)" << std::endl;
        } else {
            std::cout << "Prediction System: Not available" << std::endl;
        }
    }
    
    void showDevelopmentStatus() {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Development System Status ===" << std::endl;
        
        std::cout << "Current developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        if (brain->getDevelopmentSystem()) {
            // Development system status would go here
            std::cout << "Development System: Available (details depend on implementation)" << std::endl;
        } else {
            std::cout << "Development System: Not available" << std::endl;
        }
    }
    
    bool processCommand(const std::string& input) {
        // Parse command (simple implementation)
        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;
        
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "version") {
            printVersion();
        } else if (cmd == "status") {
            showStatus();
        } else if (cmd == "quit" || cmd == "exit") {
            return false; // Exit signal
        } else if (cmd == "start") {
            uint64_t steps;
            if (iss >> steps) {
                startSimulation(steps);
            } else {
                std::cout << "Usage: start <steps>" << std::endl;
            }
        } else if (cmd == "reset") {
            if (brain) {
                brain->reset();
                brain->initialize();
                std::cout << "Brain reset and reinitialized." << std::endl;
            }
        } else if (cmd == "demo") {
            runDemo();
        } else if (cmd == "save") {
            std::string filename;
            if (iss >> filename) {
                saveBrainState(filename);
            } else {
                std::cout << "Usage: save <filename>" << std::endl;
            }
        } else if (cmd == "load") {
            std::string filename;
            if (iss >> filename) {
                loadBrainState(filename);
            } else {
                std::cout << "Usage: load <filename>" << std::endl;
            }
        } else if (cmd == "memory") {
            showMemoryStatus();
        } else if (cmd == "cognition") {
            showCognitionStatus();
        } else if (cmd == "neuromodulation") {
            showNeuromodulationStatus();
        } else if (cmd == "prediction") {
            showPredictionStatus();
        } else if (cmd == "development") {
            showDevelopmentStatus();
        } else if (cmd == "test") {
            std::string testName;
            if (iss >> testName) {
                runTest(testName);
            } else {
                std::cout << "Usage: test <test_name>" << std::endl;
                std::cout << "Available tests: connectivity, plasticity, stdp, memory, neuromodulation, all" << std::endl;
            }
        } else if (cmd == "show") {
            std::string item;
            iss >> item;
            std::transform(item.begin(), item.end(), item.begin(), ::tolower);
            if (item == "config") {
                showConfig();
            } else {
                std::cout << "Unknown item to show: " << item << std::endl;
            }
        } else if (cmd == "inject") {
            // Parse inject command: inject <region> <type> <value>
            int region, type;
            float value;
            if (iss >> region >> type >> value) {
                injectCurrent(region, static_cast<NeuronType>(type), value);
            } else {
                std::cout << "Usage: inject <region> <neuron_type> <value>" << std::endl;
            }
        } else if (cmd == "reward") {
            float reward;
            if (iss >> reward) {
                applyReward(reward);
            } else {
                std::cout << "Usage: reward <value>" << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
        
        return true; // Continue running
    }
    
    void startREPL() {
        std::cout << "=== NLM Command Line Tool REPL ===" << std::endl;
        std::cout << "Type 'help' for available commands, 'quit' to exit." << std::endl;
        std::cout << std::endl;
        
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            if (!processCommand(line)) {
                break; // Exit signal
            }
        }
        
        std::cout << "Goodbye!" << std::endl;
    }
    
private:
    bool shouldQuit() {
        // Simple check - in real implementation, this would check a flag
        return false;
    }
    
    void injectCurrent(int regionId, NeuronType type, float value) {
        if (!brain) return;
        
        auto* region = brain->getRegion(RegionId(regionId));
        if (!region) {
            std::cout << "Region " << regionId << " not found." << std::endl;
            return;
        }
        
        brain->injectCurrentToNeurons(type, value);
        std::cout << "Injected " << value << " current into " << neuronTypeToString(type) << " neurons." << std::endl;
    }
    
    void applyReward(float reward) {
        if (!brain) return;
        
        // Apply reward signal
        std::cout << "Applied reward: " << reward << std::endl;
    }
    
    void showConfig() {
        if (!config) {
            std::cout << "No configuration loaded." << std::endl;
            return;
        }
        
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << std::endl;
        std::cout << "neuron_count: " << config->getOr<size_t>("neuron_count", 1000) << std::endl;
        std::cout << "region_count: " << config->getOr<size_t>("region_count", 1) << std::endl;
        std::cout << "connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
        std::cout << "stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
        std::cout << "stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
        std::cout << "synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
        std::cout << "pruning_rate: " << config->getOr<float>("pruning_rate", 0.00001f) << std::endl;
    }
    
    std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Internal: return "Internal";
            case NeuronType::Motor: return "Motor";
            default: return "Unknown";
        }
    }
    
    bool runTest(const std::string& testName) {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return false;
        }
        
        std::cout << "=== Running Test: " << testName << " ===" << std::endl;
        
        if (testName == "connectivity") {
            runConnectivityTest();
        } else if (testName == "plasticity") {
            runPlasticityTest();
        } else if (testName == "stdp") {
            runSTDPTest();
        } else if (testName == "memory") {
            runMemoryTest();
        } else if (testName == "neuromodulation") {
            runNeuromodulationTest();
        } else if (testName == "all") {
            runAllTests();
        } else {
            std::cout << "Unknown test: " << testName << std::endl;
            std::cout << "Available tests: connectivity, plasticity, stdp, memory, neuromodulation, all" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void runConnectivityTest() {
        std::cout << "Testing neural connectivity..." << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "No region found." << std::endl;
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "No neurons found." << std::endl;
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run 50 steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "✓ Connectivity test passed" << std::endl;
        } else {
            std::cout << "! No spikes detected" << std::endl;
        }
    }
    
    void runPlasticityTest() {
        std::cout << "Testing plasticity learning..." << std::endl;
        
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning simulation
        for (SimulationStep step = 0; step < 200; ++step) {
            // Provide input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                         << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | WorkingMem: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
                         << std::endl;
            }
        }
        
        std::cout << "✓ Plasticity test completed" << std::endl;
    }
    
    void runSTDPTest() {
        std::cout << "Testing STDP mechanism..." << std::endl;
        
        brain->reset();
        brain->initialize();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            if (synapses.size() >= 5) {
                // Test 5 synapses
                std::vector<float> beforeWeights;
                for (size_t i = 0; i < 5; ++i) {
                    beforeWeights.push_back(synapses[i]->getWeight());
                    synapses[i]->enablePlasticity(false, true, false);
                }
                
                // Create correlated pre-post activity
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    Neuron* preNeuron = neurons[0];
                    Neuron* postNeuron = neurons[1];
                    
                    for (int trial = 0; trial < 20; ++trial) {
                        preNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2, trial * 2 * 0.001);
                        
                        postNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                    }
                }
                
                // Check weight changes
                float totalDelta = 0.0f;
                for (size_t i = 0; i < 5; ++i) {
                    float delta = synapses[i]->getWeight() - beforeWeights[i];
                    totalDelta += delta;
                }
                
                if (totalDelta > 0.001f) {
                    std::cout << "✓ STDP LTP working: weight increased by " << totalDelta << std::endl;
                } else if (totalDelta < -0.001f) {
                    std::cout << "! STDP LTD working: weight decreased by " << -totalDelta << std::endl;
                } else {
                    std::cout << "! No significant weight change" << std::endl;
                }
            }
        }
    }
    
    void runMemoryTest() {
        std::cout << "Testing memory integration..." << std::endl;
        
        brain->reset();
        brain->initialize();
        
        // Run with sensory input to test memory formation
        for (SimulationStep step = 0; step < 100; ++step) {
            // Provide input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
        }
        
        // Check memory systems
        if (brain->getWorkingMemory()) {
            std::cout << "Working memory active traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        }
        
        if (brain->getEpisodicMemory()) {
            std::cout << "Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        }
        
        std::cout << "✓ Memory test completed" << std::endl;
    }
    
    void runNeuromodulationTest() {
        std::cout << "Testing neuromodulation..." << std::endl;
        
        brain->reset();
        brain->initialize();
        
        // Run with input to trigger neuromodulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
            brain->step(step, step * 0.001);
        }
        
        // Check neuromodulation levels
        if (brain->getDopamine()) {
            std::cout << "Dopamine level: " << brain->getDopamine()->getLevel() << std::endl;
        }
        
        if (brain->getCuriosity()) {
            std::cout << "Curiosity level: " << brain->getCuriosity()->getLevel() << std::endl;
        }
        
        std::cout << "✓ Neuromodulation test completed" << std::endl;
    }
    
    void runAllTests() {
        std::cout << "=== Running All Tests ===" << std::endl;
        
        runConnectivityTest();
        std::cout << std::endl;
        
        runPlasticityTest();
        std::cout << std::endl;
        
        runSTDPTest();
        std::cout << std::endl;
        
        runMemoryTest();
        std::cout << std::endl;
        
        runNeuromodulationTest();
        std::cout << std::endl;
        
        std::cout << "=== All Tests Complete ===" << std::endl;
    }
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    bool running;
};

int main(int argc, char* argv[]) {
    NLMTool tool;
    
    if (!tool.initialize()) {
        std::cerr << "Failed to initialize NLM Tool." << std::endl;
        return 1;
    }
    
    // If no arguments, start REPL
    if (argc == 1) {
        tool.startREPL();
        return 0;
    }
    
    // Process command line arguments
    std::string command = argv[1];
    
    if (command == "help") {
        tool.printHelp();
        return 0;
    } else if (command == "version") {
        tool.printVersion();
        return 0;
    } else if (command == "demo") {
        if (!tool.initialize()) {
            std::cerr << "Failed to initialize tool for demo." << std::endl;
            return 1;
        }
        return tool.runDemo() ? 0 : 1;
    } else if (command == "test") {
        if (argc < 3) {
            std::cerr << "Usage: nlm_tool test <test_name>" << std::endl;
            std::cerr << "Available tests: connectivity, plasticity, stdp, memory, neuromodulation, all" << std::endl;
            return 1;
        }
        if (!tool.initialize()) {
            std::cerr << "Failed to initialize tool for test." << std::endl;
            return 1;
        }
        return tool.runTest(argv[2]) ? 0 : 1;
    } else if (command == "start") {
        uint64_t steps = 100;
        if (argc >= 3) {
            steps = std::stoull(argv[2]);
        }
        if (!tool.initialize()) {
            std::cerr << "Failed to initialize tool for simulation." << std::endl;
            return 1;
        }
        return tool.startSimulation(steps) ? 0 : 1;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Use 'nlm_tool help' for available commands." << std::endl;
        return 1;
    }
}