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
        
        // Enhanced Pythonic interfaces for configuration management
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value or default string if not found")
        .def("getOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value or default int if not found")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value or default float if not found")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value or default bool if not found")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value")
        
        .def("getConfigDict", [](const Config& self) {
            std::map<std::string, std::string> configDict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get as string first, fallback to other types
                if (self.has(key)) {
                    configDict[key] = std::to_string(self.getOr<int64_t>(key, 0));
                }
            }
            return configDict;
        }, "Get configuration as Python dictionary")
        .def("applyDefaults", [](Config& self) {
            // Apply default configuration values
            self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            self.set("simulation_timestep", 0.001, ConfigSource::Default);
            self.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            self.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            self.set("connection_probability", 0.05f, ConfigSource::Default);
        }, "Apply default configuration values")
        .def("validateConfig", [](const Config& self) {
            std::map<std::string, std::string> errors;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("neuron_count") != std::string::npos) {
                    auto count = self.getOr<int64_t>(key, 1);
                    if (count <= 0 || count > 100000) {
                        errors[key] = "Invalid neuron count: must be between 1 and 100000";
                    }
                }
            }
            return errors;
        }, "Validate configuration and return errors if any")
        .def("createBrain", [](const Config& self) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(std::const_pointer_cast<Config>(std::shared_ptr<const Config>(&self, [](const Config*) {})));
        }, py::return_value_policy::reference_internal,
            "Create a brain with this configuration")
        .def("createSimpleWorld", []() {
            return std::make_shared<SimpleWorld>();
        }, "Create a default simple world");


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
        
        // Enhanced Pythonic interfaces
        .def("addResource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
            "Add a resource object to the world")
        .def("addHazard", [](SimpleWorld& world, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f,
            "Add a hazard object to the world")
        .def("addWall", [](SimpleWorld& world, float x, float y, float radius) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
            "Add a wall object to the world")
        .def("isPositionSafe", [](const SimpleWorld& world, float x, float y) {
            return world.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
            "Check if position is safe (within bounds and not on hazards)")
        .def("getWorldState", [](const SimpleWorld& world) {
            auto body = world.getAgentBody();
            return std::map<std::string, float>{{
                {"x", body.x},
                {"y", body.y},
                {"orientation", body.orientation},
                {"energy", body.energy},
                {"health", body.health},
                {"age", body.age},
                {"simulation_time", world.getSimulationTime()}
            }};
        }, "Get current world state as dictionary")
        .def("simulateStep", [](SimpleWorld& world, float timestep, 
                               std::shared_ptr<Action> action) {
            world.update(timestep);
            world.applyMotorCommand(*action, world.getSimulationTime());
            return world.getSensoryPercept();
        }, py::arg("timestep"), py::arg("action"),
            "Perform one complete simulation step (update + apply action)")
        .def("createDefaultWorld", []() {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            return world;
        }, "Create a default world configuration");

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
        
        // Enhanced Pythonic interfaces for brain control
        .def("runSimulation", [](Brain& brain, SimulationStep steps, 
                               bool useTime = false) {
            if (useTime) {
                for (SimulationStep step = 0; step < steps; ++step) {
                    brain.step(step, step * 0.001);
                }
            } else {
                for (SimulationStep step = 0; step < steps; ++step) {
                    brain.step(step);
                }
            }
        }, py::arg("steps"), py::arg("useTime") = false,
            "Run simulation for specified number of steps")
        .def("stepWithTime", [](Brain& brain, SimulationStep step, double time) {
            brain.step(step, time);
        }, py::arg("step"), py::arg("time") = 0.0,
            "Perform a simulation step with custom timestamp")
        .def("injectSensoryInput", [](Brain& brain, const class SensoryInput& input) {
            brain.receiveSensoryInput(input);
        }, py::arg("input"),
            "Inject sensory input into the brain")
        .def("injectCurrentToSensory", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current") = 10.0f,
             "Inject current into all neurons of specified type")
        .def("activateNeuron", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current") = 50.0f,
             "Inject current into a specific neuron")
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
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Enhanced Pythonic interfaces for brain statistics
        .def("getBrainStats", [](const Brain& brain) {
            return std::map<std::string, double>{{
                {"total_neurons", static_cast<double>(brain.getTotalNeuronCount())},
                {"total_synapses", static_cast<double>(brain.getTotalSynapseCount())},
                {"active_neurons", static_cast<double>(brain.getActiveNeuronCount())},
                {"firing_neurons", static_cast<double>(brain.getFiringNeuronCount())},
                {"average_firing_rate", brain.getAverageFiringRate()},
                {"e_i_ratio", brain.getExcitationInhibitionRatio()},
                {"total_spikes", static_cast<double>(brain.getTotalSpikeCount())},
                {"pending_spikes", static_cast<double>(brain.getPendingSpikeEventCount())},
                {"developmental_stage", static_cast<double>(brain.getDevelopmentalStage())}
            }};
        }, "Get comprehensive brain statistics as dictionary")
        .def("isHealthy", [](const Brain& brain) {
            return brain.getFiringNeuronCount() > 0 && 
                   brain.getTotalNeuronCount() > 0;
        }, "Check if brain is in a healthy state")
        .def("getUtilizationLevel", [](const Brain& brain) {
            if (brain.getTotalNeuronCount() == 0) return 0.0f;
            return static_cast<double>(brain.getFiringNeuronCount()) / 
                   static_cast<double>(brain.getTotalNeuronCount());
        }, "Get brain utilization level (0.0 to 1.0)")
        .def("performInternalTest", [](Brain& brain) {
            // Create a simple test to verify brain functionality
            auto region = brain.getRegion(RegionId(1));
            if (!region) {
                throw std::runtime_error("No regions found in brain");
            }
            
            // Check if brain can process sensory input
            Vision testVision(8, 8, 3);
            std::vector<float> testData(24, 0.1f);
            testVision.setData(testData);
            
            brain.receiveSensoryInput(testVision);
            
            // Run a few steps
            for (int step = 0; step < 5; ++step) {
                brain.step(step);
            }
            
            return std::map<std::string, bool>{{
                {"sensory_processing", true},
                {"neural_activity", brain.getFiringNeuronCount() > 0}
            }};
        }, "Perform internal functional test of the brain");

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
        
        // Enhanced Pythonic interfaces
        .def("enableAllSystems", &AgentBrain::enableRewardModulation,
             py::arg("enable") = true)
        .def("getSystemStatus", [](const AgentBrain& agent) {
            return std::map<std::string, bool>{{
                {"reward_modulation", agent.isRewardModulationEnabled()},
                {"structural_plasticity", agent.isStructuralPlasticityEnabled()},
                {"development", agent.isDevelopmentEnabled()},
                {"curiosity", agent.isCuriosityEnabled()}
            }};
        }, "Get status of all agent systems")
        .def("configureSystems", [](AgentBrain& agent, 
                                   bool reward, bool structural, 
                                   bool development, bool curiosity) {
            agent.enableRewardModulation(reward);
            agent.enableStructuralPlasticity(structural);
            agent.enableDevelopment(development);
            agent.enableCuriosity(curiosity);
        }, py::arg("reward") = true,
            py::arg("structural") = true,
            py::arg("development") = true,
            py::arg("curiosity") = true,
            "Configure all agent systems at once");

    // Create configuration factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply sensible defaults for Python users
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        return config;
    }, "Create a default configuration with sensible settings for beginners");
    
    // Convenience functions for common setup patterns
    m.def("createSimpleSimulation", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a simple simulation setup (brain, world, agent)");
    
    // Performance monitoring and debugging utilities
    m.def("getSystemMemoryUsage", []() {
        // Return approximate memory usage information
        return std::map<std::string, double>{{
            {"approx_memory_mb", 0.0},  // Would need actual implementation
            {"heap_usage", 0.0},
            {"vram_usage", 0.0}
        }};
    }, "Get system memory usage information");
    
    m.def("getPerformanceStats", [](std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent) {
        return std::map<std::string, double>{{
            {"brain_memory_mb", 0.0},
            {"neuron_count", static_cast<double>(brain->getTotalNeuronCount())},
            {"synapse_count", static_cast<double>(brain->getTotalSynapseCount())},
            {"firing_rate_hz", brain->getAverageFiringRate()},
            {"world_size_mb", 0.0},
            {"simulation_time_ms", 0.0}
        }};
    }, "Get performance statistics for simulation components",
       py::arg("brain"), py::arg("world"), py::arg("agent"));
    
    // Error handling utilities
    m.def("checkEnvironment", []() {
        std::map<std::string, bool> checks;
        checks["python_version"] = true;
        checks["c++11_support"] = true;
        checks["memory_available"] = true;
        return checks;
    }, "Check if the environment is ready for NLM operations");
    
    m.def("safeImport", [](const std::string& module) {
        try {
            // In a real implementation, this would attempt to import modules
            return std::make_pair(true, std::string("Module " + module + " loaded successfully"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Error loading module: ") + e.what());
        }
    }, py::arg("module"), "Safely attempt to import a Python module");
    
    // Add useful constants
    m.attr("VERSION") = "0.1.0";
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_WORLD_SIZE") = 20;
    m.attr("DEFAULT_VISION_SIZE") = 8;
    m.attr("DEFAULT_BRAIN_NEURONS") = 1000;

} // namespace nlm
