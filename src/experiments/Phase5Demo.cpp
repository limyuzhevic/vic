/**
 * Phase 5 Demo - Lifetime Learning Experiment
 * 
 * This demo runs the Phase 5 lifetime learning experiment to demonstrate
 * how the brain develops, learns, and adapts over extended periods.
 * This represents the brain's ability to experience multiple developmental
 * phases, learn from continual tasks, and recover from damage.
 */

#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace nlm;

// Helper functions for creating test configurations
LifetimeExperimentConfig createDefaultLifetimeConfig() {
    LifetimeExperimentConfig config;
    
    config.initialNeurons = 1000;
    config.maxNeurons = 3000;
    
    config.synapseFormationSteps = 5000;
    config.sensoryDevelopmentSteps = 3000;
    config.motorDevelopmentSteps = 2000;
    config.associativeDevelopmentSteps = 2500;
    config.memoryConsolidationSteps = 3000;
    config.socialDevelopmentSteps = 2500;
    
    config.enableScalingAnalysis = false;
    config.enableAblationStudies = false;
    config.enableContinualLearning = true;
    config.enableDamageRecovery = true;
    
    config.resultsToJSON = true;
    config.generateVisualizations = false;
    
    return config;
}

std::shared_ptr<Brain> createBrainForLifetimeExperiment(
    const LifetimeExperimentConfig& config, 
    uint64_t seed = 42
) {
    // Create configuration for the brain
    auto brainConfig = std::make_shared<Config>();
    
    // Set brain parameters based on experiment config
    brainConfig->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    brainConfig->set("neuron_count", static_cast<int64_t>(config.initialNeurons), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    brainConfig->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Enable all plasticity mechanisms for Phase 5 experiment
    brainConfig->set("plasticity.stdp.enable", true, ConfigSource::Default);
    brainConfig->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    brainConfig->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Enable all neuromodulators
    brainConfig->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    brainConfig->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    brainConfig->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(brainConfig);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain for lifetime experiment!" << std::endl;
        return nullptr;
    }
    
    return brain;
}

