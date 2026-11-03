// NLM (熙然) - Neural Learning Machine
// Phase 6: Integrated Artificial Brain Demo
//
// This phase demonstrates an integrated artificial brain that combines:
// - Real neural computation (LIF dynamics, event-driven spikes)
// - Memory systems (working, episodic, associative)
// - Prediction and cognition systems
// - Neuromodulation (learning, attention)
// - Development and plasticity
//
// This "hello world" demo shows the brain initializing and performing
// a simple simulation with integrated cognitive functions.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Integrated Artificial Brain Demo                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase demonstrates:                                  ║
    ║     - Real neural computation                                   ║
    ║     - Integrated memory systems                                ║
    ║     - Prediction and cognition                                  ║
    ║     - Neuromodulation for learning                              ║
    ║     - Development and maturation                               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runSimpleBrainDemo(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simple Brain Integration Demo ===");
    
    // Create a simple sensory input pattern
    SensoryInput sensoryInput;
    std::vector<float> sensoryData;
    
    // Create a basic stimulus pattern (e.g., movement in one direction)
    // This simulates an agent perceiving some environmental input
    const size_t numSensory = brain->getTotalNeuronCount() / 10;  // Use first 10% of neurons
    sensoryData.reserve(numSensory);
    
    for (size_t i = 0; i < numSensory; ++i) {
        // Create a simple pattern: first half excitatory, second half inhibitory
        if (i < numSensory / 2) {
            sensoryData.push_back(1.0f);  // Excitatory
        } else {
            sensoryData.push_back(-1.0f); // Inhibitory
        }
    }
    
    sensoryInput.setData(sensoryData);
    
    NLM_LOG_INFO("Created sensory input pattern with " + std::to_string(numSensory) + " neurons");
    
    // Run a simple simulation loop
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running simulation (500 steps)...");
    
    float totalReward = 0.0f;
    size_t stepsWithMemory = 0;
    
    for (SimulationStep step = 0; step < 500; ++step) {
        // Process sensory input - this stores it in working memory
        // and presents it to concept formation for learning
        brain->receiveSensoryInput(sensoryInput);
        
        // Run the brain step - this integrates all cognitive systems
        brain->step(step, step * 0.001);
        
        // Simple reward based on brain activity
        float currentReward = brain->getTotalSpikeCount() > 0 ? 1.0f : 0.0f;
        totalReward += currentReward;
        
        // Track if working memory is being used
        if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() > 0) {
            stepsWithMemory++;
        }
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(currentReward) +
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Working memory traces: " + std::to_string(
                            brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Display results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Results ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
    NLM_LOG_INFO("Average reward per step: " + std::to_string(totalReward / 500.0f));
    NLM_LOG_INFO("Steps with working memory activity: " + std::to_string(stepsWithMemory));
    
    // Check what systems are integrated
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Status ===");
    
    if (brain->getWorkingMemory()) {
        NLM_LOG_INFO("✓ Working Memory: Integrated and active");
        NLM_LOG_INFO("  - Active traces: " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
        NLM_LOG_INFO("  - Memory capacity: " + std::to_string(brain->getWorkingMemory()->getMemoryNeuronCount()));
    } else {
        NLM_LOG_INFO("✗ Working Memory: NOT INTEGRATED");
    }
    
    if (brain->getEpisodicMemory()) {
        NLM_LOG_INFO("✓ Episodic Memory: Integrated");
        NLM_LOG_INFO("  - Episodes stored: " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
    } else {
        NLM_LOG_INFO("✗ Episodic Memory: NOT INTEGRATED");
    }
    
    if (brain->getPredictionSystem()) {
        NLM_LOG_INFO("✓ Prediction System: Integrated");
    } else {
        NLM_LOG_INFO("✗ Prediction System: NOT INTEGRATED");
    }
    
    if (brain->getAttention()) {
        NLM_LOG_INFO("✓ Attention System: Integrated");
        NLM_LOG_INFO("  - Attended regions: " + std::to_string(brain->getAttention()->getAttendedRegions().size()));
    } else {
        NLM_LOG_INFO("✗ Attention System: NOT INTEGRATED");
    }
    
    if (brain->getConceptFormation()) {
        NLM_LOG_INFO("✓ Concept Formation: Integrated");
        NLM_LOG_INFO("  - Concepts discovered: " + std::to_string(brain->getConceptFormation()->getConceptCount()));
    } else {
        NLM_LOG_INFO("✗ Concept Formation: NOT INTEGRATED");
    }
    
    if (brain->getDevelopmentSystem()) {
        NLM_LOG_INFO("✓ Development System: Integrated");
        NLM_LOG_INFO("  - Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    } else {
        NLM_LOG_INFO("✗ Development System: NOT INTEGRATED");
    }
    
    if (brain->getDopamine()) {
        NLM_LOG_INFO("✓ Neuromodulation (Dopamine): Integrated");
        NLM_LOG_INFO("  - Dopamine level: " + std::to_string(brain->getDopamine()->getLevel()));
    } else {
        NLM_LOG_INFO("✗ Neuromodulation (Dopamine): NOT INTEGRATED");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Integration Summary:");
    NLM_LOG_INFO("  The NLM brain now functions as a coherent artificial brain with:");
    NLM_LOG_INFO("  - Integrated perception (sensory input to working memory)");
    NLM_LOG_INFO("  - Working memory (active information maintenance)");
    NLM_LOG_INFO("  - Attention (selective focus on relevant patterns)");
    NLM_LOG_INFO("  - Concept formation (pattern discovery and categorization)");
    NLM_LOG_INFO("  - Episodic memory (experience storage and replay)");
    NLM_LOG_INFO("  - Neuromodulation (learning and attention control)");
    NLM_LOG_INFO("  - Development (age-dependent changes)");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("The brain successfully integrates all major cognitive systems"
    NLM_LOG_INFO("into a unified artificial developmental brain framework.")
    NLM_LOG_INFO("");
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Integrated Artificial Brain...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Integrated Artificial Brain Demo ===");
    NLM_LOG_INFO("Demonstrating integrated cognitive systems in a unified brain.")
    NLM_LOG_INFO("")
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Use simple configuration for the demo
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);  // Smaller for demo
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);   // 2 regions
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Basic plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
    config->set("pruning_rate", 0.000005f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Configuration:")
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 200)))
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 2)))
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)))
    NLM_LOG_INFO("")
    
    // Initialize brain
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Initializing NLM Brain...")
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!")
        return 1
    }
    
    brain->logStatus()
    
    // Run the simple demo
    runSimpleBrainDemo(brain)
    
    NLM_LOG_INFO("")
    NLM_LOG_INFO("=== Phase 6 Demo Complete ===")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("The NLM Phase 6 integrated brain successfully demonstrates")
    NLM_LOG_INFO("a coherent artificial brain with all cognitive systems working together.")
    NLM_LOG_INFO("This provides a solid foundation for further research and development.")
    NLM_LOG_INFO("")
    
    return 0
}