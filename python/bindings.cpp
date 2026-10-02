#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Python-friendly wrapper classes and helper functions
class PyConfigWrapper {
public:
    PyConfigWrapper() : config(std::make_shared<Config>()) {}
    
    // Load from JSON file
    bool load_from_file(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    // Load from dictionary
    void load_from_dict(const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            // Try to parse as float first, then int
            try {
                float val = py::cast<float>(item.second);
                config->set(key, val, ConfigSource::CommandLine);
            } catch (...) {
                try {
                    int val = py::cast<int>(item.second);
                    config->set(key, val, ConfigSource::CommandLine);
                } catch (...) {
                    // Skip invalid values
                }
            }
        }
    }
    
    // Get value as Python type
    py::object get(const std::string& key) {
        if (!config->has(key)) {
            return py::none();
        }
        
        // Try to get as float first
        try {
            float val = config->getOr<float>(key, 0.0f);
            return py::cast(val);
        } catch (...) {
            // Try int
            try {
                int64_t val = config->getOr<int64_t>(key, 0);
                return py::cast(val);
            } catch (...) {
                // Return as string
                return py::cast(config->getOr<std::string>(key, ""));
            }
        }
    }
    
    // Set value
    void set(const std::string& key, float value) {
        config->set(key, value, ConfigSource::CommandLine);
    }
    
    void set(const std::string& key, int64_t value) {
        config->set(key, value, ConfigSource::CommandLine);
    }
    
    // Get all keys
    py::list get_keys() {
        py::list keys;
        auto configKeys = config->getKeys();
        for (const auto& key : configKeys) {
            keys.append(key);
        }
        return keys;
    }
    
    // Get summary
    std::string summary() const {
        return config->summary();
    }
    
    // Clear
    void clear() {
        config->clear();
    }
    
    // Create default config
    static std::shared_ptr<PyConfigWrapper> create_default() {
        return std::make_shared<PyConfigWrapper>();
    }
    
    // Get underlying config
    std::shared_ptr<Config> get_config() {
        return config;
    }
    
private:
    std::shared_ptr<Config> config;
};

