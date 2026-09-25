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

    // ===== High-Level Convenience APIs =====
    
    // 1. Easy brain initialization with standard configurations
    m.def("createQuickBrain", [](const std::string& configName = "basic") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        if (configName == "basic") {
            // Basic learning brain
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else if (configName == "advanced") {
            // Advanced exploration brain with more neurons
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.15);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 1.5);
        } else if (configName == "focused") {
            // Focused learning brain (small, specific)
            config->set("brain.neuron_count", 200);
            config->set("brain.synapse_density", 0.08);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", true);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("configName") = "basic", "Create a brain with predefined configuration (basic/advanced/focused)");
    
    m.def("createTrainingBrain", [](int neuronCount = 1000, bool enableDevelopment = true) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.synapse_density", 0.12);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        
        auto brain = std::make_shared<Brain>(config);
        if (enableDevelopment) {
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
        return brain;
    }, py::arg("neuronCount") = 1000, py::arg("enableDevelopment") = true, "Create a brain optimized for training/learning");
    
    // 2. Simple training loops for reinforcement learning
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                         int maxSteps, bool verbose = false) {
        agent->reset();
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        float totalReward = 0.0f;
        
        for (int step = 0; step < maxSteps; ++step) {
            // Update world dynamics
            world->update(0.1f);
            
            // Get sensory input from world
            auto percept = world->getSensoryPercept();
            
            // Process input and get action
            agent->processSensoryInput(percept);
            agent->getBrain()->step(step);
            auto action = agent->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply neuromodulation
            auto signals = percept->getInternal();
            float reward = signals && !signals->empty() ? (*signals)[0] : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            totalReward += reward;
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Reward=" << reward << ", Total=" << totalReward << std::endl;
            }
        }
        
        return totalReward;
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps") = 1000, py::arg("verbose") = false, 
         "Run a complete reinforcement learning episode");
    
    m.def("runTrainingLoop", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                               int numEpisodes, int stepsPerEpisode, bool verbose = false) {
        std::vector<float> episodeRewards;
        std::vector<float> averageRewards;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            agent->reset();
            world->reset();
            world->setAgentStart(10.0f, 10.0f);
            
            float episodeReward = 0.0f;
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                world->update(0.1f);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                agent->getBrain()->step(step);
                auto action = agent->decodeMotorCommand();
                
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto signals = percept->getInternal();
                float reward = signals && !signals->empty() ? (*signals)[0] : 0.0f;
                agent->applyRewardModulation(reward, 0.0f);
                
                agent->updateDevelopment(0.1f);
                episodeReward += reward;
            }
            
            episodeRewards.push_back(episodeReward);
            
            // Calculate moving average
            float avgReward = 0.0f;
            int window = std::min(10, episode + 1);
            for (int i = std::max(0, episode - window + 1); i <= episode; ++i) {
                avgReward += episodeRewards[i];
            }
            avgReward /= window;
            averageRewards.push_back(avgReward);
            
            if (verbose && (episode % 10 == 0 || episode == numEpisodes - 1)) {
                std::cout << "Episode " << episode << ": Reward=" << episodeReward 
                          << ", Avg=" << avgReward << std::endl;
            }
        }
        
        return std::make_pair(episodeRewards, averageRewards);
    }, py::arg("agent"), py::arg("world"), py::arg("numEpisodes"), py::arg("stepsPerEpisode"), py::arg("verbose") = false,
         "Run a complete training loop returning rewards and averages");
    
    // 3. Built-in experiment templates
    m.def("createExplorationExperiment", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1500);
        config->set("brain.synapse_density", 0.12);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.2);
        
        auto brain = std::make_shared<Brain>(config);
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        return std::make_tuple(agent, brain, world);
    }, "Create an exploration experiment setup");
    
    m.def("createMemoryExperiment", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        
        auto brain = std::make_shared<Brain>(config);
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 6, 6);
        
        return std::make_tuple(agent, brain, world);
    }, "Create a memory consolidation experiment setup");
    
    // 4. Memory management utilities
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to checkpoint file");
    
    m.def("loadBrainCheckpoint", [](const std::string& filepath, std::shared_ptr<Config> config = nullptr) {
        if (config == nullptr) {
            config = std::make_shared<Config>();
        }
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"), py::arg("config") = nullptr, "Load brain state from checkpoint file");
    
    m.def("createMemoryReplay", [](std::shared_ptr<Brain> brain, int replaySteps = 100, float replayRate = 0.5f) {
        // This is a simplified memory replay implementation
        // In a real implementation, this would use the brain's episodic memory system
        for (int i = 0; i < replaySteps; ++i) {
            brain->step(i % 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 * replayRate)));
        }
        return "Memory replay completed";
    }, py::arg("brain"), py::arg("replaySteps") = 100, py::arg("replayRate") = 0.5f, "Replay stored experiences for memory consolidation");
    
    // 5. Visualization helpers
    m.def("collectBrainMetrics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return metrics;
    }, py::arg("brain"), "Collect brain statistics for visualization");
    
    m.def("createVisualizationData", [](std::shared_ptr<Brain> brain, int steps, float timestep = 0.1f) {
        std::vector<std::map<std::string, double>> timeSeries;
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
            std::map<std::string, double> point;
            point["step"] = i;
            point["time"] = i * timestep;
            point["firing_rate"] = brain->getAverageFiringRate();
            point["active_neurons"] = brain->getActiveNeuronCount();
            point["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            timeSeries.push_back(point);
        }
        return timeSeries;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.1f, "Generate time series data for visualization");
    
    // 6. Debugging tools
    m.def("inspectBrainState", [](std::shared_ptr<Brain> brain) {
        brain->logStatus();
        return "Brain state inspection completed";
    }, py::arg("brain"), "Log detailed brain state information for debugging");
    
    m.def("checkBrainHealth", [](std::shared_ptr<Brain> brain, float neuronHealthThreshold = 0.1f,
                               float synapseHealthThreshold = 0.05f) {
        bool healthy = true;
        
        // Check neuron health
        if (brain->getTotalNeuronCount() == 0) {
            std::cout << "WARNING: No neurons in brain" << std::endl;
            healthy = false;
        }
        
        // Check synapse health
        if (brain->getTotalSynapseCount() == 0 && brain->getTotalNeuronCount() > 10) {
            std::cout << "WARNING: No synapses despite having neurons" << std::endl;
            healthy = false;
        }
        
        // Check developmental stage consistency
        auto stage = brain->getDevelopmentalStage();
        if (stage == DevelopmentalStage::Invalid) {
            std::cout << "WARNING: Invalid developmental stage" << std::endl;
            healthy = false;
        }
        
        return healthy;
    }, py::arg("brain"), py::arg("neuronHealthThreshold") = 0.1f, py::arg("synapseHealthThreshold") = 0.05f,
         "Check brain system health and return status");
    
    m.def("debugAgentWorldInteraction", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int steps = 100) {
        agent->reset();
        world->reset();
        
        std::cout << "=== Agent-World Interaction Debug ===" << std::endl;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            
            std::cout << "Step " << step << ": ";
            auto vision = percept->getVision();
            if (vision) {
                std::cout << "Vision size: " << vision->getWidth() * vision->getHeight();
            }
            std::cout << std::endl;
            
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return "Debug session completed";
    }, py::arg("agent"), py::arg("world"), py::arg("steps") = 100,
         "Debug agent-world interaction with detailed logging");
    
    // Advanced APIs for sophisticated neural architectures
    m.def("createMultiRegionBrain", [](const std::vector<std::string>& regionNames) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 3000);
        config->set("brain.synapse_density", 0.12);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (const auto& name : regionNames) {
            brain->addRegion(name);
        }
        
        return brain;
    }, py::arg("regionNames"), "Create a brain with multiple specialized regions");
    
    m.def("enableAdvancedLearning", [](std::shared_ptr<Brain> brain, bool enablePredictiveLearning = true,
                                      bool enableMetaLearning = true) {
        if (enablePredictiveLearning) {
            // Enable prediction system integration
            auto config = brain->getConfig();
            config->set("prediction.enabled", true);
        }
        if (enableMetaLearning) {
            // Enable meta-learning capabilities
            auto config = brain->getConfig();
            config->set("development.metalearning", true);
        }
        return "Advanced learning enabled";
    }, py::arg("brain"), py::arg("enablePredictiveLearning") = true, py::arg("enableMetaLearning") = true,
         "Enable advanced learning mechanisms in the brain");
    
    m.def("createCustomExperiment", [](const std::string& name, std::function<void()> setup,
                                      std::function<void()> run) {
        std::cout << "Setting up custom experiment: " << name << std::endl;
        setup();
        
        std::cout << "Running experiment: " << name << std::endl;
        run();
        
        return "Custom experiment completed: " + name;
    }, py::arg("name"), py::arg("setup"), py::arg("run"), "Create a custom experiment with setup and run functions");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
