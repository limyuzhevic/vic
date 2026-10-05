// Configuration Presets for NLM (熙然) Neural Learning Machine
// Common use case configurations for different user needs

namespace nlm {
    namespace ConfigPresets {
        
        // Default configuration for beginners and quick testing
        inline std::shared_ptr<Config> createBeginnerConfig() {
            auto config = std::make_shared<Config>();
            
            // Small brain for quick testing and learning
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.1);
            
            // Basic neuron parameters
            config->set("brain.v_thresh", -50.0);
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 20.0);
            config->set("brain.tau_ref", 2.0);
            
            // Basic learning (simplified for learning)
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.0005);
            config->set("plasticity.structural.enable", false); // No structural changes
            
            // Simple neuromodulation
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", false); // No novelty for simplicity
            
            // Basic development
            config->set("development.enable", true);
            config->set("development.critical_period", 50);
            config->set("development.maturation_rate", 0.01);
            
            // Memory systems
            config->set("memory.working_capacity", 20);
            config->set("memory.episodic_capacity", 50);
            
            return config;
        }
        
        // Configuration for research and experimentation
        inline std::shared_ptr<Config> createResearchConfig() {
            auto config = std::make_shared<Config>();
            
            // Medium-sized brain for meaningful experiments
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.15);
            
            // Standard neuron parameters
            config->set("brain.v_thresh", -50.0);
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 20.0);
            config->set("brain.tau_ref", 2.0);
            
            // Full learning capabilities
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.005);
            config->set("plasticity.stdp.tau_plus", 20.0);
            config->set("plasticity.stdp.tau_minus", 20.0);
            
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.002);
            
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.0005);
            config->set("plasticity.structural.pruning_rate", 0.00005);
            
            // Advanced neuromodulation
            config->set("neuromod.dopamine.scale", 1.5);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction.error_weight", 0.3);
            
            // Complete development system
            config->set("development.enable", true);
            config->set("development.critical_period", 100);
            config->set("development.maturation_rate", 0.02);
            config->set("development.pruning_rate", 0.01);
            
            // Comprehensive memory systems
            config->set("memory.working_capacity", 100);
            config->set("memory.episodic_capacity", 1000);
            config->set("memory.associative_strength", 0.7);
            
            return config;
        }
        
        // Configuration for performance and scalability
        inline std::shared_ptr<Config> createPerformanceConfig() {
            auto config = std::make_shared<Config>();
            
            // Large brain for performance testing
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.05); // Lower density for performance
            
            // Optimized neuron parameters
            config->set("brain.v_thresh", -45.0); // Higher threshold
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 15.0); // Faster dynamics
            config->set("brain.tau_ref", 1.0); // Shorter refractory period
            
            // Focused learning (performance-oriented)
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.0005);
            config->set("plasticity.structural.enable", false); // No structural changes
            
            // Simple neuromodulation for performance
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", false); // No curiosity for predictability
            config->set("neuromod.novelty.enable", false);
            
            // Minimal development
            config->set("development.enable", false); // No development
            
            // Basic memory
            config->set("memory.working_capacity", 50);
            config->set("memory.episodic_capacity", 200);
            
            return config;
        }
        
        // Configuration for learning and education
        inline std::shared_ptr<Config> createEducationalConfig() {
            auto config = std::make_shared<Config>();
            
            // Medium-sized brain for educational demonstrations
            config->set("brain.neuron_count", 300);
            config->set("brain.synapse_density", 0.1);
            
            // Educational neuron parameters
            config->set("brain.v_thresh", -55.0); // Easy to reach threshold
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 25.0); // Slower dynamics for observation
            config->set("brain.tau_ref", 3.0); // Longer refractory period
            
            // Strong learning for demonstration
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01); // Strong learning
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.005);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.001); // Strong growth
            
            // Educational neuromodulation
            config->set("neuromod.dopamine.scale", 2.0); // Strong reward signals
            config->set("neuromod.curiosity.enable", true); // Strong exploration
            config->set("neuromod.novelty.enable", true); // Novelty detection
            
            // Clear development stages
            config->set("development.enable", true);
            config->set("development.critical_period", 75);
            config->set("development.maturation_rate", 0.03); // Fast maturation
            
            // Educational memory
            config->set("memory.working_capacity", 30);
            config->set("memory.episodic_capacity", 100);
            
            return config;
        }
        
        // Configuration for reinforcement learning
        inline std::shared_ptr<Config> createRLConfig() {
            auto config = std::make_shared<Config>();
            
            // RL-oriented configuration
            config->set("brain.neuron_count", 400);
            config->set("brain.synapse_density", 0.12);
            
            // RL-friendly neuron parameters
            config->set("brain.v_thresh", -48.0);
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 18.0);
            config->set("brain.tau_ref", 2.0);
            
            // RL learning with reward modulation focus
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.003);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.001);
            
            // Strong reward neuromodulation
            config->set("neuromod.dopamine.scale", 2.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction.error_weight", 0.5); // Strong prediction errors
            
            // Development for adaptation
            config->set("development.enable", true);
            config->set("development.critical_period", 60);
            
            // Episodic memory for RL
            config->set("memory.working_capacity", 80);
            config->set("memory.episodic_capacity", 500);
            
            return config;
        }
        
        // Configuration for cognitive science research
        inline std::shared_ptr<Config> createCognitiveConfig() {
            auto config = std::make_shared<Config>();
            
            // Cognitive science oriented configuration
            config->set("brain.neuron_count", 600);
            config->set("brain.synapse_density", 0.08); // Sparse connectivity
            
            // Cognitive neuron parameters
            config->set("brain.v_thresh", -52.0);
            config->set("brain.v_rest", -70.0);
            config->set("brain.v_reset", -75.0);
            config->set("brain.tau_mem", 22.0); // Slower integration
            config->set("brain.tau_ref", 2.5); // Moderate refractory period
            
            // Complex learning for cognitive modeling
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.002);
            config->set("plasticity.stdp.tau_plus", 30.0); // Longer STDP windows
            config->set("plasticity.stdp.tau_minus", 30.0);
            
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.001);
            
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.0002);
            config->set("plasticity.structural.pruning_rate", 0.00002);
            
            // Advanced neuromodulation for cognitive control
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction.error_weight", 0.8); // Very strong prediction errors
            
            // Cognitive development
            config->set("development.enable", true);
            config->set("development.critical_period", 120); // Longer critical period
            config->set("development.maturation_rate", 0.015); // Slower maturation
            
            // Working memory focus
            config->set("memory.working_capacity", 150); // Large working memory
            config->set("memory.episodic_capacity", 800); // Large episodic memory
            config->set("memory.associative_strength", 0.6); // Strong associations
            
            return config;
        }
        
        // Quick helper functions for common presets
        inline std::shared_ptr<Config> getConfig(const std::string& preset) {
            std::transform(preset.begin(), preset.end(), preset.begin(), ::tolower);
            
            if (preset == "beginner" || preset == "quick" || preset == "test") {
                return createBeginnerConfig();
            } else if (preset == "research" || preset == "experiment") {
                return createResearchConfig();
            } else if (preset == "performance" || preset == "production") {
                return createPerformanceConfig();
            } else if (preset == "educational" || preset == "tutorial") {
                return createEducationalConfig();
            } else if (preset == "rl" || preset == "reinforcement") {
                return createRLConfig();
            } else if (preset == "cognitive" || preset == "cognition") {
                return createCognitiveConfig();
            } else {
                // Default to beginner
                return createBeginnerConfig();
            }
        }
        
        // List available presets
        inline std::vector<std::string> getAvailablePresets() {
            return {
                "beginner", "quick", "test",           // Quick testing and learning
                "research", "experiment",               // Scientific research
                "performance", "production",           // High performance
                "educational", "tutorial",             // Teaching and learning
                "rl", "reinforcement",                  // Reinforcement learning
                "cognitive", "cognition"              // Cognitive modeling
            };
        }
        
        // Get preset description
        inline std::string getPresetDescription(const std::string& preset) {
            std::transform(preset.begin(), preset.end(), preset.begin(), ::tolower);
            
            if (preset == "beginner" || preset == "quick" || preset == "test") {
                return "Quick testing and learning setup with small brain and basic learning.";
            } else if (preset == "research" || preset == "experiment") {
                return "Research-oriented configuration with medium brain and full learning capabilities.";
            } else if (preset == "performance" || preset == "production") {
                return "High-performance configuration with large brain optimized for speed.";
            } else if (preset == "educational" || preset == "tutorial") {
                return "Educational configuration with strong learning signals for demonstration.";
            } else if (preset == "rl" || preset == "reinforcement") {
                return "Reinforcement learning configuration optimized for reward-based learning.";
            } else if (preset == "cognitive" || preset == "cognition") {
                return "Cognitive science configuration with focus on working memory and complex learning.";
            } else {
                return "Beginner configuration (default).";
            }
        }
    }
}

