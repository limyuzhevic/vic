#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <map>

using namespace nlm;

class CommandLineParser {
public:
    struct Options {
        std::string experimentType = "phase6";
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "experiment_results.txt";
        bool verbose = false;
        bool help = false;
        bool listExperiments = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        std::map<std::string, std::string> experimentCommands = {
            {"phase6", "Run Phase 6 Integration Experiment"},
            {"phase3", "Run Phase 3 Experiment"},
            {"phase4", "Run Phase 4 Experiment"},
            {"phase5", "Run Phase 5 Experiment"},
            {"legacy", "Run legacy Phase 2 tests (for backward compatibility)"}
        };
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--list-experiments" || arg == "-l") {
                options.listExperiments = true;
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg.find("--experiment=") != std::string::npos) {
                options.experimentType = arg.substr(arg.find("=") + 1);
            } else if (arg.find("-") == 0 && arg.length() > 1) {
                // Check for experiment type shortcuts
                if (arg == "-6") options.experimentType = "phase6";
                else if (arg == "-3") options.experimentType = "phase3";
                else if (arg == "-4") options.experimentType = "phase4";
                else if (arg == "-5") options.experimentType = "phase5";
                else if (arg == "-l") options.listExperiments = true;
            } else {
                // Try to match experiment type
                if (experimentCommands.find(arg) != experimentCommands.end()) {
                    options.experimentType = arg;
                }
            }
        }
        
        return options;
    }
    
    static void printHelp() {
        printBanner();
        std::cout << "\n";
        std::cout << "Usage: nlm [OPTIONS]\n";
        std::cout << "\n";
        std::cout << "Run neural learning machine experiments.\n";
        std::cout << "\n";
        std::cout << "Experiment Types:\n";
        std::cout << "  --experiment=phase6  (default)   Run Phase 6 Integration Experiment\n";
        std::cout << "  --experiment=phase3              Run Phase 3 Experiment\n";
        std::cout << "  --experiment=phase4              Run Phase 4 Experiment\n";
        std::cout << "  --experiment=phase5              Run Phase 5 Experiment\n";
        std::cout << "  --experiment=legacy              Run legacy Phase 2 tests (backward compatibility)\n";
        std::cout << "\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --list-experiments, -l  List available experiment types\n";
        std::cout << "  --verbose, -v            Enable verbose output\n";
        std::cout << "  --config <file>          Load configuration from file\n";
        std::cout << "  --output <file>          Save results to file\n";
        std::cout << "\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm                           Run Phase 6 Integration Experiment\n";
        std::cout << "  nlm --experiment=phase4       Run Phase 4 Experiment\n";
        std::cout << "  nlm --config myconfig.cfg     Use custom configuration\n";
        std::cout << "\n";
        std::cout << "The NLM brain simulator implements Leaky Integrate-and-Fire (LIF) neurons\n";
        std::cout << "with STDP and Hebbian plasticity, neuromodulation, memory systems,\n";
        std::cout << "and predictive cognition.\n";
    }
    
    static void printAvailableExperiments() {
        std::cout << "Available NLM Experiments:\n";
        std::cout << "==============================\n";
        std::cout << "\n";
        std::cout << "Phase 6 Integration Experiment:\n";
        std::cout << "  - Tests integration of all brain systems\n";
        std::cout << "  - Memory: Working + Episodic memory\n";
        std::cout << "  - Neuromodulation: Dopamine, Curiosity, Novelty\n";
        std::cout << "  - Prediction and planning\n";
        std::cout << "  - Development and maturation\n";
        std::cout << "  - Checkpoint save/load\n";
        std::cout << "  - Replay and consolidation\n";
        std::cout << "\n";
        std::cout << "Phase 3 Experiment:\n";
        std::cout << "  - World interaction loop\n";
        std::cout << "  - Sensory input and motor output\n";
        std::cout << "  - Reward prediction error\n";
        std::cout << "  - Developmental stages\n";
        std::cout << "  - Novelty and curiosity\n";
        std::cout << "\n";
        std::cout << "Phase 4 Experiment:\n";
        std::cout << "  - Neural prediction system\n";
        std::cout << "  - Working memory\n";
        std::cout << "  - Episodic-like memory\n";
        std::cout << "  - Concept formation\n";
        std::cout << "  - Neural attention\n";
        std::cout << "  - Predictive planning\n";
        std::cout << "  - Self-model\n";
        std::cout << "  - Social learning\n";
        std::cout << "  - Continual learning\n";
        std::cout << "\n";
        std::cout << "Phase 5 Experiment:\n";
        std::cout << "  - Performance optimizations\n";
        std::cout << "  - Memory pools and event queues\n";
        std::cout << "  - SIMD vectorization\n";
        std::cout << "  - Parallel processing\n";
        std::cout << "  - Checkpoint system\n";
        std::cout << "\n";
        std::cout << "Legacy Phase 2 Tests:\n";
        std::cout << "  - Basic neural connectivity\n";
        std::cout << "  - STDP and Hebbian plasticity\n";
        std::cout << "  - Structural plasticity\n";
        std::cout << "\n";
    }
    
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: FINAL INTEGRATION                               ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     The complete brain loop functions coherently.             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
};

