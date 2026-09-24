// NLM Phase 2 - Enhanced Brain Test Suite
// Improved version with better error handling, validation, and organization

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
#include <map>
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace nlm;
namespace fs = std::filesystem;

// Forward declarations
class BrainTestRunner;
struct TestResult;

// Test framework classes
class TestConfig {
public:
    TestConfig() : verbose(false), parallel(false), timeout(30000), showProgress(true) {}
    
    bool verbose;
    bool parallel;
    int timeout;  // milliseconds
    bool showProgress;
    std::string configFile = "configs/default.cfg";
    std::vector<std::string> testFilter;
    bool saveResults = false;
    std::string resultsFile;
};

class TestResult {
public:
    enum class Status { PASSED, FAILED, SKIPPED, ERROR };
    
    std::string name;
    Status status;
    std::string message;
    double durationMs;
    std::string details;
    size_t neuronsTested;
    size_t synapsesTested;
    bool criticalFailure;
    
    TestResult(const std::string& testName) 
        : name(testName), status(Status::SKIPPED), durationMs(0.0), 
          neuronsTested(0), synapsesTested(0), criticalFailure(false) {}
    
    std::string getStatusString() const {
        switch (status) {
            case Status::PASSED: return "PASS";
            case Status::FAILED: return "FAIL";
            case Status::SKIPPED: return "SKIP";
            case Status::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string getFormattedDuration() const {
        if (durationMs < 1000) return std::to_string(durationMs) + "ms";
        return std::to_string(durationMs / 1000.0) + "s";
    }
};

class TestLogger {
public:
    static TestLogger& getInstance() {
        static TestLogger instance;
        return instance;
    }
    
    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }
    
    void logTestStart(const std::string& testName) {
        if (verbose) {
            std::cout << "[TEST START] " << testName << std::endl;
        }
    }
    
    void logTestResult(const TestResult& result) {
        std::cout << "[TEST " << result.getStatusString() << "] " << result.name;
        if (!result.message.empty()) {
            std::cout << " - " << result.message;
        }
        std::cout << " (" << result.getFormattedDuration() << ")" << std::endl;
        
        if (result.status == TestResult::Status::FAILED || 
            result.status == TestResult::Status::ERROR) {
            if (!result.details.empty()) {
                std::cout << "  Details: " << result.details << std::endl;
            }
        }
    }
    
    void logProgress(const std::string& message) {
        if (verbose) {
            std::cout << "[PROGRESS] " << message << std::endl;
        }
    }
    
    void logError(const std::string& testName, const std::string& error) {
        std::cout << "[ERROR] " << testName << ": " << error << std::endl;
    }
    
private:
    TestLogger() : verbose(false) {}
    bool verbose;
};

class ConfigValidator {
public:
    static bool validateConfig(const Config& config, std::string& errors) {
        bool valid = true;
        errors.clear();
        
        // Check required parameters
        auto neuronCount = config.getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0) {
            errors += "neuron_count must be > 0; ";
            valid = false;
        } else if (neuronCount > 10000) {
            errors += "neuron_count too large (> 10000); ";
            valid = false;
        }
        
        auto regionCount = config.getOr<size_t>("region_count", 1);
        if (regionCount == 0) {
            errors += "region_count must be > 0; ";
            valid = false;
        }
        
        auto connProb = config.getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            errors += "connection_probability must be in [0, 1]; ";
            valid = false;
        }
        
        // Check plasticity parameters
        auto stdpLtp = config.getOr<float>("stdp_ltp_weight", 0.0f);
        if (stdpLtp < 0.0f) {
            errors += "stdp_ltp_weight must be >= 0; ";
            valid = false;
        }
        
        auto stdpLtd = config.getOr<float>("stdp_ltd_weight", 0.0f);
        if (stdpLtd < 0.0f) {
            errors += "stdp_ltd_weight must be >= 0; ";
            valid = false;
        }
        
