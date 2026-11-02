// Implementation of main_new.cpp

#include "main_new.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <chrono>

using namespace nlm;

// NLMCommandLine implementation
NLMCommandLine::Options NLMCommandLine::parseArgs(int argc, char** argv) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.mode = Mode::HELP;
        } else if (arg == "--version" || arg == "-v") {
            options.mode = Mode::VERSION;
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                std::string modeStr = argv[++i];
                if (modeStr == "basic") options.mode = Mode::BASIC;
                else if (modeStr == "plasticity") options.mode = Mode::PLIABILITY;
                else if (modeStr == "stdp") options.mode = Mode::STDP;
                else if (modeStr == "integration") options.mode = Mode::INTEGRATION;
                else if (modeStr == "script") options.mode = Mode::SCRIPT;
                else {
                    std::cerr << "ERROR: Unknown mode: " << modeStr << std::endl;
                    options.mode = Mode::HELP;
                }
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--seed") {
            if (i + 1 < argc) {
                options.seed = std::stoull(argv[++i]);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                options.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                options.steps = std::stoull(argv[++i]);
            }
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--save") {
            options.saveState = true;
        } else {
            // Custom argument
            options.customArgs.push_back(arg);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.customArgs.push_back(argv[++i]);
            }
        }
    }
    
    return options;
}

void NLMCommandLine::printHelp() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Usage: nlm [OPTIONS]                                       ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

Modes:
  --mode basic          Run basic connectivity test
  --mode plasticity     Run plasticity learning experiment  
  --mode stdp           Run STDP verification
  --mode integration    Run full integration demo (default)
  --mode script         Run script mode for batch processing

General Options:
  --config FILE         Load configuration from FILE (default: configs/default.cfg)
  --seed NUMBER         Random seed (default: 42)
  --neurons NUMBER      Number of neurons (default: 500)
  --steps NUMBER        Number of simulation steps (default: 1000)
  --verbose             Enable verbose logging
  --quiet               Suppress informational output
  --output FILE         Save results to FILE
  --save                Save brain state after execution
  -h, --help            Show this help message
  -v, --version         Show version information

Examples:
  nlm --mode basic                    # Basic test
  nlm --mode plasticity --steps 5000  # Extended plasticity test
  nlm --config custom.cfg --seed 123   # Custom configuration
  nlm --mode script --output results.txt  # Script mode for automation

For more information:
  http://nlm.readthedocs.io
)";
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM (Neural Learning Machine) v2.6.0 - Phase 6 Integration"
              << std::endl;
    std::cout << "Built with C++20, implementing real spiking neural computation"
              << std::endl;
    std::cout << "Features: LIF neurons, STDP, Hebbian learning, structural plasticity"
              << std::endl;
    std::cout << "Integrated: Memory, prediction, cognition, neuromodulation"
              << std::endl;
}

// AdvancedBrainOperations implementation
void AdvancedBrainOperations::runLearningExperiment(std::shared_ptr<Brain> brain, size_t steps) {
    NLM_LOG_INFO("Running advanced learning experiment...");
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    // Enable all plasticity types
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // STDP, Hebbian, structural
        }
    }
    
    // Patterned input for selective learning
    std::vector<NeuronId> patternNeurons;
    if (region) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (size_t step = 0; step < steps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0 && step > 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Learning: " + std::to_string(AdvancedBrainOperations::calculateLearningScore(brain)));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void AdvancedBrainOperations::runPatternLearning(std::shared_ptr<Brain> brain,
                                               const std::vector<float>& pattern,
                                               size_t cycles) {
    NLM_LOG_INFO("Running pattern learning experiment...");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Enable plasticity for selective learning
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, false, false);  // STDP only
    }
    
    // Create distinct pre-post pairs for pattern association
    std::vector<std::pair<NeuronId, NeuronId>> synapses;
    for (auto* neuron : region->getAllNeurons()) {
        if (neuron->getType() == NeuronType::Sensory && synapses.size() < pattern.size()) {
            // Find postsynaptic partners
            for (auto* post : region->getAllNeurons()) {
                if (post->getType() == NeuronType::Internal) {
                    synapses.emplace_back(neuron->getId(), post->getId());
                    break;
                }
            }
        }
    }
    
    // Run pattern learning cycles
    for (size_t cycle = 0; cycle < cycles; ++cycle) {
        // Present pattern
        for (size_t i = 0; i < std::min(pattern.size(), synapses.size()); ++i) {
            auto& syn = synapses[i];
            brain->injectCurrent(syn.first, pattern[i] * 50.0f);
            brain->step(cycle * 10 + i, (cycle * 10 + i) * 0.001);
            brain->injectCurrent(syn.second, pattern[i] * 50.0f);
            brain->step(cycle * 10 + i + 1, (cycle * 10 + i + 1) * 0.001);
        }
        
        if (cycle % 5 == 0) {
            NLM_LOG_INFO("Cycle " + std::to_string(cycle) + "/" + std::to_string(cycles));
        }
    }
}

