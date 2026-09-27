#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>
#include <limits>

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
        // Advanced convenience methods for improved user experience
        .def("runSimulation", [](Brain& self, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
            }
        }, py::arg("steps"),
             "Run a simple simulation for the given number of steps")
        .def("runSimulationWithTime", [](Brain& self, size_t steps, double timestep) {
            Timestamp time = 0.0;
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, time);
                time += timestep;
            }
        }, py::arg("steps"), py::arg("timestep") = 0.1,
             "Run a simulation with timestamps")
        .def("createBrainSnapshot", [](Brain& self) {
            return self.getTotalNeuronCount();
        }, "Get current brain snapshot statistics (neuron count)")
        .def("configureForExperiment", [](Brain& self, const std::string& experimentType) {
            auto config = self.getConfig();
            if (experimentType == "simple") {
                config->set("neuron_count", 1000);
                config->set("region_count", 1);
                config->set("connection_probability", 0.1f);
            } else if (experimentType == "complex") {
                config->set("neuron_count", 5000);
                config->set("region_count", 4);
                config->set("connection_probability", 0.05f);
            }
            self.initialize();
        }, py::arg("experimentType"),
             "Configure brain for a specific experiment type")
        .def("getLearningMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["e_i_ratio"] = self.getExcitationInhibitionRatio();
            metrics["average_firing_rate"] = self.getAverageFiringRate();
            metrics["pending_spikes"] = self.getPendingSpikeEventCount();
            return metrics;
        }, "Get comprehensive learning metrics")
        .def("simulateEpisode", [](Brain& self, size_t steps, double timestep, 
                                 const std::function<float(size_t)> onStep) {
            float totalReward = 0.0f;
            Timestamp time = 0.0;
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, time);
                if (onStep) {
                    totalReward += onStep(i);
                }
                time += timestep;
            }
            return totalReward;
        }, py::arg("steps"), py::arg("timestep") = 0.1, py::arg("onStep") = py::none(),
             "Run an episode with optional callback per step");

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
        // Enhanced AgentBrain methods for advanced usage
        .def("createLearningEnvironment", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            self.initialize(*world);
            // Enable all learning subsystems by default for advanced experimentation
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return self.getBrain();
        }, py::arg("world"),
             "Create a learning environment with all subsystems enabled")
        .def("runAdaptiveEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                     size_t steps, double timestep) {
            py::dict stats;
            float totalReward = 0.0f;
            Timestamp time = 0.0;
            
            for (size_t i = 0; i < steps; ++i) {
                world->update(timestep);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                auto brain = self.getBrain();
                brain->step(i, time);
                
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, time);
                
                // Calculate reward based on world state
                auto body = world->getAgentBody();
                float reward = body.energy / world->getMaxEnergy();
                self.applyRewardModulation(reward, 0.0f);
                self.updateDevelopment(timestep);
                
                totalReward += reward;
                time += timestep;
            }
            
            stats["total_reward"] = totalReward;
            stats["steps_completed"] = steps;
            stats["final_curiosity"] = self.getCuriosityLevel();
            stats["final_novelty"] = self.getNoveltyLevel();
            stats["final_prediction_error"] = self.getPredictionError();
            return stats;
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
             "Run an adaptive episode and return comprehensive statistics")
        .def("enableAllLearning", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning subsystems")
        .def("disableAllLearning", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning subsystems")
        .def("isLearningEnabled", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Check which learning subsystems are enabled");

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

    // Convenience factory functions for common scenarios
    m.def("createAndInitializeBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain in one call");

    m.def("createFullAgentSystem", [](std::shared_ptr<Config> config) -> py::tuple {
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        py::tuple result = py::make_tuple(brain, agent, world);
        return result;
    }, py::arg("config"), "Create a complete agent system (brain, agent, world)");

    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t steps, double timestep) {
        py::dict results;
        Timestamp time = 0.0;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, time);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, time);
            
            auto body = world->getAgentBody();
            float reward = body.energy / world->getMaxEnergy();
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(timestep);
            
            time += timestep;
        }
        
        results["total_steps"] = steps;
        results["final_energy"] = world->getAgentBody().energy;
        results["final_x"] = world->getAgentBody().x;
        results["final_y"] = world->getAgentBody().y;
        results["brain_neuron_count"] = brain->getTotalNeuronCount();
        results["brain_spike_count"] = brain->getTotalSpikeCount();
        
        return results;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
         "Run a complete simulation and return results");

    // Performance monitoring utilities
    m.def("runPerformanceBenchmark", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto startTime = std::chrono::steady_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        py::dict results;
        results["total_steps"] = steps;
        results["total_time_ms"] = duration.count();
        results["avg_step_time_ms"] = duration.count() / static_cast<double>(steps);
        results["steps_per_second"] = steps / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("steps"),
         "Run a performance benchmark and return timing statistics");

    m.def("createBrainProfiler", []() {
        class BrainProfiler {
        public:
            struct StepMetrics {
                size_t stepNumber;
                double timestamp;
                double stepTimeMs;
                size_t firingNeurons;
                size_t totalSpikes;
                float eiratio;
                float firingRate;
            };
            
            void reset() {
                steps_.clear();
                startTime_ = std::chrono::steady_clock::now();
                totalSteps_ = 0;
            }
            
            void recordStep(size_t step, double timestamp, std::shared_ptr<Brain> brain) {
                auto endTime = std::chrono::steady_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - lastStepTime_);
                
                StepMetrics metrics;
                metrics.stepNumber = step;
                metrics.timestamp = timestamp;
                metrics.stepTimeMs = stepDuration.count();
                metrics.firingNeurons = brain->getFiringNeuronCount();
                metrics.totalSpikes = brain->getTotalSpikeCount();
                metrics.eiratio = brain->getExcitationInhibitionRatio();
                metrics.firingRate = brain->getAverageFiringRate();
                
                steps_.push_back(metrics);
                lastStepTime_ = endTime;
                totalSteps_++;
            }
            
            py::dict getStatistics() {
                if (steps_.empty()) {
                    py::dict empty;
                    empty["error"] = "No steps recorded";
                    return empty;
                }
                
                double totalTime = 0.0;
                size_t maxFiring = 0;
                size_t maxSpikes = 0;
                float minEiRatio = std::numeric_limits<float>::max();
                float maxEiRatio = std::numeric_limits<float>::min();
                
                for (const auto& step : steps_) {
                    totalTime += step.stepTimeMs;
                    maxFiring = std::max(maxFiring, step.firingNeurons);
                    maxSpikes = std::max(maxSpikes, step.totalSpikes);
                    minEiRatio = std::min(minEiRatio, step.eiratio);
                    maxEiRatio = std::max(maxEiRatio, step.eiratio);
                }
                
                py::dict stats;
                stats["total_steps"] = totalSteps_;
                stats["total_time_ms"] = totalTime;
                stats["avg_step_time_ms"] = totalTime / steps_.size();
                stats["min_step_time_ms"] = steps_.empty() ? 0 : steps_.front().stepTimeMs;
                stats["max_step_time_ms"] = steps_.empty() ? 0 : steps_.back().stepTimeMs;
                stats["max_firing_neurons"] = maxFiring;
                stats["max_total_spikes"] = maxSpikes;
                stats["min_ei_ratio"] = minEiRatio;
                stats["max_ei_ratio"] = maxEiRatio;
                stats["avg_ei_ratio"] = (minEiRatio + maxEiRatio) / 2.0f;
                
                if (!steps_.empty()) {
                    py::list stepList;
                    for (const auto& step : steps_) {
                        py::dict stepDict;
                        stepDict["step"] = step.stepNumber;
                        stepDict["time_ms"] = step.stepTimeMs;
                        stepDict["firing_neurons"] = step.firingNeurons;
                        stepDict["total_spikes"] = step.totalSpikes;
                        stepDict["ei_ratio"] = step.eiratio;
                        stepDict["firing_rate"] = step.firingRate;
                        stepList.append(stepDict);
                    }
                    stats["step_details"] = stepList;
                }
                
                return stats;
            }
            
            std::vector<StepMetrics> getDetailedSteps() {
                return steps_;
            }
            
        private:
            std::vector<StepMetrics> steps_;
            std::chrono::steady_clock::time_point startTime_;
            std::chrono::steady_clock::time_point lastStepTime_;
            size_t totalSteps_ = 0;
        };
        
        return std::make_shared<BrainProfiler>();
    }, "Create a brain profiler for detailed step-by-step monitoring");

    m.def("exportProfilerResults", [](const std::shared_ptr<BrainProfiler>& profiler, const std::string& filename) {
        if (!profiler) {
            throw std::runtime_error("Profiler is null");
        }
        
        auto stats = profiler->getStatistics();
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "NLM Brain Profiler Results\n";
        file << "===========================" << std::endl;
        file << "Total Steps: " << stats["total_steps"].cast<size_t>() << std::endl;
        file << "Total Time (ms): " << stats["total_time_ms"].cast<double>() << std::endl;
        file << "Average Step Time (ms): " << stats["avg_step_time_ms"].cast<double>() << std::endl;
        file << "Max Firing Neurons: " << stats["max_firing_neurons"].cast<size_t>() << std::endl;
        file << "Max Total Spikes: " << stats["max_total_spikes"].cast<size_t>() << std::endl;
        file << "E/I Ratio Range: [" << stats["min_ei_ratio"].cast<float>() << 
                  ", " << stats["max_ei_ratio"].cast<float>() << "]" << std::endl;
        
        file << "\nStep Details:\n";
        auto stepDetails = stats["step_details"].cast<py::list>();
        for (size_t i = 0; i < stepDetails.size(); ++i) {
            auto step = stepDetails[i].cast<py::dict>();
            file << "Step " << step["step"].cast<size_t>() << 
                     ": time=" << step["time_ms"].cast<double>() << "ms, "
                     << "firing=" << step["firing_neurons"].cast<size_t>() << ", "
                     << "spikes=" << step["total_spikes"].cast<size_t>() << ", "
                     << "ei_ratio=" << step["ei_ratio"].cast<float>() << std::endl;
        }
        
        file.close();
        return true;
    }, py::arg("profiler"), py::arg("filename"),
         "Export profiler results to a text file");

    // Error handling utilities
    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        py::dict validationErrors;
        bool isValid = true;
        
        // Check required fields
        if (!config->has("brain.neuron_count")) {
            validationErrors["brain.neuron_count"] = "Missing required field: brain.neuron_count";
            isValid = false;
        } else {
            size_t neuronCount = config->get<size_t>("brain.neuron_count");
            if (neuronCount == 0) {
                validationErrors["brain.neuron_count"] = "Neuron count must be greater than 0";
                isValid = false;
            } else if (neuronCount > 100000) {
                validationErrors["brain.neuron_count"] = "Neuron count too large (max: 100000)";
                isValid = false;
            }
        }
        
        if (!config->has("brain.synapse_density")) {
            validationErrors["brain.synapse_density"] = "Missing required field: brain.synapse_density";
            isValid = false;
        } else {
            float density = config->get<float>("brain.synapse_density");
            if (density < 0.0f || density > 1.0f) {
                validationErrors["brain.synapse_density"] = "Synapse density must be between 0.0 and 1.0";
                isValid = false;
            }
        }
        
        if (!config->has("region_count")) {
            validationErrors["region_count"] = "Missing required field: region_count";
            isValid = false;
        } else {
            size_t regionCount = config->get<size_t>("region_count");
            if (regionCount == 0) {
                validationErrors["region_count"] = "Region count must be greater than 0";
                isValid = false;
            }
        }
        
        if (isValid) {
            validationErrors["status"] = "Configuration is valid";
        }
        
        return validationErrors;
    }, py::arg("config"),
         "Validate brain configuration and return any validation errors");

    m.def("safeBrainStep", [](std::shared_ptr<Brain> brain, size_t step, double time = 0.0) {
        try {
            if (!brain) {
                throw std::runtime_error("Brain pointer is null");
            }
            
            // Check if brain has been initialized
            if (brain->getTotalNeuronCount() == 0) {
                throw std::runtime_error("Brain has not been properly initialized (0 neurons)");
            }
            
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            py::dict errorInfo;
            errorInfo["success"] = false;
            errorInfo["error"] = std::string("Brain::step failed: ") + e.what();
            errorInfo["step"] = step;
            errorInfo["time"] = time;
            return errorInfo;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
         "Safely execute a brain step with error handling and validation");

    m.def("createExperimentRunner", [](std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t numEpisodes,
                                     size_t stepsPerEpisode) {
        class ExperimentRunner {
        public:
            struct EpisodeResult {
                size_t episodeNumber;
                double totalReward;
                size_t stepsCompleted;
                size_t finalNeuronCount;
                size_t finalSpikeCount;
                double avgStepTime;
                std::chrono::steady_clock::time_point startTime;
                std::chrono::steady_clock::time_point endTime;
            };
            
            ExperimentRunner(std::shared_ptr<Brain> b, std::shared_ptr<AgentBrain> a, 
                           std::shared_ptr<SimpleWorld> w)
                : brain_(b), agent_(a), world_(w) {}
            
            py::list runExperiment() {
                py::list results;
                
                for (size_t episode = 0; episode < numEpisodes_; ++episode) {
                    EpisodeResult episodeResult;
                    episodeResult.episodeNumber = episode;
                    episodeResult.startTime = std::chrono::steady_clock::now();
                    
                    double episodeTotalReward = 0.0;
                    size_t episodeStepsCompleted = 0;
                    
                    for (size_t step = 0; step < stepsPerEpisode_; ++step) {
                        auto stepStart = std::chrono::steady_clock::now();
                        
                        world_->update(0.1);
                        agent_->processSensoryInput(world_->getSensoryPercept());
                        brain_->step(step);
                        auto action = agent_->decodeMotorCommand();
                        world_->applyMotorCommand(action, world_->getSimulationTime());
                        
                        auto body = world_->getAgentBody();
                        float reward = body.energy / world_->getMaxEnergy();
                        agent_->applyRewardModulation(reward, 0.0f);
                        agent_->updateDevelopment(0.1);
                        
                        episodeTotalReward += reward;
                        episodeStepsCompleted++;
                        
                        auto stepEnd = std::chrono::steady_clock::now();
                        auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart);
                        episodeResult.avgStepTime += stepDuration.count();
                    }
                    
                    episodeResult.endTime = std::chrono::steady_clock::now();
                    episodeResult.totalReward = episodeTotalReward;
                    episodeResult.stepsCompleted = episodeStepsCompleted;
                    episodeResult.finalNeuronCount = brain_->getTotalNeuronCount();
                    episodeResult.finalSpikeCount = brain_->getTotalSpikeCount();
                    episodeResult.avgStepTime = episodeResult.avgStepTime / episodeStepsCompleted;
                    
                    py::dict episodeData;
                    episodeData["episode"] = episodeResult.episodeNumber;
                    episodeData["total_reward"] = episodeResult.totalReward;
                    episodeData["steps_completed"] = episodeResult.stepsCompleted;
                    episodeData["final_neurons"] = episodeResult.finalNeuronCount;
                    episodeData["final_spikes"] = episodeResult.finalSpikeCount;
                    episodeData["avg_step_time_us"] = episodeResult.avgStepTime;
                    episodeData["duration_ms"] = std::chrono::duration_cast<std::chrono::milliseconds>(episodeResult.endTime - episodeResult.startTime).count();
                    
                    results.append(episodeData);
                    
                    // Reset for next episode
                    brain_->reset();
                    agent_->reset();
                    world_->reset();
                }
                
                return results;
            }
            
            py::dict getExperimentSummary(const py::list& results) {
                py::dict summary;
                double totalReward = 0.0;
                size_t totalSteps = 0;
                size_t totalNeurons = 0;
                size_t totalSpikes = 0;
                double totalDuration = 0.0;
                
                for (size_t i = 0; i < results.size(); ++i) {
                    auto episode = results[i].cast<py::dict>();
                    totalReward += episode["total_reward"].cast<double>();
                    totalSteps += episode["steps_completed"].cast<size_t>();
                    totalNeurons = episode["final_neurons"].cast<size_t>();
                    totalSpikes = episode["final_spikes"].cast<size_t>();
                    totalDuration += episode["duration_ms"].cast<double>();
                }
                
                size_t numEpisodes = results.size();
                summary["num_episodes"] = numEpisodes;
                summary["total_reward"] = totalReward;
                summary["avg_reward_per_episode"] = totalReward / numEpisodes;
                summary["total_steps"] = totalSteps;
                summary["final_neurons"] = totalNeurons;
                summary["final_spikes"] = totalSpikes;
                summary["avg_duration_per_episode_ms"] = totalDuration / numEpisodes;
                summary["steps_per_second"] = totalSteps / (totalDuration / 1000.0);
                
                return summary;
            }
            
        private:
            std::shared_ptr<Brain> brain_;
            std::shared_ptr<AgentBrain> agent_;
            std::shared_ptr<SimpleWorld> world_;
            size_t numEpisodes_ = 10;
            size_t stepsPerEpisode_ = 100;
        };
        
        return std::make_shared<ExperimentRunner>(brain, agent, world);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
         "Create an experiment runner for running multiple episodes of simulation");

    // Advanced configuration utilities
    m.def("createAdvancedConfig", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        // Helper to convert Python types to C++ types
        auto setConfigValue = [&config](const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                config->set(key, value.cast<size_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        };
        
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            setConfigValue(key, item.second);
        }
        
        return config;
    }, py::arg("configDict"),
         "Create configuration from a Python dictionary for advanced configuration");

    m.def("mergeConfigs", [](std::shared_ptr<Config> baseConfig, std::shared_ptr<Config> overrideConfig) {
        if (!baseConfig || !overrideConfig) {
            throw std::runtime_error("One or both configs are null");
        }
        
        auto mergedConfig = std::make_shared<Config>(*baseConfig);
        
        // Get all keys from override config and set them
        auto overrideKeys = overrideConfig->getKeys();
        for (const auto& key : overrideKeys) {
            if (config->has(key)) {
                if (config->get(key).index() == overrideConfig->get(key).index()) {
                    // Same type, copy value
                    if (auto intVal = config->get<int>(key)) {
                        mergedConfig->set(key, *intVal);
                    } else if (auto floatVal = config->get<float>(key)) {
                        mergedConfig->set(key, *floatVal);
                    } else if (auto doubleVal = config->get<double>(key)) {
                        mergedConfig->set(key, *doubleVal);
                    } else if (auto stringVal = config->get<std::string>(key)) {
                        mergedConfig->set(key, *stringVal);
                    } else if (auto boolVal = config->get<bool>(key)) {
                        mergedConfig->set(key, *boolVal);
                    }
                }
            }
        }
        
        return mergedConfig;
    }, py::arg("baseConfig"), py::arg("overrideConfig"),
         "Merge two configurations, with override taking precedence");

    // System utilities
    m.def("getSystemInfo", []() {
        py::dict info;
        info["python_version"] = PY_VERSION;
        info["pybind11_version"] = PYBIND11_VERSION;
        info["compiler"] = std::string(CMAKE_CXX_COMPILER_ID);
        info["cxx_standard"] = std::to_string(CMAKE_CXX_STANDARD);
        info["build_type"] = std::string(CMAKE_BUILD_TYPE);
        info["project_version"] = "0.1.0";
        return info;
    }, "Get system information about the NLM installation");

    m.def("checkInstallation", []() {
        py::dict status;
        
        try {
            import pynlm;
            status["module_imported"] = true;
            
            // Try to create a minimal brain
            auto config = pynlm.createDefaultConfig();
            auto brain = pynlm.createBrain(config);
            brain->initialize();
            status["brain_creation"] = true;
            status["neurons_created"] = brain->getTotalNeuronCount();
            
            // Try to run a simple step
            brain->step(0);
            status["step_execution"] = true;
            status["total_spikes"] = brain->getTotalSpikeCount();
            
            status["overall_status"] = "Installation appears to be working correctly";
            
        } catch (const std::exception& e) {
            status["module_imported"] = false;
            status["error"] = std::string("Installation check failed: ") + e.what();
            status["overall_status"] = "Installation has issues";
        }
        
        return status;
    }, "Check if the NLM installation is working correctly");

    m.def("createConfigFromFile", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load configuration from file: " + filename);
        }
        return config;
    }, py::arg("filename"),
         "Load configuration from a JSON file");

    m.def("saveConfigToFile", [](std::shared_ptr<Config> config, const std::string& filename) {
        if (!config->saveToFile(filename)) {
            throw std::runtime_error("Failed to save configuration to file: " + filename);
        }
        return true;
    }, py::arg("config"), py::arg("filename"),
         "Save configuration to a JSON file");

    // Neural network utilities
    m.def("analyzeNetworkTopology", [](std::shared_ptr<Brain> brain) {
        py::dict topology;
        
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        topology["total_neurons"] = totalNeurons;
        topology["total_synapses"] = totalSynapses;
        topology["synapse_density"] = totalNeurons > 0 ? static_cast<double>(totalSynapses) / totalNeurons : 0.0;
        
        // Get region information
        auto regions = brain->getRegions();
        py::list regionList;
        
        for (const auto& region : regions) {
            py::dict regionInfo;
            regionInfo["id"] = region->getId();
            regionInfo["name"] = region->getName();
            
            auto populations = region->getPopulations();
            py::list populationList;
            
            for (const auto& population : populations) {
                py::dict popInfo;
                popInfo["type"] = static_cast<int>(population->getNeuronType());
                popInfo["size"] = population->getSize();
                populationList.append(popInfo);
            }
            
            regionInfo["populations"] = populationList;
            regionList.append(regionInfo);
        }
        
        topology["regions"] = regionList;
        topology["region_count"] = regions.size();
        
        return topology;
    }, py::arg("brain"),
         "Analyze and return the network topology of the brain");

    m.def("simulateAndAnalyze", [](std::shared_ptr<Brain> brain, 
                                 std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 size_t steps) {
        if (!brain || !agent || !world) {
            throw std::runtime_error("One or more components are null");
        }
        
        py::dict results;
        
        // Run simulation
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto body = world->getAgentBody();
            float reward = body.energy / world->getMaxEnergy();
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        // Collect statistics
        results["final_neurons"] = brain->getTotalNeuronCount();
        results["final_synapses"] = brain->getTotalSynapseCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["average_firing_rate"] = brain->getAverageFiringRate();
        results["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        results["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        results["final_position"] = py::make_tuple(world->getAgentBody().x, world->getAgentBody().y);
        results["final_energy"] = world->getAgentBody().energy;
        results["final_age"] = world->getAgentBody().age;
        
        results["neuromodulation_level"] = agent->getNeuromodulationLevel();
        results["curiosity_level"] = agent->getCuriosityLevel();
        results["novelty_level"] = agent->getNoveltyLevel();
        results["prediction_error"] = agent->getPredictionError();
        
        return results;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
         "Run a complete simulation and return comprehensive results");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm