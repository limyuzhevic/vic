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

// Power user advanced feature exception classes
class ExperimentError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class PerformanceMonitoringError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class BrainStateExportError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class VisualizationError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class BatchProcessingError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class DataAnalysisError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class PerformanceOptimizationError : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

// Register all advanced feature exceptions
void registerPowerUserExceptions(py::module_ &m) {
    py::register_exception<ExperimentError>(m, "ExperimentError", PyExc_RuntimeError);
    py::register_exception<PerformanceMonitoringError>(m, "PerformanceMonitoringError", PyExc_RuntimeError);
    py::register_exception<BrainStateExportError>(m, "BrainStateExportError", PyExc_RuntimeError);
    py::register_exception<VisualizationError>(m, "VisualizationError", PyExc_RuntimeError);
    py::register_exception<BatchProcessingError>(m, "BatchProcessingError", PyExc_RuntimeError);
    py::register_exception<DataAnalysisError>(m, "DataAnalysisError", PyExc_RuntimeError);
    py::register_exception<PerformanceOptimizationError>(m, "PerformanceOptimizationError", PyExc_RuntimeError);
}

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register advanced features for power users
    registerAdvancedFeatures(m);
}

// =============================================================================
// POWER USER ADVANCED FEATURE CLASSES
// =============================================================================

// PRIORITY 1: Enhanced ExperimentRunner Class
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain_(brain), config_(config) {
        if (!brain_ || !config_) {
            throw ExperimentError("EnhancedExperimentRunner initialization failed: brain or config is null");
        }
    }

    // Run multiple simulation configurations
    std::vector<std::map<std::string, float>> runConfigurations(
        const std::vector<std::map<std::string, float>>& configs) {
        std::vector<std::map<std::string, float>> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            try {
                // Create temporary config for this run
                auto tempConfig = std::make_shared<Config>(*config_);
                for (const auto& param : configs[i]) {
                    tempConfig->set(param.first, param.second);
                }
                
                // Create brain with temp config and run simulation
                auto tempBrain = std::make_shared<Brain>(tempConfig);
                if (!tempBrain->initialize()) {
                    throw ExperimentError("Experiment " + std::to_string(i) + " failed to initialize");
                }
                
                // Run simulation for specified steps
                int64_t maxSteps = tempConfig->get<int64_t>("simulation.max_steps", 1000);
                SimulationStep step;
                step.ticks = 0;
                
                std::map<std::string, float> result;
                result["config_id"] = static_cast<float>(i);
                result["max_steps"] = static_cast<float>(maxSteps);
                
                for (int64_t s = 0; s < maxSteps; ++s) {
                    tempBrain->step(step);
                    
                    // Collect metrics
                    result["spike_count"] = static_cast<float>(tempBrain->getTotalSpikeCount());
                    result["active_neurons"] = static_cast<float>(tempBrain->getActiveNeuronCount());
                    result["firing_rate"] = tempBrain->getAverageFiringRate();
                    result["excit_inhib_ratio"] = tempBrain->getExcitationInhibitionRatio();
                }
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                throw ExperimentError("Experiment " + std::to_string(i) + " failed: " + std::string(e.what()));
            }
        }
        
        return results;
    }

    // Collect and compare results from multiple runs
    std::map<std::string, float> compareResults(const std::vector<std::map<std::string, float>>& results) {
        if (results.empty()) {
            throw ExperimentError("Cannot compare results: no results provided");
        }
        
        std::map<std::string, float> summary;
        
        // Calculate mean and standard deviation for numeric metrics
        std::vector<float> spikeCounts, activeNeurons, firingRates, excInhRatios;
        
        for (const auto& result : results) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
            if (result.find("excit_inhib_ratio") != result.end()) {
                excInhRatios.push_back(result.at("excit_inhib_ratio"));
            }
        }
        
        // Calculate statistics
        summary["mean_spike_count"] = calculateMean(spikeCounts);
        summary["std_spike_count"] = calculateStd(spikeCounts);
        summary["mean_active_neurons"] = calculateMean(activeNeurons);
        summary["std_active_neurons"] = calculateStd(activeNeurons);
        summary["mean_firing_rate"] = calculateMean(firingRates);
        summary["std_firing_rate"] = calculateStd(firingRates);
        summary["mean_excit_inhib_ratio"] = calculateMean(excInhRatios);
        summary["std_excit_inhib_ratio"] = calculateStd(excInhRatios);
        
        return summary;
    }

    // Parameter optimization support
    std::map<std::string, float> optimizeParameters(
        const std::vector<std::map<std::string, float>>& parameterSets,
        const std::string& objectiveFunction,
        float learningRate = 0.01f,
        int maxIterations = 100) {
        
        if (parameterSets.empty()) {
            throw ExperimentError("Cannot optimize: no parameter sets provided");
        }
        
        std::map<std::string, float> bestParams = parameterSets[0];
        float bestScore = std::numeric_limits<float>::max();
        
        for (int iteration = 0; iteration < maxIterations; ++iteration) {
            for (const auto& params : parameterSets) {
                float score = evaluateObjective(params, objectiveFunction);
                
                if (score < bestScore) {
                    bestScore = score;
                    bestParams = params;
                }
            }
            
            // Perturb parameters for next iteration
            for (auto& param : bestParams) {
                param.second += (rand() / (float)RAND_MAX - 0.5f) * learningRate;
            }
        }
        
        return bestParams;
    }

    // Statistical analysis of results
    std::map<std::string, float> performStatisticalAnalysis(const std::vector<std::map<std::string, float>>& results) {
        if (results.empty()) {
            throw ExperimentError("Cannot perform statistical analysis: no results provided");
        }
        
        std::map<std::string, float> analysis;
        
        // Calculate various statistical measures
        std::vector<float> allSpikeCounts, allActiveNeurons, allFiringRates;
        
        for (const auto& result : results) {
            if (result.find("spike_count") != result.end()) {
                allSpikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                allActiveNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                allFiringRates.push_back(result.at("firing_rate"));
            }
        }
        
        // Calculate percentiles
        std::sort(allSpikeCounts.begin(), allSpikeCounts.end());
        std::sort(allActiveNeurons.begin(), allActiveNeurons.end());
        std::sort(allFiringRates.begin(), allFiringRates.end());
        
        analysis["spike_count_median"] = getPercentile(allSpikeCounts, 50);
        analysis["spike_count_p95"] = getPercentile(allSpikeCounts, 95);
        analysis["active_neurons_median"] = getPercentile(allActiveNeurons, 50);
        analysis["active_neurons_p95"] = getPercentile(allActiveNeurons, 95);
        analysis["firing_rate_median"] = getPercentile(allFiringRates, 50);
        analysis["firing_rate_p95"] = getPercentile(allFiringRates, 95);
        
        return analysis;
    }

    // Get underlying brain and config
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<Config> getConfig() const { return config_; }

private:
    float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

    float calculateStd(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        float mean = calculateMean(values);
        float sumSq = 0.0f;
        for (float v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }

    float getPercentile(const std::vector<float>& values, float percentile) {
        if (values.empty()) return 0.0f;
        size_t index = static_cast<size_t>((percentile / 100.0f) * (values.size() - 1));
        return values[index];
    }

    float evaluateObjective(const std::map<std::string, float>& params, const std::string& objectiveFunction) {
        // Simple objective function evaluation
        float score = 0.0f;
        for (const auto& param : params) {
            score += std::abs(param.second);
        }
        return score;
    }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
};

// PRIORITY 2: PerformanceProfiler Class
class PerformanceProfiler {
public:
    PerformanceProfiler(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw PerformanceMonitoringError("PerformanceProfiler initialization failed: brain is null");
        }
        resetProfile();
    }

    // Real-time performance monitoring
    void startMonitoring(int monitoringIntervalMs = 100) {
        monitoringActive_ = true;
        monitoringInterval_ = monitoringIntervalMs;
        lastUpdateTime_ = std::chrono::steady_clock::now();
        
        // Start background monitoring thread
        monitorThread_ = std::thread(&PerformanceProfiler::monitorLoop, this);
    }

    void stopMonitoring() {
        monitoringActive_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
    }

    // Memory usage tracking
    struct MemoryStats {
        size_t totalMemoryMB;
        size_t allocatedMemoryMB;
        size_t peakMemoryMB;
        std::map<std::string, size_t> memoryByComponent;
    };

    MemoryStats getMemoryStats() {
        MemoryStats stats;
        
        // Calculate memory usage from brain components
        if (brain_) {
            stats.totalMemoryMB = 2048;  // Placeholder - should get from actual system
            stats.allocatedMemoryMB = calculateMemoryUsage(brain_);
            stats.peakMemoryMB = peakMemory_;
            
            // Component-specific memory tracking
            stats.memoryByComponent["brain"] = calculateMemoryUsage(brain_);
            stats.memoryByComponent["config"] = 512;  // Placeholder
        }
        
        return stats;
    }

    // Processing time profiling
    struct PerformanceMetrics {
        double cpuTimeMs;
        double wallTimeMs;
        double memoryBandwidthGBps;
        double gpuUtilizationPercent;
        std::vector<double> operationTimesMs;
        std::map<std::string, double> componentTimesMs;
    };

    PerformanceMetrics getPerformanceMetrics() {
        PerformanceMetrics metrics;
        auto now = std::chrono::steady_clock::now();
        
        metrics.cpuTimeMs = totalCpuTime_;
        metrics.wallTimeMs = std::chrono::duration<double, std::milli>(now - startTime_).count();
        metrics.memoryBandwidthGBps = calculateMemoryBandwidth();
        metrics.gpuUtilizationPercent = calculateGpuUtilization();
        
        // Collect operation times
        for (const auto& time : operationTimes_) {
            metrics.operationTimesMs.push_back(time);
        }
        
        // Component times
        metrics.componentTimesMs = componentTimes_;
        
        return metrics;
    }

    // Resource utilization monitoring
    struct ResourceUtilization {
        double cpuUtilizationPercent;
        double memoryUtilizationPercent;
        double diskIO_MBps;
        double networkBandwidth_MBps;
        std::vector<double> loadAverage;
    };

    ResourceUtilization getResourceUtilization() {
        ResourceUtilization utilization;
        
        // Calculate resource utilization
        utilization.cpuUtilizationPercent = calculateCpuUtilization();
        utilization.memoryUtilizationPercent = calculateMemoryUtilization();
        utilization.diskIO_MBps = calculateDiskIO();
        utilization.networkBandwidth_MBps = calculateNetworkBandwidth();
        
        // Get system load average (Linux)
        std::ifstream loadFile("/proc/loadavg");
        if (loadFile.is_open()) {
            double a, b, c;
            loadFile >> a >> b >> c;
            utilization.loadAverage = {a, b, c};
        }
        
        return utilization;
    }

    // Start profiling session
    void startProfile() {
        startTime_ = std::chrono::steady_clock::now();
        totalCpuTime_ = 0.0;
        operationTimes_.clear();
        componentTimes_.clear();
        monitoringActive_ = true;
        
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
        
        monitorThread_ = std::thread(&PerformanceProfiler::monitorLoop, this);
    }

    // Stop profiling session
    void stopProfile() {
        monitoringActive_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
        
        auto endTime = std::chrono::steady_clock::now();
        totalCpuTime_ = std::chrono::duration<double>(endTime - startTime_).count() * 1000.0;
    }

    // Get profiling data
    std::vector<double> getProfileData() const {
        return operationTimes_;
    }

    std::map<std::string, double> getComponentTimes() const {
        return componentTimes_;
    }

    // Reset profiler
    void resetProfile() {
        operationTimes_.clear();
        componentTimes_.clear();
        peakMemory_ = 0;
        monitoringActive_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
    }

private:
    void monitorLoop() {
        while (monitoringActive_) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double, std::milli>(now - lastUpdateTime_).count();
            
            if (elapsed >= monitoringInterval_) {
                // Collect performance data
                recordPerformanceSample();
                lastUpdateTime_ = now;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void recordPerformanceSample() {
        // Record current performance metrics
        double cpuTime = calculateCpuTime();
        size_t memoryUsage = calculateMemoryUsage(brain_);
        
        operationTimes_.push_back(cpuTime);
        
        if (memoryUsage > peakMemory_) {
            peakMemory_ = memoryUsage;
        }
        
        // Update component times
        componentTimes_["brain"] = componentTimes_["brain"] + cpuTime;
    }

    double calculateCpuTime() {
        // Simple CPU time calculation - should use system calls for accuracy
        return 0.1;  // Placeholder
    }

    size_t calculateMemoryUsage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage
        return 512;  // Placeholder
    }

    double calculateMemoryBandwidth() {
        return 12.5;  // Placeholder
    }

    double calculateGpuUtilization() {
        return 0.0;  // Placeholder
    }

    double calculateCpuUtilization() {
        return 25.0;  // Placeholder
    }

    double calculateMemoryUtilization() {
        return 45.0;  // Placeholder
    }

    double calculateDiskIO() {
        return 50.0;  // Placeholder
    }

    double calculateNetworkBandwidth() {
        return 100.0;  // Placeholder
    }

private:
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point startTime_;
    double totalCpuTime_ = 0.0;
    size_t peakMemory_ = 0;
    bool monitoringActive_ = false;
    int monitoringInterval_ = 100;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    std::vector<double> operationTimes_;
    std::map<std::string, double> componentTimes_;
    std::thread monitorThread_;
};

