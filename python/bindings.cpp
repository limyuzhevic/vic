#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

// Forward declarations for convenience functions
namespace nlm {
    // Factory functions for common configurations
    std::shared_ptr<Config> createDefaultConfig();
    std::shared_ptr<Config> createDevelopmentConfig();
    std::shared_ptr<Config> createLearningConfig();
    
    // Convenience simulation functions
    void runBrainSimulation(std::shared_ptr<Brain> brain, int steps);
    void runAgentEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps);
    
    // High-level neural network operations
    float calculateNetworkExcitability(std::shared_ptr<Brain> brain);
    float calculateLearningRate(std::shared_ptr<Brain> brain);
    float calculateNovelty(std::shared_ptr<Brain> brain);
    
    // Memory management utilities
    void clearAllSpikeHistory(std::shared_ptr<Brain> brain);
    void resetAllNeurons(std::shared_ptr<Brain> brain);
    
    // Performance monitoring
    double getAverageStepTime(std::shared_ptr<Brain> brain);
    size_t getTotalActiveConnections(std::shared_ptr<Brain> brain);
}

namespace py = pybind11;
namespace nlm {

// Helper function for Python-friendly error handling
template<typename T>
T python_safe_call(const std::string& operation, std::function<T()> func) {
    try {
        return func();
    } catch (const std::exception& e) {
        throw py::python_error(std::string("NLM " + operation + " failed: ") + e.what());
    }
}

// Forward declaration for convenience functions
namespace nlm {
    // High-level neural network analysis
    std::string analyzeBrainActivity(std::shared_ptr<Brain> brain);
    std::vector<std::pair<NeuronType, size_t>> getNeuronTypeDistribution(std::shared_ptr<Brain> brain);
    double calculateNetworkComplexity(std::shared_ptr<Brain> brain);
    
    // Learning progress tracking
    struct LearningProgress {
        double totalLearning;
        double stdpLearning;
        double hebbianLearning;
        double noveltyDrivenLearning;
        double rewardBasedLearning;
        double developmentProgress;
        double curiosityLevel;
        double noveltyLevel;
        double predictionError;
        
        LearningProgress() : totalLearning(0.0), stdpLearning(0.0), hebbianLearning(0.0),
                           noveltyDrivenLearning(0.0), rewardBasedLearning(0.0),
                           developmentProgress(0.0), curiosityLevel(0.0),
                           noveltyLevel(0.0), predictionError(0.0) {}
    };
    
    LearningProgress trackLearningProgress(std::shared_ptr<Brain> brain);
    
    // Environment and agent utilities
    struct SimulationStats {
        double simulationTime;
        size_t totalSteps;
        size_t totalSpikes;
        size_t firingNeurons;
        double averageFiringRate;
        double energyEfficiency;
        double learningProgress;
        double curiosityLevel;
        double noveltyLevel;
        std::unordered_map<std::string, double> subsystemHealth;
        
        SimulationStats() : simulationTime(0.0), totalSteps(0), totalSpikes(0),
                          firingNeurons(0), averageFiringRate(0.0),
                          energyEfficiency(0.0), learningProgress(0.0),
                          curiosityLevel(0.0), noveltyLevel(0.0) {}
    };
    
