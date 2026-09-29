#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <filesystem>
#include <argparse/argparse.hpp>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"

namespace fs = std::filesystem;

// Command-line interface for NLM
class NLMCLI {
public:
    NLMCLI(int argc, char** argv) {
        parseArguments(argc, argv);
        setupLogging();
        createOutputDirectory();
    }
    
    void run() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("NLM Command-line Interface - Phase " + std::to_string(currentPhase_));
        NLM_LOG_INFO("Starting NLM simulation...");
        
        try {
            if (currentPhase_ == 2) {
                runPhase2();
            } else if (currentPhase_ == 3) {
                runPhase3();
            } else if (currentPhase_ == 4) {
                runPhase4();
            } else if (currentPhase_ == 6) {
                runPhase6();
            } else {
                throw std::runtime_error("Invalid phase specified");
            }
            
            // Save results if requested
            if (!resultsFile_.empty()) {
                saveResults();
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            
            NLM_LOG_INFO("NLM simulation completed successfully in " + std::to_string(duration.count()) + " seconds");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("NLM simulation failed: " + std::string(e.what()));
            return 1;
        }
        
        return 0;
    }
    
private:
    void parseArguments(int argc, char** argv) {
        // Create argument parser
        argparse::ArgumentParser program("nlm", "1.0.0");
        
        // Phase selection
        program.add_argument("phase")
            .default_value("6")
            .help("Phase to run (2, 3, 4, or 6)");
        
        // Basic configuration
        program.add_argument("--config", "-c")
            .default_value("configs/default.cfg")
            .help("Configuration file");
        
        program.add_argument("--neurons", "-n")
            .default_value(1000)
            .help("Number of neurons");
        
        program.add_argument("--regions", "-r")
            .default_value(1)
            .help("Number of brain regions");
        
        program.add_argument("--steps", "-s")
            .default_value(1000)
            .help("Number of simulation steps");
        
        program.add_argument("--timestep", "-t")
            .default_value(0.001)
            .help("Simulation timestep in seconds");
        
        program.add_argument("--seed", "-seed")
            .default_value(42)
            .help("Random seed");
        
        // Logging options
        program.add_argument("--verbose", "-V")
            .default_value(false)
            .help("Enable verbose logging")
            .flag();
        
        program.add_argument("--quiet", "-q")
            .default_value(false)
            .help("Quiet mode (errors only)")
            .flag();
        
        program.add_argument("--log-level")
            .default_value("INFO")
            .help("Log level: DEBUG, INFO, WARNING, ERROR");
        
        // Phase-specific options
        program.add_argument("--plasticity", "-p")
            .default_value(false)
            .help("Enable plasticity experiments")
            .flag();
        
        program.add_argument("--stdp", "-stdp")
            .default_value(false)
            .help("Enable STDP learning")
            .flag();
        
        program.add_argument("--hebbian", "-H")
            .default_value(false)
            .help("Enable Hebbian learning")
            .flag();
        
        program.add_argument("--structural", "-S")
            .default_value(false)
            .help("Enable structural plasticity")
            .flag();
        
        // Output options
        program.add_argument("--output", "-o")
            .default_value(".")
            .help("Output directory");
        
        program.add_argument("--save-checkpoints")
            .default_value(false)
            .help("Enable automatic checkpoint saving")
            .flag();
        
        program.add_argument("--load-checkpoint")
            .default_value("")
            .help("Load checkpoint from file");
        
        program.add_argument("--results", "-R")
            .default_value("")
            .help("Save results to file (JSON/CSV)");
        
        // Advanced options
        program.add_argument("--gpu", "-g")
            .default_value(false)
            .help("Use GPU acceleration if available")
            .flag();
        
        program.add_argument("--parallel", "-par")
            .default_value(false)
            .help("Enable parallel processing")
            .flag();
        
        program.add_argument("--threads")
            .default_value(0)
            .help("Number of threads (0 = auto)");
        
        program.add_argument("--memory-limit")
            .default_value(0)
            .help("Memory limit in MB");
        
        program.add_argument("--timeout")
            .default_value(0)
            .help("Timeout in seconds (0 = unlimited)");
        
        // Parse arguments
        try {
            program.parse_args(argc, argv);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << program.help() << std::endl;
            exit(1);
        }
        
        // Extract parsed values
        currentPhase_ = std::stoi(program.get<std::string>("phase"));
        configFile_ = program.get<std::string>("--config");
        neuronCount_ = program.get<int>("--neurons");
        regionCount_ = program.get<int>("--regions");
        steps_ = program.get<int>("--steps");
        timestep_ = program.get<double>("--timestep");
        randomSeed_ = program.get<int>("--seed");
        
        verbose_ = program.get<bool>("--verbose");
        quiet_ = program.get<bool>("--quiet");
        logLevel_ = program.get<std::string>("--log-level");
        
        plasticityEnabled_ = program.get<bool>("--plasticity");
        stdpEnabled_ = program.get<bool>("--stdp");
        hebbianEnabled_ = program.get<bool>("--hebbian");
        structuralEnabled_ = program.get<bool>("--structural");
        
        outputDir_ = program.get<std::string>("--output");
        saveCheckpoints_ = program.get<bool>("--save-checkpoints");
        loadCheckpoint_ = program.get<std::string>("--load-checkpoint");
        resultsFile_ = program.get<std::string>("--results");
        
        useGPU_ = program.get<bool>("--gpu");
        useParallel_ = program.get<bool>("--parallel");
        threadCount_ = program.get<int>("--threads");
        memoryLimit_ = program.get<int>("--memory-limit");
        timeout_ = program.get<int>("--timeout");
        
        // Validate phase
        if (currentPhase_ < 2 || currentPhase_ > 6) {
            throw std::runtime_error("Phase must be 2, 3, 4, or 6");
        }
    }
    
