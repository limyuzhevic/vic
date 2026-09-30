// NLM Configuration Example: Creating Different Brain Configurations

#include "nlm/config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>

// Function to create research brain configuration
nlm::Config createResearchBrainConfig() {
    nlm::Config config;
    
    std::cout << "Creating Research Brain Configuration..." << std::endl;
    
    // Core brain parameters for research
    config.set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::User);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::User);
    
    // Memory systems for comprehensive research
    config.set("working_memory_capacity", 1000, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 50000, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 500000, nlm::ConfigSource::User);
    config.set("associative_memory_capacity", 200000, nlm::ConfigSource::User);
    config.set("procedural_memory_capacity", 10000, nlm::ConfigSource::User);
    
    // Plasticity systems for learning studies
    config.set("stdp_learning_rate", 0.02f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.01f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.003f, nlm::ConfigSource::User);
    config.set("plasticity_eligibility_decay", 0.99f, nlm::ConfigSource::User);
    
    // Development system for maturation studies
    config.set("development_enabled", true, nlm::ConfigSource::User);
    config.set("developmental_stage_count", 8, nlm::ConfigSource::User);
    config.set("developmental_stage_duration", 2000, nlm::ConfigSource::User);
    
    // Prediction system for anticipation studies
    config.set("prediction_system_enabled", true, nlm::ConfigSource::User);
    config.set("prediction_horizon", 50, nlm::ConfigSource::User);
    config.set("prediction_error_threshold", 0.1f, nlm::ConfigSource::User);
    
    // Neuromodulation for motivation studies
    config.set("dopamine_enabled", true, nlm::ConfigSource::User);
    config.set("dopamine_learning_rate", 0.3f, nlm::ConfigSource::User);
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.2f, nlm::ConfigSource::User);
    config.set("novelty_enabled", true, nlm::ConfigSource::User);
    config.set("novelty_sensitivity", 0.5f, nlm::ConfigSource::User);
    
    // Cognitive systems for comprehensive study
    config.set("planning_system_enabled", true, nlm::ConfigSource::User);
    config.set("planning_horizon", 20, nlm::ConfigSource::User);
    config.set("attention_enabled", true, nlm::ConfigSource::User);
    config.set("attention_window_size", 20, nlm::ConfigSource::User);
    config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
    
    // Recording for data collection
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 20, nlm::ConfigSource::User);
    config.set("enable_logging", true, nlm::ConfigSource::User);
    config.set("log_level", "DEBUG", nlm::ConfigSource::User);
    
    std::cout << "  - Neuron count: " << config.get<int64_t>("neuron_count") << std::endl;
    std::cout << "  - Region count: " << config.get<int64_t>("region_count") << std::endl;
    std::cout << "  - Connection probability: " << config.get<float>("connection_probability") << std::endl;
    std::cout << "  - Working memory capacity: " << config.get<size_t>("working_memory_capacity") << std::endl;
    std::cout << "  - Episodic memory capacity: " << config.get<size_t>("episodic_memory_capacity") << std::endl;
    std::cout << "  - Semantic memory capacity: " << config.get<size_t>("semantic_memory_capacity") << std::endl;
    
    return config;
}

// Function to create real-time robot configuration
nlm::Config createRealTimeRobotConfig() {
    nlm::Config config;
    
    std::cout << "\nCreating Real-Time Robot Configuration..." << std::endl;
    
    // Fast, responsive brain for real-time control
    config.set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::User);
    config.set("connection_probability", 0.3f, nlm::ConfigSource::User);  // More connections
    
    // Lightweight memory for speed
    config.set("working_memory_capacity", 50, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 200, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 1000, nlm::ConfigSource::User);
    
    // Fast learning for quick adaptation
    config.set("stdp_learning_rate", 0.05f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.02f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.005f, nlm::ConfigSource::User);
    
    // Strong curiosity for exploration
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.3f, nlm::ConfigSource::User);
    config.set("curiosity_decay_rate", 0.9f, nlm::ConfigSource::User);
    
    // Predict for quick action
    config.set("prediction_system_enabled", true, nlm::ConfigSource::User);
    config.set("prediction_horizon", 5, nlm::ConfigSource::User);
    config.set("prediction_error_threshold", 0.3f, nlm::ConfigSource::User);
    
    // Focus on quick decisions
    config.set("planning_horizon", 3, nlm::ConfigSource::User);
    config.set("action_selection_method", "epsilon_greedy", nlm::ConfigSource::User);
    config.set("exploration_exploitation_ratio", 0.4f, nlm::ConfigSource::User);
    
    // Minimal development for speed
    config.set("development_enabled", false, nlm::ConfigSource::User);
    
    // Real-time recording
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 10, nlm::ConfigSource::User);
    
    // Performance monitoring
    config.set("enable_performance_monitoring", true, nlm::ConfigSource::User);
    config.set("performance_threshold_ms", 10, nlm::ConfigSource::User);
    
    std::cout << "  - Neuron count: " << config.get<int64_t>("neuron_count") << std::endl;
    std::cout << "  - Region count: " << config.get<int64_t>("region_count") << std::endl;
    std::cout << "  - Connection probability: " << config.get<float>("connection_probability") << std::endl;
    std::cout << "  - Working memory capacity: " << config.get<size_t>("working_memory_capacity") << std::endl;
    std::cout << "  - Development enabled: " << config.get<bool>("development_enabled") << std::endl;
    
    return config;
}

