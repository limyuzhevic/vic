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
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive access to neural computation, learning, memory,
        and cognition systems for advanced users.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== BASIC TYPES AND IDENTIFIERS ==========
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
       _svc_.def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq", &SynapseId::operator==)
        .def("__ne", &SynapseId::operator!=)
        .def("__hash", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq", &RegionId::operator==)
        .def("__ne", &RegionId::operator!=)
        .def("__hash", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq", &PopulationId::operator==)
        .def("__ne", &PopulationId::operator!=);

    // ========== NEURON AND SYNAPSE TYPES ==========
    
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

    // ========== CONFIGURATION MANAGEMENT ==========
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration management for NLM system)pbdoc")
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
        .def("__repr", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Advanced configuration methods
        .def("getOr", [](Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::User,
             "Set configuration value with source tracking")
        .def("get", [](Config& self, const std::string& key) {
            // This requires a template specialization or direct access
            // For now, we'll use getOr with a type-specific approach
            auto value = self.getOr(key, "");
            return value;
        }, py::arg("key"),
             "Get configuration value (generic)")
        .def("validate", [](Config& self) {
            // Placeholder for validation logic
            return true;
        }, "Validate configuration values");

    // ========== SENSOR AND PERCEPTION ==========
    
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
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self, float mean, float stddev) {
            self.normalize(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"),
             "Normalize vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("filter", [](Audio& self, const std::vector<float>& filter) {
            self.filter(filter);
        }, py::arg("filter"), "Apply filter to audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"),
             "Get signal value by index")
        .def("setSignal", &InternalSignals::setSignal, py::arg("index"), py::arg("value"),
             "Set signal value by index");

    // ========== MOTOR ACTIONS ==========
    
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone)
        .def("addParameter", &Action::addParameter, py::arg("name"), py::arg("value"),
             "Add parameter with name and value")
        .def("getParameter", &Action::getParameter, py::arg("name"), py::arg("defaultValue") = 0.0f,
             "Get parameter value by name with default");

    // ========== WORLD AND ENVIRONMENT ==========
    
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
        .def("isColliding", &WorldObject::isColliding, py::arg("other"),
             "Check collision with another world object");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Advanced agent body state with kinematic tracking)pbdoc")
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
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            self.x += dx;
            self.y += dy;
            self.velocityX = dx / dt;
            self.velocityY = dy / dt;
            self.isMoving = true;
            self.lastActionTime = dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move agent by displacement")
        .def("rotate", [](AgentBody& self, float angle, float dt) {
            self.orientation += angle;
            self.angularVelocity = angle / dt;
            self.isTurning = true;
            self.lastActionTime = dt;
        }, py::arg("angle"), py::arg("dt"), "Rotate agent by angle")
        .def("updatePhysics", [](AgentBody& self, float dt, float friction = 0.1f) {
            // Apply friction
            self.velocityX *= (1.0f - friction);
            self.velocityY *= (1.0f - friction);
            self.angularVelocity *= (1.0f - friction);
            
            // Update position
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            
            // Reset moving flags
            self.isMoving = false;
            self.isTurning = false;
        }, py::arg("dt"), py::arg("friction") = 0.1f, "Update agent physics");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Advanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_read Strukture=" &ActionResult::message"
        .def("addLogEntry", [](ActionResult& self, const std::string& entry) {
            self.message += "\n[" + std::to_string(self.reward) + "] " + entry;
        }, py::arg("entry"), "Add log entry to result");

    // ========== WORLD SIMULATION ==========
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Advanced sensory percept data)pbdoc")
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
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            // Merge another percept into this one (vision, touch, internal, audio)
            if (auto* otherVision = other.getVision()) {
                self.setVision(otherVision);
            }
            if (auto* otherTouch = other.getTouch()) {
                self.setTouch(otherTouch);
            }
            if (auto* otherInternal = other.getInternal()) {
                self.setInternal(otherInternal);
            }
            if (auto* otherAudio = other.getAudio()) {
                self.setAudio(otherAudio);
            }
        }, py::arg("other"), "Merge another percept into this one");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Advanced simple 2D world for NLM simulation)pbdoc")
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
        // Advanced world features
        .def("generateTerrain", [](SimpleWorld& self, const std::string& type) {
            // Placeholder for terrain generation
            NLM_LOG_INFO("Terrain generation not yet implemented for type: " + type);
        }, py::arg("type"), "Generate terrain of specified type")
        .def("addResource", [](SimpleWorld& self, WorldObjectType type, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = type;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("type"), py::arg("x"), py::arg("y"), "Add resource object to world")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.radius = radius;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Add hazard object to world");

    // ========== NEURAL SYSTEM ==========
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Advanced central neural simulation brain class)pbdoc")
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
             "Inject sensory input into brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor/action output based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
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
        // Advanced brain features
        .def("setLearningRate", [](Brain& self, float rate) {
            if (auto* stdp = self.getSTDP()) stdp->setLearningRate(rate);
            if (auto* hebbian = self.getHebbian()) hebbian->setLearningRate(rate);
        }, py::arg("rate"), "Set learning rate for all plasticity rules")
        .def("getLearningRate", []() {
            // This would require a more complex interface to get current learning rates
            return 0.01f;
        }, "Get current learning rate")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add connection between regions")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove connection between regions")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("setPlasticity", [](Brain& self, bool hebbian, bool stdp, bool structural) {
            if (auto* stdpRule = self.getSTDP()) stdpRule->setEnabled(stdp);
            if (auto* hebbianRule = self.getHebbian()) hebbianRule->setEnabled(hebbian);
            if (auto* structuralRule = self.getStructuralPlasticity()) structuralRule->setEnabled(structural);
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("structural"),
             "Enable/disable plasticity rules")
        .def("getPlasticityStatus", [](Brain& self) {
            std::map<std::string, bool> status;
            if (auto* stdpRule = self.getSTDP()) status["STDP"] = stdpRule->isEnabled();
            if (auto* hebbianRule = self.getHebbian()) status["Hebbian"] = hebbianRule->isEnabled();
            if (auto* structuralRule = self.getStructuralPlasticity()) status["Structural"] = structuralRule->isEnabled();
            return status;
        }, "Get plasticity rule status");

    // ========== AGENT BRAIN INTERFACE ==========
    
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // Advanced agent features
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get detailed motor group activity statistics")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get recent thought trace from agent")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("setLearnedPreference", &AgentBrain::setLearnedPreference,
             py::arg("preference"), "Set learned preference trait")
        .def("getReflexCounts", [](AgentBrain& self) {
            std::map<std::string, int> counts;
            counts["reflexBonus"] = self.getReflexBonusCount();
            counts["reflexSuppress"] = self.getReflexSuppressCount();
            counts["wtaPulse"] = self.getWtaPulseCount();
            return counts;
        }, "Get reflex mechanism counts")
        .def("getCognitionCounts", [](AgentBrain& self) {
            std::map<std::string, int> counts;
            counts["plannerOverride"] = self.getPlannerOverrideCount();
            counts["plannerAgreement"] = self.getPlannerAgreementCount();
            counts["plannerDecision"] = self.getPlannerDecisionCount();
            counts["conceptBias"] = self.getConceptBiasCount();
            counts["selfBias"] = self.getSelfBiasCount();
            counts["socialBias"] = self.getSocialBiasCount();
            return counts;
        }, "Get cognition system counts");

    // ========== PLASTICITY RULES ==========
    
    py::class_<PlasticityRule, std::shared_ptr<PlasticityRule>>(m, "PlasticityRule", R"pbdoc(Base class for synaptic plasticity rules)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled,
             "Check if plasticity rule is enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"),
             "Enable/disable plasticity rule")
        .def("getName", &PlasticityRule::getName,
             "Get name of plasticity rule");

    py::class_<Hebbian, std::shared_ptr<Hebbian>>(m, "Hebbian", R"pbdoc(Hebbian plasticity rule implementing 'fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set Hebbian learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate,
             "Get current Hebbian learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"),
             "Set maximum synaptic weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight,
             "Get maximum synaptic weight");

    py::class_<STDP, std::shared_ptr<STDP>>(m, "STDP", R"pbdoc(Spike-Timing Dependent Plasticity implementing learning based on spike timing)pbdoc")
        .def(py::init<>())
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight,
             "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight,
             "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             "Set STDP time constant")
        .def("getTimeConstant", &STDP::getTimeConstant,
             "Get STDP time constant");

    py::class_<StructuralPlasticity, std::shared_ptr<StructuralPlasticity>>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synapse creation and elimination)pbdoc")
        .def(py::init<>())
        .def("setRates", [](StructuralPlasticity& self, float synaptogenesisRate, float pruningRate) {
            // Note: Direct rate setters may not exist, this is a placeholder for future implementation
            NLM_LOG_INFO("Structural plasticity rate setting not yet implemented");
        }, py::arg("synaptogenesisRate"), py::arg("pruningRate"),
             "Set structural plasticity rates");

    // ========== MEMORY SYSTEMS ==========
    
    py::class_<NeuralWorkingMemory, std::shared_ptr<NeuralWorkingMemory>>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system for transient information storage)pbdoc")
        .def("getActiveNeurons", [](NeuralWorkingMemory& self) {
            // Placeholder for getting active neurons from working memory
            return std::vector<NeuronId>();
        }, "Get active neurons in working memory")
        .def("getPattern", [](NeuralWorkingMemory& self, size_t index) {
            // Placeholder for getting patterns
            return std::vector<float>();
        }, py::arg("index"), "Get pattern from working memory")
        .def("storePattern", [](NeuralWorkingMemory& self, const std::vector<float>& pattern) {
            // Placeholder for storing patterns
            NLM_LOG_INFO("Pattern storage not yet implemented");
        }, py::arg("pattern"), "Store pattern in working memory")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory");

    py::class_<NeuralEpisodicMemory, std::shared_ptr<NeuralEpisodicMemory>>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for experience storage and replay)pbdoc")
        .def("storeExperience", [](NeuralEpisodicMemory& self, const std::vector<float>& perception, float reward) {
            // Placeholder for storing experiences
            NLM_LOG_INFO("Episodic memory storage not yet implemented");
        }, py::arg("perception"), py::arg("reward"),
             "Store experience in episodic memory")
        .def("recallExperience", [](NeuralEpisodicMemory& self, size_t index) {
            // Placeholder for recalling experiences
            return std::vector<float>();
        }, py::arg("index"), "Recall experience from episodic memory")
        .def("getExperienceCount", []() {
            // Placeholder for getting experience count
            return size_t(0);
        }, "Get number of stored experiences");

    py::class_<NeuralAssociativeMemory, std::shared_ptr<NeuralAssociativeMemory>>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system for pattern associations)pbdoc")
        .def("associate", [](NeuralAssociativeMemory& self, const std::vector<float>& pattern1, const std::vector<float>& pattern2) {
            // Placeholder for making associations
            NLM_LOG_INFO("Associative memory not yet implemented");
        }, py::arg("pattern1"), py::arg("pattern2"),
             "Create association between patterns")
        .def("recall", [](NeuralAssociativeMemory& self, const std::vector<float>& pattern) {
            // Placeholder for recall
            return std::vector<float>();
        }, py::arg("pattern"), "Recall associated pattern")
        .def("getAssociations", []() {
            // Placeholder for getting all associations
            return std::vector<std::pair<std::vector<float>, std::vector<float>>>();
        }, "Get all associations");

    // ========== NEUROMODULATION ==========
    
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(Base class for neuromodulators)pbdoc")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current neuromodulator level")
        .def("setLevel", [](Neuromodulator& self, float level) {
            // Note: Direct level setters may not exist, this is a placeholder
            NLM_LOG_INFO("Neuromodulator level setting not yet implemented");
        }, py::arg("level"), "Set neuromodulator level");

    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("setBaseline", [](Dopamine& self, float baseline) {
            // Placeholder for baseline setting
            NLM_LOG_INFO("Dopamine baseline setting not yet implemented");
        }, py::arg("baseline"), "Set dopamine baseline level");

    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration behavior)pbdoc")
        .def(py::init<>())
        .def("setBaseline", [](Curiosity& self, float baseline) {
            // Placeholder for baseline setting
            NLM_LOG_INFO("Curiosity baseline setting not yet implemented");
        }, py::arg("baseline"), "Set curiosity baseline level");

    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(Novelty neuromodulator for novelty detection)pbdoc")
        .def(py::init<>());

    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>());

    // ========== PERFORMANCE AND UTILITIES ==========
    
    py::class_<CheckpointSystem, std::shared_ptr<CheckpointSystem>>(m, "CheckpointSystem", R"pbdoc(Advanced checkpoint system for brain state persistence)pbdoc")
        .def("save", [](CheckpointSystem& self, const std::string& filepath, const std::string& metadata = "") {
            // Placeholder for checkpoint save
            NLM_LOG_INFO("Checkpoint save not yet implemented: " + filepath);
            return false;
        }, py::arg("filepath"), py::arg("metadata") = "",
             "Save brain state to checkpoint")
        .def("load", [](CheckpointSystem& self, const std::string& filepath) {
            // Placeholder for checkpoint load
            NLM_LOG_INFO("Checkpoint load not yet implemented: " + filepath);
            return false;
        }, py::arg("filepath"),
             "Load brain state from checkpoint")
        .def("getMetadata", [](CheckpointSystem& self, const std::string& filepath) {
            // Placeholder for metadata retrieval
            NLM_LOG_INFO("Checkpoint metadata not yet implemented");
            return std::string();
        }, py::arg("filepath"), "Get checkpoint metadata");

    // ========== FACTORY FUNCTIONS ==========
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with optimized settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createHebbianRule", []() -> std::shared_ptr<Hebbian> {
        return std::make_shared<Hebbian>();
    }, "Create a new Hebbian plasticity rule");

    m.def("createSTDPRule", []() -> std::shared_ptr<STDP> {
        return std::make_shared<STDP>();
    }, "Create a new STDP plasticity rule");

    // ========== UTILITY FUNCTIONS ==========
    
    m.def("getVersion", []() {
        return "NLM Python Bindings v0.1.0 (Advanced)";
    }, "Get version information for NLM Python bindings");

    m.def("getSystemInfo", []() {
        return "NLM C++ neural simulation framework with advanced Python bindings";
    }, "Get system information");

    // ========== CONSTANTS ==========
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Phase 6 advanced constants
    m.attr("DEVELOPMENTAL_STAGES") = py::dict(
        "Initial", DevelopmentalStage::Initial,
        "CriticalPeriod", DevelopmentalStage::CriticalPeriod,
        "Maturation", DevelopmentalStage::Maturation,
        "Adult", DevelopmentalStage::Adult,
        "Aging", DevelopmentalStage::Aging
    );

    m.attr("NEURON_TYPES") = py::dict(
        "Excitatory", NeuronType::Excitatory,
        "Inhibitory", NeuronType::Inhibitory,
        "Modulatory", NeuronType::Modulatory,
        "Sensory", NeuronType::Sensory,
        "Motor", NeuronType::Motor,
        "Internal", NeuronType::Internal
    );

    m.attr("ACTION_TYPES") = py::dict(
        "MoveForward", ActionType::MoveForward,
        "MoveBackward", ActionType::MoveBackward,
        "MoveLeft", ActionType::MoveLeft,
        "MoveRight", ActionType::MoveRight,
        "TurnLeft", ActionType::TurnLeft,
        "TurnRight", ActionType::TurnRight,
        "Look", ActionType::Look,
        "LookUp", ActionType::LookUp,
        "LookDown", ActionType::LookDown,
        "Interact", ActionType::Interact,
        "Eat", ActionType::Eat,
        "Drink", ActionType::Drink,
        "Rest", ActionType::Rest,
        "Wait", ActionType::Wait,
        "Custom", ActionType::Custom
    );

    m.attr("WORLD_OBJECT_TYPES") = py::dict(
        "Empty", WorldObjectType::Empty,
        "Resource", WorldObjectType::Resource,
        "Hazard", WorldObjectType::Hazard,
        "Wall", WorldObjectType::Wall,
        "Marker", WorldObjectType::Marker
    );

    // ========== ADVANCED HELPER CLASSES ==========
    
    py::class_<SimulationStepHelper>(m, "SimulationStepHelper", R"pbdoc(Helper class for managing simulation steps and timing)pbdoc")
        .def(py::init<>())
        .def("calculateStepDuration", [](const SimulationStepHelper& self, double timeElapsed, size_t stepsCompleted) {
            return timeElapsed / (stepsCompleted + 1);
        }, py::arg("timeElapsed"), py::arg("stepsCompleted"),
             "Calculate simulation step duration");

    py::class_<BrainStats>(m, "BrainStats", R"pbdoc(Advanced statistics for brain analysis)pbdoc")
        .def(py::init<>())
        .def("getExcitatoryInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate")
        .def("getTotalActivity", [](const BrainStats& self, const Brain& brain) {
            return brain.getTotalSpikeCount() + brain.getFiringNeuronCount();
        }, py::arg("brain"), "Get total neural activity")
        .def("getComplexityScore", [](const BrainStats& self, const Brain& brain) {
            // Simple complexity metric based on connections and activity
            float spikeCount = static_cast<float>(brain.getTotalSpikeCount());
            float neuronCount = static_cast<float>(brain.getTotalNeuronCount());
            float avgFiringRate = brain.getAverageFiringRate();
            return (spikeCount / neuronCount) * avgFiringRate;
        }, py::arg("brain"), "Get brain complexity score");

    // ========== ADVANCED EXAMPLE FUNCTIONS ==========
    
    m.def("runAdvancedExample", [](const std::string& exampleName) {
        NLM_LOG_INFO("Advanced example execution not yet implemented: " + exampleName);
    }, py::arg("exampleName"), "Run advanced example (placeholder)");

    m.def("benchmark", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Simple benchmark of brain performance
        NLM_LOG_INFO("Benchmarking brain for " + std::to_string(steps) + " steps");
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        NLM_LOG_INFO("Benchmark completed");
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");

    m.def("analyzeNetwork", [](std::shared_ptr<Brain> brain) {
        // Simple network analysis
        NLM_LOG_INFO("Network analysis not yet implemented");
        return std::map<std::string, float>{
            {"excitatoryInhibitionRatio", brain->getExcitationInhibitionRatio()},
            {"averageFiringRate", brain->getAverageFiringRate()},
            {"totalSpikeCount", static_cast<float>(brain->getTotalSpikeCount())},
            {"totalNeuronCount", static_cast<float>(brain->getTotalNeuronCount())}
        };
    }, py::arg("brain"), "Analyze network properties");

    // ========== EXPERIMENTAL FEATURES ==========
    
    m.def("experimentalTextEncoding", [](const std::string& text) {
        return AgentBrain::encodeText(text);
    }, py::arg("text"), "Encode text as neuron patterns (Phase 6 feature)");

    m.def("experimentalGetInternalRates", [](const std::shared_ptr<AgentBrain>& agent) {
        return agent->getInternalFiringRates();
    }, py::arg("agent"), "Get internal firing rates for recall probe (Phase 6)");

    // ========== LEGACY COMPATIBILITY ==========
    
    // Maintain backward compatibility with existing simple API
    m.def("createBrainSimple", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create brain with default config (legacy compatibility)");

    m.def("createWorldSimple", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create world (legacy compatibility)");

    m.def("createAgentSimple", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create agent (legacy compatibility)");

    // ========== DOCUMENTATION AND HELP ==========
    
    m.def("getDocumentation", []() {
        return R"doc(
        # NLM Python Bindings - Advanced Documentation
        
        This is an advanced Python interface for the NLM neural simulation framework.
        
        ## Key Features:
        - Real spiking neural computation with LIF neurons
        - Advanced plasticity rules (Hebbian, STDP, structural)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Cognitive systems (planning, concept formation, attention)
        - Developmental systems
        - Checkpoint persistence
        - Advanced Python API for power users
        
        ## Quick Start for Advanced Users:
        1. Create config: createDefaultConfig()
        2. Create brain: createBrain(config)
        3. Initialize brain: brain.initialize()
        4. Run simulation: brain.step(step, time)
        5. Get actions: brain.produceAction()
        
        For detailed documentation, see the NLM documentation.
        )doc";
    }, "Get detailed documentation");
}

} // namespace nlm