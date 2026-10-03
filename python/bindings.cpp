#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>

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

// Python-specific helper functions
namespace py_utils {
    // Helper to convert NeuronId to string representation
    std::string neuronIdToString(const NeuronId& id) {
        return std::format("<NeuronId: {}>", id.value);
    }
    
    // Helper to convert SynapseId to string representation
    std::string synapseIdToString(const SynapseId& id) {
        return std::format("<SynapseId: {}>", id.value);
    }
    
    // Helper to convert RegionId to string representation
    std::string regionIdToString(const RegionId& id) {
        return std::format("<RegionId: {}>", id.value);
    }
    
    // Helper to convert PopulationId to string representation
    std::string populationIdToString(const PopulationId& id) {
        return std::format("<PopulationId: {}>", id.value);
    }
    
    // Helper to create a default config with common settings
    std::shared_ptr<Config> createDefaultConfigWithSettings(int neuron_count = 1000, 
                                                           double connection_probability = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", connection_probability);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.initial_weight_mean", 0.5);
        config->set("brain.initial_weight_std", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        return config;
    }
    
    // Helper to run a complete simulation episode
    void runCompleteEpisode(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent,
                           int num_steps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent provided");
        }
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);  // Default timestep
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Optional: Apply reward modulation if enabled
            if (agent->isRewardModulationEnabled()) {
                auto reward = percept->getInternal()[0] if percept->getInternal() else 0.0;
                agent->applyRewardModulation(reward, 0.0);
            }
            
            // Optional: Update development if enabled
            if (agent->isDevelopmentEnabled()) {
                agent->updateDevelopment(0.1);
            }
        }
    }
}

// Enhanced neuron type information
struct PythonNeuronInfo {
    NeuronId id;
    std::string type_name;
    std::string region_name;
    float membrane_potential;
    bool is_firing;
    size_t last_spike_step;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This is an enhanced version with better Pythonic API, comprehensive documentation,
        and additional convenience methods for easier usage and advanced functionality.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register exception for configuration errors
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");

    // NeuronId with enhanced string representation and utilities
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return py_utils::neuronIdToString(id);
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId with enhanced string representation and utilities
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return py_utils::synapseIdToString(id);
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId with enhanced string representation and utilities
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return py_utils::regionIdToString(id);
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId with enhanced string representation and utilities
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return py_utils::populationIdToString(id);
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enhanced neuron type enumeration with additional metadata
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neurons that increase firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Neurons that decrease firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Neurons that modulate other neurons' behavior")
        .value("Sensory", NeuronType::Sensory, "Neurons that process sensory input")
        .value("Motor", NeuronType::Motor, "Neurons that generate motor output")
        .value("Internal", NeuronType::Internal, "Neurons for internal computations")
        .export_values();

    // Synapse type enumeration with enhanced descriptions
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Electrical gap junction")
        .export_values();