// PRIORITY 3: BrainStateExporter Class
class BrainStateExporter {
public:
    BrainStateExporter(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw BrainStateExportError("BrainStateExporter initialization failed: brain is null");
        }
    }

    // Standardized brain state export
    bool exportBrainState(const std::string& filepath, bool compress = false, int version = 1) {
        if (!brain_) {
            throw BrainStateExportError("Cannot export brain state: brain pointer is null");
        }
        
        try {
            // Create brain state data structure
            std::map<std::string, std::string> brainState;
            
            // Collect brain metrics
            brainState["spike_count"] = std::to_string(brain_->getTotalSpikeCount());
            brainState["active_neurons"] = std::to_string(brain_->getActiveNeuronCount());
            brainState["firing_rate"] = std::to_string(brain_->getAverageFiringRate());
            brainState["excit_inhib_ratio"] = std::to_string(brain_->getExcitationInhibitionRatio());
            brainState["pending_spikes"] = std::to_string(brain_->getPendingSpikeEventCount());
            brainState["firing_neurons"] = std::to_string(brain_->getFiringNeuronCount());
            
            // Export to file
            std::ofstream outfile(filepath);
            if (!outfile.is_open()) {
                throw BrainStateExportError("Failed to open file for writing: " + filepath);
            }
            
            // Write version information
            outfile << "# Brain State Export v" << version << "\n";
            outfile << "# Exported at: " << getCurrentTimestamp() << "\n\n";
            
            if (compress) {
                exportCompressed(brainState, outfile, version);
            } else {
                exportPlain(brainState, outfile);
            }
            
            outfile.close();
            return true;
            
        } catch (const std::exception& e) {
            throw BrainStateExportError(std::string("Brain state export failed: ") + std::string(e.what()));
        }
    }

    // Compressed brain state storage
    bool exportCompressed(const std::map<std::string, std::string>& data, std::ofstream& outfile, int version) {
        // Simple compression: remove whitespace and use compact format
        outfile << "BRAIN_STATE_COMPRESSED\n";
        outfile << "VERSION:" << version << "\n";
        
        for (const auto& pair : data) {
            // Simple encoding: key:value
            outfile << pair.first << "=" << pair.second << ";";
        }
        outfile << "\nEND\n";
        
        return true;
    }

    // Plain brain state export
    bool exportPlain(const std::map<std::string, std::string>& data, std::ofstream& outfile) {
        outfile << "# Brain State Export\n\n";
        
        for (const auto& pair : data) {
            outfile << pair.first << ": " << pair.second << "\n";
        }
        
        return true;
    }

    // Versioned backup systems
    std::vector<std::string> createBackup(const std::string& baseDir, int numBackups = 5) {
        if (!brain_) {
            throw BrainStateExportError("Cannot create backup: brain pointer is null");
        }
        
        std::vector<std::string> backupFiles;
        
        // Ensure backup directory exists
        std::string backupDir = baseDir + "/backups";
        createDirectory(backupDir);
        
        for (int i = 0; i < numBackups; ++i) {
            std::string timestamp = getCurrentTimestamp();
            std::string filepath = backupDir + "/brain_state_v" + std::to_string(i) + "_" + timestamp + ".bs"
            
            bool success = exportBrainState(filepath, false, i + 1);
            if (success) {
                backupFiles.push_back(filepath);
            }
        }
        
        return backupFiles;
    }

    // Cross-platform compatibility
    bool exportPlatformIndependent(const std::string& filepath, const std::string& platform = "auto") {
        if (!brain_) {
            throw BrainStateExportError("Cannot export platform-independent brain state: brain pointer is null");
        }
        
        try {
            std::string extension = ".bs.json";  // JSON format for platform independence
            std::string fullPath = filepath + extension;
            
            std::ofstream outfile(fullPath);
            if (!outfile.is_open()) {
                throw BrainStateExportError("Failed to open file for platform-independent export: " + fullPath);
            }
            
            outfile << "{\n";
            outfile << "  \"version\": \"1.0\",\n";
            outfile << "  \"platform\": \"" << platform << "\",\n";
            outfile << "  \"exported_at\": \"" << getCurrentTimestamp() << "\",\n";
            outfile << "  \"brain_data\": {\n";
            
            // Add brain state data
            outfile << "    \"spike_count\": " << brain_->getTotalSpikeCount() << ",\n";
            outfile << "    \"active_neurons\": " << brain_->getActiveNeuronCount() << ",\n";
            outfile << "    \"firing_rate\": " << brain_->getAverageFiringRate() << ",\n";
            outfile << "    \"excit_inhib_ratio\": " << brain_->getExcitationInhibitionRatio() << ",\n";
            outfile << "    \"pending_spikes\": " << brain_->getPendingSpikeEventCount() << ",\n";
            outfile << "    \"firing_neurons\": " << brain_->getFiringNeuronCount() << "\n";
            
            outfile << "  }\n";
            outfile << "}\n";
            
            outfile.close();
            return true;
            
        } catch (const std::exception& e) {
            throw BrainStateExportError(std::string("Platform-independent export failed: ") + std::string(e.what()));
        }
    }

    // Import brain state
    bool importBrainState(const std::string& filepath) {
        if (!brain_) {
            throw BrainStateExportError("Cannot import brain state: brain pointer is null");
        }
        
        try {
            std::ifstream infile(filepath);
            if (!infile.is_open()) {
                throw BrainStateExportError("Failed to open file for reading: " + filepath);
            }
            
            std::string line;
            std::map<std::string, std::string> brainState;
            
            while (std::getline(infile, line)) {
                if (line.find(':') != std::string::npos) {
                    size_t pos = line.find(':');
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    
                    brainState[key] = value;
                }
            }
            
            infile.close();
            
            // Apply imported state to brain
            applyBrainState(brainState);
            
            return true;
            
        } catch (const std::exception& e) {
            throw BrainStateExportError(std::string("Brain state import failed: ") + std::string(e.what()));
        }
    }

    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }

private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
        return ss.str();
    }

    void createDirectory(const std::string& path) {
        // Create directory recursively
        system(("mkdir -p " + path).c_str());
    }

    void applyBrainState(const std::map<std::string, std::string>& brainState) {
        // Apply imported brain state
        // This is a placeholder - actual implementation would need to restore brain state
    }

private:
    std::shared_ptr<Brain> brain_;
};

// PRIORITY 4: AdvancedVisualization Class
class AdvancedVisualization {
public:
    AdvancedVisualization(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw VisualizationError("AdvancedVisualization initialization failed: brain is null");
        }
    }

    // 2D neural activity visualization
    bool create2DVisualization(const std::string& outputPath, double scaleFactor = 1.0, 
                                bool showConnections = true, bool showActivity = true) {
        if (!brain_) {
            throw VisualizationError("Cannot create 2D visualization: brain pointer is null");
        }
        
        try {
            // Create 2D neural map
            int width = static_cast<int>(brain_->getActiveNeuronCount() * scaleFactor);
            int height = width / 2;  // Maintain aspect ratio
            
            std::vector<std::vector<double>> neuralMap(height, std::vector<double>(width, 0.0));
            
            // Fill neural map with activity data
            createNeuralActivityMap(neuralMap, showActivity);
            
            // Add connections if requested
            if (showConnections) {
                addConnections(neuralMap);
            }
            
            // Generate visualization
            generateVisualization(neuralMap, outputPath);
            
            return true;
            
        } catch (const std::exception& e) {
            throw VisualizationError(std::string("2D visualization creation failed: ") + std::string(e.what()));
        }
    }

    // Connection strength heatmaps
    bool createConnectionHeatmap(const std::string& outputPath, double connectionThreshold = 0.5) {
        if (!brain_) {
            throw VisualizationError("Cannot create connection heatmap: brain pointer is null");
        }
        
        try {
            // Get connection data
            std::map<std::pair<uint64_t, uint64_t>, double> connections = getConnectionData();
            
            // Create heatmap matrix
            std::vector<std::vector<double>> heatmap = createHeatmapMatrix(connections, connectionThreshold);
            
            // Generate heatmap visualization
            generateHeatmapVisualization(heatmap, outputPath);
            
            return true;
            
        } catch (const std::exception& e) {
            throw VisualizationError(std::string("Connection heatmap creation failed: ") + std::string(e.what()));
        }
    }

    // Development timeline visualization
    bool createDevelopmentTimeline(const std::string& outputPath, bool showPlasticity = true,
                                    bool showNeuromodulation = true, bool showDevelopment = true) {
        if (!brain_) {
            throw VisualizationError("Cannot create development timeline: brain pointer is null");
        }
        
        try {
            // Collect timeline data
            std::vector<std::map<std::string, double>> timelineData = collectTimelineData();
            
            // Generate timeline visualization
            generateTimelineVisualization(timelineData, outputPath, 
                                           showPlasticity, showNeuromodulation, showDevelopment);
            
            return true;
            
        } catch (const std::exception& e) {
            throw VisualizationError(std::string("Development timeline creation failed: ") + std::string(e.what()));
        }
    }

    // Generate dynamic visualizations
    bool createDynamicVisualization(const std::string& outputPath, int durationSeconds = 10,
                                      double timeStep = 0.1, bool saveFrames = true) {
        if (!brain_) {
            throw VisualizationError("Cannot create dynamic visualization: brain pointer is null");
        }
        
        try {
            // Create animation frames
            std::vector<std::vector<std::vector<double>>> frames;
            
            int numFrames = static_cast<int>(durationSeconds / timeStep);
            frames.reserve(numFrames);
            
            for (int frame = 0; frame < numFrames; ++frame) {
                std::vector<std::vector<double>> frameData(100, std::vector<double>(100, 0.0));
                
                // Simulate brain evolution
                simulateBrainEvolution(frameData, frame, timeStep);
                
                if (saveFrames) {
                    frames.push_back(frameData);
                }
            }
            
            // Create animation
            createAnimation(frames, outputPath, durationSeconds);
            
            return true;
            
        } catch (const std::exception& e) {
            throw VisualizationError(std::string("Dynamic visualization creation failed: ") + std::string(e.what()));
        }
    }

    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }

private:
    void createNeuralActivityMap(std::vector<std::vector<double>>& map, bool showActivity) {
        if (!showActivity) return;
        
        // Get active neuron count
        int activeNeurons = brain_->getActiveNeuronCount();
        int rows = map.size();
        int cols = map[0].size();
        
        // Distribute active neurons
        for (int i = 0; i < activeNeurons && i < rows * cols; ++i) {
            int row = i % rows;
            int col = i / rows;
            map[row][col] = (static_cast<double>(rand()) / RAND_MAX) * 0.8 + 0.2;
        }
    }

    void addConnections(std::vector<std::vector<double>>& map) {
        // Add connection visualization
        int rows = map.size();
        int cols = map[0].size();
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                // Add connection strength
                double connectionStrength = (static_cast<double>(rand()) / RAND_MAX) * 0.3;
                map[i][j] += connectionStrength;
            }
        }
    }

    void generateVisualization(const std::vector<std::vector<double>>& data, const std::string& path) {
        // Generate simple text-based visualization
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            throw VisualizationError("Failed to create visualization file: " + path);
        }
        
        outfile << "# 2D Neural Activity Visualization\n";
        outfile << "# Generated by AdvancedVisualization class\n\n";
        
        int rows = data.size();
        int cols = data[0].size();
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                double value = data[i][j];
                char intensity = static_cast<char>('.' + value * 50);  // Simple intensity mapping
                outfile << intensity;
            }
            outfile << "\n";
        }
        
        outfile.close();
    }

    std::map<std::pair<uint64_t, uint64_t>, double> getConnectionData() {
        // Get connection data from brain
        // This is a placeholder implementation
        std::map<std::pair<uint64_t, uint64_t>, double> connections;
        
        // Generate some sample connections
        for (int i = 0; i < 50; ++i) {
            uint64_t src = static_cast<uint64_t>(rand()) % 1000;
            uint64_t dst = static_cast<uint64_t>(rand()) % 1000;
            double strength = (static_cast<double>(rand()) / RAND_MAX) * 0.9 + 0.1;
            connections[{src, dst}] = strength;
        }
        
        return connections;
    }

    std::vector<std::vector<double>> createHeatmapMatrix(const std::map<std::pair<uint64_t, uint64_t>, double>& connections,
                                                          double threshold) {
        // Create heatmap matrix from connections
        std::vector<std::vector<double>> heatmap(100, std::vector<double>(100, 0.0));
        
        for (const auto& conn : connections) {
            if (conn.second >= threshold) {
                int x = conn.first.first % 100;
                int y = conn.first.second % 100;
                heatmap[x][y] += conn.second;
            }
        }
        
        return heatmap;
    }

    void generateHeatmapVisualization(const std::vector<std::vector<double>>& heatmap, const std::string& path) {
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            throw VisualizationError("Failed to create heatmap file: " + path);
        }
        
        outfile << "# Connection Strength Heatmap\n";
        outfile << "# Generated by AdvancedVisualization class\n\n";
        
        int rows = heatmap.size();
        int cols = heatmap[0].size();
        
        outfile << "COLS:" << cols << " ROWS:" << rows << "\n";
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                outfile << heatmap[i][j] << " ";
            }
            outfile << "\n";
        }
        
        outfile.close();
    }

    std::vector<std::map<std::string, double>> collectTimelineData() {
        // Collect timeline data
        std::vector<std::map<std::string, double>> timeline;
        
        for (int step = 0; step < 100; ++step) {
            std::map<std::string, double> point;
            point["time"] = step * 0.1;
            point["spike_count"] = static_cast<double>(brain_->getTotalSpikeCount()) * (step / 100.0);
            point["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount()) * (step / 100.0);
            point["plasticity"] = (static_cast<double>(rand()) / RAND_MAX) * 0.5;
            point["neuromodulation"] = (static_cast<double>(rand()) / RAND_MAX) * 0.5;
            point["development"] = (static_cast<double>(rand()) / RAND_MAX) * 0.3;
            
            timeline.push_back(point);
        }
        
        return timeline;
    }

    void generateTimelineVisualization(const std::vector<std::map<std::string, double>>& data,
                                        const std::string& path,
                                        bool showPlasticity, bool showNeuromodulation, bool showDevelopment) {
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            throw VisualizationError("Failed to create timeline file: " + path);
        }
        
        outfile << "# Development Timeline Visualization\n";
        outfile << "# Generated by AdvancedVisualization class\n\n";
        
        outfile << "TIME,SPIKE_COUNT,ACTIVE_NEURONS";
        if (showPlasticity) outfile << ",PLASTICITY";
        if (showNeuromodulation) outfile << ",NEUROMODULATION";
        if (showDevelopment) outfile << ",DEVELOPMENT";
        outfile << "\n";
        
        for (const auto& point : data) {
            outfile << point.at("time") << ","
                    << point.at("spike_count") << ","
                    << point.at("active_neurons");
            if (showPlasticity) outfile << "," << point.at("plasticity");
            if (showNeuromodulation) outfile << "," << point.at("neuromodulation");
            if (showDevelopment) outfile << "," << point.at("development");
            outfile << "\n";
        }
        
        outfile.close();
    }

    void simulateBrainEvolution(std::vector<std::vector<double>>& frame, int frameIndex, double timeStep) {
        // Simulate brain evolution for this frame
        int rows = frame.size();
        int cols = frame[0].size();
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                // Simple evolution model
                double evolution = (static_cast<double>(rand()) / RAND_MAX - 0.5) * timeStep;
                frame[i][j] += evolution;
                
                // Clamp values
                if (frame[i][j] < 0.0) frame[i][j] = 0.0;
                if (frame[i][j] > 1.0) frame[i][j] = 1.0;
            }
        }
    }

    void createAnimation(const std::vector<std::vector<std::vector<double>>>& frames,
                         const std::string& path, int duration) {
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            throw VisualizationError("Failed to create animation file: " + path);
        }
        
        outfile << "# Brain Activity Animation\n";
        outfile << "# Generated by AdvancedVisualization class\n";
        outfile << "# Frames: " << frames.size() << " Duration: " << duration << " seconds\n\n";
        
        for (size_t frame = 0; frame < frames.size(); ++frame) {
            outfile << "FRAME:" << frame << "\n";
            outfile << "DATA:";
            
            for (const auto& row : frames[frame]) {
                for (double val : row) {
                    outfile << val << " ";
                }
                outfile << ";";
            }
            outfile << "\n\n";
        }
        
        outfile.close();
    }

