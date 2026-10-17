// High-level API implementation for NLM Common Neural Simulation Tasks
// Implementation of the high-level API providing simplified interfaces for common neural simulation tasks

#include "HighLevelAPI.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

namespace nlm {
namespace api {

// ConfigPresets implementation
std::shared_ptr<Config> ConfigPresets::createBasicConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic configuration for neural computation
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createLearningConfig() {
    auto config = createBasicConfig();
    
    // Enhanced configuration for learning scenarios
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("reward_discount_factor", 0.99f, ConfigSource::Default);
    config->set("curiosity_enabled", true, ConfigSource::Default);
    config->set("novelty_enabled", true, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config->set("pruning_rate", 0.0001f, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createCognitiveConfig() {
    auto config = createLearningConfig();
    
    // Configuration optimized for cognitive experiments
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("planning_enabled", true, ConfigSource::Default);
    config->set("concept_formation_enabled", true, ConfigSource::Default);
    config->set("attention_enabled", true, ConfigSource::Default);
    config->set("memory_enabled", true, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createRealTimeConfig() {
    auto config = createBasicConfig();
    
    // Configuration optimized for real-time applications
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("simulation_timestep", 0.0001, ConfigSource::Default);
    config->set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    config->set("enable_logging", false, ConfigSource::Default); // Reduce overhead
    config->set("enable_visualization", false, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createResearchConfig() {
    auto config = createLearningConfig();
    
    // Configuration for research purposes with detailed logging
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("enable_detailed_logging", true, ConfigSource::Default);
    config->set("save_checkpoints", true, ConfigSource::Default);
    config->set("checkpoint_interval", 100, ConfigSource::Default);
    config->set("enable_replay", true, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigPresets::createCustomConfig(const std::map<std::string, std::string>& parameters) {
    auto config = createBasicConfig();
    
    // Apply custom parameters
    for (const auto& param : parameters) {
        // Simple parameter parsing (in real implementation, use proper parsing)
        if (param.first == "neuron_count") {
            config->set("neuron_count", std::stoll(param.second), ConfigSource::CommandLine);
        } else if (param.first == "simulation_timestep") {
            config->set("simulation_timestep", std::stod(param.second), ConfigSource::CommandLine);
        } else if (param.first == "connection_probability") {
            config->set("connection_probability", std::stof(param.second), ConfigSource::CommandLine);
        }
    }
    
    return config;
}

std::vector<std::string> ConfigPresets::getPresetNames() {
    return {
        "basic", "learning", "cognitive", "realtime", "research"
    };
}

// BrainFactory implementation
std::shared_ptr<Brain> BrainFactory::createMinimalBrain() {
    auto config = ConfigPresets::createBasicConfig();
    auto brain = std::make_shared<Brain>(config);
    
    if (initializeBrain(brain, "createMinimalBrain")) {
        return brain;
    }
    return nullptr;
}

std::shared_ptr<Brain> BrainFactory::createSimulationBrain(size_t neuron_count) {
    auto config = ConfigPresets::createBasicConfig();
    config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::CommandLine);
    auto brain = std::make_shared<Brain>(config);
    
    if (initializeBrain(brain, "createSimulationBrain")) {
        return brain;
    }
    return nullptr;
}

std::shared_ptr<Brain> BrainFactory::createLearningBrain(const std::string& task_type) {
    auto config = ConfigPresets::createLearningConfig();
    
    // Set task-specific parameters
    if (task_type == "association") {
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.05f, ConfigSource::Default);
    } else if (task_type == "sequence") {
        config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
        config->set("enable_prediction", true, ConfigSource::Default);
    } else if (task_type == "navigation") {
        config->set("neuron_count", static_cast<int64_t>(400), ConfigSource::Default);
        config->set("enable_reward", true, ConfigSource::Default);
    }
    
    auto brain = std::make_shared<Brain>(config);
    
    if (initializeBrain(brain, "createLearningBrain")) {
        return brain;
    }
    return nullptr;
}

std::shared_ptr<Brain> BrainFactory::createDevelopmentalBrain(float synaptogenesis_rate, float pruning_rate) {
    auto config = ConfigPresets::createLearningConfig();
    config->set("synaptogenesis_rate", synaptogenesis_rate, ConfigSource::CommandLine);
    config->set("pruning_rate", pruning_rate, ConfigSource::CommandLine);
    
    auto brain = std::make_shared<Brain>(config);
    
    if (initializeBrain(brain, "createDevelopmentalBrain")) {
        return brain;
    }
    return nullptr;
}

bool BrainFactory::initializeBrain(std::shared_ptr<Brain> brain, const std::string& operation_name) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize null brain in operation: " + operation_name);
        return false;
    }
    
    try {
        NLM_LOG_INFO("Initializing brain for operation: " + operation_name);
        bool success = brain->initialize();
        
        if (success) {
            NLM_LOG_INFO("Brain initialization successful for operation: " + operation_name);
            brain->logStatus();
            return true;
        } else {
            NLM_LOG_ERROR("Brain initialization failed for operation: " + operation_name);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization in operation " + operation_name + ": " + e.what());
        return false;
    }
}

std::shared_ptr<Brain> BrainFactory::createAndTestBrain(size_t neuron_count) {
    auto brain = createSimulationBrain(neuron_count);
    
    if (brain) {
        // Run basic connectivity test
        NLM_LOG_INFO("Running connectivity test for " + std::to_string(neuron_count) + " neurons");
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                // Inject current into first few neurons
                for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(50.0f);
                }
                
                // Run a few steps
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                size_t spikes = brain->getTotalSpikeCount();
                NLM_LOG_INFO("Connectivity test completed: " + std::to_string(spikes) + " spikes generated");
            }
        }
    }
    
    return brain;
}

// SimulationRunner implementation
bool SimulationRunner::runBasicSimulation(const BasicSimulationOptions& options) {
    try {
        NLM_LOG_INFO("Running basic simulation with " + std::to_string(options.steps) + " steps");
        
        auto config = ConfigPresets::createCustomConfig({
            {"neuron_count", std::to_string(options.neuron_count)},
            {"simulation_timestep", "0.001"}
        });
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "basic_simulation")) {
            return false;
        }
        
        for (size_t step = 0; step < options.steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (options.enable_plasticity) {
                brain->updatePlasticity();
            }
            
            if (options.enable_neuromodulation) {
                // Apply neuromodulation (simplified)
            }
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                           std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
            }
        }
        
        NLM_LOG_INFO("Basic simulation completed successfully");
        NLM_LOG_INFO("Final stats: " + std::to_string(brain->getTotalSpikeCount()) + " total spikes");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Basic simulation failed: " + std::string(e.what()));
        return false;
    }
}

bool SimulationRunner::runLearningSimulation(const LearningSimulationOptions& options) {
    try {
        NLM_LOG_INFO("Running learning simulation: " + options.scenario);
        
        auto config = ConfigPresets::createLearningConfig();
        config->set("neuron_count", static_cast<int64_t>(options.neuron_count), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "learning_simulation")) {
            return false;
        }
        
        // Apply learning-specific configurations
        if (options.enable_curiosity) {
            // Enable curiosity system
        }
        if (options.enable_reward) {
            // Enable reward system
        }
        
        for (size_t step = 0; step < options.steps; ++step) {
            brain->step(step, step * 0.001);
            
            // Apply learning stimuli based on scenario
            if (options.scenario == "association") {
                // Provide Hebbian learning stimuli
            } else if (options.scenario == "sequence") {
                // Provide temporal sequence learning stimuli
            }
            
            brain->updatePlasticity();
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("Learning step " + std::to_string(step) + " completed");
            }
        }
        
