#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/attentional/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Usage Example:
        -------------
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> # Run simulation steps
        >>> action = brain.produceAction()
        
        All C++ exceptions are translated to Python RuntimeError with descriptive messages.
        Null pointer references result in ValueError with operation context.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // NeuronId bindings with error checking
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId bindings
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId bindings
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>("SynapseType", m, R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with improved error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Returns True on success, False on failure.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError on failure.")
        .def("batch_set", &Config::batch_set, py::arg("keyValues"), py::arg("source") = ConfigSource::Runtime,
             "Set multiple configuration values at once. Requires vector of key-value pairs.")
        .def("batch_get", &Config::batch_get, py::arg("keys"),
             "Get multiple configuration values at once. Returns vector of optional ConfigValue.")
        .def("batch_remove", &Config::batch_remove, py::arg("keys"),
             "Remove multiple configuration keys at once.")
        .def("get_batch_stats", &Config::get_batch_stats,
             "Get batch statistics about the configuration (entry counts, types, sources).")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input.")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector. Returns list of floats.")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality. Returns list of ints.")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp. Returns int.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Requires int argument.");

    // Vision class with proper documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Initialize vision with width, height, and channels.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data. Requires list of float values.")
        .def("getWidth", &Vision::getWidth, "Get image width. Returns int.")
        .def("getHeight", &Vision::getHeight, "Get image height. Returns int.")
        .def("getChannels", &Vision::getChannels, "Get number of channels. Returns int.");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize audio with sample rate and number of samples.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data. Requires list of float values.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. Requires int argument.")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate. Returns int.")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples. Returns int.");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value. Requires float argument.")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals.");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with just a type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters.")
        .def("getType", &Action::getType, "Get action type. Returns ActionType enum.")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type. Requires ActionType argument.")
        .def("getParameters", &Action::getParameters,
             "Get action parameters. Returns list of floats.")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. Requires list of float values.")
        .def("getName", &Action::getName,
             "Get human-readable name of action. Returns string.")
        .def("clone", &Action::clone,
             "Create a copy of this action. Returns new Action object.");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object at position with type, value, and radius.")
        .def_readwrite("x", &WorldObject::x, "Object x position.")
        .def_readwrite("y", &WorldObject::y, "Object y position.")
        .def_readwrite("radius", &WorldObject::radius, "Object radius.")
        .def_readwrite("type", &WorldObject::type, "Object type.")
        .def_readwrite("value", &WorldObject::value, "Object value.")
        .def_readwrite("active", &WorldObject::active, "Whether object is active.");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Body x position.")
        .def_readwrite("y", &AgentBody::y, "Body y position.")
        .def_readwrite("orientation", &AgentBody::orientation, "Body orientation (radians).")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Body velocity X.")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Body velocity Y.")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Body angular velocity.")
        .def_readwrite("energy", &AgentBody::energy, "Body energy level.")
        .def_readwrite("health", &AgentBody::health, "Body health.")
        .def_readwrite("age", &AgentBody::age, "Body age.")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether body is moving.")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether body is turning.")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action.")
        .def("reset", &AgentBody::reset, "Reset body to initial state.");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message.")
        .def_readwrite("reward", &ActionResult::reward, "Reward value.")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful.")
        .def_readwrite("message", &ActionResult::message, "Result message.");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision data. Returns Vision object.")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data. Requires Vision object.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width. Returns int.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height. Returns int.")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch data. Returns list of floats.")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data. Requires list of float values.")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals. Returns list of floats.")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals. Requires list of float values.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception data. Returns list of floats.")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data. Requires list of float values.")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio data. Returns Audio object.")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data. Requires Audio object.")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals. Returns list of SensoryInput pointers.")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp. Returns int.")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp. Requires int argument.");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with dimensions and vision size.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world for one timestep.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command. Requires MotorCommand and current time.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept. Returns SensoryPercept object.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body. Returns AgentBody object.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object. Requires WorldObject.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid. Returns bool.")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width. Returns int.")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height. Returns int.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get max energy. Returns float.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time. Returns float.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed.")
        .def("__repr__", [](const SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld: " << world.getWidth() << "x" << world.getHeight() 
               << ", agent at (" << world.getAgentBody().x << "," << world.getAgentBody().y 
               << ")>";
            return ss.str();
        });

    // Brain class with comprehensive documentation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Raises ValueError if config is null.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success, False on failure.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Requires step number.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Requires step number and current time.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Raises ValueError if input is invalid.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True on success, False on failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success, False on failure.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns RegionId.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns NeuralRegion or None if not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions. Returns int.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs. Returns list of RegionId.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions. Returns list of NeuralRegion.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions. Returns int.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions. Returns int.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons. Returns int.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons. Returns int.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons. Returns float.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio. Returns float.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count. Returns int.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage. Returns DevelopmentalStage enum.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration. Returns Config object.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to the log system.");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with brain pointer. Raises ValueError if brain is null.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns int.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns int.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns MotorCommand.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage. Returns DevelopmentalStage enum.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level. Returns float.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level. Returns float.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level. Returns float.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error. Returns float.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain. Returns Brain object.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled. Returns bool.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled. Returns bool.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled. Returns bool.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled. Returns bool.");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration. Returns Config object.");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. Returns Brain object.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world. Returns SimpleWorld object.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. Returns AgentBrain object.");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and info
    m.def("version", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get the version of the NLM Python bindings.");

    m.def("getBuildInfo", []() {
        return "Built with C++17, pybind11 2.11.0";
    }, "Get build information.");
}

} // namespace nlm
