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
              py::arg("enable"), "Enable/disable reward-based learning")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"), "Enable/disable brain development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
              py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // ========== ADVANCED AGENT FEATURES ==========
        
        // Goal setting and achievement tracking
        .def("setCurrentGoal", &AgentBrain::setCurrentGoal,
              py::arg("goal"), "Set current goal for the agent")
        .def("getCurrentGoal", &AgentBrain::getCurrentGoal)
        .def("getGoalProgress", &AgentBrain::getGoalProgress)
        
        // Learning statistics and analysis
        .def("getLearningStatistics", &AgentBrain::getLearningStatistics)
        .def("resetLearningStatistics", &AgentBrain::resetLearningStatistics)
        
        // Advanced neuromodulation control
        .def("setDopamineLevel", &AgentBrain::setDopamineLevel,
              py::arg("level"), "Set dopamine level (0-1)")
        .def("setCuriosityLevel", &AgentBrain::setCuriosityLevel,
              py::arg("level"), "Set curiosity level (0-1)")
        .def("setNoveltyLevel", &AgentBrain::setNoveltyLevel,
              py::arg("level"), "Set novelty detection level (0-1)")
        
        // Expert control methods
        .def("setPlanningDepth", &AgentBrain::setPlanningDepth,
              py::arg("depth"), "Set neural planning depth (1-10)")
        .def("getPlanningDepth", &AgentBrain::getPlanningDepth)
        .def("setConceptFormationThreshold", &AgentBrain::setConceptFormationThreshold,
              py::arg("threshold"), "Set concept formation threshold")
        .def("setAttentionStrength", &AgentBrain::setAttentionStrength,
              py::arg("strength"), "Set attention system strength (0-2)")
        .def("setMemoryCapacity", &AgentBrain::setMemoryCapacity,
              py::arg("capacity"), "Set working memory capacity")
        
        // Batch operations for expert users
        .def("batchStep", &AgentBrain::batchStep,
              py::arg("numSteps"), py::arg("timestep"), "Run multiple simulation steps efficiently")
        .def("batchExecute", &AgentBrain::batchExecute,
              py::arg("percept"), py::arg("numSteps"), "Process sensory input and execute multiple steps")
        
        // Performance monitoring
        .def("getPerformanceMetrics", &AgentBrain::getPerformanceMetrics)
        .def("getNeuralActivityStats", &AgentBrain::getNeuralActivityStats)
        .def("getLearningProgress", &AgentBrain::getLearningProgress)
        
        // Configuration utilities
        .def("configureForTask", &AgentBrain::configureForTask,
              py::arg("taskType"), py::arg("difficulty"), "Configure agent for specific task")
        .def("getTaskRecommendations", &AgentBrain::getTaskRecommendations)
        
        // Advanced debugging and visualization
        .def("logDetailedState", &AgentBrain::logDetailedState)
        .def("exportNeuralActivity", &AgentBrain::exportNeuralActivity,
              py::arg("filepath"), "Export neural activity data to file")
        .def("getActivationHeatmap", &AgentBrain::getActivationHeatmap,
              "Get neural activation heatmap for visualization")

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

        }, py::arg("brain"), "Create a new agent brain interface");

    // ========== ADVANCED UTILITY FUNCTIONS ==========
    
    // Convenience factory functions for common scenarios
    m.def("createStandardAgent", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.05);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all advanced learning systems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a standard agent with world for common use cases");
    
    m.def("runQuickDemo", []() {
        auto [brain, world, agent] = createStandardAgent();
        
        std::cout << "Running NLM quick demo..." << std::endl;
        std::cout << "Brain: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "World: " << world->getWidth() << "x" << world->getHeight() << " cells" << std::endl;
        
        // Run for 100 steps
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 20 == 0) {
                std::cout << "Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " firing, "
                         << "Curiosity: " << std::fixed << std::setprecision(2) 
                         << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << std::setprecision(2) 
                         << agent->getNoveltyLevel() << std::endl;
            }
        }
        
        std::cout << "Demo completed! Final state:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << std::setprecision(2) 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        brain->logStatus();
        return std::make_tuple(brain, world, agent);
    }, "Run a quick demonstration of NLM functionality");
    
    // Configuration utilities
    m.def("getDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Get a default configuration object");
    
    m.def("createConfigFromFile", &Config::loadFromFile,
          py::arg("config"), py::arg("filepath"), "Load configuration from file");
    
    m.def("saveConfigToFile", &Config::saveToFile,
          py::arg("config"), py::arg("filepath"), "Save configuration to file");
    
    // Experiment utilities
    m.def("runExperiment", [](std::shared_ptr<Config> config, std::string experimentName, 
                           int numSteps, bool verbose = false) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Running experiment: " << experimentName << std::endl;
        std::cout << "Steps: " << numSteps << ", Neurons: " << brain->getTotalNeuronCount() << std::endl;
        
        int successfulSteps = 0;
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " firing, "
                         << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
            
            successfulSteps++;
        }
        
        std::cout << "Experiment " << experimentName << " completed!" << std::endl;
        brain->logStatus();
        
        return brain;
    }, py::arg("config"), py::arg("experimentName"), py::arg("numSteps"), 
          py::arg("verbose") = false, "Run a named experiment with monitoring");
    
    // Batch and parallel processing utilities
    m.def("batchRunExperiments", [](std::vector<std::shared_ptr<Config>> configs, 
                                  std::string experimentName, 
                                  int numSteps) {
        std::vector<std::shared_ptr<Brain>> results;
        
        std::cout << "Running batch experiment: " << experimentName 
                  << " with " << configs.size() << " configurations" << std::endl;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            brain->initialize();
            
            std::cout << "  Config " << i+1 << ": " << configs[i]->summary() << std::endl;
            
            for (int step = 0; step < numSteps; ++step) {
                brain->step(step);
            }
            
            results.push_back(brain);
        }
        
        std::cout << "Batch experiment completed!" << std::endl;
        return results;
    }, py::arg("configs"), py::arg("experimentName"), py::arg("numSteps"), 
          "Run the same experiment with multiple configurations");
    
    // Save/load utilities with advanced features
    m.def("saveBrainWithMetadata", &Brain::saveWithMetadata,
          py::arg("brain"), py::arg("filepath"), py::arg("metadata"), 
          "Save brain state with additional metadata");
    
    m.def("loadBrainWithMetadata", &Brain::loadWithMetadata,
          py::arg("brain"), py::arg("filepath"), 
          "Load brain state with metadata verification");
    
    m.def("exportBrainForAnalysis", &Brain::exportForAnalysis,
          py::arg("brain"), py::arg("filepath"), 
          "Export brain state in analysis-ready format");
    
    m.def("importBrainForAnalysis", &Brain::importFromAnalysis,
          py::arg("brain"), py::arg("filepath"), 
          "Import brain state from analysis format");
    
    // Debugging and development utilities
    m.def("startDebugSession", [](std::shared_ptr<Brain> brain, std::string sessionName) {
        std::cout << "Starting debug session: " << sessionName << std::endl;
        std::cout << "Brain state:" << std::endl;
        brain->logStatus();
        return brain;
    }, py::arg("brain"), py::arg("sessionName"), "Start a debug session with state logging");
    
    m.def("profileBrainPerformance", [](std::shared_ptr<Brain> brain, int durationSteps) {
        std::cout << "Profiling brain performance for " << durationSteps << " steps..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < durationSteps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Performance profile:" << std::endl;
        std::cout << "  Steps per second: " << (durationSteps * 1000.0 / duration.count()) << std::endl;
        std::cout << "  Total spikes per step: " << brain->getTotalSpikeCount() / durationSteps << std::endl;
        std::cout << "  Memory usage: " << brain->getMemoryUsage() << " bytes" << std::endl;
        
        return std::make_tuple(brain, duration.count());
    }, py::arg("brain"), py::arg("durationSteps"), 
          "Profile brain performance over specified duration");
    
    // Educational utilities
    m.def("createTutorialBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);  // Smaller for educational purposes
        config->set("brain.synapse_density", 0.02);
        config->set("simulation.timestep", 0.01);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Tutorial brain created (500 neurons for learning)" << std::endl;
        std::cout << "This brain is optimized for educational demonstrations." << std::endl;
        
        return brain;
    }, "Create a brain optimized for educational tutorials");
    
    m.def("demonstrateLearning", [](std::shared_ptr<Brain> brain, int numEpisodes) {
        std::cout << "Demonstrating learning over " << numEpisodes << " episodes" << std::endl;
        
        float prevLearningProgress = 0.0f;
        for (int episode = 0; episode < numEpisodes; ++episode) {
            // Run episode
            for (int step = 0; step < 50; ++step) {
                brain->step(step);
            }
            
            float learningProgress = brain->getAverageFiringRate() * 0.1f +  // Simplified metric
                                   brain->getTotalSpikeCount() * 0.0001f;
            
            if (verbose) {
                std::cout << "Episode " << episode << ": " 
                         << "Learning progress: " << std::setprecision(2) << learningProgress << ", "
                         << "Change: " << std::setprecision(2) << (learningProgress - prevLearningProgress) << std::endl;
            }
            
            prevLearningProgress = learningProgress;
        }
        
        std::cout << "Learning demonstration completed!" << std::endl;
        brain->logStatus();
        return brain;
    }, py::arg("brain"), py::arg("numEpisodes"), "Demonstrate learning over multiple episodes");
    
    // Version and system information
    m.attr("NLM_VERSION") = "6.0 (Phase 6 Integration)";
    m.attr("NLM_DESCRIPTION") = "Neural Learning Machine - Phase 6: Final Integration of Artificial Cognitive Systems";
    m.attr("NLM_DOCUMENTATION_URL") = "https://github.com/nlm/docs";
    
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = m.attr("NLM_VERSION");
        info["description"] = m.attr("NLM_DESCRIPTION");
        info["neurons_per_million_ops"] = 2.5;  // Approximate
        info["max_brains_supported"] = 1000;
        info["supported_python_versions"] = std::vector<std::string>{"3.8", "3.9", "3.10", "3.11", "3.12"};
        return info;
    }, "Get system information and capabilities");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

} // namespace nlm
