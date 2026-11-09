#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

// Helper functions for Python bindings
namespace py_helpers {

// Convert text to a neural representation (Phase 6)
inline std::vector<NeuronId> encode_text(const std::string& text) {
    return AgentBrain::encodeText(text);
}

// Create a default vision input
inline std::unique_ptr<Vision> create_vision(int width, int height, int channels = 3) {
    return std::make_unique<Vision>(width, height, channels);
}

// Create a default audio input
inline std::unique_ptr<Audio> create_audio(size_t sampleRate, size_t numSamples) {
    return std::make_unique<Audio>(sampleRate, numSamples);
}

// Create internal signals
inline std::unique_ptr<InternalSignals> create_internal_signals() {
    return std::make_unique<InternalSignals>();
}

// Create action from type and parameters
inline std::unique_ptr<Action> create_action(ActionType type, const std::vector<float>& params = {}) {
    return std::make_unique<Action>(type, params);
}

// Create world object
inline WorldObject create_world_object(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
    return WorldObject{x, y, type, value, radius, true};
}

// Create agent body
inline AgentBody create_agent_body(float x, float y, float orientation = 0.0f, float velocityX = 0.0f, float velocityY = 0.0f, float angularVelocity = 0.0f, float energy = 100.0f, float health = 1.0f, float age = 0.0f) {
    AgentBody body;
    body.x = x;
    body.y = y;
    body.orientation = orientation;
    body.velocityX = velocityX;
    body.velocityY = velocityY;
    body.angularVelocity = angularVelocity;
    body.energy = energy;
    body.health = health;
    body.age = age;
    body.isMoving = false;
    body.isTurning = false;
    body.lastActionTime = 0.0f;
    return body;
}

// Create action result
inline ActionResult create_action_result(float reward, bool success, const std::string& message = "") {
    return ActionResult{reward, success, message};
}

// Create sensory percept
inline SensoryPercept create_sensory_percept() {
    return SensoryPercept();
}

} // namespace py_helpers

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Real spiking neural computation with event-driven dynamics
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Cognitive systems (planning, concept formation, attention)
        - Developmental stages affecting plasticity
        - Advanced plasticity rules (STDP, Hebbian, structural)
        - Checkpoint saving and loading for persistence
        - Full Python integration with rich API
        
        Usage Example:
        ```python
        import pynlm
        
        # Create configuration
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
            
        # Save brain state
        brain.save("brain_checkpoint.bin")
        
        # Load brain state
        brain2 = pynlm.createBrain(config)
        brain2.load("brain_checkpoint.bin")
        ```
    )pbdoc";

    // Register Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // ID types - grouped together for better organization
    {
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
    }
    
    {
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
            })
            .def("__str__", [](const SynapseId& id) {
                return std::to_string(id.value);
            });
    }
    
    {
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
            })
            .def("__str__", [](const RegionId& id) {
                return std::to_string(id.value);
            });
    }
    
    {
        py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &PopulationId::value)
            .def("index", &PopulationId::index)
            .def("__eq__", &PopulationId::operator==)
            .def("__ne__", &PopulationId::operator!=);
    }

    // Enums
    {
        py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
            .value("Excitatory", NeuronType::Excitatory)
            .value("Inhibitory", NeuronType::Inhibitory)
            .value("Modulatory", NeuronType::Modulatory)
            .value("Sensory", NeuronType::Sensory)
            .value("Motor", NeuronType::Motor)
            .value("Internal", NeuronType::Internal)
            .export_values();
    }
    
    {
        py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
            .value("Excitatory", SynapseType::Excitatory)
            .value("Inhibitory", SynapseType::Inhibitory)
            .value("Modulatory", SynapseType::Modulatory)
            .value("Electrical", SynapseType::Electrical)
            .value("GapJunction", SynapseType::GapJunction)
            .export_values();
    }
    
    {
        py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
            .value("Initial", DevelopmentalStage::Initial)
            .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
            .value("Maturation", DevelopmentalStage::Maturation)
            .value("Adult", DevelopmentalStage::Adult)
            .value("Aging", DevelopmentalStage::Aging)
            .export_values();
    }
    
    {
        py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
            .value("Resting", FiringState::Resting)
            .value("Active", FiringState::Active)
            .value("Refractory", FiringState::Refractory)
            .value("Inhibited", FiringState::Inhibited)
            .export_values();
    }
    
    {
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
    }
    
    {
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
    }
    
    {
        py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
            .value("Empty", WorldObjectType::Empty)
            .value("Resource", WorldObjectType::Resource)
            .value("Hazard", WorldObjectType::Hazard)
            .value("Wall", WorldObjectType::Wall)
            .value("Marker", WorldObjectType::Marker)
            .export_values();
    }

    // Configuration class with enhanced Python interface
    {
        py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
            .def(py::init<>())
            .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
                 "Load configuration from a JSON file")
            .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
                std::vector<char*> argv;
                argv.reserve(args.size());
                for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
                return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
            }, py::arg("args"),
                 "Load configuration from command line arguments")
            .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
                 "Save configuration to a JSON file")
            .def("has", &Config::has, py::arg("key"),
                 "Check if a configuration key exists")
            .def("getKeys", &Config::getKeys,
                 "Get all configuration keys")
            .def("clear", &Config::clear,
                 "Clear all configuration entries")
            .def("summary", &Config::summary,
                 "Get a summary string of the configuration")
            .def("set", [](Config& self, const std::string& key, const std::string& value) {
                self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set configuration value (string)")
            .def("set", [](Config& self, const std::string& key, int value) {
                self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set configuration value (int)")
            .def("set", [](Config& self, const std::string& key, int64_t value) {
                self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set configuration value (int64_t)")
            .def("set", [](Config& self, const std::string& key, double value) {
                self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set configuration value (double)")
            .def("set", [](Config& self, const std::string& key, bool value) {
                self.set(key, value);
            }, py::arg("key"), py::arg("value"),
                 "Set configuration value (bool)")
            .def("get", &Config::get<std::string>, py::arg("key"),
                 "Get configuration value as string")
            .def("get", &Config::get<int64_t>, py::arg("key"),
                 "Get configuration value as int64_t")
            .def("get", &Config::get<double>, py::arg("key"),
                 "Get configuration value as double")
            .def("get", &Config::get<bool>, py::arg("key"),
                 "Get configuration value as bool")
            .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
                return self.getOr<std::string>(key, defaultValue);
            }, py::arg("key"), py::arg("default"),
                 "Get configuration value or default")
            .def("remove", &Config::remove, py::arg("key"),
                 "Remove configuration key")
            .def("__repr__", [](const Config& cfg) {
                return "<Config: " + cfg.summary() + ">";
            })
            .def("__str__", [](const Config& cfg) {
                return cfg.summary();
            });
    }

    // Sensory input classes
    {
        py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
            .def("getType", &SensoryInput::getType, "Get the type of sensory input")
            .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
            .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
            .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
            .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
                 "Set the timestamp");
    }
    
    {
        py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
                 py::arg("channels") = 3)
            .def("setData", [](Vision& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("getWidth", &Vision::getWidth)
            .def("getHeight", &Vision::getHeight)
            .def("getChannels", &Vision::getChannels);
    }
    
    {
        py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            .def("setData", [](Audio& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
            .def("getSampleRate", &Audio::getSampleRate)
            .def("getNumSamples", &Audio::getNumSamples);
    }
    
    {
        py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
            .def(py::init<>())
            .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
            .def("clearSignals", &InternalSignals::clearSignals);
    }

    // Action classes
    {
        py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
            .def(py::init<>())
            .def(py::init<ActionType>(), py::arg("type"))
            .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
            .def("getType", &Action::getType)
            .def("setType", &Action::setType, py::arg("type"))
            .def("getParameters", &Action::getParameters)
            .def("setParameters", &Action::setParameters, py::arg("params"))
            .def("getName", &Action::getName)
            .def("clone", &Action::clone)
            .def_static("create", py_helpers::create_action, py::arg("type"), py::arg("params") = std::vector<float>(),
                       "Create action from type and parameters");
    }

    // World object
    {
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
            .def_readwrite("active", &WorldObject::active)
            .def_static("create", py_helpers::create_world_object,
                       py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
                       py::arg("radius") = 0.5f,
                       "Create world object");
    }

    // Agent body
    {
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
            .def("reset", &AgentBody::reset)
            .def_static("create", py_helpers::create_agent_body,
                       py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("orientation") = 0.0f,
                       py::arg("velocityX") = 0.0f, py::arg("velocityY") = 0.0f,
                       py::arg("angularVelocity") = 0.0f, py::arg("energy") = 100.0f,
                       py::arg("health") = 1.0f, py::arg("age") = 0.0f,
                       "Create agent body with specified parameters");
    }

    // Action result
    {
        py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
            .def(py::init<>())
            .def(py::init<float, bool, std::string>(),
                 py::arg("reward"), py::arg("success"), py::arg("message") = "")
            .def_readwrite("reward", &ActionResult::reward)
            .def_readwrite("success", &ActionResult::success)
            .def_readwrite("message", &ActionResult::message)
            .def_static("create", py_helpers::create_action_result,
                       py::arg("reward"), py::arg("success"), py::arg("message") = "",
                       "Create action result");
    }

    // Sensory percept
    {
        py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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
            .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
            .def_static("create", py_helpers::create_sensory_percept,
                       "Create empty sensory percept");
    }

    // Sensory factory functions
    {
        m.def("createVision", py_helpers::create_vision,
              py::arg("width"), py::arg("height"), py::arg("channels") = 3,
              "Create vision sensory input");
        m.def("createAudio", py_helpers::create_audio,
              py::arg("sampleRate"), py::arg("numSamples"),
              "Create audio sensory input");
        m.def("createInternalSignals", py_helpers::create_internal_signals,
              "Create internal signals sensory input");
    }

    // Simple world
    {
        py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
            .def(py::init<>())
            .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
                 py::arg("visionWidth"), py::arg("visionHeight"),
                 "Configure world dimensions")
            .def("reset", &SimpleWorld::reset, "Reset world state")
            .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
                 "Set agent starting position")
            .def("update", &SimpleWorld::update, py::arg("timestep"),
                 "Update world state")
            .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
                 py::arg("cmd"), py::arg("currentTime"),
                 "Apply motor command to world")
            .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
                 py::return_value_policy::reference_internal,
                 "Get current sensory percept")
            .def("getAgentBody", &SimpleWorld::getAgentBody,
                 py::return_value_policy::reference_internal,
                 "Get agent body state")
            .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
                 "Add object to world")
            .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
                 "Remove object from world")
            .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
                 "Check if position is valid")
            .def("getWidth", &SimpleWorld::getWidth, "Get world width")
            .def("getHeight", &SimpleWorld::getHeight, "Get world height")
            .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
            .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
                 "Set maximum energy")
            .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
                 "Get energy decay rate")
            .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
                 "Set energy decay rate")
            .def("getSimulationTime", &SimpleWorld::getSimulationTime,
                 "Get current simulation time")
            .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
                 "Set random seed")
            .def("getRandomSeed", &SimpleWorld::getRandomSeed,
                 "Get random seed");
    }

    // Factory functions
    {
        m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration");
        
        m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration");
        
        m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world");
        
        m.def("encodeText", py_helpers::encode_text,
              py::arg("text"), "Encode text into neural pattern (Phase 6)");
    }

    // Brain class with enhanced Python interface
    {
        py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
            .def("injectCurrent", &Brain::injectCurrent,
                 py::arg("neuron"), py::arg("current"),
                 "Inject current into a specific neuron")
            .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
                 py::arg("type"), py::arg("current"),
                 "Inject current into all neurons of a specific type")
            .def("produceAction", &Brain::produceAction,
                 "Produce motor action based on neural activity")
            .def("reset", &Brain::reset,
                 "Reset brain state")
            .def("save", &Brain::save, py::arg("filepath"),
                 "Save brain state to file")
            .def("load", &Brain::load, py::arg("filepath"),
                 "Load brain state from file")
            .def("addRegion", &Brain::addRegion, py::arg("name") = "",
                 "Add a new neural region")
            .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
                 py::arg("id"),
                 py::return_value_policy::reference_internal,
                 "Get a region by ID")
            .def("getRegionCount", &Brain::getRegionCount,
                 "Get the number of regions")
            .def("getRegionIds", &Brain::getRegionIds,
                 "Get all region IDs")
            .def("getRegions", &Brain::getRegions,
                 py::return_value_policy::reference_internal,
                 "Get all regions")
            .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
                 "Get total neuron count across all regions")
            .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
                 "Get total synapse count across all regions")
            .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
                 "Get count of active neurons")
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
            .def("getConfig", &Brain::getConfig,
                 py::return_value_policy::reference_internal,
                 "Get the configuration")
            .def("logStatus", &Brain::logStatus,
                 "Log brain status")
            .def("getWorkingMemory", &Brain::getWorkingMemory,
                 py::return_value_policy::reference_internal,
                 "Get working memory system")
            .def("getEpisodicMemory", &Brain::getEpisodicMemory,
                 py::return_value_policy::reference_internal,
                 "Get episodic memory system")
            .def("getAssociativeMemory", &Brain::getAssociativeMemory,
                 py::return_value_policy::reference_internal,
                 "Get associative memory system")
            .def("getPredictionSystem", &Brain::getPredictionSystem,
                 py::return_value_policy::reference_internal,
                 "Get prediction system")
            .def("getPlanner", &Brain::getPlanner,
                 py::return_value_policy::reference_internal,
                 "Get neural planner")
            .def("getConceptFormation", &Brain::getConceptFormation,
                 py::return_value_policy::reference_internal,
                 "Get concept formation system")
            .def("getAttention", &Brain::getAttention,
                 py::return_value_policy::reference_internal,
                 "Get attention system")
            .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
                 py::return_value_policy::reference_internal,
                 "Get development system")
            .def("getDopamine", &Brain::getDopamine,
                 py::return_value_policy::reference_internal,
                 "Get dopamine system")
            .def("getCuriosity", &Brain::getCuriosity,
                 py::return_value_policy::reference_internal,
                 "Get curiosity system")
            .def("getNovelty", &Brain::getNovelty,
                 py::return_value_policy::reference_internal,
                 "Get novelty system")
            .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
                 py::return_value_policy::reference_internal,
                 "Get prediction error signal")
            .def("getRandomGenerator", &Brain::getRandomGenerator,
                 py::return_value_policy::reference_internal,
                 "Get random generator");
    }

    // AgentBrain class
    {
        py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
                 py::return_value_policy::reference_internal,
                 "Get the underlying brain")
            .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
                 py::arg("enable"))
            .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
                 py::arg("enable"))
            .def("enableDevelopment", &AgentBrain::enableDevelopment,
                 py::arg("enable"))
            .def("enableCuriosity", &AgentBrain::enableCuriosity,
                 py::arg("enable"))
            .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
            .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
            .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
            .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    }

    // Expose invalid ID values for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Provide common utility functions
    m.def("get_version", []() { return "NLM Python Bindings 1.0"; },
          "Get the version of the NLM Python bindings");
    
    m.def("print_brain_info", [](const Brain& brain) {
        brain.logStatus();
    }, py::arg("brain"), "Print brain information");

} // namespace nlm