    // Enhanced developmental stage enumeration with additional info
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental state")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Enhanced plasticity period")
        .value("Maturation", DevelopmentalStage::Maturation, "Neural maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stable state")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes")
        .export_values();

    // Enhanced firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron recovering from spike")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by other neurons")
        .export_values();

    // Enhanced action type enumeration with more options
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in environment")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with environment object")
        .value("Eat", ActionType::Eat, "Consume food/resource")
        .value("Drink", ActionType::Drink, "Consume water")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without movement")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    // Enhanced motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward one unit")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward one unit")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left 90 degrees")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right 90 degrees")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Attempt interaction")
        .value("Wait", MotorCommand::Wait, "No movement")
        .export_values();

    // World object type enumeration with enhanced descriptions
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Harmful object")
        .value("Wall", WorldObjectType::Wall, "Immovable barrier")
        .value("Marker", WorldObjectType::Marker, "Location marker")
        .export_values();

    // Enhanced Config class with better Python API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        Configuration management for neural simulation parameters.
        Provides access to brain settings, plasticity rules, neuromodulation,
        and simulation parameters through key-value pairs.
        
        Configuration values can be integers, floats, booleans, strings,
        or lists of these types.
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"),
            "Get configuration value as string (throws if key doesn't exist or type mismatch)")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"),
            "Get configuration value as int (throws if key doesn't exist or type mismatch)")
        .def("getFloat", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"),
            "Get configuration value as float (throws if key doesn't exist or type mismatch)")
        .def("getBool", [](const Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"),
            "Get configuration value as bool (throws if key doesn't exist or type mismatch)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
        
        Abstract base class representing sensory data from the environment.
        Subclasses include Vision, Audio, and InternalSignals.
        "pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input
        
        Represents visual input from the environment with width, height, and color channels.
        Data is stored as a flattened array of RGB values (0-1 range).
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw std::invalid_argument("Data size doesn't match vision dimensions");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getDataAsArray", [](const Vision& self) {
            return self.getData();
        }, "Get vision data as Python list")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class with Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
        
        Represents audio input with sample rate and amplitude data.
        Data is stored as a vector of float amplitudes (-1 to 1 range).
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw std::invalid_argument("Audio data size doesn't match expected samples");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("getDataAsArray", [](const Audio& self) {
            return self.getData();
        }, "Get audio data as Python list")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
        
        Represents internal brain state signals such as hunger, thirst, energy levels,
        and other homeostatic signals.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all internal signals as Python list")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals");

    // Enhanced Action class with more Pythonic interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
        
        Represents a motor command or behavioral action with type and parameters.
        Actions are the output of the brain and input to the world simulation.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            return result;
        }, "Convert action to Python dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            ActionType type = py::cast<ActionType>(d["type"]);
            std::vector<float> params;
            if (d.contains("parameters")) {
                params = py::cast<std::vector<float>>(d["parameters"]);
            }
            return Action(type, params);
        }, py::arg("dict"), "Create action from Python dictionary");

    // Enhanced WorldObject class with Python properties
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
        
        Represents an object in the simulated world with position, type, and properties.
        Objects can be resources, hazards, walls, or markers.
        "pbdoc")
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
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given position")
        .def("isWithinRange", [](const WorldObject& obj, float x, float y, float range) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx*dx + dy*dy) <= (range*range);
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if within range of position");

    // Enhanced AgentBody class with Python property access
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state
        
        Represents the physical state of the agent in the world including position,
        orientation, velocity, and physiological state.
        "pbdoc")
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
        .def("distanceTo", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given position")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
        
        Represents the outcome of executing a motor command in the world.
        Contains reward signal and success status for learning.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class with Python convenience methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data
        
        Container for all sensory inputs received by the agent in one timestep.
        Includes vision, touch, internal signals, proprioception, and audio data.
        "pbdoc")
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
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict result;
            
            auto vision = percept.getVision();
            if (vision) {
                py::dict vision_dict;
                vision_dict["width"] = vision->getWidth();
                vision_dict["height"] = vision->getHeight();
                vision_dict["channels"] = vision->getChannels();
                vision_dict["data"] = vision->getData();
                result["vision"] = vision_dict;
            }
            
            auto touch = percept.getTouch();
            if (touch) {
                result["touch"] = *touch;
            }
            
            auto internal = percept.getInternal();
            if (internal) {
                result["internal"] = *internal;
            }
            
            auto proprioception = percept.getProprioception();
            if (proprioception) {
                result["proprioception"] = *proprioception;
            }
            
            auto audio = percept.getAudio();
            if (audio) {
                py::dict audio_dict;
                audio_dict["sampleRate"] = audio->getSampleRate();
                audio_dict["data"] = audio->getData();
                result["audio"] = audio_dict;
            }
            
            return result;
        }, "Convert percept to Python dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            auto percept = std::make_unique<SensoryPercept>();
            
            if (d.contains("vision")) {
                auto vision_dict = d["vision"].cast<py::dict>();
                auto vision = std::make_unique<Vision>(
                    vision_dict["width"].cast<size_t>(),
                    vision_dict["height"].cast<size_t>(),
                    vision_dict["channels"].cast<size_t>()
                );
                vision->setData(vision_dict["data"].cast<std::vector<float>>());
                percept->setVision(std::move(vision));
            }
            
            if (d.contains("touch")) {
                percept->setTouch(d["touch"].cast<std::vector<float>>());
            }
            
            if (d.contains("internal")) {
                percept->setInternal(d["internal"].cast<std::vector<float>>());
            }
            
            if (d.contains("proprioception")) {
                percept->setProprioception(d["proprioception"].cast<std::vector<float>>());
            }
            
            if (d.contains("audio")) {
                auto audio_dict = d["audio"].cast<py::dict>();
                auto audio = std::make_unique<Audio>(
                    audio_dict["sampleRate"].cast<size_t>(),
                    audio_dict["data"].cast<std::vector<float>>().size()
                );
                audio->setData(audio_dict["data"].cast<std::vector<float>>());
                audio->setSampleRate(audio_dict["sampleRate"].cast<size_t>());
                percept->setAudio(std::move(audio));
            }
            
            if (d.contains("timestamp")) {
                percept->setTimestamp(d["timestamp"].cast<double>());
            }
            
            return percept;
        }, py::arg("dict"), "Create percept from Python dictionary")
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision() != nullptr;
        }, "Check if percept contains vision data")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return percept.getTouch() != nullptr;
        }, "Check if percept contains touch data")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return percept.getInternal() != nullptr;
        }, "Check if percept contains internal signals")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return percept.getAudio() != nullptr;
        }, "Check if percept contains audio data");

    // Enhanced SimpleWorld class with Pythonic API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation
        
        A simple 2D grid-based world for neural simulation with sensory processing,
        motor actions, and object interaction.
        
        The world provides the environment in which the brain operates, handling
        physics, perception, and action execution.
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics for one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world simulation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept (agent's view of world)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (position, velocity, etc.)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (inside world and not blocking)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in units")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed")
        .def("getVisionDimensions", [](const SimpleWorld& world) {
            auto percept = world.getSensoryPercept();
            auto vision = percept.getVision();
            if (vision) {
                return std::make_pair(vision->getWidth(), vision->getHeight());
            }
            return std::make_pair(0, 0);
        }, "Get agent vision dimensions as (width, height) tuple")
        .def("getAgentPosition", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            return body.orientation;
        }, "Get agent orientation in radians");

    // Enhanced Brain class with comprehensive Python API
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        The core neural computation engine implementing Leaky Integrate-and-Fire (LIF)
        neurons with event-driven spike propagation and multiple plasticity mechanisms.
        
        The brain integrates memory, prediction, cognition, and neuromodulation systems
        to create intelligent behavior through spiking neural dynamics.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with step number and timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
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
             "Add a new neural region (name optional)")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system access")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system access")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system access")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system access")
        .def("getWorkingMemory", [](const Brain& brain) {
            return brain.getWorkingMemory();
        }, py::return_value_policy::reference_internal,
            "Get working memory access")
        .def("getEpisodicMemory", [](const Brain& brain) {
            return brain.getEpisodicMemory();
        }, py::return_value_policy::reference_internal,
            "Get episodic memory access")
        .def("getAssociativeMemory", [](const Brain& brain) {
            return brain.getAssociativeMemory();
        }, py::return_value_policy::reference_internal,
            "Get associative memory access")
        .def("getPredictionSystem", [](const Brain& brain) {
            return brain.getPredictionSystem();
        }, py::return_value_policy::reference_internal,
            "Get prediction system access")
        .def("getPlanner", [](const Brain& brain) {
            return brain.getPlanner();
        }, py::return_value_policy::reference_internal,
            "Get neural planner access")
        .def("getConceptFormation", [](const Brain& brain) {
            return brain.getConceptFormation();
        }, py::return_value_policy::reference_internal,
            "Get concept formation access")
        .def("getAttention", [](const Brain& brain) {
            return brain.getAttention();
        }, py::return_value_policy::reference_internal,
            "Get attentional selection access")
        .def("getDevelopmentSystem", [](const Brain& brain) {
            return brain.getDevelopmentSystem();
        }, py::return_value_policy::reference_internal,
            "Get development system access")
        .def("getDopamine", [](const Brain& brain) {
            return brain.getDopamine();
        }, py::return_value_policy::reference_internal,
            "Get dopamine neuromodulation access")
        .def("getCuriosity", [](const Brain& brain) {
            return brain.getCuriosity();
        }, py::return_value_policy::reference_internal,
            "Get curiosity neuromodulation access")
        .def("getNovelty", [](const Brain& brain) {
            return brain.getNovelty();
        }, py::return_value_policy::reference_internal,
            "Get novelty neuromodulation access")
        .def("getPredictionError", [](const Brain& brain) {
            return brain.getPredictionErrorSignal();
        }, py::return_value_policy::reference_internal,
            "Get prediction error signal access")
        .def("getRandomGenerator", [](const Brain& brain) {
            return brain.getRandomGenerator();
        }, py::return_value_policy::reference_internal,
            "Get random number generator access")
        .def("getNeuromodulationLevel", [](const Brain& brain) {
            auto agentBrain = std::make_shared<AgentBrain>(brain.getSharedPtr());
            return agentBrain->getNeuromodulationLevel();
        }, "Get overall neuromodulation level")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("toDict", [](const Brain& brain) {
            py::dict result;
            result["neuronCount"] = brain.getTotalNeuronCount();
            result["synapseCount"] = brain.getTotalSynapseCount();
            result["firingNeurons"] = brain.getFiringNeuronCount();
            result["activeNeurons"] = brain.getActiveNeuronCount();
            result["totalSpikes"] = brain.getTotalSpikeCount();
            result["pendingSpikes"] = brain.getPendingSpikeEventCount();
            result["avgFiringRate"] = brain.getAverageFiringRate();
            result["eiRatio"] = brain.getExcitationInhibitionRatio();
            result["developmentalStage"] = brain.getDevelopmentalStage();
            return result;
        }, "Convert brain state to Python dictionary")
        .def_static("createDefaultConfig", &py_utils::createDefaultConfigWithSettings,
                    py::arg("neuron_count") = 1000, py::arg("connection_probability") = 0.1,
                    "Create a default configuration with specified parameters")
        .def_static("runCompleteEpisode", &py_utils::runCompleteEpisode,
                    py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
                    "Run a complete simulation episode with all systems integrated");

    // Enhanced AgentBrain class with Pythonic API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world
        
        The agent brain serves as the interface between the neural brain and the
        simulated world, handling sensory processing and motor output generation.
        
        It integrates all brain subsystems and provides the essential functions
        for autonomous behavior in an environment.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with neural brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world environment")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("toDict", [](const AgentBrain& agent) {
            py::dict result;
            result["curiosityLevel"] = agent.getCuriosityLevel();
            result["noveltyLevel"] = agent.getNoveltyLevel();
            result["predictionError"] = agent.getPredictionError();
            result["neuromodulationLevel"] = agent.getNeuromodulationLevel();
            result["developmentalStage"] = agent.getDevelopmentalStage();
            result["rewardModEnabled"] = agent.isRewardModulationEnabled();
            result["plasticityEnabled"] = agent.isStructuralPlasticityEnabled();
            result["developmentEnabled"] = agent.isDevelopmentEnabled();
            result["curiosityEnabled"] = agent.isCuriosityEnabled();
            return result;
        }, "Convert agent state to Python dictionary")
        .def_static("createAndInitialize", [](std::shared_ptr<Brain> brain,
                                             std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            return agent;
        }, py::arg("brain"), py::arg("world"),
            "Create and initialize agent brain with world in one call");

    // Helper functions with Pythonic interfaces
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions for common operations
    m.def("run_simulation", [](std::shared_ptr<Brain> brain,
                              std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent,
                              int num_steps) {
        py_utils::runCompleteEpisode(brain, world, agent, num_steps);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation episode with all systems integrated");

    // Global constants and invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";

    // Documentation and help functions
    m.def("getDocumentation", []() {
        py::dict doc;
        doc["brain"] = "Central neural computation engine";
        doc["config"] = "Configuration management for simulation parameters";
        doc["world"] = "2D environment for agent interaction";
        doc["agent"] = "Interface between brain and world";
        return doc;
    }, "Get basic documentation dictionary");

    m.def("getExamples", []() {
        py::list examples;
        
        // Example 1: Simple brain
        py::dict ex1;
        ex1["title"] = "Simple Brain Simulation";
        ex1["code"] = R"python(
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        )python";
        ex1["description"] = "Basic brain simulation without environment";
        examples.append(ex1);
        
        // Example 2: Complete agent
        py::dict ex2;
        ex2["title"] = "Complete Agent Simulation";
        ex2["code"] = R"python(
import pynlm

# Setup complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
print("Agent simulation complete!")
        )python";
        ex2["description"] = "Complete agent with environment interaction";
        examples.append(ex2);
        
        // Example 3: Configuration example
        py::dict ex3;
        ex3["title"] = "Custom Configuration";
        ex3["code"] = R"python(
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -45.0)  # Higher threshold
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Verify configuration
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"EI ratio: {brain.getExcitationInhibitionRatio():.3f}")
        )python";
        ex3["description"] = "Custom configuration with specific parameters";
        examples.append(ex3);
        
        return examples;
    }, "Get example code snippets");

    m.def("getQuickStart", []() {
        py::dict quickstart;
        
        quickstart["installation"] = R"bash(
pip install pynlm
        )bash";
        
        quickstart["basic_usage"] = R"python(
import pynlm

# Simplest possible brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
        )python";
        
        quickstart["complete_example"] = R"python(
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for i in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
        )python";
        
        return quickstart;
    }, "Get quick start guide");
}

} // namespace nlm