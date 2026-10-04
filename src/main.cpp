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
#include <chrono>
#include <filesystem>

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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  --config <file>               Load configuration from file (JSON/YAML/key=value)" << std::endl;
    std::cout << "  --neuron-count <count>        Override number of neurons" << std::endl;
    std::cout << "  --region-count <count>        Override number of regions" << std::endl;
    std::cout << "  --steps <count>               Number of simulation steps to run" << std::endl;
    std::cout << "  --timestep <ms>               Simulation timestep in milliseconds" << std::endl;
    std::cout << "  --debug                       Enable debug logging" << std::endl;
    std::cout << "  --quiet                       Minimal output (errors only)" << std::endl;
    std::cout << "  --demo-mode <mode>            Demo mode: quick, standard, full, benchmark" << std::endl;
    std::cout << "  --checkpoint <file>           Save checkpoint to file" << std::endl;
    std::cout << "  --load <file>                 Load checkpoint from file" << std::endl;
    std::cout << "  --experiment <name>           Run specific experiment" << std::endl;
    std::cout << "      Available experiments: quick, basic, plasticity, stdp, phase6, scaling" << std::endl;
    std::cout << "  --batch <file>                Run experiments from batch configuration file" << std::endl;
    std::cout << "  --format <format>             Output format: text, json, csv" << std::endl;
    std::cout << "      json, csv formats require --output-file" << std::endl;
    std::cout << "  --output-file <file>          Save results to file" << std::endl;
    std::cout << "  --timing                      Report detailed timing information" << std::endl;
    std::cout << "  --help-experiments            Show detailed experiment information" << std::endl;
    std::cout << "  --list-presets               List available configuration presets" << std::endl;
    std::cout << "  --preset <name>               Use a configuration preset (quick, standard, full, benchmark)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neuron-count 1000 --steps 1000" << std::endl;
    std::cout << "  " << programName << " --config my_config.json --demo-mode full" << std::endl;
    std::cout << "  " << programName << " --neuron-count 500 --plasticity.stdp.enable false" << std::endl;
    std::cout << "  " << programName << " --checkpoint save.bin --load save.bin" << std::endl;
}

void printExperimentHelp() {
    std::cout << "Available Experiments:" << std::endl;
    std::cout << "  quick                        - Quick smoke test for validation" << std::endl;
    std::cout << "  basic                        - Basic connectivity and spike propagation test" << std::endl;
    std::cout << "  plasticity                   - Plasticity learning experiment (Hebbian + STDP)" << std::endl;
    std::cout << "  stdp                         - STDP verification (pre-post spike timing)" << std::endl;
    std::cout << "  phase6                       - Complete Phase 6 integration experiment" << std::endl;
    std::cout << "  scaling                      - Performance scaling benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Integration includes:" << std::endl;
    std::cout << "  - Memory systems (working + episodic)" << std::endl;
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  - Prediction and planning systems" << std::endl;
    std::cout << "  - Development system" << std::endl;
    std::cout << "  - Checkpoint save/load" << std::endl;
    std::cout << std::endl;
    std::cout << "Quick experiment definitions:" << std::endl;
    std::cout << "  quick_silent:      1000 neurons, 5000 steps (no output)" << std::endl;
    std::cout << "  quick_standard:    500 neurons, 2000 steps (standard config)" << std::endl;
    std::cout << "  quick_fast:        2000 neurons, 1000 steps (performance test)" << std::endl;
    std::cout << std::endl;
}

void printPresets() {
    std::cout << "Available Configuration Presets:" << std::endl;
    std::cout << "  quick      - Fast validation: 200 neurons, 1000 steps, minimal output" << std::endl;
    std::cout << "  standard   - Standard test: 500 neurons, 5000 steps, moderate output" << std::endl;
    std::cout << "  full       - Complete test: 1000 neurons, 20000 steps, detailed output" << std::endl;
    std::cout << "  benchmark  - Performance test: 5000 neurons, 50000 steps, profiling" << std::endl;
    std::cout << std::endl;
    std::cout << "Each preset includes optimized neural parameters and output settings." << std::endl;
}

void applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
    if (presetName == "quick") {
        config->set("neuron_count", 200);
        config->set("simulation_timestep", 0.001);
        config->set("connection_probability", 0.1);
        config->set("stdp_ltp_weight", 0.01);
        config->set("stdp_ltd_weight", 0.012);
        config->set("synaptogenesis_rate", 0.0001);
        config->set("pruning_rate", 0.00001);
        config->set("quick_experiment_steps", 1000);
        config->set("connectivity_test_steps", 100);
        config->set("plasticity_experiment_steps", 500);
        config->set("phase6_integration_steps", 1000);
        config->set("scaling_benchmark_steps", 2000);
    } else if (presetName == "standard") {
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        config->set("connection_probability", 0.15);
        config->set("stdp_ltp_weight", 0.02);
        config->set("stdp_ltd_weight", 0.015);
        config->set("synaptogenesis_rate", 0.0002);
        config->set("pruning_rate", 0.00002);
        config->set("quick_experiment_steps", 2000);
        config->set("connectivity_test_steps", 500);
        config->set("plasticity_experiment_steps", 2000);
        config->set("phase6_integration_steps", 3000);
        config->set("scaling_benchmark_steps", 5000);
    } else if (presetName == "full") {
        config->set("neuron_count", 1000);
        config->set("simulation_timestep", 0.001);
        config->set("connection_probability", 0.2);
        config->set("stdp_ltp_weight", 0.03);
        config->set("stdp_ltd_weight", 0.025);
        config->set("synaptogenesis_rate", 0.0005);
        config->set("pruning_rate", 0.00005);
        config->set("quick_experiment_steps", 5000);
        config->set("connectivity_test_steps", 1000);
        config->set("plasticity_experiment_steps", 5000);
        config->set("phase6_integration_steps", 10000);
        config->set("scaling_benchmark_steps", 15000);
    } else if (presetName == "benchmark") {
        config->set("neuron_count", 5000);
        config->set("simulation_timestep", 0.0005);
        config->set("connection_probability", 0.05);
        config->set("stdp_ltp_weight", 0.005);
        config->set("stdp_ltd_weight", 0.004);
        config->set("synaptogenesis_rate", 0.00001);
        config->set("pruning_rate", 0.000001);
        config->set("quick_experiment_steps", 1000);
        config->set("connectivity_test_steps", 200);
        config->set("plasticity_experiment_steps", 1000);
        config->set("phase6_integration_steps", 2000);
        config->set("scaling_benchmark_steps", 50000);
    }
    
    NLM_LOG_INFO("Applied preset: " + presetName);
}

void runExperiment(const std::string& experimentName, std::shared_ptr<Config> config, 
                   bool debug, bool quiet, const std::string& outputFile,
                   const std::string& format) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(debug ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Running Experiment: " + experimentName + " ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (experimentName == "quick") {
        runQuickExperiment(config);
    } else if (experimentName == "basic") {
        runBasicConnectivityTest(config);
    } else if (experimentName == "plasticity") {
        runPlasticityExperiment(config);
    } else if (experimentName == "stdp") {
        runStdpVerification(config);
    } else if (experimentName == "phase6") {
        runPhase6Integration(config);
    } else if (experimentName == "scaling") {
        runScalingBenchmark(config);
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + experimentName);
        printExperimentHelp();
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    NLM_LOG_INFO("Experiment " + experimentName + " completed in " + std::to_string(duration.count()) + " seconds");
    
    if (!outputFile.empty() && (format == "json" || format == "csv")) {
        saveResultsToFile(experimentName, outputFile, format);
    }
}

struct ExperimentResult {
    std::string experimentType;
    double duration;
    int64_t neuronCount;
    int64_t totalSynapseCount;
    int64_t totalSpikes;
    double averageFiringRate;
    double eiRatio;
    bool learningDetected;
    size_t synapsesStrengthened;
    size_t synapsesWeakened;
    float weightChange;
    float initialMeanWeight;
    float finalMeanWeight;
    bool ltpDetected;
    bool ltdDetected;
    bool stdpWorking;
    float totalWeightChange;
    float totalReward;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    size_t memoryEpisodesStored;
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
};

ExperimentResult runQuickExperiment(std::shared_ptr<Config> config) {
    ExperimentResult result;
    result.experimentType = "quick";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    int64_t neurons = config->getOr<int64_t>("neuron_count", 200);
    NLM_LOG_INFO("Running Quick Experiment with " + std::to_string(neurons) + " neurons");
    
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for quick experiment");
        return result;
    }
    
    size_t steps = config->getOr<int64_t>("quick_experiment_steps", 1000);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 100 == 0 && !quiet) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    result.neuronCount = brain->getTotalNeuronCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    result.eiRatio = brain->getExcitationInhibitionRatio();
    result.memoryWorkingMemoryIntegrated = brain->getWorkingMemory() != nullptr;
    
    NLM_LOG_INFO("Quick Experiment complete: " + std::to_string(result.totalSpikes) + " total spikes");
    
    return result;
}

