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

    // Agent brain interface
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
        
        // Advanced operations for power users
        .def("batchStep", [](Brain& self, size_t steps, double dt) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * dt);
            }
        }, py::arg("steps"), py::arg("dt") = 0.001,
             "Perform multiple simulation steps efficiently")
        .def("batchInjectCurrent", [](Brain& self, NeuronType type, double current, size_t repetitions, double dt) {
            for (size_t i = 0; i < repetitions; ++i) {
                self.injectCurrentToNeurons(type, current);
                self.step(i, i * dt);
            }
        }, py::arg("type"), py::arg("current"), py::arg("repetitions") = 100, py::arg("dt") = 0.001,
             "Inject current multiple times for pattern generation")
        .def("getNeuronStats", [](Brain& self) {
            std::unordered_map<std::string, size_t> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive brain statistics")
        .def("getSimulationTime", [](Brain& self) {
            if (auto* spikeSystem = self.getSpikeSystem()) {
                return spikeSystem->getSimulationTime();
            }
            return 0.0;
        }, "Get current simulation time")
        .def("stimulatePattern", [](Brain& self, const std::vector<NeuronId>& neurons, double current, size_t steps) {
            auto* spikeSystem = self.getSpikeSystem();
            if (!spikeSystem) return;
            
            for (size_t step = 0; step < steps; ++step) {
                for (const auto& neuronId : neurons) {
                    self.injectCurrent(neuronId, current);
                }
                self.step(step, step * 0.001);
            }
        }, py::arg("neurons"), py::arg("current") = 50.0, py::arg("steps") = 100,
             "Stimulate specific neurons with a pattern")
        .def("recordSpikes", [](Brain& self, size_t duration, double dt) {
            std::vector<std::vector<NeuronId>> spikeHistory;
            spikeHistory.reserve(duration);
            
            for (size_t step = 0; step < duration; ++step) {
                self.step(step, step * dt);
                if (auto* spikeSystem = self.getSpikeSystem()) {
                    spikeHistory.push_back(spikeSystem->getLastSpikes(10));
                }
            }
            return spikeHistory;
        }, py::arg("duration") = 1000, py::arg("dt") = 0.001,
             "Record spike activity over time")
        .def("resetWithPattern", [](Brain& self, double current, size_t steps) {
            self.reset();
            if (auto config = self.getConfig()) {
                config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
            }
            self.initialize();
            for (size_t i = 0; i < steps; ++i) {
                self.injectCurrentToNeurons(NeuronType::Excitatory, current);
                self.step(i, i * 0.001);
            }
        }, py::arg("current") = 10.0, py::arg("steps") = 50,
             "Reset brain with controlled initialization pattern");

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

    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Basic validation
        if (!config.has("neuron_count")) {
            errors.push_back("Missing required parameter: neuron_count");
        }
        
        if (config.has("neuron_count")) {
            auto count = config.getOr<int64_t>("neuron_count", 0);
            if (count <= 0 || count > 1000000) {
                errors.push_back("neuron_count out of valid range (1-1,000,000)");
            }
        }
        
        if (!config.has("region_count")) {
            warnings.push_back("region_count not specified, using default");
        }
        
        if (!config.has("simulation_timestep") || config.getOr<double>("simulation_timestep", 0.001) <= 0.0) {
            errors.push_back("Invalid simulation_timestep value");
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("config"), "Validate configuration and return errors/warnings");

    m.def("exportToJSON", [](const Brain& brain, const std::string& filename) {
        auto config = brain.getConfig();
        return config->saveToFile(filename);
    }, py::arg("brain"), py::arg("filename"), "Export brain configuration to JSON");

    m.def("importFromJSON", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filename);
        return config;
    }, py::arg("filename"), "Import brain configuration from JSON file");

    m.def("getAllConfigKeys", []() {
        std::vector<std::string> allKeys;
        // Add common configuration keys
        allKeys.insert(allKeys.end(), {
            "neuron_count",
            "region_count", 
            "connection_probability",
            "simulation_timestep",
            "random_seed",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_tau",
            "synaptogenesis_rate",
            "pruning_rate",
            "enable_reward_modulation",
            "enable_curiosity",
            "enable_development",
            "enable_structural_plasticity"
        });
        return allKeys;
    }, "Get list of all available configuration keys");

    m.def("getConfigFromJSON", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load configuration from JSON file: " + filename);
        }
        return config;
    }, py::arg("filename"), "Load configuration from JSON file with error handling");

    m.def("createConfigWithValidation", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load configuration from file: " + filename);
        }
        
        // Validate configuration
        auto errors = validateConfig(config);
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors.first) {
                errorMsg += "ERROR: " + error + "\n";
            }
            for (const auto& warning : errors.second) {
                errorMsg += "WARNING: " + warning + "\n";
            }
            throw std::runtime_error(errorMsg);
        }
        
        return config;
    }, py::arg("filename"), "Create configuration with validation and error handling");

    m.def("performanceMonitor", [](Brain& brain, std::function<void(const std::string&)> callback, double interval) {
        std::thread monitorThread([&]() {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t stepCount = 0;
            
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(interval * 1000)));
                
                auto stats = brain.getNeuronStats();
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                std::stringstream ss;
                ss << "Performance Monitor - Step " << stepCount 
                   << " | Neurons: " << stats["total_neurons"]
                   << " | Synapses: " << stats["total_synapses"]
                   << " | Spikes: " << stats["total_spikes"]
                   << " | Duration: " << duration.count() << "ms"
                   << " | Rate: " << (stats["total_spikes"] / (duration.count() / 1000.0)) << " spikes/sec";
                
                callback(ss.str());
                stepCount++;
            }
        });
        
        monitorThread.detach();
        return std::thread([&]() {
            std::this_thread::sleep_for(std::chrono::hours(24)); // Run indefinitely
        });
    }, py::arg("brain"), py::arg("callback"), py::arg("interval") = 5.0, "Start performance monitoring thread with configurable interval");

    m.def("safeLoadBrain", [](const std::string& configFile, const std::string& brainFile) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            throw std::runtime_error("Failed to load configuration from: " + configFile);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        if (!brainFile.empty() && brain->load(brainFile)) {
            NLM_LOG_INFO("Successfully loaded brain state from: " + brainFile);
        }
        
        return brain;
    }, py::arg("configFile"), py::arg("brainFile") = "", "Safely load brain with configuration and error handling");

    m.def("batchLoadBrains", [](const std::vector<std::string>& configFiles) {
        std::vector<std::shared_ptr<Brain>> brains;
        brains.reserve(configFiles.size());
        
        for (const auto& filename : configFiles) {
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(filename)) {
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    brains.push_back(brain);
                    NLM_LOG_INFO("Successfully loaded brain from: " + filename);
                } else {
                    NLM_LOG_WARNING("Failed to initialize brain from: " + filename);
                }
            } else {
                NLM_LOG_WARNING("Failed to load configuration from: " + filename);
            }
        }
        
        return brains;
    }, py::arg("configFiles"), "Load multiple brains from configuration files with error handling");

    m.def("getSystemInfo", []() {
        std::map<std::string, std::string> info;
        
        info["nlm_version"] = "Phase 6.0 (Final Integration)";
        info["cpp_standard"] = "C++20";
        info["build_type"] = "Release";
        info["python_bindings"] = "1.0.0";
        info["last_update"] = "2026-09-24";
        info["features"] = "Full neural simulation with memory, prediction, cognition, neuromodulation";
        info["supported_formats"] = "JSON, YAML, Simple Config";
        
        return info;
    }, "Get comprehensive system information");

    m.def("checkBrainHealth", [](const Brain& brain) {
        std::map<std::string, std::string> healthStatus;
        
        healthStatus["brain_initialized"] = brain.getTotalNeuronCount() > 0 ? "yes" : "no";
        healthStatus["region_count"] = std::to_string(brain.getRegionCount());
        healthStatus["neuron_count"] = std::to_string(brain.getTotalNeuronCount());
        healthStatus["synapse_count"] = std::to_string(brain.getTotalSynapseCount());
        healthStatus["active_neurons"] = std::to_string(brain.getActiveNeuronCount());
        healthStatus["firing_neurons"] = std::to_string(brain.getFiringNeuronCount());
        healthStatus["total_spikes"] = std::to_string(brain.getTotalSpikeCount());
        healthStatus["excitation_inhibition_ratio"] = std::to_string(brain.getExcitationInhibitionRatio());
        
        // Check for potential issues
        if (brain.getTotalNeuronCount() > 100000) {
            healthStatus["warning"] = "High neuron count may impact performance";
        }
        
        return healthStatus;
    }, py::arg("brain"), "Check NLM system health status with detailed metrics");

    m.def("createResearchConfiguration", [](const std::string& researchType) {
        auto config = std::make_shared<Config>();
        
        if (researchType == "memory") {
            config->set("neuron_count", static_cast<int64_t>(8000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config->set("connection_probability", 0.08, ConfigSource::Default);
            config->set("enable_reward_modulation", true, ConfigSource::Default);
            config->set("enable_curiosity", true, ConfigSource::Default);
            config->set("enable_development", true, ConfigSource::Default);
            
        } else if (researchType == "prediction") {
            config->set("neuron_count", static_cast<int64_t>(6000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.06, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
            
        } else if (researchType == "development") {
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
            config->set("connection_probability", 0.05, ConfigSource::Default);
            config->set("enable_development", true, ConfigSource::Default);
            config->set("enable_structural_plasticity", true, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.00002, ConfigSource::Default);
            
        } else {
            // Default research configuration
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.05, ConfigSource::Default);
            config->set("enable_reward_modulation", true, ConfigSource::Default);
            config->set("enable_curiosity", true, ConfigSource::Default);
            config->set("enable_development", true, ConfigSource::Default);
            config->set("enable_structural_plasticity", true, ConfigSource::Default);
        }
        
        return config;
    }, py::arg("researchType") = "default", "Create research-grade configuration for specific research areas");

    m.def("exportBenchmarkData", [](const Brain& brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open benchmark file for writing: " + filename);
        }
        
        // Export brain statistics for benchmarking
        file << "# NLM Benchmark Data\n";
        file << "# Generated on " << getCurrentTimestamp() << "\n\n";
        
        auto stats = brain.getNeuronStats();
        file << "brain_statistics:\n";
        file << "  total_neurons: " << stats["total_neurons"] << "\n";
        file << "  total_synapses: " << stats["total_synapses"] << "\n";
        file << "  active_neurons: " << stats["active_neurons"] << "\n";
        file << "  firing_neurons: " << stats["firing_neurons"] << "\n";
        file << "  total_spikes: " << stats["total_spikes"] << "\n";
        file << "  excitation_inhibition_ratio: " << brain.getExcitationInhibitionRatio() << "\n\n";
        
        // Export configuration
        auto config = brain.getConfig();
        file << "configuration:\n";
        for (const auto& key : config->getKeys()) {
            file << "  " << key << ": " << getConfigValueString(config, key) << "\n";
        }
        
        NLM_LOG_INFO("Benchmark data exported to: " + filename);
        return true;
    }, py::arg("brain"), py::arg("filename"), "Export benchmark data for performance analysis");

} // namespace nlm
