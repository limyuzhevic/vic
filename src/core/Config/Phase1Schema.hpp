// Phase 1 schema - Basic skeleton configuration
struct Phase1Schema {
    static ConfigSchema create() {
        ConfigSchema schema;
        schema.version = "1.0.0";
        schema.filename = "phase1_schema.json";
        
        // Random seed for reproducible experiments
        ConfigSchema::Field field;
        field.key = "random_seed";
        field.type = "int";
        field.required = false;
        field.description = "Random seed for reproducible experiments";
        field.min = 0;
        schema.addFieldWithConstraints(field);
        
        // Simulation timestep in seconds
        field.key = "simulation_timestep";
        field.type = "double";
        field.required = false;
        field.description = "Simulation timestep in seconds";
        field.min = std::variant<int, double>(0.000001);
        schema.addFieldWithConstraints(field);
        
        // Number of neurons in the brain
        field.key = "neuron_count";
        field.type = "int";
        field.required = false;
        field.description = "Number of neurons in the brain";
        field.min = 10;
        field.max = 1000000;
        schema.addFieldWithConstraints(field);
        
        // Number of brain regions
        field.key = "region_count";
        field.type = "int";
        field.required = false;
        field.description = "Number of brain regions";
        field.min = 1;
        field.max = 100;
        schema.addFieldWithConstraints(field);
        
        // Connection probability for random connectivity
        field.key = "connection_probability";
        field.type = "double";
        field.required = false;
        field.description = "Connection probability for random connectivity";
        field.min = std::variant<int, double>(0.0);
        field.max = std::variant<int, double>(1.0);
        schema.addFieldWithConstraints(field);
        
        // Plasticity learning rate
        field.key = "plasticity_learning_rate";
        field.type = "double";
        field.required = false;
        field.description = "Plasticity learning rate";
        field.min = std::variant<int, double>(0.0);
        field.max = std::variant<int, double>(1.0);
        schema.addFieldWithConstraints(field);
        
        // STDP LTP weight
        field.key = "stdp_ltp_weight";
        field.type = "double";
        field.required = false;
        field.description = "STDP LTP weight";
        field.min = std::variant<int, double>(0.0);
        schema.addFieldWithConstraints(field);
        
        // STDP LTD weight
        field.key = "stdp_ltd_weight";
        field.type = "double";
        field.required = false;
        field.description = "STDP LTD weight";
        field.min = std::variant<int, double>(0.0);
        schema.addFieldWithConstraints(field);
        
        // STDP time constant
        field.key = "stdp_time_constant";
        field.type = "double";
        field.required = false;
        field.description = "STDP time constant (ms)";
        field.min = std::variant<int, double>(0.0);
        schema.addFieldWithConstraints(field);
        
        // Development synaptogenesis rate
        field.key = "development_synaptogenesis_rate";
        field.type = "double";
        field.required = false;
        field.description = "Development synaptogenesis rate";
        field.min = std::variant<int, double>(0.0);
        schema.addFieldWithConstraints(field);
        
        // Development pruning rate
        field.key = "development_pruning_rate";
        field.required = false;
        field.description = "Development pruning rate";
        field.type = "double";
        field.min = std::variant<int, double>(0.0);
        schema.addFieldWithConstraints(field);
        
        // Dopamine baseline
        field.key = "dopamine_baseline";
        field.type = "double";
        field.required = false;
        field.description = "Dopamine baseline value";
        schema.addFieldWithConstraints(field);
        
        // Reward discount factor
        field.key = "reward_discount_factor";
        field.type = "double";
        field.required = false;
        field.description = "Reward discount factor";
        field.min = std::variant<int, double>(0.0);
        field.max = std::variant<int, double>(1.0);
        schema.addFieldWithConstraints(field);
        
        // Environment settings
        field.key = "environment_name";
        field.type = "string";
        field.required = false;
        field.description = "Environment name";
        schema.addFieldWithConstraints(field);
        
        field.key = "environment_width";
        field.type = "int";
        field.required = false;
        field.description = "Environment width";
        field.min = 1;
        schema.addFieldWithConstraints(field);
        
        field.key = "environment_height";
        field.type = "int";
        field.required = false;
        field.description = "Environment height";
        field.min = 1;
        schema.addFieldWithConstraints(field);
        
        // Logging settings
        field.key = "log_level";
        field.type = "string";
        field.required = false;
        field.description = "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)";
        field.allowedValues = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
        schema.addFieldWithConstraints(field);
        
        field.key = "log_to_file";
        field.type = "bool";
        field.required = false;
        field.description = "Log to file";
        schema.addFieldWithConstraints(field);
        
        field.key = "log_filename";
        field.type = "string";
        field.required = false;
        field.description = "Log filename";
        schema.addFieldWithConstraints(field);
        
        // Simulation settings
        field.key = "max_simulation_steps";
        field.type = "int64_t";
        field.required = false;
        field.description = "Maximum simulation steps";
        field.min = 1;
        schema.addFieldWithConstraints(field);
        
        field.key = "simulation_time_limit";
        field.type = "double";
        field.required = false;
        field.description = "Simulation time limit (seconds)";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Visualization settings
        field.key = "visualization_enabled";
        field.type = "bool";
        field.required = false;
        field.description = "Enable visualization";
        schema.addFieldWithConstraints(field);
        
        field.key = "visualization_update_rate";
        field.type = "double";
        field.required = false;
        field.description = "Visualization update rate (Hz)";
        field.min = 1.0;
        schema.addFieldWithConstraints(field);
        
        return schema;
    }
};