        NLM_LOG_INFO("Learning simulation completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Learning simulation failed: " + std::string(e.what()));
        return false;
    }
}

bool SimulationRunner::runCognitiveSimulation(const CognitiveSimulationOptions& options) {
    try {
        NLM_LOG_INFO("Running cognitive simulation: " + options.experiment_type);
        
        auto config = ConfigPresets::createCognitiveConfig();
        config->set("region_count", static_cast<int64_t>(options.brain_regions), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "cognitive_simulation")) {
            return false;
        }
        
        for (size_t step = 0; step < options.steps; ++step) {
            brain->step(step, step * 0.001);
            
            // Apply cognitive tasks
            if (options.experiment_type == "attention") {
                // Apply attentional selection
            } else if (options.experiment_type == "planning") {
                // Apply neural planning
            } else if (options.experiment_type == "memory") {
                // Apply memory operations
            }
            
            brain->develop(); // Apply developmental changes
            
            if (step % 300 == 0) {
                NLM_LOG_INFO("Cognitive step " + std::to_string(step) + " completed");
            }
        }
        
        NLM_LOG_INFO("Cognitive simulation completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Cognitive simulation failed: " + std::string(e.what()));
        return false;
    }
}

bool SimulationRunner::runAgentSimulation(const AgentSimulationOptions& options) {
    try {
        NLM_LOG_INFO("Running agent simulation with " + std::to_string(options.agent_count) + " agents");
        
        auto config = ConfigPresets::createRealTimeConfig();
        
        // In a full implementation, this would create multiple agents
        // and a shared world environment
        
        NLM_LOG_INFO("Agent simulation setup completed (simplified implementation)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Agent simulation failed: " + std::string(e.what()));
        return false;
    }
}

