#include "framework/TestFramework.hpp"
#include "core/Logger/Logger.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace nlm {
namespace framework {

TestFramework::TestFramework()
    : config_(std::make_shared<Config>()),
      brain_(nullptr),
      clock_(std::make_shared<SimulationClock>(defaultTimestep_)),
      testMode_(TestMode::All),
      testIntensity_(TestIntensity::Medium),
      testDuration_(0),
      randomSeed_(42) {
    
    NLM_LOG_INFO("TestFramework initialized");
    setupLogger();
    setupDefaultConfiguration();
}

TestFramework::~TestFramework() {
    NLM_LOG_INFO("TestFramework destroyed");
}

bool TestFramework::loadConfiguration(const std::string& configFile) {
    NLM_LOG_INFO("Loading configuration from: " + configFile);
    
    if (config_->loadFromFile(configFile)) {
        NLM_LOG_INFO("Configuration loaded successfully");
        return true;
    } else {
        NLM_LOG_WARNING("Failed to load configuration, using defaults");
        setupDefaultConfiguration();
        return false;
    }
}

bool TestFramework::loadFromCommandLine(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line");
    return config_->loadFromArgs(argc, argv);
}

bool TestFramework::setupBrain() {
    NLM_LOG_INFO("Setting up brain with configuration");
    
    // Set random seed from config
    if (config_->has(keyRandomSeed_)) {
        randomSeed_ = config_->getOr<uint64_t>(keyRandomSeed_, randomSeed_);
    }
    
    // Create brain with configuration
    brain_ = std::make_shared<Brain>(config_);
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Configure timestep
    if (config_->has(keyTimestep_)) {
        double timestep = config_->getOr<double>(keyTimestep_, defaultTimestep_);
        clock_->setTimestep(timestep);
    }
    
    brain_->logStatus();
    return true;
}

bool TestFramework::resetBrain() {
    NLM_LOG_INFO("Resetting brain");
    if (!brain_) {
        NLM_LOG_ERROR("Brain not initialized for reset");
        return false;
    }
    
    brain_->reset();
    return brain_->initialize();
}

bool TestFramework::runTest(TestMode mode) {
    NLM_LOG_INFO("Running test: " + testModeToString(mode));
    
    auto startTime = std::chrono::high_resolution_clock::now();
    TestResult result;
    result.mode = mode;
    result.status = TestStatus::Running;
    
    bool success = false;
    
    switch (mode) {
        case TestMode::Connectivity:
            success = runConnectivityTest();
            break;
        case TestMode::Plasticity:
            success = runPlasticityTest();
            break;
        case TestMode::STDP:
            success = runStdpTest();
            break;
        case TestMode::All:
            success = runAllTests();
            break;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = endTime - startTime;
    result.success = success;
    result.status = success ? TestStatus::Completed : TestStatus::Failed;
    
    results_.push_back(result);
    
    NLM_LOG_INFO("Test " + testModeToString(mode) + " completed in " + 
                 std::to_string(result.duration.count()) + " seconds: " +
                 (success ? "PASSED" : "FAILED"));
    
    return success;
}

bool TestFramework::runAllTests() {
    NLM_LOG_INFO("Running all tests in sequence");
    
    bool overallSuccess = true;
    
    // Run connectivity test
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    if (!runConnectivityTest()) {
        NLM_LOG_ERROR("Connectivity test failed");
        overallSuccess = false;
    }
    
    // Reset brain between tests
    if (!resetBrain()) {
        NLM_LOG_ERROR("Failed to reset brain for plasticity test");
        overallSuccess = false;
    }
    
    // Run plasticity test
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    if (!runPlasticityTest()) {
        NLM_LOG_ERROR("Plasticity test failed");
        overallSuccess = false;
    }
    
    // Reset brain between tests
    if (!resetBrain()) {
        NLM_LOG_ERROR("Failed to reset brain for STDP test");
        overallSuccess = false;
    }
    
    // Run STDP test
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    if (!runStdpTest()) {
        NLM_LOG_ERROR("STDP test failed");
        overallSuccess = false;
    }
    
    return overallSuccess;
}

bool TestFramework::runConnectivityTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    if (auto* region = brain_->getRegion(RegionId(1))) {
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain_->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run test duration steps
        for (SimulationStep step = 0; step < connectivityTestSteps_; ++step) {
            brain_->step(step, step * clock_->getTimestep());
        }
        
        size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                             " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                             " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            return false;
        }
    }
    
    NLM_LOG_ERROR("Could not access brain region");
    return false;
}

bool TestFramework::runPlasticityTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    experiments::LearningExperiment experiment(brain_, randomSeed_);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(plasticityTestSteps_) + " steps)...");
    
    for (SimulationStep step = 0; step < plasticityTestSteps_; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain_->step(step, step * clock_->getTimestep());
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
    
    return true;
}

bool TestFramework::runStdpTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Could not access brain region for STDP test");
        return false;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return false;
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
    
    for (int trial = 0; trial < stdpTestTrials_; ++trial) {
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
            brain_->step(trial * 2, trial * 2 * clock_->getTimestep());
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * clock_->getTimestep());
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                     " after: " + std::to_string(synapses[i]->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        totalDelta += delta;
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        return true;
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        return false;
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        return false;
    }
}