// Phase 2 schema - Enhanced with neuromodulation and memory
struct Phase2Schema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase1Schema::create();
        schema.version = "2.0.0";
        schema.filename = "phase2_schema.json";
        
        // Add Phase 2 specific fields
        ConfigSchema::Field field;
        
        // Neuromodulation settings
        field.key = "dopamine_max";
        field.type = "double";
        field.required = false;
        field.description = "Maximum dopamine level";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "dopamine_decay_rate";
        field.type = "double";
        field.required = false;
        field.description = "Dopamine decay rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Working memory settings
        field.key = "working_memory_size";
        field.type = "int";
        field.required = false;
        field.description = "Working memory capacity (number of items)";
        field.min = 1;
        field.max = 1000;
        schema.addFieldWithConstraints(field);
        
        field.key = "working_memory_decay_rate";
        field.type = "double";
        field.required = false;
        field.description = "Working memory decay rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Episodic memory settings
        field.key = "episodic_memory_size";
        field.type = "int";
        field.required = false;
        field.description = "Episodic memory capacity (number of episodes)";
        field.min = 1;
        field.max = 10000;
        schema.addFieldWithConstraints(field);
        
        field.key = "episodic_memory_similarity_threshold";
        field.type = "double";
        field.required = false;
        field.description = "Episodic memory similarity threshold";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Semantic memory settings
        field.key = "semantic_memory_learning_rate";
        field.type = "double";
        field.required = false;
        field.description = "Semantic memory learning rate";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Prediction error settings
        field.key = "prediction_error_decay_rate";
        field.type = "double";
        field.required = false;
        field.description = "Prediction error decay rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Novelty settings
        field.key = "novelty_threshold";
        field.type = "double";
        field.required = false;
        field.description = "Novelty detection threshold";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Curiosity settings
        field.key = "curiosity_strength";
        field.type = "double";
        field.required = false;
        field.description = "Curiosity modulation strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Add mutually exclusive groups
        std::vector<std::string> neuromodulationGroups = {"dopamine_baseline", "dopamine_max"};
        schema.fields.back().mutuallyExclusiveGroups.push_back(neuromodulationGroups);
        
        return schema;
    }
};

