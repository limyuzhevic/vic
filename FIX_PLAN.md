# Detailed Fix Plan for NLM Python Bindings

## Phase 1: Usability Improvements

### 1.1 Add Factory Methods for Common Object Creation

**Problem**: Python users cannot create neural components directly.

**Solution**: Add factory methods to bindings.cpp:

```cpp
// Add to the PYBIND11_MODULE(pynlm, m) block

// Neuron factory method
m.def("createNeuron", [](NeuronType type) {
    // This would require adding a factory method to Neuron class
    // For now, create a neuron using existing API
    return std::make_shared<Neuron>(type);
}, py::arg("type") = NeuronType::Internal, 
   "Create a new neuron with specified type. Use pynlm.createBrain() for neural populations instead.");

// Synapse factory method  
m.def("createSynapse", [](std::shared_ptr<Neuron> source, std::shared_ptr<Neuron> destination, 
                          SynapticWeight weight) {
    return std::make_shared<Synapse>(source->getId(), destination->getId(), weight);
}, py::arg("source"), py::arg("destination"), py::arg("weight") = 1.0f,
   "Create a new synapse between two neurons.");

// Region factory method
m.def("createRegion", [](const std::string& name, size_t neuron_count) {
    RegionId id;  // Would need better ID generation
    auto region = std::make_shared<NeuralRegion>(id, name);
    
    // Add populations (simplified - would need proper population factory)
    for (size_t i = 0; i < neuron_count; i += 10) {
        region->addPopulation(10, NeuronType::Internal);
    }
    
    return region;
}, py::arg("name"), py::arg("neuron_count") = 100,
   "Create a new neural region with specified name and neuron count.");

// Population factory method
m.def("createPopulation", [](NeuronType type, size_t size) {
    return std::make_shared<NeuralPopulation>(type, size);
}, py::arg("type") = NeuronType::Internal, py::arg("size") = 10,
   "Create a new neuron population with specified type and size.");
```

### 1.2 Improve Error Handling

**Problem**: Error messages are not descriptive and error handling is inconsistent.

**Solution**: Add better error handling wrappers:

```cpp
// Replace current loadFromArgs with better error handling
m.def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
    try {
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str());
        bool success = self.loadFromArgs(static_cast<int>(argv.size()), argv.data();
        if (!success) {
            throw py::value_error("Configuration load failed");
        }
        return success;
    } catch (const std::exception& e) {
        throw py::value_error(std::string("Config loadFromArgs failed: " + e.what();
    }
}, py::arg("args"),
   "Load configuration from command line arguments. Raises RuntimeError on failure.");

// Add better error handling for world operations
m.def("applyMotorCommand", [](SimpleWorld& world, MotorCommand cmd, double currentTime) {
    ActionResult result = world.applyMotorCommand(cmd, currentTime);
    if (!result.success) {
        throw py::value_error(std::string("Motor command failed: " + result.message);
    }
    return result;
}, py::arg("world"), py::arg("cmd"), py::arg("currentTime"),
   "Apply motor command to world. Raises RuntimeError if command fails.");

// Add error checking for brain operations
m.def("step", [](Brain& brain, SimulationStep step) {
    try {
        brain.step(step);
        return true;
    } catch (const std::exception& e) {
        throw py::value_error(std::string("Brain step failed: " + e.what();
    }
}, py::arg("brain"), py::arg("step"),
   "Perform a simulation step. Raises RuntimeError on failure.");
```

### 1.3 Add Convenience Methods

**Problem**: No convenience methods for common operations.

**Solution**: Add convenience functions:

```cpp
// Episode runner convenience function
m.def("runEpisode", [](std::shared_ptr<Brain> brain, 
                      std::shared_ptr<AgentBrain> agent,
                      std::shared_ptr<SimpleWorld> world,
                      double timestep = 0.1, int steps = 1000) {
    // Initialize agent
    agent->initialize(*world);
    
    // Run simulation loop
    for (int step = 0; step < steps; ++step) {
        world->update(timestep);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime();
    }
}, py::arg("brain"), py::arg("agent"), py::arg("world"),
          py::arg("timestep") = 0.1, py::arg("steps") = 1000,
          "Run a complete agent episode with brain, agent, and world.");

// Brain statistics convenience function
m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
    py::dict stats;
    stats["total_neurons"] = brain->getTotalNeuronCount();
    stats["total_synapses"] = brain->getTotalSynapseCount();
    stats["active_neurons"] = brain->getActiveNeuronCount();
    stats["firing_neurons"] = brain->getFiringNeuronCount();
    stats["average_firing_rate"] = brain->getAverageFiringRate();
    stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
    stats["total_spike_count"] = brain->getTotalSpikeCount();
    stats["development_stage"] = brain->getDevelopmentalStage();
    return stats;
}, py::arg("brain"),
   "Get comprehensive brain statistics as a dictionary.");

// Helper function to create complete agent setup
m.def("createAgentSetup", [](std::shared_ptr<Config> config, 
                            std::shared_ptr<SimpleWorld> world) {
    py::dict setup;
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    setup["brain"] = brain;
    setup["agent"] = agent;
    setup["world"] = world;
    
    return setup;
}, py::arg("config"), py::arg("world"),
   "Create a complete agent setup with brain, agent, and world.");
```

