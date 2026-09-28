    // Enhanced type and utility functions
    m.def("createPyConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for Python users
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        
        return config;
    }, "Create a Python-friendly default configuration");
    
    // Type conversion utilities
    m.def("neuron_id_to_str", [](const NeuronId& id) {
        return "n" + std::to_string(id.value);
    }, py::arg("id"), "Convert neuron ID to string representation");
    
    m.def("synapse_id_to_str", [](const SynapseId& id) {
        return "s" + std::to_string(id.value);
    }, py::arg("id"), "Convert synapse ID to string representation");
    
    m.def("region_id_to_str", [](const RegionId& id) {
        return "r" + std::to_string(id.value);
    }, py::arg("id"), "Convert region ID to string representation");
    
    // String representation utilities
    m.def("format_neuron_id", [](const NeuronId& id) {
        return "Neuron_" + std::to_string(id.value);
    }, py::arg("id"), "Format neuron ID as readable string");
    
    m.def("format_synapse_id", [](const SynapseId& id) {
        return "Synapse_" + std::to_string(id.value);
    }, py::arg("id"), "Format synapse ID as readable string");
    
    m.def("format_region_id", [](const RegionId& id) {
        return "Region_" + std::to_string(id.value);
    }, py::arg("id"), "Format region ID as readable string");
    
    // Configuration validation
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        py::dict validation;
        bool isValid = true;
        
        // Check brain configuration
        if (auto neuronCount = config->getOr<size_t>("neuron_count")) {
            validation["neuron_count_ok"] = (*neuronCount > 0 && *neuronCount <= 100000);
            if (!validation["neuron_count_ok"].cast<bool>()) {
                isValid = false;
            }
        } else {
            validation["neuron_count_ok"] = false;
            isValid = false;
        }
        
        if (auto connProb = config->getOr<float>("connection_probability")) {
            validation["connection_probability_ok"] = (*connProb >= 0.0f && *connProb <= 1.0f);
            if (!validation["connection_probability_ok"].cast<bool>()) {
                isValid = false;
            }
        } else {
            validation["connection_probability_ok"] = false;
            isValid = false;
        }
        
        validation["valid"] = isValid;
        validation["message"] = isValid ? "Configuration is valid" : "Configuration has issues";
        
        return validation;
    }, py::arg("config"), "Validate configuration and return validation results");
    
    // Brain statistics and analysis
    m.def("analyze_brain_statistics", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        
        py::dict analysis;
        
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        analysis["ei_ratio"] = brain->getExcitationInhibitionRatio();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["pending_events"] = brain->getPendingSpikeEventCount();
        
        // Calculate derived statistics
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        
        if (totalNeurons > 0) {
            analysis["firing_percentage"] = (static_cast<float>(firingNeurons) / totalNeurons) * 100.0f;
        } else {
            analysis["firing_percentage"] = 0.0f;
        }
        
        if (brain->getExcitationInhibitionRatio() > 0) {
            analysis["excitatory_synapse_percentage"] = 
                100.0f / (1.0f + brain->getExcitationInhibitionRatio());
        } else {
            analysis["excitatory_synapse_percentage"] = 0.0f;
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze brain statistics and return analysis results");
    
    // Simulation utilities
    m.def("create_simulated_environment", [](int width = 20, int height = 20) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), 8, 8);
        
        // Add some initial objects
        WorldObject resource1(width/4, height/4, WorldObjectType::Resource, 1.0f);
        world->addObject(resource1);
        
        WorldObject hazard1(width*3/4, height*3/4, WorldObjectType::Hazard, -0.5f);
        world->addObject(hazard1);
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       "Create a simulated environment with default objects");
    
    m.def("get_simulation_constants", []() {
        py::dict constants;
        
        constants["default_timestep"] = 0.001;
        constants["default_neuron_count"] = 1000;
        constants["default_synapse_density"] = 0.1f;
        constants["default_connection_probability"] = 0.05f;
        constants["min_neuron_count"] = 1;
        constants["max_neuron_count"] = 100000;
        constants["min_connection_probability"] = 0.0f;
        constants["max_connection_probability"] = 1.0f;
        
        return constants;
    }, "Get NLM simulation constants");
    
    m.def("format_spike_count", [](size_t count) {
        if (count == 0) return std::string("0");
        else if (count < 1000) return std::to_string(count);
        else if (count < 1000000) return std::to_string(count/1000) + "K";
        else return std::to_string(count/1000000) + "M";
    }, py::arg("count"), "Format spike count in human-readable format");
    
    m.def("format_neuron_count", [](size_t count) {
        if (count == 0) return std::string("0");
        else if (count < 1000) return std::to_string(count);
        else if (count < 1000000) return std::to_string(count/1000) + "K";
        else return std::to_string(count/1000000) + "M";
    }, py::arg("count"), "Format neuron count in human-readable format");
    
    m.def("format_synapse_count", [](size_t count) {
        if (count == 0) return std::string("0");
        else if (count < 1000) return std::to_string(count);
        else if (count < 1000000) return std::to_string(count/1000) + "K";
        else return std::to_string(count/1000000) + "M";
    }, py::arg("count"), "Format synapse count in human-readable format");
    
    // Helper for creating actions
    m.def("create_action", [](ActionType type, const std::vector<float>& params = {}) {
        return std::make_unique<Action>(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>(),
       "Create an action with specified type and parameters");
    
    m.def("get_action_name", [](ActionType type) {
        return Action::getName(type);
    }, py::arg("type"), "Get human-readable name for action type");
    
    m.def("get_action_names", []() {
        py::list names;
        names.append("MoveForward");
        names.append("MoveBackward");
        names.append("TurnLeft");
        names.append("TurnRight");
        names.append("Look");
        names.append("Interact");
        names.append("Eat");
        names.append("Drink");
        names.append("Rest");
        names.append("Wait");
        names.append("Custom");
        return names;
    }, "Get list of available action names");
    
    // Enhanced NeuralId utilities
    m.def("format_neuron_id", [](const NeuronId& id, const std::string& prefix = "N") {
        return prefix + std::to_string(id.value);
    }, py::arg("id"), py::arg("prefix") = "N",
       "Format neuron ID with custom prefix");
    
    m.def("format_synapse_id", [](const SynapseId& id, const std::string& prefix = "S") {
        return prefix + std::to_string(id.value);
    }, py::arg("id"), py::arg("prefix") = "S",
       "Format synapse ID with custom prefix");
    
    m.def("format_region_id", [](const RegionId& id, const std::string& prefix = "R") {
        return prefix + std::to_string(id.value);
    }, py::arg("id"), py::arg("prefix") = "R",
       "Format region ID with custom prefix");
    
    // Type conversion utilities
    m.def("to_neuron_id", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Convert unsigned integer to NeuronId");
    
    m.def("to_synapse_id", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Convert unsigned integer to SynapseId");
    
    m.def("to_region_id", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Convert unsigned integer to RegionId");
    
    // Memory management utilities
    m.def("cleanup_brain", [](std::shared_ptr<Brain> brain) {
        if (brain) {
            brain->reset();
        }
    }, py::arg("brain"), "Clean up brain memory");
    
    m.def("cleanup_world", [](std::shared_ptr<SimpleWorld> world) {
        if (world) {
            world->reset();
        }
    }, py::arg("world"), "Clean up world memory");
    
    m.def("cleanup_agent", [](std::shared_ptr<AgentBrain> agent) {
        if (agent) {
            agent->reset();
        }
    }, py::arg("agent"), "Clean up agent memory");
    
    // System information
    m.def("get_version_info", []() {
        py::dict version;
        version["version"] = "0.1.0";
        version["major"] = 0;
        version["minor"] = 1;
        version["patch"] = 0;
        version["build"] = "Phase 6 - Enhanced Python Bindings";
        version["description"] = "NLM Neural Learning Machine - Enhanced Python Bindings";
        return version;
    }, "Get version information");
    
    m.def("get_feature_list", []() {
        py::list features;
        
        // Core features
        features.append("pythonic_property_accessors");
        features.append("context_manager_support");
        features.append("enhanced_error_handling");
        features.append("factory_methods_with_defaults");
        features.append("brain_statistics_aggregation");
        features.append("world_simulation_methods");
        features.append("agent_management_methods");
        features.append("type_improvements");
        
        // Specific features
        features.append("brain.neuron_count_property");
        features.append("brain.synapse_count_property");
        features.append("brain.ei_ratio_property");
        features.append("world.getTime()_property");
        features.append("agent.getSystemStatus()_method");
        features.append("enhanced_validation_methods");
        features.append("simulation_episode_methods");
        features.append("memory_cleanup_methods");
        features.append("human_readable_formatting");
        features.append("configuration_helper_methods");
        
        return features;
    }, "Get list of available Python features");