void AdvancedBrainOperations::runMemoryTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Running memory integration test...");
    
    // Test working memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemory->getCapacity()));
        
        // Store some patterns
        for (int i = 0; i < 10; ++i) {
            std::vector<NeuronId> neurons;
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Internal && neurons.size() < 5) {
                        neurons.push_back(neuron->getId());
                    }
                }
            }
            workingMemory->store(neurons, 0.8f + 0.1f * i);
        }
        
        NLM_LOG_INFO("Working memory: " + std::to_string(workingMemory->getActiveTraces()) + " active traces");
    }
    
    // Test episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
        
        // Force episode storage
        for (int step = 0; step < 50; step += 10) {
            brain->step(step, step * 0.001);
        }
        
        size_t episodeCount = episodicMemory->getEpisodes().size();
        NLM_LOG_INFO("Episodic memory: " + std::to_string(episodeCount) + " episodes stored");
    }
}

void AdvancedBrainOperations::runDevelopmentSimulation(std::shared_ptr<Brain> brain, 
                                                     size_t lifespan) {
    NLM_LOG_INFO("Running developmental simulation...");
    
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    
    for (size_t step = 0; step < lifespan; ++step) {
        // Update development every 100 steps
        if (step % 100 == 0 && step > 0) {
            brain->develop();
            auto stage = brain->getDevelopmentalStage();
            NLM_LOG_INFO("Stage " + std::to_string(static_cast<int>(stage)) + 
                        " at step " + std::to_string(step));
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("Developmental step " + std::to_string(step));
        }
    }
}

void AdvancedBrainOperations::runNeuromodulationStudy(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Running neuromodulation study...");
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (dopamine) {
        NLM_LOG_INFO("Initial dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    
    if (curiosity) {
        NLM_LOG_INFO("Initial curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    
    if (novelty) {
        NLM_LOG_INFO("Initial novelty level: " + std::to_string(novelty->getLevel()));
    }
    
    // Run with varying input to see neuromodulation responses
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (int step = 0; step < 100; ++step) {
            // Apply different levels of stimulation
            float stimulation = 0.5f + 0.5f * std::sin(step * 0.1f);
            
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory) {
                    brain->injectCurrent(neuron->getId(), stimulation * 40.0f);
                }
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 20 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) +
                            " | Dopamine: " + (dopamine ? std::to_string(dopamine->getLevel()) : "N/A") +
                            " | Curiosity: " + (curiosity ? std::to_string(curiosity->getLevel()) : "N/A"));
            }
        }
    }
}

// Convenience functions implementation
namespace Convenience {
    std::shared_ptr<Brain> createMinimalBrain(size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize minimal brain");
        }
        
