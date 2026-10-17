#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <sstream>

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Advanced percept analysis
        .def("getDominantSignal", &SensoryPercept::getDominantSignal)
        .def("hasVision", &SensoryPercept::hasVision)
        .def("hasTouch", &SensoryPercept::hasTouch)
        .def("hasInternal", &SensoryPercept::hasInternal)
        .def("hasAudio", &SensoryPercept::hasAudio);

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
        // ========== ADVANCED BRAIN FUNCTIONALITY ==========
        
        // Access to integrated memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system for active information storage")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system for experience storage")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system for pattern associations")
        
        // Access to prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system for forward modeling and error computation")
        
        // Access to cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system for abstract pattern discovery")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system for competitive focus")
        
        // Access to development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system for age-based plasticity modulation")
        
        // Advanced control and analysis functions
        .def("getAllNeurons", [](Brain& self) {
            std::vector<NeuronId> neuronIds;
            for (const auto& region : self.getRegions()) {
                auto neurons = region->getAllNeurons();
                for (const auto& neuron : neurons) {
                    neuronIds.push_back(neuron->getId());
                }
            }
            return neuronIds;
        }, "Get all neuron IDs in the brain")
        
        .def("getAllSynapses", [](Brain& self) {
            std::vector<SynapseId> synapseIds;
            for (const auto& region : self.getRegions()) {
                auto synapses = region->getSynapses();
                for (const auto& synapse : synapses) {
                    synapseIds.push_back(synapse->getId());
                }
            }
            return synapseIds;
        }, "Get all synapse IDs in the brain")
        
        // Advanced simulation control
        .def("runEpisode", [](Brain& self, AgentBrain& agent, SimpleWorld& world, int numSteps) {
            // Run a complete episode simulation
            for (int step = 0; step < numSteps; ++step) {
                world.update(0.1);  // Default timestep
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                self.step(step);
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("agent"), py::arg("world"), py::arg("numSteps"),
             "Run a complete simulation episode")
        
        // Advanced analysis and debugging
        .def("analyzeNetwork", [](Brain& self) {
            std::map<std::string, float> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["spike_count"] = self.getFiringNeuronCount();
            return stats;
        }, "Get comprehensive network statistics")
        
        .def("getMemoryStats", [](Brain& self) {
            std::map<std::string, std::any> stats;
            if (auto* wm = self.getWorkingMemory()) {
                stats["working_memory_capacity"] = wm->getCapacity();
                stats["working_memory_size"] = wm->getCurrentSize();
            }
            if (auto* em = self.getEpisodicMemory()) {
                stats["episodic_memory_episodes"] = em->getEpisodeCount();
            }
            return stats;
        }, "Get memory system statistics")
        
        .def("getPredictionStats", [](Brain& self) {
            std::map<std::string, float> stats;
            if (auto* ps = self.getPredictionSystem()) {
                stats["prediction_error"] = ps->getPredictionError();
                stats["prediction_confidence"] = ps->getConfidence();
                stats["error_history_size"] = ps->getErrorHistory().size();
            }
            return stats;
        }, "Get prediction system statistics")
        
        .def("getCognitionStats", [](Brain& self) {
            std::map<std::string, std::any> stats;
            if (auto* cf = self.getConceptFormation()) {
                stats["concept_count"] = cf->getConceptCount();
                // Get stability of concepts
                std::vector<float> stabilities;
                for (size_t i = 0; i < cf->getConceptCount(); ++i) {
                    if (auto* concept = cf->getConcept(i)) {
                        stabilities.push_back(cf->getConceptStability(i));
                    }
                }
                if (!stabilities.empty()) {
                    float avgStability = std::accumulate(stabilities.begin(), stabilities.end(), 0.0f) / stabilities.size();
                    stats["avg_concept_stability"] = avgStability;
                }
            }
            if (auto* planner = self.getPlanner()) {
                stats["planning_depth"] = planner->getPlanningDepth();
                stats["planning_confidence"] = planner->getPlanningConfidence();
            }
            return stats;
        }, "Get cognition system statistics")
        
        .def("logDetailedStatus", [](Brain& self) {
            std::stringstream ss;
            ss << "=== DETAILED NLM BRAIN STATUS ===\n";
            
            ss << "Neural Statistics:\n";
            ss << "  Total Neurons: " << self.getTotalNeuronCount() << "\n";
            ss << "  Total Synapses: " << self.getTotalSynapseCount() << "\n";
            ss << "  Active Neurons: " << self.getActiveNeuronCount() << "\n";
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "  Average Firing Rate: " << self.getAverageFiringRate() << " Hz\n";
            ss << "  E/I Ratio: " << self.getExcitationInhibitionRatio() << "\n";
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << "\n";
            
            ss << "\nDevelopmental State:\n";
            ss << "  Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "  Region Count: " << self.getRegionCount() << "\n";
            
            ss << "\nMemory System Status:\n";
            if (auto* wm = self.getWorkingMemory()) {
                ss << "  Working Memory: " << wm->getCurrentSize() << "/" << wm->getCapacity() << " active\n";
            }
            if (auto* em = self.getEpisodicMemory()) {
                ss << "  Episodic Memory: " << em->getEpisodeCount() << " episodes\n";
            }
            
            ss << "\nPrediction System:\n";
            if (auto* ps = self.getPredictionSystem()) {
                ss << "  Prediction Error: " << ps->getPredictionError() << "\n";
                ss << "  Confidence: " << ps->getConfidence() << "\n";
            }
            
            ss << "\nCognition Systems:\n";
            if (auto* cf = self.getConceptFormation()) {
                ss << "  Concepts: " << cf->getConceptCount() << " discovered\n";
            }
            
            ss << "\n=== END DETAILED STATUS ===\n";
            return ss.str();
        }, "Get detailed brain status information")
        
        // Advanced debugging and analysis tools
        .def("debugAnalyzeNetwork", [](Brain& self) {
            std::map<std::string, std::any> analysis;
            
            // Neural network analysis
            analysis["neuron_count"] = self.getTotalNeuronCount();
            analysis["synapse_count"] = self.getTotalSynapseCount();
            analysis["firing_rate"] = self.getAverageFiringRate();
            analysis["e_i_ratio"] = self.getExcitationInhibitionRatio();
            analysis["total_spikes"] = self.getTotalSpikeCount();
            
            // Memory system analysis
            if (auto* wm = self.getWorkingMemory()) {
                analysis["working_memory_utilization"] = 
                    static_cast<double>(wm->getCurrentSize()) / wm->getCapacity();
            }
            
            if (auto* em = self.getEpisodicMemory()) {
                analysis["episodic_memory_episodes"] = em->getEpisodeCount();
            }
            
            // Prediction system analysis
            if (auto* ps = self.getPredictionSystem()) {
                analysis["prediction_error"] = ps->getPredictionError();
                analysis["prediction_confidence"] = ps->getConfidence();
                analysis["error_history_size"] = ps->getErrorHistory().size();
                if (!ps->getErrorHistory().empty()) {
                    double sum = std::accumulate(ps->getErrorHistory().begin(), 
                                                ps->getErrorHistory().end(), 0.0);
                    analysis["average_prediction_error"] = sum / ps->getErrorHistory().size();
                }
            }
            
            // Cognition system analysis
            if (auto* cf = self.getConceptFormation()) {
                analysis["concept_count"] = cf->getConceptCount();
                if (cf->getConceptCount() > 0) {
                    double total_stability = 0.0;
                    for (size_t i = 0; i < cf->getConceptCount(); ++i) {
                        total_stability += cf->getConceptStability(i);
                    }
                    analysis["average_concept_stability"] = total_stability / cf->getConceptCount();
                }
            }
            
            return analysis;
        }, "Perform detailed network analysis and return comprehensive statistics")
        
        .def("simulateEpisode", [](Brain& self, AgentBrain& agent, SimpleWorld& world, 
                                 int numSteps, bool verbose = false) {
            std::map<std::string, std::any> episodeStats;
            
            // Store initial state
            episodeStats["initial_neurons"] = self.getTotalNeuronCount();
            episodeStats["initial_synapses"] = self.getTotalSynapseCount();
            episodeStats["initial_spikes"] = self.getTotalSpikeCount();
            
            // Run simulation
            std::vector<float> firingRates;
            for (int step = 0; step < numSteps; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                self.step(step);
                firingRates.push_back(self.getAverageFiringRate());
                
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                if (verbose && step % 100 == 0) {
                    // Log progress every 100 steps
                    NLM_LOG_INFO("Episode step " + std::to_string(step) +
                                ": " + std::to_string(self.getAverageFiringRate()) + " Hz firing rate");
                }
            }
            
            // Calculate episode statistics
            episodeStats["final_neurons"] = self.getTotalNeuronCount();
            episodeStats["final_synapses"] = self.getTotalSynapseCount();
            episodeStats["final_spikes"] = self.getTotalSpikeCount();
            episodeStats["total_spike_increase"] = 
                self.getTotalSpikeCount() - episodeStats["initial_spikes"].get<size_t>();
            
            // Calculate firing rate statistics
            if (!firingRates.empty()) {
                double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
                episodeStats["average_firing_rate"] = sum / firingRates.size();
                double sumSq = std::inner_product(firingRates.begin(), firingRates.end(), 
                                                 firingRates.begin(), 0.0);
                episodeStats["firing_rate_variance"] = 
                    (sumSq / firingRates.size()) - std::pow(episodeStats["average_firing_rate"].get<double>(), 2);
            }
            
            return episodeStats;
        }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("verbose") = false,
             "Run a complete episode simulation and return detailed statistics")
        
        .def("extractNetworkFeatures", [](Brain& self) {
            std::map<std::string, std::vector<float>> features;
            
            // Extract neural features
            std::vector<float> neuronFeatures;
            for (const auto& region : self.getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        neuronFeatures.push_back(neuron->getMembranePotential());
                        neuronFeatures.push_back(neuron->getThreshold());
                        neuronFeatures.push_back(neuron->getFiringState() == FiringState::Active ? 1.0f : 0.0f);
                    }
                }
            }
            features["neuron_states"] = neuronFeatures;
            
            // Extract synaptic features
            std::vector<float> synapseFeatures;
            for (const auto& region : self.getRegions()) {
                for (const auto* synapse : region->getSynapses()) {
                    synapseFeatures.push_back(synapse->getWeight());
                    synapseFeatures.push_back(synapse->getDelay());
                    synapseFeatures.push_back(synapse->getEligibilityTrace());
                }
            }
            features["synapse_properties"] = synapseFeatures;
            
            return features;
        }, "Extract detailed network features for machine learning or analysis")
        
        .def("getSystemHealth", [](Brain& self) {
            std::map<std::string, std::string> health;
            
            // Check system health indicators
            health["neural_stability"] = self.getTotalSpikeCount() > 1000 ? "healthy" : "low_activity";
            health["memory_function"] = (self.getTotalNeuronCount() > 100) ? "operational" : "underdeveloped";
            
            if (auto* ps = self.getPredictionSystem()) {
                health["prediction_stability"] = (ps->getConfidence() > 0.1f) ? "stable" : "unstable";
            }
            
            if (auto* cf = self.getConceptFormation()) {
                health["cognitive_development"] = (cf->getConceptCount() > 0) ? "developing" : "initial";
            }
            
            float firingRate = self.getAverageFiringRate();
            if (firingRate < 1.0f) {
                health["overall_status"] = "low_activity";
            } else if (firingRate < 10.0f) {
                health["overall_status"] = "normal";
            } else if (firingRate < 50.0f) {
                health["overall_status"] = "high_activity";
            } else {
                health["overall_status"] = "overload";
            }
            
            return health;
        }, "Get overall system health status and stability indicators")

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
}

} // namespace nlm
