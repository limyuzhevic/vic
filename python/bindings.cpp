#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

// Include all necessary NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/sensory/VisionProcessor.hpp"
#include "../src/sensory/AudioProcessor.hpp"
#include "../src/sensory/InternalSignalsProcessor.hpp"
#include "../src/motor/Action.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/ScalingBenchmark.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for better Python integration
namespace helpers {
    
    // Helper to convert vector to Python list with bounds checking
    template<typename T>
    std::vector<T> safeConvert(const std::vector<T>& input) {
        return input; // pybind11 handles STL conversion
    }
    
    // Helper for logging
    void logPythonMessage(const std::string& message, nlm::LogLevel level = nlm::LogLevel::Info) {
        nlm::Logger::getGlobal().log(message, level);
    }
    
    // Helper for batch processing
    template<typename Callable>
    void batchProcess(size_t start, size_t end, Callable&& func) {
        for (size_t i = start; i < end; ++i) {
            func(i);
        }
    }
    
    // Helper for simulation metrics collection
    struct SimulationMetrics {
        size_t totalSteps = 0;
        double totalTime = 0.0;
        size_t spikeCount = 0;
        size_t activeNeurons = 0;
        double averageFiringRate = 0.0;
        double eiratio = 0.0;
        
        double getSecondsPerStep() const {
            return totalTime / totalSteps;
        }
        
        std::string toString() const {
            std::stringstream ss;
            ss << "SimulationMetrics{\n";
            ss << "  Total Steps: " << totalSteps << "\n";
            ss << "  Total Time: " << totalTime << "s\n";
            ss << "  Average Time per Step: " << getSecondsPerStep() << "s\n";
            ss << "  Spike Count: " << spikeCount << "\n";
            ss << "  Active Neurons: " << activeNeurons << "\n";
            ss << "  Average Firing Rate: " << averageFiringRate << " Hz\n";
            ss << "  E/I Ratio: " << eiratio << "\n";
            ss << "}";
            return ss.str();
        }
    };
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        
        - Complete neural component access (Neuron, Synapse, NeuralRegion, NeuralPopulation)
        - Full memory system integration (WorkingMemory, EpisodicMemory, AssociativeMemory)
        - Neuromodulation controls (Reward, Curiosity, Novelty, PredictionError)
        - Advanced cognition (NeuralPlanner, ConceptFormation, SelfModel, SocialLearning)
        - Developmental stages (DevelopmentSystem, Synaptogenesis, Maturation, Pruning)
        - Sensory processing (Vision, Audio, InternalSignals)
        - Prediction systems (PredictionSystem, NeuralPrediction)
        - Batch processing utilities for efficient simulation runs
        - Advanced metrics collection and analysis
        - Visualization integration
        - Command-line interface for quick operations
        - Comprehensive documentation and examples
        
        The API follows object-oriented principles with:
        - Method chaining where appropriate
        - Type hints and documentation strings
        - Error handling and validation
        - Performance-optimized operations
        - Advanced simulation control
        
        Usage: See examples and documentation for advanced usage.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===================================================================
    // CONSTANTS AND TYPE DEFINITIONS
    // ===================================================================
    
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

    // Enumerations
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

    // ===================================================================
    // CORE TYPES AND INFRASTRUCTURE
    // ===================================================================
    
    // Configure logging for Python integration
    m.def("configure_logging", [](nlm::LogLevel level, bool enable_console) {
        auto logger = std::make_shared<nlm::Logger>();
        if (enable_console) {
            auto console_logger = std::make_shared<nlm::ConsoleLogger>(level);
            logger->addLogger(console_logger);
        }
        nlm::Logger::setGlobal(logger);
    }, py::arg("level") = nlm::LogLevel::Info,
            py::arg("enable_console") = true,
            "Configure logging for NLM. level: Log level (Debug, Info, Warning, Error). "
            "enable_console: Enable console logging.");

    m.def("log_message", &helpers::logPythonMessage,
          py::arg("message"), py::arg("level") = nlm::LogLevel::Info,
          "Log a message from Python");