        return brain;
    }
    
    std::shared_ptr<Brain> createLearningBrain(size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize learning brain");
        }
        
        return brain;
    }
    
    std::shared_ptr<Brain> createDevelopmentalBrain(size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("development.enabled", true, ConfigSource::Default);
        config->set("development.initial_plasticity", 1.0f, ConfigSource::Default);
        config->set("development.critical_period_length", 500, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize developmental brain");
        }
        
        return brain;
    }
    
    void runForSteps(std::shared_ptr<Brain> brain, size_t steps,
                    std::function<void(size_t)> callback) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            if (callback) {
                callback(step);
            }
        }
    }
    
    float calculateLearningScore(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0f;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0.0f;
        
        float totalChange = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (std::abs(weight) > 0.01f) {
                totalChange += std::abs(weight);
                synapseCount++;
            }
        }
        
        if (synapseCount == 0) return 0.0f;
        return totalChange / synapseCount;
    }
    
    std::vector<float> getWeightDistribution(std::shared_ptr<Brain> brain) {
        std::vector<float> weights;
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
            }
        }
        
        return weights;
    }
    
    void printBrainStats(std::shared_ptr<Brain> brain, bool verbose) {
        if (!brain) return;
        
        std::cout << "=== NLM Brain Statistics ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        if (verbose) {
            std::cout << "\n=== Advanced Statistics ===" << std::endl;
            
            auto weights = getWeightDistribution(brain);
            if (!weights.empty()) {
                float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
                float mean = sum / weights.size();
                float variance = 0.0f;
                for (float w : weights) {
                    variance += (w - mean) * (w - mean);
                }
                variance /= weights.size();
                float stddev = std::sqrt(variance);
                
                std::cout << "Weight statistics:" << std::endl;
                std::cout << "  Mean: " << mean << std::endl;
                std::cout << "  Stddev: " << stddev << std::endl;
                std::cout << "  Min: " << *std::min_element(weights.begin(), weights.end()) << std::endl;
                std::cout << "  Max: " << *std::max_element(weights.begin(), weights.end()) << std::endl;
                
                size_t positive = std::count_if(weights.begin(), weights.end(), 
                                             [](float w) { return w > 0; });
                size_t negative = std::count_if(weights.begin(), weights.end(), 
                                             [](float w) { return w < 0; });
                size_t zero = std::count_if(weights.begin(), weights.end(), 
                                         [](float w) { return w == 0; });
                
                std::cout << "  Positive weights: " << positive << std::endl;
                std::cout << "  Negative weights: " << negative << std::endl;
                std::cout << "  Zero weights: " << zero << std::endl;
            }
        }
    }
}

// NLMMain implementation
NLMMain::NLMMain() : initialized_(false) {}

NLMMain::~NLMMain() {
    cleanup();
}

bool NLMMain::initialize(const NLMCommandLine::Options& options) {
    options_ = options;
    
    try {
        // Setup logging
        setupLogging(options_.verbose);
        
        // Create configuration
        config_ = std::make_shared<Config>();
        
        // Load configuration file if specified
        if (config_->loadFromFile(options_.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options_.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + options_.configFile + ")");
        }
        
        // Apply configuration based on mode
        if (options_.mode == NLMCommandLine::Mode::INTEGRATION || 
            options_.mode == NLMCommandLine::Mode::BASIC ||
            options_.mode == NLMCommandLine::Mode::PLIABILITY ||
            options_.mode == NLMCommandLine::Mode::STDP) {
            applyPhase6Defaults(config_);
        } else {
            applyPhase2Defaults(config_);
        }
        
        // Apply advanced options
        applyAdvancedOptions(config_, options_);
        
        // Apply command line specific options
        config_->set("random_seed", static_cast<int64_t>(options_.seed), ConfigSource::CommandLine);
        config_->set("neuron_count", static_cast<int64_t>(options_.neuronCount), ConfigSource::CommandLine);
        
        // Create brain
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Log startup information
        printBanner();
        NLM_LOG_INFO("NLM Phase " + std::to_string(static_cast<int>(options_.mode)) + ": " + 
                    getModeString(options_.mode));
        NLM_LOG_INFO("Configuration: " + std::to_string(options_.neuronCount) + " neurons, " +
                    std::to_string(options_.seed) + " seed");
        
        initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during initialization: " + std::string(e.what()));
        return false;
    }
}