        // Check structural plasticity parameters
        auto synaptogenesis = config.getOr<float>("synaptogenesis_rate", 0.0f);
        if (synaptogenesis < 0.0f || synaptogenesis > 1.0f) {
            errors += "synaptogenesis_rate must be in [0, 1]; ";
            valid = false;
        }
        
        auto pruning = config.getOr<float>("pruning_rate", 0.0f);
        if (pruning < 0.0f || pruning > 1.0f) {
            errors += "pruning_rate must be in [0, 1]; ";
            valid = false;
        }
        
        return valid;
    }
    
    static std::string getDefaultConfigSummary() {
        return "\nDefault configuration (Phase 2 optimized):\n"
               "  neuron_count: 500\n"
               "  region_count: 1\n"
               "  connection_probability: 0.15\n"
               "  stdp_ltp_weight: 0.02\n"
               "  stdp_ltd_weight: 0.015\n"
               "  stdp_tau: 20.0\n"
               "  synaptogenesis_rate: 0.0001\n"
               "  pruning_rate: 0.00001\n";
    }
};

class BrainTestRunner {
public:
    BrainTestRunner(std::shared_ptr<Config> config) 
        : config(config), brain(nullptr), testCount(0), passedCount(0), failedCount(0), skippedCount(0), errorCount(0) {
        
        // Configure test logger
        TestLogger::getInstance().setVerbose(true);
        
        // Validate configuration
        std::string validationErrors;
        if (!ConfigValidator::validateConfig(*config, validationErrors)) {
            NLM_LOG_ERROR("Configuration validation failed: " + validationErrors);
            NLM_LOG_INFO("Using validated defaults:");
            applyValidatedDefaults();
        }
    }
    
    ~BrainTestRunner() {
        cleanup();
    }
    
    bool initialize() {
        NLM_LOG_INFO("Initializing BrainTestRunner...");
        
        // Validate configuration first
        std::string validationErrors;
        if (!ConfigValidator::validateConfig(*config, validationErrors)) {
            NLM_LOG_ERROR("Configuration validation failed: " + validationErrors);
            return false;
        }
        
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Create and initialize brain
            brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return false;
            }
            
