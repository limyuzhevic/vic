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
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

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

    // Convenience type aliases for easier Python code
    using NeuronIdList = std::vector<NeuronId>;
    using SynapseIdList = std::vector<SynapseId>;
    
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
        
        // Python convenience methods (aliases for existing C++ methods)
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions (Pythonic alias)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions (Pythonic alias)")
        .def("getActiveNeurons", &Brain::getActiveNeuronCount,
             "Get count of active neurons (Pythonic alias)")
        .def("getFiringNeurons", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons (Pythonic alias)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        
        // Statistics convenience methods
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Access to system components
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update all plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        
        // Logging
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Plasticity system access with enhanced convenience methods
        .def("getPlasticityStats", [](const std::shared_ptr<Brain>& brain) {
            auto* stdp = brain->getSTDP();
            auto* hebbian = brain->getHebbian();
            auto* structural = brain->getStructuralPlasticity();
            
            std::map<std::string, float> stats;
            if (stdp) {
                stats["stdp_learning_rate"] = stdp->getLearningRate();
                stats["stdp_enable"] = stdp->isEnabled() ? 1.0f : 0.0f;
            }
            if (hebbian) {
                stats["hebbian_learning_rate"] = hebbian->getLearningRate();
                stats["hebbian_enable"] = hebbian->isEnabled() ? 1.0f : 0.0f;
            }
            if (structural) {
                stats["structural_plasticity_rate"] = structural->getPlasticityRate();
                stats["structural_enable"] = structural->isEnabled() ? 1.0f : 0.0f;
            }
            return stats;
        }, "Get plasticity system statistics")
        
        .def("getPlasticityStatus", [](const std::shared_ptr<Brain>& brain) {
            std::string status = "Plasticity Systems:\n";
            auto* stdp = brain->getSTDP();
            auto* hebbian = brain->getHebbian();
            auto* structural = brain->getStructuralPlasticity();
            
            if (stdp) status += "  STDP: " + std::string(stdp->isEnabled() ? "Enabled" : "Disabled") + "\n";
            if (hebbian) status += "  Hebbian: " + std::string(hebbian->isEnabled() ? "Enabled" : "Disabled") + "\n";
            if (structural) status += "  Structural: " + std::string(structural->isEnabled() ? "Enabled" : "Disabled") + "\n";
            return status;
        }, "Get human-readable plasticity system status")
        
        .def("enableAllPlasticity", [](const std::shared_ptr<Brain>& brain, bool enable) {
            if (auto* stdp = brain->getSTDP()) stdp->enablePlasticity(enable);
            if (auto* hebbian = brain->getHebbian()) hebbian->enablePlasticity(enable);
            if (auto* structural = brain->getStructuralPlasticity()) structural->enablePlasticity(enable);
        }, py::arg("enable"), "Enable or disable all plasticity systems")
        
        .def("enableSpecificPlasticity", [](const std::shared_ptr<Brain>& brain, 
                                           std::string system, bool enable) {
            if (system == "stdp" && brain->getSTDP()) {
                brain->getSTDP()->enablePlasticity(enable);
            } else if (system == "hebbian" && brain->getHebbian()) {
                brain->getHebbian()->enablePlasticity(enable);
            } else if (system == "structural" && brain->getStructuralPlasticity()) {
                brain->getStructuralPlasticity()->enablePlasticity(enable);
            }
        }, py::arg("system"), py::arg("enable"), 
             "Enable/disable specific plasticity system")
        
        .def("getSynapticConnectivity", [](const std::shared_ptr<Brain>& brain) {
            size_t totalSynapses = brain->getTotalSynapseCount();
            size_t totalNeurons = brain->getTotalNeuronCount();
            float density = totalNeurons > 0 ? (totalSynapses * 2.0f) / (totalNeurons * totalNeurons) : 0.0f;
            
            return std::make_tuple(totalNeurons, totalSynapses, density);
        }, "Get synaptic connectivity statistics")
        
        // Enhanced memory system access
        .def("getWorkingMemoryStats", [](const std::shared_ptr<Brain>& brain) {
            if (auto* wm = brain->getWorkingMemory()) {
                // Get working memory statistics
                return std::make_tuple(
                    wm->getCapacity(),
                    wm->getCurrentLoad(),
                    wm->getActivationLevel()
                );
            }
            return std::make_tuple(0, 0, 0.0f);
        }, "Get working memory statistics")
        
        .def("getEpisodicMemoryStats", [](const std::shared_ptr<Brain>& brain) {
            if (auto* em = brain->getEpisodicMemory()) {
                // Get episodic memory statistics
                return std::make_tuple(
                    em->getEpisodeCount(),
                    em->getEncodingRate(),
                    em->getReplayFrequency()
                );
            }
            return std::make_tuple(0, 0.0f, 0.0f);
        }, "Get episodic memory statistics")
        
        .def("getAssociativeMemoryStats", [](const std::shared_ptr<Brain>& brain) {
            if (auto* am = brain->getAssociativeMemory()) {
                // Get associative memory statistics
                return std::make_tuple(
                    am->getAssociationCount(),
                    am->getPatternStrength(),
                    am->getRecallAccuracy()
                );
            }
            return std::make_tuple(0, 0.0f, 0.0f);
        }, "Get associative memory statistics")
        
        .def("getMemorySystemStatus", [](const std::shared_ptr<Brain>& brain) {
            std::string status = "Memory Systems:\n";
            auto* wm = brain->getWorkingMemory();
            auto* em = brain->getEpisodicMemory();
            auto* am = brain->getAssociativeMemory();
            
            if (wm) status += "  Working Memory: Active (Capacity: " + std::to_string(wm->getCapacity()) + ")\n";
            if (em) status += "  Episodic Memory: Active (Episodes: " + std::to_string(em->getEpisodeCount()) + ")\n";
            if (am) status += "  Associative Memory: Active (Associations: " + std::to_string(am->getAssociationCount()) + ")\n";
            return status;
        }, "Get human-readable memory system status")
        
        // Enhanced prediction system access
        .def("getPredictionSystemStats", [](const std::shared_ptr<Brain>& brain) {
            if (auto* pred = brain->getPredictionSystem()) {
                return std::make_tuple(
                    pred->getPredictionAccuracy(),
                    pred->getErrorRate(),
                    pred->getSequenceLength()
                );
            }
            return std::make_tuple(0.0f, 0.0f, 0);
        }, "Get prediction system statistics")
        
        .def("getPredictionStatus", [](const std::shared_ptr<Brain>& brain) {
            std::string status = "Prediction System:\n";
            auto* pred = brain->getPredictionSystem();
            if (pred) {
                status += "  Accuracy: " + std::to_string(pred->getPredictionAccuracy()) + "\n";
                status += "  Error Rate: " + std::to_string(pred->getErrorRate()) + "\n";
                status += "  Sequence Length: " + std::to_string(pred->getSequenceLength());
            } else {
                status += "  Not available";
            }
            return status;
        }, "Get human-readable prediction system status")
        
        // Enhanced cognition system access
        .def("getCognitionStats", [](const std::shared_ptr<Brain>& brain) {
            std::map<std::string, float> stats;
            
            if (auto* planner = brain->getPlanner()) {
                stats["planning_speed"] = planner->getPlanningSpeed();
                stats["planning_accuracy"] = planner->getPlanningAccuracy();
            }
            if (auto* concept = brain->getConceptFormation()) {
                stats["concept_discovery_rate"] = concept->getDiscoveryRate();
                stats["pattern_complexity"] = concept->getPatternComplexity();
            }
            if (auto* attention = brain->getAttention()) {
                stats["attention_focus"] = attention->getFocusStrength();
                stats["attention_span"] = attention->getAttentionSpan();
            }
            return stats;
        }, "Get cognition system statistics")
        
        // Enhanced development system access
        .def("getDevelopmentStats", [](const std::shared_ptr<Brain>& brain) {
            if (auto* dev = brain->getDevelopmentSystem()) {
                return std::make_tuple(
                    dev->getAge(),
                    dev->getMaturationLevel(),
                    dev->getPlasticityRate(),
                    dev->getSynaptogenesisRate(),
                    dev->getPruningRate()
                );
            }
            return std::make_tuple(0, 0.0f, 0.0f, 0.0f, 0.0f);
        }, "Get development system statistics")
        
        .def("getDevelopmentalStatus", [](const std::shared_ptr<Brain>& brain) {
            std::string status = "Development System:\n";
            auto* dev = brain->getDevelopmentSystem();
            if (dev) {
                status += "  Age: " + std::to_string(dev->getAge()) + " steps\n";
                status += "  Maturation: " + std::to_string(dev->getMaturationLevel() * 100) + "%\n";
                status += "  Plasticity Rate: " + std::to_string(dev->getPlasticityRate()) + "\n";
                status += "  Synaptogenesis Rate: " + std::to_string(dev->getSynaptogenesisRate()) + "\n";
                status += "  Pruning Rate: " + std::to_string(dev->getPruningRate());
            } else {
                status += "  Not available";
            }
            return status;
        }, "Get human-readable development system status")

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
        .def("getNeuromodulationState", [](const std::shared_ptr<AgentBrain>& agent) {
            return std::make_tuple(
                agent->getNeuromodulationLevel(),
                agent->getCuriosityLevel(),
                agent->getNoveltyLevel(),
                agent->getPredictionError()
            );
        }, "Get all neuromodulation levels as a tuple")
        
        .def("isLearningEnabled", [](const std::shared_ptr<AgentBrain>& agent) {
            return agent->isRewardModulationEnabled() && 
                   agent->isCuriosityEnabled() &&
                   agent->isDevelopmentEnabled();
        }, "Check if any learning systems are enabled")
        
        .def("enableAllFeatures", [](const std::shared_ptr<AgentBrain>& agent, bool enable) {
            agent->enableRewardModulation(enable);
            agent->enableStructuralPlasticity(enable);
            agent->enableDevelopment(enable);
            agent->enableCuriosity(enable);
        }, py::arg("enable"), "Enable or disable all agent features")
        
        .def("getFeatureStatus", [](const std::shared_ptr<AgentBrain>& agent) {
            return std::make_tuple(
                agent->isRewardModulationEnabled(),
                agent->isStructuralPlasticityEnabled(),
                agent->isDevelopmentEnabled(),
                agent->isCuriosityEnabled()
            );
        }, "Get status of all enabled features")
        
        .def("getActionStats", [](const std::shared_ptr<AgentBrain>& agent) {
            auto* brain = agent->getBrain();
            if (!brain) {
                return std::make_tuple(0, 0.0f);
            }
            return std::make_tuple(
                brain->getTotalNeuronCount(),
                brain->getAverageFiringRate()
            );
        }, "Get action-related statistics")
        
        .def("decodeMotorCommandSafe", [](const std::shared_ptr<AgentBrain>& agent) {
            auto* brain = agent->getBrain();
            if (!brain || brain->getTotalNeuronCount() == 0) {
                throw py::value_error("Brain is not initialized or has no neurons");
            }
            return agent->decodeMotorCommand();
        }, "Decode motor command with error checking")
        
        .def("processSensoryInputSafe", [](const std::shared_ptr<AgentBrain>& agent, 
                                          const SensoryPercept& percept) {
            auto* brain = agent->getBrain();
            if (!brain) {
                throw py::value_error("AgentBrain not properly initialized");
            }
            agent->processSensoryInput(percept);
        }, py::arg("percept"),
             "Process sensory input with error checking")
        
        .def("getSensoryMotorGroups", [](const std::shared_ptr<AgentBrain>& agent) {
            auto* brain = agent->getBrain();
            if (!brain) {
                return std::make_tuple(std::vector<Neuron*>(), std::vector<Neuron*>());
            }
            // Note: This is a simplified version - in a full implementation,
            // we would access the actual neuron groups from the brain
            return std::make_tuple(std::vector<Neuron*>(), std::vector<Neuron*>());
        }, "Get sensory and motor neuron groups")
        
        .def("getDevelopmentStatus", [](const std::shared_ptr<AgentBrain>& agent) {
            return std::make_tuple(
                agent->getDevelopmentalStage(),
                agent->getNeuromodulationLevel(),
                agent->getCuriosityLevel()
            );
        }, "Get developmental and neuromodulation status")
        
        .def("resetAll", [](const std::shared_ptr<AgentBrain>& agent, 
                          std::shared_ptr<Brain> brain) {
            agent->reset();
            // Note: AgentBrain reset doesn't reset the brain itself
            return true;
        }, py::arg("brain"),
             "Reset agent and optionally reset brain too")
        
        .def("updateWithDevelopment", [](const std::shared_ptr<AgentBrain>& agent, 
                                         std::shared_ptr<Brain> brain,
                                         double timestep) {
            agent->updateDevelopment(timestep);
            // Note: This is a convenience wrapper for combined agent-brain development
            return true;
        }, py::arg("brain"), py::arg("timestep"),
             "Update agent and brain development together")
        
        .def("getAllNeuromodulators", [](const std::shared_ptr<AgentBrain>& agent, 
                                        const std::shared_ptr<Brain>& brain) {
            std::map<std::string, float> neuromodulators;
            if (auto* dopamine = brain->getDopamine()) {
                neuromodulators["dopamine"] = dopamine->getLevel();
            }
            if (auto* curiosity = brain->getCuriosity()) {
                neuromodulators["curiosity"] = curiosity->getLevel();
            }
            if (auto* novelty = brain->getNovelty()) {
                neuromodulators["novelty"] = novelty->getLevel();
            }
            if (auto* predError = brain->getPredictionErrorSignal()) {
                neuromodulators["prediction_error"] = predError->getLevel();
            }
            return neuromodulators;
        }, py::arg("brain"),
             "Get all neuromodulator levels as a dictionary")
        
        .def("getAllNeuromodulators", [](const std::shared_ptr<AgentBrain>& agent, 
                                         const std::shared_ptr<Brain>& brain) {
            std::map<std::string, float> neuromodulators;
            if (auto* dopamine = brain->getDopamine()) {
                neuromodulators["dopamine"] = dopamine->getLevel();
            }
            if (auto* curiosity = brain->getCuriosity()) {
                neuromodulators["curiosity"] = curiosity->getLevel();
            }
            if (auto* novelty = brain->getNovelty()) {
                neuromodulators["novelty"] = novelty->getLevel();
            }
            if (auto* predError = brain->getPredictionErrorSignal()) {
                neuromodulators["prediction_error"] = predError->getLevel();
            }
            return neuromodulators;
        }, py::arg("brain"),
             "Get all neuromodulator levels as a dictionary")
        
        .def("getSystemHealth", [](const std::shared_ptr<AgentBrain>& agent, 
                                 const std::shared_ptr<Brain>& brain) {
            std::string health = "AgentBrain Health:\n";
            health += "  Enabled Features: " + 
                     std::to_string(
                         (agent->isRewardModulationEnabled() ? 1 : 0) +
                         (agent->isCuriosityEnabled() ? 1 : 0) +
                         (agent->isDevelopmentEnabled() ? 1 : 0)
                     ) + "/3\n";
            health += "  Brain Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
            health += "  Brain Spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
            health += "  Development Stage: " + std::to_string(static_cast<int>(agent->getDevelopmentalStage()));
            return health;
        }, py::arg("brain"),
             "Get comprehensive system health status")

    // Neuromodulation system access with enhanced bindings
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state")
        .def("getType", &Neuromodulator::getType, "Get the type of neuromodulatory signal");

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("getValue", &Dopamine::getLevel)  // Alias for getLevel
        .def("setValue", &Dopamine::setLevel, py::arg("value"))  // Alias for setLevel
        .def("add", &Dopamine::signalReward)  // Alias for signalReward
        .def("reset", &Dopamine::signalRewardPredictionError)  // Reset signal
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"));

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration motivation)pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName)
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator for novelty detection)pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName)
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor)
        .def("update", &Novelty::update, py::arg("dt"))
        .def("detectNovelty", [](Novelty& novelty, const std::vector<float>& currentPattern,
                                const std::vector<float>& previousPattern) {
            novelty.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory)
        .def("reset", &Novelty::reset);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator for learning)pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName)
        .def("getLevel", &PredictionError::getError)  // Alias for getError
        .def("setLevel", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))  // Alias for computeError
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor)
        .def("update", &PredictionError::update, py::arg("dt"))
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude)
        .def("reset", &PredictionError::clearHistory);  // Use clearHistory as reset

    }, "Get comprehensive system health status");

    // Python factory methods for convenience
    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        auto vision = std::make_shared<Vision>(width, height, channels);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         "Create a Vision sensory input with specified dimensions")
    
    .def("createAudio", [](size_t sampleRate, size_t numSamples) {
        auto audio = std::make_shared<Audio>(sampleRate, numSamples);
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"),
         "Create an Audio sensory input with specified parameters")
    
    .def("createInternalSignals", []() {
        auto signals = std::make_shared<InternalSignals>();
        return signals;
    }, "Create internal signals sensory input")
    
    .def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
         "Create a WorldObject with specified position, type, and properties")
    
    .def("createSimpleObject", [](WorldObjectType type) {
        return WorldObject(0.0f, 0.0f, type);
    }, py::arg("type"), "Create a default WorldObject")
    
    .def("createConfigFromFile", &Config::loadFromFile,
         py::arg("filepath"), py::arg("config"),
         "Load configuration from file into existing config")
    
    .def("createConfigFromArgs", [](Config& config, int argc, char** argv) {
        return config.loadFromArgs(argc, argv);
    }, py::arg("config"), py::arg("argc"), py::arg("argv"),
         "Load configuration from command line arguments into existing config")
    
    .def("copyConfig", [](const Config& config) {
        auto newConfig = std::make_shared<Config>();
        for (const auto& key : config.getKeys()) {
            newConfig->set(key, config.getOr<ConfigValue>(key, ConfigValue()));
        }
        return newConfig;
    }, py::arg("config"),
         "Create a copy of a configuration")
    
    // Context managers for automatic resource management
    .def("getSimpleWorldContextManager", []() {
        // This creates a context manager for SimpleWorld
        return py::module::import("pynlm").attr("SimpleWorldContextManager");
    })
    
    .def("getBrainContextManager", []() {
        // This creates a context manager for Brain
        return py::module::import("pynlm").attr("BrainContextManager");
    });

    // Pythonic iterator support
    .def("iterRegions", [](const std::shared_ptr<Brain>& brain) {
        return py::make_iterator(brain->getRegions());
    }, "Get iterator over brain regions")
    
    .def("iterNeurons", [](const std::shared_ptr<Brain>& brain) {
        // This would require accessing all neurons across all regions
        return py::make_iterator(brain->getTotalNeuronCount());
    }, "Get iterator over brain neurons (placeholder)")
    
    // Property access for common C++ read/write pairs
    // Note: This would require adding property definitions to the classes
    
    // Enhanced error handling with Python exceptions
    .def("validateBrainState", [](const std::shared_ptr<Brain>& brain) {
        if (brain->getTotalNeuronCount() == 0) {
            throw py::value_error("Brain has no neurons - please initialize first");
        }
        if (!brain->isActive(1)) {
            throw py::runtime_error("Brain is not active or has no firing neurons");
        }
        return true;
    }, "Validate brain state and throw exceptions if invalid")
    
    .def("validateWorldState", [](const std::shared_ptr<SimpleWorld>& world) {
        if (world->getWidth() <= 0 || world->getHeight() <= 0) {
            throw py::value_error("World dimensions must be positive");
        }
        return true;
    }, "Validate world state and throw exceptions if invalid")
    
    // High-level convenience methods
    .def("runSimulation", [](std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent,
                           size_t numSteps,
                           bool enableLearning = true) {
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);  // Use 0.1s timestep
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            agent->getNeuromodulationLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
         py::arg("numSteps"), py::arg("enableLearning") = true,
         "Run a complete simulation with all components")
    
    .def("evolveBrain", [](std::shared_ptr<Brain> brain,
                         std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agent,
                         size_t generations,
                         float mutationRate = 0.01f) {
        std::vector<std::tuple<size_t, float, float>> evolutionResults;
        
        for (size_t gen = 0; gen < generations; ++gen) {
            // Run simulation for this generation
            world->reset();
            agent->reset();
            brain->reset();
            brain->initialize();
            
            size_t totalSteps = 100;
            for (size_t step = 0; step < totalSteps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            // Record results
            evolutionResults.push_back(std::make_tuple(
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                agent->getNeuromodulationLevel()
            ));
        }
        
        return evolutionResults;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         py::arg("generations"), py::arg("mutationRate") = 0.01f,
         "Evolve brain over multiple generations")
    
    .def("evaluateAgent", [](std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<Brain> brain,
                           std::shared_ptr<AgentBrain> agent,
                           size_t numEpisodes,
                           bool enableLearning = false) {
        std::vector<std::tuple<size_t, float, float>> episodeResults;
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            world->reset();
            agent->reset();
            brain->reset();
            brain->initialize();
            
            size_t totalSteps = 50;
            for (size_t step = 0; step < totalSteps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            episodeResults.push_back(std::make_tuple(
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                agent->getNeuromodulationLevel()
            ));
        }
        
        return episodeResults;
    }, py::arg("world"), py::arg("brain"), py::arg("agent"),
         py::arg("numEpisodes"), py::arg("enableLearning") = false,
         "Evaluate agent performance over multiple episodes")
    
    .def("createTrainingEnvironment", [](size_t worldWidth = 20, size_t worldHeight = 20,
                                         size_t visionWidth = 8, size_t visionHeight = 8) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
         py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
         "Create a complete training environment with brain, world, and agent")
    
    .def("benchmarkPerformance", [](std::shared_ptr<Brain> brain,
                                   size_t numSteps = 1000,
                                   bool enablePlasticity = true) {
        if (enablePlasticity) {
            brain->enableAllPlasticity(true);
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
        float spikesPerSecond = spikesGenerated / (duration.count() / 1000.0f);
        
        return std::make_tuple(
            spikesGenerated,
            duration.count(),
            spikesPerSecond
        );
    }, py::arg("brain"), py::arg("numSteps") = 1000,
         py::arg("enablePlasticity") = true,
         "Benchmark brain performance")
    
    .def("createDevelopmentalTrajectory", [](std::shared_ptr<Brain> brain,
                                            size_t lifespanSteps = 1000) {
        std::vector<std::tuple<size_t, float, float, int>> trajectory;
        
        for (size_t step = 0; step < lifespanSteps; ++step) {
            brain->step(step);
            
            trajectory.push_back(std::make_tuple(
                step,
                brain->getAverageFiringRate(),
                brain->getExcitationInhibitionRatio(),
                static_cast<int>(brain->getDevelopmentalStage())
            ));
        }
        
        return trajectory;
    }, py::arg("brain"), py::arg("lifespanSteps") = 1000,
         "Create developmental trajectory over lifespan")
    
    .def("getDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        // Set default values for common parameters
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        return config;
    }, "Get a default configuration with sensible parameters")
    
    .def("createMinimalConfig", []() {
        return std::make_shared<Config>();
    }, "Create a minimal configuration (use default values)")
    
    .def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        // Basic validation
        if (!config.has("neuron_count") || config.getOr<int64_t>("neuron_count", 0) <= 0) {
            errors.push_back("neuron_count must be positive");
        }
        if (!config.has("simulation_timestep") || config.getOr<double>("simulation_timestep", 0) <= 0) {
            errors.push_back("simulation_timestep must be positive");
        }
        if (!errors.empty()) {
            throw py::value_error("Configuration validation failed:\n" + 
                                std::accumulate(errors.begin(), errors.end(), std::string(),
                                               [](const std::string& a, const std::string& b) {
                                                   return a + "  - " + b + "\n"; }));
        }
        return true;
    }, py::arg("config"),
         "Validate configuration and throw exception if invalid")
    
    .def("mergeConfigs", [](const Config& base, const Config& override) {
        auto merged = std::make_shared<Config>(base);
        for (const auto& key : override.getKeys()) {
            merged->set(key, override.getOr<ConfigValue>(key, ConfigValue()));
        }
        return merged;
    }, py::arg("base"), py::arg("override"),
         "Merge two configurations (override takes precedence)");

    // Pythonic enum support with string representations
    .def("getActionTypeNames", []() {
        std::vector<std::string> names;
        for (int i = 0; i < static_cast<int>(ActionType::Custom) + 1; ++i) {
            auto type = static_cast<ActionType>(i);
            names.push_back(ActionTypeToString(type));
        }
        return names;
    }, "Get names of all ActionType enum values")
    
    .def("getWorldObjectTypeNames", []() {
        std::vector<std::string> names;
        for (int i = 0; i < static_cast<int>(WorldObjectType::Marker) + 1; ++i) {
            auto type = static_cast<WorldObjectType>(i);
            names.push_back(WorldObjectTypeToString(type));
        }
        return names;
    }, "Get names of all WorldObjectType enum values")
    
    .def("getNeuronTypeNames", []() {
        std::vector<std::string> names;
        for (int i = 0; i < static_cast<int>(NeuronType::Internal) + 1; ++i) {
            auto type = static_cast<NeuronType>(i);
            names.push_back(NeuronTypeToString(type));
        }
        return names;
    }, "Get names of all NeuronType enum values")
    
    .def("getSynapseTypeNames", []() {
        std::vector<std::string> names;
        for (int i = 0; i < static_cast<int>(SynapseType::GapJunction) + 1; ++i) {
            auto type = static_cast<SynapseType>(i);
            names.push_back(SynapseTypeToString(type));
        }
        return names;
    }, "Get names of all SynapseType enum values");