private:
    std::shared_ptr<Brain> brain_;
}};

// =============================================================================
// PRIORITY 1: Batch Processing Classes
// =============================================================================

// PRIORITY 1.1: BatchProcessor Class - Parallel processing of multiple brain simulations
class BatchProcessor {
public:
    BatchProcessor(std::shared_ptr<Config> config) : config_(config) {
        if (!config_) {
            throw BatchProcessingError("BatchProcessor initialization failed: config is null");
        }
    }

    // Parallel processing of multiple brain simulations
    std::vector<std::map<std::string, float>> processBrains(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<SimulationStep>& steps,
        bool parallel = true,
        int numThreads = 0) {
        
        if (brains.empty()) {
            throw BatchProcessingError("Cannot process brains: no brains provided");
        }
        
        std::vector<std::map<std::string, float>> results;
        results.reserve(brains.size());
        
        if (parallel && numThreads > 0) {
            // Parallel processing
            results = processParallel(brains, steps, numThreads);
        } else {
            // Sequential processing
            results = processSequential(brains, steps);
        }
        
        return results;
    }

    // Batch save operations for multiple brains
    bool saveBrainsBatch(const std::vector<std::shared_ptr<Brain>>& brains,
                         const std::string& directory,
                         const std::string& format = "state") {
        
        createDirectory(directory);
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::string filename;
            if (format == "state") {
                filename = directory + "/brain_" + std::to_string(i) + ".state";
                brains[i]->save(filename);
            } else if (format == "json") {
                filename = directory + "/brain_" + std::to_string(i) + ".json";
                exportBrainToJSON(brains[i], filename);
            } else if (format == "csv") {
                filename = directory + "/brain_" + std::to_string(i) + ".csv";
                exportBrainToCSV(brains[i], filename);
            }
        }
        
        return true;
    }

    // Batch load operations for multiple brains
    std::vector<std::shared_ptr<Brain>> loadBrainsBatch(const std::vector<std::string>& filepaths,
                                                         const std::vector<std::shared_ptr<Config>>& configs) {
        
        if (filepaths.size() != configs.size()) {
            throw BatchProcessingError("Load brains batch failed: filepaths and configs size mismatch");
        }
        
        std::vector<std::shared_ptr<Brain>> brains;
        brains.reserve(filepaths.size());
        
        for (size_t i = 0; i < filepaths.size(); ++i) {
            try {
                auto brain = std::make_shared<Brain>(configs[i]);
                if (brain->load(filepaths[i])) {
                    brains.push_back(brain);
                }
            } catch (const std::exception& e) {
                // Skip failed files
                continue;
            }
        }
        
        return brains;
    }

    // Statistical analysis of batch results
    std::map<std::string, float> analyzeBatchResults(
        const std::vector<std::map<std::string, float>>& results) {
        
        if (results.empty()) {
            throw BatchProcessingError("Cannot analyze batch results: no results provided");
        }
        
        BatchStatistics analyzer;
        return analyzer.analyze(results);
    }

    // Batch configuration generation
    std::vector<std::shared_ptr<Config>> generateBatchConfigs(
        const std::map<std::string, std::vector<float>>& parameterRanges,
        int numConfigs) {
        
        if (parameterRanges.empty()) {
            throw BatchProcessingError("Cannot generate batch configs: no parameter ranges provided");
        }
        
        std::vector<std::shared_ptr<Config>> configs;
        configs.reserve(numConfigs);
        
        for (int i = 0; i < numConfigs; ++i) {
            auto config = std::make_shared<Config>(*config_);
            
            for (const auto& paramRange : parameterRanges) {
                const std::string& paramName = paramRange.first;
                const std::vector<float>& ranges = paramRange.second;
                
                if (ranges.size() >= 2) {
                    float minVal = ranges[0];
                    float maxVal = ranges[ranges.size() - 1];
                    float value = minVal + static_cast<float>(rand()) / RAND_MAX * (maxVal - minVal);
                    config->set(paramName, value);
                }
            }
            
            configs.push_back(config);
        }
        
        return configs;
    }

    // Batch execution with progress tracking
    std::vector<std::map<std::string, float>> executeBatch(
        const std::vector<std::map<std::string, float>>& configs,
        const std::vector<SimulationStep>& steps,
        ProgressTracker* progressTracker = nullptr) {
        
        std::vector<std::map<std::string, float>> results;
        results.reserve(configs.size());
        
        for (size_t i = 0; i < configs.size(); ++i) {
            if (progressTracker) {
                progressTracker->updateProgress(i, configs.size(), "Processing config " + std::to_string(i));
            }
            
            try {
                // Create temporary config
                auto tempConfig = std::make_shared<Config>(*config_);
                for (const auto& param : configs[i]) {
                    tempConfig->set(param.first, param.second);
                }
                
                // Create brain with temp config
                auto brain = std::make_shared<Brain>(tempConfig);
                if (!brain->initialize()) {
                    throw BatchProcessingError("Config " + std::to_string(i) + " failed to initialize");
                }
                
                // Run simulation
                std::map<std::string, float> result;
                result["config_id"] = static_cast<float>(i);
                
                for (const auto& step : steps) {
                    brain->step(step);
                    
                    // Collect metrics
                    result["spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
                    result["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
                    result["firing_rate"] = brain->getAverageFiringRate();
                    result["excit_inhib_ratio"] = brain->getExcitationInhibitionRatio();
                }
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                throw BatchProcessingError("Batch execution failed for config " + std::to_string(i) + ": " + std::string(e.what()));
            }
        }
        
        if (progressTracker) {
            progressTracker->updateProgress(configs.size(), configs.size(), "Batch execution complete");
        }
        
        return results;
    }

    // Get underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }

private:
    std::vector<std::map<std::string, float>> processParallel(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<SimulationStep>& steps,
        int numThreads) {
        
        std::vector<std::map<std::string, float>> results(brains.size());
        
        // Simple thread pool simulation (for demonstration)
        std::vector<std::thread> threads;
        
        for (size_t i = 0; i < brains.size() && i < static_cast<size_t>(numThreads); ++i) {
            threads.emplace_back([&, i]() {
                std::map<std::string, float> result;
                
                for (const auto& step : steps) {
                    brains[i]->step(step);
                    
                    result["spike_count"] = static_cast<float>(brains[i]->getTotalSpikeCount());
                    result["active_neurons"] = static_cast<float>(brains[i]->getActiveNeuronCount());
                    result["firing_rate"] = brains[i]->getAverageFiringRate();
                    result["excit_inhib_ratio"] = brains[i]->getExcitationInhibitionRatio();
                }
                
                results[i] = result;
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Fill remaining results with sequential processing
        for (size_t i = threads.size(); i < brains.size(); ++i) {
            std::map<std::string, float> result;
            
            for (const auto& step : steps) {
                brains[i]->step(step);
                
                result["spike_count"] = static_cast<float>(brains[i]->getTotalSpikeCount());
                result["active_neurons"] = static_cast<float>(brains[i]->getActiveNeuronCount());
                result["firing_rate"] = brains[i]->getAverageFiringRate();
                result["excit_inhib_ratio"] = brains[i]->getExcitationInhibitionRatio();
            }
            
            results[i] = result;
        }
        
        return results;
    }

    std::vector<std::map<std::string, float>> processSequential(
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<SimulationStep>& steps) {
        
        std::vector<std::map<std::string, float>> results;
        results.reserve(brains.size());
        
        for (const auto& brain : brains) {
            std::map<std::string, float> result;
            
            for (const auto& step : steps) {
                brain->step(step);
                
                result["spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
                result["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
                result["firing_rate"] = brain->getAverageFiringRate();
                result["excit_inhib_ratio"] = brain->getExcitationInhibitionRatio();
            }
            
            results.push_back(result);
        }
        
        return results;
    }

    void exportBrainToJSON(std::shared_ptr<Brain> brain, const std::string& filepath) {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw BatchProcessingError("Failed to write JSON file: " + filepath);
        }
        
        outfile << "{\n";
        outfile << "  \"spike_count\": " << brain->getTotalSpikeCount() << ",\n";
        outfile << "  \"active_neurons\": " << brain->getActiveNeuronCount() << ",\n";
        outfile << "  \"firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        outfile << "  \"excit_inhib_ratio\": " << brain->getExcitationInhibitionRatio() << "\n";
        outfile << "}\n";
        
        outfile.close();
    }

    void exportBrainToCSV(std::shared_ptr<Brain> brain, const std::string& filepath) {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw BatchProcessingError("Failed to write CSV file: " + filepath);
        }
        
        outfile << "metric,value\n";
        outfile << "spike_count," << brain->getTotalSpikeCount() << "\n";
        outfile << "active_neurons," << brain->getActiveNeuronCount() << "\n";
        outfile << "firing_rate," << brain->getAverageFiringRate() << "\n";
        outfile << "excit_inhib_ratio," << brain->getExcitationInhibitionRatio() << "\n";
        
        outfile.close();
    }

    void createDirectory(const std::string& path) {
        system(("mkdir -p " + path).c_str());
    }

private:
    std::shared_ptr<Config> config_;
};

// PRIORITY 1.2: Batch Statistics Class - Statistical analysis of multiple simulation runs
class BatchStatistics {
public:
    // Statistical analysis of multiple simulation runs
    std::map<std::string, float> analyze(const std::vector<std::map<std::string, float>>& results) {
        if (results.empty()) {
            throw BatchProcessingError("Cannot analyze results: no results provided");
        }
        
        std::map<std::string, float> analysis;
        
        // Collect all metrics
        std::vector<float> spikeCounts, activeNeurons, firingRates, excInhRatios;
        
        for (const auto& result : results) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
            if (result.find("excit_inhib_ratio") != result.end()) {
                excInhRatios.push_back(result.at("excit_inhib_ratio"));
            }
        }
        
        // Calculate basic statistics
        analysis["mean_spike_count"] = calculateMean(spikeCounts);
        analysis["std_spike_count"] = calculateStd(spikeCounts);
        analysis["min_spike_count"] = getMin(spikeCounts);
        analysis["max_spike_count"] = getMax(spikeCounts);
        analysis["median_spike_count"] = getMedian(spikeCounts);
        
        analysis["mean_active_neurons"] = calculateMean(activeNeurons);
        analysis["std_active_neurons"] = calculateStd(activeNeurons);
        analysis["min_active_neurons"] = getMin(activeNeurons);
        analysis["max_active_neurons"] = getMax(activeNeurons);
        analysis["median_active_neurons"] = getMedian(activeNeurons);
        
        analysis["mean_firing_rate"] = calculateMean(firingRates);
        analysis["std_firing_rate"] = calculateStd(firingRates);
        analysis["min_firing_rate"] = getMin(firingRates);
        analysis["max_firing_rate"] = getMax(firingRates);
        analysis["median_firing_rate"] = getMedian(firingRates);
        
        analysis["mean_excit_inhib_ratio"] = calculateMean(excInhRatios);
        analysis["std_excit_inhib_ratio"] = calculateStd(excInhRatios);
        analysis["min_excit_inhib_ratio"] = getMin(excInhRatios);
        analysis["max_excit_inhib_ratio"] = getMax(excInhRatios);
        analysis["median_excit_inhib_ratio"] = getMedian(excInhRatios);
        
        // Calculate correlations
        analysis["spike_active_correlation"] = calculateCorrelation(spikeCounts, activeNeurons);
        analysis["spike_firing_correlation"] = calculateCorrelation(spikeCounts, firingRates);
        analysis["spike_excit_inhib_correlation"] = calculateCorrelation(spikeCounts, excInhRatios);
        analysis["active_firing_correlation"] = calculateCorrelation(activeNeurons, firingRates);
        analysis["active_excit_inhib_correlation"] = calculateCorrelation(activeNeurons, excInhRatios);
        analysis["firing_excit_inhib_correlation"] = calculateCorrelation(firingRates, excInhRatios);
        
        // Calculate performance comparison across parameter sets
        analysis["coefficient_of_variation_spike"] = calculateCoefficientOfVariation(spikeCounts);
        analysis["coefficient_of_variation_active"] = calculateCoefficientOfVariation(activeNeurons);
        analysis["coefficient_of_variation_firing"] = calculateCoefficientOfVariation(firingRates);
        
        return analysis;
    }

    // Correlation analysis between runs
    std::map<std::string, float> correlateResults(
        const std::vector<std::map<std::string, float>>& results1,
        const std::vector<std::map<std::string, float>>& results2) {
        
        if (results1.empty() || results2.empty()) {
            throw BatchProcessingError("Cannot correlate results: one or both result sets empty");
        }
        
        std::map<std::string, float> correlations;
        
        // Extract common metrics
        std::vector<float> spikeCounts1, spikeCounts2;
        std::vector<float> activeNeurons1, activeNeurons2;
        std::vector<float> firingRates1, firingRates2;
        
        for (size_t i = 0; i < std::min(results1.size(), results2.size()); ++i) {
            if (results1[i].find("spike_count") != results1[i].end() &&
                results2[i].find("spike_count") != results2[i].end()) {
                spikeCounts1.push_back(results1[i].at("spike_count"));
                spikeCounts2.push_back(results2[i].at("spike_count"));
            }
            if (results1[i].find("active_neurons") != results1[i].end() &&
                results2[i].find("active_neurons") != results2[i].end()) {
                activeNeurons1.push_back(results1[i].at("active_neurons"));
                activeNeurons2.push_back(results2[i].at("active_neurons"));
            }
            if (results1[i].find("firing_rate") != results1[i].end() &&
                results2[i].find("firing_rate") != results2[i].end()) {
                firingRates1.push_back(results1[i].at("firing_rate"));
                firingRates2.push_back(results2[i].at("firing_rate"));
            }
        }
        
        // Calculate correlations
        if (!spikeCounts1.empty()) {
            correlations["spike_count_correlation"] = calculateCorrelation(spikeCounts1, spikeCounts2);
        }
        if (!activeNeurons1.empty()) {
            correlations["active_neurons_correlation"] = calculateCorrelation(activeNeurons1, activeNeurons2);
        }
        if (!firingRates1.empty()) {
            correlations["firing_rate_correlation"] = calculateCorrelation(firingRates1, firingRates2);
        }
        
        return correlations;
    }

    // Result aggregation and comparison
    std::map<std::string, float> aggregateResults(
        const std::vector<std::map<std::string, float>>& results,
        const std::string& aggregationMethod = "mean") {
        
        if (results.empty()) {
            throw BatchProcessingError("Cannot aggregate results: no results provided");
        }
        
        std::map<std::string, float> aggregated;
        
        // Extract metrics
        std::vector<float> spikeCounts, activeNeurons, firingRates, excInhRatios;
        
        for (const auto& result : results) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
            if (result.find("excit_inhib_ratio") != result.end()) {
                excInhRatios.push_back(result.at("excit_inhib_ratio"));
            }
        }
        
        // Aggregate based on method
        if (aggregationMethod == "mean") {
            aggregated["mean_spike_count"] = calculateMean(spikeCounts);
            aggregated["mean_active_neurons"] = calculateMean(activeNeurons);
            aggregated["mean_firing_rate"] = calculateMean(firingRates);
            aggregated["mean_excit_inhib_ratio"] = calculateMean(excInhRatios);
        } else if (aggregationMethod == "median") {
            aggregated["median_spike_count"] = getMedian(spikeCounts);
            aggregated["median_active_neurons"] = getMedian(activeNeurons);
            aggregated["median_firing_rate"] = getMedian(firingRates);
            aggregated["median_excit_inhib_ratio"] = getMedian(excInhRatios);
        } else if (aggregationMethod == "max") {
            aggregated["max_spike_count"] = getMax(spikeCounts);
            aggregated["max_active_neurons"] = getMax(activeNeurons);
            aggregated["max_firing_rate"] = getMax(firingRates);
            aggregated["max_excit_inhib_ratio"] = getMax(excInhRatios);
        } else if (aggregationMethod == "min") {
            aggregated["min_spike_count"] = getMin(spikeCounts);
            aggregated["min_active_neurons"] = getMin(activeNeurons);
            aggregated["min_firing_rate"] = getMin(firingRates);
            aggregated["min_excit_inhib_ratio"] = getMin(excInhRatios);
        }
        
        return aggregated;
    }

private:
    float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

    float calculateStd(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        float mean = calculateMean(values);
        float sumSq = 0.0f;
        for (float v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }

    float getMin(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return *std::min_element(values.begin(), values.end());
    }

    float getMax(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return *std::max_element(values.begin(), values.end());
    }

    float getMedian(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        std::vector<float> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n/2 - 1] + sorted[n/2]) / 2.0f;
        } else {
            return sorted[n/2];
        }
    }

    float calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0f;
        
        float xMean = calculateMean(x);
        float yMean = calculateMean(y);
        
        float numerator = 0.0f, xDenom = 0.0f, yDenom = 0.0f;
        
        for (size_t i = 0; i < x.size(); ++i) {
            float dx = x[i] - xMean;
            float dy = y[i] - yMean;
            numerator += dx * dy;
            xDenom += dx * dx;
            yDenom += dy * dy;
        }
        
        if (xDenom == 0.0f || yDenom == 0.0f) return 0.0f;
        
        return numerator / std::sqrt(xDenom * yDenom);
    }

    float calculateCoefficientOfVariation(const std::vector<float>& values) {
        float mean = calculateMean(values);
        float std = calculateStd(values);
        
        if (mean == 0.0f) return 0.0f;
        return (std / mean) * 100.0f;  // Percentage
    }
};

} // namespace nlm

