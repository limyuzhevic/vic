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
    
    // ========== ADVANCED PYTHON API FUNCTIONS ==========
    
    // Brain analysis and statistics
    m.def("analyzeBrainDynamics", [](std::shared_ptr<Brain> brain, size_t steps) {
        std::map<std::string, double> stats;
        
        if (!brain || !brain->initialize()) {
            return stats;
        }
        
        double totalSpikeTime = 0.0;
        size_t totalSpikes = 0;
        size_t maxFiring = 0;
        size_t minFiring = std::numeric_limits<size_t>::max();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            size_t firing = brain->getFiringNeuronCount();
            totalSpikes += firing;
            totalSpikeTime += firing;
            
            if (firing > maxFiring) maxFiring = firing;
            if (firing < minFiring && firing > 0) minFiring = firing;
        }
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        double avgFiring = totalSpikeTime / steps;
        double firingVariance = 0.0;
        
        stats["average_firing_rate"] = avgFiring;
        stats["max_firing_rate"] = maxFiring;
        stats["min_firing_rate"] = minFiring > 0 ? minFiring : 0.0;
        stats["total_spikes"] = totalSpikes;
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["neurons_count"] = totalNeurons;
        
        return stats;
    }, py::arg("brain"), py::arg("steps"),
         "Analyze brain dynamics over time steps and return statistical metrics");
    
    // Configuration validation and utilities
    m.def("validateConfiguration", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Check for required configuration keys
        std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "brain.v_thresh"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate parameter ranges
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount <= 0) {
            errors.push_back("brain.neuron_count must be positive (got " + std::to_string(neuronCount) + ")");
        }
        
        auto connProb = config.getOr<double>("brain.connection_probability", 0.1);
        if (connProb < 0.0 || connProb > 1.0) {
            errors.push_back("brain.connection_probability must be between 0 and 1 (got " + std::to_string(connProb) + ")");
        }
        
        auto threshold = config.getOr<double>("brain.v_thresh", -50.0);
        if (threshold > 0) {
            errors.push_back("brain.v_thresh should be negative (got " + std::to_string(threshold) + ")");
        }
        
        return errors;
    }, py::arg("config"),
         "Validate configuration and return list of errors");
    
    // Advanced brain simulation control
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                          std::shared_ptr<AgentBrain> agentBrain, size_t maxSteps) {
        if (!brain || !world || !agentBrain) {
            throw std::runtime_error("Invalid brain, world, or agent brain");
        }
        
        // Initialize systems
        brain->initialize();
        world->reset();
        agentBrain->initialize(*world);
        
        // Run simulation
        double totalReward = 0.0;
        size_t stepsCompleted = 0;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input from world
            const auto& percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command from brain activity
            auto motorCmd = agentBrain->decodeMotorCommand();
            
            // Apply motor command to world
            auto result = world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Apply reward modulation
            agentBrain->applyRewardModulation(result.reward, 0.0f);
            
            totalReward += result.reward;
            stepsCompleted++;
            
            // Update development
            agentBrain->updateDevelopment(0.1);
            
            // Check for termination conditions
            if (!result.success) {
                break;
            }
        }
        
        // Return episode statistics
        std::map<std::string, double> episodeStats;
        episodeStats["total_reward"] = totalReward;
        episodeStats["steps_completed"] = stepsCompleted;
        episodeStats["average_reward_per_step"] = stepsCompleted > 0 ? totalReward / stepsCompleted : 0.0;
        episodeStats["final_dopamine_level"] = agentBrain->getNeuromodulationLevel();
        episodeStats["final_curiosity_level"] = agentBrain->getCuriosityLevel();
        
        return episodeStats;
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain"), py::arg("maxSteps"),
         "Run a complete simulation episode and return statistics");
    
    // Brain checkpointing utilities
    m.def("checkpointBrain", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Save brain state to file for checkpointing");
    
    m.def("restoreBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Load brain state from file (checkpoint)");
    
    // Statistical analysis functions
    m.def("computeBrainStatistics", [](const Brain& brain) {
        std::map<std::string, double> stats;
        
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Compute additional metrics
        if (brain.getTotalNeuronCount() > 0) {
            double firingRate = brain.getAverageFiringRate();
            stats["firing_efficiency"] = firingRate / brain.getTotalNeuronCount();
            stats["network_activity"] = (double)brain.getFiringNeuronCount() / brain.getTotalNeuronCount();
        }
        
        return stats;
    }, py::arg("brain"),
         "Compute comprehensive brain statistics");
    
    // Configuration template creation
    m.def("createBrainConfigTemplate", []() {
        auto config = std::make_shared<Config>();
        
        // Brain parameters
        config->set("brain.neuron_count", 1000, ConfigSource::Default, "Total number of neurons");
        config->set("brain.synapse_density", 0.1, ConfigSource::Default, "Synapse connectivity density (0.0-1.0)");
        config->set("brain.connection_probability", 0.05, ConfigSource::Default, "Connection probability between neurons");
        config->set("brain.v_thresh", -50.0, ConfigSource::Default, "Neuron firing threshold (mV)");
        config->set("brain.v_rest", -70.0, ConfigSource::Default, "Resting potential (mV)");
        config->set("brain.v_reset", -75.0, ConfigSource::Default, "Reset potential after spike (mV)");
        config->set("brain.tau_mem", 20.0, ConfigSource::Default, "Membrane time constant (ms)");
        
        // Plasticity
        config->set("plasticity.stdp.enable", true, ConfigSource::Default, "Enable Spike-Timing Dependent Plasticity");
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default, "STDP learning rate");
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default, "Enable Hebbian plasticity");
        
        // World parameters
        config->set("world.width", 20.0, ConfigSource::Default, "World width");
        config->set("world.height", 20.0, ConfigSource::Default, "World height");
        
        // Simulation
        config->set("simulation.timestep", 0.001, ConfigSource::Default, "Simulation timestep (s)");
        config->set("random_seed", 42, ConfigSource::Default, "Random seed for reproducibility");
        
        return config;
    }, "Create a comprehensive brain configuration template");
    
    // Visualization helpers (console-based)
    m.def("visualizeBrainNetwork", [](const Brain& brain, bool showRegions = true, bool showConnections = true) {
        std::stringstream output;
        output << "=== NLM Brain Network Visualization ===\n\n";
        
        auto regionIds = brain.getRegionIds();
        output << "Network Statistics:" << std::endl;
        output << "  Total Regions: " << regionIds.size() << std::endl;
        output << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        output << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        output << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        output << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        
        if (showRegions) {
            output << "\nRegions:" << std::endl;
            for (size_t i = 0; i < regionIds.size(); ++i) {
                auto* region = brain.getRegion(regionIds[i]);
                if (region) {
                    output << "  Region " << regionIds[i].value() << ": " << region->getName() << std::endl;
                    output << "    Neurons: " << region->getTotalNeuronCount() << " (" << region->getPopulationCount() << " populations)" << std::endl;
                }
            }
        }
        
        output << "\nVisualization complete! (Console-based network diagram not implemented)";
        return output.str();
    }, py::arg("brain"), py::arg("showRegions") = true, py::arg("showConnections") = true,
         "Generate text-based visualization of brain network structure");
    
    // Error handling and exception utilities
    m.def("setBrainLogLevel", [](int level) {
        auto logger = Logger::getInstance();
        logger->setLevel(static_cast<Logger::Level>(level));
    }, py::arg("level"),
         "Set NLM logger level (0=Off, 1=Error, 2=Warning, 3=Info, 4=Debug)");
    
    m.def("getBrainLogLevel", []() {
        auto logger = Logger::getInstance();
        return static_cast<int>(logger->getLevel());
    }, "Get current NLM logger level");
    
    // Performance monitoring
    m.def("getPerformanceMetrics", [](const Brain& brain) {
        std::map<std::string, double> metrics;
        
        // Basic performance metrics
        metrics["total_spike_count"] = brain.getTotalSpikeCount();
        metrics["firing_neuron_count"] = brain.getFiringNeuronCount();
        metrics["active_neuron_count"] = brain.getActiveNeuronCount();
        metrics["average_firing_rate"] = brain.getAverageFiringRate();
        metrics["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        
        // Derived metrics
        size_t totalNeurons = brain.getTotalNeuronCount();
        if (totalNeurons > 0) {
            metrics["network_utilization"] = (double)brain.getFiringNeuronCount() / totalNeurons;
            metrics["synaptic_efficiency"] = brain.getTotalSpikeCount() / std::max(brain.getTotalSynapseCount(), 1u);
        }
        
        return metrics;
    }, py::arg("brain"),
         "Get performance metrics for monitoring brain operation");
    
    // Configuration file utilities
    m.def("exportConfiguration", [](const Config& config, const std::string& filename) {
        return config.saveToFile(filename);
    }, py::arg("config"), py::arg("filename"),
         "Export configuration to file");
    
    m.def("importConfiguration", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filename);
        return config;
    }, py::arg("filename"),
         "Import configuration from file");
    
    // Utility functions for advanced users
    m.def("getNetworkDensity", [](const Brain& brain) {
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t totalPossibleSynapses = totalNeurons * (totalNeurons - 1) / 2; // Assuming all-to-all possible
        return (double)brain.getTotalSynapseCount() / totalPossibleSynapses;
    }, py::arg("brain"),
         "Calculate network density (actual synapses / maximum possible)");
    
    m.def("getModularityScore", [](const Brain& brain) {
        // Simple modularity calculation based on region activity patterns
        auto regionIds = brain.getRegionIds();
        if (regionIds.size() < 2) return 0.0;
        
        double modularity = 0.0;
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto* regionI = brain.getRegion(regionIds[i]);
            if (!regionI) continue;
            
            for (size_t j = i + 1; j < regionIds.size(); ++j) {
                auto* regionJ = brain.getRegion(regionIds[j]);
                if (!regionJ) continue;
                
                // Calculate connectivity between regions
                double connectivity = 0.0;
                for (const auto* syn : regionI->getSynapses()) {
                    if (syn->getDestinationNeuron().index() >= brain.getTotalNeuronCount()) continue;
                    auto* regionK = brain.getRegion(regionIds[j]);
                    if (regionK && regionK->hasNeuron(syn->getDestinationNeuron().index())) {
                        connectivity += syn->getWeight();
                    }
                }
                
                modularity += connectivity * connectivity;
            }
        }
        
        return modularity;
    }, py::arg("brain"),
         "Calculate modularity score based on region connectivity patterns");
    
    // Debug and diagnostic functions
    m.def("diagnoseBrainState", [](const Brain& brain) {
        std::vector<std::string> diagnoses;
        
        // Check for common issues
        if (brain.getTotalNeuronCount() == 0) {
            diagnoses.push_back("ERROR: Brain has no neurons");
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            diagnoses.push_back("WARNING: Brain has no synapses");
        }
        
        if (brain.getAverageFiringRate() == 0.0) {
            diagnoses.push_back("WARNING: Brain has no firing activity");
        }
        
        if (brain.getExcitationInhibitionRatio() == 0.0) {
            diagnoses.push_back("WARNING: No excitation-inhibition balance");
        }
        
        if (diagnoses.empty()) {
            diagnoses.push_back("Brain state appears normal");
        }
        
        return diagnoses;
    }, py::arg("brain"),
         "Diagnostic function to check for common brain state issues");
}

} // namespace nlm