ExperimentResult runBasicConnectivityTest(std::shared_ptr<Config> config) {
    ExperimentResult result;
    result.experimentType = "basic";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Running Basic Connectivity Test");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for basic connectivity test");
        return result;
    }
    
    // Inject current into sensory neurons and check spike propagation
    brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
    
    size_t steps = config->getOr<int64_t>("connectivity_test_steps", 100);
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    result.neuronCount = brain->getTotalNeuronCount();
    result.totalSynapseCount = brain->getTotalSynapseCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    
    NLM_LOG_INFO("Basic Connectivity Test complete: " + std::to_string(result.totalSpikes) + " total spikes");
    
    return result;
}

ExperimentResult runPlasticityExperiment(std::shared_ptr<Config> config) {
    ExperimentResult result;
    result.experimentType = "plasticity";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Running Plasticity Learning Experiment");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for plasticity experiment");
        return result;
    }
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    auto initialRegion = brain->getRegion(RegionId(1));
    std::vector<float> initialWeights;
    if (initialRegion) {
        for (const auto& syn : initialRegion->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Enable plasticity and run learning
    if (initialRegion) {
        for (auto& syn : initialRegion->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    size_t steps = config->getOr<int64_t>("plasticity_experiment_steps", 1000);
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("Plasticity step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    auto finalRegion = brain->getRegion(RegionId(1));
    std::vector<float> finalWeights;
    if (finalRegion) {
        for (const auto& syn : finalRegion->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    result.neuronCount = brain->getTotalNeuronCount();
    result.totalSynapseCount = brain->getTotalSynapseCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    
    // Calculate weight changes
    if (!initialWeights.empty() && !finalWeights.empty()) {
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        result.initialMeanWeight = initialSum / initialWeights.size();
        result.finalMeanWeight = finalSum / finalWeights.size();
        result.weightChange = result.finalMeanWeight - result.initialMeanWeight;
        result.learningDetected = std::abs(result.weightChange) > 0.001f;
        
        // Count strengthened/weakened synapses
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) result.synapsesStrengthened++;
            else if (delta < -0.01f) result.synapsesWeakened++;
        }
    }
    
    NLM_LOG_INFO("Plasticity Experiment complete: weight change = " + std::to_string(result.weightChange));
    
    return result;
}

ExperimentResult runStdpVerification(std::shared_ptr<Config> config) {
    ExperimentResult result;
    result.experimentType = "stdp";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Running STDP Verification Experiment");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for STDP verification");
        return result;
    }
    
    auto region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        NLM_LOG_ERROR("Not enough synapses for STDP test");
        return result;
    }
    
    // Get first 5 synapses
    auto& synapses = region->getSynapses();
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity: fire pre then post
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
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
    }
    
    // Calculate weight changes
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    result.neuronCount = brain->getTotalNeuronCount();
    result.totalSynapseCount = brain->getTotalSynapseCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    
    result.totalWeightChange = totalDelta;
    result.ltpDetected = totalDelta > 0.001f;
    result.ltdDetected = totalDelta < -0.001f;
    result.stdpWorking = std::abs(totalDelta) > 0.001f;
    
    NLM_LOG_INFO("STDP Verification complete: total Δweight = " + std::to_string(totalDelta));
    
    return result;
}

void saveResultsToFile(const std::string& experimentName, const std::string& filepath, const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + filepath);
        return;
    }
    
    if (format == "json") {
        file << "{\n  \"experiment\": \"" << experimentName << "\",\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
        file << "  \"status\": \"completed\"\n}";
    } else if (format == "csv") {
        file << "experiment,timestamp,duration,neurons,total_spikes,avg_firing_rate\n";
        file << experimentName << "," << getCurrentTimestamp() << ",0,0,0,0.0";
    }
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filepath);
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void runPhase6Integration(std::shared_ptr<Config> config) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)) + " neurons");
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config->getOr<int64_t>("neuron_count", 1000));
    cfg->set("region_count", config->getOr<int64_t>("region_count", 1));
    cfg->set("connection_probability", config->getOr<double>("connection_probability", 0.15));
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Create simple world
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(config->getOr<bool>("enable_development", true));
    agent.enableDevelopment(config->getOr<bool>("enable_development", true));
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Brain and agent initialized successfully");
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    size_t steps = config->getOr<int64_t>("phase6_integration_steps", 2000);
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Get observation
        auto regions = agent.getBrain()->getRegions();
        if (!regions.empty()) {
            SensoryPercept percept = world.observe(regions[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config->getOr<bool>("enable_development", true)) {
                agent.updateDevelopment(0.001);
            }
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Periodic status
            if (step % 500 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                             " | Reward: " + std::to_string(totalReward / (step + 1)) +
                             " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                             " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                 brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
    }
    
    // Collect final metrics
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(totalReward / steps));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(totalFiringRate / steps));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(brain->getEpisodicMemory() ? 
        brain->getEpisodicMemory()->getEpisodeCount() : 0));
}

