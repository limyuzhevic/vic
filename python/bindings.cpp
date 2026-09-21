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
        .def("getOr", py::overload_cast<const std::string&, int>(&Config::getOr), py::arg("key"), py::arg("default_value"),
             "Get integer value or default if key doesn't exist")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr), py::arg("key"), py::arg("default_value"),
             "Get float/double value or default if key doesn't exist")
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::getOr), py::arg("key"), py::arg("default_value"),
             "Get boolean value or default if key doesn't exist")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr), py::arg("key"), py::arg("default_value"),
             "Get string value or default if key doesn't exist")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get), py::arg("key"),
             "Get integer value from configuration")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get), py::arg("key"),
             "Get float/double value from configuration")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get), py::arg("key"),
             "Get boolean value from configuration")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get), py::arg("key"),
             "Get string value from configuration")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set), py::arg("key"), py::arg("value"),
             "Set integer value in configuration")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set), py::arg("key"), py::arg("value"),
             "Set float/double value in configuration")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set), py::arg("key"), py::arg("value"),
             "Set boolean value in configuration")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set), py::arg("key"), py::arg("value"),
             "Set string value in configuration")
        .def("__getitem__", [](Config& self, const std::string& key) {
            // Try different types based on what's available
            if (auto val = self.get<int>(key)) return *val;
            if (auto val = self.get<double>(key)) return *val;
            if (auto val = self.get<bool>(key)) return *val;
            if (auto val = self.get<std::string>(key)) return *val;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value (supports int, float, bool, string)")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                self.set(key, ConfigValue(nullptr));
            } else if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::type_error("Unsupported value type for config");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("__contains__", &Config::has, py::arg("key"), "Check if key exists in config")
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
        .def("runEpisode", [](SimpleWorld& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                            uint64_t maxSteps, float timestep) {
            float totalReward = 0.0f;
            for (uint64_t step = 0; step < maxSteps; ++step) {
                auto percept = self.getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * timestep);
                auto cmd = agent->decodeMotorCommand();
                self.applyMotorCommand(cmd, step * timestep);
            }
            return totalReward / maxSteps;
        }, py::arg("brain"), py::arg("agent"), py::arg("max_steps") = 1000ULL, 
           py::arg("timestep") = 0.001f, "Run an episode and return average reward")
        .def("createDefaultConfig", [](SimpleWorld& self, size_t width, size_t height) {
            auto cfg = std::make_shared<Config>();
            cfg->set("world_width", width);
            cfg->set("world_height", height);
            cfg->set("neuron_count", width * height * 10);
            cfg->set("region_count", 1);
            return cfg;
        }, py::arg("width"), py::arg("height"), "Create a default configuration for the world");

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
        .def("clear", &Brain::reset,
             "Clear/reset brain state (alias for reset)")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("checkpoint", &Brain::save, py::arg("filepath"),
             "Create a checkpoint (alias for save)")
        .def("restore", &Brain::load, py::arg("filepath"),
             "Restore from checkpoint (alias for load)")
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
        .def("getFirstRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get the first region (convenience method)")
        .def("getPrimaryRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get primary region (convenience method)")
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
        .def("getDefaultConfig", [](std::shared_ptr<Brain> brain) {
            return brain->getConfig();
        }, py::return_value_policy::reference_internal,
             "Get the default configuration (alias)")
        .def("updateFromConfig", [](Brain& self) {
            return self.initialize();
        }, "Update brain parameters from configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("runSimulation", [](Brain& self, std::shared_ptr<AgentBrain> agent, 
                               std::shared_ptr<SimpleWorld> world,
                               uint64_t steps, float timestep) {
            float totalReward = 0.0f;
            for (uint64_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                self.step(step, step * timestep);
                auto cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(cmd, step * timestep);
                totalReward += cmd.type == MotorCommand::Wait ? 0.0f : 1.0f;
            }
            return totalReward / steps;
        }, py::arg("agent"), py::arg("world"), py::arg("steps") = 1000ULL,
           py::arg("timestep") = 0.001f,
           "Run a complete simulation episode (convenience method)")

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode (alias for clear)")
        .def("clear", &AgentBrain::reset,
             "Clear agent state (alias for reset)")
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
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("getBrainReference", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get reference to underlying brain (convenience alias)")
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
            std::string status = "Status: ";
            status += "RewardMod:" + std::string(self.isRewardModulationEnabled() ? "ON" : "OFF") + ", ";
            status += "Dev:" + std::string(self.isDevelopmentEnabled() ? "ON" : "OFF") + ", ";
            status += "Curiosity:" + std::string(self.isCuriosityEnabled() ? "ON" : "OFF");
            return status;
        }, "Get a human-readable status string of agent brain configuration")
        .def("applyConfiguration", [](AgentBrain& self, const Config& config) {
            self.getBrain()->getConfig()->loadFromArgs(0, nullptr);
        }, py::arg("config"), "Apply configuration to agent brain (deprecated, use brain.setConfig)")
        .def("createEpisode", [](AgentBrain& self) {
            return self.reset();
        }, "Create a new episode (convenience method)")
        .def("setupSimulation", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                                 uint64_t steps, float timestep) {
            self.initialize(world);
            return self;
        }, py::arg("world"), py::arg("steps") = 1000ULL, py::arg("timestep") = 0.001f,
           "Set up simulation environment (initialize world and agent)")

    // Add helper function to create a brain with typical phase 6 configuration
    m.def("createPhase6Brain", []() {
        auto config = std::make_shared<Config>();
        
        // Set typical Phase 6 configuration values
        config->set("neuron_count", 1000ULL);
        config->set("region_count", 1ULL);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42ULL);
        
        return config;
    }, "Create a default Phase 6 configuration for an integrated brain");
    
    // Create a simple world with typical size and parameters
    m.def("createPhase6World", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 8, 8);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.001f);
        world->setRandomSeed(42);
        return world;
    }, "Create a Phase 6 world with typical settings");
    
    // Create a complete integrated agent (convenience function)
    m.def("createIntegratedAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all Phase 6 systems
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an integrated agent with all systems enabled");
    
    // Helper function to run a complete Phase 6 simulation episode
    m.def("runPhase6Episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                               std::shared_ptr<SimpleWorld> world, uint64_t steps, float timestep) {
        float totalReward = 0.0f;
        
        // Initialize world and agent for new episode
        world->reset();
        agent->reset();
        
        // Run simulation
        for (uint64_t step = 0; step < steps; ++step) {
            // Get sensory percept from world
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * timestep);
            
            // Get motor command
            auto cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(cmd, step * timestep);
            
            // Accumulate reward (simple heuristic: movement gets reward)
            if (cmd.type != MotorCommand::Wait) {
                totalReward += 1.0f;
            }
            
            // Apply reward modulation
            agent->applyRewardModulation(totalReward / (step + 1), 0.0f);
            
            // Update development if enabled
            if (agent->isDevelopmentEnabled()) {
                agent->updateDevelopment(timestep);
            }
        }
        
        return totalReward / steps;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), 
       py::arg("steps") = 1000ULL, py::arg("timestep") = 0.001f,
       "Run a complete Phase 6 simulation episode and return average reward");
    
    // Helper function to set up default Phase 6 configuration
    m.def("setupDefaultPhase6Config", [](std::shared_ptr<Config> config) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        // Standard Phase 6 configuration
        config->set("neuron_count", 1000ULL);
        config->set("region_count", 1ULL);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001f);
        config->set("replay_interval", 100ULL);
        config->set("consolidation_interval", 1000ULL);
        config->set("random_seed", 42ULL);
        config->set("checkpoint_dir", "./checkpoints");
        
        return config;
    }, py::arg("config") = nullptr, "Set up a default Phase 6 configuration");
    
    // Add a convenient method to get the first region (common in Phase 6)
    m.def("getFirstRegion", [](std::shared_ptr<Brain> brain) -> NeuralRegion* {
        if (brain && !brain->getRegions().empty()) {
            return brain->getRegions()[0].get();
        }
        return nullptr;
    }, py::arg("brain"), "Get the first region from a brain (convenience method for Phase 6)");
    
    // Add a function to check if an agent has all necessary systems for Phase 6
    m.def("isPhase6Ready", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) return false;
        
        bool hasBrain = agent->getBrain() != nullptr;
        bool hasNeuromodulation = agent->isRewardModulationEnabled() || 
                                  agent->isDevelopmentEnabled() ||
                                  agent->isCuriosityEnabled();
        
        return hasBrain && hasNeuromodulation;
    }, py::arg("agent"), "Check if an agent is ready for Phase 6 integration");

} // namespace nlm
