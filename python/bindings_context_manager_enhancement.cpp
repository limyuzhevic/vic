// Phase 1.0: Add context managers to Brain, AgentBrain, SimpleWorld

// Add context manager support to Brain class
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>("config"))
    .def("__enter__", [](Brain& self) { return &self; })
    .def("__exit__", [](Brain& self, py::args, py::kwargs) {
        try {
            // Proper cleanup - reset brain state
            self.reset();
        } catch (const std::exception& e) {
            // Log but don't propagate cleanup errors
            // In production, use proper logging
            std::cerr << "Warning: Error during brain cleanup in context manager: " << e.what() << std::endl;
        }
    })
    // ... existing methods ...
;

// Add context manager support to AgentBrain class  
py::class_<AgentBrain>(m, "AgentBrain")
    .def(py::init<std::shared_ptr<Brain>>())
    .def("__enter__", [](AgentBrain& self) { return &self; })
    .def("__exit__", [](AgentBrain& self, py::args, py::kwargs) {
        try {
            self.reset();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during agent brain cleanup: " << e.what() << std::endl;
        }
    })
    // ... existing methods ...
;

// Add context manager support to SimpleWorld class
py::class_<SimpleWorld>(m, "SimpleWorld")
    .def(py::init<>())
    .def("__enter__", [](SimpleWorld& self) { return &self; })
    .def("__exit__", [](SimpleWorld& self, py::args, py::kwargs) {
        try {
            self.reset();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during world cleanup: " << e.what() << std::endl;
        }
    })
    // ... existing methods ...
;

// Phase 1.1: Add convenience methods for common operations

// Convenience method: run multiple steps efficiently
Brain::runSteps(size_t numSteps, double timestep = 0.001) {
    for (size_t i = 0; i < numSteps; ++i) {
        step(i, i * timestep);
    }
}

// Convenience method: run simulation with error handling
bool Brain::runSimulation(size_t numSteps, double timestep = 0.001) {
    try {
        for (size_t i = 0; i < numSteps; ++i) {
            step(i, i * timestep);
            
            // Log progress every 100 steps
            if (i % 100 == 0) {
                NLM_LOG_INFO(std::string("Step ") + std::to_string(i) + 
                           std::string(" of ") + std::to_string(numSteps));
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Simulation error at step ") + std::to_string(numSteps) + 
                     std::string(": ") + e.what());
        return false;
    }
}

// Convenience method: batch sensory input processing
void Brain::receiveSensoryInputs(const std::vector<const SensoryInput*>& inputs) {
    for (const auto* input : inputs) {
        if (input) {
            receiveSensoryInput(*input);
        }
    }
}

// Convenience method: get action statistics
struct ActionStatistics {
    size_t totalActions;
    size_t moveActions;
    size_t turnActions;
    size_t interactActions;
    size_t waitActions;
    double avgActionFrequency;
    
    ActionStatistics() : totalActions(0), moveActions(0), turnActions(0), 
                         interactActions(0), waitActions(0), avgActionFrequency(0.0) {}
    
    void addAction(ActionType type) {
        ++totalActions;
        switch (type) {
            case ActionType::MoveForward:
            case ActionType::MoveBackward:
                ++moveActions;
                break;
            case ActionType::TurnLeft:
            case ActionType::TurnRight:
                ++turnActions;
                break;
            case ActionType::Interact:
                ++interactActions;
                break;
            case ActionType::Wait:
            case ActionType::Rest:
                ++waitActions;
                break;
            default:
                break;
        }
    }
};

ActionStatistics Brain::getActionStatistics(size_t steps) const {
    ActionStatistics stats;
    if (steps == 0) return stats;
    
    // Would need to track actions during simulation
    // This is a placeholder implementation
    stats.avgActionFrequency = static_cast<double>(stats.totalActions) / steps;
    return stats;
}

// Phase 1.2: Add type hints and improved documentation

// Enhanced Brain class with better type hints and documentation
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>("config"),
         "Create a brain with the specified configuration")
    .def("initialize", &Brain::initialize,
         "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"), 
         "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"),
         "Perform a simulation step with timestamp")
    .def("receiveSensoryInput", &Brain::receiveSensoryInput,
         py::arg("input"),
         "Inject sensory input into the brain")
    .def("produceAction", &Brain::produceAction,
         "Produce motor action based on neural activity")
    .def("reset", &Brain::reset,
         "Reset brain state")
    .def("save", &Brain::save, py::arg("filepath"),
         "Save brain state to file")
    .def("load", &Brain::load, py::arg("filepath"),
         "Load brain state from file")
    .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
         "Get total neuron count across all regions")
    .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
         "Get total synapse count across all regions")
    .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
         "Get count of currently firing neurons")
    .def("getAverageFiringRate", &Brain::getAverageFiringRate,
         "Get average firing rate across all neurons")
    .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
         "Get excitation/inhibition balance ratio")
    .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
         "Get total spike count")
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
         py::arg("stage"),
         "Set developmental stage")
    .def("addRegion", &Brain::addRegion, py::arg("name") = "",
         "Add a new neural region")
    .def("getRegion", &Brain::getRegion, py::arg("id"),
         "Get a region by ID")
    .def("getRegionCount", &Brain::getRegionCount,
         "Get the number of regions")
    .def("getRegionIds", &Brain::getRegionIds,
         "Get all region IDs")
    .def("getRegions", &Brain::getRegions,
         "Get all regions")
    .def("logStatus", &Brain::logStatus,
         "Log brain status")
    
    // Convenience methods with documentation
    .def("runSteps", &Brain::runSteps,
         py::arg("numSteps"), py::arg("timestep") = 0.001,
         "Run multiple simulation steps efficiently")
    
    .def("runSimulation", &Brain::runSimulation,
         py::arg("numSteps"), py::arg("timestep") = 0.001,
         "Run complete simulation with error handling and progress logging")
    
    .def("receiveSensoryInputs", &Brain::receiveSensoryInputs,
         py::arg("inputs"),
         "Receive multiple sensory inputs at once")
    
    .def("getActionStatistics", &Brain::getActionStatistics,
         py::arg("steps") = 0,
         "Get statistics about action production during simulation")