    void setupLogging() {
        // Configure logging based on verbosity
        LogLevel logLevel;
        
        if (quiet_) {
            logLevel = LogLevel::Error;
        } else if (verbose_) {
            logLevel = LogLevel::Debug;
        } else {
            logLevel = LogLevel::Info;
        }
        
        // Set up logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Logging initialized with level: " + logLevel_);
    }
    
    void createOutputDirectory() {
        if (!outputDir_.empty() && outputDir_ != ".") {
            fs::create_directories(outputDir_);
            NLM_LOG_INFO("Output directory: " + outputDir_);
        }
    }
    
    void runPhase2() {
        NLM_LOG_INFO("Running Phase 2: Real Neural Computation");
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Load from config file
        if (!configFile_.empty() && configFile_ != "configs/default.cfg") {
            config->loadFromFile(configFile_);
        }
        
        // Set configuration values
        config->set("random_seed", static_cast<int64_t>(randomSeed_), ConfigSource::CommandLine);
        config->set("simulation_timestep", timestep_, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(regionCount_), ConfigSource::CommandLine);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        brain->logStatus();
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment if enabled
        if (plasticityEnabled_) {
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
        }
        
        // Reset and run STDP verification if enabled
        if (stdpEnabled_) {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        }
        
        // Final brain status
        NLM_LOG_INFO("Phase 2 completed successfully");
        brain->logStatus();
    }
    
    void runPhase3() {
        NLM_LOG_INFO("Running Phase 3: World Interaction");
        
        // For now, just log that phase 3 would run
        // Full phase 3 implementation would include world simulation
        NLM_LOG_INFO("Phase 3 implementation would include world interaction experiments");
    }
    
    void runPhase4() {
        NLM_LOG_INFO("Running Phase 4: Advanced Integration");
        
        // For now, just log that phase 4 would run
        // Full phase 4 implementation would include advanced integration
        NLM_LOG_INFO("Phase 4 implementation would include advanced integration experiments");
    }
    
    void runPhase6() {
        NLM_LOG_INFO("Running Phase 6: Final Integration");
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Load from config file
        if (!configFile_.empty() && configFile_ != "configs/default.cfg") {
            config->loadFromFile(configFile_);
        }
        
        // Set configuration values
        config->set("random_seed", static_cast<int64_t>(randomSeed_), ConfigSource::CommandLine);
        config->set("simulation_timestep", timestep_, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(regionCount_), ConfigSource::CommandLine);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for Phase 6");
        }
        
