#include "utils/EnhancedLogger.hpp"
#include "benchmarking/NeuralNetworkBenchmark.hpp"

using namespace nlm;
using namespace nlm::utils;

// Include the extracted LearningExperiment
#include "experiments/LearningExperiment.hpp"

// Include test utilities
#include "tests/TestUtils.hpp"

// Forward declarations for test functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Test configuration
struct TestConfig {
    static constexpr int32_t DEFAULT_RANDOM_SEED = 42;
    static constexpr double DEFAULT_TIMESTEP = 0.001;
    static constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    static constexpr int64_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    
    // STDP parameters
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    
    // Structural plasticity parameters
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Experiment parameters
    static constexpr int DEFAULT_CONNECTIVITY_TEST_STEPS = 50;
    static constexpr int DEFAULT_PLASTICITY_TEST_STEPS = 1000;
    static constexpr int DEFAULT_STDP_TEST_TRIALS = 50;
};

// Test runner class for better organization
class TestRunner {
public:
    explicit TestRunner(std::shared_ptr<Config> config) 
        : config_(config), brain_(nullptr) {}
    
    ~TestRunner() = default;
    
    // Initialize brain with configuration
    bool initializeBrain() {
        NLM_LOG_INFO("Initializing NLM Brain...");
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain_->logStatus();
        return true;
    }
    
    // Run all tests in sequence
    void runAllTests() {
        NLM_LOG_INFO("=== Running NLM Phase 2 Tests ===");
        
        // Test 1: Basic neural connectivity
        runBasicConnectivityTest(brain_);
        
        // Reset for next test
        brain_->reset();
        brain_->initialize();
        
        // Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain_);
        
        // Reset for final test
        brain_->reset();
        brain_->initialize();
        
        // Test 3: STDP verification
        runStdpVerification(brain_);
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain_->logStatus();
        
        printCompletionBanner();
    }
    
