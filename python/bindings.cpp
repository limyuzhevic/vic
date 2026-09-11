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

// Custom exception for configuration validation errors
class ConfigValidationException : public std::runtime_error {
public:
    ConfigValidationException(const std::string& message) 
        : std::runtime_error(message) {}
    
    ConfigValidationException(const std::string& message, const std::string& key, const std::string& expected)
        : std::runtime_error(message), configKey(key), expectedValue(expected) {}
    
    const std::string& getKey() const { return configKey; }
    const std::string& getExpected() const { return expectedValue; }
    
private:
    std::string configKey;
    std::string expectedValue;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<ConfigValidationException>(m, "ConfigValidationError");

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

    // Fix: Remove duplicate Config class definition - keeping only one with all improvements
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
        
        // Convenience methods for common configuration keys
        .def("getNeuronCount", &Config::getOr<int>, Config::BRAIN_NEURON_COUNT, 1000,
             "Get number of neurons (default: 1000)")
        .def("setNeuronCount", &Config::set<int>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set number of neurons")
        .def("getSynapseDensity", &Config::getOr<double>, Config::BRAIN_SYNAPSE_DENSITY, 0.1,
             "Get synapse density (default: 0.1)")
        .def("setSynapseDensity", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set synapse density")
        .def("getConnectionProbability", &Config::getOr<double>, Config::BRAIN_CONNECTION_PROBABILITY, 0.05,
             "Get connection probability (default: 0.05)")
        .def("setConnectionProbability", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set connection probability")
        .def("enableSTDP", &Config::set<bool>, Config::PLASTICITY_STDP_ENABLE, true, ConfigSource::Runtime,
             "Enable STDP learning (default: true)")
        .def("isSTDPEnabled", &Config::getOr<bool>, Config::PLASTICITY_STDP_ENABLE, true,
             "Check if STDP is enabled (default: true)")
        .def("getSTDPLearningRate", &Config::getOr<double>, Config::PLASTICITY_STDP_LEARNING_RATE, 0.001,
             "Get STDP learning rate (default: 0.001)")
        .def("setSTDPLearningRate", &Config::set<double>, Config::PLASTICITY_STDP_LEARNING_RATE, py::arg("value"), ConfigSource::Runtime,
             "Set STDP learning rate")
        .def("getDopamineScale", &Config::getOr<double>, Config::NEUROMOD_DOPAMINE_SCALE, 1.0,
             "Get dopamine scale (default: 1.0)")
        .def("setDopamineScale", &Config::set<double>, Config::NEUROMOD_DOPAMINE_SCALE, py::arg("value"), ConfigSource::Runtime,
             "Set dopamine scale")

        // Phase 6 configuration methods
        .def("getDopamineBaseline", &Config::getOr<double>, "dopamine_baseline", 0.1,
             "Get dopamine baseline (default: 0.1)")
        .def("setDopamineBaseline", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set dopamine baseline")
        .def("getCuriosityBaseline", &Config::getOr<double>, "curiosity_baseline", 0.5,
             "Get curiosity baseline (default: 0.5)")
        .def("setCuriosityBaseline", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set curiosity baseline")
        .def("getNoveltyThreshold", &Config::getOr<double>, "novelty_threshold", 0.3,
             "Get novelty threshold (default: 0.3)")
        .def("setNoveltyThreshold", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set novelty threshold")
        .def("getRewardDecay", &Config::getOr<double>, "reward_decay", 0.95,
             "Get reward decay (default: 0.95)")
        .def("setRewardDecay", &Config::set<double>, py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set reward decay")

        // Validation convenience methods
        .def("validate", &Config::validate,
             "Validate configuration against schema (returns True if valid)")
        .def("validateWithDetails", [](Config& self) {
            auto errors = self.validateWithDetails();
            return errors; // Return list of validation errors
        }, "Validate configuration and return detailed error list")
        .def("addValidationDescriptor", &Config::addValidationDescriptor,
             "Add a validation descriptor for configuration keys")
        .def("setSchema", &Config::setSchema,
             "Set configuration schema for validation")
        .def("getSchema", &Config::getSchema,
             "Get current configuration schema")
        .def("clearSchema", &Config::clearSchema,
             "Clear configuration schema")

        // Pythonic API: Context manager for configuration
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, const py::object&, const py::object&, const py::object&) {})  // Ignore exceptions

        // Pythonic API: Dictionary-like interface for configuration
        .def("items", [](Config& self) {
            std::vector<std::pair<std::string, ConfigValue>> items;
            for (const auto& key : self.getKeys()) {
                items.emplace_back(key, self.get<ConfigValue>(key).value());
            }
            return items;
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<ConfigValue>(key);
            if (!value) {
                py::raise(py::error_already_set());
            }
            return *value;
        })
        .def("__setitem__", [](Config& self, const std::string& key, const ConfigValue& value) {
            self.set(key, value);
        })
        .def("__contains__", &Config::has)
        .def("__len__", &Config::getKeys)

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

    // Add convenience methods for world configuration
    m.def("configureWorld", [](std::shared_ptr<SimpleWorld> world, int width, int height,
                              int visionWidth, int visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("world"), py::arg("width"), py::arg("height"),
       py::arg("visionWidth"), py::arg("visionHeight"),
       "Configure world with common parameters")
    .def("setupAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Set up agent with brain and world")
    .def("runEpisode", [](std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<Brain> brain,
                         int steps) {
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return std::make_tuple(world, agent, brain);
    }, py::arg("world"), py::arg("agent"), py::arg("brain"), py::arg("steps"),
       "Run complete agent episode");

    // Add statistical analysis helpers
    m.def("computeStats", [](const Brain& brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["pending_spikes"] = brain.getPendingSpikeEventCount();
        return stats;
    }, "Compute comprehensive brain statistics");

    // Add development stage utilities
    m.def("getDevelopmentStageName", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return std::string("Initial");
            case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
            case DevelopmentalStage::Maturation: return std::string("Maturation");
            case DevelopmentalStage::Adult: return std::string("Adult");
            case DevelopmentalStage::Aging: return std::string("Aging");
            default: return std::string("Unknown");
        }
    }, "Get human-readable name for developmental stage");

    // Add simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain;
    }, py::arg("brain"), py::arg("steps"), "Run simple simulation");

    // Add memory management utilities
    m.def("saveBrainWithBackup", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        std::string backupPath = filepath + ".backup";
        brain->save(backupPath);
        bool success = brain->save(filepath);
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain with backup");

    // Add configuration comparison utilities
    m.def("compareConfigs", [](const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        for (const auto& key : config1.getKeys()) {
            auto value1 = config1.get<ConfigValue>(key);
            auto value2 = config2.get<ConfigValue>(key);
            if (value1 != value2) {
                differences.push_back(key + " differs");
            }
        }
        return differences;
    }, "Compare two configurations and return differences");

} // namespace nlm
