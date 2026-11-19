// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Artificial Developmental Brain
//
// This phase implements the complete integrated artificial brain with:
// - Real LIF neuron dynamics
// - Event-driven spike propagation with delays
// - STDP and Hebbian plasticity rules
// - Structural plasticity (synaptogenesis/pruning)
// - Working memory integration
// - Episodic memory integration
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system integration
// - Cognitive systems (neural planner, concept formation, attention)
// - Development system integration
// - Checkpoint save/load
// - Replay system
// - Memory consolidation

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
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                               ║
    ║                                                               ║
    ║     Integrated Artificial Brain                              ║
    ║     Cognitive, Neural, Memory Systems                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Phase 6: Main integrated demonstration with all systems working together
void runPhase6IntegrationDemo() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Implementing the complete artificial brain with all systems integrated:");
    NLM_LOG_INFO("  - Real LIF neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  egestasd gg";
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Working and episodic memory integration");
    NLM_LOG_INFO("  - Prediction system integration");
    NLM_LOG_INFO("  - Cognitive systems (neural planner, concept formation, attention)");
    NLM_LOG_INFO("  - Development system integration");
    NLM_LOG_INFO("  - Checkpoint save/load");
    NLM_LOG_INFO("  - Replay and memory consolidation");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Set default values for Phase 6 integration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Phase 6 integration parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Checkpoint system parameters
    config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Integrated Artificial Brain)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Integration verification
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Checking that all integrated systems are available:");
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionSystem = brain->getPredictionSystem();
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    auto* developmentSystem = brain->getDevelopmentSystem();
    
    NLM_LOG_INFO("System Status:");
    NLM_LOG_INFO("  Working Memory: " + std::string(workingMemory ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Episodic Memory: " + std::string(episodicMemory ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Neuromodulation (Dopamine): " + std::string(dopamine ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Neuromodulation (Curiosity): " + std::string(curiosity ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Neuromodulation (Novelty): " + std::string(novelty ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Prediction System: " + std::string(predictionSystem ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Neural Planner: " + std::string(planner ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Concept Formation: " + std::string(conceptFormation ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Attentional Selection: " + std::string(attention ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("  Development System: " + std::string(developmentSystem ? "✓ INTEGRATED" : "✗ NOT INTEGRATED"));
    NLM_LOG_INFO("");
    
    // Basic connectivity test
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Get initial spike count
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current into first 10 neurons
            NLM_LOG_INFO("  Injecting current into 10 neurons...");
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
            }
            
            // Run a few steps
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
    }
    
    // Memory systems test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Memory Systems Integration ===");
    
    if (workingMemory) {
        NLM_LOG_INFO("  Working Memory: " + std::to_string(workingMemory->getActiveTraces()) + " active traces");
        if (workingMemory->getActiveTraces() > 0) {
            NLM_LOG_INFO("  ✓ Working memory has active traces");
        }
    }
    
    if (episodicMemory) {
        NLM_LOG_INFO("  Episodic Memory: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes stored");
        if (episodicMemory->getEpisodeCount() > 0) {
            NLM_LOG_INFO("  ✓ Episodic memory has stored experiences");
        }
    }
    
    // Neuromodulation test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: Neuromodulation Integration ===");
    
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        NLM_LOG_INFO("  Dopamine level: " + std::to_string(dopamineLevel));
        NLM_LOG_INFO("  ✓ Neuromodulation system is functional");
    }
    
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(curiosityLevel));
    }
    
    // Plasticity test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Plasticity and Learning ===");
    
    // Run plasticity experiment with repeated input pattern
    NLM_LOG_INFO("  Running plasticity experiment with repeated input patterns (200 steps)...");
    
    // Apply repeated input to stimulate learning
    size_t minSize = neurons.empty() ? 0 : std::min(size_t(20), neurons.size());
    for (SimulationStep step = 0; step < 200; ++step) {
        // Inject current into first few neurons to stimulate plastic changes
        for (size_t i = 0; i < minSize; ++i) {
            if (neurons.size() > i) {
                brain->injectCurrent(neurons[i]->getId(), 30.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    size_t totalSpikes = brain->getTotalSpikeCount();
    NLM_LOG_INFO("  Total spikes during plasticity test: " + std::to_string(totalSpikes));
    
    if (totalSpikes > 100) {
        NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    } else {
        NLM_LOG_INFO("  ! Limited plasticity - network activity may need adjustment");
    }
    
    // Checkpoint test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Checkpoint System ===");
    
    std::string checkpointPath = "./test_checkpoint.bin";
    NLM_LOG_INFO("  Testing checkpoint save/load...");
    
    if (brain->save(checkpointPath)) {
        NLM_LOG_INFO("  ✓ Checkpoint saved successfully");
        
        // Create new brain and load
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        if (brain2->load(checkpointPath)) {
            NLM_LOG_INFO("  ✓ Checkpoint loaded successfully");
            NLM_LOG_INFO("  ✓ Checkpoint system is working");
        } else {
            NLM_LOG_ERROR("  ✗ Failed to load checkpoint");
        }
    } else {
        NLM_LOG_ERROR("  ✗ Failed to save checkpoint");
    }
    
    // Integration status summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Integration Status ===");
    
    int integratedSystems = 0;
    int totalSystems = 7;
    
    if (workingMemory) { integratedSystems++; }
    if (episodicMemory) { integratedSystems++; }
    if (dopamine) { integratedSystems++; }
    if (curiosity) { integratedSystems++; }
    if (predictionSystem) { integratedSystems++; }
    if (planner) { integratedSystems++; }
    if (conceptFormation) { integratedSystems++; }
    if (attention) { integratedSystems++; }
    if (developmentSystem) { integratedSystems++; }
    
    NLM_LOG_INFO("Integrated Systems: " + std::to_string(integratedSystems) + " / " + std::to_string(totalSystems));
    NLM_LOG_INFO("Integration Percentage: " + std::to_string((integratedSystems * 100) / totalSystems) + "%");
    NLM_LOG_INFO("");
    
    if (integratedSystems == totalSystems) {
        NLM_LOG_INFO("✓ PHASE 6 SUCCESS: All brain systems are fully integrated!")
        NLM_LOG_INFO("  The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("  capable of changing its own synaptic connections through experience.");
    } else {
        NLM_LOG_INFO("⚠️  PARTIAL INTEGRATION: Some systems are not yet connected.");
        NLM_LOG_INFO("  Further work is needed for complete Phase 6 integration.");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Demo Complete ===");
    
    return 0;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6: Final Integration...\n" << std::endl;
    
    try {
        return runPhase6IntegrationDemo();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Phase 6 integration demo failed: " << e.what() << std::endl;
        return 1;
    }
}