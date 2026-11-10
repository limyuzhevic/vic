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
        } else {
            NLM_LOG_INFO("ERROR: No region 1 found in brain for weight recording");
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        } else {
            NLM_LOG_INFO("  WARNING: No weights recorded (no synapses or no region)");
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        } else {
            NLM_LOG_INFO("ERROR: No region 1 found in brain for weight recording");
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        } else {
            NLM_LOG_INFO("  WARNING: No weights recorded (no synapses or no region)");
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
    
    if (patternNeurons.empty()) {
        NLM_LOG_INFO("  WARNING: No sensory neurons found for plasticity test");
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
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

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("  WARNING: No region 1 found in brain for STDP test");
        return;
    }
    
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) {
        NLM_LOG_INFO("  WARNING: No spike system found in brain for STDP test");
        return;
    }
    
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

    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
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

void printHelp() {
    printBanner();
    std::cout << "\nUsage: nlm [OPTIONS]\n" << std::endl;
    std::cout << "Phase 2: Real Neural Computation Demo\n" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config FILE, -c FILE        Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neuron-count N, -n N        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region-count R, -r R        Number of regions (default: 1)" << std::endl;
    std::cout << "  --timestep T, -t T             Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --seed S, -s S                Random seed (default: 42)" << std::endl;
    std::cout << "  --connection-prob P, -p P     Connection probability (default: 0.15)" << std::endl;
    std::cout << "  --steps-only                  Run connectivity test only" << std::endl;
    std::cout << "  --no-plasticity              Disable plasticity for faster testing" << std::endl;
    std::cout << "  --help-plasticity            Show plasticity configuration options" << std::endl;
    std::cout << "  --help-development            Show development configuration options" << std::endl;
    std::cout << "  --help-neuromodulation        Show neuromodulation configuration options" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  nlm                                    # Run full demo with defaults" << std::endl;
    std::cout << "  nlm --config myconfig.cfg              # Use custom configuration" << std::endl;
    std::cout << "  nlm --neuron-count 1000 --region-count 2   # Create larger brain" << std::endl;
    std::cout << "  nlm --steps-only                      # Run only connectivity test" << std::endl;
    std::cout << "\nFor more information, see the documentation in docs/";
}

void printPlasticityHelp() {
    std::cout << "\n=== Plasticity Configuration Options ===\n" << std::endl;
    std::cout << "STDP (Spike-Timing Dependent Plasticity):" << std::endl;
    std::cout << "  --stdp-ltp-weight W         LTP weight (default: 0.02)" << std::endl;
    std::cout << "  --stdp-ltd-weight W         LTD weight (default: 0.015)" << std::endl;
    std::cout << "  --stdp-tau T                STDP time constant in ms (default: 20.0)" << std::endl;
    std::cout << "\nHebbian Learning:" << std::endl;
    std::cout << "  --hebbian-rate R            Hebbian learning rate (default: 0.01)" << std::endl;
    std::cout << "\nStructural Plasticity:" << std::endl;
    std::cout << "  --synaptogenesis-rate R     Rate of new synapse formation (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate R            Rate of synapse pruning (default: 0.00001)" << std::endl;
}

void printDevelopmentHelp() {
    std::cout << "\n=== Development Configuration Options ===\n" << std::endl;
    std::cout << "Developmental stages and timing:" << std::endl;
    std::cout << "  --developmental-stage S      Initial developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)" << std::endl;
    std::cout << "  --replay-interval I          Memory replay interval (default: 100 steps)" << std::endl;
    std::cout << "  --consolidation-interval C   Memory consolidation interval (default: 1000 steps)" << std::endl;
}

void printNeuromodulationHelp() {
    std::cout << "\n=== Neuromodulation Configuration Options ===\n" << std::endl;
    std::cout << "Neuromodulators that affect learning and behavior:" << std::endl;
    std::cout << "  --dopamine-level D          Dopamine baseline level (default: 0.0)" << std::endl;
    std::cout << "  --curiosity-level C         Curiosity baseline level (default: 0.0)" << std::endl;
    std::cout << "  --novelty-level N           Novelty detection baseline (default: 0.0)" << std方が std::endl;
    std::cout << "  --prediction-error E        Prediction error baseline (default: 0.0)" << std::endl;
}

