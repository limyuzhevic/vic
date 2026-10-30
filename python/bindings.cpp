#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <numeric>
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Simple, Pythonic API for brain simulation
        - Advanced agent training and learning capabilities
        - Multiple world scenarios (arena, maze, resource collection, obstacle course)
        - Development stages and neuromodulation
        - Easy integration with custom environments
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== ID TYPES ==========
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

    // ========== ENUMERATIONS ==========
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

    // ========== CONFIGURATION ==========
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
        });

    // Configuration presets
    m.def("createDefaultConfig", [](const std::string& preset) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (preset == "young") {
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        } else if (preset == "adult") {
            config->set("neuron_count", 2000, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
        } else {
            config->set("neuron_count", 1000, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
        }
        return config;
    }, py::arg("preset") = "default", "Create a configuration with preset settings");

    m.def("createBrain", [](std::shared_ptr<Config> config, const std::string& name) -> std::shared_ptr<Brain> {
        if (!config) {
            config = std::make_shared<Config>();
        }
        config->set("brain_name", name, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("config") = nullptr, py::arg("name") = "brain",
       "Create a brain with optional configuration and name");

    // ========== SENSORY SYSTEMS ==========
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

    // ========== MOTOR SYSTEMS ==========
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

    // ========== WORLD INTERFACE ==========
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

    // ========== WORLD SCENARIOS ==========
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // World creation functions
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createArenaWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, 
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create an arena world with configurable dimensions");

    m.def("createMazeWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight, 
                                const std::vector<std::string>& mazeLayout) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        // Simple maze generation
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                if (mazeLayout[y].size() > x && mazeLayout[y][x] == '#') {
                    WorldObject wall;
                    wall.x = static_cast<float>(x);
                    wall.y = static_cast<float>(y);
                    wall.type = WorldObjectType::Wall;
                    world->addObject(wall);
                }
            }
        }
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       py::arg("mazeLayout") = std::vector<std::string>(),
       "Create a maze world with walls");

    m.def("createResourceWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight, 
                                    size_t numResources, size_t numHazards) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        // Add resources
        for (size_t i = 0; i < numResources; ++i) {
            WorldObject resource;
            resource.x = static_cast<float>(rand() % width);
            resource.y = static_cast<float>(rand() % height);
            resource.type = WorldObjectType::Resource;
            resource.value = 1.0f;
            world->addObject(resource);
        }
        
        // Add hazards
        for (size_t i = 0; i < numHazards; ++i) {
            WorldObject hazard;
            hazard.x = static_cast<float>(rand() % width);
            hazard.y = static_cast<float>(rand() % height);
            hazard.type = WorldObjectType::Hazard;
            hazard.value = -1.0f;
            world->addObject(hazard);
        }
        
        return world;
    }, py::arg("width") = 30, py::arg("height") = 30,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       py::arg("numResources") = 15, py::arg("numHazards") = 5,
       "Create a resource collection world with rewards and hazards");

    m.def("createObstacleWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        // Create outer walls
        for (size_t x = 0; x < width; ++x) {
            WorldObject wall1;
            wall1.x = static_cast<float>(x);
            wall1.y = 0.0f;
            wall1.type = WorldObjectType::Wall;
            world->addObject(wall1);
            
            WorldObject wall2;
            wall2.x = static_cast<float>(x);
            wall2.y = static_cast<float>(height - 1);
            wall2.type = WorldObjectType::Wall;
            world->addObject(wall2);
        }
        
        for (size_t y = 0; y < height; ++y) {
            WorldObject wall1;
            wall1.x = 0.0f;
            wall1.y = static_cast<float>(y);
            wall1.type = WorldObjectType::Wall;
            world->addObject(wall1);
            
            WorldObject wall2;
            wall2.x = static_cast<float>(width - 1);
            wall2.y = static_cast<float>(y);
            wall2.type = WorldObjectType::Wall;
            world->addObject(wall2);
        }
        
        return world;
    }, py::arg("width") = 40, py::arg("height") = 20,
       py::arg("visionWidth") = 10, py::arg("visionHeight") = 8,
       "Create an obstacle course world with walls");

    m.def("createCustomScenario", [](const std::string& scenarioType, 
                                     const std::map<std::string, std::string>& parameters) {
        auto world = std::make_shared<SimpleWorld>();
        
        if (scenarioType == "resource_collection") {
            world->configure(30, 30, 8, 8);
            
            // Add resources
            for (int i = 0; i < 10; ++i) {
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 30);
                resource.y = static_cast<float>(rand() % 30);
                resource.type = WorldObjectType::Resource;
                resource.value = 1.0f;
                world->addObject(resource);
            }
            
            // Add hazards
            for (int i = 0; i < 5; ++i) {
                WorldObject hazard;
                hazard.x = static_cast<float>(rand() % 30);
                hazard.y = static_cast<float>(rand() % 30);
                hazard.type = WorldObjectType::Hazard;
                hazard.value = -1.0f;
                world->addObject(hazard);
            }
            
            // Add start position
            world->setAgentStart(15, 15);
            
        } else if (scenarioType == "obstacle_course") {
            world->configure(40, 20, 10, 8);
            
            // Create walls for obstacle course
            for (int x = 0; x < 40; ++x) {
                WorldObject wall1;
                wall1.x = static_cast<float>(x);
                wall1.y = 0.0f;
                wall1.type = WorldObjectType::Wall;
                world->addObject(wall1);
                
                WorldObject wall2;
                wall2.x = static_cast<float>(x);
                wall2.y = 19.0f;
                wall2.type = WorldObjectType::Wall;
                world->addObject(wall2);
            }
            
            for (int y = 1; y < 19; ++y) {
                WorldObject wall1;
                wall1.x = 0.0f;
                wall1.y = static_cast<float>(y);
                wall1.type = WorldObjectType::Wall;
                world->addObject(wall1);
                
                WorldObject wall2;
                wall2.x = 39.0f;
                wall2.y = static_cast<float>(y);
                wall2.type = WorldObjectType::Wall;
                world->addObject(wall2);
            }
            
            world->setAgentStart(5, 10);
        }
        
        return world;
    }, py::arg("scenarioType") = "resource_collection", 
       py::arg("parameters") = std::map<std::string, std::string>(),
       "Create a pre-configured simulation scenario");

    // ========== AGENT BRAIN INTERFACE ==========
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
        
        // Enhanced error handling and validation for runEpisode
        .def("runEpisode", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world, 
                             size_t steps, bool enableLearning) {
            if (!self) {
                throw py::value_error("AgentBrain cannot be null");
            }
            if (!world) {
                throw py::value_error("World cannot be null");
            }
            if (steps == 0) {
                throw py::value_error("Number of steps must be greater than 0");
            }
            if (steps > 1000000) {
                throw py::value_error("Number of steps cannot exceed 1,000,000");
            }
            
            self->reset();
            self->initialize(*world);
            
            double totalReward = 0.0;
            size_t curiosityEvents = 0;
            
            for (size_t step = 0; step < steps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process input
                self->processSensoryInput(percept);
                self->updateDevelopment(0.1);
                
                // Decode action
                MotorCommand action = self->decodeMotorCommand();
                
                // Apply action to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Simple reward system based on action type
                float reward = 0.0f;
                if (action == MotorCommand::MoveForward) {
                    reward = 0.1f;  // Small positive reward for moving
                }
                
                // Apply learning
                if (enableLearning) {
                    self->applyRewardModulation(reward, 0.0f);
                }
                
                // Track curiosity events
                if (self->getCuriosityLevel() > 0.5f) {
                    curiosityEvents++;
                }
                
                totalReward += reward;
            }
            
            return std::make_tuple(totalReward, curiosityEvents, self->getPredictionError());
        }, py::arg("world"), py::arg("steps") = 100, py::arg("enableLearning") = true,
           "Run a complete episode in the world and return (total_reward, curiosity_events, prediction_error). "
           "Valid steps: 1-1000000. Enables learning by default if enableLearning=True.")
        
        // Enhanced error handling and validation for train method
        .def("train", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world, 
                         size_t episodes, size_t stepsPerEpisode, bool verbose) {
            if (!self) {
                throw py::value_error("AgentBrain cannot be null");
            }
            if (!world) {
                throw py::value_error("World cannot be null");
            }
            if (episodes == 0) {
                throw py::value_error("Number of episodes must be greater than 0");
            }
            if (episodes > 1000) {
                throw py::value_error("Number of episodes cannot exceed 1000");
            }
            if (stepsPerEpisode == 0) {
                throw py::value_error("Number of steps per episode must be greater than 0");
            }
            if (stepsPerEpisode > 10000) {
                throw py::value_error("Number of steps per episode cannot exceed 10000");
            }
            
            std::vector<double> episodeRewards;
            std::vector<double> curiosityLevels;
            double bestReward = -std::numeric_limits<double>::max();
            size_t bestEpisode = 0;
            
            self->enableRewardModulation(true);
            self->enableCuriosity(true);
            
            for (size_t episode = 0; episode < episodes; ++episode) {
                // Reset and initialize
                self->reset();
                self->initialize(*world);
                
                double episodeReward = 0.0;
                double maxCuriosity = 0.0;
                
                for (size_t step = 0; step < stepsPerEpisode; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    
                    self->processSensoryInput(percept);
                    self->updateDevelopment(0.1);
                    
                    MotorCommand action = self->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Simple reward
                    float reward = (action != MotorCommand::Wait) ? 0.1f : -0.01f;
                    self->applyRewardModulation(reward, 0.0f);
                    
                    episodeReward += reward;
                    maxCuriosity = std::max(maxCuriosity, self->getCuriosityLevel());
                }
                
                episodeRewards.push_back(episodeReward);
                curiosityLevels.push_back(maxCuriosity);
                
                if (verbose && (episode % 10 == 0 || episode == episodes - 1)) {
                    std::cout << "Episode " << episode << ": Reward = " << std::fixed << std::setprecision(2) 
                             << episodeReward << ", Max Curiosity = " << std::setprecision(3) << maxCuriosity << std::endl;
                }
                
                if (episodeReward > bestReward) {
                    bestReward = episodeReward;
                    bestEpisode = episode;
                }
                
                // Small decay between episodes to prevent runaway behavior
                if (episode < episodes - 1) {
                    self->applyRewardModulation(-0.01f, 0.0f);
                }
            }
            
            double avgReward = 0.0;
            double avgCuriosity = 0.0;
            if (!episodeRewards.empty()) {
                double sumRewards = std::accumulate(episodeRewards.begin(), episodeRewards.end(), 0.0);
                double sumCuriosity = std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0);
                avgReward = sumRewards / episodeRewards.size();
                avgCuriosity = sumCuriosity / curiosityLevels.size();
            }
            
            return std::make_tuple(
                episodeRewards,  // all episode rewards
                curiosityLevels, // all max curiosity levels per episode
                avgReward,       // average reward
                avgCuriosity,    // average curiosity
                bestEpisode,     // best episode index
                bestReward       // best reward achieved
            );
        }, py::arg("world"), py::arg("episodes") = 100, py::arg("stepsPerEpisode") = 100, py::arg("verbose") = false,
           "Train the agent for multiple episodes and return detailed statistics. "
           "Valid ranges: episodes (1-1000), stepsPerEpisode (1-10000). "
           "Returns (episode_rewards, curiosity_levels, avg_reward, avg_curiosity, best_episode, best_reward)")
        
        // Enhanced error handling and validation for trainSimple method
        .def("trainSimple", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world) {
            if (!self) {
                throw py::value_error("AgentBrain cannot be null");
            }
            if (!world) {
                throw py::value_error("World cannot be null");
            }
            
            self->reset();
            self->enableRewardModulation(true);
            self->enableCuriosity(true);
            
            double totalReward = 0.0;
            for (size_t step = 0; step < 100; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self->processSensoryInput(percept);
                self->updateDevelopment(0.1);
                
                MotorCommand action = self->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                float reward = 0.1f;
                self->applyRewardModulation(reward, 0.0f);
                totalReward += reward;
            }
            
            return totalReward;
        }, py::arg("world"), "Quick training method for simple scenarios with validation. Returns total reward accumulated during training.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced agent training methods
    m.def("createTrainingScenario", [](const std::string& scenarioType, 
                                      const std::map<std::string, double>& parameters) {
        auto world = std::make_shared<SimpleWorld>();
        
        if (scenarioType == "random_walk") {
            world->configure(40, 40, 8, 8);
            world->setAgentStart(20, 20);
            
            // Add scattered resources for exploration
            for (int i = 0; i < 20; ++i) {
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 40);
                resource.y = static_cast<float>(rand() % 40);
                resource.type = WorldObjectType::Resource;
                resource.value = 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 2.0f;
                world->addObject(resource);
            }
            
        } else if (scenarioType == "goal_achievement") {
            world->configure(50, 50, 12, 12);
            world->setAgentStart(10, 10);
            
            // Add goal zone
            WorldObject goal;
            goal.x = 40.0f;
            goal.y = 40.0f;
            goal.type = WorldObjectType::Marker;
            goal.value = 10.0f;
            goal.radius = 3.0f;
            world->addObject(goal);
            
            // Add some resources along the way
            for (int i = 0; i < 10; ++i) {
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 45);
                resource.y = static_cast<float>(rand() % 45);
                resource.type = WorldObjectType::Resource;
                resource.value = 1.0f;
                world->addObject(resource);
            }
            
        } else if (scenarioType == "avoidance") {
            world->configure(60, 40, 10, 8);
            world->setAgentStart(5, 5);
            
            // Add moving hazards
            for (int i = 0; i < 5; ++i) {
                WorldObject hazard;
                hazard.x = static_cast<float>(i * 10);
                hazard.y = 20.0f;
                hazard.type = WorldObjectType::Hazard;
                hazard.value = -5.0f;
                world->addObject(hazard);
            }
            
        } else if (scenarioType == "sparse_reward") {
            world->configure(35, 35, 8, 8);
            world->setAgentStart(15, 15);
            
            // Add very few rewards to encourage exploration
            for (int i = 0; i < 3; ++i) {
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 35);
                resource.y = static_cast<float>(rand() % 35);
                resource.type = WorldObjectType::Resource;
                resource.value = 5.0f;  // High reward to encourage finding them
                world->addObject(resource);
            }
        }
        
        return world;
    }, py::arg("scenarioType") = "random_walk", 
       py::arg("parameters") = std::map<std::string, double>(),
       "Create a specialized training scenario with specific objectives");

    m.def("createAdvancedTrainingWorld", [](const std::string& trainingMode,
                                           const std::string& difficulty = "medium") {
        auto world = std::make_shared<SimpleWorld>();
        
        if (trainingMode == "reinforcement_learning") {
            world->configure(50, 50, 12, 12);
            world->setAgentStart(10, 10);
            
            // Create diverse environment for RL
            for (int i = 0; i < 25; ++i) {
                // Resources with different values
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 50);
                resource.y = static_cast<float>(rand() % 50);
                resource.type = WorldObjectType::Resource;
                resource.value = (static_cast<float>(rand()) / RAND_MAX) * 3.0f + 0.5f;
                world->addObject(resource);
            }
            
            // Hazards for negative reinforcement
            for (int i = 0; i < 8; ++i) {
                WorldObject hazard;
                hazard.x = static_cast<float>((i * 6) % 50);
                hazard.y = static_cast<float>((i * 4) % 50);
                hazard.type = WorldObjectType::Hazard;
                hazard.value = -2.0f - (static_cast<float>(rand()) / RAND_MAX) * 3.0f;
                world->addObject(hazard);
            }
            
        } else if (trainingMode == "curiosity_driven") {
            world->configure(45, 45, 10, 10);
            world->setAgentStart(5, 5);
            
            // Many novel objects to trigger curiosity
            for (int i = 0; i < 30; ++i) {
                WorldObject object;
                object.x = static_cast<float>(rand() % 45);
                object.y = static_cast<float>(rand() % 45);
                object.type = static_cast<WorldObjectType>(rand() % 5);
                object.value = 0.5f + (static_cast<float>(rand()) / RAND_MAX) * 1.5f;
                world->addObject(object);
            }
        }
        
        return world;
    }, py::arg("trainingMode") = "reinforcement_learning",
       py::arg("difficulty") = "medium",
       "Create an advanced training world with specific learning objectives");

    m.def("createCurriculumTraining", [](const std::vector<std::string>& stages) {
        // Create a curriculum where each stage builds on the previous one
        std::vector<std::shared_ptr<SimpleWorld>> curriculumWorlds;
        
        for (const auto& stage : stages) {
            auto world = std::make_shared<SimpleWorld>();
            
            if (stage == "stage1_simple") {
                world->configure(20, 20, 6, 6);
                world->setAgentStart(10, 10);
                // Add few resources - basic navigation
            } else if (stage == "stage2_intermediate") {
                world->configure(35, 35, 8, 8);
                world->setAgentStart(5, 5);
                // Add moderate resources - path finding
                for (int i = 0; i < 8; ++i) {
                    WorldObject resource;
                    resource.x = static_cast<float>(rand() % 35);
                    resource.y = static_cast<float>(rand() % 35);
                    resource.type = WorldObjectType::Resource;
                    resource.value = 1.0f;
                    world->addObject(resource);
                }
            } else if (stage == "stage3_advanced") {
                world->configure(40, 40, 10, 10);
                world->setAgentStart(15, 15);
                // Add complex environment - obstacle avoidance
                for (int i = 0; i < 15; ++i) {
                    WorldObject obj;
                    obj.x = static_cast<float>(rand() % 40);
                    obj.y = static_cast<float>(rand() % 40);
                    obj.type = (rand() % 3 == 0) ? WorldObjectType::Resource : WorldObjectType::Hazard;
                    obj.value = 1.0f;
                    world->addObject(obj);
                }
            }
            
            curriculumWorlds.push_back(world);
        }
        
        return curriculumWorlds;
    }, py::arg("stages") = std::vector<std::string>{
        "stage1_simple", "stage2_intermediate", "stage3_advanced"
    }, "Create a curriculum learning progression across multiple stages");

    // Brain creation convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createYoungConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        return config;
    }, "Create a 'young' brain configuration with high plasticity");

    m.def("createAdultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
        return config;
    }, "Create an 'adult' brain configuration with stable connectivity");

    // Convenience function for common agent setup
    m.def("createAgent", [](const std::string& configPreset = "default") -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = createDefaultConfig(configPreset);
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 8, 8);
        world->setAgentStart(15, 15);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("configPreset") = "default", "Create a complete agent setup (brain, world, agent) with default configuration");

    // ========== CONSTANTS AND UTILITIES ==========
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Training scenario presets
    m.def("getTrainingScenarios", []() {
        return std::vector<std::string>{"simple", "resource_collection", "obstacle_course", "maze", "complex"};
    }, "Get available training scenario types");

    m.def("createTrainingWorld", [](const std::string& scenario, const std::string& configPreset = "default") {
        // Create a training world based on the scenario
        auto world = std::make_shared<SimpleWorld>();
        
        if (scenario == "simple") {
            world->configure(20, 20, 6, 6);
            world->setAgentStart(10, 10);
        } else if (scenario == "resource_collection") {
            world->configure(30, 30, 8, 8);
            world->setAgentStart(15, 15);
            // Add some resources
            for (int i = 0; i < 8; ++i) {
                WorldObject resource;
                resource.x = static_cast<float>(rand() % 30);
                resource.y = static_cast<float>(rand() % 30);
                resource.type = WorldObjectType::Resource;
                resource.value = 1.0f;
                world->addObject(resource);
            }
        } else if (scenario == "obstacle_course") {
            world->configure(40, 20, 10, 8);
            // Create walls (same as createObstacleWorld)
            for (size_t x = 0; x < 40; ++x) {
                WorldObject wall1;
                wall1.x = static_cast<float>(x);
                wall1.y = 0.0f;
                wall1.type = WorldObjectType::Wall;
                world->addObject(wall1);
                
                WorldObject wall2;
                wall2.x = static_cast<float>(x);
                wall2.y = 19.0f;
                wall2.type = WorldObjectType::Wall;
                world->addObject(wall2);
            }
            for (size_t y = 0; y < 20; ++y) {
                WorldObject wall1;
                wall1.x = 0.0f;
                wall1.y = static_cast<float>(y);
                wall1.type = WorldObjectType::Wall;
                world->addObject(wall1);
                
                WorldObject wall2;
                wall2.x = 39.0f;
                wall2.y = static_cast<float>(y);
                wall2.type = WorldObjectType::Wall;
                world->addObject(wall2);
            }
            world->setAgentStart(5, 10);
        }
        
        return world;
    }, py::arg("scenario") = "simple", py::arg("configPreset") = "default",
       "Create a training world for agent training");
}

} // namespace nlm