bool NLMMain::run() {
    if (!initialized_ || !brain_) {
        NLM_LOG_ERROR("Application not properly initialized");
        return false;
    }
    
    try {
        switch (options_.mode) {
            case NLMCommandLine::Mode::BASIC:
                return runBasicMode(brain_);
            case NLMCommandLine::Mode::PLIABILITY:
                return runPlasticityMode(brain_);
            case NLMCommandLine::Mode::STDP:
                return runStdpMode(brain_);
            case NLMCommandLine::Mode::INTEGRATION:
                return runIntegrationMode(brain_);
            case NLMCommandLine::Mode::SCRIPT:
                return runScriptMode(brain_);
            case NLMCommandLine::Mode::HELP:
                NLMCommandLine::printHelp();
                return true;
            case NLMCommandLine::Mode::VERSION:
                NLMCommandLine::printVersion();
                return true;
            default:
                NLM_LOG_ERROR("Unknown mode: " + std::to_string(static_cast<int>(options_.mode)));
                return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during execution: " + std::string(e.what()));
        return false;
    }
}

bool NLMMain::cleanup() {
    if (brain_) {
        brain_.reset();
    }
    if (config_) {
        config_.reset();
    }
    initialized_ = false;
    return true;
}

void NLMMain::applyPhase2Defaults(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void NLMMain::applyPhase6Defaults(std::shared_ptr<Config> config) {
    // Phase 6 integration configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Enable all plasticity systems
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Memory systems
    config->set("memory.working_memory.capacity", 100, ConfigSource::Default);
    config->set("memory.episodic_memory.max_episodes", 1000, ConfigSource::Default);
    
    // Development
    config->set("development.enabled", true, ConfigSource::Default);
    config->set("development.initial_plasticity", 1.0f, ConfigSource::Default);
    
    // Replay
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
}

void NLMMain::applyAdvancedOptions(std::shared_ptr<Config> config, 
                                  const NLMCommandLine::Options& options) {
    // Apply custom arguments
    for (size_t i = 0; i + 1 < options.customArgs.size(); i += 2) {
        std::string key = options.customArgs[i];
        std::string value = options.customArgs[i + 1];
        
        // Parse value based on type
        if (value.find('.') != std::string::npos) {
            // Float value
            float floatValue = std::stof(value);
            config->set(key, floatValue, ConfigSource::CommandLine);
        } else {
            // Integer value
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::CommandLine);
            } catch (...) {
                // Treat as string
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
}

bool NLMMain::runBasicMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Mode: Basic Neural Connectivity ===");
    
    // Use the legacy function for compatibility
    runBasicConnectivityTest(brain);
    
    if (options_.saveState && options_.outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + options_.outputFile);
        brain->save(options_.outputFile);
    }
    
    Convenience::printBrainStats(brain, options_.verbose);
    return true;
}

bool NLMMain::runPlasticityMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Mode: Plasticity Learning Experiment ===");
    
    // Use the legacy function for compatibility
    runPlasticityExperiment(brain);
    
    if (options_.saveState && !options_.outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + options_.outputFile);
        brain->save(options_.outputFile);
    }
    
    Convenience::printBrainStats(brain, options_.verbose);
    return true;
}

bool NLMMain::runStdpMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Mode: STDP Verification ===");
    
    // Use the legacy function for compatibility
    runStdpVerification(brain);
    
    if (options_.saveState && !options_.outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + options_.outputFile);
        brain->save(options_.outputFile);
    }
    
    Convenience::printBrainStats(brain, options_.verbose);
    return true;
}

bool NLMMain::runIntegrationMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Mode: Full Integration Demo ===");
    
    // Run all three legacy tests for comprehensive verification
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Run advanced operations
    AdvancedBrainOperations::runMemoryTest(brain);
    AdvancedBrainOperations::runNeuromodulationStudy(brain);
    
    if (options_.saveState && !options_.outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + options_.outputFile);
        brain->save(options_.outputFile);
    }
    
    Convenience::printBrainStats(brain, options_.verbose);
    return true;
}

bool NLMMain::runScriptMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Mode: Script Mode ===");
    
    // Script mode: run simulation and output results to file
    std::ofstream outputFile;
    if (!options_.outputFile.empty()) {
        outputFile.open(options_.outputFile);
        if (!outputFile.is_open()) {
            NLM_LOG_ERROR("Failed to open output file: " + options_.outputFile);
            return false;
        }
    }
    
    // Run simulation
    for (size_t step = 0; step < options_.steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Log to console if not quiet
        if (!options_.quiet) {
            std::cout << "Step " << step << 
                     ", Spikes: " << brain->getTotalSpikeCount() <<
                     ", Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
        
        // Log to file if specified
        if (outputFile.is_open()) {
            outputFile << step << ","
                      << brain->getTotalSpikeCount() << ","
                      << brain->getFiringNeuronCount() << ","
                      << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    outputFile.close();
    Convenience::printBrainStats(brain, options_.verbose);
    return true;
}

void NLMMain::setupLogging(bool verbose) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void NLMMain::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Advanced Integration                             ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

std::string NLMMain::getModeString(NLMCommandLine::Mode mode) {
    switch (mode) {
        case NLMCommandLine::Mode::BASIC: return "Basic Neural Computation";
        case NLMCommandLine::Mode::PLIABILITY: return "Plasticity Learning";
        case NLMCommandLine::Mode::STDP: return "STDP Verification";
        case NLMCommandLine::Mode::INTEGRATION: return "Full Integration Demo";
        case NLMCommandLine::Mode::SCRIPT: return "Script Mode";
        case NLMCommandLine::Mode::HELP: return "Help";
        case NLMCommandLine::Mode::VERSION: return "Version Info";
        default: return "Unknown Mode";
    }
}

// Legacy compatibility function implementations
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
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
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
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
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
    syn->enablePlasticity(false, true, false);
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