// Legacy experiment classes extracted from old main.cpp
class BasicConnectivityExperiment {
public:
    struct Results {
        size_t spikesGenerated = 0;
        float avgMembranePotential = 0.0f;
        float avgThreshold = 0.0f;
        bool success = false;
    };
    
    Results run(std::shared_ptr<Brain> brain) {
        Results results;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Neural Connectivity Test ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return results;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return results;
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
        results.spikesGenerated = spikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            results.success = true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            float vSum = 0.0f, tSum = 0.0f;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                vSum += neurons[i]->getMembranePotential();
                tSum += neurons[i]->getThreshold();
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            if (!neurons.empty()) {
                results.avgMembranePotential = vSum / std::min(size_t(3), neurons.size());
                results.avgThreshold = tSum / std::min(size_t(3), neurons.size());
            }
        }
        
        return results;
    }
};

class PlasticityLearningExperiment {
public:
    struct Results {
        float initialMeanWeight = 0.0f;
        float finalMeanWeight = 0.0f;
        float weightChange = 0.0f;
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        size_t totalSpikes = 0;
        bool learningOccurred = false;
    };
    
    Results run(std::shared_ptr<Brain> brain) {
        Results results;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Record initial state
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        std::vector<float> initialWeights;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            results.initialMeanWeight = sum / initialWeights.size();
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            NLM_LOG_INFO("  Mean weight: " + std::to_string(results.initialMeanWeight));
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
        std::vector<float> finalWeights;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        results.totalSpikes = brain->getTotalSpikeCount();
        
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            results.finalMeanWeight = sum / finalWeights.size();
        }
        
        results.weightChange = results.finalMeanWeight - results.initialMeanWeight;
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(results.totalSpikes));
        if (!finalWeights.empty()) {
            NLM_LOG_INFO("  Mean weight: " + std::to_string(results.finalMeanWeight));
        }
        
        // Compute weight changes
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++results.strengthened;
            else if (delta < -0.01f) ++results.weakened;
            else ++results.unchanged;
        }
        
        // Determine if learning occurred
        results.learningOccurred = (std::abs(results.weightChange) > 0.001f) ||
                                   (results.strengthened > 0 || results.weakened > 0);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(results.initialMeanWeight));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(results.finalMeanWeight));
        NLM_LOG_INFO("  Change: " + std::to_string(results.weightChange));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(results.strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(results.weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(results.unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(results.totalSpikes));
        
        if (results.learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        return results;
    }
};

class StdpVerificationExperiment {
public:
    struct Results {
        std::vector<float> beforeWeights;
        std::vector<float> afterWeights;
        std::vector<float> deltas;
        float totalDelta = 0.0f;
        bool potentiation = false;
        bool depression = false;
        bool noChange = false;
    };
    
    Results run(std::shared_ptr<Brain> brain) {
        Results results;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Verification Experiment ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return results;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return results;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        // Record initial weights
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            results.beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " before: " + std::to_string(results.beforeWeights[i]));
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
        for (size_t i = 0; i < results.beforeWeights.size(); ++i) {
            float delta = synapses[i]->getWeight() - results.beforeWeights[i];
            results.deltas.push_back(delta);
            results.afterWeights.push_back(synapses[i]->getWeight());
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " after: " + std::to_string(results.afterWeights[i]) +
                       " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        for (float delta : results.deltas) {
            results.totalDelta += delta;
        }
        
        NLM_LOG_INFO("");
        if (results.totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            results.potentiation = true;
        } else if (results.totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            results.depression = true;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            results.noChange = true;
        }
        
        return results;
    }
};