## Phase 2: Functionality Gaps

### 2.1 Expose Missing Methods

**Problem**: Many useful methods are not exposed to Python.

**Solution**: Add missing method bindings:

```cpp
// Expose spike system methods
m.def("getSpikeHistory", [](std::shared_ptr<Brain> brain) {
    auto spike_sys = brain->getSpikeSystem();
    return spike_sys->getSpikeHistory();
}, py::arg("brain"),
   "Get spike history from brain's spike system.");

// Expose plasticity system access
m.def("getPlasticitySystems", [](std::shared_ptr<Brain> brain) {
    py::dict plasticity;
    plasticity["stdp"] = brain->getSTDP();
    plasticity["hebbian"] = brain->getHebbian();
    plasticity["structural"] = brain->getStructuralPlasticity();
    return plasticity;
}, py::arg("brain"),
   "Get access to all plasticity systems.");

// Expose memory systems
m.def("getMemorySystems", [](std::shared_ptr<Brain> brain) {
    py::dict memory;
    memory["working"] = brain->getWorkingMemory();
    memory["episodic"] = brain->getEpisodicMemory();
    memory["associative"] = brain->getAssociativeMemory();
    return memory;
}, py::arg("brain"),
   "Get access to all memory systems.");

// Expose prediction system
m.def("getPredictionSystem", [](std::shared_ptr<Brain> brain) {
    return brain->getPredictionSystem();
}, py::arg("brain"),
   "Get access to prediction system.");

// Expose cognition systems
m.def("getCognitionSystems", [](std::shared_ptr<Brain> brain) {
    py::dict cognition;
    cognition["planner"] = brain->getPlanner();
    cognition["concept_formation"] = brain->getConceptFormation();
    cognition["attention"] = brain->getAttention();
    return cognition;
}, py::arg("brain"),
   "Get access to all cognition systems.");

// Expose development systems
m.def("getDevelopmentSystems", [](std::shared_ptr<Brain> brain) {
    py::dict development;
    development["development"] = brain->getDevelopmentSystem();
    development["dopamine"] = brain->getDopamine();
    development["curiosity"] = brain->getCuriosity();
    development["novelty"] = brain->getNovelty();
    development["prediction_error"] = brain->getPredictionErrorSignal();
    return development;
}, py::arg("brain"),
   "Get access to all neuromodulation and development systems.");

// Add shared_ptr version of getRegion for safer access
m.def("getRegionAsShared", [](std::shared_ptr<Brain> brain, RegionId id) {
    return brain->getRegion(id);  // This needs to be modified in Brain.hpp
}, py::arg("brain"), py::arg("id"),
   "Get region as shared pointer (safer than raw pointer).");

// Add helper to get all neurons
m.def("getAllNeurons", [](std::shared_ptr<Brain> brain) {
    std::vector<std::shared_ptr<Neuron>> all_neurons;
    for (const auto& region : brain->getRegions()) {
        auto region_neurons = region->getAllNeurons();
        for (auto* neuron : region_neurons) {
            all_neurons.push_back(std::shared_ptr<Neuron>(neuron));
        }
    }
    return all_neurons;
}, py::arg("brain"),
   "Get all neurons in brain as shared pointers.");
```

### 2.2 Expose Phase 6 Advanced Features

**Problem**: Phase 6 features are not fully exposed.

**Solution**: Add Phase 6 feature bindings:

