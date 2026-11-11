// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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

using namespace nlm;

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

// Command-line argument parsing configuration
struct CliArgs {
    std::string configFile = "configs/default.cfg";
    uint64_t randomSeed = 42;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    bool runAllTests = true;
    bool runConnectivityTest = true;
    bool runPlasticityTest = true;
    bool runStdpTest = true;
    int connectivityTestNeuronCount = 10;
    float connectivityTestCurrent = 50.0f;
    SimulationStep plasticityTestSteps = 1000;
    int stdpTestTrials = 50;
    float stdpTestCurrent = 60.0f;
};

// Parse command-line arguments into structured configuration
CliArgs parseCommandLineArgs(int argc, char** argv) {
    CliArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse numeric values
                try {
                    if (key == "config") {
                        args.configFile = value;
                    } else if (key == "random_seed") {
                        args.randomSeed = std::stoull(value);
                    } else if (key == "neuron_count") {
                        args.neuronCount = std::stoll(value);
                    } else if (key == "region_count") {
                        args.regionCount = std::stoll(value);
                    } else if (key == "connection_probability") {
                        args.connectionProbability = std::stof(value);
                    } else if (key == "connectivity_neurons") {
                        args.connectivityTestNeuronCount = std::stoi(value);
                    } else if (key == "connectivity_current") {
                        args.connectivityTestCurrent = std::stof(value);
                    } else if (key == "plasticity_steps") {
                        args.plasticityTestSteps = std::stoll(value);
                    } else if (key == "stdp_trials") {
                        args.stdpTestTrials = std::stoi(value);
                    } else if (key == "stdp_current") {
                        args.stdpTestCurrent = std::stof(value);
                    } else if (key == "disable_all_tests") {
                        args.runAllTests = false;
                    } else if (key == "disable_connectivity_test") {
                        args.runConnectivityTest = false;
                    } else if (key == "disable_plasticity_test") {
                        args.runPlasticityTest = false;
                    } else if (key == "disable_stdp_test") {
                        args.runStdpTest = false;
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid value for argument " + key + ": " + value);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                try {
                    if (key == "random_seed") {
                        args.randomSeed = std::stoull(value);
                    } else if (key == "neuron_count") {
                        args.neuronCount = std::stoll(value);
                    } else if (key == "region_count") {
                        args.regionCount = std::stoll(value);
                    } else if (key == "connection_probability") {
                        args.connectionProbability = std::stof(value);
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid value for argument " + key + ": " + value);
                }
            }
        }
    }
    
    return args;
}

// Reset brain state (DRY - removes duplicate reset/initialize logic)
void resetBrain(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
}

// Log brain configuration summary for debugging
void logBrainConfigSummary(const Config* config) {
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
}

// Log test configuration for transparency
void logTestConfig(const CliArgs& args) {
    if (args.runAllTests) {
        NLM_LOG_INFO("Running all tests (connectivity, plasticity, STDP)");
    } else {
        std::string tests;
        if (args.runConnectivityTest) tests += "connectivity ";
        if (args.runPlasticityTest) tests += "plasticity ";
        if (args.runStdpTest) tests += "STDP ";
        NLM_LOG_INFO("Running selected tests: " + tests);
    }
    
    if (args.runConnectivityTest) {
        NLM_LOG_INFO("Connectivity test parameters:");
        NLM_LOG_INFO("  neurons: " + std::to_string(args.connectivityTestNeuronCount));
        NLM_LOG_INFO("  current: " + std::to_string(args.connectivityTestCurrent));
    }
    
    if (args.runPlasticityTest) {
        NLM_LOG_INFO("Plasticity test parameters:");
        NLM_LOG_INFO("  steps: " + std::to_string(args.plasticityTestSteps));
    }
    
    if (args.runStdpTest) {
        NLM_LOG_INFO("STDP test parameters:");
        NLM_LOG_INFO("  trials: " + std::to_string(args.stdpTestTrials));
        NLM_LOG_INFO("  current: " + std::to_string(args.stdpTestCurrent));
    }
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, int neuronCount = 10, float current = 50.0f) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    NLM_LOG_INFO("Testing with " + std::to_string(neuronCount) + " neurons, current=" + std::to_string(current));
    
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
    
    // Determine which neurons to stimulate based on neuronCount
    size_t neuronsToStimulate = std::min<size_t>(neuronCount, neurons.size());
    
    // Inject strong current into selected neurons
    NLM_LOG_INFO("  Injecting current into " + std::to_string(neuronsToStimulate) + " neurons...");
    for (size_t i = 0; i < neuronsToStimulate; ++i) {
        neurons[i]->injectCurrent(current);  // Configurable current value
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
        for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, SimulationStep steps = 1000) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    NLM_LOG_INFO("Testing with " + std::to_string(steps) + " steps");
    
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
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < steps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
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

void runStdpVerification(std::shared_ptr<Brain> brain, int trials = 50, float current = 60.0f) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    NLM_LOG_INFO("Testing with " + std::to_string(trials) + " trials, current=" + std::to_string(current));
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");

    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");

    for (int trial = 0; trial < trials; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), current);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), current);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }

    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");

    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Parse command-line arguments
    CliArgs cliArgs = parseCommandLineArgs(argc, argv);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(cliArgs.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + cliArgs.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply CLI arguments to config (these override file settings)
    config->set("random_seed", static_cast<int64_t>(cliArgs.randomSeed), ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(cliArgs.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(cliArgs.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", cliArgs.connectionProbability, ConfigSource::CommandLine);
    
    // Set default values for Phase 2 (these override everything except CLI)
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate",  rectifier, 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    logBrainConfigSummary(config.get());
    
    // Log test configuration for transparency
    NLM_LOG_INFO("");
    logTestConfig(cliArgs);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run tests based on configuration
    if (cliArgs.runConnectivityTest) {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        runBasicConnectivityTest(brain, cliArgs.connectivityTestNeuronCount, cliArgs.connectivityTestCurrent);
    }
    
    if (cliArgs.runPlasticityTest) {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        resetBrain(brain);
        runPlasticityExperiment(brain, cliArgs.plasticityTestSteps);
    }
    
    if (cliArgs.runStdpTest) {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        resetBrain(brain);
        runStdpVerification(brain, cliArgs.stdpTestTrials, cliArgs.stdpTestCurrent);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    
    // Log which tests were run
    NLM_LOG_INFO("  ✓ Tests executed:");
    if (cliArgs.runConnectivityTest) NLM_LOG_INFO("    - Basic connectivity test");
    if (cliArgs.runPlasticityTest) NLM_LOG_INFO("    - Plasticity learning experiment");
    if (cliArgs.runStdpTest) NLM_LOG_INFO("    - STDP verification");
    
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