int main(int argc, char** argv) {
    auto options = CommandLineParser::parse(argc, argv);
    
    // Print help or list experiments
    if (options.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (options.listExperiments) {
        CommandLineParser::printAvailableExperiments();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    CommandLineParser::printBanner();
    std::cout << "Initializing NLM Phase 6 Simulation...\n" << std::endl;
    
    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
        std::string configFile = options.configFile;
        if (configFile != "configs/default.cfg") {
            if (config->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_INFO("Failed to load configuration from: " + configFile);
                NLM_LOG_INFO("Using default configuration.");
            }
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set default values for Phase 6
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
        NLM_LOG_INFO("");
        
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        if (options.experimentType == "legacy") {
            // Run legacy Phase 2 tests for backward compatibility
            BasicConnectivityExperiment connectivityExp;
            auto connectivityResults = connectivityExp.run(brain);
            
            // Reset brain for plasticity experiment
            brain->reset();
            brain->initialize();
            
            PlasticityLearningExperiment plasticityExp;
            auto plasticityResults = plasticityExp.run(brain);
            
            // Reset and run STDP verification
            brain->reset();
            brain->initialize();
            
            StdpVerificationExperiment stdpExp;
            auto stdpResults = stdpExp.run(brain);
            
        } else if (options.experimentType == "phase3") {
            // Run Phase 3 experiment
            NLM_LOG_INFO("Running Phase 3 Experiment...");
            Phase3Experiment phase3Exp;
            phase3Exp.run(*brain);
            
        } else if (options.experimentType == "phase4") {
            // Run Phase 4 experiment
            NLM_LOG_INFO("Running Phase 4 Experiment...");
            Phase4Experiment phase4Exp;
            phase4Exp.run(*brain);
            
        } else if (options.experimentType == "phase5") {
            // Run Phase 5 experiment
            NLM_LOG_INFO("Running Phase 5 Experiment...");
            Phase5Experiment phase5Exp;
            phase5Exp.run(*brain);
            
        } else {
            // Default: Run Phase 6 Integration Experiment
            NLM_LOG_INFO("Running Phase 6 Integration Experiment...");
            Phase6IntegratedExperiment phase6Exp;
            
            Phase6Config phase6Config;
            phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
            phase6Config.regionCount = config->getOr<int64_t>("region_count", 2);
            phase6Config.connectionProbability = config->getOr<float>("connection_probability", 0.1f);
            phase6Config.maxSteps = 5000;
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            phase6Config.checkpointPath = "./nlm_phase6_checkpoint.bin";
            
            auto result = phase6Exp.run(phase6Config);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Phase 6 Integration Results ===");
            NLM_LOG_INFO("Total Reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Average Firing Rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Episodes Stored: " + std::to_string(result.memoryEpisodesStored));
            NLM_LOG_INFO("Novelty Level: " + std::to_string(result.noveltyLevel));
            NLM_LOG_INFO("Curiosity Level: " + std::to_string(result.curiosityLevel));
            NLM_LOG_INFO("Dopamine Level: " + std::to_string(result.dopamineLevel));
            NLM_LOG_INFO("Wall Time: " + std::to_string(result.totalWallClockTime) + "s");
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Integration Status:");
            NLM_LOG_INFO("  Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
            NLM_LOG_INFO("  Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
            NLM_LOG_INFO("  Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
            NLM_LOG_INFO("  Prediction: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
            NLM_LOG_INFO("  Development: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
            NLM_LOG_INFO("  Checkpointing: " + std::string(result.checkpointingWorks ? "YES" : "NO"));
            NLM_LOG_INFO("  Replay: " + std::string(result.replayWorks ? "YES" : "NO"));
            
        } else {
            NLM_LOG_ERROR("Unknown experiment type: " + options.experimentType);
            return 1;
        }
        
        // Save results if requested
        if (!options.outputFile.empty()) {
            NLM_LOG_INFO("Saving results to: " + options.outputFile);
            // TODO: Implement results serialization
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Simulation Complete ===");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during simulation: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