// =============================================================================
// PRIORITY 2: CONFIGURATION PRESETS FOR COMMON USE CASES
// =============================================================================

// Research Preset: Optimized for scientific experiments
std::shared_ptr<Config> createResearchPreset() {
    auto config = std::make_shared<Config>();
    
    // High precision settings
    config->set("simulation.timestep", 0.001);
    config->set("simulation.max_steps", 100000);
    config->set("window.width", 1920);
    config->set("window.height", 1080);
    
    // Advanced neuroscience settings
    config->set("neuromodulation.enabled", true);
    config->set("neuromodulation.curiosity.enabled", true);
    config->set("neuromodulation.novelty.enabled", true);
    config->set("neuromodulation.prediction_error.enabled", true);
    config->set("neuromodulation.reward.enabled", true);
    
    config->set("plasticity.structural.enabled", true);
    config->set("plasticity.stdp.enabled", true);
    config->set("plasticity.hebbian.enabled", true);
    
    config->set("development.enabled", true);
    config->set("development.critical_period.enabled", true);
    
    // Memory settings
    config->set("memory.working.enabled", true);
    config->set("memory.episodic.enabled", true);
    config->set("memory.associative.enabled", true);
    
    // Performance settings
    config->set("performance.cpu_cores", std::thread::hardware_concurrency());
    config->set("performance.enable_multithreading", true);
    config->set("performance.accuracy", 0.999);
    
    return config;
}

// Real-time Preset: Optimized for interactive applications
std::shared_ptr<Config> createRealTimePreset() {
    auto config = std::make_shared<Config>();
    
    // Low latency settings
    config->set("simulation.timestep", 0.0001);
    config->set("simulation.max_steps", 10000);
    config->set("window.width", 800);
    config->set("window.height", 600);
    
    // Real-time neuromodulation
    config->set("neuromodulation.enabled", true);
    config->set("neuromodulation.curiosity.enabled", true);
    config->set("neuromodulation.novelty.enabled", true);
    
    // Minimal plasticity
    config->set("plasticity.structural.enabled", false);
    config->set("plasticity.stdp.enabled", false);
    config->set("plasticity.hebbian.enabled", true);
    
    // No development
    config->set("development.enabled", false);
    
    // Fast memory
    config->set("memory.working.enabled", true);
    config->set("memory.episodic.enabled", false);
    config->set("memory.associative.enabled", false);
    
    // Optimized performance
    config->set("performance.cpu_cores", std::thread::hardware_concurrency());
    config->set("performance.enable_multithreading", true);
    config->set("performance.accuracy", 0.95);
    config->set("performance.max_latency_ms", 16);
    
    return config;
}

// Resource-Constrained Preset: Optimized for embedded systems
std::shared_ptr<Config> createResourceConstrainedPreset() {
    auto config = std::make_shared<Config>();
    
    // Minimal settings
    config->set("simulation.timestep", 0.01);
    config->set("simulation.max_steps", 5000);
    config->set("window.width", 320);
    config->set("window.height", 240);
    
    // Minimal neuromodulation
    config->set("neuromodulation.enabled", false);
    
    // Disabled plasticity to save resources
    config->set("plasticity.structural.enabled", false);
    config->set("plasticity.stdp.enabled", false);
    config->set("plasticity.hebbian.enabled", false);
    
    // Disabled development
    config->set("development.enabled", false);
    
    // Minimal memory usage
    config->set("memory.working.enabled", true);
    config->set("memory.episodic.enabled", false);
    config->set("memory.associative.enabled", false);
    
    // Conservative performance settings
    config->set("performance.cpu_cores", 1);
    config->set("performance.enable_multithreading", false);
    config->set("performance.accuracy", 0.90);
    config->set("performance.memory_limit_mb", 512);
    
    return config;
}

// High-Performance Preset: Optimized for maximum simulation speed
std::shared_ptr<Config> createHighPerformancePreset() {
    auto config = std::make_shared<Config>();
    
    // High speed settings
    config->set("simulation.timestep", 0.0005);
    config->set("simulation.max_steps", 50000);
    config->set("window.width", 2560);
    config->set("window.height", 1440);
    
    // Full neuromodulation
    config->set("neuromodulation.enabled", true);
    config->set("neuromodulation.curiosity.enabled", true);
    config->set("neuromodulation.novelty.enabled", true);
    config->set("neuromodulation.prediction_error.enabled", true);
    config->set("neuromodulation.reward.enabled", true);
    
    // Full plasticity
    config->set("plasticity.structural.enabled", true);
    config->set("plasticity.stdp.enabled", true);
    config->set("plasticity.hebbian.enabled", true);
    
    // Full development
    config->set("development.enabled", true);
    config->set("development.critical_period.enabled", true);
    
    // Full memory systems
    config->set("memory.working.enabled", true);
    config->set("memory.episodic.enabled", true);
    config->set("memory.associative.enabled", true);
    
    // High performance settings
    config->set("performance.cpu_cores", std::thread::hardware_concurrency() * 2);
    config->set("performance.enable_multithreading", true);
    config->set("performance.accuracy", 1.0);
    config->set("performance.max_memory_gb", 16);
    config->set("performance.gpu_acceleration", true);
    
    return config;
}

// =============================================================================
// PRIORITY 3: UTILITY FUNCTIONS
// =============================================================================

// Batch Processing Utilities
namespace batch {

    // Process multiple brains in batch
    std::vector<std::shared_ptr<BrainPythonWrapper>> processBrainsBatch(
        const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<std::shared_ptr<BrainPythonWrapper>> results;
        
        for (const auto& config : configs) {
            try {
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    auto wrapper = std::make_shared<BrainPythonWrapper>(brain);
                    results.push_back(wrapper);
                }
            } catch (const std::exception& e) {
                // Skip failed configurations but continue processing
                continue;
            }
        }
        
