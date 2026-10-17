# High-level API for NLM Common Neural Simulation Tasks
// Comprehensive high-level API providing simplified interfaces for common neural simulation tasks
// This API abstracts away the complexity of the low-level NLM components and provides intuitive interfaces

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {
namespace api {

// High-level configuration presets
class ConfigPresets {
public:
    // Create default configuration for basic neural computation
    static std::shared_ptr<Config> createBasicConfig();
    
    // Create configuration optimized for learning scenarios
    static std::shared_ptr<Config> createLearningConfig();
    
    // Create configuration optimized for cognitive experiments
    static std::shared_ptr<Config> createCognitiveConfig();
    
    // Create configuration for real-time applications
    static std::shared_ptr<Config> createRealTimeConfig();
    
    // Create configuration for research experiments
    static std::shared_ptr<Config> createResearchConfig();
    
    // Create configuration with custom parameters
    static std::shared_ptr<Config> createCustomConfig(
        const std::map<std::string, std::string>& parameters);
    
    // Get available preset names
    static std::vector<std::string> getPresetNames();
};

// Simple brain creation utilities
class BrainFactory {
public:
    // Create a brain with minimal configuration
    static std::shared_ptr<Brain> createMinimalBrain();
    
    // Create a brain with typical parameters for simulation
    static std::shared_ptr<Brain> createSimulationBrain(size_t neuron_count = 1000);
    
    // Create a brain configured for specific learning task
    static std::shared_ptr<Brain> createLearningBrain(
        const std::string& task_type = "association");
    
    // Create a brain with specific developmental parameters
    static std::shared_ptr<Brain> createDevelopmentalBrain(
        float synaptogenesis_rate = 0.001f,
        float pruning_rate = 0.0001f);
    
    // Initialize brain with error handling and logging
    static bool initializeBrain(std::shared_ptr<Brain> brain,
                               const std::string& operation_name = "");
    
    // Create brain and run basic connectivity test
    static std::shared_ptr<Brain> createAndTestBrain(size_t neuron_count = 500);
};

// Simulation runner for common scenarios
class SimulationRunner {
public:
    // Basic brain simulation runner
    struct BasicSimulationOptions {
        size_t steps = 1000;
        size_t neuron_count = 500;
        bool enable_plasticity = true;
        bool enable_neuromodulation = true;
        std::string config_preset = "basic";
    };
    
    static bool runBasicSimulation(const BasicSimulationOptions& options = {});
    
    // Learning scenario simulation
    struct LearningSimulationOptions {
        std::string scenario = "association"; // association, sequence, navigation
        size_t steps = 2000;
        float learning_rate = 0.01f;
        size_t neuron_count = 200;
        bool enable_curiosity = true;
        bool enable_reward = true;
    };
    
    static bool runLearningSimulation(const LearningSimulationOptions& options = {});
    
    // Cognitive experiment simulation
    struct CognitiveSimulationOptions {
        std::string experiment_type = "attention"; // attention, planning, concept
        size_t steps = 3000;
        size_t brain_regions = 2;
        float plasticity_strength = 0.05f;
    };
    
    static bool runCognitiveSimulation(const CognitiveSimulationOptions& options = {});
    
    // Multi-agent environment simulation
    struct AgentSimulationOptions {
        size_t agent_count = 1;
        size_t world_size = 20;
        size_t steps = 1000;
        bool enable_social_learning = false;
    };
    
    static bool runAgentSimulation(const AgentSimulationOptions& options = {});
};

// Learning scenario templates
class LearningScenarios {
public:
    // Hebbian association learning
    struct AssociationScenario {
        size_t neurons_per_pattern = 50;
        size_t patterns = 5;
        float pattern_strength = 1.0f;
        float noise_level = 0.1f;
    };
    
    static bool runAssociationLearning(const AssociationScenario& scenario = {});
    
    // Sequence learning (temporal patterns)
    struct SequenceScenario {
        size_t sequence_length = 10;
        size_t sequences = 20;
        float temporal_gap = 0.1f;
        float sequence_strength = 0.8f;
    };
    
    static bool runSequenceLearning(const SequenceScenario& scenario = {});
    
    // Navigation learning
    struct NavigationScenario {
        size_t maze_width = 10;
        size_t maze_height = 10;
        size_t steps = 500;
        float reward_step = 10.0f;
        float penalty_step = -1.0f;
        float exploration_rate = 0.1f;
    };
    
    static bool runNavigationLearning(const NavigationScenario& scenario = {});
    
    // Concept formation learning
    struct ConceptScenario {
        size_t stimulus_dimensions = 5;
        size_t clusters = 3;
        float cluster_separation = 2.0f;
        float noise_variance = 0.1f;
    };
    
    static bool runConceptFormation(const ConceptScenario& scenario = {});
};

// Experiment templates
class ExperimentTemplates {
public:
    // Phase 2: Real neural computation demonstration
    struct Phase2Demo {
        size_t neurons = 500;
        size_t steps = 1000;
        bool show_plasticity = true;
        bool show_stdp = true;
    };
    
    static bool runPhase2Demo(const Phase2Demo& demo = {});
    
    // Phase 3: World interaction demonstration
    struct Phase3Demo {
        size_t world_width = 20;
        size_t world_height = 20;
        size_t steps = 500;
        bool enable_sensory = true;
        bool enable_motor = true;
    };
    
    static bool runPhase3Demo(const Phase3Demo& demo = {});
    
    // Phase 4: Cognitive experiments demonstration
    struct Phase4Demo {
        std::string cognitive_task = "attention"; // attention, planning, memory
        size_t steps = 1000;
        bool enable_memory = true;
        bool enable_planning = true;
    };
    
    static bool runPhase4Demo(const Phase4Demo& demo = {});
    
    // Integration demonstration (Phase 6)
    struct IntegrationDemo {
        size_t neurons = 200;
        size_t regions = 2;
        size_t steps = 2000;
        bool enable_all_systems = true;
        bool show_progress = true;
    };
    
    static bool runIntegrationDemo(const IntegrationDemo& demo = {});
};

// Visualization and analysis utilities
class AnalysisTools {
public:
    // Brain state analysis
    struct BrainStats {
        size_t total_neurons;
        size_t active_neurons;
        size_t total_spikes;
        float average_firing_rate;
        float excitation_inhibition_ratio;
        size_t working_memory_size;
        size_t episodic_memory_size;
    };
    
    static BrainStats analyzeBrain(std::shared_ptr<Brain> brain);
    
    // Activity patterns analysis
    static std::vector<float> analyzeActivityPattern(std::shared_ptr<Brain> brain, size_t steps);
    
    // Learning progress analysis
    static std::vector<float> analyzeLearningProgress(std::shared_ptr<Brain> brain, 
                                                       size_t snapshot_interval = 100);
    
    // Generate simple visualization data
    static std::string generateVisualizationData(std::shared_ptr<Brain> brain);
    
    // Export brain state for external analysis
    static bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
};

// Quick start and tutorial utilities
class QuickStart {
public:
    // Step-by-step guided setup
    static bool guidedSetup();
    
    // Create example simulation with minimal user input
    static bool createExampleSimulation();
    
    // Run common experiment examples
    static bool runExampleExperiments();
    
    // Get tutorial content
    static std::string getTutorialContent(const std::string& topic);
    
    // Check if example files exist and can be run
    static bool validateExampleEnvironment();
};

// Configuration management
class ConfigurationManager {
public:
    // Save configuration to file with timestamp
    static bool saveConfigWithTimestamp(std::shared_ptr<Config> config, const std::string& base_name = "nlm_config");
    
    // Load configuration from file or use preset
    static std::shared_ptr<Config> loadOrCreateConfig(const std::string& filename = "");
    
    // Create configuration from string representation
    static std::shared_ptr<Config> parseConfigString(const std::string& config_string);
    
    // Validate configuration against schema
    static bool validateConfig(std::shared_ptr<Config> config, const std::string& preset = "");
    
    // Get configuration summary as human-readable string
    static std::string getConfigSummary(std::shared_ptr<Config> config);
};

// Performance monitoring and optimization
class PerformanceMonitor {
public:
    // Measure simulation performance
    struct PerformanceMetrics {
        double simulation_time_ms;
        size_t neurons_processed;
        size_t spikes_generated;
        double memory_usage_mb;
        double cpu_usage_percent;
    };
    
    static PerformanceMetrics measurePerformance(std::function<void()> simulation_func);
    
    // Profile simulation with detailed timing
    static void profileSimulation(std::shared_ptr<Brain> brain, size_t steps);
    
    // Compare performance across different configurations
    static std::vector<PerformanceMetrics> benchmarkConfigurations(
        const std::vector<std::shared_ptr<Config>>& configs);
};

// Error recovery and validation
class ValidationTools {
public:
    // Validate brain configuration before initialization
    static bool validateBrainConfig(std::shared_ptr<Config> config);
    
    // Check if simulation parameters are within valid ranges
    static bool validateSimulationParameters(size_t steps, size_t neuron_count);
    
    // Recovery from simulation errors
    static bool recoverFromError(std::shared_ptr<Brain> brain, const std::exception& error);
    
    // Validate simulation completion
    static bool validateSimulationSuccess(std::shared_ptr<Brain> brain);
};

} // namespace api
} // namespace nlm
