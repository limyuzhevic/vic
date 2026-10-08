// NLM (熙然) - Neural Learning Machine
// Command-Line Interface for Brain Control
// 
// This provides a comprehensive command-line interface for interacting with
// the NLM artificial brain, allowing users to control, monitor, and experiment
// with the neural system.

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
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace nlm;

// Global brain instance
std::shared_ptr<Brain> g_brain;
std::shared_ptr<Config> g_config;

// Command prototypes
void printWelcome();
void printHelp();
void printBrainStatus();
void printStatistics();
void runExperiment(std::string experimentName);
void saveBrain(std::string filepath);
void loadBrain(std::string filepath);
void resetBrain();
void stepBrain(int steps);
void injectInput(std::vector<float> input);
void printMemoryStatus();
void printNeuromodulationStatus();
void setParameter(std::string name, std::string value);
void printParameter(std::string name);
void runInteractiveMode();
void runScript(std::string filename);

// Command parsing
void parseCommand(const std::string& line);
std::vector<std::string> splitString(const std::string& str, char delimiter);
std::string toLowerCase(const std::string& str);

// Main CLI entry point
int main(int argc, char** argv) {
    printWelcome();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    g_config = std::make_shared<Config>();
    
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (g_config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    g_brain = std::make_shared<Brain>(g_config);
    
    if (!g_brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    NLM_LOG_INFO("NLM Brain ready. Type 'help' for commands.");
    
    // Handle command-line commands
    if (argc > 1) {
        std::string cmd = argv[1];
        
        if (cmd == "run") {
            // Run a specific experiment
            if (argc > 2) {
                runExperiment(argv[2]);
            } else {
                std::cout << "Usage: nlm run <experiment_name>" << std::endl;
                std::cout << "Experiments: connectivity, plasticity, stdp, learning, demo" << std::endl;
            }
        } else if (cmd == "save") {
            // Save brain state
            std::string filepath = "brain_state.checkpoint";
            if (argc > 2) filepath = argv[2];
            saveBrain(filepath);
        } else if (cmd == "load") {
            // Load brain state
            std::string filepath = "brain_state.checkpoint";
            if (argc > 2) filepath = argv[2];
            loadBrain(filepath);
        } else if (cmd == "step") {
            // Run simulation for a few steps
            int steps = 100;
            if (argc > 2) steps = std::stoi(argv[2]);
            stepBrain(steps);
        } else if (cmd == "interactive") {
            // Enter interactive mode
            runInteractiveMode();
        } else if (cmd == "script") {
            // Run script file
            if (argc > 2) runScript(argv[2]);
            else std::cout << "Usage: nlm script <filename>" << std::endl;
        } else if (cmd == "help") {
            printHelp();
        } else if (cmd == "status") {
            printBrainStatus();
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            printHelp();
        }
    } else {
        // Enter interactive mode by default
        runInteractiveMode();
    }
    
    return 0;
}

void printWelcome() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "\nNLM Brain Command-Line Interface" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;
    
    std::cout << "Commands:" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << "  status                 Show brain status" << std::endl;
    std::cout << "  stats                  Show detailed statistics" << std::endl;
    std::cout << "  run <experiment>       Run a specific experiment" << std::endl;
    std::cout << "  save <file>            Save brain state to file" << std::endl;
    std::cout << "  load <file>            Load brain state from file" << std::endl;
    std::cout << "  reset                  Reset brain to initial state" << std::endl;
    std::cout << "  step <steps>            Run simulation for specified steps" << std::endl;
    std::cout << "  input <values>         Inject sensory input (comma-separated)" << std::endl;
    std::cout << "  memory                 Show memory system status" << std::endl;
    std::cout << "  neuromod                Show neuromodulation status" << std::endl;
    std::cout << "  set <key> <value>      Set configuration parameter" << std::endl;
    std::cout << "  get <key>              Get configuration parameter value" << std::endl;
    std::cout << "  interactive            Enter interactive mode (default)" << std::endl;
    std::cout << "  script <file>          Run commands from script file" << std::endl;
    std::cout << "  exit                   Exit program" << std::endl;
    std::cout << "  quit                   Exit program" << std::endl << std::endl;
    
    std::cout << "Experiments:" << std::endl;
    std::cout << "  connectivity          Test basic neural connectivity" << std::endl;
    std::cout << "  plasticity            Test synaptic plasticity and learning" << std::endl;
    std::cout << "  stdp                  Test Spike-Timing-Dependent Plasticity" << std::endl;
    std::cout << "  learning              Run learning experiment" << std::endl;
    std::cout << "  demo                  Run Phase 6 integration demo" << std::endl;
    std::cout << "  ablation              Run ablation study" << std::endl;
    std::cout << "  benchmark             Run performance benchmark" << std::endl << std::endl;
}

void printHelp() {
    std::cout << "\n=== NLM Brain Command-Line Interface Help ===\n" << std::endl;
    
    std::cout << "Basic Commands:" << std::endl;
    std::cout << "  help                 Show this help message" << std::endl;
    std::cout << "  status              Display current brain status and statistics" << std::endl;
    std::cout << "  stats               Show detailed performance statistics" << std::endl;
    std::cout << "  exit|quit           Exit the program" << std::endl << std::endl;
    
    std::cout << "Brain Control Commands:" << std::endl;
    std::cout << "  reset              Reset brain to initial state" << std::endl;
    std::cout << "  step [n]           Run simulation for n steps (default: 100)" << std::endl;
    std::cout << "  save <file>        Save current brain state to checkpoint file" << std::endl;
    std::cout << "  load <file>        Load brain state from checkpoint file" << std::endl;
    std::cout << "  inject <values>    Inject sensory input (comma-separated, e.g., '0.5,0.3,0.8')" << std::endl << std::endl;
    
    std::cout << "Memory System Commands:" << std::endl;
    std::cout << "  memory             Show working memory, episodic memory status" << std::endl;
    std::cout << "  episodic           Show episodic memory details" << std::endl;
    std::cout << "  working            Show working memory details" << std::endl;
    std::cout << "  consolidate        Manually consolidate memories" << std::endl;
    std::cout << "  replay             Replay recent episodes" << std::endl << std::endl;
    
    std::cout << "Neuromodulation Commands:" << std::endl;
    std::cout << "  neuromod            Show dopamine, curiosity, novelty levels" << std::endl;
    std::cout << "  set-dopamine <val>  Set dopamine level (0.0-1.0)" << std::endl;
    std::cout << "  set-curiosity <val> Set curiosity level (0.0-1.0)" << std::endl;
    std::cout << "  set-novelty <val>   Set novelty detection sensitivity" << std::endl << std::endl;
    
    std::cout << "Configuration Commands:" << std::endl;
    std::cout << "  set <key> <value>   Set configuration parameter" << std::endl;
    std::cout << "  get <key>           Get configuration parameter value" << std::endl;
    std::cout << "  config              Show all configuration parameters" << std::endl << std::endl;
    
    std::cout << "Experiment Commands:" << std::endl;
    std::cout << "  run <name>          Run specific experiment" << std::endl;
    std::cout << "  demo                Run Phase 6 integration demo" << std::endl;
    std::cout << "  benchmark           Run performance benchmark" << std::endl;
    std::cout << "  ablation            Run ablation study" << std::endl << std::endl;
    
    std::cout << "Advanced Commands:" << std::endl;
    std::cout << "  interactive         Enter interactive mode" << std::endl;
    std::cout << "  script <file>      Run commands from script file" << std::endl;
    std::cout << "  loglevel <level>   Set logging level (trace, debug, info, warn, error, critical)" << std::endl;
    std::cout << "  clearmem           Clear working memory" << std::endl;
    std::cout << "  development        Apply development effects" << std::endl << std::endl;
    
    std::cout << "Parameter Examples:" << std::endl;
    std::cout << "  set neuron_count 2000" << std::endl;
    std::cout << "  set simulation_timestep 0.0005" << std::endl;
    std::cout << "  set connection_probability 0.2" << std::endl;
    std::cout << "  set stdp_ltp_weight 0.025" << std::endl;
    std::cout << "  get neuron_count" << std::endl << std::endl;
}

void printBrainStatus() {
    std::cout << "\n=== NLM Brain Status ===\n" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    g_brain->logStatus();
    std::cout << std::endl;
}

void printStatistics() {
    std::cout << "\n=== Brain Performance Statistics ===\n" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    std::cout << "Neuron Statistics:" << std::endl;
    std::cout << "  Total neurons: " << g_brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Active neurons: " << g_brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons (this step): " << g_brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << g_brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Excitation/Inhibition ratio: " << g_brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Synapse Statistics:" << std::endl;
    std::cout << "  Total synapses: " << g_brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Pending spike events: " << g_brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "  Total spike count: " << g_brain->getTotalSpikeCount() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Memory Statistics:" << std::endl;
    auto* workingMem = g_brain->getWorkingMemory();
    if (workingMem) {
        std::cout << "  Working memory traces: " << workingMem->getActiveTraces() << std::endl;
    }
    
    auto* episodicMem = g_brain->getEpisodicMemory();
    if (episodicMem) {
        std::cout << "  Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
    }
    std::cout << std::endl;
}

void runExperiment(std::string experimentName) {
    std::cout << "\n=== Running Experiment: " << experimentName << " ===\n" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    if (experimentName == "connectivity") {
        std::cout << "Running basic connectivity test..." << std::endl;
        // Inject current into a few neurons and see if spikes propagate
        auto* region = g_brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                std::cout << "  No neurons found!" << std::endl;
                return;
            }
            
            // Get initial spike count
            size_t initialSpikes = g_brain->getTotalSpikeCount();
            
            // Inject strong current into first 10 neurons
            std::cout << "  Injecting current into 10 neurons..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
            }
            
            // Run a few steps
            for (SimulationStep step = 0; step < 50; ++step) {
                g_brain->step(step, step * 0.001);
            }
            
            size_t spikes = g_brain->getTotalSpikeCount() - initialSpikes;
            std::cout << "  Spikes generated: " << spikes << std::endl;
            
            if (spikes > 0) {
                std::cout << "  ✓ Spikes propagate through network" << std::endl;
            } else {
                std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
            }
        }
    } else if (experimentName == "plasticity") {
        std::cout << "Running plasticity learning experiment..." << std::endl;
        
        // Reset brain
        g_brain->reset();
        g_brain->initialize();
        
        // Enable plasticity on synapses
        if (auto* region = g_brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        std::cout << "  Applying repeated input patterns (1000 steps)..." << std::endl;
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < g_brain->getTotalNeuronCount() / 4; ++i) {
                g_brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            g_brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "    Step " << step << 
                    " | Spikes: " << g_brain->getTotalSpikeCount() <<
                    " | Firing: " << g_brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "  ✓ Plasticity experiment complete" << std::endl;
        std::cout << "  Final spikes: " << g_brain->getTotalSpikeCount() << std::endl;
    } else if (experimentName == "stdp") {
        std::cout << "Running STDP verification..." << std::endl;
        
        auto* region = g_brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  No region found!" << std::endl;
            return;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            std::cout << "  Not enough synapses for STDP test" << std::endl;
            return;
        }
        
        std::cout << "  Testing STDP on 5 synapses:" << std::endl;
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        std::cout << "\n  Creating correlated pre->post activity (potentiation)..." << std::endl;
        
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
                g_brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                g_brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        std::cout << "  After correlated activity:" << std::endl;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            std::cout << "    Synapse " << i << " after: " << synapses[i]->getWeight()
                << " (Δ=" << delta << ")" << std::endl;
        }
        
        // Check if weights increased (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "\n  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "\n  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "\n  ! No change: STDP may not be triggering" << std::endl;
        }
    } else if (experimentName == "learning") {
        std::cout << "Running comprehensive learning experiment..." << std::endl;
        
        // Record initial state
        size_t initialSynapses = g_brain->getTotalSynapseCount();
        std::cout << "  Initial synapses: " << initialSynapses << std::endl;
        
        // Enable plasticity
        if (auto* region = g_brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning stimuli
        for (SimulationStep step = 0; step < 500; ++step) {
            // Random stimulation
            if (rand() % 100 < 10) {  // 10% chance
                g_brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
            
            g_brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                std::cout << "    Step " << step << 
                    " | Spikes: " << g_brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        size_t finalSynapses = g_brain->getTotalSynapseCount();
        std::cout << "  Final synapses: " << finalSynapses << std::endl;
        std::cout << "  Total spikes: " << g_brain->getTotalSpikeCount() << std::endl;
        std::cout << "  ✓ Learning experiment complete" << std::endl;
    } else if (experimentName == "demo") {
        std::cout << "Running Phase 6 integration demo..." << std::endl;
        std::cout << "This demonstrates the complete integrated brain:
 - Memory systems working together
 - Neuromodulation affecting learning
 - Prediction and cognition integrated
 - Development affecting plasticity
 - Replay and consolidation functional
" << std::endl;
        
        // Run demo sequence
        for (SimulationStep step = 0; step < 200; ++step) {
            // Natural stimulus pattern
            if (step % 50 == 0) {
                std::vector<float> input(3, 0.5f);
                input[step % 3] = 1.0f;
                injectInput(input);
            }
            
            g_brain->step(step, step * 0.001);
            
            if (step % 50 == 49) {
                std::cout << "    Step " << step << 
                    " | Active neurons: " << g_brain->getActiveNeuronCount() <<
                    " | Spikes: " << g_brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "\n  ✓ Phase 6 demo complete" << std::endl;
        printBrainStatus();
    } else {
        std::cout << "Unknown experiment: " << experimentName << std::endl;
        std::cout << "Available experiments: connectivity, plasticity, stdp, learning, demo" << std::endl;
    }
}

void saveBrain(std::string filepath) {
    std::cout << "\nSaving brain state to: " << filepath << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    if (g_brain->save(filepath)) {
        std::cout << "✓ Brain state saved successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to save brain state" << std::endl;
    }
}

void loadBrain(std::string filepath) {
    std::cout << "\nLoading brain state from: " << filepath << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    if (g_brain->load(filepath)) {
        std::cout << "✓ Brain state loaded successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to load brain state" << std::endl;
    }
}

void resetBrain() {
    std::cout << "\nResetting brain to initial state..." << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    g_brain->reset();
    std::cout << "✓ Brain reset complete" << std::endl;
}

void stepBrain(int steps) {
    std::cout << "\nRunning simulation for " << steps << " steps..." << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    for (SimulationStep step = 0; step < steps; ++step) {
        g_brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << 
                " | Active: " << g_brain->getActiveNeuronCount() <<
                " | Firing: " << g_brain->getFiringNeuronCount() <<
                " | Spikes: " << g_brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "\n✓ Simulation complete" << std::endl;
    printBrainStatus();
}

void injectInput(std::vector<float> input) {
    std::cout << "\nInjecting sensory input: [";
    for (size_t i = 0; i < input.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << input[i];
    }
    std::cout << "]" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    auto sensoryInput = std::make_unique<SensoryInput>(input);
    g_brain->receiveSensoryInput(*sensoryInput);
    std::cout << "✓ Input injected" << std::endl;
}

void printMemoryStatus() {
    std::cout << "\n=== Memory System Status ===\n" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    // Working memory
    auto* workingMem = g_brain->getWorkingMemory();
    if (workingMem) {
        std::cout << "Working Memory:" << std::endl;
        std::cout << "  Active traces: " << workingMem->getActiveTraces() << std::endl;
        std::cout << "  Capacity: " << workingMem->getCapacity() << std::endl;
    } else {
        std::cout << "Working Memory: Not initialized" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Episodic memory
    auto* episodicMem = g_brain->getEpisodicMemory();
    if (episodicMem) {
        std::cout << "Episodic Memory:" << std::endl;
        std::cout << "  Episodes stored: " << episodicMem->getEpisodeCount() << std::endl;
        std::cout << "  Max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
    } else {
        std::cout << "Episodic Memory: Not initialized" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Associative memory
    auto* associativeMem = g_brain->getAssociativeMemory();
    if (associativeMem) {
        std::cout << "Associative Memory: Available" << std::endl;
    }
}

void printNeuromodulationStatus() {
    std::cout << "\n=== Neuromodulation System Status ===\n" << std::endl;
    
    if (!g_brain) {
        std::cout << "Brain not initialized!" << std::endl;
        return;
    }
    
    // Dopamine
    auto* dopamine = g_brain->getDopamine();
    if (dopamine) {
        std::cout << "Dopamine:" << std::endl;
        std::cout << "  Level: " << dopamine->getLevel() << std::endl;
        std::cout << "  Plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
    } else {
        std::cout << "Dopamine: Not initialized" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Curiosity
    auto* curiosity = g_brain->getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity:" << std::endl;
        std::cout << "  Level: " << curiosity->getLevel() << std::endl;
    } else {
        std::cout << "Curiosity: Not initialized" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Novelty
    auto* novelty = g_brain->getNovelty();
    if (novelty) {
        std::cout << "Novelty:" << std::endl;
        std::cout << "  Detection level: " << novelty->getDetectionLevel() << std::endl;
    } else {
        std::cout << "Novelty: Not initialized" << std::endl;
    }
    
    std::cout << std::endl;
}

void setParameter(std::string name, std::string value) {
    std::cout << "\nSetting parameter: " << name << " = " << value << std::endl;
    
    if (!g_config) {
        std::cout << "Configuration not initialized!" << std::endl;
        return;
    }
    
    try {
        // Try to parse as float first
        float fValue = std::stof(value);
        g_config->set(name, fValue, ConfigSource::User);
        std::cout << "✓ Parameter set successfully (float)" << std::endl;
    } catch (...) {
        try {
            // Try to parse as integer
            int64_t iValue = std::stoll(value);
            g_config->set(name, iValue, ConfigSource::User);
            std::cout << "✓ Parameter set successfully (integer)" << std::endl;
        } catch (...) {
            try {
                // Try to parse as string
                g_config->set(name, value, ConfigSource::User);
                std::cout << "✓ Parameter set successfully (string)" << std::endl;
            } catch (...) {
                std::cout << "✗ Failed to set parameter: invalid value format" << std::endl;
            }
        }
    }
}

void printParameter(std::string name) {
    std::cout << "\nGetting parameter: " << name << std::endl;
    
    if (!g_config) {
        std::cout << "Configuration not initialized!" << std::endl;
        return;
    }
    
    auto value = g_config->get<std::string>(name);
    if (value) {
        std::cout << name << " = " << *value << std::endl;
    } else {
        std::cout << "Parameter not found" << std::endl;
    }
}

void runInteractiveMode() {
    std::cout << "\n=== Interactive Mode ===\n" << std::endl;
    std::cout << "Type 'help' for available commands, 'exit' to quit.\n" << std::endl;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip empty lines and comments
        }
        
        std::string command = toLowerCase(line);
        
        if (command == "exit" || command == "quit") {
            std::cout << "Exiting..." << std::endl;
            break;
        } else if (command == "help") {
            printHelp();
        } else if (command == "status") {
            printBrainStatus();
        } else if (command == "stats") {
            printStatistics();
        } else if (command == "reset") {
            resetBrain();
        } else if (command.find("step ") == 0) {
            std::string stepsStr = line.substr(5);
            try {
                int steps = std::stoi(stepsStr);
                stepBrain(steps);
            } catch (...) {
                std::cout << "Invalid step count. Usage: step <steps>" << std::endl;
            }
        } else if (command.find("save ") == 0) {
            std::string filepath = line.substr(5);
            saveBrain(filepath);
        } else if (command.find("load ") == 0) {
            std::string filepath = line.substr(5);
            loadBrain(filepath);
        } else if (command.find("inject ") == 0) {
            std::string inputStr = line.substr(7);
            std::vector<float> input = parseCommand(inputStr);
            injectInput(input);
        } else if (command == "memory") {
            printMemoryStatus();
        } else if (command == "neuromod") {
            printNeuromodulationStatus();n        } else if (command.find("set ") == 0) {
            std::vector<std::string> parts = splitString(line, ' ');
            if (parts.size() >= 3) {
                std::string key = parts[1];
                std::string value = parts[2];
                for (size_t i = 3; i < parts.size(); ++i) {
                    value += " " + parts[i];
                }
                setParameter(key, value);
            } else {
                std::cout << "Usage: set <key> <value>" << std::endl;
            }
        } else if (command.find("get ") == 0) {
            std::vector<std::string> parts = splitString(line, ' ');
            if (parts.size() >= 2) {
                std::string key = parts[1];
                printParameter(key);
            } else {
                std::cout << "Usage: get <key>" << std::endl;
            }
        } else if (command == "config") {
            std::cout << "Configuration parameters:" << std::endl;
            // Print all config values
            if (g_config) {
                auto params = g_config->getAllParameters();
                for (const auto& param : params) {
                    std::cout << "  " << param.first << " = " << param.second << std::endl;
                }
            }
        } else if (command.find("run ") == 0) {
            std::string experiment = line.substr(4);
            runExperiment(experiment);
        } else if (command == "demo") {
            runExperiment("demo");
        } else if (command == "benchmark") {
            runExperiment("benchmark");
        } else if (command == "ablation") {
            runExperiment("ablation");
        } else {
            std::cout << "Unknown command: " << line << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
        
        std::cout << std::endl;
    }
}

void runScript(std::string filename) {
    std::cout << "\nRunning script: " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file) {
        std::cout << "✗ Cannot open script file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        std::string trimmed = line;
        size_t commentPos = trimmed.find('#');
        if (commentPos != std::string::npos) {
            trimmed = trimmed.substr(0, commentPos);
        }
        
        // Remove leading/trailing whitespace
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        if (!trimmed.empty()) {
            std::cout << "Script line " << lineNumber << ": " << line << std::endl;
            parseCommand(trimmed);
        }
    }
    
    file.close();
    std::cout << "\n✓ Script execution complete" << std::endl;
}

void parseCommand(const std::string& line) {
    std::string command = toLowerCase(line);
    
    // Parse based on command type
    if (command.find("step ") == 0) {
        std::string stepsStr = line.substr(5);
        try {
            int steps = std::stoi(stepsStr);
            stepBrain(steps);
        } catch (...) {
            std::cout << "Error: Invalid step count" << std::endl;
        }
    } else if (command.find("inject ") == 0) {
        std::string inputStr = line.substr(7);
        std::vector<std::string> values = splitString(inputStr, ',');
        std::vector<float> input;
        for (const auto& val : values) {
            try {
                input.push_back(std::stof(val));
            } catch (...) {
                std::cout << "Warning: Invalid input value ignored" << std::endl;
            }
        }
        injectInput(input);
    } else if (command.find("set ") == 0) {
        std::vector<std::string> parts = splitString(line, ' ');
        if (parts.size() >= 3) {
            std::string key = parts[1];
            std::string value = parts[2];
            for (size_t i = 3; i < parts.size(); ++i) {
                value += " " + parts[i];
            }
            setParameter(key, value);
        }
    } else if (command.find("get ") == 0) {
        std::vector<std::string> parts = splitString(line, ' ');
        if (parts.size() >= 2) {
            std::string key = parts[1];
            printParameter(key);
        }
    } else {
        std::cout << "Unknown command: " << line << std::endl;
    }
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}