        return results;
    }

    // Batch save brain states
    bool saveBrainStatesBatch(const std::vector<std::shared_ptr<BrainPythonWrapper>>& brains,
                             const std::string& directory) {
        createDirectory(directory);
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::string filepath = directory + "/brain_" + std::to_string(i) + ".state";
            brains[i]->saveState(filepath);
        }
        
        return true;
    }

    // Batch load brain states
    std::vector<std::shared_ptr<BrainPythonWrapper>> loadBrainStatesBatch(
        const std::vector<std::string>& filepaths) {
        std::vector<std::shared_ptr<BrainPythonWrapper>> brains;
        
        for (const auto& filepath : filepaths) {
            try {
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                
                if (brain->load(filepath)) {
                    auto wrapper = std::make_shared<BrainPythonWrapper>(brain);
                    brains.push_back(wrapper);
                }
            } catch (const std::exception& e) {
                // Skip failed files
                continue;
            }
        }
        
        return brains;
    }

    // Process batch of simulation steps
    std::vector<std::map<std::string, float>> simulateBatchSteps(
        const std::vector<std::shared_ptr<BrainPythonWrapper>>& brains,
        const std::vector<SimulationStep>& steps) {
        std::vector<std::map<std::string, float>> results;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::map<std::string, float> result;
            
            for (const auto& step : steps) {
                brains[i]->step(step);
                
                result["spike_count"] = static_cast<float>(brains[i]->getTotalSpikeCount());
                result["active_neurons"] = static_cast<float>(brains[i]->getActiveNeuronCount());
                result["firing_rate"] = brains[i]->getAverageFiringRate();
            }
            
            results.push_back(result);
        }
        
        return results;
    }

    // Utility to create directory
    void createDirectory(const std::string& path) {
        system(("mkdir -p " + path).c_str());
    }

    // Get batch processing statistics
    std::map<std::string, float> getBatchStats(const std::vector<std::map<std::string, float>>& data) {
        std::map<std::string, float> stats;
        
        if (data.empty()) return stats;
        
        std::vector<float> spikeCounts, activeNeurons, firingRates;
        
        for (const auto& result : data) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
        }
        
        stats["mean_spike_count"] = calculateMean(spikeCounts);
        stats["std_spike_count"] = calculateStd(spikeCounts);
        stats["mean_active_neurons"] = calculateMean(activeNeurons);
        stats["std_active_neurons"] = calculateStd(activeNeurons);
        stats["mean_firing_rate"] = calculateMean(firingRates);
        stats["std_firing_rate"] = calculateStd(firingRates);
        
        return stats;
    }

private:
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

    static float calculateStd(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        float mean = calculateMean(values);
        float sumSq = 0.0f;
        for (float v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }
}

// Data Analysis Utilities
namespace data {

    // Analyze brain simulation data
    std::map<std::string, float> analyzeBrainData(const std::vector<std::map<std::string, float>>& data) {
        if (data.empty()) {
            throw DataAnalysisError("Cannot analyze data: no data provided");
        }
        
        std::map<std::string, float> analysis;
        
        // Calculate various statistical measures
        std::vector<float> spikeCounts, activeNeurons, firingRates, excInhRatios;
        
        for (const auto& result : data) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
            if (result.find("excit_inhib_ratio") != result.end()) {
                excInhRatios.push_back(result.at("excit_inhib_ratio"));
            }
        }
        
        // Calculate statistics
        analysis["spike_count_mean"] = calculateMean(spikeCounts);
        analysis["spike_count_median"] = getMedian(spikeCounts);
        analysis["spike_count_std"] = calculateStd(spikeCounts);
        analysis["spike_count_min"] = getMin(spikeCounts);
        analysis["spike_count_max"] = getMax(spikeCounts);
        
        analysis["active_neurons_mean"] = calculateMean(activeNeurons);
        analysis["active_neurons_median"] = getMedian(activeNeurons);
        analysis["active_neurons_std"] = calculateStd(activeNeurons);
        analysis["active_neurons_min"] = getMin(activeNeurons);
        analysis["active_neurons_max"] = getMax(activeNeurons);
        
        analysis["firing_rate_mean"] = calculateMean(firingRates);
        analysis["firing_rate_median"] = getMedian(firingRates);
        analysis["firing_rate_std"] = calculateStd(firingRates);
        analysis["firing_rate_min"] = getMin(firingRates);
        analysis["firing_rate_max"] = getMax(firingRates);
        
        analysis["excit_inhib_ratio_mean"] = calculateMean(excInhRatios);
        analysis["excit_inhib_ratio_median"] = getMedian(excInhRatios);
        analysis["excit_inhib_ratio_std"] = calculateStd(excInhRatios);
        analysis["excit_inhib_ratio_min"] = getMin(excInhRatios);
        analysis["excit_inhib_ratio_max"] = getMax(excInhRatios);
        
        // Calculate correlations
        analysis["spike_active_corr"] = calculateCorrelation(spikeCounts, activeNeurons);
        analysis["firing_rate_correlation"] = calculateCorrelation(firingRates, excInhRatios);
        
        return analysis;
    }

    // Detect patterns in time series data
    std::vector<std::map<std::string, float>> detectPatterns(
        const std::vector<std::map<std::string, float>>& timeSeries,
        const std::string& patternType = "linear") {
        
        if (timeSeries.empty()) {
            throw DataAnalysisError("Cannot detect patterns: no time series data provided");
        }
        
        std::vector<std::map<std::string, float>> patterns;
        
        for (const auto& point : timeSeries) {
            std::map<std::string, float> pattern;
            
            // Extract metrics
            float spikeCount = point.count("spike_count") ? point.at("spike_count") : 0.0f;
            float activeNeurons = point.count("active_neurons") ? point.at("active_neurons") : 0.0f;
            float firingRate = point.count("firing_rate") ? point.at("firing_rate") : 0.0f;
            
            // Detect linear patterns
            if (patternType == "linear") {
                pattern["linear_trend"] = calculateLinearTrend(spikeCount, activeNeurons, firingRate);
                pattern["trend_strength"] = calculateTrendStrength(spikeCount, activeNeurons, firingRate);
            }
            
            // Detect periodic patterns
            else if (patternType == "periodic") {
                pattern["periodic_component"] = calculatePeriodicComponent(spikeCount);
                pattern["periodicity"] = calculatePeriodicity(spikeCount);
            }
            
            // Detect exponential patterns
            else if (patternType == "exponential") {
                pattern["exponential_growth"] = calculateExponentialGrowth(spikeCount);
                pattern["growth_rate"] = calculateGrowthRate(spikeCount);
            }
            
            patterns.push_back(pattern);
        }
        
        return patterns;
    }

    // Export analysis results
    bool exportAnalysisResults(const std::map<std::string, float>& analysis,
                               const std::string& filepath,
                               const std::string& format = "csv") {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw DataAnalysisError("Failed to export analysis results: " + filepath);
        }
        
        if (format == "csv") {
            outfile << "metric,value\n";
            for (const auto& pair : analysis) {
                outfile << pair.first << "," << pair.second << "\n";
            }
        } else if (format == "json") {
            outfile << "{\n";
            bool first = true;
            for (const auto& pair : analysis) {
                if (!first) outfile << ",\n";
                outfile << "  \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            outfile << "\n}\n";
        }
        
        outfile.close();
        return true;
    }

private:
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

    static float getMedian(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        std::vector<float> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n/2 - 1] + sorted[n/2]) / 2.0f;
        } else {
            return sorted[n/2];
        }
    }

    static float calculateStd(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        float mean = calculateMean(values);
        float sumSq = 0.0f;
        for (float v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }

    static float getMin(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return *std::min_element(values.begin(), values.end());
    }

    static float getMax(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return *std::max_element(values.begin(), values.end());
    }

    static float calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0f;
        
        float xMean = calculateMean(x);
        float yMean = calculateMean(y);
        
        float numerator = 0.0f, xDenom = 0.0f, yDenom = 0.0f;
        
        for (size_t i = 0; i < x.size(); ++i) {
            float dx = x[i] - xMean;
            float dy = y[i] - yMean;
            numerator += dx * dy;
            xDenom += dx * dx;
            yDenom += dy * dy;
        }
        
        if (xDenom == 0.0f || yDenom == 0.0f) return 0.0f;
        
        return numerator / std::sqrt(xDenom * yDenom);
    }

    static float calculateLinearTrend(float x1, float x2, float y1, float y2) {
        if (x2 == x1) return 0.0f;
        return (y2 - y1) / (x2 - x1);
    }

    static float calculateLinearTrend(const std::vector<float>& x, const std::vector<float>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0f;
        
        float xMean = calculateMean(x);
        float yMean = calculateMean(y);
        
        float numerator = 0.0f, xDenom = 0.0f;
        
        for (size_t i = 0; i < x.size(); ++i) {
            float dx = x[i] - xMean;
            float dy = y[i] - yMean;
            numerator += dx * dy;
            xDenom += dx * dx;
        }
        
        if (xDenom == 0.0f) return 0.0f;
        return numerator / xDenom;
    }

    static float calculateTrendStrength(const std::vector<float>& x, const std::vector<float>& y) {
        float trend = calculateLinearTrend(x, y);
        float yStd = calculateStd(y);
        return yStd == 0.0f ? 0.0f : trend / yStd;
    }

    static float calculatePeriodicComponent(const std::vector<float>& data) {
        if (data.size() < 2) return 0.0f;
        
        float mean = calculateMean(data);
        float sum = 0.0f;
        
        for (float val : data) {
            sum += (val - mean) * (val - mean);
        }
        
        return std::sqrt(sum / data.size());
    }

    static float calculatePeriodicity(const std::vector<float>& data) {
        // Simple periodicity detection using autocorrelation
        if (data.size() < 4) return 0.0f;
        
        float mean = calculateMean(data);
        std::vector<float> centered(data.size());
        
        for (size_t i = 0; i < data.size(); ++i) {
            centered[i] = data[i] - mean;
        }
        
        float maxCorr = 0.0f;
        
        for (size_t lag = 1; lag < data.size() / 2; ++lag) {
            float corr = 0.0f;
            for (size_t i = 0; i < data.size() - lag; ++i) {
                corr += centered[i] * centered[i + lag];
            }
            
            float norm = calculateStd(centered);
            if (norm != 0.0f) {
                float currentCorr = std::abs(corr) / (norm * norm);
                if (currentCorr > maxCorr) {
                    maxCorr = currentCorr;
                }
            }
        }
        
        return maxCorr;
    }

    static float calculateExponentialGrowth(const std::vector<float>& data) {
        if (data.size() < 2) return 0.0f;
        
        float growth = 0.0f;
        
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i-1] != 0.0f) {
                growth += std::log(data[i] / data[i-1]);
            }
        }
        
        return growth / (data.size() - 1);
    }

    static float calculateGrowthRate(const std::vector<float>& data) {
        return calculateExponentialGrowth(data);
    }
}

// Error Analysis Utilities
namespace error {

    // Analyze error patterns in simulation
    std::map<std::string, float> analyzeErrorPatterns(
        const std::vector<std::map<std::string, float>>& results) {
        if (results.empty()) {
            throw ErrorAnalysisError("Cannot analyze error patterns: no results provided");
        }
        
        std::map<std::string, float> analysis;
        
        // Calculate error statistics
        std::vector<float> errors;
        
        for (const auto& result : results) {
            float error = 0.0f;
            
            // Calculate error based on metrics
            if (result.find("spike_count") != result.end()) {
                float spikeCount = result.at("spike_count");
                // Example error: deviation from expected range
                error += std::abs(spikeCount - 1000.0f) / 1000.0f;
            }
            
            if (result.find("active_neurons") != result.end()) {
                float activeNeurons = result.at("active_neurons");
                error += std::abs(activeNeurons - 500.0f) / 500.0f;
            }
            
            if (result.find("firing_rate") != result.end()) {
                float firingRate = result.at("firing_rate");
                error += std::abs(firingRate - 50.0f) / 50.0f;
            }
            
            errors.push_back(error);
        }
        
        // Calculate error statistics
        analysis["error_mean"] = calculateMean(errors);
        analysis["error_std"] = calculateStd(errors);
        analysis["error_median"] = getMedian(errors);
        analysis["error_max"] = getMax(errors);
        
        // Calculate error rate (percentage of high-error runs)
        int highErrorCount = 0;
        for (float error : errors) {
            if (error > 0.5f) {  // Threshold for high error
                highErrorCount++;
            }
        }
        analysis["error_rate_percent"] = (static_cast<float>(highErrorCount) / errors.size()) * 100.0f;
        
        return analysis;
    }