// Function to create educational agent configuration
nlm::Config createEducationalAgentConfig() {
    nlm::Config config;
    
    std::cout << "\nCreating Educational Agent Configuration..." << std::endl;
    
    // Medium-sized brain for learning
    config.set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::User);
    config.set("connection_probability", 0.12f, nlm::ConfigSource::User);
    
    // Rich semantic memory for knowledge
    config.set("working_memory_capacity", 300, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 2000, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 50000, nlm::ConfigSource::User);
    config.set("procedural_memory_capacity", 500, nlm::ConfigSource::User);
    
    // Gentle learning for educational use
    config.set("stdp_learning_rate", 0.01f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.005f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.001f, nlm::ConfigSource::User);
    
    // Strong development system
    config.set("development_enabled", true, nlm::ConfigSource::User);
    config.set("developmental_stage_count", 6, nlm::ConfigSource::User);
    config.set("developmental_stage_duration", 1000, nlm::ConfigSource::User);
    config.set("developmental_plasticity_modifier", 2.0f, nlm::ConfigSource::User);
    
    // Reward-based learning
    config.set("dopamine_enabled", true, nlm::ConfigSource::User);
    config.set("dopamine_learning_rate", 0.15f, nlm::ConfigSource::User);
    config.set("reward_sensitivity", 1.5f, nlm::ConfigSource::User);
    config.set("punishment_sensitivity", 0.8f, nlm::ConfigSource::User);
    
    // Focused attention and planning
    config.set("attention_enabled", true, nlm::ConfigSource::User);
    config.set("attention_window_size", 10, nlm::ConfigSource::User);
    config.set("planning_system_enabled", true, nlm::ConfigSource::User);
    config.set("planning_horizon", 8, nlm::ConfigSource::User);
    
    // Concept formation for learning
    config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
    config.set("concept_threshold", 0.6f, nlm::ConfigSource::User);
    
    // Educational recording
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 5, nlm::ConfigSource::User);
    config.set("enable_logging", true, nlm::ConfigSource::User);
    config.set("log_level", "INFO", nlm::ConfigSource::User);
    
    // Learning-specific features
    config.set("enable_feedback_learning", true, nlm::ConfigSource::User);
    config.set("feedback_delay", 10, nlm::ConfigSource::User);
    config.set("feedback_strength", 0.5f, nlm::ConfigSource::User);
    
    std::cout << "  - Neuron count: " << config.get<int64_t>("neuron_count") << std::endl;
    std::cout << "  - Region count: " << config.get<int64_t>("region_count") << std::endl;
    std::cout << "  - Development enabled: " << config.get<bool>("development_enabled") << std::endl;
    std::cout << "  - Concept formation enabled: " << config.get<bool>("concept_formation_enabled") << std::endl;
    
    return config;
}

// Function to create embedded system configuration
nlm::Config createEmbeddedSystemConfig() {
    nlm::Config config;
    
    std::cout << "\nCreating Embedded System Configuration..." << std::endl;
    
    // Configure for embedded systems with limited resources
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::User);
    config.set("connection_probability", 0.2f, nlm::ConfigSource::User);
    
    // Minimal memory usage
    config.set("working_memory_capacity", 20, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 100, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 500, nlm::ConfigSource::User);
    
    // Fast learning for adaptation
    config.set("stdp_learning_rate", 0.08f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.04f, nlm::ConfigSource::User);
    
    // Basic neuromodulation
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.5f, nlm::ConfigSource::User);
    
    // Minimal development for resource efficiency
    config.set("development_enabled", false, nlm::ConfigSource::User);
    
    // Basic recording
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 5, nlm::ConfigSource::User);
    config.set("log_level", "WARN", nlm::ConfigSource::User);
    
    // Memory optimization
    config.set("enable_memory_optimization", true, nlm::ConfigSource::User);
    config.set("memory_pool_size", 1024, nlm::ConfigSource::User);  // 1KB pool
    
    // CPU optimization
    config.set("enable_cpu_optimization", true, nlm::ConfigSource::User);
    config.set("optimize_for_cache", true, nlm::ConfigSource::User);
    
    std::cout << "  - Neuron count: " << config.get<int64_t>("neuron_count") << std::endl;
    std::cout << "  - Region count: " << config.get<int64_t>("region_count") << std::endl;
    std::cout << "  - Development enabled: " << config.get<bool>("development_enabled") << std::endl;
    std::cout << "  - Memory optimization enabled: " << config.get<bool>("enable_memory_optimization") << std::endl;
    
    return config;
}