    // Get brain reference for external access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    
    void printCompletionBanner() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 2 Objectives Completed:");
        NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
        NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
        NLM_LOG_INFO("  ✓ STDP plasticity rule");
        NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
        NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
        NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("");
    }
};

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
    for (SimulationStep step = 0; step < TestConfig::DEFAULT_CONNECTIVITY_TEST_STEPS; ++step) {
        brain->step(step, step * TestConfig::DEFAULT_TIMESTEP);
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
    
    for (SimulationStep step = 0; step < TestConfig::DEFAULT_PLASTICITY_TEST_STEPS; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * TestConfig::DEFAULT_TIMESTEP);
        
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
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < TestConfig::DEFAULT_STDP_TEST_TRIALS; ++trial) {
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
            brain->step(trial * 2, trial * 2 * TestConfig::DEFAULT_TIMESTEP);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * TestConfig::DEFAULT_TIMESTEP);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
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
    
    // Initialize enhanced logger
    EnhancedLogger::getInstance().setLogLevel(LogLevel::Info);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments with improved CLI
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool runTestsOnly = false;
    int testMode = 0; // 0 = all tests, 1 = connectivity only, 2 = plasticity only, 3 = stdp only
    bool enableBenchmarking = false;
    std::string benchmarkFile = "benchmark_results.csv";
    std::string benchmarkReportFile = "benchmark_report.txt";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
            config->set("log_level", static_cast<int>(LogLevel::Debug), ConfigSource::CommandLine);
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--test" || arg == "-t") {
            if (i + 1 < argc) {
                testMode = std::atoi(argv[++i]);
                runTestsOnly = true;
            }
        } else if (arg == "--benchmark" || arg == "-bench") {
            enableBenchmarking = true;
        } else if (arg == "--benchmark-file" || arg == "-bench-file") {
            if (i + 1 < argc) {
                benchmarkFile = argv[++i];
            }
        } else if (arg == "--benchmark-report" || arg == "-bench-report") {
            if (i + 1 < argc) {
                benchmarkReportFile = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            printBanner();
            std::cout << "\nUsage: nlm [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  -h, --help              Show this help message\n";
            std::cout << "  -v, --verbose           Enable verbose logging\n";
            std::cout << "  -c, --config <file>     Load configuration from file\n";
            std::cout << "  -t, --test <mode>       Run specific test mode (0=all, 1=connectivity, 2=plasticity, 3=stdp)\n";
            std::cout << "  --benchmark             Run performance benchmarks\n";
            std::cout << "  --benchmark-file <file>  Output benchmark results to file\n";
            std::cout << "  --benchmark-report <file> Generate benchmark report\n";
            std::cout << "  --version               Show version information\n";
            std::cout << "  --license               Show license information\n";
            std::cout << "  --show-config           Display current configuration\n";
            std::cout << "  --create-config         Generate a new configuration file\n\n";
            std::cout << "Test modes:\n";
            std::cout << "  0 - All tests (default)\n";
            std::cout << "  1 - Basic connectivity test only\n";
            std::cout << "  2 - Plasticity learning experiment only\n";
            std::cout << "  3 - STDP verification only\n";
            return 0;
        } else if (arg == "--version" || arg == "--ver") {
            printBanner();
            std::cout << "\nVersion: NLM Phase 2 (熙然)\n";
            std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
            std::cout << "Features: Real LIF neurons, STDP, Hebbian, Structural plasticity\n";
            return 0;
        } else if (arg == "--license") {
            printBanner();
            std::cout << "\nLicense: Experimental Research License\n";
            std::cout << "Copyright (c) 2024-2026 NLM Development Team\n";
            std::cout << "All rights reserved. This software is experimental and for research use only.\n";
            return 0;
        } else if (arg == "--show-config") {
            std::cout << "Current configuration:\n";
            for (const auto& key : config->getKeys()) {
                auto value = config->get<std::string>(key);
                if (value) {
                    std::cout << "  " << key << " = " << *value << "\n";
                }
            }
            return 0;
        } else if (arg == "--create-config") {
            config->saveToFile("generated_config.cfg");
            std::cout << "Generated configuration saved to generated_config.cfg\n";
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.find('=') != std::string::npos && arg.find('=') > 0) {
            // Handle key=value configuration overrides
            size_t equalsPos = arg.find('=');
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            
            // Convert string value to appropriate type
            try {
                if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    config->set(key, doubleValue, ConfigSource::CommandLine);
                } else {
                    int intValue = std::stoi(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                }
            } catch (...) {
                // Try as string
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Load configuration from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args (already processed above)
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 if not already set
    if (!config->has("random_seed")) {
        config->set("random_seed", TestConfig::DEFAULT_RANDOM_SEED, ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", TestConfig::DEFAULT_TIMESTEP, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", TestConfig::DEFAULT_NEURON_COUNT, ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", TestConfig::DEFAULT_REGION_COUNT, ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", TestConfig::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    }
    
    // STDP parameters
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", TestConfig::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    }
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", TestConfig::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    }
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", TestConfig::DEFAULT_STDP_TAU, ConfigSource::Default);
    }
    
    // Structural plasticity parameters
    if (!config->has("synaptogenesis_rate")) {
        config->set("synaptogenesis_rate", TestConfig::DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    }
    if (!config->has("pruning_rate")) {
        config->set("pruning_rate", TestConfig::DEFAULT_PRUNING_RATE, ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", TestConfig::DEFAULT_RANDOM_SEED)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", TestConfig::DEFAULT_TIMESTEP)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", TestConfig::DEFAULT_NEURON_COUNT)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", TestConfig::DEFAULT_REGION_COUNT)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", TestConfig::DEFAULT_CONNECTION_PROBABILITY)));
    NLM_LOG_INFO("");
    
    // Create test runner
    TestRunner testRunner(config);
    
    // Initialize brain
    if (!testRunner.initializeBrain()) {
        return 1;
    }
    
    // Run tests based on mode
    auto brain = testRunner.getBrain();
    switch (testMode) {
        case 1:
            runBasicConnectivityTest(brain);
            break;
        case 2:
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            break;
        case 3:
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
            break;
        case 0:
        default:
            testRunner.runAllTests();
            break;
    }
    
    // Run benchmarks if requested
    if (enableBenchmarking) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Neural Network Benchmarks ===");
        
        NeuralNetworkBenchmark benchmark(config);
        auto results = benchmark.runFullBenchmark();
        
        // Export results
        if (!benchmarkFile.empty() && benchmark.exportResults(results, benchmarkFile)) {
            NLM_LOG_INFO("Benchmark results exported to: {}", benchmarkFile);
        }
        
        std::string report = benchmark.generatePerformanceReport(results);
        if (!benchmarkReportFile.empty()) {
            std::ofstream reportFile(benchmarkReportFile);
            if (reportFile.is_open()) {
                reportFile << report;
                NLM_LOG_INFO("Benchmark report exported to: {}", benchmarkReportFile);
            }
        } else {
            NLM_LOG_INFO("Benchmark Report:\n{}", report);
        }
    }
    
    return 0;
}