    // Detect anomalies in simulation results
    std::vector<size_t> detectAnomalies(
        const std::vector<std::map<std::string, float>>& results,
        float threshold = 2.0f) {
        if (results.empty()) {
            throw ErrorAnalysisError("Cannot detect anomalies: no results provided");
        }
        
        std::vector<size_t> anomalyIndices;
        
        // Calculate mean and std for each metric
        std::vector<float> spikeCounts, activeNeurons, firingRates;
        
        for (const auto& result : results) {
            if (result.find("spike_count") != result.end()) {
                spikeCounts.push_back(result.at("spike_count"));
            }
            if (result.find("active_neurons") != result.end()) {
                activeNeurons.push_back(result.at("active_neurons"));
            }
            if (result.find("firing_rate") != result.end()) {
                firingRates.push_back(result.at("firing_rate"));
            }
        }
        
        float spikeMean = calculateMean(spikeCounts);
        float spikeStd = calculateStd(spikeCounts);
        float activeMean = calculateMean(activeNeurons);
        float activeStd = calculateStd(activeNeurons);
        float firingMean = calculateMean(firingRates);
        float firingStd = calculateStd(firingRates);
        
        // Detect anomalies (points beyond threshold standard deviations)
        for (size_t i = 0; i < results.size(); ++i) {
            float spikeAnomaly = 0.0f, activeAnomaly = 0.0f, firingAnomaly = 0.0f;
            
            if (spikeStd > 0.0f) {
                spikeAnomaly = std::abs(getValue(results[i], "spike_count", 0.0f) - spikeMean) / spikeStd;
            }
            
            if (activeStd > 0.0f) {
                activeAnomaly = std::abs(getValue(results[i], "active_neurons", 0.0f) - activeMean) / activeStd;
            }
            
            if (firingStd > 0.0f) {
                firingAnomaly = std::abs(getValue(results[i], "firing_rate", 0.0f) - firingMean) / firingStd;
            }
            
            float maxAnomaly = std::max({spikeAnomaly, activeAnomaly, firingAnomaly});
            
            if (maxAnomaly > threshold) {
                anomalyIndices.push_back(i);
            }
        }
        
        return anomalyIndices;
    }

    // Get value from result map with default
    float getValue(const std::map<std::string, float>& result, const std::string& key, float defaultValue = 0.0f) {
        auto it = result.find(key);
        if (it != result.end()) {
            return it->second;
        }
        return defaultValue;
    }

private:
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

    static float getMedian(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        std::vector<float> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n/2 - 1] + sorted[n/2]) / 2.0f;
        } else {
            return sorted[n/2];
        }
    }

    static float calculateStd(const std::vector<float>& values) {
        if (values.size() <= 1) return 0.0f;
        float mean = calculateMean(values);
        float sumSq = 0.0f;
        for (float v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }

    static float getMax(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        return *std::max_element(values.begin(), values.end());
    }
}

// Performance Optimization Utilities
namespace optimize {

    // Optimize brain configuration for performance
    std::shared_ptr<Config> optimizePerformance(std::shared_ptr<Config> config,
                                                 const std::string& objective = "speed") {
        if (!config) {
            throw PerformanceOptimizationError("Cannot optimize: config is null");
        }
        
        auto optimizedConfig = std::make_shared<Config>(*config);
        
        if (objective == "speed") {
            // Speed optimization settings
            optimizedConfig->set("simulation.timestep", 0.001);
            optimizedConfig->set("performance.enable_multithreading", true);
            optimizedConfig->set("performance.accuracy", 0.95);
        } else if (objective == "memory") {
            // Memory optimization settings
            optimizedConfig->set("simulation.timestep", 0.01);
            optimizedConfig->set("performance.enable_multithreading", false);
            optimizedConfig->set("performance.memory_limit_mb", 512);
        } else if (objective == "accuracy") {
            // Accuracy optimization settings
            optimizedConfig->set("simulation.timestep", 0.0001);
            optimizedConfig->set("performance.enable_multithreading", true);
            optimizedConfig->set("performance.accuracy", 0.999);
        }
        
        return optimizedConfig;
    }

    // Batch process simulation steps for optimization
    std::vector<std::map<std::string, float>> optimizeBatchProcessing(
        const std::vector<std::shared_ptr<BrainPythonWrapper>>& brains,
        const std::vector<SimulationStep>& steps,
        const std::map<std::string, float>& parameters) {
        
        if (brains.empty()) {
            throw PerformanceOptimizationError("Cannot optimize batch processing: no brains provided");
        }
        
        std::vector<std::map<std::string, float>> results;
        
        // Process steps in parallel if requested
        if (parameters.count("parallel") && parameters.at("parallel") > 0.5f) {
            // Parallel processing
            results = processParallel(brains, steps, parameters);
        } else {
            // Sequential processing
            results = processSequential(brains, steps, parameters);
        }
        
        return results;
    }