// Phase 3 schema - Advanced cognitive functions
struct Phase3Schema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase2Schema::create();
        schema.version = "3.0.0";
        schema.filename = "phase3_schema.json";
        
        // Add Phase 3 specific fields
        ConfigSchema::Field field;
        
        // Concept formation settings
        field.key = "concept_formation_threshold";
        field.type = "double";
        field.required = false;
        field.description = "Concept formation threshold";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "concept_generalization_strength";
        field.type = "double";
        field.required = false;
        field.description = "Concept generalization strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Neural planning settings
        field.key = "planning_horizon";
        field.type = "int";
        field.required = false;
        field.description = "Planning horizon (time steps)";
        field.min = 1;
        field.max = 1000;
        schema.addFieldWithConstraints(field);
        
        field.key = "planning_exploration_weight";
        field.type = "double";
        field.required = false;
        field.description = "Planning exploration weight";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Cognitive control settings
        field.key = "cognitive_control_strength";
        field.type = "double";
        field.required = false;
        field.description = "Cognitive control strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "cognitive_control_decay_rate";
        field.type = "double";
        field.required = false;
        field.description = "Cognitive control decay rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Working memory consolidation
        field.key = "working_memory_consolidation_rate";
        field.type = "double";
        field.required = false;
        field.description = "Working memory consolidation rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Episodic memory retrieval
        field.key = "episodic_memory_retrieval_strength";
        field.type = "double";
        field.required = false;
        field.description = "Episodic memory retrieval strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Add required combinations
        std::vector<std::string> cognitiveRequirements = {"concept_formation_threshold", "concept_generalization_strength"};
        schema.fields.back().requiredCombinations.push_back(cognitiveRequirements);
        
        return schema;
    }
};

// Phase 4 schema - Motor and action systems
struct Phase4Schema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase3Schema::create();
        schema.version = "4.0.0";
        schema.filename = "phase4_schema.json";
        
        // Add Phase 4 specific fields
        ConfigSchema::Field field;
        
        // Motor system settings
        field.key = "motor_noise_level";
        field.type = "double";
        field.required = false;
        field.description = "Motor noise level";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "motor_gain";
        field.type = "double";
        field.required = false;
        field.description = "Motor gain factor";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Action selection settings
        field.key = "action_selection_temperature";
        field.type = "double";
        field.required = false;
        field.description = "Action selection temperature (for softmax)";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "action_selection_exploration_rate";
        field.type = "double";
        field.required = false;
        field.description = "Action selection exploration rate";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Reward prediction settings
        field.key = "reward_prediction_error_scaling";
        field.type = "double";
        field.required = false;
        field.description = "Reward prediction error scaling factor";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Action outcomes
        field.key = "action_persistence";
        field.type = "double";
        field.required = false;
        field.description = "Action persistence (repetition probability)";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        return schema;
    }
};

// Phase 6 schema - Integration and advanced features
struct Phase6Schema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase4Schema::create();
        schema.version = "6.0.0";
        schema.filename = "phase6_schema.json";
        
        // Add Phase 6 specific fields
        ConfigSchema::Field field;
        
        // Integration settings
        field.key = "integration_learning_rate";
        field.type = "double";
        field.required = false;
        field.description = "Integration learning rate across systems";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "integration_coherence_target";
        field.type = "double";
        field.required = false;
        field.description = "Target system coherence level";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Meta-learning settings
        field.key = "meta_learning_rate";
        field.type = "double";
        field.required = false;
        field.description = "Meta-learning rate (learning how to learn)";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "meta_learning_threshold";
        field.type = "double";
        field.required = false;
        field.description = "Meta-learning threshold for switching strategies";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        // Self-organization settings
        field.key = "self_organization_strength";
        field.type = "double";
        field.required = false;
        field.description = "Self-organization strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        field.key = "self_organization_decay_rate";
        field.type = "double";
        field.required = false;
        field.description = "Self-organization decay rate";
        field.min = 0.0;
        schema.addFieldWithConstraints(field);
        
        // Global modulation
        field.key = "global_modulation_strength";
        field.type = "double";
        field.required = false;
        field.description = "Global neuromodulation strength";
        field.min = 0.0;
        field.max = 1.0;
        schema.addFieldWithConstraints(field);
        
        return schema;
    }
};