// Function to create configuration file
void createConfigFile(const std::string& filename, const nlm::Config& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create configuration file: " << filename << std::endl;
        return;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration Example" << std::endl;
    file << std::endl;
    
    // Write configuration settings
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        if (std::is_same<nlm::Config::mapped_type, int64_t>::value) {
            int64_t value = config.get<int64_t>(key);
            file << key << " = " << value << std::endl;
        } else if (std::is_same<nlm::Config::mapped_type, float>::value) {
            float value = config.get<float>(key);
            file << key << " = " << value << std::endl;
        } else if (std::is_same<nlm::Config::mapped_type, bool>::value) {
            bool value = config.get<bool>(key);
            file << key << " = " << (value ? "true" : "false") << std::endl;
        } else if (std::is_same<nlm::Config::mapped_type, size_t>::value) {
            size_t value = config.get<size_t>(key);
            file << key << " = " << value << std::endl;
        } else if (std::is_same<nlm::Config::mapped_type, std::string>::value) {
            std::string value = config.get<std::string>(key);
            file << key << " = \"" << value << "\"" << std::endl;
        }
    }
    
    file.close();
    std::cout << "Configuration saved to: " << filename << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration Example ===" << std::endl;
    
    // Create different configuration types
    nlm::Config researchConfig = createResearchBrainConfig();
    nlm::Config robotConfig = createRealTimeRobotConfig();
    nlm::Config educationalConfig = createEducationalAgentConfig();
    nlm::Config embeddedConfig = createEmbeddedSystemConfig();
    
    // Save configurations to files
    createConfigFile("research_config.cfg", researchConfig);
    createConfigFile("robot_config.cfg", robotConfig);
    createConfigFile("educational_config.cfg", educationalConfig);
    createConfigFile("embedded_config.cfg", embeddedConfig);
    
    // Show configuration comparison
    std::cout << "\n=== Configuration Comparison ===" << std::endl;
    std::cout << "1. Research Brain:" << std::endl;
    std::cout << "   - Neuron count: " << researchConfig.get<int64_t>("neuron_count") << std::endl;
    std::cout << "   - Region count: " << researchConfig.get<int64_t>("region_count") << std::endl;
    std::cout << "   - Memory capacity: " << researchConfig.get<size_t>("semantic_memory_capacity") << std::endl;
    
    std::cout << "\n2. Real-Time Robot:" << std::endl;
    std::cout << "   - Neuron count: " << robotConfig.get<int64_t>("neuron_count") << std::endl;
    std::cout << "   - Development enabled: " << robotConfig.get<bool>("development_enabled") << std::endl;
    std::cout << "   - Curiosity threshold: " << robotConfig.get<float>("curiosity_threshold") << std::endl;
    
    std::cout << "\n3. Educational Agent:" << std::endl;
    std::cout << "   - Neuron count: " << educationalConfig.get<int64_t>("neuron_count") << std::endl;
    std::cout << "   - Development stages: " << educationalConfig.get<int64_t>("developmental_stage_count") << std::endl;
    std::cout << "   - Concept formation: " << educationalConfig.get<bool>("concept_formation_enabled") << std::endl;
    
    std::cout << "\n4. Embedded System:" << std::endl;
    std::cout << "   - Neuron count: " << embeddedConfig.get<int64_t>("neuron_count") << std::endl;
    std::cout << "   - Memory optimization: " << embeddedConfig.get<bool>("enable_memory_optimization") << std::endl;
    std::cout << "   - CPU optimization: " << embeddedConfig.get<bool>("enable_cpu_optimization") << std::endl;
    
    std::cout << "\n=== Configuration Example Completed ===" << std::endl;
    
    return 0;
}
