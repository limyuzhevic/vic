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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Memory system access
    m.def("createBrainWithMemory", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        return std::shared_ptr<Brain>();
    }, py::arg("config"), "Create and initialize a brain with full memory systems");

    // Advanced checkpoint management
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                   const std::string& name = "") {
        if (name.empty()) {
            return brain->save(filepath);
        } else {
            // Use checkpoint manager for named checkpoints
            auto& manager = brain->getImplementation()->checkpointManager;
            if (manager) {
                return manager->saveImmediately(name);
            }
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("name") = "",
       "Save brain checkpoint with optional name");

    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                   const std::string& name = "") {
        if (name.empty()) {
            return brain->load(filepath);
        } else {
            // Try to load by name
            auto& manager = brain->getImplementation()->checkpointManager;
            if (manager) {
                return manager->load(name);
            }
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("name") = "",
       "Load brain checkpoint with optional name");

    m.def("listBrainCheckpoints", [](std::shared_ptr<Brain> brain) {
        auto& manager = brain->getImplementation()->checkpointManager;
        if (!manager) return std::vector<std::string>();
        
        auto infos = manager->listCheckpoints();
        std::vector<std::string> names;
        names.reserve(infos.size());
        for (const auto& info : infos) {
            names.push_back(info.name);
        }
        return names;
    }, py::arg("brain"), "List available brain checkpoints");

    // Advanced sensory and action utilities
    m.def("createSensoryInput", [](const std::string& type, const std::vector<float>& data) {
        if (type == "vision") {
            auto input = std::make_unique<Vision>();
            input->setData(data);
            return std::unique_ptr<SensoryInput>(input.release());
        } else if (type == "audio") {
            auto input = std::make_unique<Audio>();
            input->setData(data);
            return std::unique_ptr<SensoryInput>(input.release());
        } else {
            auto input = std::make_unique<InternalSignals>();
            for (float val : data) input->addSignal(val);
            return std::unique_ptr<SensoryInput>(input.release());
        }
    }, py::arg("type"), py::arg("data"), "Create sensory input of specified type");

    // Advanced action utilities
    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
       "Create action with type and parameters");

    // World utilities
    m.def("createWorldWithObjects", [](size_t width, size_t height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, width / 3, height / 3);
        return world;
    }, py::arg("width"), py::arg("height"), "Create world with basic configuration");

    // Memory system statistics
    m.def("getBrainMemoryStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        if (auto* episodic = brain->getEpisodicMemory()) {
            stats["episodes"] = episodic->getEpisodeCount();
            stats["maxEpisodes"] = episodic->getMaxEpisodes();
            stats["replayEnabled"] = episodic->isReplayEnabled();
        }
        if (auto* working = brain->getWorkingMemory()) {
            // Working memory stats would depend on the implementation
            stats["workingMemorySize"] = 0; // Placeholder
        }
        if (auto* associative = brain->getAssociativeMemory()) {
            stats["associations"] = associative->getAssociationCount();
        }
        return stats;
    }, py::arg("brain"), "Get comprehensive brain memory statistics");

    // Advanced configuration utilities
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set default advanced values
        config->set("brain.neuron_count", 1000);
        config->set("brain.sensory_neurons", 100);
        config->set("brain.motor_neurons", 50);
        config->set("plasticity.stdp_enabled", true);
        config->set("plasticity.hebbian_enabled", true);
        config->set("plasticity.structural_enabled", true);
        config->set("memory.episodic_enabled", true);
        config->set("memory.associative_enabled", true);
        config->set("memory.working_enabled", true);
        config->set("cognition.planner_enabled", true);
        config->set("cognition.concept_formation_enabled", true);
        config->set("cognition.attention_enabled", true);
        config->set("neuromodulation.dopamine_enabled", true);
        config->set("neuromodulation.curiosity_enabled", true);
        config->set("neuromodulation.novelty_enabled", true);
        config->set("neuromodulation.prediction_error_enabled", true);
        
        return config;
    }, "Create advanced configuration with all systems enabled");

    m.def("configureBrainForPerformance", [](std::shared_ptr<Brain> brain) {
        // High-performance configuration
        auto config = brain->getConfig();
        
        config->set("brain.neuron_count", 2000);
        config->set("brain.sensory_neurons", 200);
        config->set("brain.motor_neurons", 100);
        config->set("simulation.timestep", 0.0005);
        config->set("plasticity.stdp_ltp_weight", 0.02f);
        config->set("plasticity.stdp_ltd_weight", 0.025f);
        config->set("plasticity.synaptogenesis_rate", 0.0005f);
        config->set("plasticity.pruning_rate", 0.00005f);
        config->set("memory.episodic_capacity", 5000);
        config->set("memory.replay_interval", 50);
        config->set("memory.consolidation_interval", 500);
        
        // Re-initialize brain with new config
        brain->reset();
        return brain->initialize();
    }, py::arg("brain"), "Configure brain for high-performance operation");

    m.def("configureBrainForLearning", [](std::shared_ptr<Brain> brain) {
        // Learning-focused configuration
        auto config = brain->getConfig();
        
        config->set("brain.neuron_count", 1500);
        config->set("brain.sensory_neurons", 150);
        config->set("brain.motor_neurons", 75);
        config->set("simulation.timestep", 0.001);
        config->set("plasticity.stdp_ltp_weight", 0.01f);
        config->set("plasticity.stdp_ltd_weight", 0.012f);
        config->set("plasticity.synaptogenesis_rate", 0.0002f);
        config->set("plasticity.pruning_rate", 0.00002f);
        config->set("memory.episodic_capacity", 2000);
        config->set("memory.replay_interval", 100);
        config->set("memory.consolidation_interval", 1000);
        config->set("cognition.planning_depth", 5);
        config->set("cognition.planning_confidence", 0.7f);
        
        brain->reset();
        return brain->initialize();
    }, py::arg("brain"), "Configure brain for enhanced learning capability");

    m.def("configureBrainForStability", [](std::shared_ptr<Brain> brain) {
        // Stability-focused configuration (conservative)
        auto config = brain->getConfig();
        
        config->set("brain.neuron_count", 800);
        config->set("brain.sensory_neurons", 80);
        config->set("brain.motor_neurons", 40);
        config->set("simulation.timestep", 0.002);
        config->set("plasticity.stdp_ltp_weight", 0.005f);
        config->set("plasticity.stdp_ltd_weight", 0.006f);
        config->set("plasticity.synaptogenesis_rate", 0.00005f);
        config->set("plasticity.pruning_rate", 0.00001f);
        config->set("memory.episodic_capacity", 500);
        config->set("memory.replay_interval", 200);
        config->set("memory.consolidation_interval", 2000);
        config->set("cognition.planning_depth", 2);
        config->set("cognition.planning_confidence", 0.9f);
        
        brain->reset();
        return brain->initialize();
    }, py::arg("brain"), "Configure brain for stable operation");

    // Advanced utility functions
    m.def("validateBrainConfiguration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->get<int64_t>("brain.neuron_count").has_value()) {
            errors.push_back("brain.neuron_count not set");
        }
        
        if (!config->get<int64_t>("brain.sensory_neurons").has_value()) {
            errors.push_back("brain.sensory_neurons not set");
        }
        
        if (!config->get<int64_t>("brain.motor_neurons").has_value()) {
            errors.push_back("brain.motor_neurons not set");
        }
        
        double timestep = config->getOr<double>("simulation.timestep", 0.001);
        if (timestep <= 0.0 || timestep > 0.1) {
            errors.push_back("simulation.timestep must be between 0.0001 and 0.1");
        }
        
        return errors;
    }, py::arg("config"), "Validate brain configuration and return any errors");

    m.def("exportBrainConfig", [](std::shared_ptr<Config> config) {
        std::string jsonStr;
        config->saveToFile(jsonStr);
        return jsonStr;
    }, py::arg("config"), "Export configuration to JSON string");

    m.def("importBrainConfig", [](const std::string& jsonStr) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(jsonStr);
        return config;
    }, py::arg("jsonStr"), "Import configuration from JSON string");

    // Phase 6 specific utilities
    m.def("enablePhase6Features", [](std::shared_ptr<Brain> brain) {
        auto config = brain->getConfig();
        
        // Enable all Phase 6 features
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept_formation.enabled", true);
        config->set("cognition.attention.enabled", true);
        config->set("neuromodulation.dopamine.enabled", true);
        config->set("neuromodulation.curiosity.enabled", true);
        config->set("neuromodulation.novelty.enabled", true);
        config->set("neuromodulation.prediction_error.enabled", true);
        config->set("development.enabled", true);
        
        brain->reset();
        return brain->initialize();
    }, py::arg("brain"), "Enable all Phase 6 advanced features");

    m.def("getPhase6Status", [](std::shared_ptr<Brain> brain) {
        py::dict status;
        
        // Check if brain has advanced systems
        status["prediction_system"] = brain->getPredictionSystem() != nullptr;
        status["neural_planner"] = brain->getPlanner() != nullptr;
        status["concept_formation"] = brain->getConceptFormation() != nullptr;
        status["attention_system"] = brain->getAttention() != nullptr;
        status["development_system"] = brain->getDevelopmentSystem() != nullptr;
        status["dopamine"] = brain->getDopamine() != nullptr;
        status["curiosity"] = brain->getCuriosity() != nullptr;
        status["novelty"] = brain->getNovelty() != nullptr;
        status["prediction_error"] = brain->getPredictionErrorSignal() != nullptr;
        
        status["has_episodic_memory"] = brain->getEpisodicMemory() != nullptr;
        status["has_associative_memory"] = brain->getAssociativeMemory() != nullptr;
        status["has_working_memory"] = brain->getWorkingMemory() != nullptr;
        
        return status;
    }, py::arg("brain"), "Get status of all Phase 6 systems");

    m.def("benchmarkBrainPerformance", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Run performance benchmark
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikesGenerated = finalSpikes - initialSpikes;
        double spikesPerSecond = (double)spikesGenerated / (duration.count() / 1000.0);
        
        py::dict results;
        results["steps"] = steps;
        results["duration_ms"] = duration.count();
        results["spikes_generated"] = spikesGenerated;
        results["spikes_per_second"] = spikesPerSecond;
        results["average_spikes_per_step"] = (double)spikesGenerated / steps;
        
        return results;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance over specified steps");

    m.def("getBrainSystemStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        
        // Basic brain stats
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Developmental stage
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // System-specific stats
        if (auto* spikeSystem = brain->getSpikeSystem()) {
            stats["pending_spikes"] = spikeSystem->getPendingSpikeEventCount();
        }
        
        if (auto* stdp = brain->getSTDP()) {
            stats["stdp_enabled"] = stdp->isEnabled();
        }
        
        if (auto* hebbian = brain->getHebbian()) {
            stats["hebbian_enabled"] = hebbian->isEnabled();
        }
        
        if (auto* structural = brain->getStructuralPlasticity()) {
            stats["structural_plasticity_enabled"] = structural->isEnabled();
        }
        
        if (auto* development = brain->getDevelopmentSystem()) {
            stats["developmental_stage_name"] = std::string(development->getStageName());
            stats["plasticity_modifier"] = development->getPlasticityModifier();
        }
        
        return stats;
    }, py::arg("brain"), "Get comprehensive statistics about all brain systems");

    m.def("createBrainWithAdvancedMemory", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            // Enable advanced memory features
            if (auto* episodic = brain->getEpisodicMemory()) {
                episodic->enableReplay(true);
                episodic->setMaxEpisodes(2000);
            }
            
            if (auto* associative = brain->getAssociativeMemory()) {
                // Enable associative learning
            }
            
            if (auto* working = brain->getWorkingMemory()) {
                // Configure working memory
            }
            
            return brain;
        }
        
        return std::shared_ptr<Brain>();
    }, py::arg("config"), "Create brain with advanced memory systems");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain) {
        py::dict state;
        
        // Export all major system states
        state["brain_state"] = brain->save("temp_brain_state.bin");
        
        // System-specific states
        if (auto* episodic = brain->getEpisodicMemory()) {
            auto episodes = episodic->getEpisodes();
            py::list episodeList;
            for (const auto& ep : episodes) {
                py::dict epDict;
                epDict["timestamp"] = ep.timestamp;
                epDict["reward"] = ep.reward;
                epDict["action"] = static_cast<int>(ep.action);
                // Add other fields as needed
                episodeList.append(epDict);
            }
            state["episodes"] = episodeList;
        }
        
        return state;
    }, py::arg("brain"), "Export comprehensive brain state");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const py::dict& state) {
        // Restore brain state from exported state
        bool success = true;
        
        // Restore brain state from file
        if (state.contains("brain_state")) {
            success = brain->load("temp_brain_state.bin");
        }
        
        // Restore episodic memory
        if (success && state.contains("episodes")) {
            if (auto* episodic = brain->getEpisodicMemory()) {
                std::vector<EpisodicMemoryItem> episodes;
                py::list episodeList = state["episodes"].cast<py::list>();
                
                for (size_t i = 0; i < episodeList.size(); ++i) {
                    py::dict epDict = episodeList[i].cast<py::dict>();
                    EpisodicMemoryItem ep;
                    ep.timestamp = epDict["timestamp"].cast<SimulationStep>();
                    ep.reward = epDict["reward"].cast<float>();
                    ep.action = static_cast<ActionType>(epDict["action"].cast<int>());
                    // Restore other fields as needed
                    episodes.push_back(ep);
                }
                
                episodic->restoreEpisodes(episodes);
            }
        }
        
        return success;
    }, py::arg("brain"), py::arg("state"), "Import brain state from exported state");

} // namespace nlm
