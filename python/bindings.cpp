#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <mutex>

// Core headers
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"

// Brain headers
#include "../src/brain/Brain.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Synapse.hpp"

// Sensory headers
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Motor headers
#include "../src/motor/Action.hpp"

// Agent headers
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// World headers
#include "../src/world/SimpleWorld.hpp"

// Neuromodulation headers
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Development headers
#include "../src/development/DevelopmentSystem.hpp"

// Prediction headers
#include "../src/prediction/PredictionSystem.hpp"

// Plasticity headers
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Dynamics headers
#include "../src/dynamics/SpikeSystem.hpp"

namespace py = pybind11;

// Custom exception hierarchy for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

// Registration function for validation decorators
template<typename Func>
auto validate(Func&& func, const std::string& errorMsg) {
    return [func = std::move(func), errorMsg](auto&&... args) {
        try {
            return func(std::forward<decltype(args)>(args)...);
        } catch (const std::exception& e) {
            throw ValidationError(errorMsg + ": " + e.what());
        }
    };
}

// Forward declarations for builder patterns
class RegionBuilder;
class NeuronBuilder;
class PopulationBuilder;
class SynapseBuilder;

// Builder classes for fluent API construction
class RegionBuilder {
public:
    RegionBuilder(std::shared_ptr<Brain> brain, const std::string& name) 
        : brain_(brain), name_(name) {}
    
    RegionBuilder& withNeuronCount(size_t count) {
        neuronCount_ = count;
        return *this;
    }
    
    RegionBuilder& withNeuronType(NeuronType type) {
        neuronType_ = type;
        return *this;
    }
    
    RegionBuilder& withConnectionProbability(float prob) {
        connectionProb_ = prob;
        return *this;
    }
    
    RegionBuilder& withInitialWeight(float weight) {
        initialWeight_ = weight;
        return *this;
    }
    
    RegionId build() {
        if (!brain_) throw ValidationError("Brain instance not set for region builder");
        
        RegionId regionId = brain_->addRegion(name_);
        if (neuronCount_ > 0) {
            brain_->getRegion(regionId)->initializeRandom(*brain_->getRandomGenerator(), 
                                                          connectionProb_, 
                                                          initialWeight_, 
                                                          0.1f);
        }
        return regionId;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::string name_;
    size_t neuronCount_ = 0;
    NeuronType neuronType_ = NeuronType::Internal;
    float connectionProb_ = 0.1f;
    float initialWeight_ = 1.0f;
};

class NeuronBuilder {
public:
    NeuronBuilder(std::shared_ptr<NeuralRegion> region) : region_(region) {}
    
    NeuronBuilder& withType(NeuronType type) {
        neuronType_ = type;
        return *this;
    }
    
    NeuronBuilder& withThreshold(float threshold) {
        threshold_ = threshold;
        return *this;
    }
    
    NeuronBuilder& withRestingPotential(float potential) {
        restingPotential_ = potential;
        return *this;
    }
    
    NeuronBuilder& withRefractoryPeriod(uint32_t steps) {
        refractoryPeriod_ = steps;
        return *this;
    }
    
    NeuronId build() {
        if (!region_) throw ValidationError("Region instance not set for neuron builder");
        return region_->addNeuron(neuronCount_, neuronType_);
    }
    
private:
    std::shared_ptr<NeuralRegion> region_;
    size_t neuronCount_ = 1;
    NeuronType neuronType_ = NeuronType::Excitatory;
    float threshold_ = -55.0f;
    float restingPotential_ = -70.0f;
    uint32_t refractoryPeriod_ = 5;
};

// Context manager classes for automatic resource management
class BrainSession : public std::enable_shared_from_this<BrainSession> {
public:
    explicit BrainSession(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) throw ValidationError("Brain instance cannot be null");
        brain_->initialize();
    }
    
    ~BrainSession() {
        try {
            brain_->reset();
        } catch (...) {
            // Log error but don't propagate in destructor
        }
    }
    
    BrainSession(const BrainSession&) = delete;
    BrainSession& operator=(const BrainSession&) = delete;
    
    BrainSession(BrainSession&&) noexcept = default;
    BrainSession& operator=(BrainSession&&) noexcept = default;
    
    // Context manager interface
    void __enter__() {
        // Initialize is already done in constructor
    }
    
    void __exit__(int, const char*, int, const char*) {
        brain_->reset();
    }
    
    // Session operations
    void step() {
        brain_->step(currentStep_++);
    }
    
    void stepWithTime(Timestamp time) {
        brain_->step(currentStep_++, time);
    }
    
