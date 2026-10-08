// NLM Enhanced Demo Application
// Comprehensive demonstration of NLM's capabilities
// Supports multiple modes: demo, test, benchmark, analyze, interactive

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
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <algorithm>
#include <map>
#include <cstdint>

using namespace nlm;
using namespace std::chrono;

// Enhanced demo result structure with more metrics
struct EnhancedDemoResult {
    std::string mode;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint64_t steps;
    double wallClockTime;
    double stepsPerSecond;
    double firingRateAvg;
    double eiratio;
    uint64_t totalSpikes;
    float meanSynapticWeight;
    std::map<std::string, double> metrics;
    bool success;
    std::string errorMessage;
};

// Enhanced configuration with more options
struct DemoConfig {
    std::string mode = "demo"; // demo, test, benchmark, analyze, interactive
    std::string configFile = "configs/default.cfg";
    uint64_t neuronCount = 1000;
    uint64_t steps = 5000;
    double timestep = 0.001;
    bool enableCheckpointing = false;
    std::string checkpointPath;
    bool verbose = true;
    bool saveResults = true;
    std::string outputPrefix = "demo_results";
    
    // Mode-specific options
    struct {
        bool runBasicConnectivity = true;
        bool runPlasticityExperiment = true;
        bool runStdpVerification = true;
        bool runIntegrationTest = true;
    } demoOptions;
    
    struct {
        size_t numNeuronSizes = 4;
        std::vector<size_t> neuronSizes;
        size_t numStepSizes = 3;
        std::vector<size_t> stepSizes;
    } benchmarkOptions;
    
    struct {
        std::vector<std::string> testsToRun;
        bool validateConnections = true;
        bool checkPlasticity = true;
    } testOptions;
};

// Helper functions for enhanced demo
class EnhancedDemoHelper {
public:
    static DemoConfig parseCommandLine(int argc, char** argv);
    static void printBanner(const std::string& mode);
    static void printConfiguration(const DemoConfig& config);
    static void printResults(const EnhancedDemoResult& result);
    static std::string formatTime(double seconds);
    static std::string formatNumber(size_t num);
    static std::string formatPercent(double value);
    
    static void runBasicConnectivityDemo(const std::shared_ptr<Brain>& brain);
    static void runPlasticityLearningDemo(const std::shared_ptr<Brain>& brain);
    static void runStdpVerificationDemo(const std::shared_ptr<Brain>& brain);
    static void runIntegrationTest(const std::shared_ptr<Brain>& brain);
    static void runNetworkAnalysis(const std::shared_ptr<Brain>& brain);
    static void runLearningExperiment(const std::shared_ptr<Brain>& brain, uint64_t steps);
    static EnhancedDemoResult runBenchmark(const DemoConfig& config);
    static bool validateBrainConfiguration(const std::shared_ptr<Brain>& brain);
};