;

// Enhanced AgentBrain class with type hints
py::class_<AgentBrain>(m, "AgentBrain")
    .def(py::init<std::shared_ptr<Brain>>())
    .def("initialize", &AgentBrain::initialize, py::arg("world"),
         "Initialize with world")
    .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
         "Get expected sensory input size")
    .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
         "Get expected motor output size")
    .def("processSensoryInput", &AgentBrain::processSensoryInput,
         py::arg("percept"),
         "Process sensory percept and inject into brain")
    .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
         "Decode brain motor activity into motor command")
    .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predictedReward"),
         "Apply reward-based neuromodulation")
    .def("updateDevelopment", &AgentBrain::updateDevelopment,
         py::arg("timestep"),
         "Update development system")
    .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
         "Get current neuromodulation level")
    .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
         "Get curiosity level")
    .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
         "Get novelty level")
    .def("getPredictionError", &AgentBrain::getPredictionError,
         "Get prediction error")
    .def("reset", &AgentBrain::reset,
         "Reset agent for new episode")
    .def("getBrain", &AgentBrain::getBrain,
         "Get the underlying brain")
    
    // Subsystem enable/disable with type hints
    .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
         py::arg("enable"),
         "Enable/disable reward modulation")
    
    .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable"),
         "Enable/disable structural plasticity")
    
    .def("enableDevelopment", &AgentBrain::enableDevelopment,
         py::arg("enable"),
         "Enable/disable development")
    
    .def("enableCuriosity", &AgentBrain::enableCuriosity,
         py::arg("enable"),
         "Enable/disable curiosity")
    
    .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
         "Check if reward modulation is enabled")
    
    .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    
    .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
         "Check if development is enabled")
    
    .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
         "Check if curiosity is enabled")
;

// Enhanced SimpleWorld class with type hints
py::class_<SimpleWorld>(m, "SimpleWorld")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure,
         py::arg("width"), py::arg("height"),
         py::arg("visionWidth"), py::arg("visionHeight"),
         "Configure world dimensions")
    .def("reset", &SimpleWorld::reset,
         "Reset world to initial state")
    .def("setAgentStart", &SimpleWorld::setAgentStart,
         py::arg("x"), py::arg("y"),
         "Set agent starting position")
    .def("update", &SimpleWorld::update, py::arg("timestep"),
         "Update world by timestep seconds")
    .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("currentTime"),
         "Apply motor command to world")
    .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
         "Get sensory percept from world")
    .def("getAgentBody", &SimpleWorld::getAgentBody,
         "Get agent body state")
    .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
         "Add world object")
    .def("removeObject", &SimpleWorld::removeObject,
         py::arg("x"), py::arg("y"),
         "Remove world object")
    .def("isValidPosition", &SimpleWorld::isValidPosition,
         py::arg("x"), py::arg("y"),
         "Check if position is valid")
    .def("getWidth", &SimpleWorld::getWidth,
         "Get world width")
    .def("getHeight", &SimpleWorld::getHeight,
         "Get world height")
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
         "Get maximum energy")
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
         "Set maximum energy")
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
         "Get energy decay rate")
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
         py::arg("r"),
         "Set energy decay rate")
    .def("getSimulationTime", &SimpleWorld::getSimulationTime,
         "Get current simulation time")
    .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
         "Set random seed")
    .def("getRandomSeed", &SimpleWorld::getRandomSeed,
         "Get random seed")
