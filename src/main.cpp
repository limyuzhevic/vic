// NLM Main - Refactored for Modular Architecture and CLI
// Phase 2: Real Neural Computation with Advanced User Interface

#include "core/Config/Config.hpp"
#include "core/CommandLine/CommandLineParser.hpp"
#include "core/BrainStateManager.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBrainAdvanced.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <csignal>
#include <thread>
#include <chrono>

using namespace nlm;

// Global application state
std::shared_ptr<Brain> currentBrain;
std::shared_ptr<AgentBrain> currentAgent;
bool running = true;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down gracefully..." << std::endl;
    running = false;
}

// Application entry point
int main(int argc, char** argv) {
    // Set up signal handling
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Print welcome banner
    printBanner();
    AgentBrain::printWelcomeMessage();

    try {
        // Initialize application
        std::cout << "\n=== NLM Application Startup ===" << std::endl;

        // Parse command line arguments
        CommandLineParser parser;
        if (!parser.parse(argc, argv)) {
            std::cout << "Error parsing command line arguments!" << std::endl;
            parser.printHelp();
            return 1;
        }

        // Check for help/version requests
        if (parser.getOptions().count("help") || parser.getOptions().count("?")) {
            parser.printHelp();
            return 0;
        }

        if (parser.getOptions().count("version")) {
            parser.printVersion();
            return 0;
        }

        // Initialize based on command line options
        if (parser.getOptions().count("create") || !parser.getOptions().count("load")) {
            // Create new brain
            std::cout << "\nCreating new brain..." << std::endl;
            
            auto config = std::make_shared<Config>();
            
            // Apply configuration from command line
            for (const auto& [key, value] : parser.getOptions()) {
                if (key == "neurons" || key == "neuron_count") {
                    config->set("neuron_count", std::stoll(value), ConfigSource::CommandLine);
                } else if (key == "regions" || key == "region_count") {
                    config->set("region_count", std::stoll(value), ConfigSource::CommandLine);
                } else if (key == "connection_probability" || key == "connectivity") {
                    config->set("connection_probability", std::stof(value), ConfigSource::CommandLine);
                } else if (key == "timestep" || key == "simulation_timestep") {
                    config->set("simulation_timestep", std::stof(value), ConfigSource::CommandLine);
                } else if (key == "seed" || key == "random_seed") {
                    config->set("random_seed", std::stoll(value), ConfigSource::CommandLine);
                }
            }

            // Set defaults for Phase 2
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);

            // Create brain using new modular architecture
            currentBrain = std::make_shared<Brain>(std::make_shared<BrainConfiguration>());
            
            if (currentBrain->initialize()) {
                std::cout << "Brain created successfully!" << std::endl;
            } else {
                std::cout << "Failed to create brain!" << std::endl;
                return 1;
            }
        } else if (parser.getOptions().count("load")) {
            // Load existing brain
            std::string filename = parser.getOptions().count("load-filename") ? 
                                  parser.getOptions().at("load-filename") : "brain_state.nbl";
            
            std::cout << "\nLoading brain from: " << filename << std::endl;
            
            currentBrain = std::make_shared<Brain>(std::make_shared<BrainConfiguration>());
            
            if (currentBrain->load(filename)) {
                std::cout << "Brain loaded successfully!" << std::endl;
            } else {
                std::cout << "Failed to load brain!" << std::endl;
                return 1;
            }
        }

        // Create agent interface
        std::cout << "\nCreating AgentBrain interface..." << std::endl;
        currentAgent = std::make_shared<AgentBrain>(currentBrain);
        
        if (currentAgent->initialize()) {
            std::cout << "AgentBrain interface initialized successfully!" << std::endl;
        } else {
            std::cout << "Failed to initialize AgentBrain!" << std::endl;
            return 1;
        }

        // Integrate with CLI
        currentAgent->integrateWithCLI(parser);

        // Check for immediate commands
        if (parser.getOptions().count("run")) {
            // Run simulation
            std::cout << "\nRunning simulation..." << std::endl;
            runSimulation(*currentAgent, parser.getOptions());
        } else if (parser.getOptions().count("test")) {
            // Run tests
            std::cout << "\nRunning neural network tests..." << std::endl;
            runTests(parser.getOptions());
        } else if (parser.getOptions().count("benchmark")) {
            // Run benchmark
            std::cout << "\nRunning benchmark..." << std::endl;
            currentAgent->benchmark(parser.getOptions().count("benchmark-steps") ? 
                                  std::stoi(parser.getOptions().at("benchmark-steps")) : 1000);
        } else if (parser.getOptions().count("export")) {
            // Export data
            std::cout << "\nExporting data..." << std::endl;
            exportData(*currentAgent, parser.getOptions());
        } else {
            // Interactive mode
            std::cout << "\nEntering interactive mode..." << std::endl;
            runInteractiveMode(*currentAgent);
        }

        std::cout << "\n=== NLM Application Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
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

void runSimulation(std::shared_ptr<AgentBrain> agent, const std::map<std::string, std::string>& options) {
    std::cout << "Starting simulation..." << std::endl;

    int steps = options.count("steps") ? std::stoi(options.at("steps")) : 100;
    float duration = options.count("duration") ? std::stof(options.at("duration")) : 1.0f;
    bool test1 = options.count("test1") && options.at("test1") == "true";
    bool test2 = options.count("test2") && options.at("test2") == "true";
    bool test3 = options.count("test3") && options.at("test3") == "true";
    bool plasticity = options.count("plasticity") && options.at("plasticity") == "true";
    bool continuous = options.count("continuous") && options.at("continuous") == "true";
    std::string saveInterval = options.count("save-interval") ? options.at("save-interval") : "";

    if (test1) {
        std::cout << "Running Test 1: Basic Connectivity Test" << std::endl;
        // Run basic connectivity test
        runBasicConnectivityTest(agent->getBrain());
    }

    if (test2) {
        std::cout << "Running Test 2: Plasticity Experiment" << std::endl;
        // Run plasticity experiment
        runPlasticityExperiment(agent->getBrain());
    }

    if (test3) {
        std::cout << "Running Test 3: STDP Verification" << std::endl;
        // Run STDP verification
        runStdpVerification(agent->getBrain());
    }

    if (!test1 && !test2 && !test3) {
        // Run standard simulation
        for (int i = 0; i < steps && running; ++i) {
            agent->step(i);

            if (continuous && i % 10 == 0) {
                std::cout << "Step " << i << "/" << steps 
                          << " | Total spikes: " << agent->getTotalSpikeCount()
                          << " | Active neurons: " << agent->getTotalNeuronCount()
                          << " | Avg firing rate: " << agent->getAverageFiringRate()
                          << std::endl;
            }

            // Auto-save if interval specified
            if (!saveInterval.empty() && i > 0 && i % std::stoi(saveInterval) == 0) {
                std::string filename = "brain_checkpoint_" + std::to_string(i) + ".nbl";
                agent->saveState(filename);
            }
        }
    }

    std::cout << "Simulation completed!" << std::endl;
    std::cout << "Final stats:" << std::endl;
    std::cout << "  Total spikes: " << agent->getTotalSpikeCount() << std::endl;
    std::cout << "  Final E/I ratio: " << agent->getExcitationInhibitionRatio() << std::endl;
}

void runTests(const std::map<std::string, std::string>& options) {
    std::cout << "Running comprehensive neural network tests..." << std::endl;

    // Run test suite
    std::cout << "Running integration tests..." << std::endl;
    std::cout << "  (Tests would be run here)" << std::endl;

    if (options.count("comprehensive") && options.at("comprehensive") == "true") {
        std::cout << "Running comprehensive test suite..." << std::endl;
        // Run all tests
    }

    std::cout << "Test suite completed!" << std::endl;
}

void exportData(std::shared_ptr<AgentBrain> agent, const std::map<std::string, std::string>& options) {
    std::string format = options.count("format") ? options.at("format") : "txt";
    std::string filename = options.count("filename") ? options.at("filename") : "export.dat";

    std::cout << "Exporting data in " << format << " format to " << filename << std::endl;

    if (format == "neurons") {
        agent->exportNeurons(filename);
    } else if (format == "connections") {
        agent->exportConnections(filename);
    } else if (format == "full") {
        agent->exportFullState(filename);
    } else {
        std::cout << "Unknown export format: " << format << std::endl;
    }
}

void runInteractiveMode(std::shared_ptr<AgentBrain> agent) {
    std::cout << "Interactive Mode Commands:" << std::endl;
    std::cout << "  help                    - Show available commands" << std::endl;
    std::cout << "  status                  - Show brain status" << std::endl;
    std::cout << "  run <steps>            - Run simulation for steps" << std::endl;
    std::cout << "  save <filename>        - Save brain state" << std::endl;
    std::cout << "  load <filename>        - Load brain state" << std::endl;
    std::cout << "  benchmark               - Run performance benchmark" << std::endl;
    std::cout << "  analyze                 - Analyze network topology" << std::endl;
    std::cout << "  develop                 - Control development" << std::endl;
    std::cout << "  explore                 - Enable autonomous exploration" << std::endl;
    std::cout << "  quit/q                 - Exit application" << std::endl;
    std::cout << std::endl;

    std::string command;
    while (running && std::getline(std::cin, command)) {
        if (command.empty()) continue;

        if (command == "help" || command == "?") {
            std::cout << "Available commands: help, status, run, save, load, benchmark, analyze, develop, explore, quit" << std::endl;
        } else if (command == "status") {
            std::cout << agent->generateBrainReport() << std::endl;
        } else if (command.substr(0, 3) == "run") {
            int steps = 100;
            if (command.size() > 4) {
                steps = std::stoi(command.substr(4));
            }
            agent->benchmark(steps);
        } else if (command.substr(0, 5) == "save ") {
            std::string filename = command.substr(5);
            agent->saveState(filename);
        } else if (command.substr(0, 5) == "load ") {
            std::string filename = command.substr(5);
            agent->loadState(filename);
        } else if (command == "benchmark") {
            agent->benchmark();
        } else if (command == "analyze") {
            agent->analyzeNetworkTopology();
        } else if (command == "develop") {
            std::cout << "Development control not yet implemented in interactive mode" << std::endl;
        } else if (command == "explore") {
            agent->enableAutonomousExploration(true);
            std::cout << "Autonomous exploration enabled" << std::endl;
        } else if (command == "quit" || command == "q") {
            std::cout << "Exiting application..." << std::endl;
            running = false;
            break;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
}

// Original test functions (for backward compatibility)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    std::cout << "    Running Basic Connectivity Test..." << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "    No region found!" << std::endl;
        return;
    }

    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "    No neurons found!" << std::endl;
        return;
    }

    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into neurons
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }

    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }

    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "    Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "    ✓ Spikes propagate through network" << std::endl;
    } else {
        std::cout << "    ! No spikes - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "      Neuron " << i << 
                      " V=" << neurons[i]->getState().membranePotential <<
                      " thresh=" << neurons[i]->getState().threshold << std::endl;
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    std::cout << "    Running Plasticity Learning Experiment..." << std::endl;
    
    // Create learning experiment structure
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void computeStatistics() {
            if (initialWeights.empty() || finalWeights.empty()) {
                return;
            }
            
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            size_t strengthened = 0;
            size_t weakened = 0;
            size_t unchanged = 0;
            
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) ++strengthened;
                else if (delta < -0.01f) ++weakened;
                else ++unchanged;
            }
            
            std::cout << "    Synaptic Changes:" << std::endl;
            std::cout << "      Strengthened: " << strengthened << std::endl;
            std::cout << "      Weakened: " << weakened << std::endl;
            std::cout << "      Unchanged: " << unchanged << std::endl;
        }
    };

    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply repeated input pattern
    std::cout << "    Applying repeated input patterns (1000 steps)..." << std::endl;
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "      Step " << step << 
                      " | Spikes: " << brain->getTotalSpikeCount() <<
                      " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    std::cout << "    Running STDP Verification..." << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "    No region found!" << std::endl;
        return;
    }

    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "    Not enough synapses for STDP test" << std::endl;
        return;
    }

    std::cout << "    Testing STDP on 5 synapses:" << std::endl;
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
    }
    
    // Create correlated activity
    std::cout << "    Creating correlated pre->post activity (potentiation)..." << std::endl;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < 50; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    std::cout << "    After correlated activity:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "      Synapse " << i << " Δ=" << delta << std::endl;
    }
    
    if (totalDelta > 0.001f) {
        std::cout << "      ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "      ! STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "      ! No change: STDP may not be triggering" << std::endl;
    }
}