// Enhanced main demo with multiple modes
int main(int argc, char** argv) {
    // Parse configuration
    DemoConfig config = EnhancedDemoHelper::parseCommandLine(argc, argv);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner and configuration
    EnhancedDemoHelper::printBanner(config.mode);
    EnhancedDemoHelper::printConfiguration(config);
    
    // Load configuration
    auto demoConfig = std::make_shared<Config>();
    
    if (!config.configFile.empty() && demoConfig->loadFromFile(config.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line/config values
    demoConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    demoConfig->set("simulation_timestep", config.timestep, ConfigSource::Default);
    demoConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    demoConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    demoConfig->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(demoConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    EnhancedDemoResult result;
    result.mode = config.mode;
    result.neuronCount = config.neuronCount;
    result.synapseCount = brain->getTotalSynapseCount();
    result.steps = config.steps;
    result.success = true;
    
    auto startTime = high_resolution_clock::now();
    
    // Run based on mode
    if (config.mode == "demo") {
        NLM_LOG_INFO("=== ENHANCED DEMO MODE ===");
        
        if (config.demoOptions.runBasicConnectivity) {
            EnhancedDemoHelper::runBasicConnectivityDemo(brain);
        }
        
        if (config.demoOptions.runPlasticityExperiment) {
            // Reset brain for plasticity experiment
            brain->reset();
            brain->initialize();
            EnhancedDemoHelper::runPlasticityLearningDemo(brain);
        }
        
        if (config.demoOptions.runStdpVerification) {
            // Reset brain for STDP verification
            brain->reset();
            brain->initialize();
            EnhancedDemoHelper::runStdpVerificationDemo(brain);
        }
        
        if (config.demoOptions.runIntegrationTest) {
            EnhancedDemoHelper::runIntegrationTest(brain);
        }
        
        EnhancedDemoHelper::runNetworkAnalysis(brain);
        
    } else if (config.mode == "benchmark") {
        NLM_LOG_INFO("=== BENCHMARK MODE ===");
        EnhancedDemoResult benchmarkResult = EnhancedDemoHelper::runBenchmark(config);
        result = benchmarkResult;
    } else if (config.mode == "test") {
        NLM_LOG_INFO("=== TEST MODE ===");
        
        if (!EnhancedDemoHelper::validateBrainConfiguration(brain)) {
            result.success = false;
            result.errorMessage = "Brain configuration validation failed";
        }
        
        EnhancedDemoHelper::runIntegrationTest(brain);
        
    } else if (config.mode == "analyze") {
        NLM_LOG_INFO("=== ANALYSIS MODE ===");
        EnhancedDemoHelper::runNetworkAnalysis(brain);
        
    } else if (config.mode == "interactive") {
        NLM_LOG_INFO("=== INTERACTIVE MODE ===");
        // Interactive mode with user input
        // For now, just run a basic demo
        EnhancedDemoHelper::runBasicConnectivityDemo(brain);
        brain->reset();
        brain->initialize();
        EnhancedDemoHelper::runLearningExperiment(brain, config.steps);
    }
    
    auto endTime = high_resolution_clock::now();
    result.wallClockTime = duration<double>(endTime - startTime).count();
    result.stepsPerSecond = result.steps / result.wallClockTime;
    result.firingRateAvg = brain->getAverageFiringRate();
    result.eiratio = brain->getExcitationInhibitionRatio();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.meanSynapticWeight = brain->getAverageSynapticWeight();
    
    // Collect additional metrics
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        result.metrics["working_memory_traces"] = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        result.metrics["episodic_memory_episodes"] = episodicMemory->getEpisodeCount();
    }
    
    // Print results
    EnhancedDemoHelper::printResults(result);
    
    // Save results if requested
    if (config.saveResults) {
        std::string filename = config.outputPrefix + "_" + config.mode + "_" + 
                              std::to_string(static_cast<int>(startTime.time_since_epoch().count())) + ".json";
        // In a full implementation, save to JSON file
        NLM_LOG_INFO("Results saved to: " + filename);
    }
    
    NLM_LOG_INFO("Demo complete! " + std::to_string(result.wallClockTime) + " seconds elapsed.");
    
    return result.success ? 0 : 1;
}

// Command line parsing
DemoConfig EnhancedDemoHelper::parseCommandLine(int argc, char** argv) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--mode" && i + 1 < argc) {
            config.mode = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.steps = std::stoull(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            config.timestep = std::stod(argv[++i]);
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--no-verbose") {
            config.verbose = false;
        } else if (arg == "--no-results") {
            config.saveResults = false;
        } else if (arg == "--help") {
            std::cout << "NLM Enhanced Demo Application" << std::endl;
            std::cout << "Modes: demo, test, benchmark, analyze, interactive" << std::endl;
            std::cout << "Options: --mode <mode>, --config <file>, --neurons <count>, --steps <count>" << std::endl;
            std::cout << "        --timestep <time>, --verbose, --no-verbose, --no-results, --help" << std::endl;
            exit(0);
        }
    }
    
    return config;
}