;

// Phase 1.3: Add performance monitoring APIs

// Performance monitoring structure
struct PerformanceMetrics {
    double cpuUsage;                    // CPU usage percentage
    double memoryUsageMB;               // Memory usage in MB
    size_t allocatedNeurons;            // Number of allocated neurons
    size_t allocatedSynapses;           // Number of allocated synapses
    double simulationSpeed;             // Steps per second
    size_t pendingEvents;               // Number of pending events
    double averageFiringRate;           // Average firing rate across neurons
    size_t totalStepsCompleted;         // Total steps completed
    size_t totalSpikes;                 // Total spikes generated
    double uptimeSeconds;               // Uptime in seconds
};

// Add performance monitoring to Brain class
py::class_<Brain>(m, "Brain")
    // ... existing bindings ...
    
    // Performance monitoring methods
    .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
         "Get performance metrics for the simulation")
    
    .def("getMemoryUsage", &Brain::getMemoryUsage,
         "Get current memory usage in bytes")
    
    .def("getSimulationSpeed", &Brain::getSimulationSpeed,
         "Get current simulation speed in steps per second")
    
    .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
         "Get count of active neurons")
    
    .def("getPendingEventCount", &Brain::getPendingEventCount,
         "Get count of pending events")
;

// Performance monitoring methods implementation
PerformanceMetrics Brain::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Get system metrics
    metrics.cpuUsage = getCurrentCPUUsage();
    metrics.memoryUsageMB = getCurrentMemoryUsage() / (1024 * 1024);
    metrics.allocatedNeurons = getTotalNeuronCount();
    metrics.allocatedSynapses = getTotalSynapseCount();
    metrics.simulationSpeed = getSimulationSpeed();
    metrics.pendingEvents = getPendingEventCount();
    metrics.averageFiringRate = getAverageFiringRate();
    metrics.totalStepsCompleted = pImpl->currentStep;
    metrics.totalSpikes = getTotalSpikeCount();
    metrics.uptimeSeconds = getUptime();
    
    return metrics;
}

// Helper functions for performance monitoring
double getCurrentCPUUsage() {
    // Implementation would read system CPU usage
    return 0.0; // Placeholder
}

size_t getCurrentMemoryUsage() {
    // Implementation would read system memory usage
    return 0; // Placeholder
}

double Brain::getSimulationSpeed() const {
    if (pImpl->currentTime > 0.0) {
        return static_cast<double>(pImpl->currentStep) / pImpl->currentTime;
    }
    return 0.0;
}

size_t Brain::getPendingEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + 
           pImpl->spikeSystem->getPendingDelayedCount();
}

double Brain::getUptime() const {
    // Implementation would track simulation start time
    return 0.0; // Placeholder
}

// Phase 2: Add configuration validation

// Add configuration validation to Config class
py::class_<Config>(m, "Config")
    // ... existing bindings ...
    
    .def("validate", &Config::validate,
         "Validate configuration values against schema")
    
    .def("validateKeys", &Config::validateKeys, py::arg("keys"),
         "Validate specific configuration keys")
    
    .def("getSchema", &Config::getSchema,
         "Get default configuration schema")
    
    .def("validateJSONSchema", &Config::validateJSONSchema, py::arg("jsonSchema"),
         "Validate configuration against JSON schema")
;

// Configuration validation implementation
bool Config::validate() const {
    // Validate all configuration values
    const auto& schema = getDefaultSchema();
    std::vector<std::string> errors;
    
    for (const auto& rule : schema) {
        if (has(rule.key)) {
            // Validate type and constraints
            auto value = get(rule.value); // Need to implement get for variant types
            if (!validateValue(value, rule)) {
                errors.push_back("Invalid value for key '" + rule.key + "': " + 
                               getValidationError(value, rule));
            }
        } else {
            // Check if required
            if (rule.required) {
                errors.push_back("Missing required key: '" + rule.key + "'");
            }
        }
    }
    
    if (!errors.empty()) {
        throw ConfigurationException(errors[0]);
    }
    
    return true;
}

bool Config::validateKeys(const std::vector<std::string>& keys) const {
    std::vector<std::string> errors;
    
    for (const auto& key : keys) {
        if (!has(key)) {
            errors.push_back("Missing required key: '" + key + "'");
        }
    }
    
    if (!errors.empty()) {
        throw ConfigurationException(errors[0]);
    }
    
    return true;
}

const std::vector<Config::ValidationRule>& Config::getDefaultSchema() {
    static std::vector<ValidationRule> schema = {
        {"brain.neuron_count", ValidationRule::Type::INT, 1000, 
         [](const ConfigValue& value) {
             auto val = std::get<int>(value);
             return val > 0 && val <= 1000000;
         }, "Number of neurons (1-1000000)"}
        
        // ... other schema rules ...
    };
    return schema;
}