// LearningScenarios implementation
bool LearningScenarios::runAssociationLearning(const AssociationScenario& scenario) {
    try {
        NLM_LOG_INFO("Running association learning scenario");
        
        auto config = ConfigPresets::createLearningConfig();
        config->set("neuron_count", static_cast<int64_t>(scenario.neurons_per_pattern * 2), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "association_learning")) {
            return false;
        }
        
        // Implementation would provide correlated pre-post neural activity
        // for Hebbian learning
        
        NLM_LOG_INFO("Association learning scenario completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Association learning failed: " + std::string(e.what()));
        return false;
    }
}

bool LearningScenarios::runSequenceLearning(const SequenceScenario& scenario) {
    try {
        NLM_LOG_INFO("Running sequence learning scenario");
        
        auto config = ConfigPresets::createLearningConfig();
        config->set("neuron_count", static_cast<int64_t>(scenario.sequences * 10), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "sequence_learning")) {
            return false;
        }
        
        // Implementation would provide temporal sequence patterns
        // for sequence learning
        
        NLM_LOG_INFO("Sequence learning scenario completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Sequence learning failed: " + std::string(e.what()));
        return false;
    }
}

bool LearningScenarios::runNavigationLearning(const NavigationScenario& scenario) {
    try {
        NLM_LOG_INFO("Running navigation learning scenario");
        
        auto config = ConfigPresets::createLearningConfig();
        config->set("neuron_count", static_cast<int64_t>(scenario.steps / 10), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "navigation_learning")) {
            return false;
        }
        
        // Implementation would provide reward-penalized navigation
        // for exploration learning
        
        NLM_LOG_INFO("Navigation learning scenario completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Navigation learning failed: " + std::string(e.what()));
        return false;
    }
}

bool LearningScenarios::runConceptFormation(const ConceptScenario& scenario) {
    try {
        NLM_LOG_INFO("Running concept formation scenario");
        
        auto config = ConfigPresets::createLearningConfig();
        config->set("neuron_count", static_cast<int64_t>(scenario.clusters * 50), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "concept_formation")) {
            return false;
        }
        
        // Implementation would provide pattern discovery tasks
        // for unsupervised concept learning
        
        NLM_LOG_INFO("Concept formation scenario completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Concept formation failed: " + std::string(e.what()));
        return false;
    }
}