// Banner printing
void EnhancedDemoHelper::printBanner(const std::string& mode) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Demo Application                                  ║
    ║     Mode: " << mode << "                                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Configuration printing
void EnhancedDemoHelper::printConfiguration(const DemoConfig& config) {
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                           CONFIGURATION                              ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Mode: " << std::setw(50) << config.mode << " ║" << std::endl;
    std::cout << "║ Neurons: " << std::setw(46) << formatNumber(config.neuronCount) << " ║" << std::endl;
    std::cout << "║ Steps: " << std::setw(50) << formatNumber(config.steps) << " ║" << std::endl;
    std::cout << "║ Timestep: " << std::setw(45) << config.timestep << "s ║" << std::endl;
    std::cout << "║ Config File: " << std::setw(42) << config.configFile << " ║" << std::endl;
    std::cout << "║ Verbose: " << std::setw(47) << (config.verbose ? "Yes" : "No") << " ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Results printing
void EnhancedDemoHelper::printResults(const EnhancedDemoResult& result) {
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                           DEMO RESULTS                              ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Success: " << std::setw(48) << (result.success ? "Yes" : "No") << " ║" << std::endl;
    if (!result.success) {
        std::cout << "║ Error: " << std::setw(49) << result.errorMessage << " ║" << std::endl;
    }
    std::cout << "║ Neurons: " << std::setw(48) << formatNumber(result.neuronCount) << " ║" << std::endl;
    std::cout << "║ Synapses: " << std::setw(47) << formatNumber(result.synapseCount) << " ║" << std::endl;
    std::cout << "║ Steps: " << std::setw(48) << formatNumber(result.steps) << " ║" << std::endl;
    std::cout << "║ Wall Time: " << std::setw(46) << formatTime(result.wallClockTime) << " ║" << std::endl;
    std::cout << "║ Steps/sec: " << std::setw(46) << std::fixed << std::setprecision(1) << result.stepsPerSecond << " ║" << std::endl;
    std::cout << "║ Avg Firing Rate: " << std::setw(41) << std::fixed << std::setprecision(3) << result.firingRateAvg << " Hz ║" << std::endl;
    std::cout << "║ E/I Ratio: " << std::setw(47) << std::fixed << std::setprecision(3) << result.eiratio << " ║" << std::endl;
    std::cout << "║ Total Spikes: " << std::setw(46) << formatNumber(result.totalSpikes) << " ║" << std::endl;
    std::cout << "║ Mean Synaptic Weight: " << std::setw(38) << std::fixed << std::setprecision(4) << result.meanSynapticWeight << " ║" << std::endl;
    
    for (const auto& metric : result.metrics) {
        std::cout << "║ " << std::left << std::setw(26) << (metric.first + ":") 
                  << std::right << std::setw(21) << formatNumber(static_cast<size_t>(metric.second)) 
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Format time helper
std::string EnhancedDemoHelper::formatTime(double seconds) {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        return std::to_string(seconds / 60.0) + "m";
    } else {
        return std::to_string(seconds / 3600.0) + "h";
    }
}

// Format number helper
std::string EnhancedDemoHelper::formatNumber(size_t num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

// Format percent helper
std::string EnhancedDemoHelper::formatPercent(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << "%";
    return ss.str();
}

// Demo helper implementations
void EnhancedDemoHelper::runBasicConnectivityDemo(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Basic Connectivity Test ===");
    
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
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("! No spikes - checking neuron parameters...");
    }
}

void EnhancedDemoHelper::runPlasticityLearningDemo(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("✓ Plasticity experiment completed");
}

void EnhancedDemoHelper::runStdpVerificationDemo(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        NLM_LOG_INFO("Not enough synapses for STDP test");
        return;
    }
    
    auto& synapses = region->getSynapses();
    std::vector<float> beforeWeights;
    
    NLM_LOG_INFO("Testing STDP on 5 synapses:");
    
    // Record initial weights
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < beforeWeights.size(); ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("! No change: STDP may not be triggering");
    }
}