// Development environment schema
struct DevelopmentSchema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase6Schema::create();
        schema.version = "dev-1.0.0";
        schema.filename = "development_schema.json";
        
        // Add development-specific fields
        ConfigSchema::Field field;
        
        field.key = "enable_debug_mode";
        field.type = "bool";
        field.required = false;
        field.description = "Enable debug mode";
        schema.addFieldWithConstraints(field);
        
        field.key = "enable_validation_detailed";
        field.type = "bool";
        field.required = false;
        field.description = "Enable detailed validation output";
        schema.addFieldWithConstraints(field);
        
        field.key = "validation_strategy";
        field.type = "string";
        field.required = false;
        field.description = "Validation strategy (strict, lenient, report)";
        field.allowedValues = {"strict", "lenient", "report"};
        schema.addFieldWithConstraints(field);
        
        field.key = "enable_schema_docs";
        field.type = "bool";
        field.required = false;
        field.description = "Generate schema documentation";
        schema.addFieldWithConstraints(field);
        
        field.key = "schema_docs_output_dir";
        field.type = "string";
        field.required = false;
        field.description = "Schema documentation output directory";
        schema.addFieldWithConstraints(field);
        
        // Enable debug and detailed validation
        schema.fields.back().dependencies = {"validation_strategy"};
        
        return schema;
    }
};

// Production environment schema
struct ProductionSchema {
    static ConfigSchema create() {
        ConfigSchema schema = Phase6Schema::create();
        schema.version = "prod-1.0.0";
        schema.filename = "production_schema.json";
        
        // Add production-specific fields
        ConfigSchema::Field field;
        
        field.key = "performance_monitoring";
        field.type = "bool";
        field.required = false;
        field.description = "Enable performance monitoring";
        schema.addFieldWithConstraints(field);
        
        field.key = "performance_sample_rate";
        field.type = "int";
        field.required = false;
        field.description = "Performance metrics sampling rate (Hz)";
        field.min = 1;
        schema.addFieldWithConstraints(field);
        
        field.key = "error_reporting_enabled";
        field.type = "bool";
        field.required = false;
        field.description = "Enable error reporting";
        schema.addFieldWithConstraints(field);
        
        field.key = "error_reporting_endpoint";
        field.type = "string";
        field.required = false;
        field.description = "Error reporting endpoint URL";
        schema.addFieldWithConstraints(field);
        
        field.key = "log_retention_days";
        field.type = "int";
        field.required = false;
        field.description = "Log retention period in days";
        field.min = 1;
        field.max = 365;
        schema.addFieldWithConstraints(field);
        
        field.key = "enable_hot_reloading";
        field.type = "bool";
        field.required = false;
        field.description = "Enable configuration hot-reloading";
        schema.addFieldWithConstraints(field);
        
        field.key = "hot_reload_check_interval";
        field.type = "double";
        field.required = false;
        field.description = "Hot-reload check interval (seconds)";
        field.min = 0.1;
        field.max = 60.0;
        schema.addFieldWithConstraints(field);
        
        // Add validation for error reporting
        std::vector<std::string> errorDeps = {"error_reporting_enabled", "error_reporting_endpoint"};
        schema.fields.back().dependencies = errorDeps;
        
        return schema;
    }
};

// Default schema (Phase 1)
ConfigSchema getDefaultSchema() {
    return Phase1Schema::create();
}

ConfigSchema getPhase2Schema() {
    return Phase2Schema::create();
}

ConfigSchema getPhase3Schema() {
    return Phase3Schema::create();
}

ConfigSchema getPhase4Schema() {
    return Phase4Schema::create();
}

ConfigSchema getPhase6Schema() {
    return Phase6Schema::create();
}

ConfigSchema getDevelopmentSchema() {
    return DevelopmentSchema::create();
}

ConfigSchema getProductionSchema() {
    return ProductionSchema::create();
}