        brain->logStatus();
        
        // Run comprehensive integration test
        runPhase6IntegrationTest(brain);
        
        NLM_LOG_INFO("Phase 6 completed successfully");
        brain->logStatus();
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
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
            brain->step(step, step * timestep_);
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
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * timestep_);
            
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
                brain->step(trial * 2, trial * 2 * timestep_);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * timestep_);
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
    
    void runPhase6IntegrationTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Phase 6 Integration Test ===");
        
        // Comprehensive test to verify all systems are integrated
        NLM_LOG_INFO("Testing memory systems...");
        
        // Test working memory
        if (brain->getWorkingMemory()) {
            NLM_LOG_INFO("  ✓ Working memory available");
        } else {
            NLM_LOG_INFO("  ! Working memory not available");
        }
        
        NLM_LOG_INFO("Testing neuromodulation systems...");
        
        // Test neuromodulation
        if (brain->getDopamine()) {
            NLM_LOG_INFO("  ✓ Dopamine system available");
        } else {
            NLM_LOG_INFO("  ! Dopamine system not available");
        }
        
        if (brain->getCuriosity()) {
            NLM_LOG_INFO("  ✓ Curiosity system available");
        } else {
            NLM_LOG_INFO("  ! Curiosity system not available");
        }
        
        NLM_LOG_INFO("Testing prediction system...");
        
        // Test prediction system
        if (brain->getPredictionSystem()) {
            NLM_LOG_INFO("  ✓ Prediction system available");
        } else {
            NLM_LOG_INFO("  ! Prediction system not available");
        }
        
        NLM_LOG_INFO("Testing cognition systems...");
        
        // Test cognition systems
        if (brain->getNeuralPlanner()) {
            NLM_LOG_INFO("  ✓ Neural planner available");
        } else {
            NLM_LOG_INFO("  ! Neural planner not available");
        }
        
        if (brain->getConceptFormation()) {
            NLM_LOG_INFO("  ✓ Concept formation available");
        } else {
            NLM_LOG_INFO("  ! Concept formation not available");
        }
        
        NLM_LOG_INFO("Running simulation steps...");
        
        // Run simulation steps
        for (SimulationStep step = 0; step < steps_; ++step) {
            brain->step(step, step * timestep_);
            
            // Inject some input to keep neurons active
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
            
            if (step % 500 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("Phase 6 integration test completed");
    }
    
    void saveResults() {
        // Save results to file
        if (fs::exists(outputDir_)) {
            std::string resultsPath = outputDir_ + "/results.json";
            
            // In a full implementation, this would serialize the simulation results
            NLM_LOG_INFO("Saving results to: " + resultsPath);
            
            // For now, just log that results would be saved
            NLM_LOG_INFO("Results would be saved to JSON/CSV format");
        }
    }
    
    // LearningExperiment struct from main.cpp
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
            }
            
            NLM_LOG_INFO("Initial state recorded:");
            NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float mean = sum / initialWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            // Record final weights from first region
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
            
            NLM_LOG_INFO("Final state recorded:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float mean = sum / finalWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
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
    
    // Configuration fields
    int currentPhase_ = 6;
    std::string configFile_ = "configs/default.cfg";
    int neuronCount_ = 1000;
    int regionCount_ = 1;
    int steps_ = 1000;
    double timestep_ = 0.001;
    int randomSeed_ = 42;
    
    // Flags
    bool verbose_ = false;
    bool quiet_ = false;
    bool plasticityEnabled_ = false;
    bool stdpEnabled_ = false;
    bool hebbianEnabled_ = false;
    bool structuralEnabled_ = false;
    bool saveCheckpoints_ = false;
    bool useGPU_ = false;
    bool useParallel_ = false;
    
    // String options
    std::string logLevel_ = "INFO";
    std::string outputDir_ = ".";
    std::string loadCheckpoint_ = "";
    std::string resultsFile_ = "";
    
    // Numeric options
    int threadCount_ = 0;
    int memoryLimit_ = 0;
    int timeout_ = 0;
};

int main(int argc, char** argv) {
    try {
        NLMCLI cli(argc, argv);
        return cli.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}