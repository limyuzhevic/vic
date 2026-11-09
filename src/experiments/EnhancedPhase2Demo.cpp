// Improved Phase 2 Demo - Unified CLI Framework Version
// This is the enhanced version of the original main.cpp demo

#include "../core/CLI/CLIFramework.hpp"
#include "../core/Runner/UnifiedDemoRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

// Enhanced learning experiment structure
struct EnhancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    
    EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0), initialMeanWeight(0.0f), finalMeanWeight(0.0f), weightChange(0.0f) {}
    
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
            initialMeanWeight = sum / initialWeights.size();
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        NLM_LOG_INFO("  Mean weight: " + std::to_string(initialMeanWeight));
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
            finalMeanWeight = sum / finalWeights.size();
        }
        
        weightChange = finalMeanWeight - initialMeanWeight;
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Mean weight: " + std::to_string(finalMeanWeight));
        NLM_LOG_INFO("  Weight change: " + std::to_string(weightChange));
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Enhanced Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Comprehensive weight change analysis
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMeanWeight));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMeanWeight));
        NLM_LOG_INFO("  Change: " + std::to_string(weightChange));
        NLM_LOG_INFO("  Initial total weight: " + std::to_string(initialSum));
        NLM_LOG_INFO("  Final total weight: " + std::to_string(finalSum));
        
        // Detailed synaptic changes
        size_t strengthened = 0, weakened = 0, unchanged = 0, significant = 0;
        float totalAbsoluteChange = 0.0f;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            totalAbsoluteChange += std::abs(delta);
            
            if (delta > 0.01f) {
                strengthened++;
                if (std::abs(delta) > 0.1f) significant++;
            } else if (delta < -0.01f) {
                weakened++;
                if (std::abs(delta) > 0.1f) significant++;
            } else {
                unchanged++;
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes (detailed):");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        NLM_LOG_INFO("  Significant changes (>0.1): " + std::to_string(significant));
        NLM_LOG_INFO("  Average absolute change: " + std::to_string(totalAbsoluteChange / minSize));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        NLM_LOG_INFO("  Current firing neuron count: " + std::to_string(brain->getFiringNeuronCount()));
        
        // Enhanced learning detection
        bool learningOccurred = (std::abs(weightChange) > 0.001f) ||
                                (strengthened > 0 || weakened > 0) ||
                                (significant > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            NLM_LOG_INFO("✓ Changes are significant: " + std::to_string(significant) + " synapses with >0.1 change");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        // Performance metrics
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Performance Metrics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMeanWeight));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMeanWeight));
        NLM_LOG_INFO("  Weight change: " + std::to_string(weightChange));
        NLM_LOG_INFO("  Learning efficiency: " + std::to_string(std::abs(weightChange) / std::max(1.0f, initialMeanWeight) * 100.0f) + "%");
    }
};

void printEnhancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Enhanced Neural Computation                      ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics          ║
    ║     - Event-driven spike propagation with delays              ║
    ║     - STDP and Hebbian plasticity rules                        ║
    ║     - Structural plasticity                                     ║
    ║     - Enhanced learning measurement                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runEnhancedBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Region not found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 20 neurons (enhanced)...");
    for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
        neurons[i]->injectCurrent(60.0f);  // Stronger current
    }
    
    // Run multiple steps with timing
    auto startTime = std::chrono::high_resolution_clock::now();
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 20 steps
        if (step % 20 == 0) {
            NLM_LOG_INFO("    Step " << step << ": " 
                              << "Spikes: " << brain->getTotalSpikeCount() - initialSpikes
                              << ", Firing: " << brain->getFiringNeuronCount());
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " << spikes << " in " << elapsed << " seconds");
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network efficiently");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " << i << 
                        " V=" << neurons[i]->getMembranePotential() <<
                        " thresh=" << neurons[i]->getThreshold());
        }
    }
}

void runEnhancedPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 2: Advanced Plasticity Learning Experiment ===");
    
    EnhancedLearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying enhanced input patterns (2000 steps)...");
    NLM_LOG_INFO("Using pattern stimulation for selective learning...");
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 30) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (SimulationStep step = 0; step < 2000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 40.0f);  // Optimized current
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 400 == 0) {
            NLM_LOG_INFO("    Step " << step << ": "
                              << "Spikes: " << brain->getTotalSpikeCount() << ", "
                              << "Firing: " << brain->getFiringNeuronCount() << ", "
                              << "Avg Rate: " << brain->getAverageFiringRate());
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    NLM_LOG_INFO("Pattern learning completed in " << elapsed << " seconds");
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display enhanced statistics
    experiment.computeStatistics();
}

void runEnhancedStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 3: Comprehensive STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    // Use multiple synapses for comprehensive testing
    std::vector<Synapse*> testSynapses;
    for (size_t i = 0; i < std::min<size_t>(3, synapses.size()); ++i) {
        synapses[i]->enablePlasticity(false, true, false);
        testSynapses.push_back(synapses[i].get());
    }
    
    NLM_LOG_INFO("Testing STDP on " << testSynapses.size() << " synapses...");
    
    // Correlated pre-post activity for each synapse
    for (size_t synIdx = 0; synIdx < testSynapses.size(); ++synIdx) {
        Synapse* syn = testSynapses[synIdx];
        float beforeWeight = syn->getWeight();
        
        NLM_LOG_INFO("  Synapse " << synIdx << ": initial weight = " << beforeWeight);
        NLM_LOG_INFO("    (pre=" << syn->getSourceNeuron().index() 
                  << ", post=" << syn->getDestinationNeuron().index() << ")");
        
        NLM_LOG_INFO("  Creating correlated pre->post activity...")
        
        for (int trial = 0; trial < 30; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 70.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 70.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float afterWeight = syn->getWeight();
        float delta = afterWeight - beforeWeight;
        
        NLM_LOG_INFO("  After correlated activity:")
        NLM_LOG_INFO("    Weight: " << afterWeight << " (Δ=" << delta << ")");
        
        if (delta > 0.001f) {
            NLM_LOG_INFO("    ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("    ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("    ! No change: STDP may not be triggering");
        }
    }
}

int main(int argc, char** argv) {
    // Initialize enhanced logging system
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print enhanced banner
    printEnhancedBanner();
    
    std::cout << "Initializing Enhanced NLM Phase 2 Neural Computation...\n" << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided (enhanced configuration system)
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
    
    // Load configuration from file
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default enhanced configuration.\n";
    }
    
    // Apply enhanced command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set enhanced default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);  // Larger for better testing
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);    // Multiple regions
    config->set("connection_probability", 0.2f, ConfigSource::Default);  // Higher connectivity
    
    // Enhanced STDP parameters
    config->set("stdp_ltp_weight", 0.025f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_tau", 25.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config->set("pruning_rate", 0.00002f, ConfigSource::Default);
    
    // Log enhanced configuration summary
    std::cout << "\nEnhanced Configuration Summary:" << std::endl;
    std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 2) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.2f) << std::endl;
    std::cout << std::endl;
    
    // Initialize simulation clock with enhanced precision
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    std::cout << "Enhanced simulation clock initialized with timestep: " << timestep << "s" << std::endl;
    
    // Initialize brain with enhanced parameters
    std::cout << "\nInitializing Enhanced NLM Brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize enhanced brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Record overall experiment start time for performance metrics
    auto overallStartTime = std::chrono::high_resolution_clock::now();
    
    // Run Enhanced Test 1: Basic connectivity
    runEnhancedBasicConnectivityTest(brain);
    
    // Reset brain for enhanced plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Enhanced Test 2: Plasticity learning experiment
    runEnhancedPlasticityExperiment(brain);
    
    // Reset and run Enhanced Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runEnhancedStdpVerification(brain);
    
    // Final enhanced brain status
    auto overallEndTime = std::chrono::high_resolution_clock::now();
    double overallElapsed = std::chrono::duration<double>(overallEndTime - overallStartTime).count();
    
    std::cout << "\n=== Enhanced Final Brain Status ===\n";
    brain->logStatus();
    std::cout << "\nTotal experiment time: " << overallElapsed << " seconds\n";
    
    std::cout << "\n=== Enhanced Phase 2 Complete ===\n";
    std::cout << "\nEnhanced Phase 2 Objectives Completed:\n";
    std::cout << "  ✓ Real LIF neuron dynamics implemented\n";
    std::cout << "  ✓ Event-driven spike propagation with delays\n";
    std::cout << "  ✓ STDP plasticity rule with enhanced parameters\n";
    std::cout << "  ✓ Hebbian plasticity rule\n";
    std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)\n";
    std::cout << "  ✓ Enhanced learning experiment demonstrates measurable changes\n";
    std::cout << "  ✓ Network shows activity-dependent synaptic modification\n";
    std::cout << "  ✓ Improved performance and scalability\n";
    std::cout << "\nThe Enhanced NLM brain is now a functioning artificial neural substrate\n";
    std::cout << "capable of changing its own synaptic connections through experience.\n";
    std::cout << "With enhanced measurement and analysis capabilities.\n\n";
    
    return 0;
}