bool validateConfig(const Config& config) {
    bool isValid = true;
    
    // Validate neuron count
    auto neuronCountOpt = config.get<int64_t>("neuron_count");
    if (neuronCountOpt) {
        int64_t neuronCount = *neuronCountOpt;
        if (neuronCount < 10) {
            NLM_LOG_WARNING("Neuron count very low (" + std::to_string(neuronCount) + ") - may affect simulation stability");
            isValid = false;
        } else if (neuronCount > 50000) {
            NLM_LOG_WARNING("Neuron count high (" + std::to_string(neuronCount) + ") - may be slow to run");
            isValid = false;
        }
    }
    
    // Validate region count
    auto regionCountOpt = config.get<int64_t>("region_count");
    if (regionCountOpt) {
        int64_t regionCount = *regionCountOpt;
        if (regionCount < 1) {
            NLM_LOG_WARNING("Region count must be at least 1");
            isValid = false;
        } else if (regionCount > 10) {
            NLM_LOG_WARNING("High region count (" + std::to_string(regionCount) + ") - may increase memory usage");
            isValid = false;
        }
    }
    
    // Validate connection probability
    auto connectionProbOpt = config.get<float>("connection_probability");
    if (connectionProbOpt) {
        float connectionProb = *connectionProbOpt;
        if (connectionProb < 0.01) {
            NLM_LOG_WARNING("Connection probability very low (" + std::to_string(connectionProb) + ") - may result in sparse connectivity");
            isValid = false;
        } else if (connectionProb > 0.5) {
            NLM_LOG_WARNING("Connection probability high (" + std::to_string(connectionProb) + ") - may result in dense connectivity");
            isValid = false;
        }
    }
    
    // Validate timestep
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (timestepOpt) {
        double timestep = *timestepOpt;
        if (timestep <= 0.0 || timestep > 0.1) {
            NLM_LOG_WARNING("Simulation timestep out of recommended range (0.0001 to 0.1s): " + std::to_string(timestep));
            isValid = false;
        }
    }
    
    // Validate STDP parameters
    auto stdpLtpOpt = config.get<float>("stdp_ltp_weight");
    if (stdpLtpOpt) {
        float ltp = *stdpLtpOpt;
        if (ltp < 0.0 || ltp > 1.0) {
            NLM_LOG_WARNING("STDP LTP weight out of recommended range [0.0, 1.0]: " + std::to_string(ltp));
            isValid = false;
        }
    }
    
    auto stdpLtdOpt = config.get<float>("stdp_ltd_weight");
    if (stdpLtdOpt) {
        float ltd = *stdpLtdOpt;
        if (ltd < 0.0 || ltd > 1.0) {
            NLM_LOG_WARNING("STDP LTD weight out of recommended range [0.0, 1.0]: " + std::to_string(ltd));
            isValid = false;
        }
    }
    
    return isValid;
}