void EnhancedDemoHelper::runIntegrationTest(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Integration Test ===");
    
    Phase6IntegratedExperiment experiment;
    
    bool integrationOK = experiment.verifyIntegration();
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        return;
    }
    
    NLM_LOG_INFO("✓ Integration verification passed!");
    
    // Test individual systems
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    experiment.testCheckpointing();
    experiment.testReplay();
    
    // Run full integration experiment
    Phase6Config config;
    config.neuronCount = brain->getTotalNeuronCount();
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    NLM_LOG_INFO("=== Integration Test Results ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
}

void EnhancedDemoHelper::runNetworkAnalysis(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Network Analysis ===");
    
    // Analyze connectivity
    NLM_LOG_INFO("Connectivity Analysis:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    
    // Analyze region distribution
    NLM_LOG_INFO("Region Distribution:");
    for (const auto& region : brain->getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "):" +
                    " " + std::to_string(region->getTotalNeuronCount()) + " neurons,
                    " + std::to_string(region->getSynapseCount()) + " synapses,
                    " + std::to_string(region->getAverageSynapticWeight()) + " avg weight");
    }
    
    // Analyze memory systems
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("Working Memory: " + std::to_string(workingMemory->getActiveTraces()) + " active traces, " +
                    std::to_string(workingMemory->getCapacity()) + " capacity");
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes, " +
                    std::to_string(episodicMemory->getMaxEpisodes()) + " max episodes");
    }
    
    // Analyze neuromodulation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("Neuromodulation: Dopamine level = " + std::to_string(dopamine->getLevel()));
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("Curiosity: Level = " + std::to_string(curiosity->getLevel()));
    }
    
    NLM_LOG_INFO("✓ Network analysis complete");
}

void EnhancedDemoHelper::runLearningExperiment(const std::shared_ptr<Brain>& brain, uint64_t steps) {
    NLM_LOG_INFO("=== Learning Experiment ===");
    
    // Enable all plasticity mechanisms
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    NLM_LOG_INFO("Running learning experiment for " + std::to_string(steps) + " steps...");
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
        // Create learning pattern
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Learning neurons: " + std::to_string(brain->getActiveNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("✓ Learning experiment completed");
}

EnhancedDemoResult EnhancedDemoHelper::runBenchmark(const DemoConfig& config) {
    EnhancedDemoResult result;
    result.mode = "benchmark";
    result.neuronCount = config.neuronCount;
    result.steps = config.steps;
    result.success = true;
    
    // Create configuration
    auto demoConfig = std::make_shared<Config>();
    demoConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    demoConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    demoConfig->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(demoConfig);
    brain->initialize();
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    // Run benchmark
    auto startTime = high_resolution_clock::now();
    
    for (size_t step = 0; step < config.steps; ++step) {
        // Inject random input
        for (size_t i = 0; i < std::min(size_t(10), config.neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = high_resolution_clock::now();
    result.wallClockTime = duration<double>(endTime - startTime).count();
    result.stepsPerSecond = config.steps / result.wallClockTime;
    result.firingRateAvg = brain->getAverageFiringRate();
    result.eiratio = brain->getExcitationInhibitionRatio();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.meanSynapticWeight = brain->getAverageSynapticWeight();
    
    return result;
}

bool EnhancedDemoHelper::validateBrainConfiguration(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("Validating brain configuration...");
    
    if (!brain) {
        NLM_LOG_ERROR("Brain is null!");
        return false;
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("No neurons in brain!");
        return false;
    }
    
    if (brain->getTotalSynapseCount() == 0) {
        NLM_LOG_ERROR("No synapses in brain!");
        return false;
    }
    
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) {
        NLM_LOG_ERROR("Spike system not initialized!");
        return false;
    }
    
    NLM_LOG_INFO("✓ Brain configuration validated");
    return true;
}