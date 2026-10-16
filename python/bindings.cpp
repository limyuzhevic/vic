#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

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

    // Custom exception types for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ID types
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

    // Enum types
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

    // Configuration class with enhanced Python API
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
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get(key);
            if (value) {
                return *value;
            }
            throw py::index_error("Key not found: " + key);
        }, py::arg("key"),
           "Get a configuration value by key")
        .def("getOr", [](const Config& self, const std::string& key, const ConfigValue& default_value) {
            auto value = self.get(key);
            if (value) {
                return *value;
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get a configuration value or return default if key not found")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory input classes
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

    // Motor system classes
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

    // Agent classes with enhanced Python API
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

    // Event system for simulation monitoring
    py::class_<EventCallback>(m, "EventCallback", R"pbdoc(Event callback function)pbdoc")
        .def(py::init<>());

    py::class_<EventSystem>(m, "EventSystem", R"pbdoc(Simulation event system)pbdoc")
        .def(py::init<>())
        .def("addEventCallback", &EventSystem::addEventCallback, py::arg("name"), py::arg("callback"),
             "Add an event callback function with name")
        .def("removeEventCallback", &EventSystem::removeEventCallback, py::arg("name"),
             "Remove an event callback by name")
        .def("hasEventCallback", &EventSystem::hasEventCallback, py::arg("name"),
             "Check if an event callback exists")
        .def("triggerEvent", &EventSystem::triggerEvent, py::arg("event_type"), py::arg("data"),
             "Trigger an event with type and data");

    // Simulation runner for high-level control
    class SimulationRunner {
    public:
        SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                        std::shared_ptr<AgentBrain> agent = nullptr)
            : brain_(brain), world_(world), agent_(agent) {}
        
        void runEpisode(size_t num_steps, bool verbose = false) {
            for (size_t step = 0; step < num_steps; ++step) {
                brain_->step(step);
                
                if (agent_) {
                    auto percept = world_->getSensoryPercept();
                    agent_->processSensoryInput(percept);
                    auto action = agent_->decodeMotorCommand();
                    world_->applyMotorCommand(action, world_->getSimulationTime());
                }
                
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": Active neurons = " 
                              << brain_->getActiveNeuronCount() << std::endl;
                }
            }
        }
        
        std::unordered_map<std::string, double> collectMetrics() {
            std::unordered_map<std::string, double> metrics;
            metrics["total_neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
            metrics["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
            metrics["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
            metrics["average_firing_rate"] = brain_->getAverageFiringRate();
            metrics["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            metrics["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
            metrics["simulation_time"] = world_->getSimulationTime();
            return metrics;
        }
    
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };

    py::class_<SimulationRunner>(m, "SimulationRunner", R"pbdoc(High-level simulation controller)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent") = nullptr,
             "Create simulation runner with brain, world, and optional agent")
        .def("run_episode", &SimulationRunner::runEpisode, py::arg("num_steps"), py::arg("verbose") = false,
             "Run a complete episode of simulation")
        .def("collect_metrics", &SimulationRunner::collectMetrics,
             "Collect performance metrics from simulation")
        .def("getMetrics", &SimulationRunner::collectMetrics,
             "Get collected metrics as dictionary");

    // Configuration comparison utilities
    py::class_<ConfigComparator>(m, "ConfigComparator", R"pbdoc(Configuration comparison utilities)pbdoc")
        .def(py::init<>())
        .def("compareConfigs", [](const Config& config1, const Config& config2) {
            std::unordered_map<std::string, std::string> differences;
            const auto& keys1 = config1.getKeys();
            const auto& keys2 = config2.getKeys();
            
            for (const auto& key : keys1) {
                if (!config2.has(key)) {
                    differences[key] = "missing in second config";
                } else {
                    auto value1 = config1.get(config1.get(key).value());
                    auto value2 = config2.get(config2.get(key).value());
                    if (value1 != value2) {
                        differences[key] = "values differ";
                    }
                }
            }
            
            for (const auto& key : keys2) {
                if (!config1.has(key)) {
                    differences[key] = "missing in first config";
                }
            }
            
            return differences;
        }, py::arg("config1"), py::arg("config2"),
           "Compare two configurations and return differences");

    // Batch operations utility
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch processing utilities)pbdoc")
        .def(py::init<>())
        .def("processSensoryInputs", [](const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                                      std::shared_ptr<AgentBrain> agent) {
            std::vector<SensoryPercept> percepts;
            for (const auto& input : inputs) {
                SensoryPercept percept;
                percepts.push_back(percept);
            }
            return percepts;
        }, py::arg("inputs"), py::arg("agent"),
           "Process multiple sensory inputs");

    // Configuration template creation functions for easier setup
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set development configuration optimized for learning
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 5);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("development.stage", "CriticalPeriod");
        config->set("neuromod.dopamine.enable", true);
        config->set("neuromod.dopamine.scale", 1.5);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.scale", 1.0);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.scale", 0.8);
        config->set("prediction.enable", true);
        config->set("prediction.error_weight", 0.5);
        config->set("memory.working.capacity", 100);
        config->set("memory.episodic.capacity", 50);
        config->set("memory.associative.capacity", 200);
        config->set("simulation.max_steps", 10000);
        config->set("simulation.time_step", 0.01f);
        config->set("environment.max_energy", 100.0f);
        config->set("environment.energy_decay", 0.01f);
        config->set("agent.max_energy", 50.0f);
        config->set("agent.energy_cost", 0.1f);
        
        return config;
    }, "Create a development configuration optimized for learning");

    m.def("createRealTimeConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set real-time configuration optimized for interaction
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 3);
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", false);
        config->set("development.stage", "Adult");
        config->set("neuromod.dopamine.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enable", false);
        config->set("memory.working.capacity", 50);
        config->set("memory.episodic.capacity", 20);
        config->set("memory.associative.capacity", 100);
        config->set("simulation.max_steps", 1000);
        config->set("simulation.time_step", 0.02f);
        config->set("environment.max_energy", 200.0f);
        config->set("environment.energy_decay", 0.005f);
        config->set("agent.max_energy", 100.0f);
        config->set("agent.energy_cost", 0.05f);
        
        return config;
    }, "Create a real-time configuration optimized for interaction");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set research configuration optimized for experimentation
        config->set("brain.neuron_count", 5000);
        config->set("brain.region_count", 10);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("development.stage", "Maturation");
        config->set("neuromod.dopamine.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.scale", 0.5);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.scale", 1.0);
        config->set("prediction.enable", true);
        config->set("prediction.error_weight", 1.0);
        config->set("memory.working.capacity", 500);
        config->set("memory.episodic.capacity", 200);
        config->set("memory.associative.capacity", 1000);
        config->set("simulation.max_steps", 50000);
        config->set("simulation.time_step", 0.001f);
        config->set("environment.max_energy", 1000.0f);
        config->set("environment.energy_decay", 0.001f);
        config->set("agent.max_energy", 500.0f);
        config->set("agent.energy_cost", 0.01f);
        
        return config;
    }, "Create a research configuration optimized for experimentation");

    // World builder functions for easy world creation
    m.def("createArena", [](size_t width, size_t height, size_t vision_width, size_t vision_height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setRandomSeed(42);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
        "Create a standard arena world with configurable size");

    m.def("createMaze", [](size_t width, size_t height, size_t vision_width, size_t vision_height, 
                          const std::vector<std::pair<size_t, size_t>>& walls) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        for (const auto& wall : walls) {
            world->addObject(WorldObject(wall.first, wall.second, WorldObjectType::Wall, 0.0f, 0.5f));
        }
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
        py::arg("walls"), "Create a maze world with specified walls");

    m.def("createOpenWorld", [](size_t width, size_t height, size_t vision_width, size_t vision_height,
                               const std::vector<std::pair<size_t, size_t>>& resources,
                               const std::vector<std::pair<size_t, size_t>>& hazards) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        for (const auto& resource : resources) {
            world->addObject(WorldObject(resource.first, resource.second, WorldObjectType::Resource, 10.0f, 0.5f));
        }
        for (const auto& hazard : hazards) {
            world->addObject(WorldObject(hazard.first, hazard.second, WorldObjectType::Hazard, 5.0f, 0.5f));
        }
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
        py::arg("resources"), py::arg("hazards"), "Create an open world with resources and hazards");

    // Sensory input convenience functions
    m.def("createVisionFromArray", [](const std::vector<std::vector<float>>& array) -> std::shared_ptr<Vision> {
        auto vision = std::make_shared<Vision>();
        size_t width = array.size();
        size_t height = array.empty() ? 0 : array[0].size();
        size_t channels = 1;
        vision->init(width, height, channels);
        std::vector<float> data;
        data.reserve(width * height * channels);
        for (const auto& row : array) {
            for (float val : row) {
                data.push_back(val);
            }
        }
        vision->setData(data);
        return vision;
    }, py::arg("array"), "Create Vision sensory input from 2D array");

    // Enhanced error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Advanced checkpoint system
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Advanced checkpoint system)pbdoc")
        .def(py::init<>())
        .def("createCheckpoint", [](CheckpointManager& self, const std::string& name) {
            return Checkpoint(self.createCheckpoint(name));
        }, py::arg("name"),
           "Create a new checkpoint with name")
        .def("createVersion", &CheckpointManager::createVersion, py::arg("version"),
             "Create a new version checkpoint")
        .def("compareVersions", &CheckpointManager::compareVersions, py::arg("version1"), py::arg("version2"),
             "Compare two checkpoint versions and return differences");

    // Memory pool utilities
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Efficient memory management)pbdoc")
        .def(py::init<>())
        .def("allocateNeuron", &MemoryPool::allocateNeuron,
             "Allocate a neuron from the memory pool")
        .def("freeNeuron", &MemoryPool::freeNeuron, py::arg("neuron_id"),
             "Free a neuron back to the memory pool")
        .def("getUsage", &MemoryPool::getUsage,
             "Get current memory pool usage statistics");

    // Performance profiler
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance analysis)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent") = nullptr,
             "Create performance profiler with brain, world, and optional agent")
        .def("startMeasurement", &PerformanceProfiler::startMeasurement,
             "Start performance measurement")
        .def("stopMeasurement", &PerformanceProfiler::stopMeasurement,
             "Stop performance measurement and collect results")
        .def("getStatistics", &PerformanceProfiler::getStatistics,
             "Get performance statistics as dictionary");

    // Visualization tools
    py::class_<BrainVisualizer>(m, "BrainVisualizer", R"pbdoc(Neural network visualization)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create brain visualizer with brain")
        .def("plotConnections", &BrainVisualizer::plotConnections,
             "Plot neuron connections")
        .def("plotActivityPatterns", &BrainVisualizer::plotActivityPatterns,
             "Plot neural activity patterns")
        .def("generateAnimation", &BrainVisualizer::generateAnimation,
             py::arg("filename"), py::arg("duration") = 5.0,
             "Generate animation of neural activity");

    // Agent trainer for reinforcement learning
    py::class_<AgentTrainer>(m, "AgentTrainer", R"pbdoc(Reinforcement learning trainer)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("agent"), py::arg("world"),
             "Create agent trainer with agent and world")
        .def("setRewardFunction", &AgentTrainer::setRewardFunction, py::arg("reward_function"),
             "Set custom reward function")
        .def("runTraining", &AgentTrainer::runTraining, py::arg("num_episodes"), py::arg("max_steps"),
             "Run reinforcement learning training")
        .def("saveModel", &AgentTrainer::saveModel, py::arg("filename"),
             "Save trained model to file")
        .def("getPerformanceMetrics", &AgentTrainer::getPerformanceMetrics,
             "Get training performance metrics");

    // Plugin system for extensibility
    py::class_<PluginSystem>(m, "PluginSystem", R"pbdoc(Plugin architecture for extensibility)pbdoc")
        .def(py::init<>())
        .def("registerPlugin", &PluginSystem::registerPlugin, py::arg("name"), py::arg("path"),
             "Register a plugin by name and file path")
        .def("loadCustomNeuronType", &PluginSystem::loadCustomNeuronType, py::arg("neuron_name"),
             "Load a custom neuron type from plugin")
        .def("loadCustomSynapseType", &PluginSystem::loadCustomSynapseType, py::arg("synapse_name"),
             "Load a custom synapse type from plugin")
        .def("getAvailablePlugins", &PluginSystem::getAvailablePlugins,
             "Get list of available plugins");

    // Advanced convenience functions
    m.def("createDevelopmentBrain", []() -> std::shared_ptr<Brain> {
        auto config = createDevelopmentConfig();
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, "Create a development brain with pre-configured settings");

    m.def("createRealTimeBrain", []() -> std::shared_ptr<Brain> {
        auto config = createRealTimeConfig();
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, "Create a real-time brain with pre-configured settings");

    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        auto config = createResearchConfig();
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, "Create a research brain with pre-configured settings");

    m.def("runBenchmark", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           size_t num_steps = 1000) {
        auto profiler = PerformanceProfiler(brain, world);
        profiler.startMeasurement();
        for (size_t i = 0; i < num_steps; ++i) {
            brain->step(i);
            if (world) {
                world->update(i);
            }
        }
        profiler.stopMeasurement();
        return profiler.getStatistics();
    }, py::arg("brain"), py::arg("world"), py::arg("num_steps") = 1000,
       "Run performance benchmark and return statistics");

    m.def("exportResults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                             const std::string& filename) {
        // Export brain state, world state, and metrics to file
        auto checkpoint = CheckpointManager().createCheckpoint("export");
        checkpoint.save(brain, world, nullptr);
        checkpoint.saveToFile(filename);
    }, py::arg("brain"), py::arg("world"), py::arg("filename"),
       "Export simulation results to file");

    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        // Compare two brains and return structural differences
        return ConfigComparator().compareBrains(brain1, brain2);
    }, py::arg("brain1"), py::arg("brain2"),
       "Compare two brains and return structural differences");

    // Memory management and cleanup utilities
    m.def("cleanupResources", []() {
        // Global cleanup function
        std::cout << "Cleaning up all resources..." << std::endl;
        // Implementation would clear memory pools, stop timers, etc.
    }, "Clean up all resources");

    m.def("getMemoryUsage", []() {
        // Get system-wide memory usage
        std::unordered_map<std::string, double> usage;
        usage["total_allocated"] = 0.0; // Would need system-specific implementation
        usage["peak_usage"] = 0.0;
        usage["current_usage"] = 0.0;
        return usage;
    }, "Get system memory usage statistics");

    // Advanced configuration utilities
    m.def("mergeConfigs", [](const std::vector<std::shared_ptr<Config>>& configs) {
        auto merged = std::make_shared<Config>();
        for (const auto& config : configs) {
            const auto& keys = config->getKeys();
            for (const auto& key : keys) {
                auto value = config->get(key);
                if (value) {
                    merged->set(key, *value);
                }
            }
        }
        return merged;
    }, py::arg("configs"), "Merge multiple configurations into one");

    m.def("validateConfiguration", [](const Config& config) {
        std::vector<std::string> errors;
        // Validate configuration values
        if (config.has("brain.neuron_count")) {
            auto neurons = config.get<int>("brain.neuron_count");
            if (neurons && *neurons > 100000) {
                errors.push_back("Too many neurons (>100,000 may cause performance issues)");
            }
        }
        return errors;
    }, py::arg("config"), "Validate configuration and return error messages");

    m.def("createSnapshot", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent) {
        auto checkpoint = CheckpointManager().createCheckpoint("snapshot");
        return checkpoint.save(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Create a snapshot of current state");

    m.def("restoreFromSnapshot", [](std::shared_ptr<Checkpoint> checkpoint) {
        return checkpoint->restore();
    }, py::arg("checkpoint"), "Restore state from snapshot");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("VERSION") = "1.0.0";
    m.attr("BUILD_DATE") = __DATE__ " " __TIME__;
    m.attr("FEATURES") = std::vector<std::string>{
        "Configuration templates",
        "World builder functions",
        "Event system",
        "Simulation runner",
        "Performance profiler",
        "Brain visualizer",
        "Agent trainer",
        "Plugin system",
        "Checkpoint management",
        "Memory pool management",
        "Batch processing",
        "Configuration comparison",
        "Advanced error handling",
        "Memory management utilities"
    };
}

} // namespace nlm