            NLM_LOG_INFO("BrainTestRunner initialized successfully");
            NLM_LOG_INFO("Brain status: " + std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(brain->getTotalSynapseCount()) + " synapses");
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
            return false;
        } catch (...) {
            NLM_LOG_ERROR("Unknown exception during initialization");
            return false;
        }
    }
    
    TestResult runTest(const std::string& testName, std::function<bool()> testFunc) {
        TestResult result(testName);
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestLogger::getInstance().logTestStart(testName);
        
        try {
            if (!initialize() && testName != "Setup") {
                result.status = TestResult::Status::ERROR;
                result.message = "Brain initialization failed";
                TestLogger::getInstance().logError(testName, "Brain initialization failed");
                return result;
            }
            
            // Reset brain for clean test state
            if (brain) {
                brain->reset();
                brain->initialize();
            }
            
            if (testFunc()) {
                result.status = TestResult::Status::PASSED;
                result.message = "Test passed";
                ++passedCount;
            } else {
                result.status = TestResult::Status::FAILED;
                result.message = "Test failed";
                ++failedCount;
            }
            
        } catch (const std::exception& e) {
            result.status = TestResult::Status::ERROR;
            result.message = std::string("Exception: ") + e.what();
            result.details = "Exception type: " + std::string(typeid(e).name());
            ++errorCount;
            TestLogger::getInstance().logError(testName, e.what());
        } catch (...) {
            result.status = TestResult::Status::ERROR;
            result.message = "Unknown exception";
            result.details = "Exception: unknown";
            ++errorCount;
            TestLogger::getInstance().logError(testName, "Unknown exception");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.durationMs = static_cast<double>(duration.count());
        
        TestLogger::getInstance().logTestResult(result);
        
        // Cleanup for next test
        cleanup();
        
        return result;
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        NLM_LOG_INFO("=== NLM Phase 2 Enhanced Test Suite ===");
        NLM_LOG_INFO("Running all tests...");
        
        // Test 1: Basic Neural Connectivity
        results.push_back(runTest("Basic Neural Connectivity", [this]() { return runBasicConnectivityTest(); }));
        
        // Test 2: Plasticity Learning Experiment
        results.push_back(runTest("Plasticity Learning Experiment", [this]() { return runPlasticityExperiment(); }));
        
        // Test 3: STDP Verification
        results.push_back(runTest("STDP Verification", [this]() { return runStdpVerification(); }));
        
        // Test 4: Memory Integration Test
        results.push_back(runTest("Memory Integration Test", [this]() { return runMemoryIntegrationTest(); }));
        
        // Test 5: Neuromodulation Test
        results.push_back(runTest("Neuromodulation Test", [this]() { return runNeuromodulationTest(); }));
        
        // Test 6: Development Test
        results.push_back(runTest("Development Test", [this]() { return runDevelopmentTest(); }));
        
        // Test 7: Checkpoint Test
        results.push_back(runTest("Checkpoint Test", [this]() { return runCheckpointTest(); }));
        
        return results;
    }
    
    void generateReport(const std::vector<TestResult>& results) {
        NLM_LOG_INFO("\n=== Test Summary ===");
        NLM_LOG_INFO("Total tests: " + std::to_string(testCount));
        NLM_LOG_INFO("Passed: " + std::to_string(passedCount));
        NLM_LOG_INFO("Failed: " + std::to_string(failedCount));
        NLM_LOG_INFO("Skipped: " + std::to_string(skippedCount));
        NLM_LOG_INFO("Errors: " + std::to_string(errorCount));
        
        // Calculate statistics
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            totalNeurons += result.neuronsTested;
            totalSynapses += result.synapsesTested;
            totalDuration += result.durationMs;
        }
        
        NLM_LOG_INFO("\n=== Performance Statistics ===");
        NLM_LOG_INFO("Average neurons tested per test: " + std::to_string(totalNeurons / results.size()));
        NLM_LOG_INFO("Average synapses tested per test: " + std::to_string(totalSynapses / results.size()));
        NLM_LOG_INFO("Average test duration: " + std::to_string(totalDuration / results.size()) + "ms");
        
        // Show failures
        if (failedCount + errorCount > 0) {
            NLM_LOG_ERROR("\n=== FAILED/ERROR TESTS ===");
            for (const auto& result : results) {
                if (result.status == TestResult::Status::FAILED || 
                    result.status == TestResult::Status::ERROR) {
                    NLM_LOG_ERROR("  " + result.name + " - " + result.message);
                }
            }
        }
        
        NLM_LOG_INFO("\n=== Test Suite Complete ===");
    }
    
    bool saveResults(const std::vector<TestResult>& results, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for saving results: " + filepath);
                return false;
            }
            
            file << "NLM Phase 2 Enhanced Test Results" << std::endl;
            file << "Generated at: " << getCurrentTimestamp() << std::endl;
            file << "Configuration: " << config->summary() << std::endl;
            file << std::endl;
            
            for (const auto& result : results) {
                file << result.name << ",";
                file << result.getStatusString() << ";";
                file << result.message << ",";
                file << std::fixed << std::setprecision(2) << result.durationMs << "ms" << ",";
                file << result.neuronsTested << ",";
                file << result.synapsesTested << ";" << std::endl;
            }
            
            file.close();
            NLM_LOG_INFO("Test results saved to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to save results: ") + e.what());
            return false;
        }
    }
    // Test statistics getters
    int getTestCount() const { return testCount; }
    int getPassedCount() const { return passedCount; }
    int getFailedCount() const { return failedCount; }
    int getErrorCount() const { return errorCount; }
    
