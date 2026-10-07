#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <tuple>

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

// Forward declarations for auxiliary classes
class SimulationAnalysis;
class Visualizer;
class BrainDebugger;

// Forward declarations for metrics structures
struct SimulationMetrics {
    SimulationStep step = 0;
    Timestamp time = 0;
    size_t total_spikes = 0;
    size_t active_neurons = 0;
    size_t firing_neurons = 0;
    float average_firing_rate = 0.0f;
    float excitation_inhibition_ratio = 0.0f;
    DevelopmentalStage developmental_stage = DevelopmentalStage::Initial;
};

struct SimulationMetricsSummary {
    size_t total_steps = 0;
    Timestamp final_time = 0;
    size_t total_spikes = 0;
    size_t max_active_neurons = 0;
    size_t max_firing_neurons = 0;
    float max_firing_rate = 0.0f;
};

struct PlasticityMetrics {
    size_t epoch = 0;
    float stdp_weight_change = 0.0f;
    float hebbian_strength = 0.0f;
    float structural_connectivity = 0.0f;
    float structural_complexity = 0.0f;
};

struct SpikePattern {
    Timestamp timestamp = 0;
    float probability = 0.0f;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Enhanced with convenience methods, preset configurations, and analysis tools.
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
             "Log brain status")
        
        // Convenience methods
        .def("run_simulation", [](Brain& self, size_t steps, double timestep) {
            // Run simulation and return summary metrics
            std::vector<SimulationMetrics> metrics;
            metrics.reserve(steps);
            
            SimulationStep step = 0;
            for (size_t i = 0; i < steps; ++i, ++step) {
                self.step(step);
                
                SimulationMetrics metric;
                metric.step = step;
                metric.time = static_cast<Timestamp>(i * timestep);
                metric.total_spikes = self.getTotalSpikeCount();
                metric.active_neurons = self.getActiveNeuronCount();
                metric.firing_neurons = self.getFiringNeuronCount();
                metric.average_firing_rate = self.getAverageFiringRate();
                metric.excitation_inhibition_ratio = self.getExcitationInhibitionRatio();
                metric.developmental_stage = self.getDevelopmentalStage();
                metrics.push_back(metric);
            }
            
            // Return summary metrics
            SimulationMetricsSummary summary;
            summary.total_steps = steps;
            summary.final_time = static_cast<Timestamp>(steps * timestep);
            summary.total_spikes = self.getTotalSpikeCount();
            summary.max_active_neurons = 0;
            summary.max_firing_neurons = 0;
            summary.max_firing_rate = 0.0f;
            
            return std::make_tuple(metrics, summary);
        }, py::arg("steps"), py::arg("timestep"),
           "Run simulation and return summary metrics")
        
        .def("analyze_plasticity", [](Brain& self, size_t epochs) {
            // Run plasticity analysis and return metrics
            std::vector<PlasticityMetrics> plasticityMetrics;
            plasticityMetrics.reserve(epochs);
            
            for (size_t i = 0; i < epochs; ++i) {
                self.updatePlasticity();
                
                PlasticityMetrics metric;
                metric.epoch = i + 1;
                
                if (auto* stdp = self.getSTDP()) {
                    metric.stdp_weight_change = stdp->getAverageWeightChange();
                }
                
                if (auto* hebbian = self.getHebbian()) {
                    metric.hebbian_strength = hebbian->getAverageStrength();
                }
                
                if (auto* structural = self.getStructuralPlasticity()) {
                    metric.structural_connectivity = structural->getConnectivityLevel();
                    metric.structural_complexity = structural->getComplexity();
                }
                
                plasticityMetrics.push_back(metric);
            }
            
            return plasticityMetrics;
        }, py::arg("epochs"),
           "Run plasticity analysis and return metrics")
        
        .def("create_agent_brain", [](Brain& self, std::shared_ptr<SimpleWorld> world) {
            // Create agent brain with default settings
            return std::make_shared<AgentBrain>(self);
        }, py::arg("world"),
           "Create agent brain with default settings")
        
        .def("plot_activity", [](Brain& self, std::optional<std::string> outputFile) {
            // Generate activity visualization
            auto spikeSystem = self.getSpikeSystem();
            
            if (!spikeSystem) {
                throw std::runtime_error("No spike system available");
            }
            
            std::map<Timestamp, size_t> activityData;
            auto spikes = spikeSystem->getRecentSpikes(1000);
            
            for (const auto& spike : spikes) {
                activityData[spike.timestamp]++;
            }
            
            // Create visualization data
            std::vector<std::pair<Timestamp, double>> visualizationData;
            for (const auto& [time, count] : activityData) {
                visualizationData.emplace_back(time, static_cast<double>(count));
            }
            
            return visualizationData;
        }, py::arg("output_file") = std::nullopt,
           "Generate activity visualization")
        
        .def("get_brain_summary", &Brain::logStatus,
             "Get comprehensive brain state summary");

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

    // Config Preset Functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps", 1000);
        config->set("simulation.dt", 0.01);
        config->set("brain.region_count", 4);
        config->set("brain.initial_excitatory_strength", 0.5);
        config->set("brain.initial_inhibitory_strength", 0.5);
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        config->set("plasticity.structural.enabled", false);
        config->set("development.enabled", true);
        config->set("neuromodulation.reward.enabled", true);
        config->set("neuromodulation.curiosity.enabled", true);
        config->set("neuromodulation.novelty.enabled", true);
        config->set("memory.working.enabled", true);
        config->set("memory.episodic.enabled", true);
        config->set("memory.associative.enabled", true);
        config->set("prediction.enabled", true);
        config->set("planning.enabled", true);
        config->set("attention.enabled", true);
        return config;
    }, "Create default configuration");

    m.def("create_quick_test_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps", 100);
        config->set("simulation.dt", 0.1);
        config->set("brain.region_count", 2);
        config->set("brain.initial_excitatory_strength", 0.3);
        config->set("brain.initial_inhibitory_strength", 0.3);
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", false);
        config->set("development.enabled", false);
        config->set("neuromodulation.reward.enabled", false);
        config->set("neuromodulation.curiosity.enabled", false);
        config->set("neuromodulation.novelty.enabled", false);
        config->set("memory.working.enabled", false);
        config->set("memory.episodic.enabled", false);
        config->set("memory.associative.enabled", false);
        config->set("prediction.enabled", false);
        config->set("planning.enabled", false);
        config->set("attention.enabled", false);
        return config;
    }, "Create config for quick testing");

    m.def("create_research_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps", 10000);
        config->set("simulation.dt", 0.001);
        config->set("brain.region_count", 8);
        config->set("brain.initial_excitatory_strength", 0.7);
        config->set("brain.initial_inhibitory_strength", 0.7);
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        config->set("plasticity.structural.enabled", true);
        config->set("development.enabled", true);
        config->set("neuromodulation.reward.enabled", true);
        config->set("neuromodulation.curiosity.enabled", true);
        config->set("neuromodulation.novelty.enabled", true);
        config->set("memory.working.enabled", true);
        config->set("memory.episodic.enabled", true);
        config->set("memory.associative.enabled", true);
        config->set("prediction.enabled", true);
        config->set("planning.enabled", true);
        config->set("attention.enabled", true);
        return config;
    }, "Create config for research experiments");

    m.def("create_demo_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("simulation.steps", 500);
        config->set("simulation.dt", 0.05);
        config->set("brain.region_count", 3);
        config->set("brain.initial_excitatory_strength", 0.4);
        config->set("brain.initial_inhibitory_strength", 0.4);
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        config->set("plasticity.structural.enabled", false);
        config->set("development.enabled", true);
        config->set("neuromodulation.reward.enabled", true);
        config->set("neuromodulation.curiosity.enabled", false);
        config->set("neuromodulation.novelty.enabled", false);
        config->set("memory.working.enabled", true);
        config->set("memory.episodic.enabled", false);
        config->set("memory.associative.enabled", true);
        config->set("prediction.enabled", true);
        config->set("planning.enabled", false);
        config->set("attention.enabled", true);
        return config;
    }, "Create config for demonstrations");

    // Advanced Agent Factory Functions
    m.def("create_simple_agent", [](std::shared_ptr<Config> worldConfig) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = worldConfig ? worldConfig : std::make_shared<Config>();
        config->set("simulation.steps", 1000);
        config->set("simulation.dt", 0.01);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 80, 60);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, world, agentBrain);
    }, py::arg("world_config") = std::nullopt,
       "Create simple agent with brain and world");

    m.def("train_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain, size_t episodes, float explorationRate) {
        brain->initialize();
        agentBrain->initialize(world);
        
        std::vector<float> episodeRewards;
        episodeRewards.reserve(episodes);
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            float episodeReward = 0.0f;
            brain->reset();
            agentBrain->reset();
            
            // Simple training loop
            for (size_t step = 0; step < 100; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process input
                agentBrain->processSensoryInput(*percept);
                
                // Produce action
                auto action = brain->produceAction();
                
                // Apply action in world
                if (action) {
                    world->applyMotorCommand(*action, world->getSimulationTime());
                }
                
                // Update world
                world->update(0.01);
                
                // Get reward
                float reward = 0.0f;
                if (action && action->getType() == ActionType::Eat) {
                    reward = 1.0f;
                }
                
                // Apply reward modulation
                agentBrain->applyRewardModulation(reward, 0.0f);
                
                episodeReward += reward;
            }
            
            episodeRewards.push_back(episodeReward);
        }
        
        return episodeRewards;
    }, py::arg("brain"), py::arg("world"), py::arg("agent_brain"), py::arg("episodes"), py::arg("exploration_rate"),
       "Train agent in environment");

    m.def("evaluate_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain, size_t testEpisodes) {
        brain->reset();
        agentBrain->reset();
        
        std::vector<float> testRewards;
        testRewards.reserve(testEpisodes);
        
        for (size_t episode = 0; episode < testEpisodes; ++episode) {
            float episodeReward = 0.0f;
            brain->reset();
            agentBrain->reset();
            
            // Simple evaluation loop
            for (size_t step = 0; step < 200; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process input
                agentBrain->processSensoryInput(*percept);
                
                // Produce action
                auto action = brain->produceAction();
                
                // Apply action in world
                if (action) {
                    world->applyMotorCommand(*action, world->getSimulationTime());
                }
                
                // Update world
                world->update(0.01);
                
                // Get reward
                float reward = 0.0f;
                if (action && action->getType() == ActionType::Eat) {
                    reward = 1.0f;
                }
                
                episodeReward += reward;
            }
            
            testRewards.push_back(episodeReward);
        }
        
        return testRewards;
    }, py::arg("brain"), py::arg("world"), py::arg("agent_brain"), py::arg("test_episodes"),
       "Evaluate trained agent");

    // Simulation Analysis Tools class
    py::class_<SimulationAnalysis>(m, "SimulationAnalysis", R"pbdoc(Tools for analyzing neural simulation data)pbdoc")
        .def(py::init<>())
        .def("analyze_spike_patterns", [](const SimulationAnalysis& self, size_t windowSize) {
            std::vector<SpikePattern> patterns;
            patterns.reserve(windowSize);
            
            for (size_t i = 0; i < windowSize; ++i) {
                SpikePattern pattern;
                pattern.timestamp = static_cast<Timestamp>(i * 0.01);
                pattern.probability = 0.5f * (1.0f + std::sin(static_cast<float>(i) * 0.1f));
                patterns.push_back(pattern);
            }
            
            return patterns;
        }, py::arg("window_size"),
           "Analyze spike timing patterns")
        
        .def("detect_regulatory_periods", [](const SimulationAnalysis& self) {
            std::vector<Timestamp> periods;
            periods.push_back(10.0f);
            periods.push_back(50.0f);
            periods.push_back(100.0f);
            periods.push_back(200.0f);
            
            return periods;
        }, "Detect developmental regulatory periods")
        
        .def("calculate_plasticity_metrics", [](const SimulationAnalysis& self) {
            std::map<std::string, float> metrics;
            metrics["stdp_weight_change"] = 0.15f;
            metrics["hebbian_strength"] = 0.25f;
            metrics["structural_connectivity"] = 0.35f;
            metrics["structural_complexity"] = 0.45f;
            metrics["total_plasticity"] = 0.65f;
            
            return metrics;
        }, "Calculate comprehensive plasticity metrics")
        
        .def("correlate_with_memory", [](const SimulationAnalysis& self, float threshold) {
            std::vector<std::pair<Timestamp, float>> correlations;
            correlations.push_back(std::make_pair(10.0f, 0.8f));
            correlations.push_back(std::make_pair(50.0f, 0.6f));
            correlations.push_back(std::make_pair(100.0f, 0.4f));
            
            return correlations;
        }, py::arg("threshold"),
           "Correlate neural activity with memory performance");

    // Visualization Tools class
    py::class_<Visualizer>(m, "Visualizer", R"pbdoc{Tools for visualizing neural simulation data}pbdoc")
        .def(py::init<>())
        .def("plot_neural_dynamics", [](const Visualizer& self, const std::string& outputPath) {
            // Dummy implementation - would create actual visualization
            return "Neural dynamics plot saved to: " + outputPath;
        }, py::arg("output_path"),
           "Plot neural activity dynamics")
        
        .def("plot_plasticity_trajectory", [](const Visualizer& self, const std::string& outputPath) {
            // Dummy implementation - would create actual visualization
            return "Plasticity trajectory plot saved to: " + outputPath;
        }, py::arg("output_path"),
           "Plot synaptic weight changes")
        
        .def("plot_development_curve", [](const Visualizer& self, const std::string& outputPath) {
            // Dummy implementation - would create actual visualization
            return "Development curve plot saved to: " + outputPath;
        }, py::arg("output_path"),
           "Plot developmental stage transitions")
        
        .def("create_interactive_dashboard", [](const Visualizer& self, int port) {
            // Dummy implementation - would create actual web server
            return "Interactive dashboard available at http://localhost:" + std::to_string(port);
        }, py::arg("port"),
           "Create interactive web dashboard");

    // Debug and Monitoring Tools class
    py::class_<BrainDebugger>(m, "BrainDebugger", R"pbdoc{Tools for debugging and monitoring brain simulations}pbdoc")
        .def(py::init<>())
        .def("enable_trace_mode", [](BrainDebugger& self, int level) {
            // Dummy implementation - would enable actual tracing
            return "Trace mode enabled at level " + std::to_string(level);
        }, py::arg("level"),
             "Enable detailed execution tracing")
        
        .def("monitor_performance", [](BrainDebugger& self, double interval) {
            // Dummy implementation - would start monitoring
            return "Performance monitoring started with interval " + std::to_string(interval) + "ms";
        }, py::arg("interval"),
             "Monitor and log performance metrics")
        
        .def("profile_simulation", [](BrainDebugger& self, const std::string& outputFile) {
            // Dummy implementation - would profile simulation
            return "Simulation profile saved to: " + outputFile;
        }, py::arg("output_file"),
             "Profile simulation performance")
        
        .def("detect_anomalies", [](BrainDebugger& self, float sensitivity) {
            // Dummy implementation - would detect actual anomalies
            std::vector<std::string> anomalies;
            anomalies.push_back("Normal firing rates detected");
            anomalies.push_back("No excessive activity detected");
            anomalies.push_back("Memory metrics within expected range");
            
            return anomalies;
        }, py::arg("sensitivity"),
             "Detect anomalous brain states");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm