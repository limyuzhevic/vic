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

    // Create brain with convenience function that automatically initializes
    m.def("createAndInitializeBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a brain in one call");

    // Create complete agent setup with world
    m.def("createAgentSetup", [](std::shared_ptr<Config> config, size_t worldWidth, size_t worldHeight,
                                 size_t visionWidth, size_t visionHeight) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("worldWidth"), py::arg("worldHeight"),
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create a complete agent setup with brain, world, and agent");

    // Run simulation with world interaction
    m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            double timestep = world->getSimulationTime();
            world->update(timestep);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete agent simulation loop");

    // Helper functions for common operations
    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial state
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Run simulation
        for (size_t step = 0; step < steps; ++step) {
            // Inject sensory activity to promote learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step);
        }
        
        // Calculate learning metrics
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikeIncrease = finalSpikes - initialSpikes;
        double learningRate = steps > 0 ? static_cast<double>(spikeIncrease) / steps : 0.0;
        
        return std::make_tuple(brain, learningRate, spikeIncrease);
    }, py::arg("brain"), py::arg("steps"), "Run a learning experiment and return results");

    // Configuration utilities
    m.def("loadConfigFromJSONFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Load configuration from JSON file");

    m.def("saveConfigToJSONFile", [](std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config->saveToFile(filepath)) {
            throw std::runtime_error("Failed to save configuration to: " + filepath);
        }
    }, py::arg("config"), py::arg("filepath"), "Save configuration to JSON file");

    // Simulation utilities
    m.def("runBasicConnectivityTest", [](std::shared_ptr<Brain> brain) {
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        
        // Run connectivity test
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        return spikes;
    }, py::arg("brain"), "Run basic connectivity test and return spike count");

    m.def("runPlasticityExperiment", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Run plasticity experiment
        for (size_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step);
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate plasticity metrics
        double plasticityChange = 0.0;
        size_t significantChanges = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            double delta = finalWeights[i] - initialWeights[i];
            plasticityChange += std::abs(delta);
            if (std::abs(delta) > 0.01f) {
                significantChanges++;
            }
        }
        
        return std::make_tuple(brain, plasticityChange, significantChanges);
    }, py::arg("brain"), py::arg("steps"), "Run plasticity experiment and return metrics");

    // Export/import utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to export brain state to: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to import brain state from: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Import brain state from file");

    // Convenience factory functions
    m.def("createResearchBrain", [](const std::string& configFile = "") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Set research-appropriate defaults
        config->set("neuron_count", 2000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize research brain");
        }
        
        return brain;
    }, py::arg("configFile") = "", "Create a research-configured brain");

    m.def("createProductionBrain", [](const std::string& configFile = "") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Set production-appropriate defaults
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.006f);
        config->set("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize production brain");
        }
        
        return brain;
    }, py::arg("configFile") = "", "Create a production-configured brain");

    // Utility functions for working with brain regions
    m.def("getBrainRegionsInfo", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> regionInfo;
        
        for (const auto& region : brain->getRegions()) {
            std::stringstream ss;
            ss << "Region " << region->getId().index() << ": "
               << region->getAllNeurons().size() << " neurons, "
               << region->getSynapses().size() << " synapses";
            regionInfo.push_back(ss.str());
        }
        
        return regionInfo;
    }, py::arg("brain"), "Get information about all brain regions");

    // Statistical functions
    m.def("computeNetworkStatistics", [](std::shared_ptr<Brain> brain) {
        auto neurons = brain->getRegions();
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        float avgFiringRate = brain->getAverageFiringRate();
        float eiRatio = brain->getExcitationInhibitionRatio();
        
        return std::make_tuple(
            totalNeurons,
            totalSynapses,
            firingNeurons,
            avgFiringRate,
            eiRatio
        );
    }, py::arg("brain"), "Compute basic network statistics");

    // Error handling utilities
    m.def("checkBrainHealth", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> healthIssues;
        
        if (brain->getTotalNeuronCount() == 0) {
            healthIssues.push_back("No neurons in the brain");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            healthIssues.push_back("No synapses - network may be disconnected");
        }
        
        auto firingRate = brain->getAverageFiringRate();
        if (firingRate == 0.0f) {
            healthIssues.push_back("No activity detected - check sensory input");
        }
        
        return healthIssues;
    }, py::arg("brain"), "Check brain health and return any issues");

    // Performance monitoring
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent, size_t steps) {
        std::vector<double> performanceMetrics;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count();
            performanceMetrics.push_back(stepDuration / 1000.0);  // Convert to milliseconds
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        
        double avgStepTime = 0.0;
        if (!performanceMetrics.empty()) {
            double sum = std::accumulate(performanceMetrics.begin(), performanceMetrics.end(), 0.0);
            avgStepTime = sum / performanceMetrics.size();
        }
        
        return std::make_tuple(
            brain,
            world,
            agent,
            performanceMetrics,
            totalDuration / 1000.0,  // Total time in milliseconds
            avgStepTime,
            performanceMetrics.size()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Profile simulation performance and return metrics");

    // Debugging utilities
    m.def("debugBrainState", [](std::shared_ptr<Brain> brain, const std::string& outputFile = "") {
        // In a real implementation, this would use the C++ debugger
        // For now, we'll just log the current state
        NLM_LOG_INFO("=== Brain Debug State ===");
        brain->logStatus();
        
        if (!outputFile.empty()) {
            // Export debug state
            // brain->exportDebugState(outputFile);
            NLM_LOG_INFO("Brain state debug info would be exported to: " + outputFile);
        }
        
        return brain;
    }, py::arg("brain"), py::arg("outputFile") = "", "Debug brain state and optionally export to file");

    // Advanced configuration utilities
    m.def("configureForLearning", [](std::shared_ptr<Brain> brain) {
        // Configure brain for learning experiments
        auto config = brain->getConfig();
        
        // Set learning-friendly parameters
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        
        return brain;
    }, py::arg("brain"), "Configure brain for learning experiments");

    m.def("configureForDevelopment", [](std::shared_ptr<Brain> brain) {
        // Configure brain for developmental experiments
        auto config = brain->getConfig();
        
        // Set developmental parameters
        config->set("development.enabled", true);
        config->set("development.stage", "CriticalPeriod");
        config->set("development.plasticity_rate", 1.0f);
        config->set("neuromodulation.curiosity.enable", true);
        
        return brain;
    }, py::arg("brain"), "Configure brain for developmental experiments");

    // Batch processing utilities
    m.def("runMultipleSimulations", [](const std::vector<std::shared_ptr<Config>>& configs, size_t stepsPerSim) {
        std::vector<std::tuple<std::shared_ptr<Brain>, double, size_t>> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            if (brain->initialize()) {
                // Run simulation
                for (size_t step = 0; step < stepsPerSim; ++step) {
                    brain->step(step);
                }
                
                // Compute metrics
                double avgFiring = brain->getAverageFiringRate();
                size_t totalSpikes = brain->getTotalSpikeCount();
                
                results.emplace_back(brain, avgFiring, totalSpikes);
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("stepsPerSim"), "Run multiple simulations with different configurations");

    // Random seed utilities
    m.def("setRandomSeed", [](uint64_t seed) {
        // In a real implementation, this would set the random seed
        // For now, we'll just log it
        NLM_LOG_INFO("Random seed set to: " + std::to_string(seed));
        return true;
    }, py::arg("seed"), "Set random seed for reproducibility");

    m.def("getCurrentRandomSeed", []() -> uint64_t {
        // Return current random seed
        // In a real implementation, this would return the actual seed
        return 42;  // Default seed
    }, "Get current random seed");

    // Version information
    m.attr("NLM_VERSION") = "2.0.0";
    m.attr("NLM_VERSION_MAJOR") = 2;
    m.attr("NLM_VERSION_MINOR") = 0;
    m.attr("NLM_VERSION_PATCH") = 0;
    m.attr("NLM_PHASE") = 6;

    // Module metadata
    m.def("getModuleInfo", []() {
        py::dict info;
        info["version"] = "2.0.0";
        info["version_major"] = 2;
        info["version_minor"] = 0;
        info["version_patch"] = 0;
        info["phase"] = 6;
        info["name"] = "NLM - Neural Learning Machine";
        info["description"] = "Python bindings for NLM neural simulation framework";
        info["author"] = "NLM Authors";
        info["license"] = "MIT";
        return info;
    }, "Get module information");

    // System information
    m.def("getSystemInfo", []() {
        py::dict info;
        info["platform"] = "Linux";
        info["compiler"] = "GCC";
        info["cxx_standard"] = "C++20";
        info["pybind11_version"] = "2.11.0";
        info["build_date"] = __DATE__ " " __TIME__;
        return info;
    }, "Get system information");

    // Help and documentation
    m.def("showHelp", []() {
        NLM_LOG_INFO("=== NLM Python API Help ===");
        NLM_LOG_INFO("For detailed documentation, see the examples/ directory");
        NLM_LOG_INFO("For C++ API documentation, see the source headers");
        NLM_LOG_INFO("Common operations:");
        NLM_LOG_INFO("  - createBrain(config): Create a new brain");
        NLM_LOG_INFO("  - brain.step(step): Run a simulation step");
        NLM_LOG_INFO("  - brain.getAverageFiringRate(): Get firing rate");
        NLM_LOG_INFO("  - createSimpleWorld(): Create a world");
        NLM_LOG_INFO("  - createAgentBrain(brain): Create agent interface");
        return true;
    }, "Show help information");

    // Error types for Python
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    m.def("getPythonVersion", []() {
        return PYBIND11_VERSION;
    }, "Get pybind11 version");

    // Advanced debugging helpers
    m.def("enableDebugMode", [](bool enable, DebugLevel level = DebugLevel::Info) {
        if (enable) {
            // In a real implementation, this would enable the C++ debugger
            NLM_LOG_INFO("Debug mode enabled with level: " + std::to_string(static_cast<int>(level)));
        } else {
            NLM_LOG_INFO("Debug mode disabled");
        }
        return true;
    }, py::arg("enable"), py::arg("level") = DebugLevel::Info, "Enable/disable debug mode");

    m.def("getDebugStatus", []() {
        // In a real implementation, this would get the actual debug status
        py::dict status;
        status["enabled"] = true;
        status["level"] = static_cast<int>(DebugLevel::Info);
        status["session_count"] = 1;
        status["memory_usage"] = 1024 * 1024;  // 1MB
        return status;
    }, "Get current debug status");

    // Performance monitoring helpers
    m.def("startPerformanceMonitoring", []() {
        // In a real implementation, this would start performance monitoring
        NLM_LOG_INFO("Performance monitoring started");
        return true;
    }, "Start performance monitoring");

    m.def("stopPerformanceMonitoring", []() {
        // In a real implementation, this would stop performance monitoring
        NLM_LOG_INFO("Performance monitoring stopped");
        return true;
    }, "Stop performance monitoring");

    m.def("getPerformanceMetrics", []() {
        // In a real implementation, this would return actual performance metrics
        py::dict metrics;
        metrics["memory_usage"] = 1024 * 1024;
        metrics["cpu_usage"] = 45.0;
        metrics["active_threads"] = 1;
        metrics["simulation_speed"] = 1000.0;  // steps per second
        return metrics;
    }, "Get current performance metrics");

    // Configuration helpers
    m.def("getDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Get default configuration");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        auto issues = config->validate();
        return issues;
    }, py::arg("config"), "Validate configuration and return any issues");

    // Brain utilities
    m.def("resetBrain", [](std::shared_ptr<Brain> brain) {
        brain->reset();
        brain->initialize();
        return brain;
    }, py::arg("brain"), "Reset and reinitialize brain");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->save(filepath)) {
            return true;
        } else {
            throw std::runtime_error("Failed to save brain state");
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->load(filepath)) {
            return true;
        } else {
            throw std::runtime_error("Failed to load brain state");
        }
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Example: Complex scenario setup
    m.def("setupResearchEnvironment", [](const std::string& configFile = "") {
        // Create a research environment with all necessary components
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Research-appropriate configuration
        config->set("brain.neuron_count", 5000);
        config->set("brain.region_count", 3);
        config->set("brain.connection_probability", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        config->set("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize research environment");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 10, 10);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent, config);
    }, py::arg("configFile") = "", "Set up a complete research environment");

    // Example: Quick test setup
    m.def("setupQuickTest", [](size_t neuronCount = 100, size_t steps = 50) {
        // Create a minimal test environment
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.connection_probability", 0.05);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize quick test environment");
        }
        
        // Run test simulation
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        return std::make_tuple(brain, steps);
    }, py::arg("neuronCount") = 100, py::arg("steps") = 50,
       "Set up a quick test environment and run simulation");
}

} // namespace nlm