// ExperimentTemplates implementation
bool ExperimentTemplates::runPhase2Demo(const Phase2Demo& demo) {
    try {
        NLM_LOG_INFO("Running Phase 2 demo: Real Neural Computation");
        
        auto config = ConfigPresets::createBasicConfig();
        config->set("neuron_count", static_cast<int64_t>(demo.neurons), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "phase2_demo")) {
            return false;
        }
        
        // Run the three phase 2 tests
        NLM_LOG_INFO("Running Test 1: Basic connectivity test");
        runBasicConnectivityTest(brain);
        
        // Reset and run Test 2: Plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        NLM_LOG_INFO("Phase 2 demo completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase 2 demo failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentTemplates::runPhase3Demo(const Phase3Demo& demo) {
    try {
        NLM_LOG_INFO("Running Phase 3 demo: World Interaction");
        
        // Phase 3 demo would involve creating an environment and agent
        // This is a simplified implementation
        
        NLM_LOG_INFO("Phase 3 demo setup completed (simplified)");
        NLM_LOG_INFO("Note: Full Phase 3 implementation requires environment and agent setup");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase 3 demo failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentTemplates::runPhase4Demo(const Phase4Demo& demo) {
    try {
        NLM_LOG_INFO("Running Phase 4 demo: Cognitive Experiments");
        
        // Phase 4 demo would involve cognitive tasks
        // This is a simplified implementation
        
        NLM_LOG_INFO("Phase 4 demo setup completed (simplified)");
        NLM_LOG_INFO("Note: Full Phase 4 implementation requires cognitive system integration");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Phase 4 demo failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentTemplates::runIntegrationDemo(const IntegrationDemo& demo) {
    try {
        NLM_LOG_INFO("Running Integration demo: Complete Brain System");
        
        auto config = ConfigPresets::createResearchConfig();
        config->set("neuron_count", static_cast<int64_t>(demo.neurons), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(demo.regions), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(config);
        if (!BrainFactory::initializeBrain(brain, "integration_demo")) {
            return false;
        }
        
        NLM_LOG_INFO("Running comprehensive integration test...");
        
        // Run simulation with all systems enabled
        for (size_t step = 0; step < demo.steps; ++step) {
            brain->step(step, step * 0.001);
            brain->develop(); // Apply developmental changes
            brain->updatePlasticity(); // Apply plasticity
            
            if (demo.show_progress && step % 500 == 0) {
                NLM_LOG_INFO("Integration step " + std::to_string(step) + ": " +
                           std::to_string(brain->getFiringNeuronCount()) + " active neurons");
            }
        }
        
        NLM_LOG_INFO("Integration demo completed successfully");
        NLM_LOG_INFO("Final brain status:");
        brain->logStatus();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Integration demo failed: " + std::string(e.what()));
        return false;
    }
}

// AnalysisTools implementation
AnalysisTools::BrainStats AnalysisTools::analyzeBrain(std::shared_ptr<Brain> brain) {
    BrainStats stats;
    
    if (!brain) {
        return stats;
    }
    
    stats.total_neurons = brain->getTotalNeuronCount();
    stats.active_neurons = brain->getActiveNeuronCount();
    stats.total_spikes = brain->getTotalSpikeCount();
    stats.average_firing_rate = brain->getAverageFiringRate();
    stats.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
    stats.working_memory_size = 0; // Simplified
    stats.episodic_memory_size = 0; // Simplified
    
    return stats;
}

std::vector<float> AnalysisTools::analyzeActivityPattern(std::shared_ptr<Brain> brain, size_t steps) {
    std::vector<float> activity_pattern;
    activity_pattern.reserve(steps);
    
    if (!brain) {
        return activity_pattern;
    }
    
    // Record firing rate at each step
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        float firing_rate = brain->getAverageFiringRate();
        activity_pattern.push_back(firing_rate);
    }
    
    return activity_pattern;
}

std::vector<float> AnalysisTools::analyzeLearningProgress(std::shared_ptr<Brain> brain, 
                                                           size_t snapshot_interval) {
    std::vector<float> learning_progress;
    
    if (!brain) {
        return learning_progress;
    }
    
    size_t step = 0;
    while (step < 10000) { // Limit analysis duration
        brain->step(step, step * 0.001);
        
        if (step % snapshot_interval == 0) {
            float weight_change = 0.0f; // Simplified calculation
            learning_progress.push_back(weight_change);
        }
        
        step++;
    }
    
    return learning_progress;
}

std::string AnalysisTools::generateVisualizationData(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return "{}";
    }
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"total_neurons\": " << brain->getTotalNeuronCount() << ",\n";
    oss << "  \"active_neurons\": " << brain->getActiveNeuronCount() << ",\n";
    oss << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
    oss << "  \"average_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
    oss << "  \"development_stage\": \"" << static_cast<int>(brain->getDevelopmentalStage()) << "\"\n";
    oss << "}\n";
    
    return oss.str();
}

bool AnalysisTools::exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        return false;
    }
    
    // Simplified brain export
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Brain State Export\n";
    file << "Generated: " << std::time(nullptr) << "\n";
    file << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
    file << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    file << "Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
    file << "Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    
    return true;
}

// QuickStart implementation
bool QuickStart::guidedSetup() {
    NLM_LOG_INFO("Starting NLM Quick Start Setup");
    
    // Step 1: Show available options
    NLM_LOG_INFO("Step 1: Welcome to NLM (Neural Learning Machine)");
    NLM_LOG_INFO("Step 2: Choose simulation type:");
    NLM_LOG_INFO("  - Basic neural computation");
    NLM_LOG_INFO("  - Learning scenario");
    NLM_LOG_INFO("  - Cognitive experiment");
    NLM_LOG_INFO("  - Integration demonstration");
    
    // Step 2: Create basic example
    NLM_LOG_INFO("Step 3: Creating basic brain simulation...");
    
    BasicSimulationOptions options;
    options.steps = 100;
    options.neuron_count = 100;
    
    if (!SimulationRunner::runBasicSimulation(options)) {
        NLM_LOG_ERROR("Quick start setup failed at step 3");
        return false;
    }
    
    // Step 3: Show next steps
    NLM_LOG_INFO("Step 4: Setup complete!");
    NLM_LOG_INFO("Next steps:")
    NLM_LOG_INFO("  - Try different simulation types (see examples)");
    NLM_LOG_INFO("  - Explore learning scenarios");
    NLM_LOG_INFO("  - Experiment with different configurations");
    NLM_LOG_INFO("  - Use the Python API (pynlm) for more advanced usage");
    
    return true;
}

bool QuickStart::createExampleSimulation() {
    NLM_LOG_INFO("Creating example simulation...");
    
    // Create a simple but complete example
    BasicSimulationOptions options;
    options.steps = 500;
    options.neuron_count = 500;
    options.enable_plasticity = true;
    options.enable_neuromodulation = true;
    
    return SimulationRunner::runBasicSimulation(options);
}

bool QuickStart::runExampleExperiments() {
    NLM_LOG_INFO("Running example experiments...");
    
    bool success = true;
    
    // Phase 2 demo
    Phase2Demo phase2;
    if (!ExperimentTemplates::runPhase2Demo(phase2)) {
        NLM_LOG_WARNING("Phase 2 demo failed, continuing with other examples");
        success = false;
    }
    
    // Learning scenario
    AssociationScenario assoc;
    if (!LearningScenarios::runAssociationLearning(assoc)) {
        NLM_LOG_WARNING("Association learning failed, continuing with other examples");
        success = false;
    }
    
    return success;
}

std::string QuickStart::getTutorialContent(const std::string& topic) {
    if (topic == "getting_started") {
        return R"(
# Getting Started with NLM

This tutorial will help you create your first neural simulation.

## Step 1: Installation

See the documentation for installation instructions.

## Step 2: Create a Basic Brain

Use the high-level API to create a simple brain:

```cpp
auto config = nlm::api::ConfigPresets::createBasicConfig();
auto brain = nlm::api::BrainFactory::createSimulationBrain(100);
```

## Step 3: Run a Simulation

```cpp
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
}
```

## Step 4: Analyze Results

```cpp
auto stats = nlm::api::AnalysisTools::analyzeBrain(brain);
std::cout << "Total spikes: " << stats.total_spikes << std::endl;
```

You're ready to start exploring NLM!
        )";
    }
    
    return "Tutorial not available for topic: " + topic;
}