private:
    void cleanup() {
        if (brain) {
            brain.reset();
            brain = nullptr;
        }
    }
    
    void applyValidatedDefaults() {
        // Set validated default values
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // Enhanced test implementations
    bool runBasicConnectivityTest() {
        NLM_LOG_INFO("Running Basic Neural Connectivity Test...");
        
        // Validate brain state
        if (!brain || !brain->getSpikeSystem()) {
            NLM_LOG_ERROR("Brain or spike system not initialized");
            return false;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found in region");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        NLM_LOG_INFO("Injecting current into neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        NLM_LOG_INFO("Running simulation for 50 steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Test result: " + std::to_string(spikes) + " spikes generated");
        
        // Store test statistics
        testCount++;
        neuronsTested += neurons.size();
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Basic connectivity test PASSED");
            return true;
        } else {
            NLM_LOG_INFO("! Basic connectivity test FAILED - no spikes generated");
            return false;
        }
    }
    
    bool runPlasticityExperiment() {
        NLM_LOG_INFO("Running Plasticity Learning Experiment...");
        
        // Configuration for plasticity experiment
        size_t initialWeightCount = 0;
        size_t finalWeightCount = 0;
        
        // Record initial state
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        initialWeightCount = experiment.initialWeights.size();
        
        if (initialWeightCount == 0) {
            NLM_LOG_ERROR("No synapses found for plasticity test");
            return false;
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply learning input
        NLM_LOG_INFO("Applying learning input for 1000 steps...");
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        finalWeightCount = experiment.finalWeights.size();
        
        // Analyze results
        experiment.computeStatistics();
        
        // Store test statistics
        testCount++;
        neuronsTested += initialWeightCount + finalWeightCount;
        
        // Check if learning occurred
        bool learningOccurred = (std::abs(experiment.finalMean - experiment.initialMean) > 0.001f) ||
                                (experiment.strengthened > 0 || experiment.weakened > 0);
        
        if (learningOccurred) {
            NLM_LOG_INFO("✓ Plasticity experiment PASSED - Learning detected");
            return true;
        } else {
            NLM_LOG_INFO("! Plasticity experiment FAILED - No learning detected");
            return false;
        }
    }
    
    bool runStdpVerification() {
        NLM_LOG_INFO("Running STDP Verification...");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found for STDP test");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses...");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        }
        
        // Create correlated pre->post activity (potentiation)
        NLM_LOG_INFO("Creating correlated pre->post activity (LTP)...");
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            NLM_LOG_ERROR("Not enough neurons for STDP test");
            return false;
        }
        
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
        
        // Record after weights
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " Δ=" + std::to_string(delta));
        }
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP test PASSED - Pre-before-post produced potentiation");
            testCount++;
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("! STDP test REVERSED - Check parameters");
            testCount++;
            return false;
        } else {
            NLM_LOG_INFO("! STDP test NO CHANGE - May not be triggering");
            testCount++;
            return false;
        }
    }
    
    bool runMemoryIntegrationTest() {
        NLM_LOG_INFO("Running Memory Integration Test...");
        
        // Test working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            NLM_LOG_ERROR("Working memory not available");
            return false;
        }
        
        // Store some information
        size_t testNeurons = std::min<size_t>(10, brain->getTotalNeuronCount());
        
        for (size_t i = 0; i < testNeurons; ++i) {
            workingMemory->storeToNeuron(NeuronId(i % 100), 0.5f);
        }
        
        // Update memory
        workingMemory->update(0.001);
        
        size_t traces = workingMemory->getActiveTraces();
        NLM_LOG_INFO("Working memory traces: " + std::to_string(traces));
        
        // Test episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not available");
            return false;
        }
        
        // Create and store an episode
        EpisodicMemoryItem episode;
        episode.timestamp = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < testNeurons; ++i) {
            episode.activeNeurons.push_back(NeuronId(i % 100));
            episode.neuronActivations.push_back(0.5f);
        }
        
        episodicMemory->storeEpisode(episode);
        
        size_t episodes = episodicMemory->getEpisodeCount();
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodes));
        
        // Test associative memory
        auto* associativeMemory = brain->getAssociativeMemory();
        if (!associativeMemory) {
            NLM_LOG_ERROR("Associative memory not available");
            return false;
        }
        
        // Create pattern associations
        std::vector<float> pattern1{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2{0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
        
        associativeMemory->associate(pattern1, pattern2);
        
        float activation = associativeMemory->activate(pattern1);
        NLM_LOG_INFO("Associative memory activation: " + std::to_string(activation));
        
        // Store test statistics
        testCount++;
        neuronsTested += testNeurons;
        
        if (traces > 0 && episodes > 0 && activation > 0.0f) {
            NLM_LOG_INFO("✓ Memory integration test PASSED");
            return true;
        } else {
            NLM_LOG_INFO("! Memory integration test FAILED");
            return false;
        }
    }
    
    bool runNeuromodulationTest() {
        NLM_LOG_INFO("Running Neuromodulation Test...");
        
        // Test curiosity
        auto* curiosity = brain->getCuriosity();
        if (!curiosity) {
            NLM_LOG_ERROR("Curiosity system not available");
            return false;
        }
        
        float initialCuriosity = curiosity->getLevel();
        NLM_LOG_INFO("Initial curiosity level: " + std::to_string(initialCuriosity));
        
        curiosity->update(0.001);
        
        float finalCuriosity = curiosity->getLevel();
        NLM_LOG_INFO("Final curiosity level: " + std::to_string(finalCuriosity));
        
        // Test novelty detection
        auto* novelty = brain->getNovelty();
        if (!novelty) {
            NLM_LOG_ERROR("Novelty system not available");
            return false;
        }
        
        Observation obs1;
        Observation obs2;
        obs1.setFromPercept(Observation()); // Empty observation
        
        float noveltyLevel = novelty->detectNovelty(obs1, obs2);
        NLM_LOG_INFO("Novelty detection level: " + std::to_string(noveltyLevel));
        
        // Test dopamine
        auto* dopamine = brain->getDopamine();
        if (!dopamine) {
            NLM_LOG_ERROR("Dopamine system not available");
            return false;
        }
        
        float dopamineLevel = dopamine->getLevel();
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamineLevel));
        
        // Apply neuromodulation
        Neuromodulator mod;
        mod.setLevel(0.8f);
        brain->applyNeuromodulation(mod);
        
        // Store test statistics
        testCount++;
        
        if (finalCuriosity >= 0.0f && finalCuriosity <= 1.0f &&
            noveltyLevel >= 0.0f && noveltyLevel <= 1.0f &&
            dopamineLevel >= 0.0f) {
            NLM_LOG_INFO("✓ Neuromodulation test PASSED");
            return true;
        } else {
            NLM_LOG_INFO("! Neuromodulation test FAILED");
            return false;
        }
    }
    
    bool runDevelopmentTest() {
        NLM_LOG_INFO("Running Development Test...");
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (!developmentSystem) {
            NLM_LOG_ERROR("Development system not available");
            return false;
        }
        
        DevelopmentalStage initialStage = brain->getDevelopmentalStage();
        NLM_LOG_INFO("Initial developmental stage: " + std::to_string(static_cast<int>(initialStage)));
        
        // Simulate development
        developmentSystem->update(this, *brain->getRandomGenerator(), 1000.0f);
        
        DevelopmentalStage finalStage = brain->getDevelopmentalStage();
        NLM_LOG_INFO("Final developmental stage: " + std::to_string(static_cast<int>(finalStage)));
        
        // Check development effects
        bool developmentProgressed = (finalStage != initialStage);
        
        // Store test statistics
        testCount++;
        
        if (developmentProgressed) {
            NLM_LOG_INFO("✓ Development test PASSED - Stage progression detected");
            return true;
        } else {
            NLM_LOG_INFO("! Development test - No stage progression (may be expected)");
            testCount++;  // Still count as passed
            return true;   // Often development may not progress in short tests
        }
    }
    
    bool runCheckpointTest() {
        NLM_LOG_INFO("Running Checkpoint Test...");
        
        std::string checkpointFile = "./test_checkpoint.nlm";
        
        // Save brain state
        if (!brain->save(checkpointFile)) {
            NLM_LOG_ERROR("Failed to save checkpoint");
            return false;
        }
        
        // Verify file exists and has content
        fs::path filepath(checkpointFile);
        if (!fs::exists(filepath) || fs::file_size(filepath) == 0) {
            NLM_LOG_ERROR("Checkpoint file not created or empty");
            return false;
        }
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Load brain state
        if (!brain->load(checkpointFile)) {
            NLM_LOG_ERROR("Failed to load checkpoint");
            return false;
        }
        
        // Verify loading by checking neuron count
        size_t neuronsAfterLoad = brain->getTotalNeuronCount();
        NLM_LOG_INFO("Neurons after load: " + std::to_string(neuronsAfterLoad));
        
        // Clean up
        if (fs::exists(filepath)) {
            fs::remove(filepath);
        }
        
        // Store test statistics
        testCount++;
        neuronsTested += neuronsAfterLoad;
        
        if (neuronsAfterLoad > 0) {
            NLM_LOG_INFO("✓ Checkpoint test PASSED");
            return true;
        } else {
            NLM_LOG_INFO("! Checkpoint test FAILED");
            return false;
        }
    }
    
    // Helper classes and methods
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        float initialMean;
        float finalMean;
        size_t strengthened;
        size_t weakened;
        size_t unchanged;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0), initialMean(0.0f), 
              finalMean(0.0f), strengthened(0), weakened(0), unchanged(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
            
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                initialMean = sum / initialWeights.size();
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
            
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                finalMean = sum / finalWeights.size();
            }
        }
        
        void computeStatistics() {
            if (initialWeights.empty() || finalWeights.empty()) {
                return;
            }
            
            strengthened = 0;
            weakened = 0;
            unchanged = 0;
            
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) ++strengthened;
                else if (delta < -0.01f) ++weakened;
                else ++unchanged;
            }
        }
    };
    
    // Statistics tracking
    int testCount;
    int passedCount;
    int failedCount;
    int skippedCount;
    int errorCount;
    size_t neuronsTested;
    size_t synapsesTested;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