```cpp
// Expose SelfModel and SocialLearning
m.def("getSelfModel", [](std::shared_ptr<AgentBrain> agent) {
    // Would need to add getSelfModel() method to AgentBrain
    // For now, return null if method exists
#ifdef AGENTBRAIN_HAS_GETSELMODEL
    return agent->getSelfModel();
#else
    return std::shared_ptr<SelfModel>(nullptr);
#endif
}, py::arg("agent"),
   "Get agent's self-model (if available).");

m.def("getSocialLearning", [](std::shared_ptr<AgentBrain> agent) {
    // Would need to add getSocialLearning() method to AgentBrain
#ifdef AGENTBRAIN_HAS_GETSOCIALLEARNING
    return agent->getSocialLearning();
#else
    return std::shared_ptr<SocialLearning>(nullptr);
#endif
}, py::arg("agent"),
   "Get agent's social learning system (if available).");

// Enhanced text encoding with statistics
m.def("encodeTextWithStats", [](const std::string& text) {
    py::dict result;
    result["pattern"] = AgentBrain::encodeText(text);
    result["length"] = result["pattern"].size();
    result["word_length"] = text.length();
    result["is_case_insensitive"] = std::all_of(text.begin(), text.end(), 
                                                [](char c) { return std::isalpha(c); });
    return result;
}, py::arg("text"),
   "Encode text with statistics about the encoding.");

// Add thought trace with more details
m.def("getDetailedThoughtTrace", [](std::shared_ptr<AgentBrain> agent) {
    py::dict trace;
    trace["percept"] = agent->getThoughtTrace();
    trace["curiosity"] = agent->getCuriosityLevel();
    trace["novelty"] = agent->getNoveltyLevel();
    trace["prediction_error"] = agent->getPredictionError();
    trace["neuromodulation"] = agent->getNeuromodulationLevel();
    return trace;
}, py::arg("agent"),
   "Get detailed thought trace with additional metrics.");
```

### 2.3 Improve Return Value Policies

**Problem**: Some methods return references that may be unsafe in Python.

**Solution**: Add safer alternatives:

```cpp
// Add shared_ptr versions of getters for safety
m.def("getRegionShared", [](std::shared_ptr<Brain> brain, RegionId id) {
    auto* region = brain->getRegion(id);
    return std::shared_ptr<NeuralRegion>(region);
}, py::arg("brain"), py::arg("id"),
   "Get region as shared pointer (safer memory management).");

m.def("getPopulationShared", [](std::shared_ptr<NeuralRegion> region, PopulationId id) {
    auto* population = region->getPopulation(id);
    return std::shared_ptr<NeuralPopulation>(population);
}, py::arg("region"), py::arg("id"),
   "Get population as shared pointer (safer memory management).");

m.def("getSynapseShared", [](std::shared_ptr<NeuralRegion> region, SynapseId id) {
    auto* synapse = region->getSynapse(id);
    return std::shared_ptr<Synapse>(synapse);
}, py::arg("region"), py::arg("id"),
   "Get synapse as shared pointer (safer memory management).");
```

## Phase 3: Code Quality Improvements

### 3.1 Reduce Code Duplication

**Problem**: Significant duplication in ID and enum bindings.

**Solution**: Create template functions:

```cpp
// Template function to reduce ID struct duplication
template<typename T>
void bindIdType(py::module& m, const std::string& name) {
    py::class_<T>(m, name.c_str(), R"pbdoc(Unique identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const T& id) {
            return "<" + std::string(name) + ": " + 
                   std::to_string(id.value) + ">";
        });
}

// Replace individual bindings with template calls
bindIdType<NeuronId>(m, "NeuronId");
bindIdType<SynapseId>(m, "SynapseId");
bindIdType<RegionId>(m, "RegionId");
bindIdType<PopulationId>(m, "PopulationId");

// Template function for enum bindings
template<typename T>
void bindEnumType(py::module& m, const std::string& name) {
    py::enum_<T>(m, name.c_str(), R"pbdoc(Enumeration)pbdoc")
        .value("Excitatory", T::Excitatory)
        .value("Inhibitory", T::Inhibitory)
        .value("Modulatory", T::Modulatory)
        .value("Sensory", T::Sensory)
        .value("Motor", T::Motor)
        .value("Internal", T::Internal)
        .export_values();
}

// Replace individual enum bindings
bindEnumType<NeuronType>(m, "NeuronType");
bindEnumType<SynapseType>(m, "SynapseType");
bindEnumType<DevelopmentalStage>(m, "DevelopmentalStage");
// ... etc for all enums
```

### 3.2 Improve Type Safety

**Problem**: Returned references can be invalid in Python.

**Solution**: Add validation and safer alternatives:

```cpp
// Add validation methods
m.def("validateNeuronId", [](std::shared_ptr<Brain> brain, NeuronId id) {
    for (const auto& region : brain->getRegions()) {
        auto populations = region->getAllPopulations();
        for (const auto& population : populations) {
            if (population->hasNeuron(id)) return true;
        }
    }
    return false;
}, py::arg("brain"), py::arg("id"),
   "Validate that neuron ID exists in brain.");

m.def("validateSynapseId", [](std::shared_ptr<Brain> brain, SynapseId id) {
    for (const auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            if (synapse->getId() == id) return true;
        }
    }
    return false;
}, py::arg("brain"), py::arg("id"),
   "Validate that synapse ID exists in brain.");

// Add safe getter with validation
m.def("getSynapseSafe", [](std::shared_ptr<Brain> brain, SynapseId id) {
    auto* synapse = nullptr;
    for (const auto& region : brain->getRegions()) {
        auto region_synapses = region->getSynapses();
        for (const auto& syn_ptr : region_synapses) {
            if (syn_ptr->getId() == id) {
                synapse = syn_ptr.get();
                break;
            }
        }
    }
    if (!synapse) {
        throw py::value_error("Synapse ID not found");
    }
    return std::shared_ptr<Synapse>(synapse);
}, py::arg("brain"), py::arg("id"),
   "Safely get synapse with validation. Raises ValueError if not found.");
```