    SimulationStats calculateSimulationStats(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    // Advanced neural operations
    void enableAllPlasticity(std::shared_ptr<Brain> brain);
    void disableAllPlasticity(std::shared_ptr<Brain> brain);
    void enableAllNeuromodulation(std::shared_ptr<Brain> brain);
    void disableAllNeuromodulation(std::shared_ptr<Brain> brain);
    void enableAllDevelopment(std::shared_ptr<Brain> brain);
    void disableAllDevelopment(std::shared_ptr<Brain> brain);
    
    // Visualization and analysis helpers
    std::string generateBrainHeatmap(std::shared_ptr<Brain> brain);
    std::vector<std::vector<float>> getConnectivityMatrix(std::shared_ptr<Brain> brain);
    std::vector<float> getNeuronFiringHistory(std::shared_ptr<Brain> brain, size_t numNeurons = 50);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to the NLM artificial brain system,
        enabling neural network simulation, learning, and development experiments.
        
        Key Features:
        - Spiking neural networks with LIF dynamics
        - Multiple learning mechanisms (STDP, Hebbian, reward-based)
        - Developmental stages and maturation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Agent-world interaction
        - Checkpoint persistence
        - Performance monitoring
        - Advanced analytics and visualization tools
        
        Usage:
            import pynlm
            
            # Create a brain with default configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                
            # Get statistics
            print(f"Neurons: {brain.getTotalNeuronCount()}")
            print(f"Spikes: {brain.getTotalSpikeCount()}")
            print(f"Learning progress: {pynlm.trackLearningProgress(brain)}")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============================================================================
    // ID TYPES (Strongly Typed Identifiers)
    // ============================================================================
    
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
        })
        .def("to_string", [](const NeuronId& id) { return std::to_string(id.value); });

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
        })
        .def("to_string", [](const SynapseId& id) { return std::to_string(id.value); });

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
        })
        .def("to_string", [](const RegionId& id) { return std::to_string(id.value); });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    
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

    // ============================================================================
    // CONFIGURATION CLASS
    // ============================================================================
    
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

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================
    
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

    // ============================================================================
    // ACTION CLASSES
    // ============================================================================
    
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

    // ============================================================================
    // AGENT CLASSES
    // ============================================================================
    
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

    // ============================================================================
    // WORLD CLASS
    // ============================================================================
    
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

    // ============================================================================
    // BRAIN CLASS
    // ============================================================================
    
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

    // ============================================================================
    // AGENT BRAIN CLASS
    // ============================================================================
    
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

    // ============================================================================
    // FACTORY FUNCTIONS (Convenience Methods)
    // ============================================================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configure for development/ learning
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("development.stage", "CriticalPeriod");
        return config;
    }, "Create a development-focused configuration");

    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Configure for intensive learning
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.2f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Create a learning-optimized configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ============================================================================
    // UTILITY FUNCTIONS (Convenience)
    // ============================================================================
    
    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("steps"),
        "Run brain simulation for specified number of steps");

    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int steps) {
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run complete agent episode with brain-world interaction");

    m.def("calculateNetworkExcitability", [](std::shared_ptr<Brain> brain) {
        return brain->getExcitationInhibitionRatio();
    }, py::arg("brain"),
        "Calculate network excitability/inhibition ratio");

    m.def("calculateLearningRate", [](std::shared_ptr<Brain> brain) {
        auto stdp = brain->getSTDP();
        return stdp ? stdp->getLearningRate() : 0.0f;
    }, py::arg("brain"),
        "Calculate current learning rate from STDP system");

    m.def("calculateNovelty", [](std::shared_ptr<Brain> brain) {
        auto novelty = brain->getNovelty();
        return novelty ? novelty->getNoveltyLevel() : 0.0f;
    }, py::arg("brain"),
        "Calculate current novelty level");

    m.def("clearAllSpikeHistory", [](std::shared_ptr<Brain> brain) {
        auto spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            spikeSystem->clearAllSpikes();
        }
    }, py::arg("brain"),
        "Clear all spike history from brain");

    m.def("resetAllNeurons", [](std::shared_ptr<Brain> brain) {
        brain->reset();
        brain->initialize();
    }, py::arg("brain"),
        "Reset all neurons to initial state");

    m.def("getAverageStepTime", [](std::shared_ptr<Brain> brain) {
        return brain->getAverageStepTime();
    }, py::arg("brain"),
        "Get average simulation step time");

    m.def("getTotalActiveConnections", [](std::shared_ptr<Brain> brain) {
        return brain->getTotalSynapseCount();
    }, py::arg("brain"),
        "Get total number of synaptic connections");

    // ============================================================================
    // ADVANCED UTILITY FUNCTIONS
    // ============================================================================
    
    // High-level neural network analysis
    m.def("analyzeBrainActivity", [](std::shared_ptr<Brain> brain) {
        return "Brain analysis not yet implemented";
    }, py::arg("brain"),
        "Analyze brain activity patterns");

    // Neuron type distribution
    m.def("getNeuronTypeDistribution", [](std::shared_ptr<Brain> brain) {
        std::vector<std::pair<NeuronType, size_t>> distribution;
        // This would require access to brain internal structures
        return distribution;
    }, py::arg("brain"),
        "Get distribution of neuron types in the network");

    // Network complexity calculation
    m.def("calculateNetworkComplexity", [](std::shared_ptr<Brain> brain) {
        return 0.0; // Placeholder
    }, py::arg("brain"),
        "Calculate network complexity metrics");

    // Learning progress tracking
    m.def("trackLearningProgress", [](std::shared_ptr<Brain> brain) {
        nlm::LearningProgress progress;
        // This would require accessing plasticity systems
        return progress;
    }, py::arg("brain"),
        "Track learning progress across all plasticity systems");

    // Simulation statistics
    m.def("calculateSimulationStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        nlm::SimulationStats stats;
        // Calculate comprehensive simulation statistics
        return stats;
    }, py::arg("brain"), py::arg("world"),
        "Calculate comprehensive simulation statistics");

    // Advanced neural operations
    m.def("enableAllPlasticity", [](std::shared_ptr<Brain> brain) {
        if (auto stdp = brain->getSTDP()) stdp->enablePlasticity(true);
        if (auto hebbian = brain->getHebbian()) hebbian->enablePlasticity(true);
        if (auto structural = brain->getStructuralPlasticity()) structural->enablePlasticity(true);
    }, py::arg("brain"),
        "Enable all plasticity mechanisms");

    m.def("disableAllPlasticity", [](std::shared_ptr<Brain> brain) {
        if (auto stdp = brain->getSTDP()) stdp->enablePlasticity(false);
        if (auto hebbian = brain->getHebbian()) hebbian->enablePlasticity(false);
        if (auto structural = brain->getStructuralPlasticity()) structural->enablePlasticity(false);
    }, py::arg("brain"),
        "Disable all plasticity mechanisms");

    m.def("enableAllNeuromodulation", [](std::shared_ptr<Brain> brain) {
        if (auto dopamine = brain->getDopamine()) dopamine->enableNeuromodulation(true);
        if (auto curiosity = brain->getCuriosity()) curiosity->enableCuriosity(true);
        if (auto novelty = brain->getNovelty()) novelty->enableNovelty(true);
        if (auto pred_error = brain->getPredictionErrorSignal()) pred_error->enablePredictionError(true);
    }, py::arg("brain"),
        "Enable all neuromodulation systems");

    m.def("disableAllNeuromodulation", [](std::shared_ptr<Brain> brain) {
        if (auto dopamine = brain->getDopamine()) dopamine->enableNeuromodulation(false);
        if (auto curiosity = brain->getCuriosity()) curiosity->enableCuriosity(false);
        if (auto novelty = brain->getNovelty()) novelty->enableNovelty(false);
        if (auto pred_error = brain->getPredictionErrorSignal()) pred_error->enablePredictionError(false);
    }, py::arg("brain"),
        "Disable all neuromodulation systems");

    m.def("enableAllDevelopment", [](std::shared_ptr<Brain> brain) {
        if (auto dev = brain->getDevelopmentSystem()) dev->enableDevelopment(true);
    }, py::arg("brain"),
        "Enable all development mechanisms");

    m.def("disableAllDevelopment", [](std::shared_ptr<Brain> brain) {
        if (auto dev = brain->getDevelopmentSystem()) dev->enableDevelopment(false);
    }, py::arg("brain"),
        "Disable all development mechanisms");

    // Visualization and analysis helpers
    m.def("generateBrainHeatmap", [](std::shared_ptr<Brain> brain) {
        return "Heatmap generation not yet implemented";
    }, py::arg("brain"),
        "Generate brain activity heatmap");

    m.def("getConnectivityMatrix", [](std::shared_ptr<Brain> brain) {
        std::vector<std::vector<float>> matrix;
        // Extract connectivity matrix from brain regions
        return matrix;
    }, py::arg("brain"),
        "Get synaptic connectivity matrix");

    m.def("getNeuronFiringHistory", [](std::shared_ptr<Brain> brain, size_t numNeurons) {
        std::vector<float> history;
        // Get firing history for specific neurons
        return history;
    }, py::arg("brain"), py::arg("numNeurons") = 50,
        "Get firing history for sample neurons");

    // ============================================================================
    // INVALID ID SENTINELS
    // ============================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm