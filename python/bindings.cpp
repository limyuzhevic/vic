#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld,
        and memory systems including NeuralWorkingMemory,
        NeuralEpisodicMemory, and NeuralAssociativeMemory.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId - Unique identifier for a neuron
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

    // SynapseId - Unique identifier for a synapse
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

    // RegionId - Unique identifier for a brain region
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

    // PopulationId - Unique identifier for a neuron population
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class
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

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
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

    // Brain class
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

    // AgentBrain class
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

    // ===== DEVELOPMENT SYSTEM BINDINGS =====

    // DevelopmentSystem class for neural development
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system orchestrating brain development)

        The DevelopmentSystem implements developmental stages, plasticity modulation,
        and structural changes throughout the brain's lifespan. It controls:
        
        - Developmental stage progression (Initial -> CriticalPeriod -> Maturation -> Adult -> Aging)
        - Plasticity modifiers that adapt based on developmental stage
        - Critical period dynamics and plasticity windows
        - Automated development progression or manual stage control
        
        Key features:
        - Automatic development progression based on simulation time
        - Manual stage control for testing and advanced scenarios
        - Plasticity modulation across development stages
        - Integration with Brain's developmental stage management
        
        Methods:
        - getStage()/setStage(): Current developmental stage control
        - advanceStage(): Auto-advance to next developmental stage
        - update(brain, step): Development updates for simulation steps
        - update(brain, rng, dt): Development with random generator
        - getPlasticityModifier(): Current stage's plasticity factor
        - setPlasticityModifier(): Override plasticity for manual control
        - isCriticalPeriod(): Check if in critical period
        - getCriticalPeriodProgress(): Critical period completion percentage
        - getCriticalPeriodDuration(): Critical period duration
        - setCriticalPeriodDuration(): Override critical period duration
        - triggerPhaseTransition(): Force stage advancement
        - getMaturationRate()/setMaturationRate(): Neural maturation control
        "pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage (use DevelopmentStage enum)")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance to next developmental stage")
        .def("triggerPhaseTransition", &DevelopmentSystem::triggerPhaseTransition,
             "Force immediate phase transition to next stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get string name of current developmental stage")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get current stage's plasticity modifier (0.0-1.0)")
        .def("setPlasticityModifier", &DevelopmentSystem::setPlasticityModifier, py::arg("modifier"),
             "Set plasticity modifier (overrides stage-based value)")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if current stage is critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period completion percentage (0.0-1.0)")
        .def("getCriticalPeriodDuration", &DevelopmentSystem::getCriticalPeriodDuration,
             "Get critical period duration in simulation time units")
        .def("setCriticalPeriodDuration", &DevelopmentSystem::setCriticalPeriodDuration, py::arg("duration"),
             "Set critical period duration in simulation time units")
        .def("getMaturationRate", &DevelopmentSystem::getMaturationRate,
             "Get neural maturation rate")
        .def("setMaturationRate", &DevelopmentSystem::setMaturationRate, py::arg("rate"),
             "Set neural maturation rate")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age (simulation time elapsed)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age (simulation time)")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("currentStep"),
             "Update development for a single simulation step")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with random generator and timestep duration")
        .def("__repr__", [](const DevelopmentSystem& dev) {
            return "<DevelopmentSystem stage=" + std::string(dev.getStageName()) + 
                   " age=" + std::to_string(dev.getDevelopmentalAge()) + 
                   ">";
        });

    // Development system helper functions
    m.def("get_development_system", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            PyErr_SetString(PyExc_ValueError, "Brain is null");
            throw pybind11::error_already_set();
        }
        return brain->getDevelopmentSystem();
    }, py::arg("brain"),
        "Get the development system from a brain instance");

    m.def("is_development_enabled", [](const AgentBrain& agentBrain) {
        return agentBrain.isDevelopmentEnabled();
    }, py::arg("agentBrain"),
        "Check if development system is enabled in AgentBrain");

    m.def("update_development", [](AgentBrain& agentBrain, TimestepDuration timestep) {
        agentBrain.updateDevelopment(timestep);
    }, py::arg("agentBrain"), py::arg("timestep"),
        "Update development system through AgentBrain");

    m.def("auto_develop", [](std::shared_ptr<DevelopmentSystem> devSys, std::shared_ptr<Brain> brain, RandomGenerator& rng, TimestepDuration dt) {
        if (!devSys || !brain) {
            PyErr_SetString(PyExc_ValueError, "DevelopmentSystem or Brain is null");
            throw pybind11::error_already_set();
        }
        devSys->update(brain.get(), rng, dt);
    }, py::arg("devSys"), py::arg("brain"), py::arg("rng"), py::arg("dt"),
        "Perform automatic development updates");

    m.def("manual_stage_advance", [](std::shared_ptr<DevelopmentSystem> devSys) {
        if (!devSys) {
            PyErr_SetString(PyExc_ValueError, "DevelopmentSystem is null");
            throw pybind11::error_already_set();
        }
        devSys->triggerPhaseTransition();
    }, py::arg("devSys"),
        "Manually advance development to next stage");

    // Development system stage presets
    m.def("set_development_stage", [](std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
        if (!brain) {
            PyErr_SetString(PyExc_ValueError, "Brain is null");
            throw pybind11::error_already_set();
        }
        brain->setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"),
        "Set developmental stage using DevelopmentStage enum");

    m.def("get_development_stage", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            PyErr_SetString(PyExc_ValueError, "Brain is null");
            throw pybind11::error_already_set();
        }
        return brain->getDevelopmentalStage();
    }, py::arg("brain"),
        "Get developmental stage from brain");

    m.def("get_stage_name", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }, py::arg("stage"),
        "Get string name for developmental stage");

    // ===== NEUROMODULATION BINDINGS =====

    // Neuromodulator base class (abstract)

    // Neuromodulator base class (abstract)
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)

        Neuromodulator is the base class for all neuromodulators that provide
        chemical signals to the brain affecting plasticity, attention, and behavior.
        All neuromodulators must implement:
        
        - getName(): Returns the name of the neuromodulator
        - getLevel(): Gets current concentration/level
        - setLevel(level): Sets the concentration/level
        - getPlasticityFactor(): Returns the plasticity factor
        - update(dt): Updates the neuromodulator state
        "pbdoc")
        .def("getName", &Neuromodulator::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &Neuromodulator::getLevel, R"pbdoc(Get neuromodulator level)pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), R"pbdoc(Set neuromodulator level)pbdoc")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, R"pbdoc(Get plasticity factor)pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"), R"pbdoc(Update neuromodulator state)pbdoc")
        .def("isNull", [](const Neuromodulator* mod) { return mod == nullptr; }, R"pbdoc(Check if neuromodulator is null)pbdoc")
        .def("checkNull", [](Neuromodulator* mod) {
            if (!mod) {
                PyErr_SetString(PyExc_ValueError, "Neuromodulator is null");
                throw pybind11::error_already_set();
            }
        }, py::arg("modulator"), R"pbdoc(Check if neuromodulator is null, raise exception if null)pbdoc");

    // ===== BRAIN NEUROMODULATORS =====

    // Dopamine: Reward and reinforcement learning signal
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - Reward and reinforcement learning signal)

        Dopamine is the brain's primary reward neuromodulator. It signals:
        - Reward prediction errors (positive for better-than-expected rewards)
        - Actual rewards (for immediate reinforcement)
        - Promotes synaptic strengthening for rewarded behaviors
        
        Key methods:
        - signalReward(reward): Signal a reward
        - signalRewardPredictionError(error): Signal reward prediction error
        - getLevel(): Get dopamine level (range: -1.0 to 1.0)
        - update(dt): Update dopamine dynamics
        "pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), R"pbdoc(Signal a reward)pbdoc")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), R"pbdoc(Signal reward prediction error)pbdoc")
        .def("reset", &Dopamine::reset, R"pbdoc(Reset dopamine state)pbdoc")
        .def("createWithLevel", [](float level) {
            auto mod = std::make_shared<Dopamine>();
            mod->setLevel(level);
            return mod;
        }, py::arg("level"), R"pbdoc(Create dopamine neuromodulator with initial level)pbdoc");

    // Curiosity: Exploration motivation based on novelty and prediction error
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive - Exploration motivation based on novelty and prediction error)

        Curiosity is the drive for exploration, computed as a weighted combination
        of novelty and prediction error. It motivates the agent to explore novel
        or unexpected situations to learn new information.
        
        Key methods:
        - getLevel(): Get curiosity level (range: 0.0 to 1.0)
        - getExplorationDrive(): Get exploration drive (same as level)
        - update(novelty, predictionError, dt): Update curiosity based on inputs
        - setNoveltyWeight(weight): Set weight for novelty input
        - setPredictionErrorWeight(weight): Set weight for prediction error input
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), R"pbdoc(Initialize curiosity with brain reference)pbdoc")
        .def("getLevel", &Curiosity::getLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, R"pbdoc(Get exploration drive)pbdoc")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), R"pbdoc(Update curiosity based on novelty and prediction error)pbdoc")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), R"pbdoc(Set novelty weight)pbdoc")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), R"pbdoc(Set prediction error weight)pbdoc")
        .def("reset", &Curiosity::reset, R"pbdoc(Reset curiosity state)pbdoc")
        .def("createWithWeights", [](float noveltyWeight, float predictionErrorWeight) {
            auto curiosity = std::make_shared<Curiosity>();
            curiosity->setNoveltyWeight(noveltyWeight);
            curiosity->setPredictionErrorWeight(predictionErrorWeight);
            return curiosity;
        }, py::arg("noveltyWeight") = 0.7f, py::arg("predictionErrorWeight") = 0.3f, R"pbdoc(Create curiosity with custom weights)pbdoc");

    // Novelty: Novelty detection signal
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection - Computes novelty from comparison with previous observations)

        Novelty detection computes how novel an observation is compared to
        previous observations. Novelty is higher when current observations
        differ significantly from what's been experienced before.
        
        Key methods:
        - getLevel(): Get novelty level (range: 0.0 to 1.0)
        - setLevel(level): Set novelty level directly
        - detectNovelty(observation, previousObservation): Detect novelty from observations
        - detectNovelty(currentPattern, previousPattern): Detect novelty from patterns
        - update(dt): Decay novelty over time
        - getHistory(): Get novelty history
        - clearHistory(): Clear novelty history
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), R"pbdoc(Initialize novelty with brain reference)pbdoc")
        .def("getLevel", &Novelty::getLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), R"pbdoc(Set novelty level)pbdoc")
        .def("detectNovelty", [](Novelty& self, const Observation& observation, const Observation& previousObservation) {
            self.detectNovelty(observation, previousObservation);
        }, py::arg("observation"), py::arg("previousObservation"), R"pbdoc(Detect novelty from observations)pbdoc")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern, const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"), R"pbdoc(Detect novelty from patterns)pbdoc")
        .def("update", &Novelty::update, py::arg("dt"), R"pbdoc(Update and decay novelty)pbdoc")
        .def("getHistory", &Novelty::getHistory, R"pbdoc(Get novelty history)pbdoc")
        .def("clearHistory", &Novelty::clearHistory, R"pbdoc(Clear novelty history)pbdoc")
        .def("setSensitivity", [](Novelty& self, float sensitivity) {
            self.setSensitivity(sensitivity);
        }, py::arg("sensitivity"), R"pbdoc(Set novelty sensitivity)pbdoc")
        .def("getSensitivity", &Novelty::getSensitivity, R"pbdoc(Get novelty sensitivity)pbdoc")
        .def("createWithSensitivity", [](float sensitivity) {
            auto novelty = std::make_shared<Novelty>();
            novelty->setSensitivity(sensitivity);
            return novelty;
        }, py::arg("sensitivity") = 1.0f, R"pbdoc(Create novelty with sensitivity)pbdoc");

    // PredictionError: Prediction error signal for curiosity and learning
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error - Computes difference between predicted and actual values)

        Prediction error is the difference between predicted and actual outcomes.
        It's fundamental to learning and curiosity, signaling when predictions
        are wrong and prompting exploration to correct them.
        
        Key methods:
        - getError(): Get prediction error value
        - computeError(predicted, actual): Compute prediction error
        - updatePrediction(newPrediction): Update prediction with new value
        - getMagnitude(): Get error magnitude for neuromodulation
        - getHistory(): Get prediction error history
        - clearHistory(): Clear prediction error history
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), R"pbdoc(Initialize prediction error with brain reference)pbdoc")
        .def("getError", &PredictionError::getError, R"pbdoc(Get prediction error)pbdoc")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), R"pbdoc(Compute prediction error)pbdoc")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), R"pbdoc(Update prediction)pbdoc")
        .def("getHistory", &PredictionError::getHistory, R"pbdoc(Get error history)pbdoc")
        .def("clearHistory", &PredictionError::clearHistory, R"pbdoc(Clear error history)pbdoc")
        .def("getMagnitude", &PredictionError::getMagnitude, R"pbdoc(Get error magnitude)pbdoc")
        .def("reset", &PredictionError::reset, R"pbdoc(Reset prediction error state)pbdoc")
        .def("createWithInitial", [](float initialError) {
            auto predError = std::make_shared<PredictionError>();
            predError->computeError(initialError, initialError);
            return predError;
        }, py::arg("initialError") = 0.0f, R"pbdoc(Create prediction error with initial value)pbdoc");

    // ===== NEURAL WORKING MEMORY BINDINGS =====

    // NeuralWorkingMemory class
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", 
        R"pbdoc(Neural working memory system implementing persistent activity patterns)

        NeuralWorkingMemory stores information as neural activity patterns using:
        - Persistent neural activity (reverberating activity)
        - Recurrent synaptic connections for maintenance
        - Activity-dependent short-term plasticity
        - Neural competition for selective retention

        Key methods:
        - store(pattern, strength): Store neural activity pattern with given strength
        - retrieve(): Get current memory content as activity levels
        - storeToNeuron(neuron, activation): Store activation in specific neuron
        - getNeuronActivation(neuron): Get activation level of a neuron
        - update(dt): Update memory (decay and maintenance)
        - strengthenMemory(factor): Strengthen memory representation
        - runCompetition(): Run competition between memory traces
        - isWinning(neuron): Check if neuron is in winning population
        - getMemoryActivity(): Get overall memory activity level
        - clear(): Clear all memory traces
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize working memory with brain reference - transfers ownership")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store a neural activity pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, 
             py::arg("neuron"), py::arg("activation"),
             "Store activation in specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, 
             "Get current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, 
             py::arg("neuron"), "Get activation level of a specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is part of the winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate for memory traces")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             py::return_value_policy::reference_internal,
             "Get neurons currently in working memory");

    // ===== NEURAL EPISODIC MEMORY BINDINGS =====

    // EpisodicMemoryItem struct
    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", 
        R"pbdoc(Neural episodic memory item representing an experience)

        Stores neural experience data including:
        - Timestamp and sensory state
        - Location/position information
        - Action taken
        - Internal state (reward, energy, novelty)
        - Resulting sensory state and reward
        - Associated neural activity patterns
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
        .def_readwrite("action", &EpisodicMemoryItem::action)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("energy", &EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &EpisodicMemoryItem::age);

    // NeuralEpisodicMemory class
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory",
        R"pbdoc(Neural episodic memory system storing experiences as neural patterns)

        NeuralEpisodicMemory implements episodic memory with:
        - Pattern completion (Hopfield-like recall)
        - Temporal ordering of episodes
        - Relevance-weighted retrieval
        - Replay during consolidation

        Key methods:
        - storeEpisode(episode): Store new experience
        - retrieveSimilar(pattern): Find similar episodes by pattern
        - retrieveTemporal(startTime, endTime): Get episodes in time window
        - retrieveByLocation(x, y, radius): Get episodes by location
        - retrieveAfterAction(action): Get episodes after specific action
        - replayEpisode(episode): Reactivate episode neural patterns
        - getRecentEpisodes(count): Get recent episodes
        - getAverageReward(): Get average reward from episodes
        - consolidate(threshold): Clear low-relevance episodes
        - enableReplay(enable): Enable/disable replay
        - getEpisodesForReplay(count): Get episodes for replay
        - clear(): Clear all episodes
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize episodic memory with brain reference - transfers ownership")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience as an episode")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, 
             py::arg("pattern"), py::arg("maxResults") = 5,
             "Retrieve similar episodes by sensory pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location (x, y, radius)")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following a specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates neural patterns")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance (placeholder - aging based)")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Consolidate (remove low-relevance episodes)")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable episode replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"),
             "Get episodes selected for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes in sequence");

    // ===== NEURAL ASSOCIATIVE MEMORY BINDINGS =====

    // NeuralAssociativeMemory class
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory",
        R"pbdoc(Neural associative memory creating pattern associations)

        NeuralAssociativeMemory implements associative memory with:
        - Hebbian-like association formation
        - Co-activation based strengthening
        - Pattern completion via spreading activation
        - Reward-modulated association updating

        Key methods:
        - associate(patternA, patternB, strength): Create association
        - associateFromExperience(episode): Create from stored experience
        - retrieve(queryPattern): Get patterns associated with query
        - getAssociationStrength(patternA, patternB): Get association strength
        - updateAssociation(patternA, patternB, delta): Update association
        - spreadActivation(cuePattern): Spread activation through associations
        - clear(): Clear all associations
        "pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize associative memory with brain reference - transfers ownership")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"),
             "Create association from stored experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association strength based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern through associations")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear all associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

    // ===== PLASTICITY RULE BINDINGS =====

    // PlasticityRule base class (abstract)
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)

        PlasticityRule is the base class for all synaptic plasticity rules that implement
        spike-timing-dependent and Hebbian learning mechanisms. Each rule implements:

        - update(): Called each simulation step to update synaptic weights based on activity
        - applyWeightChange(): Direct application of a weight change to a synapse
        - getName(): Returns the name of the plasticity rule

        Derived classes include STDP, Hebbian, and others.
        "pbdoc")
        .def("update", &PlasticityRule::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on pre/post synaptic activity")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange, py::arg("synapse"), py::arg("delta"),
             "Apply a weight change to a synapse")
        .def("getName", &PlasticityRule::getName, "Get the name of this plasticity rule")
        .def("isEnabled", &PlasticityRule::isEnabled, "Check if the plasticity rule is enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), "Enable or disable the plasticity rule");

    // STDP (Spike-Timing Dependent Plasticity) class
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(STDP - Spike-Timing Dependent Plasticity)

        STDP implements spike-timing dependent plasticity based on the timing
        difference between pre- and post-synaptic spikes. Key features:

        - Time window τ ~ 20ms (adjustable)
        - Potentiation (A+) when pre fires before post (Δt > 0)
        - Depression (A-) when post fires before pre (Δt < 0)
        - Exponential decay of plasticity effect with time
        - Reward-modulated learning via eligibility traces

        STDP implements "neurons that fire together, wire together" with precise timing.
        "pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set Long-Term Potentiation weight (A+)")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get Long-Term Potentiation weight (A+)")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set Long-Term Depression weight (A-)")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get Long-Term Depression weight (A-)")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set STDP time constant (τ, in ms)")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant (τ, in ms)")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), "Configure STDP parameters in one call");

    // Hebbian class
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning - "neurons that fire together, wire together")

        Hebbian implements classic Hebbian learning based on co-activity of neurons.
        Key features:

        - Learning rate η controlling weight change magnitude
        - Maximum/minimum synaptic weight bounds
        - Activity-dependent learning (correlation-based)
        - "Fire together, wire together" principle

        Good for implementing associative memory and pattern completion.
        "pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate (η)")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate (η)")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"), "Set maximum synaptic weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight, "Get maximum synaptic weight");

    // StructuralPlasticity class (standalone, not inheriting PlasticityRule)
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural Plasticity - creation and removal of synapses and neurons)

        StructuralPlasticity implements dynamic changes to network structure:
        - Creation of new synapses (synaptogenesis)
        - Elimination of existing synapses (synaptic pruning)
        - Creation of new neurons (neurogenesis)
        - Elimination of existing neurons (neuronal apoptosis)

        Controls:
        - Synaptogenesis rate (creation of new connections)
        - Pruning rate (elimination of connections)
        - Activity-dependent structural changes

        Enables network growth and adaptation throughout development and learning.
        "pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse, py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"),
             "Create a new synapse connecting source to destination neuron")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse, py::arg("brain"), py::arg("synapse"), "Remove a synapse from the brain")
        .def("createNeuron", &StructuralPlasticity::createNeuron, py::arg("brain"), py::arg("type"), "Create a new neuron with given type")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron, py::arg("brain"), py::arg("neuron"), "Remove a neuron from the brain")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, "Get synaptogenesis rate (synapses created per timestep)")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"), "Set synaptogenesis rate (synapses created per timestep)")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, "Get pruning rate (synapses removed per timestep)")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), "Set pruning rate (synapses removed per timestep)")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"), "Update structural plasticity (create/remove synapses and neurons)");

    // ===== PLASTICITY CONTROL WRAPPER FUNCTIONS =====

    // STDP configuration helper
    m.def("configure_stdp", [](std::shared_ptr<STDP> stdp, float ltpWeight, float ltdWeight, float tau) {
        if (!stdp) {
            PyErr_SetString(PyExc_ValueError, "STDP rule is null");
            throw pybind11::error_already_set();
        }
        stdp->configure(ltpWeight, ltdWeight, tau);
    }, py::arg("stdp"), py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), R"pbdoc(Configure STDP parameters in one call)pbdoc");

    // Hebbian configuration helper
    m.def("configure_hebbian", [](std::shared_ptr<Hebbian> hebbian, float learningRate, float maxWeight) {
        if (!hebbian) {
            PyErr_SetString(PyExc_ValueError, "Hebbian rule is null");
            throw pybind11::error_already_set();
        }
        hebbian->setLearningRate(learningRate);
        hebbian->setMaxWeight(maxWeight);
    }, py::arg("hebbian"), py::arg("learningRate"), py::arg("maxWeight"), R"pbdoc(Configure Hebbian parameters in one call)pbdoc");

    // Getters for plasticity rules (helper functions)
    m.def("get_stdp_parameters", [](std::shared_ptr<STDP> stdp) {
        if (!stdp) {
            PyErr_SetString(PyExc_ValueError, "STDP rule is null");
            throw pybind11::error_already_set();
        }
        return std::make_tuple(stdp->getLTPWeight(), stdp->getLTDWeight(), stdp->getTimeConstant());
    }, py::arg("stdp"), R"pbdoc(Get STDP parameters as tuple (ltpWeight, ltdWeight, timeConstant))pbdoc");

    m.def("get_hebbian_parameters", [](std::shared_ptr<Hebbian> hebbian) {
        if (!hebbian) {
            PyErr_SetString(PyExc_ValueError, "Hebbian rule is null");
            throw pybind11::error_already_set();
        }
        return std::make_tuple(hebbian->getLearningRate(), hebbian->getMaxWeight());
    }, py::arg("hebbian"), R"pbdoc(Get Hebbian parameters as tuple (learningRate, maxWeight))pbdoc");

    // Brain plasticity system access helpers
    m.def("get_plasticity_system", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            PyErr_SetString(PyExc_ValueError, "Brain is null");
            throw pybind11::error_already_set();
        }
        return std::make_tuple(brain->getSTDP(), brain->getHebbian(), brain->getStructuralPlasticity());
    }, py::arg("brain"), R"pbdoc(Get plasticity system from brain (stdp, hebbian, structuralPlasticity))pbdoc");

    // Create default plasticity rule instances
    m.def("create_stdp", []() -> std::shared_ptr<STDP> {
        return std::make_shared<STDP>();
    }, R"pbdoc(Create a new STDP plasticity rule instance)pbdoc");

    m.def("create_hebbian", []() -> std::shared_ptr<Hebbian> {
        return std::make_shared<Hebbian>();
    }, R"pbdoc(Create a new Hebbian plasticity rule instance)pbdoc");

    m.def("create_structural_plasticity", []() -> std::shared_ptr<StructuralPlasticity> {
        return std::make_shared<StructuralPlasticity>();
    }, R"pbdoc(Create a new StructuralPlasticity instance)pbdoc");

    // Validate plasticity operations
    m.def("validate_plasticity_operation", [](std::shared_ptr<PlasticityRule> rule) {
        if (!rule) {
            PyErr_SetString(PyExc_ValueError, "Plasticity rule is null");
            throw pybind11::error_already_set();
        }
        std::string ruleName = rule->getName();
        bool enabled = rule->isEnabled();
        return std::make_tuple(ruleName, enabled);
    }, py::arg("rule"), R"pbdoc(Validate a plasticity rule and return its name and enabled status)pbdoc");

    // ===== EXAMPLE USAGE DOCUMENTATION =====
    m.attr("PLASTICITY_EXAMPLES") = R"pbdoc(
        Advanced Plasticity Control Examples:

        Example 1: Configure STDP learning rule
        =======================================
        >>> import pynlm as nlm
        >>> stdp = nlm.create_stdp()
        >>> # Configure with custom parameters
        >>> stdp.setLTPWeight(0.02)
        >>> stdp.setLTDWeight(0.025)
        >>> stdp.setTimeConstant(30.0)
        >>> 
        Example 2: Configure Hebbian learning
        ======================================
        >>> import pynlm as nlm
        >>> hebbian = nlm.create_hebbian()
        >>> hebbian.setLearningRate(0.01)
        >>> hebbian.setMaxWeight(2.0)
        >>> 
        Example 3: Use plasticity system with brain
        ==========================================
        >>> import pynlm as nlm
        >>> config = nlm.create_default_config()
        >>> brain = nlm.createBrain(config)
        >>> stdp, hebbian, structural = nlm.get_plasticity_system(brain)
        >>> 
        Example 4: Advanced plasticity configuration
        ===========================================
        >>> import pynlm as nlm
        >>> stdp = nlm.create_stdp()
        >>> hebbian = nlm.create_hebbian()
        >>> # Configure both in one call
        >>> nlm.configure_stdp(stdp, 0.01, 0.02, 25.0)
        >>> nlm.configure_hebbian(hebbian, 0.005, 1.5)
        >>> 
        Example 5: Validate and enable plasticity rules
        ===============================================
        >>> import pynlm as nlm
        >>> rules = [stdp, hebbian]
        >>> for rule in rules:
        ...     name, enabled = nlm.validate_plasticity_operation(rule)
        ...     print(f"Rule: {name}, Enabled: {enabled}")
        ...     rule.setEnabled(True)
        >>> 
        Best practices:
        - Always validate plasticity rules before using them
        - Use configure_*() helpers for multiple parameter settings
        - Check brain exists before structural plasticity operations
        - Enable/disable plasticity rules to control learning
        "pbdoc";
}

} // namespace nlm