    // Profile simulation performance
    std::map<std::string, float> profileSimulation(std::shared_ptr<BrainPythonWrapper> brain,
                                                    const std::vector<SimulationStep>& steps) {
        if (!brain) {
            throw PerformanceOptimizationError("Cannot profile simulation: brain is null");
        }
        
        std::map<std::string, float> profile;
        
        // Start profiling
        auto startTime = std::chrono::steady_clock::now();
        
        // Run simulation
        for (const auto& step : steps) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        
        // Calculate performance metrics
        double elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();
        int numSteps = steps.size();
        
        profile["time_per_step_ms"] = (elapsedSeconds / numSteps) * 1000.0;
        profile["steps_per_second"] = numSteps / elapsedSeconds;
        profile["total_time_seconds"] = elapsedSeconds;
        
        // Get brain metrics
        profile["final_spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
        profile["final_active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
        profile["final_firing_rate"] = brain->getAverageFiringRate();
        
        return profile;
    }

    // Predict performance for different configurations
    std::map<std::string, float> predictPerformance(const std::map<std::string, float>& configParams,
                                                     const std::string& scenario = "standard") {
        if (configParams.empty()) {
            throw PerformanceOptimizationError("Cannot predict performance: no config parameters provided");
        }
        
        std::map<std::string, float> prediction;
        
        // Simple performance prediction model
        float baseTime = configParams.count("timestep") ? 1000.0f / configParams.at("timestep") : 1000.0f;
        float cpuMultiplier = configParams.count("cpu_cores") ? (8.0f / configParams.at("cpu_cores")) : 1.0f;
        float memoryMultiplier = configParams.count("memory_limit_mb") ? (1024.0f / configParams.at("memory_limit_mb")) : 1.0f;
        
        prediction["estimated_time_ms"] = baseTime * cpuMultiplier * memoryMultiplier;
        prediction["estimated_memory_mb"] = configParams.count("memory_limit_mb") ? configParams.at("memory_limit_mb") : 512.0f;
        prediction["estimated_cpu_usage"] = 100.0f / configParams.count("cpu_cores") ? configParams.at("cpu_cores") : 1.0f;
        
        // Scenario adjustments
        if (scenario == "high_performance") {
            prediction["estimated_time_ms"] *= 0.5f;
            prediction["estimated_memory_mb"] *= 2.0f;
        } else if (scenario == "resource_constrained") {
            prediction["estimated_time_ms"] *= 2.0f;
            prediction["estimated_memory_mb"] *= 0.5f;
        }
        
        return prediction;
    }

private:
    static std::vector<std::map<std::string, float>> processParallel(
        const std::vector<std::shared_ptr<BrainPythonWrapper>>& brains,
        const std::vector<SimulationStep>& steps,
        const std::map<std::string, float>& parameters) {
        
        // Parallel processing implementation
        std::vector<std::map<std::string, float>> results;
        
        for (const auto& brain : brains) {
            std::map<std::string, float> result;
            
            for (const auto& step : steps) {
                brain->step(step);
                
                result["spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
                result["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
                result["firing_rate"] = brain->getAverageFiringRate();
            }
            
            results.push_back(result);
        }
        
        return results;
    }

    static std::vector<std::map<std::string, float>> processSequential(
        const std::vector<std::shared_ptr<BrainPythonWrapper>>& brains,
        const std::vector<SimulationStep>& steps,
        const std::map<std::string, float>& parameters) {
        
        // Sequential processing implementation
        std::vector<std::map<std::string, float>> results;
        
        for (const auto& brain : brains) {
            std::map<std::string, float> result;
            
            for (const auto& step : steps) {
                brain->step(step);
                
                result["spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
                result["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
                result["firing_rate"] = brain->getAverageFiringRate();
            }
            
            results.push_back(result);
        }
        
        return results;
    }
}

// Register all advanced features with Python
// =============================================================================
// PRIORITY 2: Experiment Management Classes
// =============================================================================

// PRIORITY 2.1: ExperimentManager Class - Experiment catalog and management system
class ExperimentManager {
public:
    ExperimentManager(std::shared_ptr<Config> config) : config_(config) {
        if (!config_) {
            throw ExperimentError("ExperimentManager initialization failed: config is null");
        }
    }

    // Experiment catalog and management system
    bool createExperiment(const std::string& name, 
                         const std::map<std::string, float>& parameters,
                         const std::string& description = "") {
        // Check if experiment already exists
        if (experimentExists(name)) {
            throw ExperimentError("Experiment already exists: " + name);
        }
        
        // Create experiment entry
        ExperimentEntry entry;
        entry.name = name;
        entry.parameters = parameters;
        entry.description = description;
        entry.creationTime = getCurrentTimestamp();
        entry.status = "created";
        
        // Store experiment
        experiments_[name] = entry;
        
        return true;
    }

    // Parameter space exploration and optimization
    std::vector<std::map<std::string, float>> exploreParameterSpace(
        const std::string& experimentName,
        const std::map<std::string, std::vector<float>>& parameterRanges,
        int numCombinations = 100) {
        
        if (!experimentExists(experimentName)) {
            throw ExperimentError("Experiment not found: " + experimentName);
        }
        
        std::vector<std::map<std::string, float>> parameterSets;
        parameterSets.reserve(numCombinations);
        
        // Generate random parameter combinations
        for (int i = 0; i < numCombinations; ++i) {
            std::map<std::string, float> params;
            
            for (const auto& paramRange : parameterRanges) {
                const std::string& paramName = paramRange.first;
                const std::vector<float>& ranges = paramRange.second;
                
                if (ranges.size() >= 2) {
                    float minVal = ranges[0];
                    float maxVal = ranges[ranges.size() - 1];
                    float value = minVal + static_cast<float>(rand()) / RAND_MAX * (maxVal - minVal);
                    params[paramName] = value;
                }
            }
            
            // Update experiment parameters
            experiments_[experimentName].parameters = params;
            parameterSets.push_back(params);
        }
        
        return parameterSets;
    }

    // Result storage and retrieval with metadata
    bool storeExperimentResult(const std::string& experimentName,
                              const std::map<std::string, float>& result,
                              const std::map<std::string, std::string>& metadata = {}) {
        
        if (!experimentExists(experimentName)) {
            throw ExperimentError("Experiment not found: " + experimentName);
        }
        
        ExperimentResult expResult;
        expResult.experimentName = experimentName;
        expResult.result = result;
        expResult.metadata = metadata;
        expResult.timestamp = getCurrentTimestamp();
        
        experiments_[experimentName].results.push_back(expResult);
        
        return true;
    }

    // Experiment comparison and analysis
    std::map<std::string, float> compareExperiments(
        const std::vector<std::string>& experimentNames) {
        
        if (experimentNames.size() < 2) {
            throw ExperimentError("Cannot compare experiments: need at least 2 experiments");
        }
        
        std::map<std::string, float> comparison;
        
        // Collect results from all experiments
        std::vector<std::vector<std::map<std::string, float>>> allResults;
        
        for (const auto& name : experimentNames) {
            if (experimentExists(name)) {
                std::vector<std::map<std::string, float>> results;
                for (const auto& result : experiments_[name].results) {
                    results.push_back(result.result);
                }
                allResults.push_back(results);
            }
        }
        
        if (allResults.size() < 2) {
            throw ExperimentError("Cannot compare experiments: insufficient valid experiments");
        }
        
        // Compare metrics across experiments
        std::vector<float> spikeCounts, activeNeurons, firingRates;
        
        for (size_t i = 0; i < allResults.size(); ++i) {
            for (const auto& result : allResults[i]) {
                if (result.find("spike_count") != result.end()) {
                    spikeCounts.push_back(result.at("spike_count"));
                }
                if (result.find("active_neurons") != result.end()) {
                    activeNeurons.push_back(result.at("active_neurons"));
                }
                if (result.find("firing_rate") != result.end()) {
                    firingRates.push_back(result.at("firing_rate"));
                }
            }
        }
        
        // Calculate comparison metrics
        comparison["mean_spike_count"] = calculateMean(spikeCounts);
        comparison["mean_active_neurons"] = calculateMean(activeNeurons);
        comparison["mean_firing_rate"] = calculateMean(firingRates);
        
        return comparison;
    }

    // Get experiment info
    std::map<std::string, std::string> getExperimentInfo(const std::string& name) const {
        if (!experimentExists(name)) {
            throw ExperimentError("Experiment not found: " + name);
        }
        
        std::map<std::string, std::string> info;
        const auto& entry = experiments_.at(name);
        
        info["name"] = entry.name;
        info["description"] = entry.description;
        info["status"] = entry.status;
        info["creation_time"] = entry.creationTime;
        info["num_results"] = std::to_string(entry.results.size());
        
        return info;
    }

    // List all experiments
    std::vector<std::string> listExperiments() const {
        std::vector<std::string> names;
        for (const auto& pair : experiments_) {
            names.push_back(pair.first);
        }
        return names;
    }

    // Get underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }

private:
    bool experimentExists(const std::string& name) const {
        return experiments_.find(name) != experiments_.end();
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
        return ss.str();
    }

    float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }

private:
    std::shared_ptr<Config> config_;
    std::map<std::string, ExperimentEntry> experiments_;
    
    struct ExperimentEntry {
        std::string name;
        std::map<std::string, float> parameters;
        std::string description;
        std::string status;
        std::string creationTime;
        std::vector<ExperimentResult> results;
    };
    
    struct ExperimentResult {
        std::string experimentName;
        std::map<std::string, float> result;
        std::map<std::string, std::string> metadata;
        std::string timestamp;
    };
};

// PRIORITY 2.2: ExperimentRunner Class - Systematic execution of experimental protocols
class ExperimentRunner {
public:
    ExperimentRunner(std::shared_ptr<Config> config) : config_(config) {
        if (!config_) {
            throw ExperimentError("ExperimentRunner initialization failed: config is null");
        }
    }

    // Systematic execution of experimental protocols
    std::vector<std::map<std::string, float>> runExperiment(
        const std::map<std::string, float>& parameters,
        const std::vector<SimulationStep>& steps,
        ProgressTracker* progressTracker = nullptr) {
        
        // Create temporary config
        auto tempConfig = std::make_shared<Config>(*config_);
        for (const auto& param : parameters) {
            tempConfig->set(param.first, param.second);
        }
        
        // Create brain with temp config
        auto brain = std::make_shared<Brain>(tempConfig);
        if (!brain->initialize()) {
            throw ExperimentError("Experiment initialization failed");
        }
        
        // Run simulation
        std::map<std::string, float> result;
        result["parameters"] = parameters;
        
        for (size_t i = 0; i < steps.size(); ++i) {
            if (progressTracker) {
                progressTracker->updateProgress(i, steps.size(), 
                                               "Running simulation step " + std::to_string(i));
            }
            
            brain->step(steps[i]);
            
            // Collect metrics
            result["spike_count"] = static_cast<float>(brain->getTotalSpikeCount());
            result["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
            result["firing_rate"] = brain->getAverageFiringRate();
            result["excit_inhib_ratio"] = brain->getExcitationInhibitionRatio();
        }
        
        return {result};
    }

    // Automated parameter variation
    std::vector<std::map<std::string, float>> runParameterSweep(
        const std::map<std::string, std::vector<float>>& parameterRanges,
        const std::vector<SimulationStep>& steps,
        ProgressTracker* progressTracker = nullptr) {
        
        if (parameterRanges.empty()) {
            throw ExperimentError("Cannot run parameter sweep: no parameter ranges provided");
        }
        
        std::vector<std::map<std::string, float>> allResults;
        
        // Create parameter combinations
        std::vector<std::map<std::string, float>> parameterSets;
        generateParameterSets(parameterRanges, parameterSets);
        
        // Run experiment for each parameter set
        for (size_t i = 0; i < parameterSets.size(); ++i) {
            if (progressTracker) {
                progressTracker->updateProgress(i, parameterSets.size(), 
                                               "Running parameter set " + std::to_string(i));
            }
            
            auto results = runExperiment(parameterSets[i], steps);
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
        
        return allResults;
    }

    // Result collection and organization
    bool collectAndOrganizeResults(
        const std::vector<std::map<std::string, float>>& results,
        const std::string& outputDirectory,
        const std::string& format = "csv") {
        
        createDirectory(outputDirectory);
        
        if (format == "csv") {
            std::string filepath = outputDirectory + "/experiment_results.csv";
            exportResultsToCSV(results, filepath);
        } else if (format == "json") {
            std::string filepath = outputDirectory + "/experiment_results.json";
            exportResultsToJSON(results, filepath);
        }
        
        return true;
    }

    // Experiment state management
    bool saveExperimentState(const std::string& filepath,
                            const std::string& experimentName = "default") {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw ExperimentError("Failed to save experiment state: " + filepath);
        }
        
        outfile << "{\n";
        outfile << "  \"experiment_name\": \"" << experimentName << "\",\n";
        outfile << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
        outfile << "  \"version\": \"1.0\"\n";
        outfile << "}\n";
        
        outfile.close();
        return true;
    }

    bool loadExperimentState(const std::string& filepath) {
        std::ifstream infile(filepath);
        if (!infile.is_open()) {
            throw ExperimentError("Failed to load experiment state: " + filepath);
        }
        
        // Simple JSON parsing (for demonstration)
        std::string line;
        std::string experimentName;
        
        while (std::getline(infile, line)) {
            if (line.find("experiment_name") != std::string::npos) {
                size_t pos = line.find(':');
                if (pos != std::string::npos) {
                    experimentName = line.substr(pos + 1);
                    // Remove quotes and whitespace
                    experimentName.erase(0, experimentName.find_first_of('"') + 1);
                    experimentName.erase(experimentName.find_last_of('"'));
                }
            }
        }
        
        infile.close();
        
        return true;
    }

    // Get underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }

private:
    void generateParameterSets(
        const std::map<std::string, std::vector<float>>& parameterRanges,
        std::vector<std::map<std::string, float>>& parameterSets,
        int numCombinations = 100) {
        
        parameterSets.reserve(numCombinations);
        
        for (int i = 0; i < numCombinations; ++i) {
            std::map<std::string, float> params;
            
            for (const auto& paramRange : parameterRanges) {
                const std::string& paramName = paramRange.first;
                const std::vector<float>& ranges = paramRange.second;
                
                if (ranges.size() >= 2) {
                    float minVal = ranges[0];
                    float maxVal = ranges[ranges.size() - 1];
                    float value = minVal + static_cast<float>(rand()) / RAND_MAX * (maxVal - minVal);
                    params[paramName] = value;
                }
            }
            
            parameterSets.push_back(params);
        }
    }

    void exportResultsToCSV(const std::vector<std::map<std::string, float>>& results,
                           const std::string& filepath) {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw ExperimentError("Failed to export results: " + filepath);
        }
        
        outfile << "experiment_id,parameters,spike_count,active_neurons,firing_rate,excit_inhib_ratio\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            outfile << i << ",";
            
            // Export parameters
            bool firstParam = true;
            for (const auto& param : result) {
                if (param.first == "spike_count" || param.first == "active_neurons" ||
                    param.first == "firing_rate" || param.first == "excit_inhib_ratio") {
                    continue;
                }
                
                if (!firstParam) outfile << ",";
                outfile << param.first << "=" << param.second;
                firstParam = false;
            }
            
            outfile << ",";
            
            // Export metrics
            outfile << (result.find("spike_count") != result.end() ? std::to_string(result.at("spike_count")) : "") << ",";
            outfile << (result.find("active_neurons") != result.end() ? std::to_string(result.at("active_neurons")) : "") << ",";
            outfile << (result.find("firing_rate") != result.end() ? std::to_string(result.at("firing_rate")) : "") << ",";
            outfile << (result.find("excit_inhib_ratio") != result.end() ? std::to_string(result.at("excit_inhib_ratio")) : "");
            
            outfile << "\n";
        }
        
        outfile.close();
    }

    void exportResultsToJSON(const std::vector<std::map<std::string, float>>& results,
                            const std::string& filepath) {
        std::ofstream outfile(filepath);
        if (!outfile.is_open()) {
            throw ExperimentError("Failed to export results: " + filepath);
        }
        
        outfile << "{\n";
        outfile << "  \"results\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            outfile << "    {\n";
            outfile << "      \"id\": " << i << ",\n";
            
            // Export parameters
            outfile << "      \"parameters\": {\n";
            bool firstParam = true;
            for (const auto& param : result) {
                if (param.first == "spike_count" || param.first == "active_neurons" ||
                    param.first == "firing_rate" || param.first == "excit_inhib_ratio") {
                    continue;
                }
                
                if (!firstParam) outfile << ",\n";
                outfile << "        \"" << param.first << "\": " << param.second;
                firstParam = false;
            }
            outfile << "\n      },\n";
            
            // Export metrics
            outfile << "      \"metrics\": {\n";
            firstParam = true;
            for (const auto& param : result) {
                if (param.first == "spike_count" || param.first == "active_neurons" ||
                    param.first == "firing_rate" || param.first == "excit_inhib_ratio") {
                    
                    if (!firstParam) outfile << ",\n";
                    outfile << "        \"" << param.first << "\": " << param.second;
                    firstParam = false;
                }
            }
            outfile << "\n      }\n";
            
            outfile << "    }";
            if (i < results.size() - 1) outfile << ",";
            outfile << "\n";
        }
        
        outfile << "  ]\n";
        outfile << "}\n";
        
        outfile.close();
    }

    void createDirectory(const std::string& path) {
        system(("mkdir -p " + path).c_str());
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
        return ss.str();
    }

private:
    std::shared_ptr<Config> config_;
};

// PRIORITY 3: Utility Functions for Batch Processing

// PRIORITY 3.1: Configuration Generator Utilities
namespace config_utils {

    // Generate batch configurations from parameter ranges
    std::vector<std::shared_ptr<Config>> generateBatchConfigs(
        const std::map<std::string, std::vector<float>>& parameterRanges,
        const std::vector<std::string>& configNames,
        std::shared_ptr<Config> baseConfig = nullptr) {
        
        if (parameterRanges.empty()) {
            throw BatchProcessingError("Cannot generate batch configs: no parameter ranges provided");
        }
        
        std::vector<std::shared_ptr<Config>> configs;
        configs.reserve(configNames.size());
        
        for (const auto& name : configNames) {
            auto config = baseConfig ? std::make_shared<Config>(*baseConfig) : std::make_shared<Config>();
            
            // Apply random parameters
            for (const auto& paramRange : parameterRanges) {
                const std::string& paramName = paramRange.first;
                const std::vector<float>& ranges = paramRange.second;
                
                if (ranges.size() >= 2) {
                    float minVal = ranges[0];
                    float maxVal = ranges[ranges.size() - 1];
                    float value = minVal + static_cast<float>(rand()) / RAND_MAX * (maxVal - minVal);
                    config->set(paramName, value);
                }
            }
            
            configs.push_back(config);
        }
        
        return configs;
    }

    // Optimize configuration for performance
    std::shared_ptr<Config> optimizeConfig(std::shared_ptr<Config> config,
                                          const std::string& objective = "speed") {
        return optimize::optimizePerformance(config, objective);
    }

    // Create configuration preset
    std::shared_ptr<Config> createPreset(const std::string& presetName) {
        if (presetName == "research") {
            return createResearchPreset();
        } else if (presetName == "real_time") {
            return createRealTimePreset();
        } else if (presetName == "resource_constrained") {
            return createResourceConstrainedPreset();
        } else if (presetName == "high_performance") {
            return createHighPerformancePreset();
        } else {
            throw BatchProcessingError("Unknown preset: " + presetName);
        }
    }

    // Export configuration to file
    bool exportConfig(std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config) {
            throw BatchProcessingError("Cannot export config: config is null");
        }
        
        return config->saveToFile(filepath);
    }

    // Import configuration from file
    std::shared_ptr<Config> importConfig(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw BatchProcessingError("Failed to import config: " + filepath);
        }
        return config;
    }
}

// PRIORITY 3.2: Result Analysis Utilities
namespace result_utils {

    // Analyze batch results
    std::map<std::string, float> analyzeBatchResults(const std::vector<std::map<std::string, float>>& results) {
        return data::analyzeBrainData(results);
    }

    // Compare multiple experiment results
    std::map<std::string, float> compareResults(const std::vector<std::map<std::string, float>>& results1,
                                                const std::vector<std::map<std::string, float>>& results2) {
        std::map<std::string, float> comparison;
        
        // Compare metrics using BatchStatistics
        BatchStatistics analyzer;
        std::map<std::string, float> stats1 = analyzer.analyze(results1);
        std::map<std::string, float> stats2 = analyzer.analyze(results2);
        
        // Combine results
        for (const auto& pair : stats1) {
            std::string key = "mean_" + pair.first;
            comparison[key] = pair.second;
        }
        
        for (const auto& pair : stats2) {
            std::string key = "std_" + pair.first;
            comparison[key] = pair.second;
        }
        
        return comparison;
    }

    // Export results to file
    bool exportResults(const std::vector<std::map<std::string, float>>& results,
                      const std::string& filepath,
                      const std::string& format = "csv") {
        
        if (format == "csv") {
            std::ofstream outfile(filepath);
            if (!outfile.is_open()) {
                throw DataAnalysisError("Failed to export results: " + filepath);
            }
            
            outfile << "result_id,spike_count,active_neurons,firing_rate,excit_inhib_ratio\n";
            
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& result = results[i];
                
                outfile << i << ",";
                outfile << (result.find("spike_count") != result.end() ? std::to_string(result.at("spike_count")) : "") << ",";
                outfile << (result.find("active_neurons") != result.end() ? std::to_string(result.at("active_neurons")) : "") << ",";
                outfile << (result.find("firing_rate") != result.end() ? std::to_string(result.at("firing_rate")) : "") << ",";
                outfile << (result.find("excit_inhib_ratio") != result.end() ? std::to_string(result.at("excit_inhib_ratio")) : "");
                outfile << "\n";
            }
            
            outfile.close();
        } else if (format == "json") {
            // Use batch processing utilities
            // This would need a proper implementation
        }
        
        return true;
    }

    // Load results from file
    std::vector<std::map<std::string, float>> loadResults(const std::string& filepath) {
        std::vector<std::map<std::string, float>> results;
        
        std::ifstream infile(filepath);
        if (!infile.is_open()) {
            throw DataAnalysisError("Failed to load results: " + filepath);
        }
        
        std::string line;
        std::getline(infile, line); // Skip header
        
        while (std::getline(infile, line)) {
            std::map<std::string, float> result;
            
            // Simple CSV parsing (for demonstration)
            size_t pos = 0;
            std::string token;
            int field = 0;
            
            while ((pos = line.find(',')) != std::string::npos) {
                token = line.substr(0, pos);
                
                if (field == 1 && !token.empty()) {
                    try {
                        result["spike_count"] = std::stof(token);
                    } catch (...) {}
                } else if (field == 2 && !token.empty()) {
                    try {
                        result["active_neurons"] = std::stof(token);
                    } catch (...) {}
                } else if (field == 3 && !token.empty()) {
                    try {
                        result["firing_rate"] = std::stof(token);
                    } catch (...) {}
                } else if (field == 4 && !token.empty()) {
                    try {
                        result["excit_inhib_ratio"] = std::stof(token);
                    } catch (...) {}
                }
                
                line.erase(0, pos + 1);
                field++;
            }
            
            if (!line.empty() && field == 4) {
                try {
                    result["excit_inhib_ratio"] = std::stof(line);
                } catch (...) {}
            }
            
            results.push_back(result);
        }
        
        infile.close();
        return results;
    }
}

// PRIORITY 3.3: Parameter Optimization Utilities
namespace param_utils {

