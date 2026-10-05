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
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) { return &self; })
        .def("__exit__", [](SimpleWorld& self, py::args) { self.reset(); });

        .def("getNeurons", &Brain::getNeurons,
             py::return_value_policy::reference_internal,
             "Get all neurons")
        .def("getSynapses", &Brain::getSynapses,
             py::return_value_policy::reference_internal,
             "Get all synapses")
        .def("getNeuronById", &Brain::getNeuronById,
             py::return_value_policy::reference_internal,
             "Get a neuron by ID")
        .def("getSynapseById", &Brain::getSynapseById,
             py::return_value_policy::reference_internal,
             "Get a synapse by ID")
        .def("getNeuronsByType", &Brain::getNeuronsByType,
             py::arg("type"),
             "Get neurons of a specific type")
        .def("getConnectionsToNeuron", &Brain::getConnectionsToNeuron,
             py::arg("neuronId"),
             "Get connections to a specific neuron")
        .def("findNeuronsByRegion", &Brain::findNeuronsByRegion,
             py::arg("regionId"),
             "Find neurons in a specific region")
        .def("getActiveNeurons", &Brain::getActiveNeurons,
             py::return_value_policy::reference_internal,
             "Get all active neurons")
        .def("getFiringNeurons", &Brain::getFiringNeurons,
             py::return_value_policy::reference_internal,
             "Get all firing neurons")
        .def("getRegionByName", &Brain::getRegionByName,
             py::arg("name"),
             py::return_value_policy::reference_internal,
             "Get region by name")
        .def("findRegionByName", &Brain::findRegionByName,
             py::arg("name"),
             py::return_value_policy::reference_internal,
             "Find region by name (returns nullptr if not found)")
        .def("getInterRegionConnections", &Brain::getInterRegionConnections,
             py::return_value_policy::reference_internal,
             "Get all inter-region connections")
        .def("getIncomingInterRegionConnections", &Brain::getIncomingInterRegionConnections,
             py::arg("targetRegion"),
             py::return_value_policy::reference_internal,
             "Get incoming inter-region connections to a region")
        .def("getOutgoingInterRegionConnections", &Brain::getOutgoingInterRegionConnections,
             py::arg("sourceRegion"),
             py::return_value_policy::reference_internal,
             "Get outgoing inter-region connections from a region")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"),
             py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add an inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove an inter-region connection")
        .def("getInterRegionConnectionCount", &Brain::getInterRegionConnectionCount,
             "Get number of inter-region connections")
        .def("clearInterRegionConnections", &Brain::clearInterRegionConnections,
             "Clear all inter-region connections")
        .def("getConnectivityMatrix", &Brain::getConnectivityMatrix,
             py::return_value_policy::reference_internal,
             "Get connectivity matrix between regions")
        .def("exportRegionGraph", &Brain::exportRegionGraph,
             "Export region connectivity as graph data")
        .def("importRegionGraph", &Brain::importRegionGraph,
             py::arg("graphData"),
             "Import region connectivity from graph data")
        // ========== ENHANCED STATISTICS ==========
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getFiringRateVariance", &Brain::getFiringRateVariance,
             "Get variance in firing rate")
        .def("getFiringRateStandardDeviation", &Brain::getFiringRateStandardDeviation,
             "Get standard deviation of firing rate")
        .def("getMaxFiringRate", &Brain::getMaxFiringRate,
             "Get maximum firing rate")
        .def("getMinFiringRate", &Brain::getMinFiringRate,
             "Get minimum firing rate")
        .def("getMedianFiringRate", &Brain::getMedianFiringRate,
             "Get median firing rate")
        // ========== MEMORY SYSTEM ACCESS ==========
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
        // ========== PREDICTION SYSTEM ==========
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        // ========== COGNITION SYSTEMS ==========
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttentionalSelection", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // ========== DEVELOPMENT SYSTEM ==========
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        // ========== NEUROMODULATION SYSTEMS ==========
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // ========== CONFIG AND RANDOM ==========
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        // ========== LOGGING ==========
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // ========== PYTHONIC INTERFACE ENHANCEMENTS ==========
        // Region iteration support
        .def("regionBegin", &Brain::regionBegin,
             "Get iterator to beginning of regions")
        .def("regionEnd", &Brain::regionEnd,
             "Get iterator to end of regions")
        .def("forEachRegion", [](const Brain& self, py::function callback) {
            for (auto rid : self.getRegionIds()) {
                auto* region = self.getRegion(rid);
                if (region) {
                    callback(rid, region);
                }
            }
        }, py::arg("callback"),
             "Iterate over all regions with callback")
        // Safe property access with defaults
        .def("getExcitationInhibitionRatioSafe", &Brain::getExcitationInhibitionRatioSafe,
             "Get excitation/inhibition ratio with default handling")
        .def("getTotalSpikeCountSafe", &Brain::getTotalSpikeCountSafe,
             "Get total spike count with default handling")
        .def("getPendingSpikeEventCountSafe", &Brain::getPendingSpikeEventCountSafe,
             "Get pending spike event count with default handling")
        // Simulation control
        .def("runForSteps", &Brain::runForSteps,
             py::arg("steps"), py::arg("verbose") = false,
             "Run simulation for specified number of steps")
        .def("runUntilStable", &Brain::runUntilStable,
             py::arg("threshold") = 0.95f, py::arg("maxSteps") = 1000,
             "Run simulation until firing rate stabilizes")
        // Data export
        .def("exportNeuronData", &Brain::exportNeuronData,
             "Export neuron data for analysis")
        .def("exportSynapseData", &Brain::exportSynapseData,
             "Export synapse data for analysis")
        // Batch operations
        .def("batchInjectCurrent", &Brain::batchInjectCurrent,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of a specific type")
        .def("batchAddRegions", &Brain::batchAddRegions,
             py::arg("count"), py::arg("prefix") = "",
             "Add multiple regions with sequential names")
        // Configuration shortcuts
        .def("setDefaultLearningRate", &Brain::setDefaultLearningRate,
             py::arg("rate"),
             "Set default learning rate")
        .def("setDefaultPlasticityEnabled", &Brain::setDefaultPlasticityEnabled,
             py::arg("enabled"),
             "Enable/disable default plasticity")
        .def("setDefaultDevelopmentEnabled", &Brain::setDefaultDevelopmentEnabled,
             py::arg("enabled"),
             "Enable/disable default development")
        // Monitoring and statistics
        .def("getLearningProgress", &Brain::getLearningProgress,
             "Get learning progress (0.0 to 1.0)")
        .def("getPerformanceScore", &Brain::getPerformanceScore,
             "Get overall performance score (0.0 to 1.0)")
        .def("isTrainingComplete", &Brain::isTrainingComplete,
             "Check if training is complete")
        // ========== CONTEXT MANAGER SUPPORT ==========
        .def("__enter__", [](Brain& self) { return &self; })
        .def("__exit__", [](Brain& self, py::args) { self.reset(); });

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
        // Context manager support
        .def("__enter__", [](AgentBrain& self) { return &self; })
        .def("__exit__", [](AgentBrain& self, py::args) { self.reset(); });

    // Global creation functions with better Pythonic interfaces
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
    
    // Simulation convenience methods
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int numSteps, bool verbose = false) {
        if (!brain) throw std::runtime_error("Brain is null");
        brain->initialize();
        
        for (nlm::SimulationStep step = 0; step < numSteps; ++step) {
            brain->step(step);
            if (verbose) {
                auto config = brain->getConfig();
                std::cout << "Step " << step << ": "
                          << "Neurons: " << brain->getTotalNeuronCount()
                          << ", Active: " << brain->getActiveNeuronCount()
                          << ", Firing: " << brain->getFiringNeuronCount()
                          << ", Firing Rate: " << brain->getAverageFiringRate()
                          << std::endl;
            }
        }
        
        return brain;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("verbose") = false,
        "Run a simulation for the specified number of steps");
    
    m.def("runWorldSimulation", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, 
                                  int numSteps, bool verbose = false) {
        if (!world || !agent) throw std::runtime_error("World or AgentBrain is null");
        
        auto brain = agent->getBrain();
        if (!brain) throw std::runtime_error("Brain is null");
        
        // Initialize brain and agent
        brain->initialize();
        agent->initialize(*world);
        
        for (int step = 0; step < numSteps; ++step) {
            // Get sensory percept from world
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Decode motor command
            auto motorCmd = agent->decodeMotorCommand();
            
            // Apply to world
            world->applyMotorCommand(motorCmd, step);
            
            // Update world
            world->update(step);
            
            if (verbose) {
                std::cout << "Step " << step << ": "
                          << "World pos: (" << world->getAgentBody()->x << ", " 
                          << world->getAgentBody()->y << ")"
                          << ", Energy: " << world->getAgentBody()->energy
                          << std::endl;
            }
        }
        
        return world;
    }, py::arg("world"), py::arg("agent"), py::arg("numSteps"), py::arg("verbose") = false,
        "Run a complete world simulation with agent");
    
    // Error handling utilities
    m.def("checkBrainHealth", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        bool healthy = brain->initialize();
        if (!healthy) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Check critical statistics
        if (brain->getTotalNeuronCount() == 0) {
            throw std::runtime_error("Brain has no neurons");
        }
        
        return true;
    }, py::arg("brain"), "Check if a brain is in a healthy state");
    
    // Statistics and introspection utilities
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        std::stringstream ss;
        ss << "Brain Statistics:\n";
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
        ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "  Regions: " << brain->getRegionCount() << "\n";
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage());
        
        return ss.str();
    }, py::arg("brain"), "Get comprehensive statistics about a brain");
    
    m.def("getWorldStats", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) throw std::runtime_error("World is null");
        
        std::stringstream ss;
        ss << "World Statistics:\n";
        ss << "  Width: " << world->getWidth() << "\n";
        ss << "  Height: " << world->getHeight() << "\n";
        ss << "  Max Energy: " << world->getMaxEnergy() << "\n";
        ss << "  Energy Decay Rate: " << world->getEnergyDecayRate() << "\n";
        ss << "  Simulation Time: " << world->getSimulationTime() << "\n";
        
        auto body = world->getAgentBody();
        ss << "  Agent Position: (" << body->x << ", " << body->y << ")\n";
        ss << "  Agent Orientation: " << body->orientation << "\n";
        ss << "  Agent Energy: " << body->energy << "\n";
        ss << "  Agent Health: " << body->health << "\n";
        ss << "  Agent Age: " << body->age;
        
        return ss.str();
    }, py::arg("world"), "Get statistics about a world");
    
    // Utility functions for common operations
    m.def("createSimpleSimulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);
        world->setAgentStart(50.0f, 50.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a simple simulation setup with default configuration");
    
    // Enhanced type safety and memory management utilities
    m.def("safeBrainAccess", [](std::shared_ptr<Brain> brain, const std::string& operation) {
        if (!brain) throw std::runtime_error("Brain is null");
        return operation + " operation on brain succeeded";
    }, py::arg("brain"), py::arg("operation"),
        "Safely access brain with proper error checking");
    
    // Development and debugging utilities
    m.def("brainStatusReport", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::runtime_error("Brain is null");
        brain->logStatus();
        auto stats = getBrainStats(brain);
        return "Brain Status Report:\n" + stats;
    }, py::arg("brain"), "Generate a comprehensive brain status report");
    
    m.def("simulateUntilStable", [](std::shared_ptr<Brain> brain, int maxSteps = 100, 
                                   float stabilityThreshold = 0.95f) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        brain->initialize();
        float currentFiringRate = 0.0f;
        
        for (nlm::SimulationStep step = 0; step < maxSteps; ++step) {
            brain->step(step);
            currentFiringRate = brain->getAverageFiringRate();
            
            if (currentFiringRate > stabilityThreshold) {
                return std::make_pair(step, currentFiringRate);
            }
        }
        
        return std::make_pair(maxSteps, currentFiringRate);
    }, py::arg("brain"), py::arg("maxSteps") = 100, py::arg("stabilityThreshold") = 0.95f,
        "Simulate until firing rate stabilizes or max steps reached");
    
    // Add special constants for easier use
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
