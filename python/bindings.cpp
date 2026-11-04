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
        .value("MoveLeft", MotorCommand::MoveLeft)
        .value("MoveRight", MotorCommand::MoveRight)
        .value("LookUp", MotorCommand::LookUp)
        .value("LookDown", MotorCommand::LookDown)
        .value("Eat", MotorCommand::Eat)
        .value("Drink", MotorCommand::Drink)
        .value("Rest", MotorCommand::Rest)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

# Add missing Config methods for advanced configuration
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Advanced configuration methods
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value or default")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int64_t>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value or default")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float value or default")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get double value or default")
        .def("getOr", [](Config& self, const std::string& key, std::string defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get string value or default")
        .def("set", [](Config& self, const std::string& key, bool value) {
            // This is a simplified version - in reality, Config::set is type-erased
            throw py::type_error("Type conversion not supported for bool");
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", [](Config& self, const std::string& key, int value) {
            // This is a simplified version - in reality, Config::set is type-erased
            throw py::type_error("Type conversion not supported for int");
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", [](Config& self, const std::string& key, float value) {
            // This is a simplified version - in reality, Config::set is type-erased
            throw py::type_error("Type conversion not supported for float");
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set", [](Config& self, const std::string& key, double value) {
            // This is a simplified version - in reality, Config::set is type-erased
            throw py::type_error("Type conversion not supported for double");
        }, py::arg("key"), py::arg("value"), "Set double value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            // This is a simplified version - in reality, Config::set is type-erased
            throw py::type_error("Type conversion not supported for string");
        }, py::arg("key"), py::arg("value"), "Set string value")
        // Configuration management
        .def("applyTemplate", &Config::applyTemplate,
             py::arg("templateName"), "Apply a configuration template")
        .def("mergeFromFile", [](Config& self, const std::string& filepath) {
            auto otherConfig = std::make_shared<Config>();
            otherConfig->loadFromFile(filepath);
            self.mergeFrom(otherConfig);
        }, py::arg("filepath"), "Merge configuration from file")
        .def("exportToJson", [](Config& self) {
            // This would export to JSON format
            // For now, return summary as a string representation
            return self.summary();
        }, "Export configuration to JSON string")
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            auto keys = self.getKeys();
            
            // Check for common configuration issues
            if (self.has("brain.neuron_count")) {
                int64_t neurons = self.getOr<int64_t>("brain.neuron_count", 0);
                if (neurons <= 0) {
                    errors.push_back("brain.neuron_count must be positive");
                }
                if (neurons > 1000000) {
                    errors.push_back("brain.neuron_count too large, may cause performance issues");
                }
            }
            
            if (self.has("brain.connection_probability")) {
                float prob = self.getOr<float>("brain.connection_probability", 0.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
                }
            }
            
            if (self.has("plasticity.stdp.learning_rate")) {
                float rate = self.getOr<float>("plasticity.stdp.learning_rate", 0.0f);
                if (rate < 0.0f || rate > 1.0f) {
                    errors.push_back("plasticity.stdp.learning_rate must be between 0.0 and 1.0");
                }
            }
            
            return errors;
        }, "Validate configuration and return list of errors")
        .def("clone", [](Config& self) {
            auto cloned = std::make_shared<Config>();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (auto value = self.get(key)) {
                    // Type-erased copy - in practice would need proper type dispatch
                    cloned->set(key, *value);
                }
            }
            return cloned;
        }, "Create a copy of this configuration");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        // Advanced world configuration methods
        .def("setObstacleGrid", [](SimpleWorld& self, const std::vector<std::vector<bool>>& obstacles) {
            for (size_t y = 0; y < obstacles.size(); ++y) {
                for (size_t x = 0; x < obstacles[y].size(); ++x) {
                    if (obstacles[y][x]) {
                        self.addObject(WorldObject(x, y, WorldObjectType::Wall, 0.5f, 0.4f));
                    }
                }
            }
        }, py::arg("obstacles"), R"pbdoc(Set up an obstacle grid where true cells become walls)pbdoc")
        .def("addResource", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add a resource object to the world (overloaded from WorldObject))pbdoc")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f, float value = 1.0f) {
            self.addObject(WorldObject(x, y, WorldObjectType::Hazard, value, radius));
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("value") = 1.0f,
             R"pbdoc(Add a hazard object at position (x,y) with specified radius and value)pbdoc")
        .def("addMarker", [](SimpleWorld& self, float x, float y, float radius = 0.5f, float value = 0.0f) {
            self.addObject(WorldObject(x, y, WorldObjectType::Marker, value, radius));
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("value") = 0.0f,
             R"pbdoc(Add a marker object at position (x,y) with specified radius and value)pbdoc")
        .def("countObjectsByType", [](const SimpleWorld& self, WorldObjectType type) {
            int count = 0;
            auto percept = self.getSensoryPercept();
            // This is a simplified version - in reality, we'd need to store object references
            return count;
        }, py::arg("type"), R"pbdoc(Count objects of a specific type in the world)pbdoc");
    
    // Create convenience functions for common world setups
    m.def("createGridWorld", [](int width, int height, float cellSize = 1.0f) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Create grid walls
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    world->addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, cellSize * 0.8f));
                }
            }
        }
        
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("cellSize") = 1.0f,
        R"pbdoc(Create a simple grid world with boundary walls)pbdoc");
    
    m.def("createMazeWorld", [](int width, int height, float cellSize = 1.0f) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        
        // Create simple maze pattern (walls at even coordinates)
        for (int x = 0; x < width; x += 2) {
            for (int y = 0; y < height; y += 2) {
                if (x < width - 2) {
                    world->addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, cellSize * 0.8f));
                }
                if (y < height - 2) {
                    world->addObject(WorldObject(x, y + 1, WorldObjectType::Wall, 0.0f, cellSize * 0.8f));
                }
            }
        }
        
        // Add some resources and hazards randomly
        for (int i = 0; i < 10; ++i) {
            int x = (rand() % (width - 4)) + 2;
            int y = (rand() % (height - 4)) + 2;
            if ((x + y) % 3 == 0) {
                world->addObject(WorldObject(x, y, WorldObjectType::Resource, 1.0f, cellSize * 0.3f));
            } else {
                world->addObject(WorldObject(x, y, WorldObjectType::Hazard, 1.0f, cellSize * 0.3f));
            }
        }
        
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("cellSize") = 1.0f,
        R"pbdoc(Create a maze world with walls, resources, and hazards)pbdoc");

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
        // getRegion is const/non-const overloaded; bind the mutable one.
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
             "Log brain status");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Advanced agent brain methods
        .def("setLearningRate", &AgentBrain::setLearningRate, py::arg("rate"),
             "Set learning rate for plasticity")
        .def("getLearningRate", &AgentBrain::getLearningRate,
             "Get current learning rate")
        .def("enableSocialLearning", &AgentBrain::enableSocialLearning, py::arg("enable"),
             "Enable social learning subsystem")
        .def("enableSelfModel", &AgentBrain::enableSelfModel, py::arg("enable"),
             "Enable self-model construction")
        .def("enableConceptFormation", &AgentBrain::enableConceptFormation, py::arg("enable"),
             "Enable concept formation subsystem")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get number of formed concepts")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence level")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get count of social observations")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent's behavior")
        .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
             "Get number of planner decisions made")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
             "Get number of planner-agreement matches")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get agent's thought trace (what it perceived, planned, and acted on)")
        .def("getMemoryNeurons", &AgentBrain::getMemoryNeurons,
             "Get neurons currently active in working memory")
        .def("getActiveActionQualities", [](const AgentBrain& self) {
            std::vector<std::pair<MotorCommand, float>> qualities;
            if (self.getBrain() && self.getBrain()->getPlanner()) {
                for (int action = 0; action < 6; ++action) {
                    MotorCommand cmd;
                    switch (static_cast<ActionType>(action)) {
                        case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                        case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                        case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                        case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                        case ActionType::Interact: cmd = MotorCommand::Interact; break;
                        default: continue;
                    }
                    float quality = self.getBrain()->getPlanner()->getActionQuality(static_cast<ActionType>(action));
                    qualities.emplace_back(cmd, quality);
                }
            }
            return qualities;
        }, "Get action qualities from planner for all actions")
        .def("enableEpisodeTracking", &AgentBrain::enableEpisodeTracking, py::arg("enable"),
             "Enable episode tracking for memory consolidation")
        .def("getEpisodeCount", &AgentBrain::getEpisodeCount,
             "Get number of episodes tracked")
        .def("getLastEpisodeReward", &AgentBrain::getLastEpisodeReward,
             "Get reward from last episode");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