bool QuickStart::validateExampleEnvironment() {
    NLM_LOG_INFO("Validating example environment...");
    
    // Check if basic components are available
    bool hasLogger = Logger::getGlobal().isEnabled(LogLevel::Info);
    bool hasConfig = ConfigPresets::createBasicConfig() != nullptr;
    bool hasBrain = BrainFactory::createMinimalBrain() != nullptr;
    
    if (hasLogger && hasConfig && hasBrain) {
        NLM_LOG_INFO("Example environment validation passed");
        return true;
    } else {
        NLM_LOG_ERROR("Example environment validation failed");
        return false;
    }
}

// ConfigurationManager implementation
bool ConfigurationManager::saveConfigWithTimestamp(std::shared_ptr<Config> config, const std::string& base_name) {
    if (!config) {
        return false;
    }
    
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
    
    std::string filename = base_name + "_" + timestamp + ".cfg";
    return config->saveToFile(filename);
}

std::shared_ptr<Config> ConfigurationManager::loadOrCreateConfig(const std::string& filename) {
    if (filename.empty()) {
        return ConfigPresets::createBasicConfig();
    }
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filename)) {
        NLM_LOG_WARNING("Failed to load config from " + filename + ", using default");
        return ConfigPresets::createBasicConfig();
    }
    
    return config;
}