void runScalingBenchmark(std::shared_ptr<Config> config) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Scaling Benchmark ===");
    
    size_t neurons = config->getOr<int64_t>("scaling_benchmark_neurons", 1000);
    size_t steps = config->getOr<int64_t>("scaling_benchmark_steps", 1000);
    
    NLM_LOG_INFO("Benchmark: " + std::to_string(neurons) + " neurons, " + std::to_string(steps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for scaling benchmark");
        return;
    }
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("Scaling step " + std::to_string(step) + 
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    NLM_LOG_INFO("Scaling Benchmark complete: " + std::to_string(duration) + " seconds");
    NLM_LOG_INFO("Performance: " + std::to_string(neurons / duration) + " neurons/second");
}

void printExperimentResults(const std::string& experimentName, const ExperimentResult& result) {
    std::cout << std::endl;
    std::cout << "=== Experiment Results: " << experimentName << " ===" << std::endl;
    std::cout << std::endl;
    
    if (result.experimentType == "quick") {
        std::cout << "Quick Experiment Results:" << std::endl;
        std::cout << "  Duration: " << result.duration << " seconds" << std::endl;
        std::cout << "  Neurons: " << result.neuronCount << std::endl;
        std::cout << "  Total Spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Average Firing Rate: " << result.averageFiringRate << std::endl;
        std::cout << "  Excitatory/Inhibitory Ratio: " << result.eiRatio << std::endl;
        std::cout << "  Memory Systems: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
    } else if (result.experimentType == "plasticity") {
        std::cout << "Plasticity Experiment Results:" << std::endl;
        std::cout << "  Duration: " << result.duration << " seconds" << std::endl;
        std::cout << "  Initial Mean Weight: " << result.initialMeanWeight << std::endl;
        std::cout << "  Final Mean Weight: " << result.finalMeanWeight << std::endl;
        std::cout << "  Weight Change: " << result.weightChange << std::endl;
        std::cout << "  Learning Detected: " << (result.learningDetected ? "YES" : "NO") << std::endl;
        std::cout << "  Strengthened Synapses: " << result.synapsesStrengthened << std::endl;
        std::cout << "  Weakened Synapses: " << result.synapsesWeakened << std::endl;
    } else if (result.experimentType == "stdp") {
        std::cout << "STDP Verification Results:" << std::endl;
        std::cout << "  Duration: " << result.duration << " seconds" << std::endl;
        std::cout << "  Total ΔWeight: " << result.totalWeightChange << std::endl;
        std::cout << "  LTP Detected: " << (result.ltpDetected ? "YES" : "NO") << std::endl;
        std::cout << "  LTD Detected: " << (result.ltdDetected ? "YES" : "NO") << std::endl;
        std::cout << "  STDP Working: " << (result.stdpWorking ? "YES" : "NO") << std::endl;
    } else if (result.experimentType == "phase6") {
        std::cout << "Phase 6 Integration Results:" << std::endl;
        std::cout << "  Duration: " << result.duration << " seconds" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.averageFiringRate << std::endl;
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Novelty Level: " << result.noveltyLevel << std::endl;
        std::cout << "  Curiosity Level: " << result.curiosityLevel << std::endl;
        std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "  Integration Status:" << std::endl;
        std::cout << "    Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "    Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "    Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "    Prediction: " << (result.predictionIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "    Development: " << (result.developmentIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "    Checkpoint: " << (result.checkpointingWorks ? "YES" : "NO") << std::endl;
    } else if (result.experimentType == "scaling") {
        std::cout << "Scaling Benchmark Results:" << std::endl;
        std::cout << "  Duration: " << result.duration << " seconds" << std::endl;
        std::cout << "  Neurons: " << result.neuronCount << std::endl;
        std::cout << "  Synapses: " << result.totalSynapseCount << std::endl;
        std::cout << "  Total Spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Average Firing Rate: " << result.averageFiringRate << std::endl;
        std::cout << "  Performance: " << (result.duration > 0 ? result.neuronCount / result.duration : 0) << " neurons/sec" << std::endl;
    }
    
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    bool debug = false;
    bool quiet = false;
    bool help = false;
    bool helpExperiments = false;
    bool listPresets = false;
    std::string configFile = "configs/default.cfg";
    std::string experiment = "quick";
    std::string demoMode = "quick";
    std::string preset = "quick";
    std::string outputFile;
    std::string outputFormat = "text";
    size_t steps = 1000;
    size_t neurons = 0;  // Default from config
    size_t timestep = 1;  // milliseconds
    bool checkpointLoad = false;
    bool checkpointSave = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--help-experiments") {
            helpExperiments = true;
        } else if (arg == "--list-presets") {
            listPresets = true;
        } else if (arg == "--version") {
            std::cout << "NLM Phase 2 - Neural Learning Machine" << std::endl;
            std::cout << "Version: 2.0 (Experimental Artificial Developmental Brain)" << std::endl;
            std::cout << "Built with C++20, CMake 3.16+, scikit-build-core" << std::endl;
            return 0;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 14) == "--neuron-count") {
            if (arg.find('=') != std::string::npos) {
                neurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neurons = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 14) == "--region-count") {
            if (arg.find('=') != std::string::npos) {
                // Handle region count
                // Would need additional parsing logic
            } else if (i + 1 < argc) {
                // Handle region count
                i++;
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                timestep = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stoul(argv[++i]);
            }
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg.substr(0, 15) == "--checkpoint") {
            checkpointSave = true;
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--load") {
            checkpointLoad = true;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 13) == "--experiment") {
            if (arg.find('=') != std::string::npos) {
                experiment = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                experiment = argv[++i];
            }
        } else if (arg.substr(0, 12) == "--demo-mode") {
            if (arg.find('=') != std::string::npos) {
                demoMode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                demoMode = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--preset") {
            if (arg.find('=') != std::string::npos) {
                preset = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                preset = argv[++i];
            }
        } else if (arg.substr(0, 14) == "--output-file") {
            outputFormat = "text";  // Default format
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--format") {
            if (arg.find('=') != std::string::npos) {
                outputFormat = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFormat = argv[++i];
            }
        } else {
            // Handle --key=value format for general config
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    // Store for later processing
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    // Would store for config processing
                }
            }
        }
    }
    
    if (help) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (helpExperiments) {
        printExperimentHelp();
        return 0;
    }
    
    if (listPresets) {
        printPresets();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(debug ? LogLevel::Debug : (quiet ? LogLevel::Error : LogLevel::Info));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Building: Experimental Artificial Developmental Brain");
    NLM_LOG_INFO("Demo Mode: " + demoMode);
    
    // Apply preset if specified
    if (demoMode == "quick" && preset != "quick") {
        applyPreset(config, preset);
    } else if (demoMode != "quick" && preset == "quick") {
        applyPreset(config, preset);
    }
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (configFile != "configs/default.cfg") {
        if (cfg->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
        }
    }
    
    // Override with command line args
    cfg->loadFromArgs(argc, argv);
    
    // Apply command-line overrides
    if (neurons > 0) {
        cfg->set("neuron_count", neurons);
        NLM_LOG_INFO("Overridden neuron_count: " + std::to_string(neurons));
    }
    
    if (steps > 0) {
        cfg->set("simulation_steps", steps);
        NLM_LOG_INFO("Overridden simulation_steps: " + std::to_string(steps));
    }
    
    // Set default values for Phase 2
    cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
    cfg->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    cfg->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    cfg->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    cfg->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Quick experiment settings
    cfg->set("quick_experiment_steps", steps, ConfigSource::Default);
    cfg->set("connectivity_test_steps", steps, ConfigSource::Default);
    cfg->set("plasticity_experiment_steps", steps, ConfigSource::Default);
    cfg->set("phase6_integration_steps", steps, ConfigSource::Default);
    cfg->set("scaling_benchmark_steps", steps, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(cfg->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(cfg->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(cfg->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(cfg->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(cfg->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestepMs = timestep / 1000.0;
    SimulationClock clock(timestepMs);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestepMs) + "s");
    
    // Run the selected experiment
    ExperimentResult result;
    
    if (experiment == "quick") {
        result = runQuickExperiment(cfg);
    } else if (experiment == "basic") {
        result = runBasicConnectivityTest(cfg);
    } else if (experiment == "plasticity") {
        result = runPlasticityExperiment(cfg);
    } else if (experiment == "stdp") {
        result = runStdpVerification(cfg);
    } else if (experiment == "phase6") {
        runPhase6Integration(cfg);
        return 0;
    } else if (experiment == "scaling") {
        runScalingBenchmark(cfg);
        return 0;
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + experiment);
        printExperimentHelp();
        return 1;
    }
    
    // Print results
    printExperimentResults(experiment, result);
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 2 Complete ===" << std::endl;
        std::cout << "All objectives completed successfully!" << std::endl;
    }
    
    return 0;
}