    std::shared_ptr<Action> produceAction() {
        return brain_->produceAction();
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Statistics
    size_t getTotalNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getTotalSynapseCount() const { return brain_->getTotalSynapseCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    
private:
    std::shared_ptr<Brain> brain_;
    SimulationStep currentStep_ = 0;
};

// Diagnostic tools
class DiagnosticTools {
public:
    static void analyzeConnectivity(const std::shared_ptr<Brain>& brain) {
        if (!brain) throw ValidationError("Brain instance is null");
        
        std::cout << "=== NEURAL NETWORK DIAGNOSTICS ===" << std::endl;
        std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(3) 
                  << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        // Check for developmental stage
        auto stage = brain->getDevelopmentalStage();
        std::cout << "Developmental Stage: " << brain->getDevelopmentSystem()->getStageName() << std::endl;
        
        // Neuromodulation levels
        if (auto dopamine = brain->getDopamine()) {
            std::cout << "Dopamine Level: " << std::fixed << std::setprecision(3) 
                      << dopamine->getLevel() << std::endl;
        }
        if (auto curiosity = brain->getCuriosity()) {
            std::cout << "Curiosity Level: " << curiosity->getLevel() << std::endl;
        }
        if (auto novelty = brain->getNovelty()) {
            std::cout << "Novelty Level: " << novelty->getLevel() << std::endl;
        }
    }
    
    static std::string getMemoryUsage() {
        // Simple implementation - in real implementation would use platform-specific APIs
        return "Memory monitoring not implemented in this build";
    }
    
    static void validateIntegrity(const std::shared_ptr<Brain>& brain) {
        if (!brain) throw ValidationError("Brain instance is null");
        
        std::cout << "=== INTEGRITY CHECK ===" << std::endl;
        
        // Check region consistency
        auto regionIds = brain->getRegionIds();
        std::cout << "Regions: " << regionIds.size() << std::endl;
        
        for (const auto& regionId : regionIds) {
            if (auto region = brain->getRegion(regionId)) {
                auto neuronCount = region->getTotalNeuronCount();
                std::cout << "  Region " << regionId.value << ": " << neuronCount << " neurons" << std::endl;
            }
        }
        
        std::cout << "Integrity check complete." << std::endl;
    }
};

// High-level convenience functions
class NLMApi {
public:
    static std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        config->set("simulation.dt", 0.1f);
        config->set("simulation.max_steps", 10000);
        config->set("brain.region_count", 3);
        config->set("brain.connection_probability", 0.1f);
        return config;
    }
    
    static std::shared_ptr<Brain> createBrain(const std::string& name = "DefaultBrain") {
        auto config = createConfig();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        return brain;
    }
    
    static std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain with provided config");
        }
        return brain;
    }
    
    static std::shared_ptr<SimpleWorld> createWorld() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 320, 240);
        return world;
    }
    
    static std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    static std::shared_ptr<Vision> createVisionSensor(size_t width, size_t height, size_t channels = 3) {
        return std::make_shared<Vision>(width, height, channels);
    }
    
    static std::shared_ptr<Audio> createAudioSensor(size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }
    
    static std::shared_ptr<InternalSignals> createInternalSensor() {
        return std::make_shared<InternalSignals>();
    }
    
    static std::shared_ptr<Neuromodulator> createNeuromodulator(const std::string& type) {
        if (type == "dopamine") {
            return std::make_shared<Dopamine>();
        } else if (type == "curiosity") {
            return std::make_shared<Curiosity>();
        } else if (type == "novelty") {
            return std::make_shared<Novelty>();
        } else {
            throw ValidationError("Unknown neuromodulator type: " + type);
        }
    }
    
    static void runSimulation(const std::shared_ptr<Brain>& brain, 
                            const std::shared_ptr<SimpleWorld>& world,
                            SimulationStep steps) {
        for (SimulationStep i = 0; i < steps; ++i) {
            // Process sensory input if world exists
            if (world) {
                auto percept = world->getSensoryPercept();
                if (auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain)) {
                    agentBrain->processSensoryInput(*percept);
                }
            }
            
            brain->step(i);
            
            // Produce and apply action if world exists
            if (world) {
                auto action = brain->produceAction();
                if (action) {
                    world->applyMotorCommand(*action, i);
                }
            }
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides advanced Pythonic API with context managers, builder patterns,
        and extensive documentation for interactive neural network development.
        
        Key Features:
        * Full support for neural regions, neurons, populations, and synapses
        * Neuromodulation systems (Dopamine, Curiosity, Novelty, Reward)
        * Development and prediction systems
        * Context manager support for automatic resource management
        * Builder patterns for fluent API construction
        * Advanced diagnostic and debugging tools
        * High-level convenience functions
        * Custom exception hierarchy with validation decorators
        * Performance optimizations with move semantics
        * Comprehensive type hints and documentation
        
        Usage Examples:
        
        ```python
        import pynlm
        
        # Create a brain with configuration
        config = pynlm.Config()
        config.loadFromFile("config.json")
        brain = pynlm.Brain(config)
        
        # Use brain as a context manager for automatic cleanup
        with brain.session() as session:
            # Run simulation
            for step in range(100):
                session.step()
                action = session.produce_action()
                if action:
                    print(f"Step {step}: {action.get_name()}")
        
        # Create regions using builder pattern
        brain = pynlm.NLMApi.create_brain("MyNeuralNet")
        region_id = (pynlm.RegionBuilder(brain, "Cortex")
                    .withNeuronCount(100)
                    .withNeuronType(pynlm.NeuronType.Excitatory)
                    .build())
        
        # Run diagnostics
        pynlm.DiagnosticTools.analyze_connectivity(brain)
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register common type aliases for better type hints
    m.attr("Matrix") = py::type<>(std::vector<std::vector<float>>());
    m.attr("Tensor3D") = py::type<>(std::vector<std::vector<std::vector<float>>>());

    // NeuronId and related types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Enums with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that increase postsynaptic firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that decrease postsynaptic firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that influence neural plasticity and learning")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that process external stimuli")
        .value("Motor", NeuronType::Motor, "Motor neurons that drive actions and movements")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing and integration")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse increasing postsynaptic potential")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse decreasing postsynaptic potential")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse for fast signal transmission")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse for direct ion flow")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for sensory system development")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase with decreasing plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable neural connections")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with gradual decline in function")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing action potentials")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period, cannot fire")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by external factors")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM)
        
        The configuration system provides flexible parameter management for neural
        network simulations. Supports loading from JSON, command line arguments,
        and programmatic creation.
        
        Attributes:
            Keys for configuration values (accessible via get())
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("settings.json")
            >>> config.get("simulation.dt")
            0.1
            >>> config.has("brain.region_count")
            True
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file)
             
             Args:
                 filepath: Path to JSON configuration file
             
             Raises:
                 ConfigurationError: If file cannot be loaded or parsed
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments)
           
           Args:
               argc: Number of command line arguments
               argv: Array of command line argument strings
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)
             
             Args:
                 filepath: Path where configuration will be saved
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists)
             
             Args:
                 key: Configuration key to check
             
             Returns:
                 True if key exists, False otherwise
             )pbdoc")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue = "") {
            std::string value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = "",
             R"pbdoc(Get configuration value with optional default)
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
             
             Returns:
                 Configuration value as string
             )pbdoc")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue = 0) {
            int value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0,
             R"pbdoc(Get configuration value as integer)
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
             
             Returns:
                 Configuration value as integer
             )pbdoc")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue = 0.0f) {
            float value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0f,
             R"pbdoc(Get configuration value as float)
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
             
             Returns:
                 Configuration value as float
             )pbdoc")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue = false) {
            bool value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = false,
             R"pbdoc(Get configuration value as boolean)
             
             Args:
                 key: Configuration key
                 default_value: Default value if key doesn't exist
             
             Returns:
                 Configuration value as boolean
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set configuration value)
             
             Args:
                 key: Configuration key
                 value: Configuration value
             )pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set configuration value as integer)
             
             Args:
                 key: Configuration key
                 value: Configuration value as integer
             )pbdoc")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(6) << value;
            self.set(key, ss.str());
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set configuration value as float)
             
             Args:
                 key: Configuration key
                 value: Configuration value as float
             )pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value ? "true" : "false");
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set configuration value as boolean)
             
             Args:
                 key: Configuration key
                 value: Configuration value as boolean
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration)
             
             Returns:
                 Summary of configuration contents
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // NeuronState structure
    py::class_<NeuronState>(m, "NeuronState", R"pbdoc(Neuron internal state representation)
        
        Contains all state variables for a single neuron in the simulation.
        Updated automatically during simulation steps.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("membranePotential", &NeuronState::membranePotential)
        .def_readwrite("restingPotential", &NeuronState::restingPotential)
        .def_readwrite("threshold", &NeuronState::threshold)
        .def_readwrite("resetPotential", &NeuronState::resetPotential)
        .def_readwrite("leakConductance", &NeuronState::leakConductance)
        .def_readwrite("synapseConductance", &NeuronState::synapseConductance)
        .def_readwrite("firingRate", &NeuronState::firingRate)
        .def_readwrite("firingState", &NeuronState::firingState)
        .def_readwrite("refractoryRemaining", &NeuronState::refractoryRemaining)
        .def_readwrite("refractoryPeriod", &NeuronState::refractoryPeriod)
        .def_readwrite("adaptationVariable", &NeuronState::adaptationVariable)
        .def_readwrite("lastSpikeTime", &NeuronState::lastSpikeTime)
        .def("__repr__", [](const NeuronState& state) {
            std::stringstream ss;
            ss << "<NeuronState membrane=" << state.membranePotential 
               << " threshold=" << state.threshold 
               << " firingRate=" << state.firingRate << ">";
            return ss.str();
        });

    // SynapseState structure
    py::class_<SynapseState>(m, "SynapseState", R"pbdoc(Synapse internal state representation)
        
        Contains state variables for synaptic dynamics including plasticity.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("weight", &SynapseState::weight)
        .def_readwrite("delay", &SynapseState::delay)
        .def_readwrite("efficacy", &SynapseState::efficacy)
        .def_readwrite("eligibilityTrace", &SynapseState::eligibilityTrace)
        .def("__repr__", [](const SynapseState& state) {
            std::stringstream ss;
            ss << "<SynapseState weight=" << state.weight 
               << " efficacy=" << state.efficacy << ">";
            return ss.str();
        });

    // Neuron class with context manager support
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Advanced spiking neuron model)
        
        Implements Leaky Integrate-and-Fire (LIF) dynamics with support for
        synaptic inputs, plasticity, and neuromodulation. Each neuron maintains
        detailed state information and spike history.
        
        Attributes:
            id: Unique neuron identifier
            type: Neuron type (Excitatory, Inhibitory, etc.)
            state: Current neuron state
        
        Examples:
            >>> neuron = neuron_builder.withType(NeuronType.Excitatory)\.build()
            >>> neuron.setThreshold(-50.0)
            >>> neuron.step(0.0)
            >>> if neuron.isFiring():
            ...     print("Neuron fired!")
        )pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def(py::init<NeuronId, NeuronType>(), py::arg("id"), py::arg("type"))
        .def("getId", &Neuron::getId,
             R"pbdoc(Get neuron identifier)
             
             Returns:
                 NeuronId: Unique identifier for this neuron
             )pbdoc")
        .def("getType", &Neuron::getType,
             R"pbdoc(Get neuron type)
             
             Returns:
                 NeuronType: The neuron's functional type
             )pbdoc")
        .def("setType", &Neuron::setType, py::arg("type"),
             R"pbdoc(Set neuron type)
             
             Args:
                 type: New neuron type
             )pbdoc")
        .def("getState", &Neuron::getState,
             R"pbdoc(Get current neuron state)
             
             Returns:
                 NeuronState: Current neuron state
             )pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             R"pbdoc(Set membrane potential)
             
             Args:
                 potential: New membrane potential (mV)
             )pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             R"pbdoc(Add to membrane potential)
             
             Args:
                 delta: Change in membrane potential (mV)
             )pbdoc")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             R"pbdoc(Get membrane potential)
             
             Returns:
                 MembranePotential: Current membrane potential (mV)
             )pbdoc")
        .def("getThreshold", &Neuron::getThreshold,
             R"pbdoc(Get firing threshold)
             
             Returns:
                 MembranePotential: Firing threshold (mV)
             )pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             R"pbdoc(Set firing threshold)
             
             Args:
                 threshold: New firing threshold (mV)
             )pbdoc")
        .def("isFiring", &Neuron::isFiring,
             R"pbdoc(Check if neuron is currently firing)
             
             Returns:
                 True if neuron is firing, False otherwise
             )pbdoc")
        .def("isRefractory", &Neuron::isRefractory,
             R"pbdoc(Check if neuron is in refractory period)
             
             Returns:
                 True if neuron is refractory, False otherwise
             )pbdoc")
        .def("getFiringRate", &Neuron::getFiringRate,
             R"pbdoc(Get firing rate)
             
             Returns:
                 FiringRate: Current firing rate (Hz)
             )pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             R"pbdoc(Set firing rate)
             
             Args:
                 rate: New firing rate (Hz)
             )pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"),
             R"pbdoc(Set refractory period)
             
             Args:
                 steps: Number of simulation steps to remain refractory
             )pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             R"pbdoc(Decrement refractory counter)
             
             Decrements the refractory remaining counter by 1 step.
             " )pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold,
             R"pbdoc(Check if neuron has reached firing threshold)
             
             Returns:
                 True if neuron should fire, False otherwise
             )pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             R"pbdoc(Get timestamp of last spike)
             
             Returns:
                 Timestamp: Time of last spike, or -1 if never spiked
             )pbdoc")
        .def("stepLIF", [](Neuron& self, Timestamp currentTime, TimestepDuration dt) {
            return self.stepLIF(currentTime, dt);
        }, py::arg("currentTime"), py::arg("dt"),
             R"pbdoc(Perform one LIF integration step)
             
             Args:
                 currentTime: Current simulation time
                 dt: Time step duration
             
             Returns:
                 True if neuron fired during this step, False otherwise
             )pbdoc")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"),
             R"pbdoc(Receive excitatory input)
             
             Args:
                 amplitude: Excitatory input amplitude (nS * mV)
             )pbdoc")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"),
             R"pbdoc(Receive inhibitory input)
             
             Args:
                 amplitude: Inhibitory input amplitude (nS * mV)
             )pbdoc")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"),
             R"pbdoc(Receive modulatory input)
             
             Args:
                 amplitude: Modulatory input amplitude (nS * mV)
             )pbdoc")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             R"pbdoc(Inject external current)
             
             Args:
                 current: External current injection (nA)
             )pbdoc")
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             R"pbdoc(Get total injected current)
             
             Returns:
                 MembranePotential: Total current (nA)
             )pbdoc")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             R"pbdoc(Clear injected current)")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             R"pbdoc(Record a spike event)
             
             Args:
                 timestamp: Time when spike occurred
             )pbdoc")
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             R"pbdoc(Get spike history)
             
             Returns:
                 List of timestamps when neuron fired
             )pbdoc")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             R"pbdoc(Clear spike history)")
        .def("reset", &Neuron::reset,
             R"pbdoc(Reset neuron to initial state)")
        .def("initializeRandom", [](Neuron& self, class RandomGenerator& rng) {
            self.initializeRandom(rng);
        }, py::arg("rng"),
             R"pbdoc(Initialize neuron with random parameters)
             
             Args:
                 rng: Random number generator
             )pbdoc")
        .def("__repr__", [](const Neuron& neuron) {
            std::stringstream ss;
            ss << "<Neuron id=" << neuron.getId().value 
               << " type=" << static_cast<int>(neuron.getType())
               << " firing=" << neuron.isFiring() << ">";
            return ss.str();
        });

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population with shared properties)
        
        A group of neurons that share the same type and are managed together.
        Provides population-level statistics and operations.
        
        Attributes:
            id: Population identifier
            size: Number of neurons in population
            neuronType: Type of all neurons in this population
        )pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId,
             R"pbdoc(Get population identifier)
             
             Returns:
                 PopulationId: Unique identifier
             )pbdoc")
        .def("getSize", &NeuralPopulation::getSize,
             R"pbdoc(Get population size)
             
             Returns:
                 int: Number of neurons in population
             )pbdoc")
        .def("isEmpty", &NeuralPopulation::isEmpty,
             R"pbdoc(Check if population is empty)
             
             Returns:
                 bool: True if population has no neurons
             )pbdoc")
        .def("getNeuron", [](const NeuralPopulation& self, NeuronIndex index) {
            return self.getNeuron(index);
        }, py::arg("index"),
             R"pbdoc(Get neuron by index)
             
             Args:
                 index: Neuron index within population
             
             Returns:
                 Neuron*: Pointer to neuron, or nullptr if index invalid
             )pbdoc")
        .def("addNeuron", [](NeuralPopulation& self, Neuron* neuron) {
            if (!neuron) throw ValidationError("Neuron pointer cannot be null");
            self.addNeuron(neuron);
        }, py::arg("neuron"),
             R"pbdoc(Add neuron to population)
             
             Args:
                 neuron: Neuron pointer to add
             )pbdoc")
        .def("getNeurons", &NeuralPopulation::getNeurons,
             R"pbdoc(Get all neurons in population)
             
             Returns:
                 List of neuron pointers
             )pbdoc")
        .def("getNeuronType", &NeuralPopulation::getNeuronType,
             R"pbdoc(Get neuron type for this population)
             
             Returns:
                 NeuronType: Type of neurons in this population
             )pbdoc")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"),
             R"pbdoc(Set neuron type for this population)
             
             Args:
                 type: New neuron type
             )pbdoc")
        .def("getActiveCount", &NeuralPopulation::getActiveCount,
             R"pbdoc(Get number of active neurons)
             
             Returns:
                 int: Count of active neurons
             )pbdoc")
        .def("getFiringCount", &NeuralPopulation::getFiringCount,
             R"pbdoc(Get number of firing neurons)
             
             Returns:
                 int: Count of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate,
             R"pbdoc(Get average firing rate)
             
             Returns:
                 float: Average firing rate (Hz)
             )pbdoc")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel,
             R"pbdoc(Get population activity level)
             
             Returns:
                 float: Fraction of active neurons (0.0 to 1.0)
             )pbdoc")
        .def("initializeRandom", [](NeuralPopulation& self, class RandomGenerator& rng) {
            self.initializeRandom(rng);
        }, py::arg("rng"),
             R"pbdoc(Initialize all neurons randomly)
             
             Args:
                 rng: Random number generator
             )pbdoc")
        .def("step", [](NeuralPopulation& self, Timestamp currentTime) {
            self.step(currentTime);
        }, py::arg("currentTime"),
             R"pbdoc(Step all neurons)
             
             Args:
                 currentTime: Current simulation time
             )pbdoc")
        .def("reset", &NeuralPopulation::reset,
             R"pbdoc(Reset all neurons)")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds,
             R"pbdoc(Get neuron IDs in this population)
             
             Returns:
                 List of neuron IDs
             )pbdoc")
        .def("recordActivity", &NeuralPopulation::recordActivity,
             R"pbdoc(Record current activity for statistics)")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential,
             R"pbdoc(Get mean membrane potential)
             
             Returns:
                 float: Average membrane potential
             )pbdoc")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential,
             R"pbdoc(Get variance of membrane potential)
             
             Returns:
                 float: Variance of membrane potential
             )pbdoc")
        .def("__repr__", [](const NeuralPopulation& pop) {
            std::stringstream ss;
            ss << "<NeuralPopulation id=" << pop.getId().value 
               << " size=" << pop.getSize()
               << " type=" << static_cast<int>(pop.getNeuronType()) << ">";
            return ss.str();
        });

    // Synapse class
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Neural synapse with plasticity)
        
        Implements synaptic transmission with dynamic weights and plasticity.
        Supports various plasticity mechanisms including STDP, Hebbian learning,
        and reward-modulated plasticity.
        
        Attributes:
            id: Synapse identifier
            source: Source neuron
            destination: Destination neuron
            type: Synapse type (excitatory, inhibitory, etc.)
        )pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId,
             R"pbdoc(Get synapse identifier)
             
             Returns:
                 SynapseId: Unique identifier
             )pbdoc")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             R"pbdoc(Get source neuron)
             
             Returns:
                 NeuronId: Source neuron identifier
             )pbdoc")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             R"pbdoc(Get destination neuron)
             
             Returns:
                 NeuronId: Destination neuron identifier
             )pbdoc")
        .def("getWeight", &Synapse::getWeight,
             R"pbdoc(Get synaptic weight)
             
             Returns:
                 SynapticWeight: Current synaptic weight
             )pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             R"pbdoc(Set synaptic weight)
             
             Args:
                 weight: New synaptic weight
             )pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             R"pbdoc(Add to synaptic weight)
             
             Args:
                 delta: Change in weight
             )pbdoc")
        .def("getDelay", &Synapse::getDelay,
             R"pbdoc(Get transmission delay)
             
             Returns:
                 Delay: Delay in simulation steps
             )pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             R"pbdoc(Set transmission delay)
             
             Args:
                 delay: New delay (simulation steps)
             )pbdoc")
        .def("getType", &Synapse::getType,
             R"pbdoc(Get synapse type)
             
             Returns:
                 SynapseType: Type of synapse
             )pbdoc")
        .def("setType", &Synapse::setType, py::arg("type"),
             R"pbdoc(Set synapse type)
             
             Args:
                 type: New synapse type
             )pbdoc")
        .def("isExcitatory", &Synapse::isExcitatory,
             R"pbdoc(Check if synapse is excitatory)
             
             Returns:
                 bool: True if excitatory, False otherwise
             )pbdoc")
        .def("isInhibitory", &Synapse::isInhibitory,
             R"pbdoc(Check if synapse is inhibitory)
             
             Returns:
                 bool: True if inhibitory, False otherwise
             )pbdoc")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             R"pbdoc(Record presynaptic spike)
             
             Args:
                 timestamp: Time of presynaptic spike
             )pbdoc")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             R"pbdoc(Record postsynaptic spike)
             
             Args:
                 timestamp: Time of postsynaptic spike
             )pbdoc")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory,
             R"pbdoc(Get presynaptic spike history)
             
             Returns:
                 List of presynaptic spike times
             )pbdoc")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory,
             R"pbdoc(Get postsynaptic spike history)
             
             Returns:
                 List of postsynaptic spike times
             )pbdoc")
        .def("clearHistory", &Synapse::clearHistory,
             R"pbdoc(Clear spike history)")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             R"pbdoc(Get plasticity flags)
             
             Returns:
                 PlasticityFlags: Current plasticity configuration
             )pbdoc")
        .def("enablePlasticity", [](Synapse& self, bool hebbian, bool stdp, bool rewardModulated) {
            self.enablePlasticity(hebbian, stdp, rewardModulated);
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             R"pbdoc(Enable/disable plasticity rules)
             
             Args:
                 hebbian: Enable Hebbian learning
                 stdp: Enable spike-timing dependent plasticity
                 rewardModulated: Enable reward-modulated plasticity
             )pbdoc")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace,
             R"pbdoc(Get eligibility trace value)
             
             Returns:
                 float: Current eligibility trace
             )pbdoc")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"),
             R"pbdoc(Set eligibility trace)
             
             Args:
                 trace: New eligibility trace value
             )pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"),
             R"pbdoc(Decay eligibility trace)
             
             Args:
                 decayRate: Decay rate per simulation step
             )pbdoc")
        .def("getEfficacy", &Synapse::getEfficacy,
             R"pbdoc(Get synaptic efficacy)
             
             Returns:
                 float: Current synaptic efficacy
             )pbdoc")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"),
             R"pbdoc(Set synaptic efficacy)
             
             Args:
                 efficacy: New efficacy value
             )pbdoc")
        .def("step", [](Synapse& self, Timestamp currentTime) {
            self.step(currentTime);
        }, py::arg("currentTime"),
             R"pbdoc(Update synapse)
             
             Args:
                 currentTime: Current simulation time
             )pbdoc")
        .def("reset", &Synapse::reset,
             R"pbdoc(Reset to initial state)")
        .def("initializeRandom", [](Synapse& self, class RandomGenerator& rng) {
            self.initializeRandom(rng);
        }, py::arg("rng"),
             R"pbdoc(Initialize with random parameters)
             
             Args:
                 rng: Random number generator
             )pbdoc")
        .def("__repr__", [](const Synapse& synapse) {
            std::stringstream ss;
            ss << "<Synapse id=" << synapse.getId().value 
               << " src=" << synapse.getSourceNeuron().value
               << " dst=" << synapse.getDestinationNeuron().value
               << " weight=" << synapse.getWeight() << ">";
            return ss.str();
        });

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural computation region)
        
        A brain region containing multiple neural populations and local connectivity.
        Manages population dynamics, synapse formation, and regional statistics.
        
        Attributes:
            id: Region identifier
            name: Region name
        )pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId,
             R"pbdoc(Get region identifier)
             
             Returns:
                 RegionId: Unique identifier
             )pbdoc")
        .def("getName", &NeuralRegion::getName,
             R"pbdoc(Get region name)
             
             Returns:
                 str: Region name
             )pbdoc")
        .def("setName", &NeuralRegion::setName, py::arg("name"),
             R"pbdoc(Set region name)
             
             Args:
                 name: New region name
             )pbdoc")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal,
             R"pbdoc(Add neural population)
             
             Args:
                 size: Number of neurons in population
                 type: Neuron type (default: Internal)
             
             Returns:
                 PopulationId: Identifier for new population
             )pbdoc")
        .def("getPopulation", [](const NeuralRegion& self, PopulationId id) {
            return self.getPopulation(id);
        }, py::arg("id"),
             R"pbdoc(Get population by ID)
             
             Args:
                 id: Population identifier
             
             Returns:
                 NeuralPopulation*: Pointer to population, or nullptr if not found
             )pbdoc")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount,
             R"pbdoc(Get number of populations)
             
             Returns:
                 int: Number of populations
             )pbdoc")
        .def("getPopulations", &NeuralRegion::getPopulations,
             R"pbdoc(Get all populations)
             
             Returns:
                 List of population pointers
             )pbdoc")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations,
             R"pbdoc(Get all population pointers as list)
             
             Returns:
                 List of population pointers
             )pbdoc")
        .def("addSynapse", [](NeuralRegion& self, NeuronId source, NeuronId destination, 
                           SynapticWeight weight = 0.0f, Delay delay = 1) {
            return self.addSynapse(source, destination, weight, delay);
        }, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             R"pbdoc(Add synapse between neurons)
             
             Args:
                 source: Source neuron ID
                 destination: Destination neuron ID
                 weight: Synaptic weight (default: 0.0)
                 delay: Transmission delay in steps (default: 1)
             
             Returns:
                 SynapseId: Identifier for new synapse
             )pbdoc")
        .def("getSynapse", [](const NeuralRegion& self, SynapseId id) {
            return self.getSynapse(id);
        }, py::arg("id"),
             R"pbdoc(Get synapse by ID)
             
             Args:
                 id: Synapse identifier
             
             Returns:
                 Synapse*: Pointer to synapse, or nullptr if not found
             )pbdoc")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount,
             R"pbdoc(Get number of synapses)
             
             Returns:
                 int: Number of synapses
             )pbdoc")
        .def("getSynapses", &NeuralRegion::getSynapses,
             R"pbdoc(Get all synapses)
             
             Returns:
                 List of synapse pointers
             )pbdoc")
        .def("getSynapsesFrom", [](const NeuralRegion& self, NeuronId neuron) {
            return self.getSynapsesFrom(neuron);
        }, py::arg("neuron"),
             R"pbdoc(Get synapses originating from neuron)
             
             Args:
                 neuron: Source neuron ID
             
             Returns:
                 List of synapse pointers
             )pbdoc")
        .def("getSynapsesTo", [](const NeuralRegion& self, NeuronId neuron) {
            return self.getSynapsesTo(neuron);
        }, py::arg("neuron"),
             R"pbdoc(Get synapses terminating at neuron)
             
             Args:
                 neuron: Destination neuron ID
             
             Returns:
                 List of synapse pointers
             )pbdoc")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount,
             R"pbdoc(Get total neuron count)
             
             Returns:
                 int: Total number of neurons
             )pbdoc")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount,
             R"pbdoc(Get active neuron count)
             
             Returns:
                 int: Number of active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount,
             R"pbdoc(Get firing neuron count)
             
             Returns:
                 int: Number of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate,
             R"pbdoc(Get average firing rate)
             
             Returns:
                 float: Average firing rate (Hz)
             )pbdoc")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight,
             R"pbdoc(Get average synaptic weight)
             
             Returns:
                 float: Average synaptic weight
             )pbdoc")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity,
             R"pbdoc(Get synaptic density)
             
             Returns:
                 float: Fraction of possible connections that are present
             )pbdoc")
        .def("step", [](NeuralRegion& self, Timestamp currentTime) {
            self.step(currentTime);
        }, py::arg("currentTime"),
             R"pbdoc(Step all components)
             
             Args:
                 currentTime: Current simulation time
             )pbdoc")
        .def("reset", &NeuralRegion::reset,
             R"pbdoc(Reset all neurons and synapses)")
        .def("initializeRandomConnectivity", [](NeuralRegion& self, class RandomGenerator& rng,
                                              float connectionProbability, float meanWeight, float weightVariance) {
            self.initializeRandomConnectivity(rng, connectionProbability, meanWeight, weightVariance);
        }, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             R"pbdoc(Initialize random connectivity)
             
             Args:
                 rng: Random number generator
                 connectionProbability: Probability of connection between neurons
                 meanWeight: Mean synaptic weight
                 weightVariance: Variance of synaptic weight
             )pbdoc")
        .def("getAllNeurons", [](const NeuralRegion& self) {
            return self.getAllNeurons();
        },
             R"pbdoc(Get all neurons in region)
             
             Returns:
                 List of neuron pointers
             )pbdoc")
        .def("getAllNeuronsConst", [](const NeuralRegion& self) {
            return self.getAllNeurons();
        },
             R"pbdoc(Get all neurons in region (const version)
             
             Returns:
                 List of const neuron pointers
             )pbdoc")
        .def("__repr__", [](const NeuralRegion& region) {
            std::stringstream ss;
            ss << "<NeuralRegion id=" << region.getId().value 
               << " name=\"" << region.getName() << "\"";
            return ss.str();
        });

    // Factory methods for neural components
    m.def("createRegionBuilder", [](std::shared_ptr<Brain> brain, const std::string& name) {
        return RegionBuilder(brain, name);
    }, py::arg("brain"), py::arg("name"),
        R"pbdoc(Create region builder for fluent API construction)
        
        Args:
            brain: Brain instance to build regions for
            name: Region name
        
        Returns:
            RegionBuilder: Builder object for constructing regions
        
        Examples:
            >>> builder = createRegionBuilder(brain, "Cortex")
            >>> region_id = builder.withNeuronCount(100).withNeuronType(NeuronType.Excitatory).build()
        )pbdoc");

    m.def("createNeuronBuilder", [](std::shared_ptr<NeuralRegion> region) {
        return NeuronBuilder(region);
    }, py::arg("region"),
        R"pbdoc(Create neuron builder for fluent API construction)
        
        Args:
            region: Region to build neurons for
        
        Returns:
            NeuronBuilder: Builder object for constructing neurons
        )pbdoc");

    m.def("createSynapseBuilder", [](std::shared_ptr<NeuralRegion> region) {
        return SynapseBuilder(region);
    }, py::arg("region"),
        R"pbdoc(Create synapse builder for fluent API construction)
        
        Args:
            region: Region to build synapses for
        
        Returns:
            SynapseBuilder: Builder object for constructing synapses
        )pbdoc");

    // Context manager support for Brain
    py::class_<BrainSession>(m, "BrainSession", R"pbdoc(Brain simulation context manager)
        
        Provides automatic resource management for brain simulations.
        Use with 'with' statement for automatic cleanup.
        
        Examples:
            >>> brain = pynlm.Brain(config)
            >>> with brain.session() as session:
            ...     for step in range(100):
            ...         session.step()
            ...         action = session.produce_action()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", &BrainSession::__enter__)
        .def("__exit__", &BrainSession::__exit__, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"))
        .def("step", &BrainSession::step,
             R"pbdoc(Perform one simulation step)")
        .def("stepWithTime", &BrainSession::stepWithTime, py::arg("currentTime"),
             R"pbdoc(Perform one simulation step with time)")
        .def("produceAction", &BrainSession::produceAction,
             R"pbdoc(Produce motor action)
             
             Returns:
                 Action*: Motor action or nullptr if none
             )pbdoc")
        .def("getBrain", &BrainSession::getBrain,
             R"pbdoc(Get underlying brain reference)")
        .def("getTotalNeuronCount", &BrainSession::getTotalNeuronCount,
             R"pbdoc(Get total neuron count)")
        .def("getTotalSynapseCount", &BrainSession::getTotalSynapseCount,
             R"pbdoc(Get total synapse count)")
        .def("getAverageFiringRate", &BrainSession::getAverageFiringRate,
             R"pbdoc(Get average firing rate)")
        .def("getExcitationInhibitionRatio", &BrainSession::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition ratio)")
        .def("getTotalSpikeCount", &BrainSession::getTotalSpikeCount,
             R"pbdoc(Get total spike count)")
        .def("getActiveNeuronCount", &BrainSession::getActiveNeuronCount,
             R"pbdoc(Get active neuron count)")
        .def("getFiringNeuronCount", &BrainSession::getFiringNeuronCount,
             R"pbdoc(Get firing neuron count)")
        .def("getNeuromodulationLevel", &BrainSession::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)")
        .def("getCuriosityLevel", &BrainSession::getCuriosityLevel,
             R"pbdoc(Get curiosity level)")
        .def("getNoveltyLevel", &BrainSession::getNoveltyLevel,
             R"pbdoc(Get novelty level)")
        .def("getPredictionError", &BrainSession::getPredictionError,
             R"pbdoc(Get prediction error)")
        .def("getDevelopmentalStage", &BrainSession::getDevelopmentalStage,
             R"pbdoc(Get developmental stage)");

    // Brain class with enhanced interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain)
        
        The main brain class coordinating all neural simulation components.
        Supports spiking neuron models, plasticity, neuromodulation, development,
        and memory systems.
        
        Attributes:
            config: Brain configuration
        
        Examples:
            >>> brain = pynlm.Brain(config)
            >>> brain.initialize()
            >>> with brain.session() as session:
            ...     for step in range(100):
            ...         session.step()
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize brain with configuration)
             
             Returns:
                 bool: True if successful, False otherwise
             )pbdoc")
        .def("step", [](Brain& self, SimulationStep currentStep) {
            self.step(currentStep);
        }, py::arg("currentStep"),
             R"pbdoc(Perform a simulation step)
             
             Args:
                 currentStep: Current simulation step
             )pbdoc")
        .def("stepWithTime", [](Brain& self, SimulationStep currentStep, Timestamp currentTime) {
            self.step(currentStep, currentTime);
        }, py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp)
             
             Args:
                 currentStep: Current simulation step
                 currentTime: Current simulation time
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Receive sensory input)
             
             Args:
                 input: Sensory input object
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into specific neuron)
             
             Args:
                 neuron: Target neuron ID
                 current: Current to inject (nA)
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current to all neurons of a type)
             
             Args:
                 type: Neuron type
                 current: Current to inject (nA)
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action)
             
             Returns:
                 Action*: Motor action or nullptr if none
             )pbdoc")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"),
             R"pbdoc(Apply neuromodulatory signal)
             
             Args:
                 signal: Neuromodulator signal
             )pbdoc")
        .def("updatePlasticity", &Brain::updatePlasticity,
             R"pbdoc(Update plasticity rules)")
        .def("develop", &Brain::develop,
             R"pbdoc(Apply developmental changes)")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state)")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file)
             
             Args:
                 filepath: Path to save file
             
             Returns:
                 bool: True if successful, False otherwise
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file)
             
             Args:
                 filepath: Path to load file
             
             Returns:
                 bool: True if successful, False otherwise
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add neural region)
             
             Args:
                 name: Region name (optional)
             
             Returns:
                 RegionId: Identifier for new region
             )pbdoc")
        .def("getRegion", [](Brain& self, RegionId id) {
            return self.getRegion(id);
        }, py::arg("id"),
             R"pbdoc(Get region by ID)
             
             Args:
                 id: Region identifier
             
             Returns:
                 NeuralRegion*: Pointer to region, or nullptr if not found
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get number of regions)
             
             Returns:
                 int: Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs)
             
             Returns:
                 List of region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             R"pbdoc(Get all regions)
             
             Returns:
                 List of region pointers
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count)
             
             Returns:
                 int: Total number of neurons
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count)
             
             Returns:
                 int: Total number of synapses
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get active neuron count)
             
             Returns:
                 int: Number of active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get firing neuron count)
             
             Returns:
                 int: Number of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate)
             
             Returns:
                 float: Average firing rate (Hz)
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition ratio)
             
             Returns:
                 float: Excitation/inhibition ratio
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count)
             
             Returns:
                 int: Total number of spikes
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get developmental stage)
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(Set developmental stage)
             
             Args:
                 stage: New developmental stage
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             R"pbdoc(Get configuration)
             
             Returns:
                 std::shared_ptr<const Config>: Configuration
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status)")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             R"pbdoc(Get spike system)
             
             Returns:
                 SpikeSystem*: Spike system pointer
             )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             R"pbdoc(Get STDP plasticity)
             
             Returns:
                 STDP*: STDP pointer
             )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             R"pbdoc(Get Hebbian plasticity)
             
             Returns:
                 Hebbian*: Hebbian pointer
             )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             R"pbdoc(Get structural plasticity)
             
             Returns:
                 StructuralPlasticity*: Structural plasticity pointer
             )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(Get working memory)
             
             Returns:
                 NeuralWorkingMemory*: Working memory pointer
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(Get episodic memory)
             
             Returns:
                 NeuralEpisodicMemory*: Episodic memory pointer
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(Get associative memory)
             
             Returns:
                 NeuralAssociativeMemory*: Associative memory pointer
             )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(Get prediction system)
             
             Returns:
                 PredictionSystem*: Prediction system pointer
             )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(Get neural planner)
             
             Returns:
                 NeuralPlanner*: Neural planner pointer
             )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(Get concept formation)
             
             Returns:
                 ConceptFormation*: Concept formation pointer
             )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(Get attentional selection)
             
             Returns:
                 AttentionalSelection*: Attentional selection pointer
             )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(Get development system)
             
             Returns:
                 DevelopmentSystem*: Development system pointer
             )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(Get dopamine neuromodulator)
             
             Returns:
                 Dopamine*: Dopamine pointer
             )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(Get curiosity neuromodulator)
             
             Returns:
                 Curiosity*: Curiosity pointer
             )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(Get novelty neuromodulator)
             
             Returns:
                 Novelty*: Novelty pointer
             )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(Get prediction error signal)
             
             Returns:
                 PredictionError*: Prediction error signal pointer
             )pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             R"pbdoc(Get random generator)
             
             Returns:
                 RandomGenerator*: Random generator pointer
             )pbdoc")
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain neurons=" << brain.getTotalNeuronCount()
               << " synapses=" << brain.getTotalSynapseCount()
               << ">";
            return ss.str();
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)
        
        Connects NLM brain to world simulation with sensory and motor interfaces.
        
        Examples:
            >>> brain = pynlm.Brain(config)
            >>> agent_brain = pynlm.AgentBrain(brain)
            >>> agent_brain.initialize(world)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world)
             
             Args:
                 world: World instance
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size)
             
             Returns:
                 int: Expected sensory input size
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size)
             
             Returns:
                 int: Expected motor output size
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain)
             
             Args:
                 percept: Sensory percept
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command)
             
             Returns:
                 Action*: Motor command
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, 
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation)
             
             Args:
                 reward: Actual reward
                 predictedReward: Predicted reward
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system)
             
             Args:
                 timestep: Simulation timestep
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)
             
             Returns:
                 float: Neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)
             
             Returns:
                 float: Curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)
             
             Returns:
                 float: Novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)
             
             Returns:
                 float: Prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode)")
        .def("getBrain", &AgentBrain::getBrain,
             R"pbdoc(Get underlying brain)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc(Enable/disable development)")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc(Enable/disable curiosity)")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled)")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled)")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled)")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled)")
        .def("__repr__", [](const AgentBrain& agent) {
            std::stringstream ss;
            ss << "<AgentBrain>";
            return ss.str();
        });

    // DevelopmentSystem class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system)
        
        Manages neural development including developmental stages,
        plasticity modulation, and structural changes.
        
        Attributes:
            stage: Current developmental stage
        )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             R"pbdoc(Get current developmental stage)
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             R"pbdoc(Set developmental stage)
             
             Args:
                 stage: New developmental stage
             )pbdoc")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             R"pbdoc(Advance to next developmental stage)")
        .def("getStageName", &DevelopmentSystem::getStageName,
             R"pbdoc(Get stage name)
             
             Returns:
                 str: Stage name
             )pbdoc")
        .def("update", [](DevelopmentSystem& self, class Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"),
             R"pbdoc(Update development (simple version))
             
             Args:
                 brain: Brain reference
                 currentStep: Current simulation step
             )pbdoc")
        .def("updateWithParams", [](DevelopmentSystem& self, class Brain* brain, class RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             R"pbdoc(Update development with full parameters)
             
             Args:
                 brain: Brain reference
                 rng: Random generator
                 dt: Time step duration
             )pbdoc")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             R"pbdoc(Get plasticity modifier for current stage)
             
             Returns:
                 float: Plasticity modifier
             )pbdoc")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             R"pbdoc(Check if in critical period)
             
             Returns:
                 bool: True if in critical period
             )pbdoc")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             R"pbdoc(Get critical period progress)
             
             Returns:
                 float: Progress through critical period (0.0 to 1.0)
             )pbdoc")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             R"pbdoc(Get developmental age)
             
             Returns:
                 float: Developmental age
             )pbdoc")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             R"pbdoc(Set developmental age)
             
             Args:
                 age: Developmental age
             )pbdoc")
        .def("__repr__", [](const DevelopmentSystem& dev) {
            std::stringstream ss;
            ss << "<DevelopmentSystem stage=" << static_cast<int>(dev.getStage()) << ">";
            return ss.str();
        });

    // PredictionSystem class
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)
        
        Predicts future sensory states and consequences. Uses predictive coding
        to minimize prediction errors.
    )pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             R"pbdoc(Make prediction for next timestep)
             
             Args:
                 currentState: Current sensory input
             
             Returns:
                 SensoryInput*: Predicted next state
             )pbdoc")
        .def("updatePredictions", [](PredictionSystem& self, const SensoryInput& predicted, const SensoryInput& actual) {
            self.updatePredictions(predicted, actual);
        }, py::arg("predicted"), py::arg("actual"),
             R"pbdoc(Update predictions based on actual observation)
             
             Args:
                 predicted: Predicted sensory input
                 actual: Actual sensory input
             )pbdoc")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             R"pbdoc(Get prediction error)
             
             Returns:
                 float: Prediction error
             )pbdoc")
        .def("getConfidence", &PredictionSystem::getConfidence,
             R"pbdoc(Get prediction confidence)
             
             Returns:
                 float: Prediction confidence
             )pbdoc")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             R"pbdoc(Get error history)
             
             Returns:
                 List of prediction errors
             )pbdoc")
        .def("clearHistory", &PredictionSystem::clearHistory,
             R"pbdoc(Clear prediction history)")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             R"pbdoc(Train prediction model)
             
             Args:
                 observation: Training observation
             )pbdoc")
        .def("__repr__", [](const PredictionSystem& pred) {
            std::stringstream ss;
            ss << "<PredictionSystem error=" << pred.getPredictionError() << ">";
            return ss.str();
        });

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract neuromodulator)
        
        Base class for neuromodulatory signals that influence plasticity,
        learning, and behavior.
        
        Attributes:
            name: Neuromodulator name
            level: Current concentration/level
        )pbdoc")
        .def("getName", &Neuromodulator::getName,
             R"pbdoc(Get neuromodulator name)
             
             Returns:
                 str: Neuromodulator name
             )pbdoc")
        .def("getLevel", &Neuromodulator::getLevel,
             R"pbdoc(Get current level)
             
             Returns:
                 float: Current level
             )pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             R"pbdoc(Set current level)
             
             Args:
                 level: New level
             )pbdoc")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             R"pbdoc(Get plasticity factor)
             
             Returns:
                 float: Plasticity factor
             )pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             R"pbdoc(Update neuromodulator state)
             
             Args:
                 dt: Time step duration
             )pbdoc")
        .def("__repr__", [](const Neuromodulator& mod) {
            std::stringstream ss;
            ss << "<" << mod.getName() << " level=" << mod.getLevel() << ">";
            return ss.str();
        });

    // Dopamine class
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)
        
        Reward and reinforcement learning signal. High levels promote learning
        of rewarding behaviors, low levels reduce learning rate.
        
        Methods:
            signalReward: Signal reward event
            signalRewardPredictionError: Signal reward prediction error
        )pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             R"pbdoc(Signal reward event)
             
             Args:
                 reward: Reward value
             )pbdoc")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             R"pbdoc(Signal reward prediction error)
             
             Args:
                 error: Prediction error value
             )pbdoc")
        .def("__repr__", [](const Dopamine& dopamine) {
            std::stringstream ss;
            ss << "<Dopamine level=" << dopamine.getLevel() << ">";
            return ss.str();
        });

    // Curiosity class
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)
        
        Exploration motivation based on novelty and prediction error.
        High levels promote exploration of novel stimuli.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", [](Curiosity& self, class Brain* brain) {
            self.initialize(brain);
        }, py::arg("brain"),
             R"pbdoc(Initialize with brain reference)
             
             Args:
                 brain: Brain reference
             )pbdoc")
        .def("getLevel", &Curiosity::getLevel,
             R"pbdoc(Get curiosity level)
             
             Returns:
                 float: Curiosity level
             )pbdoc")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             R"pbdoc(Update curiosity based on novelty and prediction error)
             
             Args:
                 novelty: Novelty level
                 predictionError: Prediction error
                 dt: Time step duration
             )pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             R"pbdoc(Get exploration drive)
             
             Returns:
                 float: Exploration drive (same as level)
             )pbdoc")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             R"pbdoc(Set novelty weight)
             
             Args:
                 weight: Novelty weight
             )pbdoc")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             R"pbdoc(Set prediction error weight)
             
             Args:
                 weight: Prediction error weight
             )pbdoc")
        .def("reset", &Curiosity::reset,
             R"pbdoc(Reset curiosity)")
        .def("__repr__", [](const Curiosity& curiosity) {
            std::stringstream ss;
            ss << "<Curiosity level=" << curiosity.getLevel() << ">";
            return ss.str();
        });

    // Novelty class
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulator)
        
        Detects novelty from comparison with previous observations.
        High levels promote exploration of novel stimuli.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", [](Novelty& self, class Brain* brain) {
            self.initialize(brain);
        }, py::arg("brain"),
             R"pbdoc(Initialize with brain reference)
             
             Args:
                 brain: Brain reference
             )pbdoc")
        .def("getLevel", &Novelty::getLevel,
             R"pbdoc(Get novelty level)
             
             Returns:
                 float: Novelty level
             )pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             R"pbdoc(Set novelty level)
             
             Args:
                 level: Novelty level
             )pbdoc")
        .def("detectNovelty", [](Novelty& self, const Observation& observation, const Observation& previousObservation) {
            self.detectNovelty(observation, previousObservation);
        }, py::arg("observation"), py::arg("previousObservation"),
             R"pbdoc(Detect novelty from observation)
             
             Args:
                 observation: Current observation
                 previousObservation: Previous observation
             )pbdoc")
        .def("detectNoveltyPattern", [](Novelty& self, const std::vector<float>& currentPattern, const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"),
             R"pbdoc(Detect novelty from sensory input pattern)
             
             Args:
                 currentPattern: Current pattern
                 previousPattern: Previous pattern
             )pbdoc")
        .def("update", &Novelty::update, py::arg("dt"),
             R"pbdoc(Decay novelty over time)
             
             Args:
                 dt: Time step duration
             )pbdoc")
        .def("getHistory", &Novelty::getHistory,
             R"pbdoc(Get novelty history)
             
             Returns:
                 List of novelty levels
             )pbdoc")
        .def("clearHistory", &Novelty::clearHistory,
             R"pbdoc(Clear novelty history)")
        .def("__repr__", [](const Novelty& novelty) {
            std::stringstream ss;
            ss << "<Novelty level=" << novelty.getLevel() << ">";
            return ss.str();
        });

    // Reward class
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward neuromodulator)
        
        Accumulates reward signals for reinforcement learning.
        High levels promote learning of rewarding behaviors.
    )pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             R"pbdoc(Get current reward value)
             
             Returns:
                 float: Current reward value
             )pbdoc")
        .def("setValue", &Reward::setValue, py::arg("value"),
             R"pbdoc(Set current reward value)
             
             Args:
                 value: Reward value
             )pbdoc")
        .def("add", &Reward::add, py::arg("delta"),
             R"pbdoc(Accumulate reward)
             
             Args:
                 delta: Reward increment
             )pbdoc")
        .def("reset", &Reward::reset,
             R"pbdoc(Reset accumulated reward)")
        .def("computeReward", [](Reward& self, const Observation& observation) {
            return self.computeReward(observation);
        }, py::arg("observation"),
             R"pbdoc(Compute reward from observation)
             
             Args:
                 observation: Observation
             
             Returns:
                 float: Computed reward
             )pbdoc")
        .def("getHistory", &Reward::getHistory,
             R"pbdoc(Get reward history)
             
             Returns:
                 List of reward values
             )pbdoc")
        .def("clearHistory", &Reward::clearHistory,
             R"pbdoc(Clear reward history)")
        .def("__repr__", [](const Reward& reward) {
            std::stringstream ss;
            ss << "<Reward value=" << reward.getValue() << ">";
            return ss.str();
        });

    // PredictionError class
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)
        
        Computes difference between predicted and actual values.
        High levels indicate prediction failure and need for learning.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", [](PredictionError& self, class Brain* brain) {
            self.initialize(brain);
        }, py::arg("brain"),
             R"pbdoc(Initialize with brain reference)
             
             Args:
                 brain: Brain reference
             )pbdoc")
        .def("getError", &PredictionError::getError,
             R"pbdoc(Get error value)
             
             Returns:
                 float: Error value
             )pbdoc")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             R"pbdoc(Compute prediction error)
             
             Args:
                 predicted: Predicted value
                 actual: Actual value
             )pbdoc")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             R"pbdoc(Update prediction)
             
             Args:
                 newPrediction: New predicted value
             )pbdoc")
        .def("getHistory", &PredictionError::getHistory,
             R"pbdoc(Get error history)
             
             Returns:
                 List of error values
             )pbdoc")
        .def("clearHistory", &PredictionError::clearHistory,
             R"pbdoc(Clear error history)")
        .def("getMagnitude", &PredictionError::getMagnitude,
             R"pbdoc(Get error magnitude)
             
             Returns:
                 float: Error magnitude
             )pbdoc")
        .def("__repr__", [](const PredictionError& error) {
            std::stringstream ss;
            ss << "<PredictionError error=" << error.getError() << ">";
            return ss.str();
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class for all sensory modalities.
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(Get sensory input type)
             
             Returns:
                 int: Input type
             )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(Get raw data)
             
             Returns:
                 List of floats: Raw sensor data
             )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(Get data dimensions)
             
             Returns:
                 List of ints: Data dimensions
             )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(Get timestamp)
             
             Returns:
                 Timestamp: Data timestamp
             )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set timestamp)
             
             Args:
                 timestamp: New timestamp
             )pbdoc");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Processes visual input from environment.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set visual data)
             
             Args:
                 data: Image data as flattened array
             )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get image width)
             
             Returns:
                 int: Image width
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get image height)
             
             Returns:
                 int: Image height
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of channels)
             
             Returns:
                 int: Number of channels
             )pbdoc");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Processes audio input from environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data)
             
             Args:
                 data: Audio samples
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set sample rate)
             
             Args:
                 sampleRate: Sample rate in Hz
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get sample rate)
             
             Returns:
                 int: Sample rate
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of samples)
             
             Returns:
                 int: Number of audio samples
             )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Processes internal neural signals and states.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add internal signal)
             
             Args:
                 value: Signal value
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all signals)");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Motor action)
        
        Represents an action that can be executed in the environment.
        
        Attributes:
            type: Action type
            parameters: Action parameters
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType,
             R"pbdoc(Get action type)
             
             Returns:
                 ActionType: Action type
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set action type)
             
             Args:
                 type: Action type
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get action parameters)
             
             Returns:
                 List of floats: Action parameters
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters)
             
             Args:
                 params: Action parameters
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get action name)
             
             Returns:
                 str: Action name
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Clone action)
             
             Returns:
                 Action*: Cloned action
             )pbdoc");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)
        
        Represents an object in the simulated world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents physical state of agent in world.
        
        Attributes:
            x: X position
            y: Y position
            orientation: Heading angle (radians)
            velocityX: X velocity
            velocityY: Y velocity
            angularVelocity: Angular velocity
            energy: Energy level
            health: Health level
            age: Age in simulation steps
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result)
        
        Result of executing an action in the world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept)
        
        Combined sensory input from all modalities.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world)
        
        Simple 2D world simulation for NLM testing and development.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions)
             
             Args:
                 width: World width in pixels
                 height: World height in pixels
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world state)")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world)
             
             Args:
                 timestep: Simulation timestep
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command)
             
             Args:
                 cmd: Motor command
                 currentTime: Current time
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get sensory percept)
             
             Returns:
                 SensoryPercept*: Sensory percept
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body)
             
             Returns:
                 AgentBody*: Agent body
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add world object)
             
             Args:
                 obj: World object
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position)
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid)
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             
             Returns:
                 bool: True if position is valid
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width)
             
             Returns:
                 int: World width
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height)
             
             Returns:
                 int: World height
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum energy)
             
             Returns:
                 float: Maximum energy
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum energy)
             
             Args:
                 e: Maximum energy
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate)
             
             Returns:
                 float: Energy decay rate
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate)
             
             Args:
                 r: Energy decay rate
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get simulation time)
             
             Returns:
                 float: Simulation time
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed)
             
             Args:
                 seed: Random seed
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed)
             
             Returns:
                 int: Random seed
             )pbdoc");

    // High-level API
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createVisionSensor", [](size_t width, size_t height, size_t channels) {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create vision sensor");

    m.def("createAudioSensor", [](size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create audio sensor");

    m.def("createInternalSensor", []() {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensor");

    m.def("createNeuromodulator", [](const std::string& type) {
        if (type == "dopamine") {
            return std::make_shared<Dopamine>();
        } else if (type == "curiosity") {
            return std::make_shared<Curiosity>();
        } else if (type == "novelty") {
            return std::make_shared<Novelty>();
        } else if (type == "reward") {
            return std::make_shared<Reward>();
        } else {
            throw ValidationError("Unknown neuromodulator type: " + type);
        }
    }, py::arg("type"), "Create neuromodulator");

    m.def("runSimulation", [](const std::shared_ptr<Brain>& brain, 
                            const std::shared_ptr<SimpleWorld>& world,
                            SimulationStep steps) {
        NLMApi::runSimulation(brain, world, steps);
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run complete simulation");

    m.def("analyzeNetwork", [](const std::shared_ptr<Brain>& brain) {
        DiagnosticTools::analyzeConnectivity(brain);
    }, py::arg("brain"), "Analyze neural network connectivity");

    m.def("checkIntegrity", [](const std::shared_ptr<Brain>& brain) {
        DiagnosticTools::validateIntegrity(brain);
    }, py::arg("brain"), "Validate network integrity");

    m.def("saveNetworkState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadNetworkState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register Python classes for introspection
    py::class_<RegionBuilder>(m, "RegionBuilder")
        .def(py::init<std::shared_ptr<Brain>, std::string>(), py::arg("brain"), py::arg("name"))
        .def("withNeuronCount", &RegionBuilder::withNeuronCount)
        .def("withNeuronType", &RegionBuilder::withNeuronType)
        .def("withConnectionProbability", &RegionBuilder::withConnectionProbability)
        .def("withInitialWeight", &RegionBuilder::withInitialWeight)
        .def("build", &RegionBuilder::build);

    py::class_<NeuronBuilder>(m, "NeuronBuilder")
        .def(py::init<std::shared_ptr<NeuralRegion>>(), py::arg("region"))
        .def("withType", &NeuronBuilder::withType)
        .def("withThreshold", &NeuronBuilder::withThreshold)
        .def("withRestingPotential", &NeuronBuilder::withRestingPotential)
        .def("withRefractoryPeriod", &NeuronBuilder::withRefractoryPeriod)
        .def("build", &NeuronBuilder::build);

    py::class_<DiagnosticTools>(m, "DiagnosticTools")
        .def_static("analyzeConnectivity", &DiagnosticTools::analyzeConnectivity)
        .def_static("validateIntegrity", &DiagnosticTools::validateIntegrity)
        .def_static("getMemoryUsage", &DiagnosticTools::getMemoryUsage);

    py::class_<NLMApi>(m, "NLMApi")
        .def_static("createConfig", &NLMApi::createConfig)
        .def_static("createBrain", [](std::shared_ptr<Config> config) { return NLMApi::createBrain(config); }, py::arg("config"))
        .def_static("createWorld", &NLMApi::createWorld)
        .def_static("createAgentBrain", [](std::shared_ptr<Brain> brain) { return NLMApi::createAgentBrain(brain); }, py::arg("brain"))
        .def_static("createVisionSensor", [](size_t width, size_t height, size_t channels) { return NLMApi::createVisionSensor(width, height, channels); }, py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def_static("createAudioSensor", [](size_t sampleRate, size_t numSamples) { return NLMApi::createAudioSensor(sampleRate, numSamples); }, py::arg("sampleRate"), py::arg("numSamples"))
        .def_static("createInternalSensor", &NLMApi::createInternalSensor)
        .def_static("createNeuromodulator", &NLMApi::createNeuromodulator)
        .def_static("runSimulation", &NLMApi::runSimulation);

    // Convenience functions
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return NLMApi::createBrain(config);
    }, py::arg("config"), "Create brain with configuration");

    m.def("createDefaultBrain", []() {
        return NLMApi::createBrain("DefaultBrain");
    }, "Create brain with default configuration");

    m.def("createDefaultWorld", []() {
        return NLMApi::createWorld();
    }, "Create world with default configuration");

    m.def("analyzeNetwork", [](const std::shared_ptr<Brain>& brain) {
        DiagnosticTools::analyzeConnectivity(brain);
    }, py::arg("brain"), "Analyze neural network connectivity");

    m.def("checkIntegrity", [](const std::shared_ptr<Brain>& brain) {
        DiagnosticTools::validateIntegrity(brain);
    }, py::arg("brain"), "Validate network integrity");

    m.def("saveNetworkState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadNetworkState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");
}

} // namespace nlm