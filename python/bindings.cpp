#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/brain/profiling/BrainProfiler.hpp"
#include "../src/brain/profiling/DebugTools.hpp"
#include "../src/brain/profiling/PerformanceMetrics.hpp"
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
        .def("step_batch", [](Brain& self, size_t numSteps, Timestamp timestep = 1.0) {
             for (size_t i = 0; i < numSteps; ++i) {
                 self.step(i, timestep * i);
             }
         }, py::arg("steps"), py::arg("timestep") = 1.0,
             "Process multiple steps efficiently")
        .def("processMultipleInputs", [](Brain& self, const std::vector<std::reference_wrapper<const SensoryInput>>& inputs) {
             for (const auto& input : inputs) {
                 self.receiveSensoryInput(input.get());
             }
         }, py::arg("inputs"),
             "Batch sensory processing")
        .def("simulateEpisode", [](Brain& self, size_t maxSteps, Timestamp timestep = 1.0) {
             for (size_t i = 0; i < maxSteps; ++i) {
                 self.step(i, timestep * i);
             }
         }, py::arg("max_steps"), py::arg("timestep") = 1.0,
             "Run complete episodes")
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
        .def("visualizeConnectivity", [](Brain& self) {
             py::dict result;
             auto regions = self.getRegions();
             for (const auto& region : regions) {
                 py::dict regionInfo;
                 auto neurons = region->getNeurons();
                 for (const auto& neuron : neurons) {
                     py::dict neuronInfo;
                     neuronInfo["id"] = neuron->getId();
                     neuronInfo["type"] = static_cast<int>(neuron->getType());
                     neuronInfo["firingRate"] = neuron->getFiringRate();
                     regionInfo["neuron_" + std::to_string(neuron->getId().value())] = neuronInfo;
                 }
                 result[region->getName()] = regionInfo;
             }
             return result;
         }, "Get connectivity data as Python structures")
        .def("getNeuralActivityHistory", [](Brain& self) {
             auto spikeSystem = self.getSpikeSystem();
             if (!spikeSystem) return py::list();
             
             auto history = spikeSystem->getSpikeHistory();
             py::list result;
             for (const auto& entry : history) {
                 py::dict event;
                 event["neuron_id"] = entry.first.value();
                 event["step"] = entry.second.first;
                 event["time"] = entry.second.second;
                 result.append(event);
             }
             return result;
         }, "Get firing rate history")
        .def("plotNeuralHeatmap", [](Brain& self) {
             auto regions = self.getRegions();
             py::dict heatmap;
             for (const auto& region : regions) {
                 std::vector<std::vector<float>> activityMap;
                 auto neurons = region->getNeurons();
                 if (neurons.empty()) continue;
                 
                 size_t rows = neurons.size();
                 size_t cols = 10;  
                 activityMap.resize(rows, std::vector<float>(cols, 0.0f));
                 
                 for (size_t i = 0; i < rows; ++i) {
                     for (size_t j = 0; j < cols; ++j) {
                         activityMap[i][j] = neurons[i]->getFiringRate();
                     }
                 }
                 
                 py::list rowList;
                 for (const auto& row : activityMap) {
                     py::list colList;
                     for (float val : row) {
                         colList.append(val);
                     }
                     rowList.append(colList);
                 }
                 heatmap[region->getName()] = rowList;
             }
             return heatmap;
         }, "Generate neural activity maps")
        .def("benchmarkPerformance", [](Brain& self, size_t numSteps) {
             auto start = std::chrono::high_resolution_clock::now();
             
             for (size_t i = 0; i < numSteps; ++i) {
                 self.step(i);
             }
             
             auto end = std::chrono::high_resolution_clock::now();
             auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
             
             py::dict stats;
             stats["total_time_ms"] = duration.count();
             stats["steps_per_second"] = numSteps * 1000.0 / duration.count();
             stats["neurons_per_step"] = self.getTotalNeuronCount() / numSteps;
             return stats;
         }, py::arg("steps"), "Performance measurement")
        .def("getMemoryUsage", [](Brain& self) {
             auto workingMem = self.getWorkingMemory();
             auto episodicMem = self.getEpisodicMemory();
             auto assocMem = self.getAssociativeMemory();
             
             py::dict memStats;
             memStats["working_memory_size"] = workingMem ? workingMem->getTraceCount() : 0;
             memStats["episodic_memory_count"] = episodicMem ? episodicMem->getEpisodicCount() : 0;
             memStats["associative_memory_size"] = assocMem ? assocMem->getAssociationCount() : 0;
             return memStats;
         }, "Memory profiling")
        .def("getPerformanceStats", [](Brain& self) {
             auto spikeSystem = self.getSpikeSystem();
             py::dict stats;
             
             stats["total_neurons"] = self.getTotalNeuronCount();
             stats["total_synapses"] = self.getTotalSynapseCount();
             stats["active_neurons"] = self.getActiveNeuronCount();
             stats["firing_neurons"] = self.getFiringNeuronCount();
             stats["average_firing_rate"] = self.getAverageFiringRate();
             stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
             stats["total_spike_count"] = self.getTotalSpikeCount();
             stats["pending_spike_events"] = spikeSystem ? spikeSystem->getPendingSpikeEventCount() : 0;
             
             auto workingMem = self.getWorkingMemory();
             if (workingMem) {
                 auto traces = workingMem->getActiveTraces();
                 stats["active_working_memory_traces"] = traces.size();
             }
             
             return stats;
         }, "Comprehensive metrics")
        .def("debugMode", &Brain::logStatus,
             py::arg("mode") = 1,
             "Enable/disable debug output")
        .def("adjustConnectionWeight", [](Brain& self, NeuronId pre, NeuronId post, float weight) {
             auto regions = self.getRegions();
             for (auto& region : regions) {
                 auto neurons = region->getNeurons();
                 for (auto& neuron : neurons) {
                     auto synapses = neuron->getOutgoingSynapses();
                     for (auto& synapse : synapses) {
                         if (synapse->getPostNeuron() == post && synapse->getPreNeuron() == pre) {
                             synapse->setWeight(weight);
                             return;
                         }
                     }
                 }
             }
         }, py::arg("pre"), py::arg("post"), py::arg("weight"),
             "Direct weight control")
        .def("setPlasticityFlags", [](Brain& self, NeuronId pre, NeuronId post, PlasticityFlags flags) {
             auto regions = self.getRegions();
             for (auto& region : regions) {
                 auto neurons = region->getNeurons();
                 for (auto& neuron : neurons) {
                     auto synapses = neuron->getOutgoingSynapses();
                     for (auto& synapse : synapses) {
                         if (synapse->getPostNeuron() == post && synapse->getPreNeuron() == pre) {
                             synapse->setPlasticityFlags(flags);
                             return;
                         }
                     }
                 }
             }
         }, py::arg("pre"), py::arg("post"), py::arg("flags"),
             "Plasticity control")
        .def("getConnectionInfo", [](Brain& self) {
             py::list connections;
             auto regions = self.getRegions();
             for (auto& region : regions) {
                 auto neurons = region->getNeurons();
                 for (auto& neuron : neurons) {
                     auto synapses = neuron->getOutgoingSynapses();
                     for (auto& synapse : synapses) {
                         py::dict connInfo;
                         connInfo["pre_neuron"] = synapse->getPreNeuron().value();
                         connInfo["post_neuron"] = synapse->getPostNeuron().value();
                         connInfo["weight"] = synapse->getWeight();
                         connInfo["delay"] = synapse->getDelay();
                         connInfo["type"] = static_cast<int>(synapse->getType());
                         connInfo["plasticity_flags"] = static_cast<int>(synapse->getPlasticityFlags());
                         connections.append(connInfo);
                     }
                 }
             }
             return connections;
         }, "Get all connection details")
        .def("getMemoryStats", [](Brain& self) {
             auto workingMem = self.getWorkingMemory();
             auto episodicMem = self.getEpisodicMemory();
             auto assocMem = self.getAssociativeMemory();
             
             py::dict stats;
             if (workingMem) {
                 stats["working_memory"] = py::dict({
                     {"trace_count", workingMem->getTraceCount()},
                     {"max_capacity", workingMem->getMaxCapacity()},
                     {"decay_rate", workingMem->getDecayRate()},
                     {"activation_threshold", workingMem->getActivationThreshold()}
                 });
             }
             
             if (episodicMem) {
                 stats["episodic_memory"] = py::dict({
                     {"episodes_count", episodicMem->getEpisodicCount()},
                     {"max_episodes", episodicMem->getMaxEpisodicCount()},
                     {"time_window", episodicMem->getTimeWindow()}
                 });
             }
             
             if (assocMem) {
                 stats["associative_memory"] = py::dict({
                     {"association_count", assocMem->getAssociationCount()},
                     {"max_associations", assocMem->getMaxAssociationCount()},
                     {"similarity_threshold", assocMem->getSimilarityThreshold()}
                 });
             }
             
             return stats;
         }, "Get detailed memory system status")
        .def("getActiveTraces", [](Brain& self) {
             auto workingMem = self.getWorkingMemory();
             py::list traces;
             if (!workingMem) return traces;
             
             auto activeTraces = workingMem->getActiveTraces();
             for (auto& trace : activeTraces) {
                 py::dict traceInfo;
                 traceInfo["id"] = trace.id;
                 traceInfo["age"] = trace.age;
                 traceInfo["strength"] = trace.strength;
                 traceInfo["pattern_id"] = trace.patternId;
                 traces.append(traceInfo);
             }
             return traces;
         }, "Working memory analysis")
        .def("getEpisodicMemory", [](Brain& self) {
             auto episodicMem = self.getEpisodicMemory();
             py::list episodes;
             if (!episodicMem) return episodes;
             
             auto episodicList = episodicMem->getEpisodicMemory();
             for (const auto& episode : episodicList) {
                 py::dict episodeInfo;
                 episodeInfo["id"] = episode.id;
                 episodeInfo["start_time"] = episode.startTime;
                 episodeInfo["end_time"] = episode.endTime;
                 episodeInfo["reward"] = episode.reward;
                 episodeInfo["context_hash"] = episode.contextHash;
                 episodes.append(episodeInfo);
             }
             return episodes;
         }, "Episodic memory access");

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
        .def("setCuriosityLevel", [](AgentBrain& self, float level) {
             self.curiosityLevel_ = level;
         }, py::arg("level"),
             "Direct neuromodulation control")
        .def("setDopamineLevel", [](AgentBrain& self, float level) {
             self.dopamineLevel_ = level;
         }, py::arg("level"),
             "Direct dopamine control")
        .def("customLearningRate", [](AgentBrain& self, float rate) {
             self.plasticityModifier_ = rate;
         }, py::arg("rate"),
             "Custom plasticity rates")
        .def("setDevelopmentStage", &AgentBrain::getDevelopmentalStage,
             py::arg("stage"),
             "Direct development control")
        .def("setMaturationRate", [](AgentBrain& self, double rate) {
             self.developmentalAge_ = 1.0 / rate;  
         }, py::arg("rate"),
             "Control developmental timing");

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

    // BrainProfiler bindings
    py::class_<nlm::BrainProfiler>(m, "BrainProfiler", R"pbdoc(NLM Brain Performance Profiler)pbdoc")
        .def(py::init<>())
        .def("record_simulation_metrics", &nlm::BrainProfiler::recordSimulationMetrics,
             py::arg("step"), py::arg("simulation_time"), py::arg("neurons_processed"),
             py::arg("spikes_processed"), py::arg("memory_stats"), py::arg("firing_stats"),
             "Record simulation metrics")
        .def("record_wall_clock_time", &nlm::BrainProfiler::recordWallClockTime,
             py::arg("operation"), py::arg("time_ns"), "Record wall clock time")
        .def("record_memory_allocation", &nlm::BrainProfiler::recordMemoryAllocation,
             py::arg("bytes"), py::arg("context") = "", "Record memory allocation")
        .def("record_firing_pattern", &nlm::BrainProfiler::recordFiringPattern,
             py::arg("neuron_id"), py::arg("firing_rate"), py::arg("activation"),
             "Record neuron firing pattern")
        .def("get_metrics", &nlm::BrainProfiler::getMetrics,
             py::arg("name"), "Get performance metrics by name")
        .def("get_memory_stats", &nlm::BrainProfiler::getMemoryStats, "Get memory statistics")
        .def("get_firing_stats", &nlm::BrainProfiler::getFiringStats, "Get firing statistics")
        .def("get_average_benchmark", &nlm::BrainProfiler::getAverageBenchmark,
             py::arg("scenario"), "Get average benchmark result")
        .def("generate_report", &nlm::BrainProfiler::generateReport, "Generate performance report")
        .def("get_performance_time_series", &nlm::BrainProfiler::getPerformanceTimeSeries,
             py::arg("metric_name"), "Get performance time series data")
        .def("reset", &nlm::BrainProfiler::reset, "Reset profiler state")
        .def("export_to_json", &nlm::BrainProfiler::exportToJSON, "Export metrics to JSON")
        .def("export_to_csv", &nlm::BrainProfiler::exportToCSV, "Export metrics to CSV")
        .def("detect_bottlenecks", &nlm::BrainProfiler::detectBottlenecks,
             "Detect performance bottlenecks")
        .def("get_system_health", &nlm::BrainProfiler::getSystemHealth,
             "Get system health metrics");

    // DebugTools bindings
    py::class_<nlm::DebugTools>(m, "DebugTools", R"pbdoc(NLM Brain Debugging Utilities)pbdoc")
        .def(py::init<>())
        .def("validate_state", &nlm::DebugTools::validateState,
             py::arg("brain"), "Validate brain state for consistency")
        .def("check_memory_leaks", &nlm::DebugTools::checkMemoryLeaks,
             "Check for memory leaks")
        .def("analyze_performance", &nlm::DebugTools::analyzePerformance,
             "Analyze performance for bottlenecks")
        .def("analyze_step", &nlm::DebugTools::analyzeStep,
             py::arg("step"), "Analyze a specific simulation step")
        .def("get_system_health", &nlm::DebugTools::getSystemHealth,
             "Get system health metrics")
        .def("take_network_snapshot", &nlm::DebugTools::takeNetworkSnapshot,
             py::arg("brain"), "Take a snapshot for visualization")
        .def("get_visualization_data", &nlm::DebugTools::getVisualizationData,
             py::arg("snapshot"), "Get neural network visualization data")
        .def("check_connectivity", &nlm::DebugTools::checkConnectivity,
             py::arg("brain"), "Check neural connectivity")
        .def("analyze_firing_patterns", &nlm::DebugTools::analyzeFiringPatterns,
             py::arg("brain"), "Analyze neuron firing patterns")
        .def("validate_neuromodulation", &nlm::DebugTools::validateNeuromodulation,
             py::arg("brain"), "Validate neuromodulation signals")
        .def("get_step_analysis_history", &nlm::DebugTools::getStepAnalysisHistory,
             py::arg("max_history") = 100, "Get step analysis history")
        .def("get_performance_history", &nlm::DebugTools::getPerformanceHistory,
             py::arg("max_history") = 100, "Get performance history")
        .def("get_memory_leak_history", &nlm::DebugTools::getMemoryLeakHistory,
             py::arg("max_history") = 100, "Get memory leak history")
        .def("generate_debug_report", &nlm::DebugTools::generateDebugReport,
             "Generate debugging report")
        .def("export_debug_data", &nlm::DebugTools::exportDebugData,
             py::arg("filepath"), "Export debug data to file")
        .def("import_debug_data", &nlm::DebugTools::importDebugData,
             py::arg("filepath"), "Import debug data from file")
        .def("clear_debug_history", &nlm::DebugTools::clearDebugHistory,
             "Clear all debug history")
        .def("get_version", &nlm::DebugTools::getVersion, "Get debug tools version");

    // PerformanceMetrics bindings
    py::class_<nlm::PerformanceMetrics> performanceMetricsClass(m, "PerformanceMetrics",
        R"pbdoc(NLM Performance Metrics Collection and Analysis)pbdoc");
    
    // Register with pybind11
    py::class_<nlm::PerformanceMetrics::TimeMetrics> timeMetricsClass(
        performanceMetricsClass, "TimeMetrics", py::dynamic_attr());
    
    py::class_<nlm::PerformanceMetrics::MemoryMetrics> memoryMetricsClass(
        performanceMetricsClass, "MemoryMetrics", py::dynamic_attr());
    
    py::class_<nlm::PerformanceMetrics::NetworkMetrics> networkMetricsClass(
        performanceMetricsClass, "NetworkMetrics", py::dynamic_attr());
    
    py::class_<nlm::PerformanceMetrics::IntegrationMetrics> integrationMetricsClass(
        performanceMetricsClass, "IntegrationMetrics", py::dynamic_attr());
    
    py::class_<nlm::PerformanceMetrics::SystemHealthMetrics> healthMetricsClass(
        performanceMetricsClass, "SystemHealthMetrics", py::dynamic_attr());
    
    py::class_<nlm::PerformanceMetrics::PerformanceReport> reportClass(
        performanceMetricsClass, "PerformanceReport", py::dynamic_attr());
    
    // PerformanceMetrics constructor
    performanceMetricsClass
        .def(py::init<>())
        .def("update_time_metrics", &nlm::PerformanceMetrics::updateTimeMetrics,
             py::arg("wall_time"), py::arg("sim_time"), py::arg("step_duration"),
             "Update time metrics")
        .def("update_memory_metrics", &nlm::PerformanceMetrics::updateMemoryMetrics,
             py::arg("used"), py::arg("free"), py::arg("allocated_bytes") = 0,
             "Update memory metrics")
        .def("update_network_metrics", &nlm::PerformanceMetrics::updateNetworkMetrics,
             py::arg("total_neurons"), py::arg("active_neurons"), py::arg("firing_neurons"),
             py::arg("total_synapses"), py::arg("avg_firing"), py::arg("eiratio"),
             "Update network metrics")
        .def("update_integration_metrics", &nlm::PerformanceMetrics::updateIntegrationMetrics,
             py::arg("steps"), py::arg("reward"), py::arg("episodes"),
             py::arg("dopamine"), py::arg("curiosity"), "Update integration metrics")
        .def("update_health_metrics", &nlm::PerformanceMetrics::updateHealthMetrics,
             py::arg("cpu_usage"), py::arg("memory_usage"), py::arg("stability"),
             py::arg("has_leaks"), py::arg("is_stable"), "Update health metrics")
        .def("get_performance_report", &nlm::PerformanceMetrics::getPerformanceReport,
             "Get complete performance report")
        .def("export_to_json", &nlm::PerformanceMetrics::exportToJSON,
             "Export metrics to JSON")
        .def("export_to_csv", &nlm::PerformanceMetrics::exportToCSV,
             "Export metrics to CSV")
        .def("reset", &nlm::PerformanceMetrics::reset, "Reset all metrics")
        .def("merge", &nlm::PerformanceMetrics::merge,
             py::arg("other"), "Merge metrics from another PerformanceMetrics instance");

    // Initialize submodules
    auto brainProfiler = std::make_shared<nlm::BrainProfiler>();
    auto debugTools = std::make_shared<nlm::DebugTools>();
    debugTools->setProfiler(brainProfiler);
}

} // namespace nlm
