# Minimal enhanced Python bindings for NLM
# Focus on essential improvements that don't break existing functionality

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings for NLM
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        ------------------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action
        with additional convenience methods and enhanced API.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId
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
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        
        // Enhanced configuration access methods
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with integer default")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with float default")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with double default")
        .def("getOr", [](Config& self, const std::string& key, std::string defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with string default")
        
        // Convenience configuration methods
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration integer value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration float value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration string value")
        
        // Configuration validation and analysis
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            // Basic validation
            if (self.has("neuron_count")) {
                auto neurons = self.getOr<int64_t>("neuron_count", 0);
                if (neurons < 10) {
                    warnings.push_back("Low neuron count may affect network dynamics");
                }
                if (neurons < 1) {
                    errors.push_back("Neuron count must be positive");
                }
            }
            
            if (self.has("connection_probability")) {
                auto prob = self.getOr<float>("connection_probability", 0.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    errors.push_back("Connection probability must be between 0 and 1");
                }
            }
            
            return py::make_tuple(errors, warnings);
        }, "Validate configuration and return errors/warnings")
        
        .def("mergeFrom", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                if (auto opt = other.get<std::string>(key)) {
                    self.set(key, *opt, ConfigSource::User);
                }
            }
            return self;
        }, py::arg("other_config"),
           "Merge configuration from another config object")
        
        .def("createDerived", [](const Config& self, const std::string& suffix) {
            auto derived = Config();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (auto opt = self.get<std::string>(key)) {
                    derived.set(key + suffix, *opt, ConfigSource::Default);
                }
            }
            return derived;
        }, py::arg("suffix") = "_derived",
           "Create a derived configuration with modified keys")
        
        .def("exportToDict", [](const Config& self) {
            std::map<std::string, std::string> result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (auto opt = self.get<std::string>(key)) {
                    result[key] = *opt;
                }
            }
            return result;
        }, "Export configuration to dictionary")
        
        .def("importFromDict", [](Config& self, const std::map<std::string, std::string>& data) {
            self.clear();
            for (const auto& pair : data) {
                self.set(pair.first, pair.second, ConfigSource::User);
            }
            return self;
        }, py::arg("data"), "Import configuration from dictionary")
        
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get configuration integer value")
        
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get configuration float value")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = "",
           "Get configuration string value")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        
        // Enhanced action methods
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
            return self;
        }, py::arg("value"), "Add parameter to action")
        
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        
        .def("normalizeParameters", [](Action& self, float min = 0.0f, float max = 1.0f) {
            auto params = self.getParameters();
            if (params.empty()) return self;
            
            float pMin = *std::min_element(params.begin(), params.end());
            float pMax = *std::max_element(params.begin(), params.end());
            
            if (pMax > pMin) {
                for (auto& param : params) {
                    param = min + (param - pMin) * (max - min) / (pMax - pMin);
                }
            }
            
            self.setParameters(params);
            return self;
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize action parameters to range")
        
        .def("scaleParameters", [](Action& self, float factor) {
            auto params = self.getParameters();
            for (auto& param : params) {
                param *= factor;
            }
            self.setParameters(params);
            return self;
        }, py::arg("factor"), "Scale all action parameters by factor")
        
        .def("addNoise", [](Action& self, float noiseLevel = 0.1f) {
            auto params = self.getParameters();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-noiseLevel, noiseLevel);
            
            for (auto& param : params) {
                param += dis(gen);
            }
            self.setParameters(params);
            return self;
        }, py::arg("noise_level") = 0.1f, "Add random noise to parameters")
        
        .def("createComposite", [](ActionType baseType, std::vector<float> baseParams,
                                    ActionType modifierType, std::vector<float> modifierParams) {
            // Create composite action by combining two actions
            Action baseAction(baseType, baseParams);
            Action modifierAction(modifierType, modifierParams);
            
            // Combine parameters (simple averaging)
            auto baseParamsVec = baseAction.getParameters();
            auto modifierParamsVec = modifierAction.getParameters();
            
            std::vector<float> combinedParams(std::max(baseParamsVec.size(), modifierParamsVec.size()), 0.0f);
            for (size_t i = 0; i < combinedParams.size(); ++i) {
                float baseVal = (i < baseParamsVec.size()) ? baseParamsVec[i] : 0.0f;
                float modVal = (i < modifierParamsVec.size()) ? modifierParamsVec[i] : 0.0f;
                combinedParams[i] = (baseVal + modVal) * 0.5f;
            }
            
            return Action(baseType, combinedParams);
        }, py::arg("base_type"), py::arg("base_params"), py::arg("modifier_type"), py::arg("modifier_params"),
           "Create composite action from two actions");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object)pbdoc")
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
        
        // Enhanced world object methods
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        
        .def("containsPoint", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        
        .def("getPosition", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y) tuple")
        
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
            return self;
        }, py::arg("x"), py::arg("y"), "Set object position")
        
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        
        // Enhanced agent body methods
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            return self;
        }, py::arg("x"), py::arg("y"), "Set position")
        
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
            return self;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        
        .def("updatePosition", [](AgentBody& self, float dt) {
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            return self;
        }, py::arg("dt"), "Update position based on velocity");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Advanced action result)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // Enhanced action result methods
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive")
        
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative")
        
        .def("getConfidence", [](const ActionResult& self) {
            float confidence = std::abs(self.reward) * (self.success ? 1.0f : 0.5f);
            return std::min(1.0f, confidence);
        }, "Get confidence level")
        
        .def("toDict", [](const ActionResult& self) {
            std::map<std::string, py::object> result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert to dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Advanced sensory percept)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced simple world)pbdoc")
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Convenience functions
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

    // Enhanced convenience functions for common patterns
    m.def("runSimpleSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a simple simulation with brain, world, and agent");

    m.def("createCompleteSystem", [](const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return py::make_tuple(brain, world, agent);
    }, py::arg("config_file") = "",
       "Create a complete NLM system with brain, world, and agent");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t neuronCount, size_t steps) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        auto benchmarkBrain = std::make_shared<Brain>(config);
        benchmarkBrain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            benchmarkBrain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::map<std::string, float> result;
        result["steps"] = static_cast<float>(steps);
        result["time_ms"] = static_cast<float>(duration.count());
        result["steps_per_second"] = static_cast<float>(steps) / (duration.count() / 1000.0f);
        result["spikes_per_step"] = static_cast<float>(benchmarkBrain->getTotalSpikeCount()) / steps;
        
        return result;
    }, py::arg("brain"), py::arg("neuron_count"), py::arg("steps"),
       "Benchmark brain performance");

    m.def("analyzeBrainNetwork", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, py::object> analysis;
        
        // Get basic statistics
        std::map<std::string, float> stats;
        stats["total_neurons"] = static_cast<float>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<float>(brain->getTotalSynapseCount());
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        analysis["statistics"] = stats;
        
        // Get neuron distribution (simplified)
        std::map<std::string, int> distribution;
        // In practice, you'd iterate through regions and populations
        
        analysis["distribution"] = distribution;
        
        return analysis;
    }, py::arg("brain"), "Analyze brain network structure and properties");

    m.def("createTestEnvironment", [](int width, int height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        // Add some objects
        for (int i = 0; i < 5; ++i) {
            float x = static_cast<float>(rand() % width);
            float y = static_cast<float>(rand() % height);
            WorldObject obj(x, y, WorldObjectType::Resource);
            world->addObject(obj);
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, "Create test environment with objects");

    m.def("validateEnvironment", [](std::shared_ptr<SimpleWorld> world) {
        std::map<std::string, bool> validation;
        
        auto body = world->getAgentBody();
        validation["agent_exists"] = true;
        validation["agent_has_energy"] = body.energy > 0;
        validation["world_has_dimensions"] = world->getWidth() > 0 && world->getHeight() > 0;
        
        // Check for potential issues
        validation["world_too_small"] = world->getWidth() < 10 || world->getHeight() < 10;
        validation["agent_out_of_bounds"] = body.x < 0 || body.x > world->getWidth() ||
                                           body.y < 0 || body.y > world->getHeight();
        
        return validation;
    }, py::arg("world"), "Validate environment for simulation");

    m.def("exportSystemInfo", []() {
        std::map<std::string, std::string> info;
        info["version"] = "1.0.0-enhanced";
        info["description"] = "Enhanced NLM Python bindings with additional convenience methods";
        info["features"] = "Configuration management, enhanced brain analysis, simulation tools";
        return info;
    }, "Get system information and available features");

    // Version and utility functions
    m.attr("__version__") = "1.0.0-enhanced";
    
    m.def("getVersion", []() {
        return std::string("NLM Python Bindings 1.0.0-enhanced");
    }, "Get version information");
    
    m.def("getAvailableFunctions", []() {
        return std::vector<std::string>{
            "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
            "runSimpleSimulation", "createCompleteSystem", "benchmarkBrain",
            "analyzeBrainNetwork", "createTestEnvironment", "validateEnvironment",
            "getVersion", "getAvailableFunctions"
        };
    }, "Get list of available functions");

    // Error handling and validation
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (config.has("neuron_count")) {
            auto neurons = config.getOr<int64_t>("neuron_count", 0);
            if (neurons < 1) errors.push_back("Neuron count must be positive");
            if (neurons > 100000) warnings.push_back("Large neuron count may impact performance");
        }
        
        if (config.has("connection_probability")) {
            auto prob = config.getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) errors.push_back("Connection probability must be between 0 and 1");
        }
        
        return py::make_tuple(errors, warnings);
    }, "Validate configuration and return errors/warnings");

    // Examples and documentation
    m.def("getExamples", []() {
        return std::vector<std::string>{
            "# Basic Usage",
            "brain = pynlm.createBrain(pynlm.createDefaultConfig())",
            "brain.initialize()",
            "for i in range(100):",
            "    brain.step(i)",
            "",
            "# Complete System",
            "brain, world, agent = pynlm.createCompleteSystem()",
            "for i in range(1000):",
            "    brain, world, agent = pynlm.runSimpleSimulation(brain, world, agent, 1)",
            "",
            "# Analysis",
            "stats = brain.getBrainStats()",
            "distribution = brain.getNeuronDistribution()",
            "connectivity = brain.getConnectivityStats()"
        };
    }, "Get example code snippets");
}

} // namespace nlm