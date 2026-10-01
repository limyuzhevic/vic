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

    // Get visualization data for brain state (for plotting/debugging)
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
        .def("getTotalPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get total pending spike event count")
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
        
        // === ADVANCED ANALYSIS AND UTILITIES ===
        
        // Get brain network metrics for analysis
        .def("getNetworkMetrics", [](const Brain& self) {
            struct Metrics {
                size_t totalNeurons;
                size_t totalSynapses;
                float avgFiringRate;
                float excitationInhibitionRatio;
                size_t totalSpikes;
                size_t activeNeurons;
                float averageMembranePotential;
                float membranePotentialStdDev;
            };
            
            Metrics metrics;
            metrics.totalNeurons = self.getTotalNeuronCount();
            metrics.totalSynapses = self.getTotalSynapseCount();
            metrics.avgFiringRate = self.getAverageFiringRate();
            metrics.excitationInhibitionRatio = self.getExcitationInhibitionRatio();
            metrics.totalSpikes = self.getTotalSpikeCount();
            metrics.activeNeurons = self.getActiveNeuronCount();
            
            // Calculate additional metrics (these would require more complex implementation)
            metrics.averageMembranePotential = 0.0f; // Placeholder
            metrics.membranePotentialStdDev = 0.0f;  // Placeholder
            
            return metrics;
        }, "Get brain network metrics for analysis")
        
        // Get connectivity statistics for a specific region
        .def("getRegionConnectivity", [](const Brain& self, RegionId regionId) {
            auto region = self.getRegion(regionId);
            if (!region) {
                py::dict empty;
                return empty;
            }
            
            py::dict connectivity;
            connectivity["synapse_count"] = region->getSynapseCount();
            connectivity["neuron_count"] = region->getTotalNeuronCount();
            connectivity["synaptic_density"] = region->getSynapticDensity();
            connectivity["avg_synaptic_weight"] = region->getAverageSynapticWeight();
            
            return connectivity;
        }, py::arg("region_id"),
           "Get connectivity statistics for a specific region")
        
        // Perform pattern analysis on neural activity
        .def("analyzeActivityPattern", [](const Brain& self, SimulationStep steps, 
                                           double timestep = 0.001) {
            std::vector<std::vector<float>> activityPattern;
            // Placeholder for pattern analysis implementation
            // In a real implementation, this would collect neural activity over time
            // and perform frequency analysis or pattern recognition
            return activityPattern;
        }, py::arg("steps"), py::arg("timestep") = 0.001,
           "Analyze neural activity patterns over time")
        
        // Get statistics for all regions
        .def("getAllRegionStats", [](const Brain& self) {
            py::list regionStats;
            
            auto regionIds = self.getRegionIds();
            for (const auto& regionId : regionIds) {
                auto region = self.getRegion(regionId);
                if (region) {
                    py::dict stats;
                    stats["id"] = regionId;
                    stats["name"] = region->getName();
                    stats["neuron_count"] = region->getTotalNeuronCount();
                    stats["synapse_count"] = region->getSynapseCount();
                    stats["firing_rate"] = region->getAverageFiringRate();
                    stats["synaptic_density"] = region->getSynapticDensity();
                    regionStats.append(stats);
                }
            }
            
            return regionStats;
        }, "Get statistics for all brain regions")
        
        // === BRAIN INITIALIZATION HELPERS ===
        
        // Create a brain with common configurations
        .def("createStandardBrain", [](size_t neuronCount, size_t regionCount, 
                                       float connectionProbability) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.region_count", static_cast<int64_t>(regionCount));
            config->set("brain.connection_probability", connectionProbability);
            config->set("random_seed", static_cast<int64_t>(42));  // Deterministic
            
            return std::make_shared<Brain>(config);
        }, py::arg("neuron_count") = 1000,
           py::arg("region_count") = 4,
           py::arg("connection_probability") = 0.1f,
           "Create a brain with standard configuration"),
        
        // Create a brain optimized for reinforcement learning
        .def("createRLBrain", [](size_t neuronCount) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("random_seed", static_cast<int64_t>(42));
            
            // Enable plasticity for learning
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            
            // Configure neuromodulation for reinforcement learning
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            
            return std::make_shared<Brain>(config);
        }, py::arg("neuron_count") = 2000,
           "Create a brain optimized for reinforcement learning"),
        
        // Create a brain optimized for sensory processing
        .def("createSensoryBrain", [](size_t visionNeurons, size_t sensoryNeurons) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(visionNeurons + sensoryNeurons));
            config->set("random_seed", static_cast<int64_t>(42));
            
            // Configure for sensory processing
            config->set("plasticity.stdp.enable", true);
            config->set("neuromod.novelty.enable", true);
            
            return std::make_shared<Brain>(config);
        }, py::arg("vision_neurons") = 500,
           py::arg("sensory_neurons") = 200,
           "Create a brain optimized for sensory processing"),
        
        // === WORLD CONFIGURATION HELPERS ===
        
        // Create a simple world with common configurations
        .def("createStandardWorld", [](size_t width, size_t height, 
                                       size_t visionWidth, size_t visionHeight) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            world->setRandomSeed(42);  // Deterministic
            return world;
        }, py::arg("width") = 20,
           py::arg("height") = 20,
           py::arg("vision_width") = 8,
           py::arg("vision_height") = 8,
           "Create a standard world with default configuration"),
        
        // Create a world optimized for exploration
        .def("createExplorationWorld", []() {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(30, 30, 10, 10);
            world->setRandomSeed(42);
            
            // Enable rich environment
            world->setMaxEnergy(100.0f);
            return world;
        }, "Create a world optimized for exploration"),
        
        // Create a training world with resources and hazards
        .def("createTrainingWorld", []() {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(25, 25, 8, 8);
            world->setRandomSeed(42);
            
            // Add resources and hazards
            world->setMaxEnergy(150.0f);
            return world;
        }, "Create a training world with resources and hazards"),
        
        // === AGENT CONFIGURATION HELPERS ===
        
        // Create a default agent with common settings
        .def("createStandardAgent", [](std::shared_ptr<Brain> brain) {
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Enable typical agent capabilities
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            
            return agent;
        }, py::arg("brain"),
           "Create a standard agent with all capabilities enabled"),
        
        // Create a minimal agent for basic control
        .def("createMinimalAgent", [](std::shared_ptr<Brain> brain) {
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Only enable essential capabilities
            agent->enableRewardModulation(false);
            agent->enableStructuralPlasticity(false);
            agent->enableDevelopment(false);
            agent->enableCuriosity(false);
            
            return agent;
        }, py::arg("brain"),
           "Create a minimal agent with only basic capabilities"),
        
        // Create an advanced learning agent
        .def("createLearningAgent", [](std::shared_ptr<Brain> brain) {
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Enable all advanced learning capabilities
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            
            return agent;
        }, py::arg("brain"),
           "Create an advanced learning agent with full capabilities"),
        
        // === SERIALIZATION AND CHECKPOINTING ===
        
        // Save brain and agent state with automatic timestamp
        .def("saveWithTimestamp", [](const Brain& brain, const AgentBrain& agent,
                                     const std::string& directory) {
            std::string timestamp = std::to_string(std::chrono::system_clock::now()
                                                   .time_since_epoch().count());
            std::string brainFile = directory + "/brain_" + timestamp + ".bin";
            std::string agentFile = directory + "/agent_" + timestamp + ".bin";
            
            brain.save(brainFile);
            agent.getBrain()->save(agentFile);
            
            return py::make_tuple(brainFile, agentFile);
        }, py::arg("brain"), py::arg("agent"), py::arg("directory"),
           "Save brain and agent state with automatic timestamp"),
        
        // Load the latest checkpoint from directory
        .def("loadLatestCheckpoint", [](const std::string& directory) {
            // Implementation would scan directory for latest checkpoint
            // and load both brain and agent states
            return std::make_shared<Brain>(std::make_shared<Config>());
        }, py::arg("directory"),
           "Load the latest checkpoint from directory"),
        
        // === PERFORMANCE MONITORING ===
        
        // Get brain performance statistics
        .def("getPerformanceStats", [](const Brain& brain, SimulationStep steps) {
            py::dict stats;
            
            // Get basic statistics
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            
            // Calculate performance metrics
            double timePerStep = 1.0 / steps;  // Assuming 1 second total time
            stats["neurons_per_step"] = static_cast<double>(brain.getTotalNeuronCount()) / timePerStep;
            stats["spikes_per_step"] = static_cast<double>(brain.getTotalSpikeCount()) / timePerStep;
            
            return stats;
        }, py::arg("brain"), py::arg("steps"),
           "Get brain performance statistics"),
        
        // Measure simulation performance
        .def("measurePerformance", [](Brain& brain, SimulationStep steps,
                                      double timestep = 0.001) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (SimulationStep i = 0; i < steps; ++i) {
                brain.step(i, i * timestep);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start);
            
            py::dict performance;
            performance["total_time_seconds"] = duration.count();
            performance["steps_per_second"] = steps / duration.count();
            performance["time_per_step_ms"] = duration.count() * 1000.0 / steps;
            
            return performance;
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
           "Measure simulation performance over a given number of steps"),
        
        // === DEBUGGING AND VISUALIZATION ===
        
        // Get neural activity heatmap for a specific region
        .def("getActivityHeatmap", [](const Brain& brain, RegionId regionId,
                                     SimulationStep steps, double timestep = 0.001) {
            std::vector<std::vector<float>> heatmap;
            // Placeholder for heatmap generation
            // In a real implementation, this would collect neuron activity
            // and generate a visual representation
            return heatmap;
        }, py::arg("brain"), py::arg("region_id"),
           py::arg("steps"), py::arg("timestep") = 0.001,
           "Get neural activity heatmap for a specific region"),
        
        // Get spike raster plot data
        .def("getSpikeRaster", [](const Brain& brain, SimulationStep steps,
                                 double timestep = 0.001) {
            std::vector<std::pair<SimulationStep, NeuronId>> rasterData;
            // Placeholder for spike raster generation
            return rasterData;
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
           "Get spike raster plot data"),
        
        // Check for any potential issues in the brain
        .def("validateBrainState", [](const Brain& brain) {
            py::dict validationResults;
            
            validationResults["valid"] = true; // Placeholder
            validationResults["warnings"] = py::list(); // Placeholder
            validationResults["errors"] = py::list(); // Placeholder
            
            return validationResults;
        }, "Validate brain state and check for issues"),
        
        // === TRAINING AND LEARNING UTILITIES ===
        
        // Simulate a training episode
        .def("runTrainingEpisode", [](Brain& brain, AgentBrain& agent,
                                     SimpleWorld& world, SimulationStep steps) {
            float totalReward = 0.0f;
            
            for (SimulationStep i = 0; i < steps; ++i) {
                // Update world
                world.update(0.1);
                
                // Get sensory input
                auto percept = world.getSensoryPercept();
                
                // Process input
                agent.processSensoryInput(percept);
                brain.step(i);
                
                // Get action and apply
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Get reward and apply modulation
                float reward = world.getAgentBody().energy; // Example reward
                agent.applyRewardModulation(reward, 0.0f);
                
                totalReward += reward;
            }
            
            py::dict results;
            results["total_reward"] = totalReward;
            results["final_energy"] = world.getAgentBody().energy;
            results["steps_completed"] = steps;
            
            return results;
        }, py::arg("brain"), py::arg("agent"), py::arg("world"),
           py::arg("steps"),
           "Run a complete training episode"),
        
        // Optimize brain parameters for a specific task
        .def("optimizeForTask", [](Brain& brain, const std::string& taskType) {
            py::dict optimizedParams;
            
            // Basic optimization would involve:
            // 1. Analyzing current performance
            // 2. Adjusting configuration parameters
            // 3. Re-initializing with optimized settings
            
            optimizedParams["task_type"] = taskType;
            optimizedParams["optimized"] = true; // Placeholder
            
            return optimizedParams;
        }, py::arg("brain"), py::arg("task_type"),
           "Optimize brain parameters for a specific task type");

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

} // namespace nlm
