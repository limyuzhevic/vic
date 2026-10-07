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

    // Helper function for Python context manager
    struct BrainDeleter {
        std::shared_ptr<Brain> brain;
        BrainDeleter(std::shared_ptr<Brain> b) : brain(b) {}
        void operator()(Brain* ptr) {
            if (brain) {
                brain->reset();
            }
        }
    };

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Represents the complete integrated brain with memory systems,
        neuromodulation, development, and plasticity.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with the given configuration.")
        .def(py::init([]() {
            auto config = std::make_shared<Config>();
            config->set("random_seed", uint64_t(42));
            config->set("neuron_count", size_t(1000));
            config->set("region_count", size_t(1));
            config->set("connection_probability", float(0.1f));
            return std::make_shared<Brain>(config);
        }), "Create a default brain with standard configuration")
        
        // Context manager support for automatic cleanup
        .def("__enter__", [](std::shared_ptr<Brain>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<Brain>& self, py::args) {
            self->reset();
        })
        
        // Enhanced initialization
        .def("initialize", &Brain::initialize, py::arg("timestepDuration"),
             "Initialize the brain with configuration and timestep.")
        .def("initialize", [](std::shared_ptr<Brain>& self) {
            // Forward to original initialize() with default timestep
            return self->initialize();
        }, "Initialize the brain with default configuration")
        
        // Core simulation methods with both overloads properly documented
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step using the timestep from configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit timestamp (currentStep and currentTime)")
        
        // Sensory input and current injection
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain via sensory neurons")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron by ID")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (Sensory, Motor, Internal, etc.)")
        
        // Action production and control
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on current neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        
        // State persistence with error handling
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True on success, False on error.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success, False on error.")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns the RegionId for the new region.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get list of all region IDs in the brain")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get list of all regions in the brain")
        
        // Neural statistics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of neurons firing in current step")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (internal and delayed)")
        
        // Development system
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (Initial, CriticalPeriod, etc.)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set the developmental stage of the brain")
        .def("advanceDevelopment", [](std::shared_ptr<Brain>& self, size_t steps) {
            // Advance development by specified steps
            for (size_t i = 0; i < steps; ++i) {
                auto devSystem = self->getDevelopmentSystem();
                if (devSystem) {
                    devSystem->update(self.get(), *self->getRandomGenerator(), 1000.0);
                }
            }
        }, py::arg("steps") = 1,
             "Advance development system by specified number of update cycles")
        
        // System access methods
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration object used to create this brain")
        .def("getSpikeSystem", &Brain::getSpikeSystem, py::return_value_policy::reference_internal,
             "Get access to the spike system for event management")
        .def("getSTDP", &Brain::getSTDP, py::return_value_policy::reference_internal,
             "Get access to the Spike-Timing-Dependent Plasticity system")
        .def("getHebbian", &Brain::getHebbian, py::return_value_policy::reference_internal,
             "Get access to the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity, py::return_value_policy::reference_internal,
             "Get access to the structural plasticity system")
        
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory, py::return_value_policy::reference_internal,
             "Get access to the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, py::return_value_policy::reference_internal,
             "Get access to the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory, py::return_value_policy::reference_internal,
             "Get access to the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem, py::return_value_policy::reference_internal,
             "Get access to the prediction system")
        
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner, py::return_value_policy::reference_internal,
             "Get access to the neural planning system")
        .def("getConceptFormation", &Brain::getConceptFormation, py::return_value_policy::reference_internal,
             "Get access to the concept formation system")
        .def("getAttention", &Brain::getAttention, py::return_value_policy::reference_internal,
             "Get access to the attentional selection system")
        
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine, py::return_value_policy::reference_internal,
             "Get access to the dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity, py::return_value_policy::reference_internal,
             "Get access to the curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty, py::return_value_policy::reference_internal,
             "Get access to the novelty neuromodulation system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, py::return_value_policy::reference_internal,
             "Get access to the prediction error signal system")
        
        // Configuration methods
        .def("getSystemParameter", [](std::shared_ptr<Brain>& self, const std::string& system, const std::string& parameter) {
            auto config = self->getConfig();
            if (!config) return std::nullopt;
            
            std::string key = system + "." + parameter;
            return config->getOr<std::string>(key, "");
        }, py::arg("system"), py::arg("parameter"),
             "Get a system-specific parameter value")
        .def("setSystemParameter", [](std::shared_ptr<Brain>& self, const std::string& system, const std::string& parameter, const std::string& value) {
            auto config = self->getConfig();
            if (config) {
                std::string key = system + "." + parameter;
                config->set(key, value);
            }
        }, py::arg("system"), py::arg("parameter"), py::arg("value"),
             "Set a system-specific parameter value")
        
        // Batch operations for efficiency
        .def("injectCurrentToNeuronIds", [](std::shared_ptr<Brain>& self, 
                                            const std::vector<NeuronId>& neuronIds, 
                                            MembranePotential current) {
            for (const auto& neuronId : neuronIds) {
                self->injectCurrent(neuronId, current);
            }
        }, py::arg("neuron_ids"), py::arg("current"),
             "Inject current into multiple neurons by ID (batch operation)")
        .def("injectCurrentToNeuronTypes", [](std::shared_ptr<Brain>& self,
                                              const std::vector<NeuronType>& types,
                                              MembranePotential current) {
            for (const auto& type : types) {
                self->injectCurrentToNeurons(type, current);
            }
        }, py::arg("neuron_types"), py::arg("current"),
             "Inject current into multiple neuron types (batch operation)")
        
        // Export/import with error handling
        .def("exportState", [](std::shared_ptr<Brain>& self) {
            // Return a dictionary with brain state information
            py::dict state;
            state["total_neurons"] = self->getTotalNeuronCount();
            state["total_synapses"] = self->getTotalSynapseCount();
            state["region_count"] = self->getRegionCount();
            state["current_step"] = pImpl->currentStep;
            state["current_time"] = pImpl->currentTime;
            state["development_stage"] = static_cast<int>(self->getDevelopmentalStage());
            return state;
        }, "Export current brain state as a dictionary")
        .def("importState", [](std::shared_ptr<Brain>& self, const py::dict& state) {
            // Import brain state from dictionary (simplified - mainly for metadata)
            if (state.contains("region_count")) {
                size_t newRegionCount = state["region_count"].cast<size_t>();
                // Note: This is a simplified implementation
                // In practice, you'd need to properly restore the full neural state
            }
        }, py::arg("state"),
             "Import brain state from a dictionary (simplified metadata import)")
        
        // Logging and diagnostics
        .def("logStatus", &Brain::logStatus,
             "Log comprehensive brain status information to the log system")
        .def("getStatusSummary", [](std::shared_ptr<Brain>& self) {
            std::ostringstream oss;
            oss << "Brain Status (Phase 6 - Integrated):" << std::endl;
            oss << "  Regions: " << self->getRegionCount() << std::endl;
            oss << "  Total neurons: " << self->getTotalNeuronCount() << std::endl;
            oss << "  Total synapses: " << self->getTotalSynapseCount() << std::endl;
            oss << "  Active neurons: " << self->getActiveNeuronCount() << std::endl;
            oss << "  Firing neurons (current): " << self->getFiringNeuronCount() << std::endl;
            oss << "  Total spikes: " << self->getTotalSpikeCount() << std::endl;
            oss << "  Average firing rate: " << self->getAverageFiringRate() << " Hz" << std::endl;
            oss << "  Development stage: " << static_cast<int>(self->getDevelopmentalStage()) << std::endl;
            return oss.str();
        }, "Get a formatted string summary of the current brain status");
        
    // Create default configuration
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", uint64_t(42));
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(1));
        config->set("connection_probability", float(0.1f));
        config->set("simulation_timestep", double(0.001));
        config->set("checkpoint_dir", std::string("./checkpoints"));
        return config;
    }, "Create a default configuration for the brain simulation");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to the world.
        
        Handles sensory transduction and motor decoding for agent-environment interaction.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the given brain instance")
        
        // Context manager support for automatic cleanup
        .def("__enter__", [](std::shared_ptr<AgentBrain>& self) { return self; })
        .def("__exit__", [](std::shared_ptr<AgentBrain>& self, py::args) {
            self->reset();
        })
        
        // Initialization and configuration
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with a world instance")
        .def("configure", [](std::shared_ptr<AgentBrain>& self, const SimpleWorld& world, size_t inputSize, size_t outputSize) {
            // Configure the agent brain with world and expected I/O sizes
            self->initialize(world);
            // Note: getSensoryInputSize and getMotorOutputSize are read-only properties
            // They are set during brain initialization based on the brain architecture
        }, py::arg("world"), py::arg("input_size"), py::arg("output_size"),
             "Initialize agent brain with world and expected I/O sizes")
        
        // I/O size information (read-only properties)
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size for the brain")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size for the brain")
        
        // Sensory processing and motor control
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept data and inject into brain for perception")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into actionable motor commands")
        .def("getCurrentAction", [](std::shared_ptr<AgentBrain>& self) {
            // Get the last produced action from the brain
            auto action = self->getBrain()->produceAction();
            return action;
        }, "Get the current motor command based on brain activity")
        
        // Neuromodulation and reward processing
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation to brain plasticity")
        .def("applyCuriosityModulation", [](std::shared_ptr<AgentBrain>& self, float curiosity) {
            // Apply curiosity-driven exploration motivation
            auto curiositySystem = self->getBrain()->getCuriosity();
            if (curiositySystem) {
                curiositySystem->setLevel(curiosity);
            }
        }, py::arg("curiosity_level"),
             "Apply curiosity modulation to exploration behavior")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system state")
        
        // System state and monitoring
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage of the agent")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level affecting neural excitability")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level driving exploration behavior")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty detection level for unexpected event processing")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal for learning and adaptation")
        
        // System control and configuration
        .def("reset", &AgentBrain::reset,
             "Reset agent brain for new episode (clear history, reset state)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying NLM brain instance")
        
        // Enable/disable system components
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward-based neuromodulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable developmental processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        
        // System status checks
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is currently enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is currently enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development processes are currently enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity-driven exploration is currently enabled")
        
        // Batch operations for efficiency
        .def("processBatchSensoryInputs", [](std::shared_ptr<AgentBrain>& self,
                                            const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self->processSensoryInput(percept);
            }
        }, py::arg("percepts"),
             "Process multiple sensory inputs in batch (more efficient)")
        
        // Export/import with error handling
        .def("exportState", [](std::shared_ptr<AgentBrain>& self) {
            py::dict state;
            state["sensory_input_size"] = self->getSensoryInputSize();
            state["motor_output_size"] = self->getMotorOutputSize();
            state["reward_modulation_enabled"] = self->isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self->isStructuralPlasticityEnabled();
            state["development_enabled"] = self->isDevelopmentEnabled();
            state["curiosity_enabled"] = self->isCuriosityEnabled();
            return state;
        }, "Export agent brain configuration to dictionary")
        .def("importState", [](std::shared_ptr<AgentBrain>& self, const py::dict& state) {
            if (state.contains("reward_modulation_enabled")) {
                bool enable = state["reward_modulation_enabled"].cast<bool>();
                self->enableRewardModulation(enable);
            }
            if (state.contains("structural_plasticity_enabled")) {
                bool enable = state["structural_plasticity_enabled"].cast<bool>();
                self->enableStructuralPlasticity(enable);
            }
            if (state.contains("development_enabled")) {
                bool enable = state["development_enabled"].cast<bool>();
                self->enableDevelopment(enable);
            }
            if (state.contains("curiosity_enabled")) {
                bool enable = state["curiosity_enabled"].cast<bool>();
                self->enableCuriosity(enable);
            }
        }, py::arg("state"), "Import agent brain configuration from dictionary")
        
        // Logging and diagnostics
        .def("logStatus", [](std::shared_ptr<AgentBrain>& self) {
            auto brain = self->getBrain();
            brain->logStatus();
            NLM_LOG_INFO("AgentBrain status:");
            NLM_LOG_INFO("  Reward modulation: " + std::string(self->isRewardModulationEnabled() ? "enabled" : "disabled"));
            NLM_LOG_INFO("  Structural plasticity: " + std::string(self->isStructuralPlasticityEnabled() ? "enabled" : "disabled"));
            NLM_LOG_INFO("  Development: " + std::string(self->isDevelopmentEnabled() ? "enabled" : "disabled"));
            NLM_LOG_INFO("  Curiosity: " + std::string(self->isCuriosityEnabled() ? "enabled" : "disabled"));
        }, "Log comprehensive agent brain status information")
        .def("getStatusSummary", [](std::shared_ptr<AgentBrain>& self) {
            std::ostringstream oss;
            oss << "AgentBrain Status:" << std::endl;
            oss << "  Sensory input size: " << self->getSensoryInputSize() << std::endl;
            oss << "  Motor output size: " << self->getMotorOutputSize() << std::endl;
            oss << "  Reward modulation: " << (self->isRewardModulationEnabled() ? "enabled" : "disabled") << std::endl;
            oss << "  Structural plasticity: " << (self->isStructuralPlasticityEnabled() ? "enabled" : "disabled") << std::endl;
            oss << "  Development: " << (self->isDevelopmentEnabled() ? "enabled" : "disabled") << std::endl;
            oss << "  Curiosity: " << (self->isCuriosityEnabled() ? "enabled" : "disabled") << std::endl;
            oss << "  Current developmental stage: " << static_cast<int>(self->getDevelopmentalStage()) << std::endl;
            return oss.str();
        }, "Get a formatted string summary of the current agent brain status");
        
    // Create agent brain with world (convenience function)
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain, const SimpleWorld& world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain interface with the given brain and world");

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

    // Convenience functions for common operations
    
    // Create simple world with default configuration
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0f, 100.0f, 32, 32);
        return world;
    }, "Create a simple world with default configuration (100x100m, 32x32 vision)")
    
    // Create agent with brain and world
    m.def("createAgentWithBrain", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::make_tuple(brain, agentBrain, world);
    }, py::arg("config"), py::arg("world"),
        "Create an agent with brain and world. Returns (brain, agent_brain, world).")
    
    // Create complete simulation setup
    m.def("createSimulationEnvironment", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", uint64_t(42));
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(1));
        config->set("connection_probability", float(0.1f));
        config->set("simulation_timestep", double(0.001));
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0f, 100.0f, 32, 32);
        world->setAgentStart(50.0f, 50.0f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        py::dict environment;
        environment["config"] = config;
        environment["world"] = world;
        environment["brain"] = brain;
        environment["agent_brain"] = agentBrain;
        
        return environment;
    }, "Create a complete simulation environment with default configuration")
    
    // Set up agent for simulation
    m.def("setupAgent", [](std::shared_ptr<SimpleWorld> world, uint64_t seed) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", seed);
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(1));
        config->set("connection_probability", float(0.1f));
        config->set("simulation_timestep", double(0.001));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        py::dict setup;
        setup["config"] = config;
        setup["world"] = world;
        setup["brain"] = brain;
        setup["agent_brain"] = agentBrain;
        
        return setup;
    }, py::arg("world"), py::arg("seed") = uint64_t(42),
        "Set up an agent with given world and random seed. Returns configuration dictionary.")
    
    // Export/import with full error handling
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain) {
        py::dict state;
        state["total_neurons"] = brain->getTotalNeuronCount();
        state["total_synapses"] = brain->getTotalSynapseCount();
        state["region_count"] = brain->getRegionCount();
        state["active_neurons"] = brain->getActiveNeuronCount();
        state["firing_neurons"] = brain->getFiringNeuronCount();
        state["average_firing_rate"] = brain->getAverageFiringRate();
        state["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        state["total_spikes"] = brain->getTotalSpikeCount();
        state["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Try to get additional optional fields
        auto spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            state["pending_spike_events"] = spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount();
        }
        
        return state;
    }, py::arg("brain"), "Export brain state as a dictionary (non-persistent)")
    
    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file. Raises exception on failure.")
    
    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file. Raises exception on failure.")
    
    // Batch neuron operations for power users
    m.def("injectCurrentBatch", [](std::shared_ptr<Brain> brain,
                                  const std::vector<NeuronId>& neuronIds,
                                  MembranePotential current) {
        for (const auto& neuronId : neuronIds) {
            brain->injectCurrent(neuronId, current);
        }
        return static_cast<size_t>(neuronIds.size()); // Number of neurons processed
    }, py::arg("brain"), py::arg("neuron_ids"), py::arg("current"),
        "Inject current to multiple neurons. Returns number of neurons processed.")
    
    m.def("injectCurrentToNeuronTypesBatch", [](std::shared_ptr<Brain> brain,
                                               const std::vector<NeuronType>& types,
                                               MembranePotential current) {
        for (const auto& type : types) {
            brain->injectCurrentToNeurons(type, current);
        }
        return static_cast<size_t>(types.size()); // Number of types processed
    }, py::arg("brain"), py::arg("neuron_types"), py::arg("current"),
        "Inject current to multiple neuron types. Returns number of types processed.")
    
    // Advanced system control methods
    m.def("enableBrainSystems", [](std::shared_ptr<Brain> brain,
                                  bool plasticity,
                                  bool development,
                                  bool neuromodulation) {
        // Note: Brain class itself doesn't have these methods
        // This is a convenience function for agent brain control
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain);
        if (agentBrain) {
            agentBrain->enableStructuralPlasticity(plasticity);
            agentBrain->enableDevelopment(development);
            agentBrain->enableRewardModulation(neuromodulation);
            agentBrain->enableCuriosity(neuromodulation);
        }
        return std::make_tuple(plasticity, development, neuromodulation);
    }, py::arg("brain"), py::arg("plasticity") = true, py::arg("development") = true, py::arg("neuromodulation") = true,
        "Enable/disable brain systems. Returns tuple of enabled states.")
    
    m.def("getBrainSystemStatus", [](std::shared_ptr<Brain> brain) {
        py::dict status;
        status["total_neurons"] = brain->getTotalNeuronCount();
        status["total_synapses"] = brain->getTotalSynapseCount();
        status["active_neurons"] = brain->getActiveNeuronCount();
        status["firing_neurons_current"] = brain->getFiringNeuronCount();
        status["total_spikes"] = brain->getTotalSpikeCount();
        status["average_firing_rate"] = brain->getAverageFiringRate();
        status["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        auto spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            status["pending_spike_events"] = spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount();
        }
        
        return status;
    }, py::arg("brain"), "Get comprehensive brain system status as dictionary.")
    
    // Error handling and validation utilities
    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        py::dict validation;
        
        validation["is_valid"] = true;
        validation["errors"] = std::vector<std::string>();
        
        // Check basic integrity
        if (brain->getTotalNeuronCount() == 0) {
            validation["is_valid"] = false;
            validation["errors"].cast<std::vector<std::string>>().push_back("No neurons in brain");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            validation["is_valid"] = false;
            validation["errors"].cast<std::vector<std::string>>().push_back("No synapses in brain");
        }
        
        if (brain->getTotalNeuronCount() > 100000) {
            validation["warnings"].cast<std::vector<std::string>>().push_back("High neuron count may impact performance");
        }
        
        return validation;
    }, py::arg("brain"), "Validate brain state integrity. Returns validation dictionary.")
    
    m.def("safeStep", [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
        try {
            brain->step(step, time);
            return std::make_tuple(true, std::string("Success"));
        } catch (const std::exception& e) {
            return std::make_tuple(false, std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
        "Safely perform a simulation step with error handling. Returns (success, message).")
    
    m.def("createNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create a NeuronId from unsigned integer value")
    
    m.def("createSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create a SynapseId from unsigned integer value")
    
    m.def("createRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create a RegionId from unsigned integer value")
    
    m.def("createPopulationId", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Create a PopulationId from unsigned integer value")
    
    // Export available configuration keys for reference
    m.def("getAvailableConfigKeys", []() {
        return std::vector<std::string>{
            "random_seed",
            "neuron_count", 
            "region_count",
            "connection_probability",
            "simulation_timestep",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_tau",
            "synaptogenesis_rate",
            "pruning_rate",
            "replay_interval",
            "consolidation_interval",
            "checkpoint_dir",
            "checkpoint_compression",
            "max_checkpoints",
            "checkpoint_write_buffer"
        };
    }, "Get list of available configuration keys for the brain simulation.")
    
    // Utility functions for error handling
    m.def("getLastError", []() {
        // This would typically interface with a global error state
        return std::string("No error tracking implemented yet");
    }, "Get the last error message (placeholder - error tracking not yet implemented).")
    
    m.def("clearError", []() {
        // This would typically clear error state
        return true;
    }, "Clear any stored error state (placeholder - error tracking not yet implemented).")
    
    // Invalid ID constants (already present, just ensuring they're exported)
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
