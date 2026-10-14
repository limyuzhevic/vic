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

    // Advanced analysis and utilities for expert users

    m.def("analyze_network_connectivity", [](const std::shared_ptr<Brain>& brain) {
        struct ConnectivityAnalysis {
            size_t totalNeurons;
            size_t totalSynapses;
            float excitationInhibitionRatio;
            size_t averageConnectionsPerNeuron;
            size_t minConnectionsPerNeuron;
            size_t maxConnectionsPerNeuron;
            float averageWeight;
            float weightStandardDeviation;
        };

        ConnectivityAnalysis analysis;
        analysis.totalNeurons = brain->getTotalNeuronCount();
        analysis.totalSynapses = brain->getTotalSynapseCount();
        analysis.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        if (analysis.totalNeurons > 0) {
            analysis.averageConnectionsPerNeuron = analysis.totalSynapses / analysis.totalNeurons;
        } else {
            analysis.averageConnectionsPerNeuron = 0;
        }
        
        // For more detailed analysis, iterate through regions
        analysis.minConnectionsPerNeuron = SIZE_MAX;
        analysis.maxConnectionsPerNeuron = 0;
        analysis.averageWeight = 0.0f;
        size_t weightCount = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                analysis.averageWeight += weight;
                weightCount++;
            }
        }
        
        if (weightCount > 0) {
            analysis.averageWeight /= weightCount;
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze network connectivity and statistics");

    m.def("analyze_learning_progress", [](const std::shared_ptr<Brain>& brain, 
                                          size_t windowSize) {
        struct LearningAnalysis {
            std::vector<size_t> spikeHistory;
            std::vector<float> firingRates;
            std::vector<float> learningRates;
            float totalLearning;
            size_t learningSteps;
        };

        LearningAnalysis analysis;
        // Get spike count over time (simplified - would need to track history)
        analysis.totalLearning = brain->getTotalSpikeCount() > 0 ? 
            static_cast<float>(brain->getFiringNeuronCount()) / 
            std::max(static_cast<float>(brain->getTotalNeuronCount()), 1.0f) : 0.0f;
        analysis.learningSteps = windowSize;
        
        // Get developmental stage
        auto stage = brain->getDevelopmentalStage();
        analysis.totalLearning += static_cast<float>(static_cast<int>(stage));
        
        return analysis;
    }, py::arg("brain"), py::arg("windowSize") = 100, "Analyze learning progress and patterns");

    m.def("get_activity_statistics", [](const std::shared_ptr<Brain>& brain) {
        struct ActivityStats {
            float averageFiringRate;
            size_t maxFiringNeurons;
            size_t minFiringNeurons;
            float activityVariability;
            size_t spikeCount;
            double averageMembranePotential;
            double membranePotentialVariability;
        };

        ActivityStats stats;
        stats.spikeCount = brain->getTotalSpikeCount();
        stats.maxFiringNeurons = brain->getFiringNeuronCount();
        stats.minFiringNeurons = 0; // Simplified - would need to track history
        
        if (brain->getTotalNeuronCount() > 0) {
            stats.averageFiringRate = brain->getAverageFiringRate();
            stats.activityVariability = stats.maxFiringNeurons > 0 ? 
                1.0f - static_cast<float>(stats.minFiringNeurons) / stats.maxFiringNeurons : 0.0f;
        } else {
            stats.averageFiringRate = 0.0f;
            stats.activityVariability = 0.0f;
        }
        
        // Simplified membrane potential analysis
        stats.averageMembranePotential = -70.0; // Typical resting potential
        stats.membranePotentialVariability = 15.0; // Typical variability
        
        return stats;
    }, py::arg("brain"), "Get detailed activity statistics");

    // Advanced configuration factory methods

    m.def("create_advanced_config", []() {
        auto config = std::make_shared<Config>();
        
        // Advanced configuration for research and experimentation
        config->set("brain.neuron_count", static_cast<int64_t>(5000));
        config->set("brain.region_count", static_cast<int64_t>(3));
        config->set("brain.connection_probability", 0.05f);
        config->set("brain.initial_weight_mean", 0.5f);
        config->set("brain.initial_weight_std", 0.15f);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("brain.tau_ref", 5.0f);
        
        // STDP configuration
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.stdp.tau_plus", 20.0f);
        config->set("plasticity.stdp.tau_minus", 20.0f);
        
        // Hebbian configuration
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.correlation_window", 100.0f);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        // Development configuration
        config->set("development.initial_plasticity", 1.0f);
        config->set("development.critical_period_end", 1000.0f);
        
        // Memory configuration
        config->set("memory.working_capacity", 1000);
        config->set("memory.episodic_max_episodes", 5000);
        
        // Performance configuration
        config->set("performance.checkpoint_interval", 10000);
        config->set("performance.max_memory_usage", 1024);
        
        return config;
    }, "Create an advanced configuration optimized for research");

    m.def("create_research_config", []() {
        auto config = std::make_shared<Config>();
        
        // Research-focused configuration with balanced parameters
        config->set("brain.neuron_count", static_cast<int64_t>(2000));
        config->set("brain.region_count", static_cast<int64_t>(2));
        config->set("brain.connection_probability", 0.08f);
        config->set("brain.initial_weight_mean", 0.3f);
        config->set("brain.initial_weight_std", 0.1f);
        
        // Moderate plasticity for controlled learning
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.002f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Novelty and curiosity for exploration
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 0.8f);
        
        // Longer development for maturity
        config->set("development.initial_plasticity", 1.2f);
        config->set("development.critical_period_end", 2000.0f);
        
        return config;
    }, "Create a research-focused configuration");

    m.def("create_development_config", [](float plasticityRate, float noveltyRate) {
        auto config = std::make_shared<Config>();
        
        // Development-focused configuration
        config->set("brain.neuron_count", static_cast<int64_t>(3000));
        config->set("brain.region_count", static_cast<int64_t>(2));
        config->set("brain.connection_probability", 0.06f);
        
        // High plasticity for development
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.003f * plasticityRate);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Novelty and curiosity
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f * noveltyRate);
        
        // Strong developmental effects
        config->set("development.initial_plasticity", 1.5f);
        config->set("development.critical_period_end", 1500.0f);
        
        return config;
    }, py::arg("plasticityRate") = 1.0f, py::arg("noveltyRate") = 1.0f, 
           "Create a development-focused configuration");

    // Training utilities

    m.def("run_training_loop", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                 std::shared_ptr<AgentBrain> agent, size_t numEpisodes,
                                 size_t maxStepsPerEpisode, float timestep) {
        struct TrainingResults {
            std::vector<size_t> episodeLengths;
            std::vector<float> episodeRewards;
            std::vector<float> avgFiringRates;
            std::vector<size_t> finalNeuronCounts;
        };

        TrainingResults results;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            // Reset environment and agent
            world->reset();
            agent->reset();
            agent->initialize(*world);
            
            float episodeReward = 0.0f;
            size_t stepsCompleted = 0;
            
            for (size_t step = 0; step < maxStepsPerEpisode; ++step) {
                // Update world
                world->update(timestep);
                
                // Get sensory input and process
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Get action and apply
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Get reward and apply neuromodulation
                float reward = 0.0f;
                if (auto& internalSignals = percept.getInternal(); !internalSignals.empty()) {
                    reward = internalSignals[0];
                }
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(timestep);
                
                episodeReward += reward;
                stepsCompleted++;
                
                // Check if episode ended
                if (world->getSimulationTime() >= maxStepsPerEpisode * timestep) {
                    break;
                }
            }
            
            // Store results
            results.episodeLengths.push_back(stepsCompleted);
            results.episodeRewards.push_back(episodeReward);
            results.avgFiringRates.push_back(brain->getAverageFiringRate());
            results.finalNeuronCounts.push_back(brain->getTotalNeuronCount());
        }
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
           py::arg("numEpisodes"), py::arg("maxStepsPerEpisode"), py::arg("timestep") = 0.1f,
           "Run a complete training loop and return results");

    m.def("save_config_to_file", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        return config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Save configuration to a file");

    m.def("load_config_from_file", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from a file");

    // Helper functions for advanced users

    m.def("create_experiment_setup", [](const std::string& experimentType) {
        struct ExperimentSetup {
            std::shared_ptr<Config> config;
            std::shared_ptr<Brain> brain;
            std::shared_ptr<SimpleWorld> world;
            std::shared_ptr<AgentBrain> agent;
            std::string type;
        };

        ExperimentSetup setup;
        setup.type = experimentType;
        
        // Create basic setup
        setup.config = std::make_shared<Config>();
        setup.brain = std::make_shared<Brain>(setup.config);
        setup.world = std::make_shared<SimpleWorld>();
        setup.agent = std::make_shared<AgentBrain>(setup.brain);
        
        // Configure based on experiment type
        if (experimentType == "basic_learning") {
            setup.config->set("brain.neuron_count", static_cast<int64_t>(1000));
            setup.config->set("brain.region_count", static_cast<int64_t>(1));
            setup.config->set("brain.connection_probability", 0.1f);
            setup.agent->enableRewardModulation(true);
            setup.agent->enableCuriosity(true);
        } else if (experimentType == "structural_plasticity") {
            setup.config->set("brain.neuron_count", static_cast<int64_t>(2000));
            setup.config->set("brain.region_count", static_cast<int64_t>(2));
            setup.config->set("brain.connection_probability", 0.05f);
            setup.agent->enableStructuralPlasticity(true);
            setup.agent->enableDevelopment(true);
        } else if (experimentType == "advanced_learning") {
            setup.config = pynlm.create_advanced_config();
            setup.agent->enableRewardModulation(true);
            setup.agent->enableStructuralPlasticity(true);
            setup.agent->enableCuriosity(true);
            setup.agent->enableDevelopment(true);
        }
        
        // Initialize components
        setup.brain->initialize();
        setup.world->configure(20, 20, 8, 8);
        setup.world->reset();
        setup.agent->initialize(*setup.world);
        
        return setup;
    }, py::arg("experimentType"), "Create a complete experiment setup with brain, world, and agent");

    // Version and information
    m.attr("__version__") = "6.0.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) - Advanced Neural Simulation Framework";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__date__") = "2026";
}

} // namespace nlm
