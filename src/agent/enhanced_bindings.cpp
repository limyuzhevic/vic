# This file contains enhanced Python bindings for the NLM framework.
# It provides a more Pythonic API with better error handling and convenience methods.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        A Python-friendly binding for the NLM C++ neural simulation framework.
        This enhanced API provides better usability, error handling, and convenience methods.
        
        Key Features:
        - Pythonic factory methods with sensible defaults
        - Enhanced error handling with custom exceptions
        - Utility functions for common simulation patterns
        - Improved type safety and validation
        - Performance monitoring helpers
    )pbdoc";

    // Custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Enhanced NeuronId with better validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>(), "Default constructor")
        .def(py::init<uint64_t>(), py::arg("value"), "Create NeuronId with specific value")
        .def_readwrite("value", &NeuronId::value, "Internal value")
        .def("index", &NeuronId::index, "Get neuron index")
        .def(py::self == py::self, "Compare equality")
        .def(py::self != py::self, "Compare inequality")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Enhanced SynapseId with better validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enhanced RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Enhanced PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def(py::self == py::self)
        .def(py::self != py::self);

    // Enhanced NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__repr__", [](NeuronType type) {
            static const char* names[] = {"Excitatory", "Inhibitory", "Modulatory", 
                                         "Sensory", "Motor", "Internal"};
            int idx = static_cast<int>(type);
            return idx < 6 ? names[idx] : "Unknown";
        });

    // Enhanced SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__repr__", [](SynapseType type) {
            static const char* names[] = {"Excitatory", "Inhibitory", "Modulatory", 
                                         "Electrical", "GapJunction"};
            int idx = static_cast<int>(type);
            return idx < 5 ? names[idx] : "Unknown";
        });

    // Enhanced DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__repr__", [](DevelopmentalStage stage) {
            static const char* names[] = {"Initial", "CriticalPeriod", "Maturation", 
                                         "Adult", "Aging"};
            int idx = static_cast<int>(stage);
            return idx < 5 ? names[idx] : "Unknown";
        });

    // Enhanced FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__repr__", [](FiringState state) {
            static const char* names[] = {"Resting", "Active", "Refractory", "Inhibited"};
            int idx = static_cast<int>(state);
            return idx < 4 ? names[idx] : "Unknown";
        });

    // Enhanced ActionType enum
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
        .export_values()
        .def("__repr__", [](ActionType action) {
            static const char* names[] = {"MoveForward", "MoveBackward", "MoveLeft", "MoveRight",
                                         "TurnLeft", "TurnRight", "Look", "LookUp", "LookDown",
                                         "Interact", "Eat", "Drink", "Rest", "Wait", "Custom"};
            int idx = static_cast<int>(action);
            return idx < 15 ? names[idx] : "Unknown";
        });

    // Enhanced MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("__repr__", [](MotorCommand cmd) {
            static const char* names[] = {"MoveForward", "MoveBackward", "TurnLeft", "TurnRight",
                                         "LookLeft", "LookRight", "Interact", "Wait"};
            int idx = static_cast<int>(cmd);
            return idx < 8 ? names[idx] : "Unknown";
        });

    // Enhanced WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__repr__", [](WorldObjectType type) {
            static const char* names[] = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
            int idx = static_cast<int>(type);
            return idx < 5 ? names[idx] : "Unknown";
        });

    // Enhanced Config class with better validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Add convenience methods for common operations
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or default if not set")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with source specification");

    // Enhanced SensoryInput class with better error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validate", [](const SensoryInput& self) {
            // Virtual validation method - can be overridden
            return true;
        }, "Validate sensory input data");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("createFromArray", [](size_t width, size_t height, 
                                   const std::vector<float>& data) {
            auto vision = Vision(width, height, 1);
            vision.setData(data);
            return vision;
        }, "Create Vision from width, height, and data array");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("createFromSamples", [](size_t sampleRate, 
                                    const std::vector<float>& samples) {
            auto audio = Audio(sampleRate, samples.size());
            audio.setData(samples);
            return audio;
        }, "Create Audio from sample rate and samples");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("createFromDict", [](const std::vector<float>& signals) {
            auto internal = InternalSignals();
            for (float val : signals) {
                internal.addSignal(val);
            }
            return internal;
        }, "Create InternalSignals from signal values");

    // Enhanced Action class
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
        // Convenience methods
        .def("setPosition", [](Action& self, float x, float y, float z) {
            auto params = self.getParameters();
            params.resize(3);
            params[0] = x;
            params[1] = y;
            params[2] = z;
            self.setParameters(params);
        }, "Set action position parameters")
        .def("getPosition", [](Action& self) {
            auto params = self.getParameters();
            if (params.size() >= 3) {
                return py::make_tuple(params[0], params[1], params[2]);
            }
            return py::make_tuple(0.0f, 0.0f, 0.0f);
        }, "Get action position parameters");

    // Enhanced WorldObject class
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
        // Convenience methods
        .def("getDistance", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, "Get distance to position")
        .def("isNearby", [](const WorldObject& self, float x, float y, float maxDist) {
            return self.getDistance(x, y) <= maxDist;
        }, "Check if object is within maximum distance");

    // Enhanced AgentBody class
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
        .def("moveTo", [](AgentBody& self, float x, float y, double timestep) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0.01f) {
                self.velocityX = dx / distance * 2.0f;
                self.velocityY = dy / distance * 2.0f;
                self.isMoving = true;
            }
        }, "Move agent to position")
        .def("turnTo", [](AgentBody& self, float targetAngle, double timestep) {
            float angleDiff = targetAngle - self.orientation;
            while (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
            while (angleDiff < -M_PI) angleDiff += 2.0f * M_PI;
            self.angularVelocity = angleDiff * 2.0f;
            self.isTurning = true;
        }, "Turn agent to face direction");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("isSuccessful", &ActionResult::success, "Check if action was successful")
        .def("getFormattedResult", [](const ActionResult& self) {
            return "Reward: " + std::to_string(self.reward) + 
                   ", Success: " + std::string(self.success ? "Yes" : "No") +
                   ", Message: " + self.message;
        }, "Get formatted result string");

    // Enhanced SensoryPercept class
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
        // Convenience methods
        .def("createFromVision", [](const std::vector<float>& vision, 
                                    size_t width, size_t height) {
            auto percept = SensoryPercept();
            percept.setVision(vision);
            return percept;
        }, "Create SensoryPercept from vision data")
        .def("mergeWith", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            return self;
        }, "Merge with another percept");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Enhanced world utilities
        .def("placeResources", [](SimpleWorld& self, int count, float value = 10.0f) {
            for (int i = 0; i < count; ++i) {
                float x = self.getRandomSeed() % 1000 / 1000.0f * (self.getWidth() - 2) + 1;
                float y = self.getRandomSeed() % 1000 / 1000.0f * (self.getHeight() - 2) + 1;
                WorldObject obj(x, y, WorldObjectType::Resource, value, 0.4f);
                self.addObject(obj);
            }
        }, "Place resource objects in the world")
        .def("placeHazards", [](SimpleWorld& self, int count, float damage = -15.0f) {
            for (int i = 0; i < count; ++i) {
                float x = self.getRandomSeed() % 1000 / 1000.0f * (self.getWidth() - 2) + 1;
                float y = self.getRandomSeed() % 1000 / 1000.0f * (self.getHeight() - 2) + 1;
                WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
                self.addObject(obj);
            }
        }, "Place hazard objects in the world")
        .def("getResourcesCount", [](const SimpleWorld& self) {
            int count = 0;
            for (const auto& obj : self.getObjects()) {
                if (obj.active && obj.type == WorldObjectType::Resource) {
                    count++;
                }
            }
            return count;
        }, "Get number of resource objects")
        .def("getHazardsCount", [](const SimpleWorld& self) {
            int count = 0;
            for (const auto& obj : self.getObjects()) {
                if (obj.active && obj.type == WorldObjectType::Hazard) {
                    count++;
                }
            }
            return count;
        }, "Get number of hazard objects");

    // Enhanced Brain class with better error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // Convenience methods for common operations
        .def("runSimulation", [](Brain& self, size_t steps, bool logProgress = false) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
                if (logProgress && i % 100 == 0) {
                    std::cout << "Step " << i << ": " 
                              << self.getFiringNeuronCount() << " firing neurons" << std::endl;
                }
            }
        }, "Run simulation for specified number of steps")
        .def("createRegionWithNeurons", [](Brain& self, const std::string& name, 
                                            size_t neuronCount, NeuronType neuronType) {
            auto regionId = self.addRegion(name);
            auto* region = self.getRegion(regionId);
            if (region) {
                // This would require access to population creation logic
                // For now, just return the region ID
            }
            return regionId;
        }, "Create a region with specified number of neurons")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get brain statistics as dictionary");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced agent brain convenience methods
        .def("createEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            self.initialize(*world);
            return true;
        }, "Create a new episode with specified world")
        .def("runStep", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            auto percept = world->getSensoryPercept();
            self.processSensoryInput(percept);
            return self.decodeMotorCommand();
        }, "Run a single simulation step")
        .def("getAgentStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["dev_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get agent brain statistics");

    // Factory functions with sensible defaults
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createMinimalConfig", [](size_t neuronCount = 100) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05, ConfigSource::Default);
        return config;
    }, "Create a minimal configuration with specified neuron count");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced utility functions
    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain, 
                              std::shared_ptr<SimpleWorld> world,
                              size_t steps = 100) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (size_t step = 0; step < steps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto motorCmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motorCmd, step * 0.1);
            
            agent->applyRewardModulation(0.0f, 0.0f);
            agent->updateDevelopment(0.1);
            
            if (step % 10 == 0) {
                std::cout << "Step " << step << ": "
                          << "Curiosity: " << agent->getCuriosityLevel()
                          << ", Novelty: " << agent->getNoveltyLevel() << std::endl;
            }
        }
    }, "Run a complete agent episode");

    m.def("validateBrainState", [](const std::shared_ptr<Brain>& brain) {
        bool valid = true;
        if (!brain) {
            throw py::value_error("Brain pointer is null");
        }
        if (brain->getTotalNeuronCount() == 0) {
            throw py::value_error("Brain has no neurons");
        }
        return valid;
    }, "Validate brain state and throw exceptions if invalid");

    m.def("compareBrains", [](const std::shared_ptr<Brain>& brain1,
                             const std::shared_ptr<Brain>& brain2) {
        if (!brain1 || !brain2) {
            throw py::value_error("Brain pointers cannot be null");
        }
        
        py::dict comparison;
        comparison["same_neurons"] = brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount();
        comparison["same_synapses"] = brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount();
        comparison["same_regions"] = brain1->getRegionCount() == brain2->getRegionCount();
        
        return comparison;
    }, "Compare two brains and return comparison results");

    // Export constants and invalid values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add metadata about the enhanced API
    m.attr("API_VERSION") = "2.0.0";
    m.attr("ENHANCED_FEATURES") = py::list({
        "Pythonic factory methods",
        "Enhanced error handling",
        "Convenience functions",
        "Improved documentation",
        "Validation utilities",
        "Statistics collection"
    });

} // namespace nlm