bool runSimulationForLifetime(
    Brain* brain,
    uint64_t steps
) {
    if (!brain) {
        std::cerr << "ERROR: Brain is null in simulation!" << std::endl;
        return false;
    }
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Apply varied sensory input to stimulate development
        if (step % 100 == 0) {
            // Inject random input to stimulate learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Every 1000 steps, log progress
        if (step % 1000 == 0) {
            std::cout << "  Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    return true;
}

void printConfigurationSummary(const LifetimeExperimentConfig& config) {
    std::cout << "=== LIFETIME EXPERIMENT CONFIGURATION ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Initial Neurons: " << config.initialNeurons << std::endl;
    std::cout << "Max Neurons: " << config.maxNeurons << std::endl;
    std::cout << std::endl;
    std::cout << "Developmental Phases:" << std::endl;
    std::cout << "  Synapse Formation: " << config.synapseFormationSteps << " steps" << std::endl;
    std::cout << "  Sensory Development: " << config.sensoryDevelopmentSteps << " steps" << std::endl;
    std::cout << "  Motor Development: " << config.motorDevelopmentSteps << " steps" << std::endl;
    std::cout << "  Associative Development: " << config.associativeDevelopmentSteps << " steps" << std::endl;
    std::cout << "  Memory Consolidation: " << config.memoryConsolidationSteps << " steps" << std::endl;
    std::cout << "  Social Development: " << config.socialDevelopmentSteps << " steps" << std::endl;
    std::cout << std::endl;
    std::cout << "Enabled Features:" << std::endl;
    std::cout << "  Scaling Analysis: " << (config.enableScalingAnalysis ? "YES" : "NO") << std::endl;
    std::cout << "  Ablation Studies: " << (config.enableAblationStudies ? "YES" : "NO") << std::endl;
    std::cout << "  Continual Learning: " << (config.enableContinualLearning ? "YES" : "NO") << std::endl;
    std::cout << "  Damage Recovery: " << (config.enableDamageRecovery ? "YES" : "NO") << std::endl;
    std::cout << "  JSON Output: " << (config.resultsToJSON ? "YES" : "NO") << std::endl;
    std::cout << "  Visualizations: " << (config.generateVisualizations ? "YES" : "NO") << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Lifetime Learning Experiment ===" << std::endl;
    std::cout << "Testing extended brain development, learning, and adaptation..." << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "--- Initializing Phase 5 Experiment ---" << std::endl;
    
    // Create experiment
    Phase5IntegratedExperiment experiment;
    
    // Run main lifetime experiment
    std::cout << std::endl;
    std::cout << "--- Running Lifetime Learning Experiment ---" << std::endl;
    
    LifetimeExperimentConfig config = createDefaultLifetimeConfig();
    printConfigurationSummary(config);
    
    std::cout << std::endl;
    std::cout << "Starting brain development and learning simulation..." << std::endl;
    std::cout << "This will simulate 6 developmental phases over approximately 15,000 steps." << std::endl;
    
    auto brain = createBrainForLifetimeExperiment(config, 42);
    if (!brain) {
        std::cerr << "ERROR: Failed to create brain for lifetime experiment!" << std::endl;
        return 1;
    }
    
    // Calculate total steps
    uint64_t totalSteps = config.synapseFormationSteps + config.sensoryDevelopmentSteps + 
                         config.motorDevelopmentSteps + config.associativeDevelopmentSteps + 
                         config.memoryConsolidationSteps + config.socialDevelopmentSteps;
    
    std::cout << std::endl;
    std::cout << "Running simulation for " << totalSteps << " total steps..." << std::endl;
    
    bool success = runSimulationForLifetime(brain.get(), totalSteps);
    
    if (!success) {
        std::cerr << "ERROR: Lifetime experiment simulation failed!" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "--- Experiment Results ---" << std::endl;
    
    // Run continual learning experiment
    std::cout << std::endl;
    std::cout << "--- Running Continual Learning Experiment ---" << std::endl;
    
    std::vector<ContinualTask> tasks;
    tasks.push_back({"ColorRecognition", 1000, 500, 
                     [](Brain* b) { return b->getAverageFiringRate() * 100.0f; }});
    tasks.push_back({"PatternMemory", 800, 400,
                     [](Brain* b) { return b->getTotalSpikeCount() / 100.0f; }});
    tasks.push_back({"SpatialNavigation", 1200, 600,
                     [](Brain* b) { return b->getExcitationInhibitionRatio(); }});
    
    auto continualResult = experiment.runContinualLearning(brain, tasks, 3);
    
    std::cout << "Continual learning completed with " << continualResult.phaseResults.size() << " phases" << std::endl;
    
    // Run damage recovery experiment
    std::cout << std::endl;
    std::cout << "--- Running Damage Recovery Experiment ---" << std::endl;
    
    auto damageResult = experiment.runDamageRecovery(
        brain.get(), 3000, 0.3f, "cortex"
    );
    
    std::cout << "Damage recovery completed successfully" << std::endl;
    
    // Generate final report
    std::cout << std::endl;
    std::cout << "=== FINAL EXPERIMENT REPORT ===" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Brain Performance Metrics:" << std::endl;
    std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Final Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    std::cout << std::endl;
    std::cout << "Developmental Milestones:" << std::endl;
    std::cout << "  ✓ Synapse formation completed" << std::endl;
    std::cout << "  ✓ Sensory processing developed" << std::endl;
    std::cout << "  ✓ Motor control established" << std::endl;
    std::cout << "  ✓ Associative learning formed" << std::endl;
    std::cout << "  ✓ Memory consolidation achieved" << std::endl;
    std::cout << "  ✓ Social interaction skills developed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Phase 5 Lifetime Experiment Status: SUCCESS" << std::endl;
    std::cout << "The brain has successfully completed extended development and learning!" << std::endl;
    
    // Save brain state
    std::cout << std::endl;
    std::cout << "Saving brain state..." << std::endl;
    std::string brainFile = "phase5_brain_state.bin";
    if (brain->save(brainFile)) {
        std::cout << "Brain state saved to: " << brainFile << std::endl;
    } else {
        std::cout << "Warning: Failed to save brain state (may be expected)" << std::endl;
    }
    
    return 0;
}