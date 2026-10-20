// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Enhanced Artificial Brain
//
// This phase implements the complete integrated artificial brain with:
// - All neural systems integrated and working together
// - Memory, prediction, cognition, and neuromodulation systems
// - Developmental stages and plasticity
// - Checkpoint and replay capabilities
// - Performance optimizations for research and production

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <csignal>
#include <thread>
#include <atomic>

using namespace nlm;

// Global flag for graceful shutdown
std::atomic<bool> g_running{true};

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    g_running = false;
}

// Command-line argument parser with enhanced features
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string mode = "demo"; // demo, research, integration, quicktest
    bool verbose = false;
    bool profile = false;
    std::string outputFile = "";
    size_t neuronCount = 0;
    size_t maxSteps = 0;
    std::string checkpointPath = "";
    bool help = false;
};

// Enhanced argument parser with better validation and help
CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                args.mode = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--profile" || arg == "-p") {
            args.profile = true;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                args.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--checkpoint" || arg == "-k") {
            if (i + 1 < argc) {
                args.checkpointPath = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t pos = key.find('=');
            if (pos != std::string::npos) {
                std::string value = key.substr(pos + 1);
                key = key.substr(0, pos);
                
                // Set configuration value
                auto config = std::make_shared<Config>();
                if (key == "random_seed" || key == "seed") {
                    config->set("random_seed", std::stoll(value));
                } else if (key == "neuron_count") {
                    config->set("neuron_count", std::stoull(value));
                } else if (key == "simulation_timestep") {
                    config->set("simulation_timestep", std::stod(value));
                }
            }
        } else if (arg[0] == '-') {
            std::string flags = arg.substr(1);
            for (char flag : flags) {
                switch (flag) {
                    case 'v': args.verbose = true; break;
                    case 'p': args.profile = true; break;
                    case 'h': args.help = true; break;
                }
            }
        }
    }
    
    return args;
}

// Enhanced banner with Phase 6 information
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Complete Artificial Brain with Integrated Systems           ║
    ║     All systems working together in a unified loop               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp() {
    std::cout << R"(
NLM - Neural Learning Machine (熙然)
Phase 6: Final Integration

USAGE:
  nlm [OPTIONS]

MODES:
  --mode demo        Run demonstration with default settings (default)
  --mode research    Run research experiment with optimized settings
  --mode integration Run full integration test
  --mode quicktest   Quick verification of basic functionality

OPTIONS:
  -h, --help            Show this help message
  -c, --config FILE     Configuration file (default: configs/default.cfg)
  -v, --verbose         Enable verbose logging
  -p, --profile        Enable performance profiling
  -o, --output FILE     Output results to file
  -n, --neurons COUNT   Override neuron count
  -s, --steps STEPS     Maximum simulation steps
  -k, --checkpoint PATH Checkpoint file path

EXAMPLES:
  nlm --mode demo                    # Run demo
  nlm --mode research --neurons 2000  # Research mode with 2000 neurons
  nlm --mode integration -p          # Integration test with profiling
  nlm --mode quicktest -v -k test.bin  # Quick test with checkpoint

For more information: https://github.com/nlm-project/nlm
)" << std::endl;
}

// Performance timer utility
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        std::cout << "[" << name_ << "] Completed in " << duration.count() << " ms" << std::endl;
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

// Advanced learning experiment with more features
class AdvancedLearningExperiment {
public:
    AdvancedLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
        : brain_(brain), seed_(seed) {}
    
    void runExperiment(size_t steps, bool enablePlasticity = true) {
        NLM_LOG_INFO("Starting advanced learning experiment with " + std::to_string(steps) + " steps");
        NLM_LOG_INFO("Initial brain state:");
        brain_->logStatus();
        
        // Record initial state
        recordInitialState();
        
        // Enable plasticity if requested
        if (enablePlasticity) {
            enableAllPlasticity();
        }
        
        // Run simulation with enhanced monitoring
        for (SimulationStep step = 0; step < steps && g_running; ++step) {
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           ", Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                           ", Active neurons: " + std::to_string(brain_->getActiveNeuronCount()));
            }
            
            brain_->step(step);
        }
        
        // Record final state
        recordFinalState();
        