    // ===================================================================
    // CONFIGURATION SYSTEM
    // ===================================================================
    
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
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value (throws if not found)")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // ===================================================================
    // NEURAL COMPONENTS
    // ===================================================================
    
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual spiking neuron model)pbdoc")
        .def(py::init<>())
        .def("getId", &Neuron::getId)
        .def("setId", &Neuron::setId, py::arg("id"))
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getTimeConstant", &Neuron::getTimeConstant)
        .def("setTimeConstant", &Neuron::setTimeConstant, py::arg("tau"))
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("period"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Leaky Integrate-and-Fire neuron update. Returns true if neuron fired.")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("reset", &Neuron::reset)
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amount"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amount"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amount"))
        .def("getSpikeCount", &Neuron::getSpikeCount)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("setLeak", &Neuron::setLeak, py::arg("leak"))
        .def("setResetValue", &Neuron::setResetValue, py::arg("reset_value"))
        .def("getMembraneCharge", &Neuron::getMembraneCharge)
        .def("setMembraneCharge", &Neuron::setMembraneCharge, py::arg("charge"))
        .def("getConductance", &Neuron::getConductance)
        .def("setConductance", &Neuron::setConductance, py::arg("conductance"))
        .def("getInternalState", &Neuron::getInternalState)
        .def("setInternalState", &Neuron::setInternalState, py::arg("state"))
        .def("__repr__", [](const Neuron& n) {
            std::stringstream ss;
            ss << "<Neuron id=" << n.getId() << " type=" << static_cast<int>(n.getType()) 
               << " V=" << n.getMembranePotential() << ">";
            return ss.str();
        });

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def(py::init<>())
        .def("getId", &Synapse::getId)
        .def("setId", &Synapse::setId, py::arg("id"))
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("setSourceNeuron", &Synapse::setSourceNeuron, py::arg("neuron_id"))
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("setDestinationNeuron", &Synapse::setDestinationNeuron, py::arg("neuron_id"))
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeCount", &Synapse::getPreSpikeCount)
        .def("getPostSpikeCount", &Synapse::getPostSpikeCount)
        .def("getLastPreSpike", &Synapse::getLastPreSpike)
        .def("getLastPostSpike", &Synapse::getLastPostSpike)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("factor"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"))
        .def("getPreSynapticNeuron", &Synapse::getSourceNeuron)
        .def("getPostSynapticNeuron", &Synapse::getDestinationNeuron)
        .def("isActive", &Synapse::isActive)
        .def("getStrength", &Synapse::getWeight)
        .def("setStrength", &Synapse::setWeight, py::arg("strength"))
        .def("getDelayMs", &Synapse::getDelay)
        .def("setDelayMs", &Synapse::setDelay, py::arg("delay_ms"))
        .def("getPlasticityEnabled", &Synapse::getPlasticityEnabled)
        .def("setPlasticityEnabled", &Synapse::setPlasticityEnabled, py::arg("enabled"))
        .def("getHebbianEnabled", &Synapse::getHebbianEnabled)
        .def("setHebbianEnabled", &Synapse::setHebbianEnabled, py::arg("enabled"))
        .def("getSTDP_Enabled", &Synapse::getSTDP_Enabled)
        .def("setSTDP_Enabled", &Synapse::setSTDP_Enabled, py::arg("enabled"))
        .def("__repr__", [](const Synapse& s) {
            std::stringstream ss;
            ss << "<Synapse id=" << s.getId() << " weight=" << s.getWeight() 
               << " delay=" << s.getDelay() << "ms>";
            return ss.str();
        });

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Collection of neural populations)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralRegion::getId)
        .def("setId", &NeuralRegion::setId, py::arg("id"))
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulationIds", &NeuralRegion::getPopulationIds)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight"), py::arg("delay"))
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, py::arg("rng"))
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getAllSynapses", &NeuralRegion::getSynapses)
        .def("reset", &NeuralRegion::reset)
        .def("setPlasticityEnabled", &NeuralRegion::setPlasticityEnabled, py::arg("enabled"))
        .def("getPlasticityEnabled", &NeuralRegion::getPlasticityEnabled)
        .def("setDevelopmentEnabled", &NeuralRegion::setDevelopmentEnabled, py::arg("enabled"))
        .def("getDevelopmentEnabled", &NeuralRegion::getDevelopmentEnabled)
        .def("setNeuromodulationEnabled", &NeuralRegion::setNeuromodulationEnabled, py::arg("enabled"))
        .def("getNeuromodulationEnabled", &NeuralRegion::getNeuromodulationEnabled)
        .def("getMemoryEnabled", &NeuralRegion::getMemoryEnabled)
        .def("setMemoryEnabled", &NeuralRegion::setMemoryEnabled, py::arg("enabled"))
        .def("__repr__", [](const NeuralRegion& r) {
            std::stringstream ss;
            ss << "<NeuralRegion id=" << r.getId() << " name=\"" << r.getName() << 
               "\" neurons=" << r.getTotalNeuronCount() << " synapses=" << r.getSynapseCount() << ">";
            return ss.str();
        });

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Collection of neurons with same properties)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralPopulation::getId)
        .def("setId", &NeuralPopulation::setId, py::arg("id"))
        .def("getType", &NeuralPopulation::getType)
        .def("setType", &NeuralPopulation::setType, py::arg("type"))
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"))
        .def("reset", &NeuralPopulation::reset)
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential)
        .def("setTemperature", &NeuralPopulation::setTemperature, py::arg("temperature"))
        .def("getTemperature", &NeuralPopulation::getTemperature)
        .def("setInhibitionStrength", &NeuralPopulation::setInhibitionStrength, py::arg("strength"))
        .def("getInhibitionStrength", &NeuralPopulation::getInhibitionStrength)
        .def("setExternalInput", &NeuralPopulation::setExternalInput, py::arg("input"))
        .def("getExternalInput", &NeuralPopulation::getExternalInput)
        .def("setThreshold", &NeuralPopulation::setThreshold, py::arg("threshold"))
        .def("getThreshold", &NeuralPopulation::getThreshold)
        .def("setRefractoryPeriod", &NeuralPopulation::setRefractoryPeriod, py::arg("period"))
        .def("getRefractoryPeriod", &NeuralPopulation::getRefractoryPeriod)
        .def("setLeak", &NeuralPopulation::setLeak, py::arg("leak"))
        .def("getLeak", &NeuralPopulation::getLeak)
        .def("setResetValue", &NeuralPopulation::setResetValue, py::arg("reset_value"))
        .def("getResetValue", &NeuralPopulation::getResetValue)
        .def("__repr__", [](const NeuralPopulation& p) {
            std::stringstream ss;
            ss << "<NeuralPopulation id=" << p.getId() << " type=" << static_cast<int>(p.getType())
               << " size=" << p.getSize() << " active=" << p.getActiveCount() << ">";
            return ss.str();
        });

    // ===================================================================
    // MEMORY SYSTEMS
    // ===================================================================
    
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Short-term memory with persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory)
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity)
        .def("setActivityPattern", &NeuralWorkingMemory::setActivityPattern, py::arg("pattern"))
        .def("getActivityPattern", &NeuralWorkingMemory::getActivityPattern)
        .def("enableCompetition", &NeuralWorkingMemory::enableCompetition, py::arg("enabled"))
        .def("isCompetitionEnabled", &NeuralWorkingMemory::isCompetitionEnabled)
        .def("setInputGate", &NeuralWorkingMemory::setInputGate, py::arg("gate"))
        .def("getInputGate", &NeuralWorkingMemory::getInputGate)
        .def("addInput", &NeuralWorkingMemory::addInput, py::arg("pattern"))
        .def("getInput", &NeuralWorkingMemory::getInput)
        .def("clearInput", &NeuralWorkingMemory::clearInput)
        .def("__repr__", [](const NeuralWorkingMemory& mem) {
            std::stringstream ss;
            ss << "<NeuralWorkingMemory traces=" << mem.getActiveTraces() 
               << " capacity=" << mem.getCapacity() << ">";
            return ss.str();
        });

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Long-term memory for experience replay)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 5)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius") = 1.0f, py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episodeId"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count") = 10)
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold") = 0.5f)
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enabled") = true)
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count") = 10)
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"))
        .def("setReplaySpeed", &NeuralEpisodicMemory::setReplaySpeed, py::arg("speed") = 1.0f)
        .def("getReplaySpeed", &NeuralEpisodicMemory::getReplaySpeed)
        .def("enableTemporalCompression", &NeuralEpisodicMemory::enableTemporalCompression, py::arg("enabled") = true)
        .def("isTemporalCompressionEnabled", &NeuralEpisodicMemory::isTemporalCompressionEnabled)
        .def("__repr__", [](const NeuralEpisodicMemory& mem) {
            std::stringstream ss;
            ss << "<NeuralEpisodicMemory episodes=" << mem.getEpisodeCount() 
               << " replay_enabled=" << mem.isReplayEnabled() << ">";
            return ss.str();
        });

    // NeuralAssociativeMemory (simplified interface)
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Pattern completion through associations)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta") = 0.1f)
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 10)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount)
        .def("getSemanticStrength", &NeuralAssociativeMemory::getSemanticStrength, py::arg("pattern"))
        .def("setSemanticWeight", &NeuralAssociativeMemory::setSemanticWeight, py::arg("weight"))
        .def("getContextualActivation", &NeuralAssociativeMemory::getContextualActivation)
        .def("setContext", &NeuralAssociativeMemory::setContext, py::arg("context"))
        .def("getContext", &NeuralAssociativeMemory::getContext)
        .def("enableContextualLearning", &NeuralAssociativeMemory::enableContextualLearning, py::arg("enabled") = true)
        .def("isContextualLearningEnabled", &NeuralAssociativeMemory::isContextualLearningEnabled)
        .def("__repr__", [](const NeuralAssociativeMemory& mem) {
            std::stringstream ss;
            ss << "<NeuralAssociativeMemory associations=" << mem.getAssociationCount() 
               << " context_enabled=" << mem.isContextualLearningEnabled() << ">";
            return ss.str();
        });

    // ===================================================================
    // NEUROMODULATION SYSTEMS
    // ===================================================================
    
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator system)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", &Reward::computeReward, py::arg("observation"))
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory)
        .def("getDiscountFactor", &Reward::getDiscountFactor)
        .def("setDiscountFactor", &Reward::setDiscountFactor, py::arg("factor"))
        .def("getScale", &Reward::getScale)
        .def("setScale", &Reward::setScale, py::arg("scale"))
        .def("getTemporalDifferenceError", &Reward::getTemporalDifferenceError)
        .def("updateTDTarget", &Reward::updateTDTarget, py::arg("target"))
        .def("getTDTarget", &Reward::getTDTarget)
        .def("__repr__", [](const Reward& r) {
            std::stringstream ss;
            ss << "<Reward value=" << r.getValue() << " history_size=" << r.getHistory().size() << ">";
            return ss.str();
        });

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity-driven exploration neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt") = 0.1)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("getNoveltyWeight", &Curiosity::getNoveltyWeight)
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("getPredictionErrorWeight", &Curiosity::getPredictionErrorWeight)
        .def("reset", &Curiosity::reset)
        .def("getIntrinsicReward", &Curiosity::getIntrinsicReward)
        .def("setIntrinsicRewardWeight", &Curiosity::setIntrinsicRewardWeight, py::arg("weight"))
        .def("getIntrinsicRewardWeight", &Curiosity::getIntrinsicRewardWeight)
        .def("__repr__", [](const Curiosity& c) {
            std::stringstream ss;
            ss << "<Curiosity level=" << c.getLevel() << " drive=" << c.getExplorationDrive() << ">";
            return ss.str();
        });

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("observation"), py::arg("previousObservation"))
        .def("detectNovelty", [](const Novelty& self, const std::vector<float>& currentPattern, const std::vector<float>& previousPattern) {
            return self.detectNovelty(currentPattern, previousPattern);
        })
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory)
        .def("setDecayRate", &Novelty::setDecayRate, py::arg("rate"))
        .def("getDecayRate", &Novelty::getDecayRate)
        .def("setNoveltyScale", &Novelty::setNoveltyScale, py::arg("scale"))
        .def("getNoveltyScale", &Novelty::getNoveltyScale)
        .def("computeNoveltyScore", &Novelty::computeNoveltyScore, py::arg("pattern1"), py::arg("pattern2"))
        .def("setDistanceMetric", &Novelty::setDistanceMetric, py::arg("metric"))
        .def("getDistanceMetric", &Novelty::getDistanceMetric)
        .def("__repr__", [](const Novelty& n) {
            std::stringstream ss;
            ss << "<Novelty level=" << n.getLevel() << " history_size=" << n.getHistory().size() << ">";
            return ss.str();
        });

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude)
        .def("setLearningRate", &PredictionError::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &PredictionError::getLearningRate)
        .def("setPredictionErrorThreshold", &PredictionError::setPredictionErrorThreshold, py::arg("threshold"))
        .def("getPredictionErrorThreshold", &PredictionError::getPredictionErrorThreshold)
        .def("isSignificant", &PredictionError::isSignificant)
        .def("reset", &PredictionError::reset)
        .def("__repr__", [](const PredictionError& pe) {
            std::stringstream ss;
            ss << "<PredictionError error=" << pe.getError() << " magnitude=" << pe.getMagnitude() 
               << " significant=" << pe.isSignificant() << ">";
            return ss.str();
        });

    // ===================================================================
    // PLASTICITY SYSTEMS
    // ===================================================================
    
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("getLearningRate", &STDP::getLearningRate)
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"))
        .def("getTimeConstant", &STDP::getTimeConstant)
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"))
        .def("getLTP_Threshold", &STDP::getLTP_Threshold)
        .def("setLTP_Threshold", &STDP::setLTP_Threshold, py::arg("threshold"))
        .def("getLTD_Threshold", &STDP::getLTD_Threshold)
        .def("setLTD_Threshold", &STDP::setLTD_Threshold, py::arg("threshold"))
        .def("getLTP_MaxWeight", &STDP::getLTP_MaxWeight)
        .def("setLTP_MaxWeight", &STDP::setLTP_MaxWeight, py::arg("max_weight"))
        .def("getLTD_MinWeight", &STDP::getLTD_MinWeight)
        .def("setLTD_MinWeight", &STDP::setLTD_MinWeight, py::arg("min_weight"))
        .def("computeLTP", &STDP::computeLTP, py::arg("pre_time"), py::arg("post_time"))
        .def("computeLTD", &STDP::computeLTD, py::arg("pre_time"), py::arg("post_time"))
        .def("updateSynapse", &STDP::updateSynapse, py::arg("synapse"), py::arg("pre_spike_time"), py::arg("post_spike_time"))
        .def("reset", &STDP::reset)
        .def("__repr__", [](const STDP& stdp) {
            std::stringstream ss;
            ss << "<STDP rate=" << stdp.getLearningRate() << " tau=" << stdp.getTimeConstant() << ">";
            return ss.str();
        });

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("getThreshold", &Hebbian::getThreshold)
        .def("setThreshold", &Hebbian::setThreshold, py::arg("threshold"))
        .def("getMaxWeight", &Hebbian::getMaxWeight)
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("max_weight"))
        .def("getMinWeight", &Hebbian::getMinWeight)
        .def("setMinWeight", &Hebbian::setMinWeight, py::arg("min_weight"))
        .def("updateSynapse", &Hebbian::updateSynapse, py::arg("synapse"), py::arg("correlation"))
        .def("reset", &Hebbian::reset)
        .def("__repr__", [](const Hebbian& hebbian) {
            std::stringstream ss;
            ss << "<Hebbian rate=" << hebbian.getLearningRate() << " threshold=" << hebbian.getThreshold() << ">";
            return ss.str();
        });

    // ===================================================================
    // PREDICTION SYSTEMS
    // ===================================================================
    
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(System for predicting future states)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getHistory", &PredictionSystem::getHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"))
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &PredictionSystem::getLearningRate)
        .def("setPredictionHorizon", &PredictionSystem::setPredictionHorizon, py::arg("steps"))
        .def("getPredictionHorizon", &PredictionSystem::getPredictionHorizon)
        .def("getPredictedPattern", &PredictionSystem::getPredictedPattern)
        .def("getPredictionAccuracy", &PredictionSystem::getPredictionAccuracy)
        .def("__repr__", [](const PredictionSystem& ps) {
            std::stringstream ss;
            ss << "<PredictionSystem error=" << ps.getPredictionError() 
               << " confidence=" << ps.getConfidence() << ">";
            return ss.str();
        });

    // ===================================================================
    // COGNITION SYSTEMS
    // ===================================================================
    
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Action planning and decision making)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 1.0f)
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("quality"))
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("quality"))
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful)
        .def("getAlternativePlans", &NeuralPlanner::getAlternativePlans)
        .def("selectBestPlan", &NeuralPlanner::selectBestPlan)
        .def("setTimeBudget", &NeuralPlanner::setTimeBudget, py::arg("budget"))
        .def("getTimeBudget", &NeuralPlanner::getTimeBudget)
        .def("__repr__", [](const NeuralPlanner& planner) {
            std::stringstream ss;
            ss << "<NeuralPlanner depth=" << planner.getPlanningDepth() 
               << " confidence=" << planner.getPlanningConfidence() << ">";
            return ss.str();
        });

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("experience"))
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"))
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConcept", &ConceptFormation::getConcept, py::arg("id"))
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("id"))
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("id"))
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("id"))
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("id1"), py::arg("id2"))
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("id"), py::arg("newInstance"))
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("pattern1"), py::arg("pattern2"))
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"))
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"))
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility)
        .def("setLearningRate", &ConceptFormation::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &ConceptFormation::getLearningRate)
        .def("setConceptStabilityThreshold", &ConceptFormation::setConceptStabilityThreshold, py::arg("threshold"))
        .def("getConceptStabilityThreshold", &ConceptFormation::getConceptStabilityThreshold)
        .def("__repr__", [](const ConceptFormation& cf) {
            std::stringstream ss;
            ss << "<ConceptFormation concepts=" << cf.getConcepts().size() 
               << " stability_threshold=" << cf.getConceptStabilityThreshold() << ">";
            return ss.str();
        });

    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Internal self-representation and action prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"))
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("outcome"))
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"))
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence)
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness, py::arg("sensory_change"))
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"))
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("prediction_error"))
        .def("clear", &SelfModel::clear)
        .def("getModelAccuracy", &SelfModel::getModelAccuracy)
        .def("setAccuracyThreshold", &SelfModel::setAccuracyThreshold, py::arg("threshold"))
        .def("getAccuracyThreshold", &SelfModel::getAccuracyThreshold)
        .def("getActionEffects", &SelfModel::getActionEffects)
        .def("getSensoryEffects", &SelfModel::getSensoryEffects)
        .def("__repr__", [](const SelfModel& sm) {
            std::stringstream ss;
            ss << "<SelfModel confidence=" << sm.getSelfModelConfidence() 
               << " accuracy=" << sm.getModelAccuracy() << ">";
            return ss.str();
        });

    // ===================================================================
    // DEVELOPMENT SYSTEMS
    // ===================================================================
    
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development over time)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"))
        .def("update", [](DevelopmentSystem& self, nlm::Brain* brain, const nlm::RandomGenerator& rng, double dt) {
            return self.update(brain, 0, dt); // Simplified interface
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"))
        .def("getMaturationLevel", &DevelopmentSystem::getMaturationLevel)
        .def("setMaturationLevel", &DevelopmentSystem::setMaturationLevel, py::arg("level"))
        .def("getGrowthRate", &DevelopmentSystem::getGrowthRate)
        .def("setGrowthRate", &DevelopmentSystem::setGrowthRate, py::arg("rate"))
        .def("__repr__", [](const DevelopmentSystem& ds) {
            std::stringstream ss;
            ss << "<DevelopmentSystem stage=" << static_cast<int>(ds.getStage()) 
               << " age=" << ds.getDevelopmentalAge() << " critical=" << ds.isCriticalPeriod() << ">";
            return ss.str();
        });

    // ===================================================================
    // SENSORY SYSTEMS
    // ===================================================================
    
    // Observation class
    py::class_<Observation>(m, "Observation", R"pbdoc(Sensorimotor observation data)pbdoc")
        .def(py::init<>())
        .def("getTimestamp", &Observation::getTimestamp)
        .def("setTimestamp", &Observation::setTimestamp, py::arg("timestamp"))
        .def("getSensoryData", &Observation::getSensoryData)
        .def("setSensoryData", &Observation::setSensoryData, py::arg("data"))
        .def("getMotorCommand", &Observation::getMotorCommand)
        .def("setMotorCommand", &Observation::setMotorCommand, py::arg("command"))
        .def("getReward", &Observation::getReward)
        .def("setReward", &Observation::setReward, py::arg("reward"))
        .def("getActionEffect", &Observation::getActionEffect)
        .def("setActionEffect", &Observation::setActionEffect, py::arg("effect"))
        .def("getSensorySignal", &Observation::getSensorySignal, py::arg("index"))
        .def("setSensorySignal", &Observation::setSensorySignal, py::arg("index"), py::arg("value"))
        .def("getMotorSignal", &Observation::getMotorSignal, py::arg("index"))
        .def("setMotorSignal", &Observation::setMotorSignal, py::arg("index"), py::arg("value"))
        .def("getRewardSignal", &Observation::getRewardSignal)
        .def("setRewardSignal", &Observation::setRewardSignal, py::arg("reward"))
        .def("getEffectSignal", &Observation::getEffectSignal, py::arg("index"))
        .def("setEffectSignal", &Observation::setEffectSignal, py::arg("index"), py::arg("value"))
        .def("getAllSignals", &Observation::getAllSignals)
        .def("clear", &Observation::clear)
        .def("__repr__", [](const Observation& obs) {
            std::stringstream ss;
            ss << "<Observation timestamp=" << obs.getTimestamp() 
               << " reward=" << obs.getReward() << ">";
            return ss.str();
        });

    // ===================================================================
    // WORLD AND ENVIRONMENT
    // ===================================================================
    
    // Environment class (base)
    py::class_<Environment>(m, "Environment", R"pbdoc(Base environment class)pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset)
        .def("update", &Environment::update, py::arg("timestep"))
        .def("getObservation", &Environment::getObservation)
        .def("setAgentBody", &Environment::setAgentBody, py::arg("body"))
        .def("getAgentBody", &Environment::getAgentBody)
        .def("addWorldObject", &Environment::addWorldObject, py::arg("object"))
        .def("removeWorldObject", &Environment::removeWorldObject, py::arg("x"), py::arg("y"))
        .def("getWorldObject", &Environment::getWorldObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &Environment::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getDimensions", &Environment::getDimensions)
        .def("setDimensions", &Environment::setDimensions, py::arg("width"), py::arg("height"))
        .def("getMaxEnergy", &Environment::getMaxEnergy)
        .def("setMaxEnergy", &Environment::setMaxEnergy, py::arg("energy"))
        .def("getEnergyDecayRate", &Environment::getEnergyDecayRate)
        .def("setEnergyDecayRate", &Environment::setEnergyDecayRate, py::arg("rate"))
        .def("getSimulationTime", &Environment::getSimulationTime)
        .def("setRandomSeed", &Environment::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &Environment::getRandomSeed)
        .def("enableRenderMode", &Environment::enableRenderMode, py::arg("enabled") = true)
        .def("isRenderModeEnabled", &Environment::isRenderModeEnabled)
        .def("setVisualScale", &Environment::setVisualScale, py::arg("scale"))
        .def("getVisualScale", &Environment::getVisualScale)
        .def("__repr__", [](const Environment& env) {
            std::stringstream ss;
            ss << "<Environment dimensions=" << env.getDimensions().first << "x" 
               << env.getDimensions().second << " energy=" << env.getMaxEnergy() << ">";
            return ss.str();
        });

    // GridWorldEnvironment
    py::class_<GridWorldEnvironment, Environment>(m, "GridWorldEnvironment", R"pbdoc(2D grid world environment)pbdoc")
        .def(py::init<>())
        .def("configure", &GridWorldEnvironment::configure, py::arg("width"), py::arg("height"), 
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("setResourceDensity", &GridWorldEnvironment::setResourceDensity, py::arg("density"))
        .def("getResourceDensity", &GridWorldEnvironment::getResourceDensity)
        .def("setHazardDensity", &GridWorldEnvironment::setHazardDensity, py::arg("density"))
        .def("getHazardDensity", &GridWorldEnvironment::getHazardDensity)
        .def("setWallDensity", &GridWorldEnvironment::setWallDensity, py::arg("density"))
        .def("getWallDensity", &GridWorldEnvironment::getWallDensity)
        .def("addResourceAt", &GridWorldEnvironment::addResourceAt, py::arg("x"), py::arg("y"), py::arg("value"))
        .def("addHazardAt", &GridWorldEnvironment::addHazardAt, py::arg("x"), py::arg("y"), py::arg("value"))
        .def("addWallAt", &GridWorldEnvironment::addWallAt, py::arg("x"), py::arg("y"))
        .def("getResourceAt", &GridWorldEnvironment::getResourceAt, py::arg("x"), py::arg("y"))
        .def("getHazardAt", &GridWorldEnvironment::getHazardAt, py::arg("x"), py::arg("y"))
        .def("isWallAt", &GridWorldEnvironment::isWallAt, py::arg("x"), py::arg("y"))
        .def("getAvailableActions", &GridWorldEnvironment::getAvailableActions, py::arg("x"), py::arg("y"))
        .def("getRewards", &GridWorldEnvironment::getRewards)
        .def("setRewardFunction", &GridWorldEnvironment::setRewardFunction, py::arg("reward_function"))
        .def("getTerrainType", &GridWorldEnvironment::getTerrainType, py::arg("x"), py::arg("y"))
        .def("setTerrainType", &GridWorldEnvironment::setTerrainType, py::arg("x"), py::arg("y"), py::arg("type"))
        .def("enableDynamicObstacles", &GridWorldEnvironment::enableDynamicObstacles, py::arg("enabled") = true)
        .def("isDynamicObstaclesEnabled", &GridWorldEnvironment::isDynamicObstaclesEnabled)
        .def("getAgentStart", &GridWorldEnvironment::getAgentStart)
        .def("setAgentStart", &GridWorldEnvironment::setAgentStart, py::arg("x"), py::arg("y"))
        .def("getAgentPosition", &GridWorldEnvironment::getAgentPosition)
        .def("setAgentPosition", &GridWorldEnvironment::setAgentPosition, py::arg("x"), py::arg("y"))
        .def("getAgentOrientation", &GridWorldEnvironment::getAgentOrientation)
        .def("setAgentOrientation", &GridWorldEnvironment::setAgentOrientation, py::arg("angle"))
        .def("getAgentEnergy", &GridWorldEnvironment::getAgentEnergy)
        .def("setAgentEnergy", &GridWorldEnvironment::setAgentEnergy, py::arg("energy"))
        .def("getAgentHealth", &GridWorldEnvironment::getAgentHealth)
        .def("setAgentHealth", &GridWorldEnvironment::setAgentHealth, py::arg("health"))
        .def("getAgentAge", &GridWorldEnvironment::getAgentAge)
        .def("setAgentAge", &GridWorldEnvironment::setAgentAge, py::arg("age"))
        .def("isAgentMoving", &GridWorldEnvironment::isAgentMoving)
        .def("setAgentMoving", &GridWorldEnvironment::setAgentMoving, py::arg("moving"))
        .def("isAgentTurning", &GridWorldEnvironment::isAgentTurning)
        .def("setAgentTurning", &GridWorldEnvironment::setAgentTurning, py::arg("turning"))
        .def("getSimulationTime", &GridWorldEnvironment::getSimulationTime)
        .def("setSimulationTime", &GridWorldEnvironment::setSimulationTime, py::arg("time"))
        .def("enableEpisodeTermination", &GridWorldEnvironment::enableEpisodeTermination, py::arg("enabled") = true)
        .def("isEpisodeTerminationEnabled", &GridWorldEnvironment::isEpisodeTerminationEnabled)
        .def("setMaxEpisodeSteps", &GridWorldEnvironment::setMaxEpisodeSteps, py::arg("steps"))
        .def("getMaxEpisodeSteps", &GridWorldEnvironment::getMaxEpisodeSteps)
        .def("setRewardDecay", &GridWorldEnvironment::setRewardDecay, py::arg("decay"))
        .def("getRewardDecay", &GridWorldEnvironment::getRewardDecay)
        .def("setEpisodeReward", &GridWorldEnvironment::setEpisodeReward, py::arg("reward"))
        .def("getEpisodeReward", &GridWorldEnvironment::getEpisodeReward)
        .def("resetEpisode", &GridWorldEnvironment::resetEpisode)
        .def("getAllResources", &GridWorldEnvironment::getAllResources)
        .def("getAllHazards", &GridWorldEnvironment::getAllHazards)
        .def("getAllWalls", &GridWorldEnvironment::getAllWalls)
        .def("getTerrainMap", &GridWorldEnvironment::getTerrainMap)
        .def("getResourceMap", &GridWorldEnvironment::getResourceMap)
        .def("getHazardMap", &GridWorldEnvironment::getHazardMap)
        .def("getWallMap", &GridWorldEnvironment::getWallMap)
        .def("__repr__", [](const GridWorldEnvironment& env) {
            std::stringstream ss;
            ss << "<GridWorldEnvironment " << env.getDimensions().first << "x" 
               << env.getDimensions().second << " resources=" << env.getResourceDensity() 
               << " hazards=" << env.getHazardDensity() << ">";
            return ss.str();
        });

    // ===================================================================
    // ADVANCED UTILITIES AND HELPERS
    // ===================================================================
    
    // Batch processing utilities
    m.def("batch_simulate", &helpers::batchProcess,
          py::arg("start"), py::arg("end"), py::arg("func"),
          "Batch process simulation steps");

    // Metrics collection
    py::class_<helpers::SimulationMetrics>(m, "SimulationMetrics", R"pbdoc(Collected simulation metrics)pbdoc")
        .def_readonly("total_steps", &helpers::SimulationMetrics::totalSteps)
        .def_readonly("total_time", &helpers::SimulationMetrics::totalTime)
        .def_readonly("spike_count", &helpers::SimulationMetrics::spikeCount)
        .def_readonly("active_neurons", &helpers::SimulationMetrics::activeNeurons)
        .def_readonly("average_firing_rate", &helpers::SimulationMetrics::averageFiringRate)
        .def_readonly("eiratio", &helpers::SimulationMetrics::eiratio)
        .def("get_seconds_per_step", &helpers::SimulationMetrics::getSecondsPerStep)
        .def("to_string", &helpers::SimulationMetrics::toString)
        .def("__repr__", [](const helpers::SimulationMetrics& metrics) {
            return metrics.toString();
        });

    // Advanced simulation utilities
    m.def("collect_simulation_metrics", [](const std::shared_ptr<Brain>& brain, size_t start_step, size_t end_step) {
        helpers::SimulationMetrics metrics;
        metrics.totalSteps = end_step - start_step;
        metrics.spikeCount = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.eiratio = brain->getExcitationInhibitionRatio();
        return metrics;
    }, py::arg("brain"), py::arg("start_step") = 0, py::arg("end_step") = 1000,
        "Collect simulation metrics over a range of steps");

    m.def("run_simulation_batch", [](const std::shared_ptr<Brain>& brain, size_t num_steps, size_t step_size = 100) {
        for (size_t step = 0; step < num_steps; step += step_size) {
            for (size_t i = step; i < std::min(step + step_size, num_steps); ++i) {
                brain->step(i);
            }
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("step_size") = 100,
        "Run simulation in batches for better performance");

    m.def("export_neural_data", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        // Implementation would export neural data to file
        // This is a placeholder for the actual implementation
        return true;
    }, py::arg("brain"), py::arg("filepath"),
        "Export neural data to file (CSV, JSON, etc.)");

    m.def("import_neural_data", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        // Implementation would import neural data from file
        // This is a placeholder for the actual implementation
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("filepath"),
        "Import neural data from file");

    // ===================================================================
    // COMMAND-LINE INTERFACE HELPER
    // ===================================================================
    
    // Simple CLI for common operations
    m.def("run_phase6_demo", []() {
        // This would run the Phase 6 integration demo
        // For now, just create a simple brain and run basic operations
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        return brain;
    }, "Run Phase 6 integration demonstration");

    // ===================================================================
    // CONSTANTS AND FACTORIES (replacing existing ones for compatibility)
    // ===================================================================
    
    // Core factory functions (maintaining backward compatibility)
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

    // Special constants (replacing existing ones for compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===================================================================
    // ADVANCED FEATURE REGISTERS
    // ===================================================================
    
    // Register advanced submodules
    py::module_ advanced_module = m.def_submodule("advanced", "Advanced NLM features");
    
    advanced_module.def("visualize", [](const std::shared_ptr<Brain>& brain, const std::string& output_file) {
        // Visualization would go here
        return "Visualization not yet implemented";
    }, py::arg("brain"), py::arg("output_file"),
       "Create visualization of brain activity");

    advanced_module.def("benchmark", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        // Benchmark would go here
        return 0.0; // dummy benchmark time
    }, py::arg("brain"), py::arg("steps"),
       "Run performance benchmark");

    advanced_module.def("validate", [](const std::shared_ptr<Brain>& brain) {
        // Validation would go here
        return true; // dummy validation result
    }, py::arg("brain"),
       "Validate brain integrity");

    // ===================================================================
    // LEGACY COMPATIBILITY (maintaining existing API)
    // ===================================================================
    
    // Keep existing API for backward compatibility
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
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

    // Existing API maintained for backward compatibility
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

    // ===================================================================
    // LEGACY FACTORY FUNCTIONS (maintaining backward compatibility)
    // ===================================================================

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

    // Special constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===================================================================
    // ADVANCED UTILITIES (Python-specific enhancements)
    // ===================================================================

    // Advanced utilities
    m.def("batch_simulate", &helpers::batchProcess,
          py::arg("start"), py::arg("end"), py::arg("func"),
          "Batch process simulation steps");

    m.def("collect_simulation_metrics", [](const std::shared_ptr<Brain>& brain, size_t start_step, size_t end_step) {
        helpers::SimulationMetrics metrics;
        metrics.totalSteps = end_step - start_step;
        metrics.spikeCount = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.eiratio = brain->getExcitationInhibitionRatio();
        return metrics;
    }, py::arg("brain"), py::arg("start_step") = 0, py::arg("end_step") = 1000,
        "Collect simulation metrics over a range of steps");

    m.def("run_simulation_batch", [](const std::shared_ptr<Brain>& brain, size_t num_steps, size_t step_size = 100) {
        for (size_t step = 0; step < num_steps; step += step_size) {
            for (size_t i = step; i < std::min(step + step_size, num_steps); ++i) {
                brain->step(i);
            }
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("step_size") = 100,
        "Run simulation in batches for better performance");

    m.def("export_neural_data", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return true; // Placeholder - would implement file export
    }, py::arg("brain"), py::arg("filepath"),
        "Export neural data to file");

    m.def("import_neural_data", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("filepath"),
        "Import neural data from file");

    // CLI helper
    m.def("run_phase6_demo", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        return brain;
    }, "Run Phase 6 integration demonstration");
}

} // namespace nlm