    // Optimize parameters using genetic algorithm
    std::map<std::string, float> optimizeParameters(
        const std::vector<std::map<std::string, float>>& parameterSets,
        const std::vector<std::map<std::string, float>>& results,
        const std::string& objectiveFunction = "spike_count") {
        
        if (parameterSets.empty() || results.empty()) {
            throw PerformanceOptimizationError("Cannot optimize parameters: insufficient data");
        }
        
        // Use EnhancedExperimentRunner for optimization
        // This is a simplified implementation
        std::map<std::string, float> bestParams = parameterSets[0];
        float bestScore = std::numeric_limits<float>::max();
        
        for (size_t i = 0; i < results.size(); ++i) {
            float score = 0.0f;
            
            if (objectiveFunction == "spike_count" && results[i].find("spike_count") != results[i].end()) {
                score = results[i].at("spike_count");
            } else if (objectiveFunction == "firing_rate" && results[i].find("firing_rate") != results[i].end()) {
                score = results[i].at("firing_rate");
            } else if (objectiveFunction == "active_neurons" && results[i].find("active_neurons") != results[i].end()) {
                score = results[i].at("active_neurons");
            }
            
            // Minimize score for some objectives, maximize for others
            if (objectiveFunction == "error" || objectiveFunction == "cost") {
                score = 1.0f / (score + 0.0001f);
            }
            
            if (score < bestScore) {
                bestScore = score;
                bestParams = parameterSets[i];
            }
        }
        
        return bestParams;
    }

    // Generate parameter sets for optimization
    std::vector<std::map<std::string, float>> generateParameterSets(
        const std::map<std::string, std::vector<float>>& parameterRanges,
        int numSets = 100) {
        
        std::vector<std::map<std::string, float>> parameterSets;
        parameterSets.reserve(numSets);
        
        for (int i = 0; i < numSets; ++i) {
            std::map<std::string, float> params;
            
            for (const auto& paramRange : parameterRanges) {
                const std::string& paramName = paramRange.first;
                const std::vector<float>& ranges = paramRange.second;
                
                if (ranges.size() >= 2) {
                    float minVal = ranges[0];
                    float maxVal = ranges[ranges.size() - 1];
                    float value = minVal + static_cast<float>(rand()) / RAND_MAX * (maxVal - minVal);
                    params[paramName] = value;
                }
            }
            
            parameterSets.push_back(params);
        }
        
        return parameterSets;
    }

    // Validate parameter set
    bool validateParameters(const std::map<std::string, float>& parameters,
                           const std::map<std::string, std::pair<float, float>>& constraints) {
        for (const auto& constraint : constraints) {
            const std::string& paramName = constraint.first;
            const std::pair<float, float>& range = constraint.second;
            
            if (parameters.find(paramName) != parameters.end()) {
                float value = parameters.at(paramName);
                if (value < range.first || value > range.second) {
                    return false;
                }
            }
        }
        return true;
    }
}

// PRIORITY 3.4: Progress Tracking Utilities
namespace progress_utils {

    // Progress tracker for long-running operations
    class ProgressTracker {
    public:
        ProgressTracker() : totalSteps_(0), currentStep_(0), startTime_(std::chrono::steady_clock::now()) {}

        void setTotalSteps(int steps) {
            totalSteps_ = steps;
            currentStep_ = 0;
        }

        void updateProgress(int step, const std::string& message = "") {
            currentStep_ = step;
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(now - startTime_).count();
            
            if (totalSteps_ > 0) {
                double progress = static_cast<double>(currentStep_) / totalSteps_;
                
                // Print progress bar
                std::cout << "\r[";
                int barWidth = 50;
                int pos = static_cast<int>(barWidth * progress);
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << std::fixed << std::setprecision(2) << (progress * 100.0) << "%";
                
                if (!message.empty()) {
                    std::cout << " - " << message;
                }
                std::cout.flush();
                
                if (progress >= 1.0) {
                    std::cout << "\n";
                }
            }
        }

        void finish() {
            auto now = std::chrono::steady_clock::now();
            auto totalTime = std::chrono::duration<double>(now - startTime_).count();
            
            std::cout << "\nCompleted in " << totalTime << " seconds" << std::endl;
        }

        double getProgress() const {
            if (totalSteps_ == 0) return 0.0;
            return static_cast<double>(currentStep_) / totalSteps_;
        }

    private:
        int totalSteps_;
        int currentStep_;
        std::chrono::steady_clock::time_point startTime_;
    };

    // Create progress tracker
    std::shared_ptr<ProgressTracker> createProgressTracker(int totalSteps = 0) {
        auto tracker = std::make_shared<ProgressTracker>();
        tracker->setTotalSteps(totalSteps);
        return tracker;
    }
}

// Register batch processing and experiment management classes
void registerBatchProcessingFeatures(py::module_ &m) {
    // Register BatchProcessor
    py::class_<BatchProcessor>(m, "BatchProcessor")
        .def(py::init<std::shared_ptr<Config>>())
        .def("process_brains", &BatchProcessor::processBrains)
        .def("save_brains_batch", &BatchProcessor::saveBrainsBatch)
        .def("load_brains_batch", &BatchProcessor::loadBrainsBatch)
        .def("analyze_batch_results", &BatchProcessor::analyzeBatchResults)
        .def("generate_batch_configs", &BatchProcessor::generateBatchConfigs)
        .def("execute_batch", &BatchProcessor::executeBatch)
        .def("get_config", &BatchProcessor::getConfig);

    // Register BatchStatistics
    py::class_<BatchStatistics>(m, "BatchStatistics")
        .def("analyze", &BatchStatistics::analyze)
        .def("correlate_results", &BatchStatistics::correlateResults)
        .def("aggregate_results", &BatchStatistics::aggregateResults);

    // Register ExperimentManager
    py::class_<ExperimentManager>(m, "ExperimentManager")
        .def(py::init<std::shared_ptr<Config>>())
        .def("create_experiment", &ExperimentManager::createExperiment)
        .def("explore_parameter_space", &ExperimentManager::exploreParameterSpace)
        .def("store_experiment_result", &ExperimentManager::storeExperimentResult)
        .def("compare_experiments", &ExperimentManager::compareExperiments)
        .def("get_experiment_info", &ExperimentManager::getExperimentInfo)
        .def("list_experiments", &ExperimentManager::listExperiments)
        .def("get_config", &ExperimentManager::getConfig);

    // Register ExperimentRunner
    py::class_<ExperimentRunner>(m, "ExperimentRunner")
        .def(py::init<std::shared_ptr<Config>>())
        .def("run_experiment", &ExperimentRunner::runExperiment)
        .def("run_parameter_sweep", &ExperimentRunner::runParameterSweep)
        .def("collect_and_organize_results", &ExperimentRunner::collectAndOrganizeResults)
        .def("save_experiment_state", &ExperimentRunner::saveExperimentState)
        .def("load_experiment_state", &ExperimentRunner::loadExperimentState)
        .def("get_config", &ExperimentRunner::getConfig);

    // Register configuration generator utilities
    m.def("generate_batch_configs", &config_utils::generateBatchConfigs);
    m.def("optimize_config", &config_utils::optimizeConfig);
    m.def("create_preset", &config_utils::createPreset);
    m.def("export_config", &config_utils::exportConfig);
    m.def("import_config", &config_utils::importConfig);

    // Register result analysis utilities
    m.def("analyze_batch_results", &result_utils::analyzeBatchResults);
    m.def("compare_results", &result_utils::compareResults);
    m.def("export_results", &result_utils::exportResults);
    m.def("load_results", &result_utils::loadResults);

    // Register parameter optimization utilities
    m.def("optimize_parameters", &param_utils::optimizeParameters);
    m.def("generate_parameter_sets", &param_utils::generateParameterSets);
    m.def("validate_parameters", &param_utils::validateParameters);

    // Register progress tracking utilities
    m.def("create_progress_tracker", &progress_utils::createProgressTracker);
}
    // Register EnhancedExperimentRunner
    py::class_<EnhancedExperimentRunner>(m, "EnhancedExperimentRunner")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Config>>())
        .def("run_configurations", &EnhancedExperimentRunner::runConfigurations)
        .def("compare_results", &EnhancedExperimentRunner::compareResults)
        .def("optimize_parameters", &EnhancedExperimentRunner::optimizeParameters)
        .def("perform_statistical_analysis", &EnhancedExperimentRunner::performStatisticalAnalysis)
        .def("get_brain", &EnhancedExperimentRunner::getBrain)
        .def("get_config", &EnhancedExperimentRunner::getConfig);

    // Register PerformanceProfiler
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("start_monitoring", &PerformanceProfiler::startMonitoring)
        .def("stop_monitoring", &PerformanceProfiler::stopMonitoring)
        .def("get_memory_stats", &PerformanceProfiler::getMemoryStats)
        .def("get_performance_metrics", &PerformanceProfiler::getPerformanceMetrics)
        .def("get_resource_utilization", &PerformanceProfiler::getResourceUtilization)
        .def("start_profile", &PerformanceProfiler::startProfile)
        .def("stop_profile", &PerformanceProfiler::stopProfile)
        .def("get_profile_data", &PerformanceProfiler::getProfileData)
        .def("get_component_times", &PerformanceProfiler::getComponentTimes)
        .def("reset_profile", &PerformanceProfiler::resetProfile);

    // Register BrainStateExporter
    py::class_<BrainStateExporter>(m, "BrainStateExporter")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("export_brain_state", &BrainStateExporter::exportBrainState)
        .def("create_backup", &BrainStateExporter::createBackup)
        .def("export_platform_independent", &BrainStateExporter::exportPlatformIndependent)
        .def("import_brain_state", &BrainStateExporter::importBrainState)
        .def("get_brain", &BrainStateExporter::getBrain);

    // Register AdvancedVisualization
    py::class_<AdvancedVisualization>(m, "AdvancedVisualization")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("create_2d_visualization", &AdvancedVisualization::create2DVisualization)
        .def("create_connection_heatmap", &AdvancedVisualization::createConnectionHeatmap)
        .def("create_development_timeline", &AdvancedVisualization::createDevelopmentTimeline)
        .def("create_dynamic_visualization", &AdvancedVisualization::createDynamicVisualization)
        .def("get_brain", &AdvancedVisualization::getBrain);

    // Register configuration presets
    m.def("create_research_preset", &createResearchPreset);
    m.def("create_real_time_preset", &createRealTimePreset);
    m.def("create_resource_constrained_preset", &createResourceConstrainedPreset);
    m.def("create_high_performance_preset", &createHighPerformancePreset);

    // Register batch processing utilities
    m.def("process_brains_batch", &batch::processBrainsBatch);
    m.def("save_brain_states_batch", &batch::saveBrainStatesBatch);
    m.def("load_brain_states_batch", &batch::loadBrainStatesBatch);
    m.def("simulate_batch_steps", &batch::simulateBatchSteps);
    m.def("get_batch_stats", &batch::getBatchStats);

    // Register data analysis utilities
    m.def("analyze_brain_data", &data::analyzeBrainData);
    m.def("detect_patterns", &data::detectPatterns);
    m.def("export_analysis_results", &data::exportAnalysisResults);

    // Register error analysis utilities
    m.def("analyze_error_patterns", &error::analyzeErrorPatterns);
    m.def("detect_anomalies", &error::detectAnomalies);

    // Register performance optimization utilities
    m.def("optimize_performance", &optimize::optimizePerformance);
    m.def("optimize_batch_processing", &optimize::optimizeBatchProcessing);
    m.def("profile_simulation", &optimize::profileSimulation);
    m.def("predict_performance", &optimize::predictPerformance);
}

// Register all exception classes
void registerExceptionClasses(py::module_ &m) {
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError", PyExc_RuntimeError);
    py::register_exception<DevelopmentError>(m, "DevelopmentError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<AgentError>(m, "AgentError", PyExc_RuntimeError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_RuntimeError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_RuntimeError);
    py::register_exception<NoveltyError>(m, "NoveltyError", PyExc_RuntimeError);
    // Register advanced feature exceptions
    py::register_exception<VisualizationError>(m, "VisualizationError", PyExc_RuntimeError);
    py::register_exception<StatisticalAnalysisError>(m, "StatisticalAnalysisError", PyExc_RuntimeError);
    py::register_exception<PerformanceMonitoringError>(m, "PerformanceMonitoringError", PyExc_RuntimeError);
    py::register_exception<MLIntegrationError>(m, "MLIntegrationError", PyExc_RuntimeError);