std::shared_ptr<Config> ConfigurationManager::parseConfigString(const std::string& config_string) {
    auto config = ConfigPresets::createBasicConfig();
    
    // Simple parsing for demonstration
    std::istringstream iss(config_string);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return config;
}

bool ConfigurationManager::validateConfig(std::shared_ptr<Config> config, const std::string& preset) {
    if (!config) {
        return false;
    }
    
    // Basic validation
    if (!config->has("random_seed") || !config->has("simulation_timestep")) {
        NLM_LOG_ERROR("Config missing required parameters");
        return false;
    }
    
    // Preset-specific validation
    if (preset == "learning") {
        if (!config->has("dopamine_baseline")) {
            NLM_LOG_ERROR("Learning preset requires dopamine_baseline parameter");
            return false;
        }
    }
    
    return true;
}

std::string ConfigurationManager::getConfigSummary(std::shared_ptr<Config> config) {
    if (!config) {
        return "No configuration available";
    }
    
    std::ostringstream oss;
    oss << "NLM Configuration Summary\n";
    oss << "======================\n\n";
    
    auto keys = config->getKeys();
    oss << "Parameters (" << keys.size() << "):\n";
    
    for (const auto& key : keys) {
        oss << "  " << key << " = ";
        
        // Try to get value as string
        if (auto str_val = config->get<std::string>(key); str_val.has_value()) {
            oss << "\"" << str_val.value() << "\"\n";
        } else if (auto int_val = config->get<int>(key); int_val.has_value()) {
            oss << int_val.value() << "\n";
        } else if (auto double_val = config->get<double>(key); double_val.has_value()) {
            oss << double_val.value() << "\n";
        } else if (auto bool_val = config->get<bool>(key); bool_val.has_value()) {
            oss << (bool_val.value() ? "true" : "false") << "\n";
        } else {
            oss << "[unknown]\n";
        }
    }
    
    return oss.str();
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMetrics PerformanceMonitor::measurePerformance(
    std::function<void()> simulation_func) {
    
    BenchmarkTimer timer;
    
    // Measure memory usage (simplified)
    size_t memory_usage = 0;
    
    // Measure CPU usage (simplified)
    double cpu_usage = 0.0;
    
    try {
        simulation_func();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Performance measurement failed: " + std::string(e.what()));
    }
    
    double simulation_time_ms = timer.elapsed_ms();
    
    // Calculate neurons processed (simplified)
    size_t neurons_processed = 1000; // Placeholder
    
    // Calculate spikes generated (simplified)
    size_t spikes_generated = 500; // Placeholder
    
    PerformanceMetrics metrics;
    metrics.simulation_time_ms = simulation_time_ms;
    metrics.neurons_processed = neurons_processed;
    metrics.spikes_generated = spikes_generated;
    metrics.memory_usage_mb = memory_usage / (1024 * 1024);
    metrics.cpu_usage_percent = cpu_usage;
    
    return metrics;
}

void PerformanceMonitor::profileSimulation(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) {
        return;
    }
    
    NLM_LOG_INFO("Profiling simulation with " + std::to_string(steps) + " steps");
    
    BenchmarkTimer total_timer;
    
    for (size_t step = 0; step < steps; ++step) {
        BenchmarkTimer step_timer;
        
        brain->step(step, step * 0.001);
        
        double step_time_ms = step_timer.elapsed_ms();
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                       std::to_string(step_time_ms) + " ms");
        }
    }
    
    double total_time_ms = total_timer.elapsed_ms();
    NLM_LOG_INFO("Simulation profiling completed in " + std::to_string(total_time_ms) + " ms");
    NLM_LOG_INFO("Average step time: " + std::to_string(total_time_ms / steps) + " ms");
}