void applyCommandLineArgs(Config& config, int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Help options
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--help-plasticity") {
            printPlasticityHelp();
            exit(0);
        } else if (arg == "--help-development") {
            printDevelopmentHelp();
            exit(0);
        } else if (arg == "--help-neuromodulation") {
            printNeuromodulationHelp();
            exit(0);
        }
        
        // Config file
        if (arg.substr(0, 7) == "--config" || arg.substr(0, 2) == "-c") {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            if (!configFile.empty()) {
                if (config.loadFromFile(configFile)) {
                    NLM_LOG_INFO("Loaded configuration from command line: " + configFile);
                } else {
                    NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
                }
            }
        }
        
        // Neuron count
        if (arg.substr(0, 11) == "--neuron-count" || arg.substr(0, 2) == "-n") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                int64_t neuronCount = std::stoll(value);
                config.set("neuron_count", neuronCount, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set neuron count to: " + std::to_string(neuronCount));
            } catch (...) {
                NLM_LOG_WARNING("Invalid neuron count value: " + value + ". Using default.");
            }
        }
        
        // Region count
        if (arg.substr(0, 13) == "--region-count" || arg.substr(0, 2) == "-r") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                int64_t regionCount = std::stoll(value);
                config.set("region_count", regionCount, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set region count to: " + std::to_string(regionCount));
            } catch (...) {
                NLM_LOG_WARNING("Invalid region count value: " + value + ". Using default.");
            }
        }
        
        // Timestep
        if (arg.substr(0, 11) == "--timestep" || arg.substr(0, 2) == "-t") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                double timestep = std::stod(value);
                config.set("simulation_timestep", timestep, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set simulation timestep to: " + std::to_string(timestep) + "s");
            } catch (...) {
                NLM_LOG_WARNING("Invalid timestep value: " + value + ". Using default.");
            }
        }
        
        // Seed
        if (arg.substr(0, 6) == "--seed" || arg.substr(0, 2) == "-s") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                int64_t seed = std::stoll(value);
                config.set("random_seed", seed, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set random seed to: " + std::to_string(seed));
            } catch (...) {
                NLM_LOG_WARNING("Invalid seed value: " + value + ". Using default.");
            }
        }
        
        // Connection probability
        if (arg.substr(0, 20) == "--connection-prob" || arg.substr(0, 2) == "-p") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float prob = std::stof(value);
                config.set("connection_probability", prob, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set connection probability to: " + std::to_string(prob));
            } catch (...) {
                NLM_LOG_WARNING("Invalid connection probability value: " + value + ". Using default.");
            }
        }
        
        // Plasticity options
        if (arg.substr(0, 14) == "--stdp-ltp-weight") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float weight = std::stof(value);
                config.set("stdp_ltp_weight", weight, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set STDP LTP weight to: " + std::to_string(weight));
            } catch (...) {
                NLM_LOG_WARNING("Invalid STDP LTP weight value: " + value + ". Using default.");
            }
        }
        
        if (arg.substr(0, 14) == "--stdp-ltd-weight") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float weight = std::stof(value);
                config.set("stdp_ltd_weight", weight, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set STDP LTD weight to: " + std::to_string(weight));
            } catch (...) {
                NLM_LOG_WARNING("Invalid STDP LTD weight value: " + value + ". Using default.");
            }
        }
        
        if (arg.substr(0, 12) == "--stdp-tau") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float tau = std::stof(value);
                config.set("stdp_tau", tau, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set STDP tau to: " + std::to_string(tau));
            } catch (...) {
                NLM_LOG_WARNING("Invalid STDP tau value: " + value + ". Using default.");
            }
        }
        
        if (arg.substr(0, 20) == "--synaptogenesis-rate") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float rate = std::stof(value);
                config.set("synaptogenesis_rate", rate, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set synaptogenesis rate to: " + std::to_string(rate));
            } catch (...) {
                NLM_LOG_WARNING("Invalid synaptogenesis rate value: " + value + ". Using default.");
            }
        }
        
        if (arg.substr(0, 9) == "--pruning-rate") {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            try {
                float rate = std::stof(value);
                config.set("pruning_rate", rate, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set pruning rate to: " + std::to_string(rate));
            } catch (...) {
                NLM_LOG_WARNING("Invalid pruning rate value: " + value + ". Using default.");
            }
        }
        
        // Flag options
        if (arg == "--steps-only") {
            NLM_LOG_INFO("Steps-only mode: Only run connectivity test, skip plasticity experiments");
        } else if (arg == "--no-plasticity") {
            NLM_LOG_INFO("No-plasticity mode: Disable plasticity to run faster tests");
            config.set("stdp_ltp_weight", 0.0f, ConfigSource::CommandLine);
            config.set("stdp_ltd_weight", 0.0f, ConfigSource::CommandLine);
            config.set("synaptogenesis_rate", 0.0f, ConfigSource::CommandLine);
            config.set("pruning_rate",  Mightyf, ConfigSource::CommandLine);
        }
    }
}