// Python bindings for configuration presets
void addConfigPresets(py::module& m) {
    py::class_<nlm::ConfigPresets> configPresetsClass(m, "ConfigPresets");
    
    // Add helper functions
    m.def("getConfig", &nlm::ConfigPresets::getConfig,
          py::arg("preset") = "beginner",
          "Get a configuration preset by name (beginner, research, performance, educational, rl, cognitive, or custom)");
    
    m.def("getAvailablePresets", &nlm::ConfigPresets::getAvailablePresets,
          "Get list of available configuration presets");
    
    m.def("getPresetDescription", &nlm::ConfigPresets::getPresetDescription,
          py::arg("preset"),
          "Get description of a configuration preset");
    
    // Add individual preset functions for Python convenience
    m.def("createBeginnerConfig", &nlm::ConfigPresets::createBeginnerConfig,
          "Create configuration for beginners and quick testing");
    
    m.def("createResearchConfig", &nlm::ConfigPresets::createResearchConfig,
          "Create configuration for scientific research");
    
    m.def("createPerformanceConfig", &nlm::ConfigPresets::createPerformanceConfig,
          "Create configuration optimized for performance");
    
    m.def("createEducationalConfig", &nlm::ConfigPresets::createEducationalConfig,
          "Create configuration for educational purposes");
    
    m.def("createRLConfig", &nlm::ConfigPresets::createRLConfig,
          "Create configuration for reinforcement learning");
    
    m.def("createCognitiveConfig", &nlm::ConfigPresets::createCognitiveConfig,
          "Create configuration for cognitive science research");
}