## Phase 4: Documentation Improvements

### 4.1 Add Comprehensive Docstrings

**Problem**: Many methods lack documentation.

**Solution**: Add comprehensive docstrings:

```cpp
// Example of improved docstring
def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
    // ... implementation ...
}, py::arg("args"),
   "Load configuration from command line arguments.\n\n"
   "Args:\n"
   "    args: List of command line arguments (e.g., ['--neuron_count', '1000']).\n\n"
   "Returns:\n"
   "    bool: True if configuration loaded successfully.\n\n"
   "Raises:\n"
   "    RuntimeError: If configuration file not found or cannot be parsed.\n\n"
   "Example:\n"
   "    >>> config.loadFromArgs(['--neuron_count', '500'])",

// Add missing docstrings for important methods
def("getSpikeSystem", &Brain::getSpikeSystem,
     "Get the spike system for this brain. The spike system handles\n"
     "event-driven spike propagation and synaptic delays.\n\n"
     "Returns:\n"
     "    SpikeSystem*: Pointer to the spike system object.");

// Add comprehensive docstrings with examples
def("createBrain", &createBrain, "Create a new brain with specified configuration.\n\n"
     "Args:\n"
     "    config: Configuration object for brain parameters.\n\n"
     "Example:\n"
     "    >>> config = pynlm.createDefaultConfig()\n"
     "    >>> config.set('neuron_count', 1000)\n"
     "    >>> brain = pynlm.createBrain(config)\n"
     "    >>> brain.initialize()",
     "py::arg("config"),
     "Create a new brain with configuration.");

// Add method examples to important classes
.py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
     .def(py::init<>())
     .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
          "Load configuration from a JSON file.\n\n"
          "Args:\n"
          "    filepath: Path to JSON configuration file.\n\n"
          "Example:\n"
          "    >>> config.loadFromFile('brain_config.json')")
     // ... other methods with examples ...
```

### 4.otřeb to add comprehensive examples

**Problem**: No usage examples in bindings.

**Solution**: Add example scripts and comprehensive examples:

```cpp
// Add example module or comprehensive documentation

// Create examples directory with Python examples
// Example 1: Basic usage
// File: examples/basic_usage.py

// Example 2: Complete agent episode
// File: examples/agent_episode.py

// Example 3: Brain analysis
// File: examples/brain_analysis.py

// Example 4: Memory system usage
// File: examples/memory_systems.py

// Example 5: Advanced features
// File: examples/advanced_features.py
```

## Implementation Priority Plan

### High Priority (Week 1-2):
1. Add factory methods for object creation
2. Improve error handling with descriptive exceptions
3. Add convenience episode runner
4. Add comprehensive docstrings with examples

### Medium Priority (Week 3-4):
1. Expose missing memory systems
2. Expose missing plasticity systems
3. Add shared_ptr alternatives for type safety
4. Reduce code duplication using templates

### Low Priority (Week 5-6):
1. Add Phase 6 advanced feature exposure
2. Improve return value policies
3. Add comprehensive validation methods
4. Add more examples and tutorials

## Backward Compatibility

**Note**: All changes maintain backward compatibility:
- Existing Python code continues to work unchanged
- New functionality is additive
- Improved error handling may change exception types but maintains behavior
- New factory methods are additional APIs, not replacements

## Testing Strategy

### Unit Tests for New Functionality:
1. **Factory methods**: Test neuron, synapse, region creation
2. **Error handling**: Test exception cases and error messages
3. **Convenience methods**: Test episode runner and statistics
4. therapy for new exposed methods

### Integration Tests:
1. **Complete workflow**: Test new API in realistic scenarios
2. **Memory system tests**: Test exposed memory systems
3. **Plasticity system tests**: Test exposed plasticity systems
4. **Phase 6 features**: Test advanced Phase 6 features

### Documentation Tests:
1. **Docstring coverage**: Verify all important methods have documentation
2. **Example tests**: Test example scripts provided in documentation
3. **API discovery**: Verify users can discover new functionality

The implementation plan provides comprehensive improvements to the NLM Python bindings, addressing all identified issues while maintaining backward compatibility and providing a solid foundation for future development.