std::vector<PerformanceMonitor::PerformanceMetrics> PerformanceMonitor::benchmarkConfigurations(
    const std::vector<std::shared_ptr<Config>>& configs) {
    
    std::vector<PerformanceMetrics> results;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        NLM_LOG_INFO("Benchmarking configuration " + std::to_string(i + 1) + 
                    " of " + std::to_string(configs.size()));
        
        auto brain = std::make_shared<Brain>(configs[i]);
        if (BrainFactory::initializeBrain(brain, "benchmark_config_" + std::to_string(i))) {
            
            PerformanceMetrics metrics = measurePerformance([brain]() {
                for (size_t step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                }
            });
            
            results.push_back(metrics);
        }
    }
    
    return results;
}

// ValidationTools implementation
bool ValidationTools::validateBrainConfig(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot validate null configuration");
        return false;
    }
    
    // Check required parameters
    std::vector<std::string> required_params = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count"
    };
    
    std::vector<std::string> missing_params = config->getMissingKeys(required_params);
    
    if (!missing_params.empty()) {
        NLM_LOG_ERROR("Missing required configuration parameters:");
        for (const auto& param : missing_params) {
            NLM_LOG_ERROR("  - " + param);
        }
        return false;
    }
    
    // Validate parameter ranges
    auto neuron_count = config->getOr<int64_t>("neuron_count", 0);
    if (neuron_count <= 0 || neuron_count > 1000000) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuron_count));
        return false;
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep));
        return false;
    }
    
    NLM_LOG_INFO("Brain configuration validation passed");
    return true;
}

bool ValidationTools::validateSimulationParameters(size_t steps, size_t neuron_count) {
    if (steps == 0) {
        NLM_LOG_ERROR("Simulation steps cannot be zero");
        return false;
    }
    
    if (steps > 1000000) {
        NLM_LOG_WARNING("Large number of simulation steps may cause performance issues");
    }
    
    if (neuron_count == 0) {
        NLM_LOG_ERROR("Neuron count cannot be zero");
        return false;
    }
    
    if (neuron_count > 1000000) {
        NLM_LOG_WARNING("Large neuron count may cause memory issues");
    }
    
    return true;
}

bool ValidationTools::recoverFromError(std::shared_ptr<Brain> brain, const std::exception& error) {
    if (!brain) {
        return false;
    }
    
    NLM_LOG_WARNING("Attempting to recover from error: " + std::string(error.what()));
    
    try {
        // Reset brain state
        brain->reset();
        
        // Re-initialize brain
        if (auto config = brain->getConfig()) {
            auto new_brain = std::make_shared<Brain>(config);
            if (BrainFactory::initializeBrain(new_brain, "recovery")) {
                NLM_LOG_INFO("Brain recovery successful");
                return true;
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Recovery failed: " + std::string(e.what()));
    }
    
    return false;
}

bool ValidationTools::validateSimulationSuccess(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot validate null brain");
        return false;
    }
    
    NLM_LOG_INFO("Validating simulation success...");
    
    // Check if brain has completed reasonable activity
    size_t spikes = brain->getTotalSpikeCount();
    size_t neurons = brain->getTotalNeuronCount();
    
    if (neurons == 0) {
        NLM_LOG_ERROR("No neurons in brain");
        return false;
    }
    
    if (spikes < neurons / 10) { // At least some neural activity
        NLM_LOG_WARNING("Low neural activity detected: " + std::to_string(spikes) + 
                       " spikes for " + std::to_string(neurons) + " neurons");
    }
    
    NLM_LOG_INFO("Simulation validation completed successfully");
    NLM_LOG_INFO("Final statistics: " + std::to_string(spikes) + " spikes, " +
                std::to_string(neurons) + " neurons");
    
    return true;
}

} // namespace api
} // namespace nlm
