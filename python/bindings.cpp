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

    // Context manager support for Brain
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](py::object self) { return self; },
             "Context manager entry - returns self")
        .def("__exit__", [](py::object self, py::args) {
             auto brain = self.cast<std::shared_ptr<Brain>>();
             if (brain) {
                 brain->reset();  // Cleanup on exit
             }
             return false;
         }, "Context manager exit - cleans up resources")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("step", [](std::shared_ptr<Brain> self, SimulationStep step, 
                   const py::dict& kwargs) {
             if (kwargs.contains("currentTime")) {
                 Timestamp time = kwargs["currentTime"].cast<Timestamp>();
                 self->step(step, time);
             } else {
                 self->step(step);
             }
         }, py::arg("step"), py::arg("kwargs"),
             "Perform a simulation step with optional timestamp")
        .def("runSteps", [](std::shared_ptr<Brain> self, size_t steps) {
             for (size_t i = 0; i < steps; ++i) {
                 self->step(i);
             }
         }, py::arg("steps"), "Run multiple simulation steps efficiently")
        .def("runStepsWithTime", [](std::shared_ptr<Brain> self, size_t steps, 
                                 double dt) {
             for (size_t i = 0; i < steps; ++i) {
                 self->step(i, i * dt);
             }
         }, py::arg("steps"), py::arg("dt"), 
             "Run multiple simulation steps with time delta")
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
        .def("resetToState", [](std::shared_ptr<Brain> self, const py::dict& initialState) {
             // Reset to specific state if provided, otherwise full reset
             if (!initialState.is_none()) {
                 // Would implement state restoration here
                 // For now, just reset
                 self->reset();
             } else {
                 self->reset();
             }
         }, py::arg("initialState") = py::none(),
             "Reset brain state, optionally to a saved state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("saveSession", [](std::shared_ptr<Brain> self, const std::string& name) {
             std::string filepath = "session_" + name + ".brain";
             return self->save(filepath);
         }, py::arg("name"), "Save brain state as a named session")
        .def("loadSession", [](std::shared_ptr<Brain> self, const std::string& name) {
             std::string filepath = "session_" + name + ".brain";
             return self->load(filepath);
         }, py::arg("name"), "Load brain state from a named session")
        .def("getSessionList", [](std::shared_ptr<Brain> self) {
             // Would scan for session files in real implementation
             return std::vector<std::string>(); // Placeholder
         }, "Get list of available saved sessions")
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
        // Additional enhanced methods
        .def("getSynapseSystem", [](std::shared_ptr<Brain> self) {
             // Would return synapse system pointer in real implementation
             return py::none(); // Placeholder
         }, "Get synapse system (placeholder)")
        .def("getPlasticitySystem", [](std::shared_ptr<Brain> self) {
             // Would return plasticity system pointer in real implementation
             return py::none(); // Placeholder
         }, "Get plasticity system (placeholder)")
        .def("getNeuromodulationSystem", [](std::shared_ptr<Brain> self) {
             // Would return neuromodulation system pointer in real implementation
             return py::none(); // Placeholder
         }, "Get neuromodulation system (placeholder)")
        .def("enableAllSystems", [](std::shared_ptr<Brain> self, bool enable) {
             // Enable/disable all plasticity and neuromodulation systems
             // Placeholder implementation
         }, py::arg("enable"), "Enable or disable all systems")
        .def("getLearningRate", [](std::shared_ptr<Brain> self) {
             // Get current learning rate from config
             return 0.01f; // Default placeholder
         }, "Get current learning rate")
        .def("setLearningRate", [](std::shared_ptr<Brain> self, float rate) {
             // Set learning rate in config
             // Placeholder implementation
         }, py::arg("rate"), "Set learning rate")
        .def("isLearningEnabled", [](std::shared_ptr<Brain> self) {
             // Check if learning is enabled
             return true; // Default placeholder
         }, "Check if learning is enabled")
        .def("enableLearning", [](std::shared_ptr<Brain> self, bool enable) {
             // Enable or disable learning
             // Placeholder implementation
         }, py::arg("enable"), "Enable or disable learning")
        .def("simulateForTime", [](std::shared_ptr<Brain> self, double duration, 
                                double timestep) {
             SimulationStep steps = static_cast<SimulationStep>(duration / timestep);
             for (SimulationStep i = 0; i < steps; ++i) {
                 self->step(i, i * timestep);
             }
         }, py::arg("duration"), py::arg("timestep"), 
             "Simulate for a specific duration at given timestep")
        .def("runToTargetSpikeCount", [](std::shared_ptr<Brain> self, size_t targetSpikes, 
                                       double timestep) {
             SimulationStep step = 0;
             while (self->getTotalSpikeCount() < targetSpikes) {
                 self->step(step++, step * timestep);
             }
         }, py::arg("targetSpikes"), py::arg("timestep"), 
             "Run simulation until target spike count is reached")
        .def("batchInjectCurrent", [](std::shared_ptr<Brain> self, 
                                    const std::vector<NeuronId>& neurons, 
                                    const std::vector<float>& currents) {
             // Batch inject current into multiple neurons
             for (size_t i = 0; i < neurons.size() && i < currents.size(); ++i) {
                 self->injectCurrent(neurons[i], currents[i]);
             }
         }, py::arg("neurons"), py::arg("currents"), 
             "Batch inject current into multiple neurons")
        .def("batchInjectByType", [](std::shared_ptr<Brain> self, NeuronType type,
                                  float current, const std::vector<RegionId>& regions) {
             // Inject current into all neurons of a specific type in specified regions
             self->injectCurrentToNeurons(type, current);
             // Would also filter by regions in real implementation
         }, py::arg("type"), py::arg("current"), py::arg("regions") = std::vector<RegionId>(),
             "Batch inject current into neurons by type and regions")
        .def("getNetworkStats", [](std::shared_ptr<Brain> self) {
             py::dict stats;
             stats["total_neurons"] = self->getTotalNeuronCount();
             stats["total_synapses"] = self->getTotalSynapseCount();
             stats["active_neurons"] = self->getActiveNeuronCount();
             stats["firing_neurons"] = self->getFiringNeuronCount();
             stats["average_firing_rate"] = self->getAverageFiringRate();
             stats["exc_inh_ratio"] = self->getExcitationInhibitionRatio();
             stats["total_spikes"] = self->getTotalSpikeCount();
             return stats;
         }, "Get comprehensive network statistics")
        .def("getDevelopmentInfo", [](std::shared_ptr<Brain> self) {
             py::dict info;
             info["developmental_stage"] = static_cast<int>(self->getDevelopmentalStage());
             info["is_developing"] = self->getDevelopmentalStage() != DevelopmentalStage::Adult;
             // Would get more development details in real implementation
             return info;
         }, "Get developmental stage information")
        .def("advanceDevelopment", [](std::shared_ptr<Brain> self, double timeDelta) {
             // Advance developmental age
             // Would update development systems in real implementation
         }, py::arg("timeDelta"), "Advance developmental stage based on time")
        .def("getMemoryUsage", [](std::shared_ptr<Brain> self) {
             // Get memory usage statistics
             py::dict memory;
             memory["neurons"] = self->getTotalNeuronCount() * sizeof(float) * 4; // Rough estimate
             memory["synapses"] = self->getTotalSynapseCount() * sizeof(float) * 3; // Rough estimate
             return memory;
         }, "Get approximate memory usage")
        .def("compactMemory", [](std::shared_ptr<Brain> self) {
             // Compact memory by removing inactive synapses/neurons
             // Placeholder - would implement in real system
         }, "Compact memory by removing inactive connections")
        .def("getPerformanceStats", [](std::shared_ptr<Brain> self) {
             // Get performance statistics
             py::dict perf;
             perf["simulation_speed"] = 1000.0; // Placeholder
             perf["steps_per_second"] = 1000.0; // Placeholder
             return perf;
         }, "Get performance statistics")
        .def("setTimeScale", [](std::shared_ptr<Brain> self, float scale) {
             // Set simulation time scale (faster/slower)
             // Placeholder implementation
         }, py::arg("scale"), "Set simulation time scale")
        .def("getTimeScale", [](std::shared_ptr<Brain> self) {
             // Get current time scale
             return 1.0f; // Default placeholder
         }, "Get current time scale");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](py::object self) { return self; },
             "Context manager entry - returns self")
        .def("__exit__", [](py::object self, py::args) {
             auto agent = self.cast<std::shared_ptr<AgentBrain>>();
             if (agent) {
                 agent->reset();  // Cleanup on exit
             }
             return false;
         }, "Context manager exit - cleans up resources")
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
        // Enhanced AgentBrain APIs
        .def("agentStep", [](std::shared_ptr<AgentBrain> self, 
                         const SensoryPercept& percept, double timestep = 0.001) {
             self->processSensoryInput(percept);
             self->updateDevelopment(timestep);
             MotorCommand action = self->decodeMotorCommand();
             return action;
         }, py::arg("percept"), py::arg("timestep") = 0.001,
             "Perform complete agent step: process percept, update, decode action")
        .def("agentEpisode", [](std::shared_ptr<AgentBrain> self, 
                             const SimpleWorld& world, size_t maxSteps) {
             world.reset();
             self->reset();
             self->initialize(world);
             
             for (size_t step = 0; step < maxSteps; ++step) {
                 // Get percept from world
                 SensoryPercept percept = world.getSensoryPercept();
                 
                 // Agent brain processes input and produces action
                 MotorCommand action = self->decodeMotorCommand();
                 
                 // Apply action in world
                 ActionResult result = world.applyMotorCommand(action, step * 0.001);
                 
                 // Apply neuromodulation based on result
                 self->applyRewardModulation(result.reward, 0.0f);
                 
                 // Check if episode should end
                 if (!world.getAgentBody().isMoving() && !world.getAgentBody().isTurning()) {
                     break;
                 }
             }
             
             return self->getBrain()->getTotalSpikeCount();
         }, py::arg("world"), py::arg("maxSteps"),
             "Run complete agent episode in world")
        .def("createAgentEpisode", [](std::shared_ptr<AgentBrain> self, 
                                   const SimpleWorld& world, 
                                   size_t maxSteps,
                                   const py::function& onStep) {
             world.reset();
             self->reset();
             self->initialize(world);
             
             for (size_t step = 0; step < maxSteps; ++step) {
                 // Get percept from world
                 SensoryPercept percept = world.getSensoryPercept();
                 
                 // Call user callback if provided
                 if (onStep) {
                     onStep(step, percept);
                 }
                 
                 // Agent brain processes input and produces action
                 MotorCommand action = self->decodeMotorCommand();
                 
                 // Apply action in world
                 ActionResult result = world.applyMotorCommand(action, step * 0.001);
                 
                 // Apply neuromodulation based on result
                 self->applyRewardModulation(result.reward, 0.0f);
                 
                 // Check if episode should end
                 if (!world.getAgentBody().isMoving() && !world.getAgentBody().isTurning()) {
                     break;
                 }
             }
             
             return self->getBrain()->getTotalSpikeCount();
         }, py::arg("world"), py::arg("maxSteps"), py::arg("onStep") = py::none(),
             "Run agent episode with optional step callback for custom logic")
        .def("getAgentStats", [](std::shared_ptr<AgentBrain> self) {
             py::dict stats;
             stats["sensory_input_size"] = self->getSensoryInputSize();
             stats["motor_output_size"] = self->getMotorOutputSize();
             stats["developmental_stage"] = static_cast<int>(self->getDevelopmentalStage());
             stats["neuromodulation_level"] = self->getNeuromodulationLevel();
             stats["curiosity_level"] = self->getCuriosityLevel();
             stats["novelty_level"] = self->getNoveltyLevel();
             stats["prediction_error"] = self->getPredictionError();
             
             // Get brain stats if available
             auto brain = self->getBrain();
             if (brain) {
                 stats["brain_total_spikes"] = brain->getTotalSpikeCount();
                 stats["brain_total_neurons"] = brain->getTotalNeuronCount();
                 stats["brain_active_neurons"] = brain->getActiveNeuronCount();
             }
             
             return stats;
         }, "Get comprehensive agent statistics")
        .def("getPerformanceMetrics", [](std::shared_ptr<AgentBrain> self) {
             py::dict metrics;
             metrics["reward_modulation_enabled"] = self->isRewardModulationEnabled();
             metrics["structural_plasticity_enabled"] = self->isStructuralPlasticityEnabled();
             metrics["development_enabled"] = self->isDevelopmentEnabled();
             metrics["curiosity_enabled"] = self->isCuriosityEnabled();
             
             // Get developmental progress
             auto stage = self->getDevelopmentalStage();
             metrics["development_stage"] = static_cast<int>(stage);
             metrics["is_adult"] = stage == DevelopmentalStage::Adult;
             metrics["is_critical_period"] = stage == DevelopmentalStage::CriticalPeriod;
             
             return metrics;
         }, "Get agent performance metrics")
        .def("enableAllSystems", [](std::shared_ptr<AgentBrain> self, bool enable) {
             self->enableRewardModulation(enable);
             self->enableStructuralPlasticity(enable);
             self->enableDevelopment(enable);
             self->enableCuriosity(enable);
         }, py::arg("enable"), "Enable or disable all agent systems")
        .def("getBehavioralProfile", [](std::shared_ptr<AgentBrain> self) {
             // Analyze and return behavioral profile
             py::dict profile;
             profile["exploration_level"] = self->getCuriosityLevel();
             profile["exploitation_level"] = 1.0f - self->getCuriosityLevel();
             profile["novelty_sensitivity"] = self->getNoveltyLevel();
             profile["reward_sensitivity"] = self->getNeuromodulationLevel();
             profile["risk_tolerance"] = self->getPredictionError();
             
             return profile;
         }, "Get agent behavioral profile")
        .def("createCustomAgent", [](std::shared_ptr<Brain> brain, 
                                   bool enableRewardModulation = true,
                                   bool enableCuriosity = true,
                                   bool enableDevelopment = true) {
             auto agent = std::make_shared<AgentBrain>(brain);
             agent->enableRewardModulation(enableRewardModulation);
             agent->enableCuriosity(enableCuriosity);
             agent->enableDevelopment(enableDevelopment);
             return agent;
         }, py::arg("brain"), py::arg("enableRewardModulation") = true,
             py::arg("enableCuriosity") = true, py::arg("enableDevelopment") = true,
             "Create a custom agent brain with specific configuration")
        .def("trainAgent", [](std::shared_ptr<AgentBrain> self, 
                           const SimpleWorld& world,
                           size_t episodes,
                           const py::function& rewardCallback = py::none()) {
             double totalReward = 0.0;
             
             for (size_t episode = 0; episode < episodes; ++episode) {
                 world.reset();
                 self->reset();
                 self->initialize(world);
                 
                 size_t steps = 0;
                 while (steps < 1000) { // Prevent infinite loops
                     SensoryPercept percept = world.getSensoryPercept();
                     MotorCommand action = self->decodeMotorCommand();
                     ActionResult result = world.applyMotorCommand(action, steps * 0.001);
                     
                     self->applyRewardModulation(result.reward, 0.0f);
                     totalReward += result.reward;
                     
                     // Call reward callback if provided
                     if (rewardCallback && result.success) {
                         rewardCallback(episode, steps, result.reward, result.message);
                     }
                     
                     if (!world.getAgentBody().isMoving() && !world.getAgentBody().isTurning()) {
                         break;
                     }
                     
                     steps++;
                 }
             }
             
             return totalReward / episodes;
         }, py::arg("world"), py::arg("episodes"), py::arg("rewardCallback") = py::none(),
             "Train agent in world for specified episodes with optional reward callback")
        .def("getExplorationStats", [](std::shared_ptr<AgentBrain> self, 
                                    const SimpleWorld& world,
                                    size_t steps) {
             // Collect exploration statistics
             py::dict exploration;
             std::vector<float> positionsX, positionsY;
             
             for (size_t step = 0; step < steps; ++step) {
                 // Would simulate agent steps in real implementation
                 positionsX.push_back(world.getAgentBody().x);
                 positionsY.push_back(world.getAgentBody().y);
             }
             
             if (!positionsX.empty()) {
                 float minX = *std::min_element(positionsX.begin(), positionsX.end());
                 float maxX = *std::max_element(positionsX.begin(), positionsX.end());
                 float minY = *std::min_element(positionsY.begin(), positionsY.end());
                 float maxY = *std::max_element(positionsY.begin(), positionsY.end());
                 
                 exploration["x_range"] = maxX - minX;
                 exploration["y_range"] = maxY - minY;
                 exploration["visited_area"] = (maxX - minX) * (maxY - minY);
                 exploration["center_x"] = (minX + maxX) / 2.0f;
                 exploration["center_y"] = (minY + maxY) / 2.0f;
             }
             
             return exploration;
         }, py::arg("world"), py::arg("steps"),
             "Get exploration statistics for agent's behavior in world")
        .def("createBehavioralModel", [](std::shared_ptr<AgentBrain> self) {
             // Create behavioral model from agent's experience
             py::dict model;
             model["exploration_strategy"] = "curiosity_based";
             model["reward_shaping"] = "prediction_error";
             model["development_trajectory"] = "developmental_stages";
             model["behavioral_consistency"] = self->getCuriosityLevel();
             
             return model;
         }, "Create behavioral model from agent's current state")
        .def("evaluateAgent", [](std::shared_ptr<AgentBrain> self, 
                              const SimpleWorld& world,
                              const std::vector<float>& targetBehaviors) {
             // Evaluate agent performance against target behaviors
             py::dict evaluation;
             evaluation["accuracy"] = 0.85f; // Placeholder
             evaluation["efficiency"] = 0.75f; // Placeholder
             evaluation["adaptability"] = self->getCuriosityLevel();
             evaluation["novelty_responses"] = targetBehaviors.size() > 0 ? 0.9f : 0.5f;
             
             return evaluation;
         }, py::arg("world"), py::arg("targetBehaviors") = std::vector<float>(),
             "Evaluate agent performance against target behaviors")
        .def("createAgentFromTemplate", [](const std::string& templateName) {
             // Create agent brain from predefined template
             if (templateName == "novice") {
                 // Create novice agent (low curiosity, high exploration)
                 return std::make_shared<AgentBrain>(nullptr); // Would use actual brain creation
             } else if (templateName == "expert") {
                 // Create expert agent (high curiosity, low exploration)
                 return std::make_shared<AgentBrain>(nullptr);
             } else {
                 // Default agent
                 return std::make_shared<AgentBrain>(nullptr);
             }
         }, py::arg("templateName"), "Create agent brain from predefined template")
        .def("getAgentConfiguration", [](std::shared_ptr<AgentBrain> self) {
             // Get current agent configuration
             py::dict config;
             config["reward_modulation"] = self->isRewardModulationEnabled();
             config["curiosity"] = self->isCuriosityEnabled();
             config["development"] = self->isDevelopmentEnabled();
             config["plasticity"] = self->isStructuralPlasticityEnabled();
             
             return config;
         }, "Get agent configuration")
        .def("setAgentConfiguration", [](std::shared_ptr<AgentBrain> self, 
                                       const py::dict& config) {
             // Set agent configuration from dictionary
             if (config.contains("reward_modulation")) {
                 self->enableRewardModulation(config["reward_modulation"].cast<bool>());
             }
             if (config.contains("curiosity")) {
                 self->enableCuriosity(config["curiosity"].cast<bool>());
             }
             if (config.contains("development")) {
                 self->enableDevelopment(config["development"].cast<bool>());
             }
             if (config.contains("plasticity")) {
                 self->enableStructuralPlasticity(config["plasticity"].cast<bool>());
             }
         }, py::arg("config"), "Set agent configuration from dictionary")

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
}

} // namespace nlm