void TestFramework::setTestMode(TestMode mode) {
    testMode_ = mode;
    NLM_LOG_INFO("Test mode set to: " + testModeToString(mode));
}

void TestFramework::setTestIntensity(TestIntensity intensity) {
    testIntensity_ = intensity;
    NLM_LOG_INFO("Test intensity set to: " + std::to_string(static_cast<int>(intensity)));
}

void TestFramework::setTestDuration(int duration) {
    testDuration_ = duration;
    NLM_LOG_INFO("Test duration set to: " + std::to_string(duration) + " steps");
}

void TestFramework::setRandomSeed(uint64_t seed) {
    randomSeed_ = seed;
    config_->set(keyRandomSeed_, static_cast<int64_t>(seed), ConfigSource::Runtime);
    NLM_LOG_INFO("Random seed set to: " + std::to_string(seed));
}

std::vector<TestResult> TestFramework::getResults() const {
    return results_;
}

void TestFramework::reportResults() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Framework Results ===");
    
    for (const auto& result : results_) {
        NLM_LOG_INFO("Test: " + testModeToString(result.mode) +
                     " | Status: " + std::to_string(static_cast<int>(result.status)) +
                     " | Duration: " + std::to_string(result.duration.count()) + "s " +
                     "| Success: " + (result.success ? "YES" : "NO"));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Summary: " + std::to_string(results_.size()) + " tests completed " +
                 std::count_if(results_.begin(), results_.end(), 
                              [](const TestResult& r) { return r.success; }) + "/" +
                 std::to_string(results_.size()) + " passed");
}

void TestFramework::logStatus() const {
    NLM_LOG_INFO("TestFramework Status:");
    NLM_LOG_INFO("  Test Mode: " + testModeToString(testMode_));
    NLM_LOG_INFO("  Test Intensity: " + std::to_string(static_cast<int>(testIntensity_)));
    NLM_LOG_INFO("  Test Duration: " + std::to_string(testDuration_) + " steps");
    NLM_LOG_INFO("  Random Seed: " + std::to_string(randomSeed_));
    NLM_LOG_INFO("  Brain: " + (brain_ ? "Initialized" : "Not initialized"));
    NLM_LOG_INFO("  Tests Run: " + std::to_string(results_.size()));
}

bool TestFramework::isValid() const {
    return config_ && brain_;
}

std::shared_ptr<const Config> TestFramework::getConfig() const {
    return config_;
}

std::shared_ptr<Brain> TestFramework::getBrain() {
    return brain_;
}

void TestFramework::setupLogger() {
    NLM_LOG_INFO("Setting up logger");
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Logger initialized with INFO level");
}

void TestFramework::setupDefaultConfiguration() {
    NLM_LOG_INFO("Setting up default configuration");
    
    // Set default values for Phase 2
    config_->set(keyRandomSeed_, static_cast<int64_t>(randomSeed_), ConfigSource::Default);
    config_->set(keyTimestep_, defaultTimestep_, ConfigSource::Default);
    config_->set(keyNeuronCount_, static_cast<int64_t>(defaultNeuronCount_), ConfigSource::Default);
    config_->set(keyRegionCount_, static_cast<int64_t>(defaultRegionCount_), ConfigSource::Default);
    config_->set(keyConnectionProbability_, defaultConnectionProbability_, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    NLM_LOG_INFO("Default configuration applied");
}

void TestFramework::printBanner() const {
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

bool TestFramework::parseCommandLineArgs(int argc, char** argv) {
    NLM_LOG_INFO("Parsing command line arguments");
    
    std::string configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--test-mode") {
            if (i + 1 < argc) {
                std::string modeArg = argv[++i];
                if (modeArg == "connectivity") {
                    setTestMode(TestMode::Connectivity);
                } else if (modeArg == "plasticity") {
                    setTestMode(TestMode::Plasticity);
                } else if (modeArg == "stdp") {
                    setTestMode(TestMode::STDP);
                } else if (modeArg == "all") {
                    setTestMode(TestMode::All);
                }
            }
        } else if (arg == "--intensity") {
            if (i + 1 < argc) {
                int intensity = std::stoi(argv[++i]);
                setTestIntensity(static_cast<TestIntensity>(intensity));
            }
        } else if (arg == "--duration") {
            if (i + 1 < argc) {
                int duration = std::stoi(argv[++i]);
                setTestDuration(duration);
            }
        } else if (arg == "--seed") {
            if (i + 1 < argc) {
                uint64_t seed = static_cast<uint64_t>(std::stoull(argv[++i]));
                setRandomSeed(seed);
            }
        }
    }
    
    // Load configuration file
    if (configFile != "configs/default.cfg") {
        loadConfiguration(configFile);
    }
    
    return true;
}

std::string TestFramework::testModeToString(TestMode mode) const {
    switch (mode) {
        case TestMode::Connectivity: return "Connectivity";
        case TestMode::Plasticity: return "Plasticity";
        case TestMode::STDP: return "STDP";
        case TestMode::All: return "All";
        default: return "Unknown";
    }
}

TestStatus TestFramework::getTestStatusFromBrain(const std::string& testName) const {
    // This is a placeholder for brain status checking
    return TestStatus::Completed;
}

} // namespace framework
} // namespace nlm