class PyBrainWrapper {
public:
    PyBrainWrapper(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    bool initialize() {
        return brain->initialize();
    }
    
    void step(int currentStep, double currentTime = 0.0) {
        brain->step(currentStep, currentTime);
    }
    
    // Inject sensory input
    void receive_sensory_input(std::shared_ptr<SensoryInput> input) {
        brain->receiveSensoryInput(input);
    }
    
    // Inject current into a specific neuron
    void inject_current(NeuronId neuronId, float current) {
        brain->injectCurrent(neuronId, current);
    }
    
    // Inject current to all neurons of a type
    void inject_current_to_type(NeuronType type, float current) {
        brain->injectCurrentToNeurons(type, current);
    }
    
    // Produce action
    std::shared_ptr<Action> produce_action() {
        return brain->produceAction();
    }
    
    // Reset brain
    void reset() {
        brain->reset();
    }
    
    // Save brain state
    bool save(const std::string& filepath) {
        return brain->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Get brain statistics
    py::dict get_stats() {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        stats["prediction_error"] = brain->getPredictionError();
        return stats;
    }
    
    // Log status
    void log_status() {
        brain->logStatus();
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> get_brain() {
        return brain;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

class PySimpleWorldWrapper {
public:
    PySimpleWorldWrapper() : world(std::make_shared<SimpleWorld>()) {}
    
    void configure(int width, int height, int visionWidth, int visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world->reset();
    }
    
    void set_agent_start(float x, float y) {
        world->setAgentStart(x, y);
    }
    
    void update(float timestep) {
        world->update(timestep);
    }
    
    std::shared_ptr<SensoryPercept> get_sensory_percept() {
        return world->getSensoryPercept();
    }
    
    std::shared_ptr<AgentBody> get_agent_body() {
        return world->getAgentBody();
    }
    
    // Apply motor command
    ActionResult apply_motor_command(std::shared_ptr<Action> action, double currentTime) {
        return world->applyMotorCommand(*action, currentTime);
    }
    
    // Get width/height
    int get_width() const { return world->getWidth(); }
    int get_height() const { return world->getHeight(); }
    
    // Get simulation time
    double get_simulation_time() const { return world->getSimulationTime(); }
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> get_world() {
        return world;
    }
    
private:
    std::shared_ptr<SimpleWorld> world;
};

class PyAgentBrainWrapper {
public:
    PyAgentBrainWrapper(std::shared_ptr<Brain> brain) : agentBrain(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agentBrain->initialize(world);
    }
    
    // Process sensory input
    void process_sensory_input(std::shared_ptr<SensoryPercept> percept) {
        agentBrain->processSensoryInput(percept);
    }
    
    // Decode motor command
    std::shared_ptr<Action> decode_motor_command() {
        return agentBrain->decodeMotorCommand();
    }
    
    // Apply reward modulation
    void apply_reward_modulation(float reward, float predictedReward = 0.0f) {
        agentBrain->applyRewardModulation(reward, predictedReward);
    }
    
    // Update development
    void update_development(float timestep) {
        agentBrain->updateDevelopment(timestep);
    }
    
    // Get neuromodulation levels
    float get_neuromodulation_level() const { return agentBrain->getNeuromodulationLevel(); }
    float get_curiosity_level() const { return agentBrain->getCuriosityLevel(); }
    float get_novelty_level() const { return agentBrain->getNoveltyLevel(); }
    float get_prediction_error() const { return agentBrain->getPredictionError(); }
    
    // Get developmental stage
    int get_developmental_stage() const { return static_cast<int>(agentBrain->getDevelopmentalStage()); }
    
    // Enable/disable systems
    void enable_reward_modulation(bool enable) { agentBrain->enableRewardModulation(enable); }
    void enable_structural_plasticity(bool enable) { agentBrain->enableStructuralPlasticity(enable); }
    void enable_development(bool enable) { agentBrain->enableDevelopment(enable); }
    void enable_curiosity(bool enable) { agentBrain->enableCuriosity(enable); }
    
    // Check if systems are enabled
    bool is_reward_modulation_enabled() const { return agentBrain->isRewardModulationEnabled(); }
    bool is_structural_plasticity_enabled() const { return agentBrain->isStructuralPlasticityEnabled(); }
    bool is_development_enabled() const { return agentBrain->isDevelopmentEnabled(); }
    bool is_curiosity_enabled() const { return agentBrain->isCuriosityEnabled(); }
    
    // Get underlying agent brain
    std::shared_ptr<AgentBrain> get_agent_brain() {
        return agentBrain;
    }
    
    std::shared_ptr<Brain> get_brain() {
        return agentBrain->getBrain();
    }
    
private:
    std::shared_ptr<AgentBrain> agentBrain;
};

// Helper functions for Python
std::string format_stats(const py::dict& stats) {
    std::stringstream ss;
    ss << "Brain Statistics:" << std::endl;
    ss << "  Neuron count: " << stats["neuron_count"].cast<int>() << std::endl;
    ss << "  Synapse count: " << stats["synapse_count"].cast<int>() << std::endl;
    ss << "  Active neurons: " << stats["active_neurons"].cast<int>() << std::endl;
    ss << "  Firing neurons: " << stats["firing_neurons"].cast<int>() << std::endl;
    ss << "  Average firing rate: " << stats["average_firing_rate"].cast<float>() << std::endl;
    ss << "  E/I ratio: " << stats["excitatory_inhibitory_ratio"].cast<float>() << std::endl;
    ss << "  Total spikes: " << stats["total_spikes"].cast<int>() << std::endl;
    return ss.str();
}

std::string neuron_type_to_string(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

std::string synapse_type_to_string(SynapseType type) {
    switch (type) {
        case SynapseType::Excitatory: return "Excitatory";
        case SynapseType::Inhibitory: return "Inhibitory";
        case SynapseType::Modulatory: return "Modulatory";
        case SynapseType::Electrical: return "Electrical";
        case SynapseType::GapJunction: return "GapJunction";
        default: return "Unknown";
    }
}

std::string developmental_stage_to_string(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

std::string action_type_to_string(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        An experimental artificial developmental brain implemented in Python.
        
        This module provides Python access to the NLM (熙然) neural simulation framework,
        allowing you to create, simulate, and interact with artificial neural brains.
        
        Key Features:
        - Brain simulation with spiking neurons
        - Memory systems (working, episodic, semantic, procedural, associative)
        - Neuromodulation (dopamine, curiosity, novelty detection)
        - Prediction and learning systems
        - Developmental processes
        - Action selection and world interaction
        - Configurable simulation parameters
        
        Usage Examples:
        
        Basic Brain Simulation:
        ```python
        import pynlm
        
        # Create a brain with configuration
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Initialize the brain
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step, step * 0.001)
        
        # Get statistics
        stats = brain.get_stats()
        print(f"Neurons: {stats['neuron_count']}")
        print(f"Total spikes: {stats['total_spikes']}")
        ```
        
        Interactive Agent with World:
        ```python
        import pynlm
        
        # Create components
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        agent_brain = pynlm.create_agent_brain(brain)
        
        # Initialize
        agent_brain.initialize(world)
        world.set_agent_start(10.0, 10.0)
        
        # Run episode
        for step in range(500):
            # Get sensory input from world
            percept = world.get_sensory_percept()
            
            # Process in agent brain
            agent_brain.process_sensory_input(percept)
            
            # Produce action
            action = agent_brain.decode_motor_command()
            
            # Apply to world
            result = world.apply_motor_command(action, world.get_simulation_time())
            
            # Apply reward modulation
            agent_brain.apply_reward_modulation(result.reward)
            
            # Update world
            world.update(0.01)
            
            # Update development
            agent_brain.update_development(0.01)
        ```
        
        Memory and Learning:
        ```python
        import pynlm
        
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        agent_brain = pynlm.create_agent_brain(brain)
        
        # Enable advanced learning systems
        agent_brain.enable_reward_modulation(True)
        agent_brain.enable_structural_plasticity(True)
        agent_brain.enable_development(True)
        agent_brain.enable_curiosity(True)
        
        # Run learning episode
        for step in range(1000):
            brain.step(step, step * 0.001)
            
            # Check for curiosity-driven exploration
            if agent_brain.get_curiosity_level() > 0.5:
                print("Agent is exploring due to curiosity!")
        ```
        
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Configure Python exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // NeuronId class
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
        });

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class (Python-friendly wrapper)
    py::class_<PyConfigWrapper>(m, "Config", R"pbdoc(Python-friendly configuration wrapper)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &PyConfigWrapper::load_from_file, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_dict", &PyConfigWrapper::load_from_dict, py::arg("dict"),
             "Load configuration from a Python dictionary")
        .def("get", &PyConfigWrapper::get, py::arg("key"),
             "Get configuration value by key")
        .def("set", py::overload_cast<const std::string&, float>(&PyConfigWrapper::set),
             py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", py::overload_cast<const std::string&, int64_t>(&PyConfigWrapper::set),
             py::arg("key"), py::arg("value"), "Set configuration integer value")
        .def("get_keys", &PyConfigWrapper::get_keys,
             "Get all configuration keys")
        .def("summary", &PyConfigWrapper::summary,
             "Get configuration summary")
        .def("clear", &PyConfigWrapper::clear,
             "Clear all configuration entries")
        .def("__repr__", [](const PyConfigWrapper& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Create convenience functions for Config
    m.def("create_default_config", &PyConfigWrapper::create_default,
          "Create a default configuration");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SimpleWorld class (Python-friendly wrapper)
    py::class_<PySimpleWorldWrapper>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &PySimpleWorldWrapper::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &PySimpleWorldWrapper::reset)
        .def("set_agent_start", &PySimpleWorldWrapper::set_agent_start,
             py::arg("x"), py::arg("y"))
        .def("update", &PySimpleWorldWrapper::update, py::arg("timestep"))
        .def("apply_motor_command", &PySimpleWorldWrapper::apply_motor_command,
             py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &PySimpleWorldWrapper::get_sensory_percept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &PySimpleWorldWrapper::get_agent_body,
             py::return_value_policy::reference_internal)
        .def("add_object", &PySimpleWorldWrapper::add_object, py::arg("obj"))
        .def("remove_object", &PySimpleWorldWrapper::remove_object,
             py::arg("x"), py::arg("y"))
        .def("is_valid_position", &PySimpleWorldWrapper::is_valid_position,
             py::arg("x"), py::arg("y"))
        .def("get_width", &PySimpleWorldWrapper::get_width)
        .def("get_height", &PySimpleWorldWrapper::get_height)
        .def("get_max_energy", &PySimpleWorldWrapper::get_max_energy)
        .def("set_max_energy", &PySimpleWorldWrapper::set_max_energy, py::arg("e"))
        .def("get_energy_decay_rate", &PySimpleWorldWrapper::get_energy_decay_rate)
        .def("set_energy_decay_rate", &PySimpleWorldWrapper::set_energy_decay_rate,
             py::arg("r"))
        .def("get_simulation_time", &PySimpleWorldWrapper::get_simulation_time)
        .def("set_random_seed", &PySimpleWorldWrapper::set_random_seed, py::arg("seed"))
        .def("get_random_seed", &PySimpleWorldWrapper::get_random_seed);

    // Create convenience functions for SimpleWorld
    m.def("create_simple_world", []() {
        return std::make_shared<PySimpleWorldWrapper>();
    }, "Create a new simple world");

    // Brain class (Python-friendly wrapper)
    py::class_<PyBrainWrapper>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &PyBrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrainWrapper::*)(int)>(&PyBrainWrapper::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (PyBrainWrapper::*)(int, double)>(&PyBrainWrapper::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &PyBrainWrapper::receive_sensory_input,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &PyBrainWrapper::inject_current,
             py::arg("neuronId"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_type", &PyBrainWrapper::inject_current_to_type,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &PyBrainWrapper::produce_action,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrainWrapper::reset, "Reset brain state")
        .def("save", &PyBrainWrapper::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrainWrapper::load, py::arg("filepath"),
             "Load brain state from file")
        .def("get_stats", &PyBrainWrapper::get_stats,
             "Get brain statistics as a dictionary")
        .def("log_status", &PyBrainWrapper::log_status,
             "Log brain status")
        .def("__repr__", [](const PyBrainWrapper& brain) {
            auto stats = brain.get_stats();
            return "<Brain: " + std::to_string(stats["neuron_count"].cast<int>()) +
                   " neurons, " + std::to_string(stats["synapse_count"].cast<int>()) +
                   " synapses>";
        });

    // Create convenience functions for Brain
    m.def("create_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<PyBrainWrapper>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // AgentBrain class (Python-friendly wrapper)
    py::class_<PyAgentBrainWrapper>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &PyAgentBrainWrapper::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &PyAgentBrainWrapper::get_sensory_input_size,
             "Get expected sensory input size")
        .def("get_motor_output_size", &PyAgentBrainWrapper::get_motor_output_size,
             "Get expected motor output size")
        .def("process_sensory_input", &PyAgentBrainWrapper::process_sensory_input,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &PyAgentBrainWrapper::decode_motor_command,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &PyAgentBrainWrapper::apply_reward_modulation,
             py::arg("reward"), py::arg("predictedReward") = 0.0f,
             "Apply reward-based neuromodulation")
        .def("update_development", &PyAgentBrainWrapper::update_development,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &PyAgentBrainWrapper::get_developmental_stage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &PyAgentBrainWrapper::get_neuromodulation_level,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &PyAgentBrainWrapper::get_curiosity_level,
             "Get curiosity level")
        .def("get_novelty_level", &PyAgentBrainWrapper::get_novelty_level,
             "Get novelty level")
        .def("get_prediction_error", &PyAgentBrainWrapper::get_prediction_error,
             "Get prediction error")
        .def("reset", &PyAgentBrainWrapper::reset, "Reset agent for new episode")
        .def("enable_reward_modulation", &PyAgentBrainWrapper::enable_reward_modulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &PyAgentBrainWrapper::enable_structural_plasticity,
             py::arg("enable"))
        .def("enable_development", &PyAgentBrainWrapper::enable_development,
             py::arg("enable"))
        .def("enable_curiosity", &PyAgentBrainWrapper::enable_curiosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &PyAgentBrainWrapper::is_reward_modulation_enabled)
        .def("is_structural_plasticity_enabled", &PyAgentBrainWrapper::is_structural_plasticity_enabled)
        .def("is_development_enabled", &PyAgentBrainWrapper::is_development_enabled)
        .def("is_curiosity_enabled", &PyAgentBrainWrapper::is_curiosity_enabled);

    // Create convenience functions for AgentBrain
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<PyAgentBrainWrapper>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Helper functions
    m.def("format_stats", &format_stats,
          "Format brain statistics as a readable string",
          py::arg("stats"));

    m.def("neuron_type_to_string", &neuron_type_to_string,
          "Convert NeuronType to string", py::arg("type"));

    m.def("synapse_type_to_string", &synapse_type_to_string,
          "Convert SynapseType to string", py::arg("type"));

    m.def("developmental_stage_to_string", &developmental_stage_to_string,
          "Convert DevelopmentalStage to string", py::arg("stage"));

    m.def("action_type_to_string", &action_type_to_string,
          "Convert ActionType to string", py::arg("type"));

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Example configuration for quick setup
    m.def("get_example_config", []() {
        auto config = std::make_shared<PyConfigWrapper>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42u);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Get an example configuration");
    
    // Quick simulation helper
    m.def("run_quick_simulation", [](int steps = 1000, double timestep = 0.001) {
        auto config = std::make_shared<PyConfigWrapper>();
        config->set("neuron_count", 200);
        config->set("region_count", 1);
        config->set("simulation_timestep", timestep);
        
        auto brain = std::make_shared<PyBrainWrapper>(config);
        if (brain->initialize()) {
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * timestep);
            }
            return brain->get_stats();
        }
        return py::dict();
    }, py::arg("steps") = 1000, py::arg("timestep") = 0.001,
       "Run a quick simulation and return statistics");
}

} // namespace nlm
