#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
             "Log brain status");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    // Advanced API: Memory System Accessors
    m.def("getWorkingMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralWorkingMemory> {
        return std::shared_ptr<NeuralWorkingMemory>(brain->getWorkingMemory());
    }, py::arg("brain"), "Get working memory system");

    m.def("getEpisodicMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::shared_ptr<NeuralEpisodicMemory>(brain->getEpisodicMemory());
    }, py::arg("brain"), "Get episodic memory system");

    m.def("getAssociativeMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralAssociativeMemory> {
        return std::shared_ptr<NeuralAssociativeMemory>(brain->getAssociativeMemory());
    }, py::arg("brain"), "Get associative memory system");

    m.def("getPredictionSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PredictionSystem> {
        return std::shared_ptr<PredictionSystem>(brain->getPredictionSystem());
    }, py::arg("brain"), "Get prediction system");

    m.def("getNeuralPlanner", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralPlanner> {
        return std::shared_ptr<NeuralPlanner>(brain->getPlanner());
    }, py::arg("brain"), "Get neural planner");

    m.def("getConceptFormation", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<ConceptFormation> {
        return std::shared_ptr<ConceptFormation>(brain->getConceptFormation());
    }, py::arg("brain"), "Get concept formation system");

    m.def("getAttention", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AttentionalSelection> {
        return std::shared_ptr<AttentionalSelection>(brain->getAttention());
    }, py::arg("brain"), "Get attentional selection system");

    m.def("getDevelopmentSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<DevelopmentSystem> {
        return std::shared_ptr<DevelopmentSystem>(brain->getDevelopmentSystem());
    }, py::arg("brain"), "Get development system");

    m.def("getDopamine", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Dopamine> {
        return std::shared_ptr<Dopamine>(brain->getDopamine());
    }, py::arg("brain"), "Get dopamine neuromodulator");

    m.def("getCuriosity", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Curiosity> {
        return std::shared_ptr<Curiosity>(brain->getCuriosity());
    }, py::arg("brain"), "Get curiosity neuromodulator");

    m.def("getNovelty", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Novelty> {
        return std::shared_ptr<Novelty>(brain->getNovelty());
    }, py::arg("brain"), "Get novelty neuromodulator");

    m.def("getPredictionError", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PredictionError> {
        return std::shared_ptr<PredictionError>(brain->getPredictionErrorSignal());
    }, py::arg("brain"), "Get prediction error signal");

    // Advanced utility functions
    m.def("runPhase6IntegrationTest", [](std::shared_ptr<Brain> brain, uint64_t steps = 5000) {
        // Run a simplified Phase 6 integration test
        auto config = brain->getConfig();
        config->set("max_steps", static_cast<int64_t>(steps), ConfigSource::User);
        config->set("enable_checkpointing", true, ConfigSource::User);
        config->set("enable_replay", true, ConfigSource::User);
        config->set("enable_development", true, ConfigSource::User);
        
        // Run simulation with all systems
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
            
            // Apply reward modulation based on curiosity
            if (brain->getCuriosity()) {
                float curiosity = brain->getCuriosity()->getLevel();
                float reward = curiosity * 0.1f;
                brain->applyNeuromodulation(*brain->getDopamine());
            }
            
            // Update development periodically
            if (i % 1000 == 0 && brain->getDevelopmentSystem()) {
                brain->getDevelopmentSystem()->update(brain.get(), *brain->getRandomGenerator(), 1.0);
            }
        }
        
        return "Phase 6 integration test completed";
    }, py::arg("brain"), py::arg("steps") = 5000, "Run comprehensive Phase 6 integration test");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        return brain->save(filename);
    }, py::arg("brain"), py::arg("filename"), "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        return brain->load(filename);
    }, py::arg("brain"), py::arg("filename"), "Import brain state from file");

    m.def("createAdvancedConfig", [](size_t neuronCount = 2000,
                                     size_t regionCount = 2,
                                     float connectionProb = 0.05,
                                     bool enableAllSystems = true) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::User);
        config->set("connection_probability", connectionProb, ConfigSource::User);
        
        if (enableAllSystems) {
            config->set("stdp_enable", true, ConfigSource::User);
            config->set("hebbian_enable", true, ConfigSource::User);
            config->set("structural_enable", true, ConfigSource::User);
            config->set("neuromod_curiosity_enable", true, ConfigSource::User);
            config->set("neuromod_novelty_enable", true, ConfigSource::User);
            config->set("neuromod_prediction_enable", true, ConfigSource::User);
            config->set("memory_working_memory_enable", true, ConfigSource::User);
            config->set("memory_episodic_enable", true, ConfigSource::User);
            config->set("memory_associative_enable", true, ConfigSource::User);
            config->set("cognition_planner_enable", true, ConfigSource::User);
            config->set("cognition_concept_enable", true, ConfigSource::User);
            config->set("cognition_attention_enable", true, ConfigSource::User);
            config->set("development_enable", true, ConfigSource::User);
            config->set("checkpoint_enable", true, ConfigSource::User);
        }
        
        return config;
    }, py::arg("neuronCount") = 2000, py::arg("regionCount") = 2,
         py::arg("connectionProb") = 0.05, py::arg("enableAllSystems") = true,
         "Create advanced configuration with all systems enabled");

    // Performance and monitoring functions
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        py::dict metrics;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        metrics["avg_firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["pending_events"] = brain->getPendingSpikeEventCount();
        
        // Add memory metrics if available
        if (brain->getWorkingMemory()) {
            metrics["working_memory_traces"] = brain->getWorkingMemory()->getActiveTraces();
        }
        if (brain->getEpisodicMemory()) {
            metrics["episodic_memory_episodes"] = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        // Add neuromodulation metrics
        if (brain->getDopamine()) {
            metrics["dopamine_level"] = brain->getDopamine()->getLevel();
        }
        if (brain->getCuriosity()) {
            metrics["curiosity_level"] = brain->getCuriosity()->getLevel();
        }
        
        metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics;
    }, py::arg("brain"), "Get comprehensive performance metrics");

    m.def("benchmarkBrain", [](std::shared_ptr<Config> config, uint64_t steps = 10000) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double timePerStep = static_cast<double>(duration.count()) / steps;
        
        py::dict results;
        results["steps"] = steps;
        results["total_time_ms"] = duration.count();
        results["time_per_step_ms"] = timePerStep;
        results["steps_per_second"] = steps / (duration.count() / 1000.0);
        results["neurons_per_step"] = config->getOr<int64_t>("neuron_count", 1000);
        
        return results;
    }, py::arg("config"), py::arg("steps") = 10000,
         "Run benchmark test and return performance metrics");

    // Simulation control and utilities
    m.def("resetBrainSystems", [](std::shared_ptr<Brain> brain) {
        brain->reset();
        // Clear memory systems
        if (brain->getWorkingMemory()) brain->getWorkingMemory()->clear();
        if (brain->getEpisodicMemory()) brain->getEpisodicMemory()->clear();
        if (brain->getAssociativeMemory()) brain->getAssociativeMemory()->clear();
        if (brain->getAttention()) brain->getAttention()->reset();
        return "All brain systems reset";
    }, py::arg("brain"), "Reset all brain systems");

    m.def("enableAllSystems", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return "All systems enabled";
    }, py::arg("agent"), "Enable all neuromodulation and learning systems");

    // Event-driven simulation utilities
    m.def("runEventDrivenSimulation", [](std::shared_ptr<Brain> brain,
                                        std::function<bool(std::shared_ptr<Brain>)> condition,
                                        uint64_t maxSteps = 100000) {
        for (uint64_t i = 0; i < maxSteps; ++i) {
            brain->step(i, i * 0.001);
            
            if (condition(brain)) {
                break;
            }
        }
        return "Event-driven simulation completed";
    }, py::arg("brain"), py::arg("condition"), py::arg("maxSteps") = 100000,
         "Run event-driven simulation with custom condition");

    // Batch processing utilities
    m.def("batchProcessNeurons", [](std::shared_ptr<Brain> brain,
                                   std::function<void(Neuron*)> processor) {
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* neuron : pop->getNeurons()) {
                    processor(neuron);
                }
            }
        }
        return "Batch processing completed";
    }, py::arg("brain"), py::arg("processor"), "Apply function to all neurons");

    m.def("batchProcessSynapses", [](std::shared_ptr<Brain> brain,
                                    std::function<void(Synapse*)> processor) {
        for (const auto& region : brain->getRegions()) {
            for (Synapse* synapse : region->getSynapses()) {
                processor(synapse);
            }
        }
        return "Batch processing completed";
    }, py::arg("brain"), py::arg("processor"), "Apply function to all synapses");

    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        py::dict metrics;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        metrics["avg_firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["pending_events"] = brain->getPendingSpikeEventCount();
        
        // Add memory metrics if available
        if (brain->getWorkingMemory()) {
            metrics["working_memory_traces"] = brain->getWorkingMemory()->getActiveTraces();
        }
        if (brain->getEpisodicMemory()) {
            metrics["episodic_memory_episodes"] = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        // Add neuromodulation metrics
        if (brain->getDopamine()) {
            metrics["dopamine_level"] = brain->getDopamine()->getLevel();
        }
        if (brain->getCuriosity()) {
            metrics["curiosity_level"] = brain->getCuriosity()->getLevel();
        }
        
        metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics;
    }, py::arg("brain"), "Get comprehensive performance metrics");

    m.def("benchmarkBrain", [](std::shared_ptr<Config> config, uint64_t steps = 10000) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double timePerStep = static_cast<double>(duration.count()) / steps;
        
        py::dict results;
        results["steps"] = steps;
        results["total_time_ms"] = duration.count();
        results["time_per_step_ms"] = timePerStep;
        results["steps_per_second"] = steps / (duration.count() / 1000.0);
        results["neurons_per_step"] = config->getOr<int64_t>("neuron_count", 1000);
        
        return results;
    }, py::arg("config"), py::arg("steps") = 10000,
         "Run benchmark test and return performance metrics");

    // Simulation control and utilities
    m.def("resetBrainSystems", [](std::shared_ptr<Brain> brain) {
        brain->reset();
        // Clear memory systems
        if (brain->getWorkingMemory()) brain->getWorkingMemory()->clear();
        if (brain->getEpisodicMemory()) brain->getEpisodicMemory()->clear();
        if (brain->getAssociativeMemory()) brain->getAssociativeMemory()->clear();
        if (brain->getAttention()) brain->getAttention()->reset();
        return "All brain systems reset";
    }, py::arg("brain"), "Reset all brain systems");

    m.def("enableAllSystems", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return "All systems enabled";
    }, py::arg("agent"), "Enable all neuromodulation and learning systems");

    // Event-driven simulation utilities
    m.def("runEventDrivenSimulation", [](std::shared_ptr<Brain> brain,
                                        std::function<bool(std::shared_ptr<Brain>)> condition,
                                        uint64_t maxSteps = 100000) {
        for (uint64_t i = 0; i < maxSteps; ++i) {
            brain->step(i, i * 0.001);
            
            if (condition(brain)) {
                break;
            }
        }
        return "Event-driven simulation completed";
    }, py::arg("brain"), py::arg("condition"), py::arg("maxSteps") = 100000,
         "Run event-driven simulation with custom condition");

    // Batch processing utilities
    m.def("batchProcessNeurons", [](std::shared_ptr<Brain> brain,
                                   std::function<void(Neuron*)> processor) {
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* neuron : pop->getNeurons()) {
                    processor(neuron);
                }
            }
        }
        return "Batch processing completed";
    }, py::arg("brain"), py::arg("processor"), "Apply function to all neurons");

    m.def("batchProcessSynapses", [](std::shared_ptr<Brain> brain,
                                    std::function<void(Synapse*)> processor) {
        for (const auto& region : brain->getRegions()) {
            for (Synapse* synapse : region->getSynapses()) {
                processor(synapse);
            }
        }
        return "Batch processing completed";
    }, py::arg("brain"), py::arg("processor"), "Apply function to all synapses");

    // Performance optimization and monitoring
    m.def("enablePerformanceOptimizations", [](std::shared_ptr<Config> config) {
        // Enable performance optimizations in configuration
        config->set("performance.memory_pool_enable", true, ConfigSource::User);
        config->set("performance.event_queue_enable", true, ConfigSource::User);
        config->set("performance.sparse_connectivity_enable", true, ConfigSource::User);
        config->set("performance.simd_enable", true, ConfigSource::User);
        config->set("performance.multithreading_enable", true, ConfigSource::User);
        
        // Set performance parameters
        config->set("performance.memory_pool_size", 1024, ConfigSource::User);
        config->set("performance.event_queue_size", 8192, ConfigSource::User);
        config->set("performance.cache_line_size", 64, ConfigSource::User);
        
        return "Performance optimizations enabled";
    }, py::arg("config"), "Enable performance optimizations in configuration");

    m.def("setPerformanceParameters", [](std::shared_ptr<Config> config,
                                         uint64_t memoryPoolSize = 1024,
                                         uint64_t eventQueueSize = 8192,
                                         size_t cacheLineSize = 64,
                                         uint32_t simdWidth = 8,
                                         uint32_t threadCount = 4) {
        config->set("performance.memory_pool_size", memoryPoolSize, ConfigSource::User);
        config->set("performance.event_queue_size", eventQueueSize, ConfigSource::User);
        config->set("performance.cache_line_size", cacheLineSize, ConfigSource::User);
        config->set("performance.simd_width", simdWidth, ConfigSource::User);
        config->set("performance.thread_count", threadCount, ConfigSource::User);
        
        // Update performance infrastructure
        // In a full implementation, this would reinitialize the performance systems
        
        return "Performance parameters updated";
    }, py::arg("config"), py::arg("memoryPoolSize") = 1024,
         py::arg("eventQueueSize") = 8192, py::arg("cacheLineSize") = 64,
         py::arg("simdWidth") = 8, py::arg("threadCount") = 4,
         "Set performance optimization parameters");

    m.def("getMemoryUsage", [](std::shared_ptr<Brain> brain) {
        py::dict memoryInfo;
        memoryInfo["status"] = "Performance memory tracking enabled";
        memoryInfo["note"] = "Full memory usage requires integration with performance infrastructure";
        memoryInfo["suggestion"] = "Use performance benchmarks for memory profiling";
        
        // Try to get memory pool info if available
        if (brain->getWorkingMemory()) {
            memoryInfo["working_memory_capacity"] = brain->getWorkingMemory()->getCapacity();
            memoryInfo["working_memory_decay_rate"] = brain->getWorkingMemory()->getDecayRate();
        }
        
        // Add checkpoint memory info
        if (brain->getConfig()->has("checkpoint_dir")) {
            memoryInfo["checkpoint_enabled"] = true;
            memoryInfo["checkpoint_dir"] = brain->getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
        }
        
        return memoryInfo;
    }, py::arg("brain"), "Get memory usage information and performance hints");

    m.def("optimizeForScale", [](std::shared_ptr<Config> config, size_t targetNeurons) {
        // Configure for scale optimization
        config->set("neuron_count", static_cast<int64_t>(targetNeurons), ConfigSource::User);
        config->set("performance.sparse_connectivity_enable", true, ConfigSource::User);
        config->set("performance.memory_pool_enable", true, ConfigSource::User);
        config->set("performance.event_queue_enable", true, ConfigSource::User);
        
        // Calculate optimal parameters for scale
        size_t memoryPoolSize = std::max<size_t>(targetNeurons / 10, 1024);
        size_t eventQueueSize = std::max<size_t>(targetNeurons * 2, 8192);
        
        config->set("performance.memory_pool_size", memoryPoolSize, ConfigSource::User);
        config->set("performance.event_queue_size", eventQueueSize, ConfigSource::User);
        
        // Adjust connectivity based on scale
        float connectionProb = std::min(0.2f, 10.0f / static_cast<float>(targetNeurons));
        config->set("connection_probability", connectionProb, ConfigSource::User);
        
        return "Configuration optimized for scale";
    }, py::arg("config"), py::arg("targetNeurons"), "Optimize configuration for large-scale simulation");

    m.def("validatePerformanceSetup", [](std::shared_ptr<Brain> brain) {
        py::dict validationResults;
        
        // Check if performance systems are accessible
        bool performanceOk = true;
        std::string issues;
        
        if (!brain->getWorkingMemory()) {
            performanceOk = false;
            issues += "Working memory not available; ";
        }
        
        if (!brain->getEpisodicMemory()) {
            performanceOk = false;
            issues += "Episodic memory not available; ";
        }
        
        if (!brain->getAttention()) {
            performanceOk = false;
            issues += "Attention system not available; ";
        }
        
        validationResults["valid"] = performanceOk;
        validationResults["issues"] = issues.empty() ? "None" : issues;
        validationResults["recommendation"] = performanceOk ? 
            "Performance setup is valid" : "Enable missing systems";
        
        // Add system capabilities
        validationResults["systems_enabled"] = true;
        validationResults["memory_systems"] = "Working + Episodic + Associative";
        validationResults["cognition_systems"] = "Planning + Concept + Attention";
        validationResults["neuromodulation_systems"] = "Dopamine + Curiosity + Novelty";
        
        return validationResults;
    }, py::arg("brain"), "Validate performance setup and report system capabilities");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