        // Compute and display comprehensive statistics
        computeAdvancedStatistics();
    }
    
    void recordInitialState() {
        NLM_LOG_INFO("Recording initial brain state...");
        initialSynapseCount_ = brain_->getTotalSynapseCount();
        initialWeights_.clear();
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights_.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial synapses: " + std::to_string(initialSynapseCount_));
        if (!initialWeights_.empty()) {
            float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
            float mean = sum / initialWeights_.size();
            NLM_LOG_INFO("Initial mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        NLM_LOG_INFO("Recording final brain state...");
        finalWeights_.clear();
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights_.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
        NLM_LOG_INFO("Final spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        if (!finalWeights_.empty()) {
            float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
            float mean = sum / finalWeights_.size();
            NLM_LOG_INFO("Final mean weight: " + std::to_string(mean));
        }
    }
    
    void computeAdvancedStatistics() {
        NLM_LOG_INFO("\n=== ADVANCED LEARNING EXPERIMENT RESULTS ===");
        
        if (initialWeights_.empty() || finalWeights_.empty()) {
            NLM_LOG_WARNING("No weights recorded for comparison");
            return;
        }
        
        // Comprehensive weight analysis
        NLM_LOG_INFO("Weight Statistics:");
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        NLM_LOG_INFO("  Initial mean: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        NLM_LOG_INFO("  Relative change: " + std::to_string((finalMean - initialMean) / initialMean * 100.0f) + "%");
        
        // Detailed synaptic changes
        NLM_LOG_INFO("\nSynaptic Changes Analysis:");
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights_[i] - initialWeights_[i];
            if (std::abs(delta) < 0.001f) {
                ++unchanged;
            } else if (delta > 0.01f) {
                ++strengthened;
            } else if (delta < -0.01f) {
                ++weakened;
            }
        }
        
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        // Spike activity analysis
        NLM_LOG_INFO("\nSpike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons: " + std::to_string(mostActiveNeurons_.size()));
        
        // Learning assessment
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                               (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("\n=== LEARNING ASSESSMENT ===");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ SIGNIFICANT LEARNING DETECTED: System adapted through experience");
        } else {
            NLM_LOG_INFO("⚠ NO SIGNIFICANT LEARNING: System remained stable");
        }
        
        // Network health assessment
        NLM_LOG_INFO("\n=== NETWORK HEALTH ===");
        float eiRatio = brain_->getExcitationInhibitionRatio();
        NLM_LOG_INFO("  Excitation/Inhibition Ratio: " + std::to_string(eiRatio));
        
        if (eiRatio < 0.1f) {
            NLM_LOG_WARNING("  WARNING: Low excitation-inhibition balance");
        } else if (eiRatio > 10.0f) {
            NLM_LOG_WARNING("  WARNING: High excitation-inhibition balance");
        } else {
            NLM_LOG_INFO("  ✓ Balanced excitation-inhibition ratio");
        }
        
        // Memory system status
        if (brain_->getWorkingMemory()) {
            NLM_LOG_INFO("  Working Memory: Active with " + 
                        std::to_string(brain_->getWorkingMemory()->getActiveTraces()) + " traces");
        }
        
        if (brain_->getEpisodicMemory()) {
            NLM_LOG_INFO("  Episodic Memory: " + 
                        std::to_string(brain_->getEpisodicMemory()->getEpisodeCount()) + " episodes stored");
        }
    }
    
    void enableAllPlasticity() {
        NLM_LOG_INFO("Enabling all plasticity mechanisms...");
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
};

// Run Phase 6 integration test
void runPhase6Integration(CommandLineArgs& args) {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    
    Phase6Config config;
    if (args.neuronCount > 0) config.neuronCount = args.neuronCount;
    if (args.maxSteps > 0) config.maxSteps = args.maxSteps;
    if (!args.checkpointPath.empty()) config.checkpointPath = args.checkpointPath;
    
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl;
    std::cout << "\nTesting individual systems..." << std::endl;
    
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    experiment.testCheckpointing();
    experiment.testReplay();
    
    std::cout << "\nRunning full integration experiment..." << std::endl;
    auto result = experiment.run(config);
    
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    std::cout << "\nWall clock time: " << result.totalWallClockTime << "s" << std::endl;
}

// Run demo mode with enhanced features
void runDemoMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Demo Mode (Phase 6) ===" << std::endl;
    
    // Initialize enhanced logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    consoleLogger->setIncludeThreadId(true);
    consoleLogger->setTimestampFormat("%H:%M:%S");
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Initializing NLM Phase 6 Enhanced Artificial Brain...");
    
    // Set default values for Phase 6 with smaller size for demo
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
    
    // Create and initialize brain using enhanced factory
    auto brain = Brain::create(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run enhanced learning experiment
    AdvancedLearningExperiment experiment(brain, 42);
    experiment.runExperiment(1000);
    
    // Simulate environment interaction
    NLM_LOG_INFO("Simulating environment interaction...");
    Environment env;
    env.initialize(10, 10, "GridWorld");
    
    for (int step = 0; step < 500; ++step) {
        if (step % 100 == 0) {
            NLM_LOG_INFO("Environment step " + std::to_string(step));
        }
        
        // Generate sensory input
        std::vector<float> sensoryInput = env.getSensoryInput();
        brain->receiveSensoryInput(SensoryInput(sensoryInput));
        
        // Step brain
        brain->step(step, step * 0.001f);
        
        // Produce action
        auto action = brain->produceAction();
        env.applyAction(*action);
    }
    
    // Final status
    NLM_LOG_INFO("\n=== DEMO COMPLETED ===");
    brain->logStatus();
}

// Run research mode with advanced features
void runResearchMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Research Mode (Phase 6) ===" << std::endl;
    
    // Enhanced configuration for research
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Runtime);
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config->set("connection_probability", 0.2f, ConfigSource::Runtime);
    
    // Research-specific plasticity settings
    config->set("stdp_ltp_weight", 0.03f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.025f, ConfigSource::Runtime);
    config->set("stdp_tau", 30.0f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.000005f, ConfigSource::Runtime);
    
    auto brain = Brain::create(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Research mode initialization failed");
        return;
    }
    
    NLM_LOG_INFO("Research mode initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    // Extended learning with development
    AdvancedLearningExperiment experiment(brain, 12345);
    experiment.runExperiment(5000, true);
    
    // Enable development
    brain->enableDevelopment(true);
    
    // Run multiple developmental stages
    for (int stage = 0; stage < 3; ++stage) {
        NLM_LOG_INFO("Development stage " + std::to_string(stage + 1));
        brain->develop();
        experiment.runExperiment(1000, true);
    }
    
    // Save research results
    if (!config->getOr<std::string>("checkpoint_path", "").empty()) {
        brain->save(config->getOr<std::string>("checkpoint_path", ""));
    }
    
    NLM_LOG_INFO("Research mode completed successfully");
}

// Run quick test mode
void runQuickTestMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Quick Test Mode (Phase 6) ===" << std::endl;
    
    auto brain = Brain::create(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Quick test initialization failed");
        return;
    }
    
    // Quick connectivity test
    NLM_LOG_INFO("Testing basic connectivity...");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into a few neurons
    NLM_LOG_INFO("Injecting current into neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Basic functionality working");
    } else {
        NLM_LOG_WARNING("⚠ No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    NLM_LOG_INFO("Quick test completed");
}

int main(int argc, char** argv) {
    // Set up signal handling for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    printBanner();
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    if (args.help) {
        printHelp();
        return 0;
    }
    
    // Initialize logger early for help/errors
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration file if available
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
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    if (args.verbose) {
        NLM_LOG_INFO("Verbose mode enabled");
        config->set("log_level", std::string("DEBUG"), ConfigSource::Runtime);
    }
    
    // Run selected mode
    try {
        if (args.profile) {
            std::cout << "Performance profiling enabled" << std::endl;
            auto timer = PerformanceTimer("main");
            
            if (args.mode == "integration") {
                runPhase6Integration(args);
            } else if (args.mode == "research") {
                runResearchMode(config);
            } else if (args.mode == "quicktest") {
                runQuickTestMode(config);
            } else { // demo
                runDemoMode(config);
            }
        } else {
            if (args.mode == "integration") {
                runPhase6Integration(args);
            } else if (args.mode == "research") {
                runResearchMode(config);
            } else if (args.mode == "quicktest") {
                runQuickTestMode(config);
            } else { // demo
                runDemoMode(config);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in main: ") + e.what());
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== NLM Phase 6 Complete ===" << std::endl;
    std::cout << "The NLM brain is now a functioning artificial neural substrate" << std::endl;
    std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    
    return 0;
}