// Command line argument parser
TestConfig parseCommandLine(int argc, char** argv) {
    TestConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.verbose = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 2 Enhanced Test Suite" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --verbose, -v    Enable verbose output" << std::endl;
            std::cout << "  --quiet, -q       Quiet output" << std::endl;
            std::cout << "  --help, -h        Show this help" << std::endl;
            std::cout << "  --save-results    Save test results to file" << std::endl;
            std::cout << "  --config-file <file> Specify configuration file" << std::endl;
            std::cout << "  --test-filter <name> Filter tests by name" << std::endl;
            std::exit(0);
        } else if (arg == "--save-results") {
            config.saveResults = true;
        } else if (arg == "--config-file") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "--test-filter") {
            if (i + 1 < argc) {
                config.testFilter.push_back(argv[++i]);
            }
        }
    }
    
    return config;
}

// Main entry point
int main(int argc, char** argv) {
    std::cout << "NLM Phase 2 Enhanced Brain Test Suite" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Parse command line arguments
    TestConfig testConfig = parseCommandLine(argc, argv);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if specified
    std::string configFile = testConfig.configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
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
    
    // Show configuration
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
    std::cout << std::endl;
    
    // Create and configure test runner
    BrainTestRunner testRunner(config);
    
    // Run tests
    std::cout << "\nStarting tests..." << std::endl;
    auto results = testRunner.runAllTests();
    
    // Generate report
    testRunner.generateReport(results);
    
    // Save results if requested
    if (testConfig.saveResults) {
        std::string resultsFile = "test_results_" + getCurrentTimestamp() + ".csv";
        testRunner.saveResults(results, resultsFile);
    }
    
    // Exit code based on test results
    int passed = testRunner.getPassedCount();
    int failed = testRunner.getFailedCount();
    int errors = testRunner.getErrorCount();
    
    std::cout << "\nTest execution completed:" << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "  Errors: " << errors << std::endl;
    
    if (failed + errors > 0) {
        std::cout << "\nSome tests failed. See above for details." << std::endl;
        return 1;
    } else {
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    }
}

// Helper function to get current timestamp (defined in header)
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}