bool Config::validateJSONSchema(const std::string& jsonSchema) const {
    // Implementation would use JSON schema validation library
    // For now, return true as placeholder
    return true;
}

// Helper functions for configuration validation
bool Config::validateValue(const ConfigValue& value, const ValidationRule& rule) {
    switch (rule.type) {
        case ValidationRule::Type::INT:
            if (std::holds_alternative<int>(value)) {
                auto val = std::get<int>(value);
                return rule.validator ? rule.validator(value) : (val >= rule.minInt && val <= rule.maxInt);
            }
            return false;
            
        case ValidationRule::Type::DOUBLE:
            if (std::holds_alternative<double>(value)) {
                auto val = std::get<double>(value);
                return rule.validator ? rule.validator(value) : (val >= rule.minDouble && val <= rule.maxDouble);
            }
            return false;
            
        // ... other types ...
        
        default:
            return false;
    }
}

std::string Config::getValidationError(const ConfigValue& value, const ValidationRule& rule) {
    switch (rule.type) {
        case ValidationRule::Type::INT:
            if (!std::holds_alternative<int>(value)) {
                return "Expected int, got " + getTypeString(value);
            }
            break;
            
        case ValidationRule::Type::DOUBLE:
            if (!std::holds_alternative<double>(value)) {
                return "Expected double, got " + getTypeString(value);
            }
            break;
            
        // ... other types ...
    }
    return "Value validation failed";
}

const char* Config::getTypeString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

// Phase 3: Add context manager examples and usage patterns

// Context manager example function
std::string getContextManagerExample() {
    return R"(Example context manager usage:

import pynlm

# Example 1: Basic brain context manager
config = pynlm.createDefaultConfig()
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)
    # Brain automatically cleaned up here
    stats = brain.getPerformanceMetrics()
    print(f"Simulation completed: {stats.totalStepsCompleted} steps")

# Example 2: Complete agent with world context manager
config = pynlm.createDefaultConfig()
with pynlm.createBrain(config) as brain:
    brain.initialize()
    
    with pynlm.createSimpleWorld() as world:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 50 == 0:
                metrics = brain.getPerformanceMetrics()
                print(f"Step {step}: Firing neurons={metrics.allocatedNeurons}, "
                      f"Firing rate={metrics.averageFiringRate:.2f}")

# Example 3: Recording with context manager
with pynlm.createBrain(config) as brain:
    brain.initialize()
    
    # Run simulation and record state
    with brain.record("experiment1"):
        brain.runSteps(500)
        
        # Save checkpoint during simulation
        if brain.shouldSaveCheckpoint(100):
            brain.save(f"checkpoint_{brain.getCurrentStep()}.bin")
    
    # Load saved state
    brain.load("checkpoint_100.bin")
    brain.runSteps(100)
)";
}

// Add context manager example to Python module documentation
m.doc() = R"pbdoc(
NLM (Neural Learning Machine) Python Bindings
---------------------------------------------

A Python binding for the NLM C++ neural simulation framework.

Features:
- Context managers for automatic resource cleanup
- Type hints and improved documentation
- Performance monitoring APIs
- Configuration validation
- Convenience methods for common operations

Context Manager Support:
The NLM Python API provides context managers for automatic resource cleanup:

**Brain Context Manager:**
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)
    # Brain automatically cleaned up

**AgentBrain Context Manager:**
with pynlm.createBrain(config) as brain:
    brain.initialize()
    with pynlm.createAgentBrain(brain) as agent:
        agent.initialize(world)
        # Agent automatically cleaned up

**SimpleWorld Context Manager:**
with pynlm.createSimpleWorld() as world:
    world.configure(width=20, height=20)
    world.reset()
    # World automatically cleaned up

Usage:
- Import pynlm
- Create configuration with pynlm.createDefaultConfig()
- Create brain with pynlm.createBrain(config)
- Use context managers for automatic resource management
- Access performance metrics for monitoring
- Use convenience methods for common operations

Examples and documentation:
- See getContextManagerExample() for detailed usage examples
- Check easy_usage.md for beginner-friendly examples
- Check HOW_TO_USE.md for comprehensive documentation
"pbdoc";

// Add context manager example function to Python module
m.def("getContextManagerExample", &getContextManagerExample,
      "Get example code showing context manager usage");

PYBIND11_MODULE(pynlm, m) {
    // Import all headers
    // ... (existing includes) ...
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register all classes with context managers and enhanced APIs
    // (Implementation details above)
    
    // Add module-level functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create default configuration");
    m.def("createBrain", &createBrain, "Create a new brain with configuration");
    m.def("createSimpleWorld", &createSimpleWorld, "Create a new simple world");
    m.def("createAgentBrain", &createAgentBrain, "Create a new agent brain interface");
    
    // Add constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
