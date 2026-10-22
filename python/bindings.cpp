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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("config_file"), "Create config from JSON file")
        .def(py::init<const py::dict&>(), py::arg("config_dict"), "Create config from dictionary")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToFileWithTimestamp", [](Config& self, const std::string& base_path) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            std::string timestamp = ss.str();
            std::string filepath = base_path + "_config_" + timestamp + ".json";
            self.saveToFile(filepath);
            return filepath;
        }, py::arg("base_path"), "Save configuration with timestamp")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value by key")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set configuration value")
        .def("setFromDict", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = item.first.cast<std::string>();
                std::string value = item.second.cast<std::string>();
                self.set(key, value);
            }
        }, py::arg("config_dict"), "Set multiple configuration values from dictionary")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration entry")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getKeysByPrefix", [](const Config& self, const std::string& prefix) {
            std::vector<std::string> keys;
            for (const auto& key : self.getKeys()) {
                if (key.find(prefix) == 0) {
                    keys.push_back(key);
                }
            }
            return keys;
        }, py::arg("prefix"), "Get configuration keys by prefix")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("getAsDict", [](const Config& self) {
            py::dict config_dict;
            for (const auto& key : self.getKeys()) {
                auto value = self.getAsString(key);
                config_dict[key] = value;
            }
            return config_dict;
        }, "Get configuration as dictionary")
        .def("validate", [](const Config& self) {
            // Basic validation: check for required brain parameters
            bool valid = true;
            std::vector<std::string> errors;
            
            if (!self.has("brain.neuron_count")) {
                errors.push_back("Missing required parameter: brain.neuron_count");
                valid = false;
            }
            
            auto neuron_count = self.get<int64_t>("brain.neuron_count");
            if (neuron_count && (*neuron_count <= 0)) {
                errors.push_back("Invalid neuron count: must be positive");
                valid = false;
            }
            
            return py::make_tuple(valid, errors);
        }, "Validate configuration, returns (is_valid, errors_list)")
        .def("merge", [](Config& self, const Config& other) {
            // Merge another config into this one
            for (const auto& key : other.getKeys()) {
                auto value = other.getAsString(key);
                self.set(key, value);
            }
        }, py::arg("other"), "Merge another configuration into this one")
        .def("copy", [](const Config& self) {
            Config copy;
            for (const auto& key : self.getKeys()) {
                auto value = self.getAsString(key);
                copy.set(key, value);
            }
            return copy;
        }, "Create a copy of this configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("configureAs", [](SimpleWorld& self, const py::dict& world_config) {
            size_t width = world_config.value("width", 20UL);
            size_t height = world_config.value("height", 20UL);
            size_t vision_width = world_config.value("visionWidth", 8UL);
            size_t vision_height = world_config.value("visionHeight", 8UL);
            self.configure(width, height, vision_width, vision_height);
        }, py::arg("world_config"), "Configure world from dictionary")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("resetWithConfig", [](SimpleWorld& self, const py::dict& reset_config) {
            self.reset();
            if (reset_config.contains("agent_x") && reset_config.contains("agent_y")) {
                float x = reset_config["agent_x"].cast<float>();
                float y = reset_config["agent_y"].cast<float>();
                self.setAgentStart(x, y);
            }
        }, py::arg("reset_config"), "Reset world with agent start position")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("setAgentStartFromDict", [](SimpleWorld& self, const py::dict& position) {
            float x = position.value("x", 0.0f);
            float y = position.value("y", 0.0f);
            self.setAgentStart(x, y);
        }, py::arg("position"), "Set agent start position from dictionary")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("updateMultiple", [](SimpleWorld& self, size_t steps, float timestep = 0.1f) {
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Update world for multiple steps")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("applyMotorCommandFromAction", [](SimpleWorld& self, const Action& action, float current_time) {
            // Extract ActionType from Action object
            ActionType action_type = action.getType();
            self.applyMotorCommand(action_type, current_time);
        }, py::arg("action"), py::arg("current_time"),
             "Apply action object to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getSensoryPerceptAsDict", [](SimpleWorld& self) {
            py::dict percept_dict;
            auto percept = self.getSensoryPercept();
            auto vision = percept->getVision();
            if (vision) {
                auto vision_data = vision->getData();
                percept_dict["vision"] = vision_data;
            }
            return percept_dict;
        }, "Get sensory percept as dictionary")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getAgentBodyAsDict", [](SimpleWorld& self) {
            py::dict body_dict;
            auto body = self.getAgentBody();
            body_dict["x"] = body->x;
            body_dict["y"] = body->y;
            body_dict["orientation"] = body->orientation;
            body_dict["velocity_x"] = body->velocityX;
            body_dict["velocity_y"] = body->velocityY;
            body_dict["angular_velocity"] = body->angularVelocity;
            body_dict["energy"] = body->energy;
            body_dict["health"] = body->health;
            body_dict["age"] = body->age;
            body_dict["is_moving"] = body->isMoving;
            body_dict["is_turning"] = body->isTurning;
            return body_dict;
        }, "Get agent body state as dictionary")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("addObjectFromDict", [](SimpleWorld& self, const py::dict& obj_dict) {
            float x = obj_dict.value("x", 0.0f);
            float y = obj_dict.value("y", 0.0f);
            auto type = obj_dict.value("type", pynlm::WorldObjectType::Empty);
            float value = obj_dict.value("value", 0.0f);
            float radius = obj_dict.value("radius", 0.5f);
            
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
        }, py::arg("obj_dict"), "Add object from dictionary")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
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
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("runSimulationEpisode", [](SimpleWorld& self, size_t steps, float timestep = 0.1f) {
            py::dict episode_stats;
            episode_stats["steps_executed"] = steps;
            episode_stats["final_time"] = self.getSimulationTime();
            
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
                if (i % 100 == 0) {
                    // Could log progress here
                }
            }
            
            episode_stats["final_time"] = self.getSimulationTime();
            return episode_stats;
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Run world simulation episode, returns episode statistics")
        .def("benchmarkWorld", [](SimpleWorld& self, size_t steps, float timestep = 0.1f) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            py::dict benchmark;
            benchmark["steps_performed"] = steps;
            benchmark["total_time_ms"] = static_cast<double>(duration.count());
            benchmark["time_per_step_ms"] = static_cast<double>(duration.count()) / steps;
            return benchmark;
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Benchmark world performance over specified steps")
        .def("createExperimentWorld", [](SimpleWorld& self, const py::dict& experiment_config) {
            // Configure world for specific experiment type
            std::string experiment_type = experiment_config.value("type", "default");
            
            if (experiment_type == "resource_collection") {
                self.configure(30, 30, 10, 10);
                // Add some resources
                for (int i = 0; i < 5; ++i) {
                    WorldObject resource(5.0f + i * 5.0f, 5.0f, WorldObjectType::Resource, 1.0f, 0.5f);
                    self.addObject(resource);
                }
            } else if (experiment_type == "predator_prey") {
                self.configure(50, 50, 8, 8);
                // Add predator and prey
                WorldObject predator(10.0f, 10.0f, WorldObjectType::Hazard, -1.0f, 1.0f);
                WorldObject prey(40.0f, 40.0f, WorldObjectType::Resource, 1.0f, 0.5f);
                self.addObject(predator);
                self.addObject(prey);
            }
            
            return self;
        }, py::arg("experiment_config"), "Configure world for specific experiment type");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<std::string>(), py::arg("config_file"), "Create brain from configuration file")
        .def(py::init<>(), "Create brain with default configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initialize", [](Brain& self, const std::string& config_file) {
            auto config = std::make_shared<Config>();
            config->loadFromFile(config_file);
            self.getConfig()->loadFromConfig(*config);
            return self.initialize();
        }, py::arg("config_file"), "Initialize brain with configuration file")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepMultiple", &Brain::step, py::arg("steps"),
             "Perform multiple simulation steps efficiently")
        .def("stepUntil", [](Brain& self, size_t target_spikes) {
            size_t current_spikes = self.getTotalSpikeCount();
            size_t steps = 0;
            while (self.getTotalSpikeCount() < current_spikes + target_spikes && steps < 1000000) {
                self.step(steps);
                steps++;
            }
            return steps;
        }, py::arg("target_spikes"), 
             "Step until reaching target spike count (returns steps taken)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("receiveSensoryInput", [](Brain& self, const py::dict& sensory_data) {
            // Convenience method for creating sensory input from dictionary
            auto vision = std::make_shared<Vision>();
            if (sensory_data.contains("vision")) {
                auto vision_data = sensory_data["vision"].cast<std::vector<float>>();
                vision->setData(vision_data);
                self.receiveSensoryInput(vision);
            }
        }, py::arg("sensory_data"),
             "Convenience method: receive sensory input from dictionary")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentRandom", [](Brain& self, NeuronType type, float current_mean, float current_std) {
            auto rng = self.getRandomGenerator();
            if (rng) {
                std::normal_distribution<float> dist(current_mean, current_std);
                for (const auto& region : self.getRegions()) {
                    for (const auto& population : region->getPopulations()) {
                        for (const auto& neuron : population->getNeurons()) {
                            if (neuron->getType() == type) {
                                float random_current = dist(*rng);
                                self.injectCurrent(neuron->getId(), random_current);
                            }
                        }
                    }
                }
            }
        }, py::arg("type"), py::arg("current_mean"), py::arg("current_std"),
             "Inject random current to all neurons of type")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("produceActionWithConfidence", [](Brain& self) -> std::pair<std::unique_ptr<Action>, float> {
            auto action = self.produceAction();
            float confidence = 0.5f; // Default confidence, could be calculated from activity
            return {std::move(action), confidence};
        }, "Produce motor action with confidence score")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("saveWithTimestamp", [](Brain& self, const std::string& base_path) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            std::string timestamp = ss.str();
            std::string filepath = base_path + "_brain_" + timestamp + ".bin";
            bool success = self.save(filepath);
            if (success) {
                return filepath + " (saved successfully)";
            } else {
                return std::string("Failed to save brain state");
            }
        }, py::arg("base_path"), "Save brain state with timestamp")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("loadLatest", [](Brain& self, const std::string& directory) {
            // Find latest saved file in directory
            std::vector<std::string> files;
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.path().extension() == ".bin" && 
                    entry.path().filename().string().find("brain_") != std::string::npos) {
                    files.push_back(entry.path().string());
                }
            }
            if (files.empty()) {
                throw std::runtime_error("No saved brain files found in directory: " + directory);
            }
            std::sort(files.begin(), files.end(), 
                     [](const std::string& a, const std::string& b) {
                         return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
                     });
            return self.load(files[0]);
        }, py::arg("directory"), "Load latest saved brain state from directory")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("addRegion", [](Brain& self, const py::dict& region_config) {
            std::string name = region_config.value("name", "");
            RegionId region_id = self.addRegion(name);
            
            // Configure the region if region_config contains settings
            auto region = self.getRegion(region_id);
            if (region_config.contains("neuron_count")) {
                region->setNeuronCount(region_config["neuron_count"].cast<size_t>());
            }
            if (region_config.contains("connection_probability")) {
                region->setConnectionProbability(region_config["connection_probability"].cast<float>());
            }
            return region_id;
        }, py::arg("region_config"), "Add region with configuration dictionary")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegion", [](Brain& self, const std::string& name) {
            for (const auto& region_id : self.getRegionIds()) {
                auto region = self.getRegion(region_id);
                if (region && region->getName() == name) {
                    return region;
                }
            }
            throw std::runtime_error("Region not found: " + name);
        }, py::arg("name"), "Get region by name")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getConfigAsDict", [](const Brain& self) {
            py::dict config_dict;
            auto config = self.getConfig();
            for (const auto& key : config->getKeys()) {
                auto value = config->getAsString(key);
                config_dict[key] = value;
            }
            return config_dict;
        }, "Get configuration as dictionary")
        .def("updateConfig", [](Brain& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = item.first.cast<std::string>();
                std::string value = item.second.cast<std::string>();
                auto config = self.getConfig();
                config->set(key, value);
            }
        }, py::arg("updates"), "Update brain configuration from dictionary")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("simulateEpisode", [](Brain& self, size_t steps, const py::dict& world_state = py::dict()) {
            size_t initial_spikes = self.getTotalSpikeCount();
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
                // Apply world state if provided
                if (world_state.contains("time_step")) {
                    // In a real implementation, this would interact with the world
                }
            }
            size_t final_spikes = self.getTotalSpikeCount();
            size_t spikes_generated = final_spikes - initial_spikes;
            
            py::dict result;
            result["steps_executed"] = steps;
            result["spikes_generated"] = spikes_generated;
            result["final_spike_count"] = final_spikes;
            return result;
        }, py::arg("steps"), py::arg("world_state") = py::dict(),
             "Run simulation episode, returns episode statistics")
        .def("benchmarkPerformance", [](Brain& self, size_t steps) {
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            py::dict benchmark;
            benchmark["steps_performed"] = steps;
            benchmark["total_time_ms"] = static_cast<double>(duration.count());
            benchmark["time_per_step_ms"] = static_cast<double>(duration.count()) / steps;
            benchmark["spikes_per_second"] = (self.getTotalSpikeCount() * 1000.0) / duration.count();
            return benchmark;
        }, py::arg("steps"), "Benchmark brain performance over specified steps");

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
        .def("processSensoryInputFromDict", [](AgentBrain& self, const py::dict& percept_dict) {
            auto percept = std::make_shared<SensoryPercept>();
            if (percept_dict.contains("vision")) {
                auto vision_data = percept_dict["vision"].cast<std::vector<float>>();
                auto vision = std::make_shared<Vision>();
                vision->setData(vision_data);
                percept->setVision(vision);
            }
            self.processSensoryInput(percept);
        }, py::arg("percept_dict"), "Process sensory input from dictionary")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decodeMotorCommandWithConfidence", [](AgentBrain& self) -> std::pair<std::unique_ptr<Action>, float> {
            auto action = self.decodeMotorCommand();
            float confidence = 0.7f; // Default confidence
            return {std::move(action), confidence};
        }, "Decode motor command with confidence score")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyRewardModulationSimplified", [](AgentBrain& self, float reward) {
            self.applyRewardModulation(reward, 0.0f);
        }, py::arg("reward"), "Apply reward modulation with zero prediction")
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
        .def("getAgentStatus", [](const AgentBrain& self) {
            py::dict status;
            status["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            status["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            status["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            status["development_enabled"] = self.isDevelopmentEnabled();
            status["curiosity_enabled"] = self.isCuriosityEnabled();
            return status;
        }, "Get comprehensive agent status as dictionary")
        .def("runEpisode", [](AgentBrain& self, size_t steps, float timestep = 0.1f) {
            py::dict episode_stats;
            size_t initial_spikes = self.getBrain()->getTotalSpikeCount();
            
            for (size_t i = 0; i < steps; ++i) {
                self.updateDevelopment(timestep);
                
                // In a real implementation, this would:
                // 1. Get sensory input from world
                // 2. Process it through agent
                // 3. Step brain
                // 4. Decode motor command
                // 5. Apply reward modulation
                
                if (i % 100 == 0) {
                    // Log progress occasionally
                }
            }
            
            size_t final_spikes = self.getBrain()->getTotalSpikeCount();
            episode_stats["steps_executed"] = steps;
            episode_stats["spikes_generated"] = final_spikes - initial_spikes;
            return episode_stats;
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Run agent episode, returns episode statistics")
        .def("benchmarkAgent", [](AgentBrain& self, size_t steps, float timestep = 0.1f) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                self.updateDevelopment(timestep);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            py::dict benchmark;
            benchmark["steps_performed"] = steps;
            benchmark["total_time_ms"] = static_cast<double>(duration.count());
            benchmark["time_per_step_ms"] = static_cast<double>(duration.count()) / steps;
            return benchmark;
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Benchmark agent performance over specified steps");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createDefaultConfigFromDict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from dictionary");
    
    m.def("createDefaultConfigFromJson", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create configuration from JSON file");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createBrainFromFile", [](const std::string& config_file) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        return std::make_shared<Brain>(config);
    }, py::arg("config_file"), "Create brain from configuration file");
    
    m.def("createBrainFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), "Create brain from configuration dictionary");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createSimpleWorldFromConfig", [](const py::dict& world_config) {
        auto world = std::make_shared<SimpleWorld>();
        size_t width = world_config.value("width", 20UL);
        size_t height = world_config.value("height", 20UL);
        size_t vision_width = world_config.value("visionWidth", 8UL);
        size_t vision_height = world_config.value("visionHeight", 8UL);
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("world_config"), "Create world from configuration dictionary");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    m.def("runCompleteSimulation", [](const py::dict& simulation_config) {
        // Convenience function to run a complete simulation
        auto config = std::make_shared<Config>();
        for (auto item : simulation_config["config"].cast<py::dict>()) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        size_t steps = simulation_config.value("steps", 1000UL);
        
        py::dict results;
        size_t initial_spikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        size_t final_spikes = brain->getTotalSpikeCount();
        
        results["steps_executed"] = steps;
        results["spikes_generated"] = final_spikes - initial_spikes;
        results["final_spike_count"] = final_spikes;
        results["total_neurons"] = brain->getTotalNeuronCount();
        results["total_synapses"] = brain->getTotalSynapseCount();
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        
        return results;
    }, py::arg("simulation_config"), "Run complete simulation from configuration dictionary");
    
    m.def("createBrainWithDefaults", [](const py::dict& overrides = py::dict()) {
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("brain.neuron_count", "1000");
        config->set("brain.synapse_density", "0.1");
        config->set("brain.connection_probability", "0.05");
        config->set("plasticity.stdp.enable", "true");
        config->set("plasticity.hebbian.enable", "true");
        config->set("neuromod.dopamine.scale", "1.0");
        config->set("neuromod.curiosity.enable", "true");
        
        // Apply user overrides
        for (auto item : overrides) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("overrides") = py::dict(), "Create brain with sensible defaults");
    
    m.def("benchmarkSystem", [](const py::dict& benchmark_config) {
        py::dict results;
        
        // Benchmark brain initialization
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", benchmark_config.value("neuron_count", "500"));
        
        auto brain = std::make_shared<Brain>(config);
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        results["brain_initialization_us"] = static_cast<double>(duration.count());
        
        // Benchmark brain steps
        size_t steps = benchmark_config.value("steps", 1000UL);
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        results["brain_steps_us"] = static_cast<double>(duration.count());
        results["steps_performed"] = steps;
        results["time_per_step_us"] = static_cast<double>(duration.count()) / steps;
        
        return results;
    }, py::arg("benchmark_config"), "Benchmark NLM system performance");
    
    m.def("validateBrainConfig", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        
        auto brain = std::make_shared<Brain>(config);
        return brain->initialize();
    }, py::arg("config_dict"), "Validate brain configuration, returns initialization success");
    
    m.def("exportPythonAPIExamples", []() {
        return R"pbdoc(
# Python API Usage Examples:

# Example 1: Create and run a simple brain
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
print(f"Total neurons: {brain.getTotalNeuronCount()}")

# Example 2: Create brain with custom configuration
config_dict = {
    "brain.neuron_count": "2000",
    "brain.synapse_density": "0.15",
    "plasticity.stdp.enable": "true"
}
brain = pynlm.createBrainFromDict(config_dict)

# Example 3: Run complete simulation
sim_config = {
    "config": {"brain.neuron_count": "1000"},
    "steps": 500
}
results = pynlm.runCompleteSimulation(sim_config)

# Example 4: Use context manager pattern
brain = pynlm.createBrainWithDefaults()
brain.initialize()
# ... do work ...
brain.reset()

# Example 5: Benchmark performance
benchmark_config = {"neuron_count": "1000", "steps": 1000}
perf_results = pynlm.benchmarkSystem(benchmark_config)
)pbdoc";
    }, "Get Python API usage examples");
    
    m.attr("__version__") = "0.1.0";
    m.attr("DEFAULT_CONFIG") = py::dict({
        {"brain.neuron_count", "1000"},
        {"brain.synapse_density", "0.1"},
        {"brain.connection_probability", "0.05"},
        {"plasticity.stdp.enable", "true"},
        {"plasticity.hebbian.enable", "true"},
        {"neuromod.dopamine.scale", "1.0"},
        {"neuromod.curiosity.enable", "true"}
    });
    m.attr("NEURON_TYPES") = py::dict({
        {"EXCITATORY", static_cast<int>(NeuronType::Excitatory)},
        {"INHIBITORY", static_cast<int>(NeuronType::Inhibitory)},
        {"MODULATORY", static_cast<int>(NeuronType::Modulatory)},
        {"SENSORY", static_cast<int>(NeuronType::Sensory)},
        {"MOTOR", static_cast<int>(NeuronType::Motor)},
        {"INTERNAL", static_cast<int>(NeuronType::Internal)}
    });
    m.attr("ACTION_TYPES") = py::dict({
        {"MOVE_FORWARD", static_cast<int>(ActionType::MoveForward)},
        {"MOVE_BACKWARD", static_cast<int>(ActionType::MoveBackward)},
        {"TURN_LEFT", static_cast<int>(ActionType::TurnLeft)},
        {"TURN_RIGHT", static_cast<int>(ActionType::TurnRight)},
        {"INTERACT", static_cast<int>(ActionType::Interact)},
        {"EAT", static_cast<int>(ActionType::Eat)},
        {"DRINK", static_cast<int>(ActionType::Drink)},
        {"REST", static_cast<int>(ActionType::Rest)}
    });
}

} // namespace nlm
