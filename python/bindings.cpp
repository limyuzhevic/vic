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

    // === ADVANCED BRAIN OPERATIONS ===
    
    // Batch processing for advanced users
    m.def("batchProcessSensoryInput", [](std::shared_ptr<AgentBrain> agent, const std::vector<SensoryPercept>& percepts) {
        for (const auto& percept : percepts) {
            agent->processSensoryInput(percept);
        }
    }, py::arg("agent"), py::arg("percepts"),
       "Process multiple sensory inputs in batch (advanced use)");
    
    m.def("batchDecodeMotorCommands", [](std::shared_ptr<AgentBrain> agent, size_t count) {
        std::vector<MotorCommand> commands;
        for (size_t i = 0; i < count; ++i) {
            commands.push_back(agent->decodeMotorCommand());
        }
        return commands;
    }, py::arg("agent"), py::arg("count"),
       "Decode multiple motor commands in batch (advanced use)");
    
    // Simulation helpers
    m.def("runSimulationSteps", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                   std::shared_ptr<AgentBrain> agent, size_t steps,
                                   double timestep = 0.1, bool verbose = false) {
        float totalReward = 0.0f;
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(percept.getInternal()[0] if !percept.getInternal().empty() else 0.0f, 0.0f);
            
            totalReward += (percept.getInternal()[0] if !percept.getInternal().empty() else 0.0f);
            
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " 
                          << "Firing: " << brain->getFiringNeuronCount() << " neurons, "
                          << "Reward: " << totalReward / (i + 1) << std::endl;
            }
        }
        return totalReward;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       py::arg("timestep") = 0.1, py::arg("verbose") = false,
       "Run a complete simulation for multiple steps (advanced use)");
    
    // Checkpoint utilities
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->save(filepath)) {
            return true;
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Failed to save brain checkpoint");
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file with error handling");
    
    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->load(filepath)) {
            return true;
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Failed to load brain checkpoint");
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file with error handling");
    
    // Configuration utilities
    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to load configuration from file");
            return nullptr;
        }
        return config;
    }, py::arg("filepath"), "Create configuration from file with error handling");
    
    // World utilities
    m.def("createWorldWithObjects", [](size_t numResources = 5, size_t numHazards = 3) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        world->reset();
        return world;
    }, py::arg("numResources") = 5, py::arg("numHazards") = 3,
       "Create a world with default objects (advanced use)");
    
    // Statistics and analysis helpers
    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        return stats;
    }, py::arg("brain"),
       "Get comprehensive brain statistics (advanced analysis)");
    
// Simulation state utilities
    m.def("resetSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        brain->reset();
        world->reset();
        agent->reset();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Reset all simulation components (advanced use)");

    // === ENHANCED CONFIGURATION UTILITIES ===

    // Enhanced configuration utilities with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->enableValidation();
        return config;
    }, "Create a default configuration with validation enabled");

    m.def("createConfigFromFile", [](const std::string& filepath, bool validate = true) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath, validate)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to load configuration from file");
            return nullptr;
        }
        return config;
    }, py::arg("filepath"), py::arg("validate") = true,
       "Create configuration from file with error handling");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        if (!config->validate()) {
            auto errors = config->getValidationErrors();
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  " + error.key + ": " + error.errorMessage + " (" + 
                           std::to_string(static_cast<int>(error.source)) + ")\n";
            }
            PyErr_SetString(PyExc_RuntimeError, errorMsg.c_str());
            return false;
        }
        return true;
    }, py::arg("config"), "Validate configuration with detailed error messages");

    m.def("getConfigSummary", [](std::shared_ptr<Config> config) {
        std::string summary = config->summary();
        // Add validation status
        if (config->isValid()) {
            summary += "\n✓ Configuration is valid";
        } else {
            summary += "\n✗ Configuration has validation errors";
            auto errors = config->getValidationErrors();
            for (const auto& error : errors) {
                summary += "\n  - " + error.key + ": " + error.errorMessage;
            }
        }
        return summary;
    }, py::arg("config"), "Get configuration summary with validation status");

    // Enhanced brain creation with validation
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config->validate()) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid configuration for brain creation");
            return nullptr;
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain with validated configuration");

    // Simulation utilities with error checking
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent, size_t steps,
                               double timestep = 0.1, bool verbose = false) {
        if (!brain || !world || !agent) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid simulation components");
            return 0.0f;
        }
        
        float totalReward = 0.0f;
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(percept.getInternal()[0] if !percept.getInternal().empty() else 0.0f, 0.0f);
            
            totalReward += (percept.getInternal()[0] if !percept.getInternal().empty() else 0.0f);
            
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " 
                          << "Firing: " << brain->getFiringNeuronCount() << " neurons, "
                          << "Reward: " << totalReward / (i + 1) << std::endl;
            }
        }
        return totalReward;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       py::arg("timestep") = 0.1, py::arg("verbose") = false,
       "Run a complete simulation with error checking");

    // Enhanced checkpoint utilities with validation
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid brain for checkpoint saving");
            return false;
        }
        if (brain->save(filepath)) {
            return true;
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Failed to save brain checkpoint");
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file with error handling");

    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid brain for checkpoint loading");
            return false;
        }
        if (brain->load(filepath)) {
            return true;
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Failed to load brain checkpoint");
            return false;
        }
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file with error handling");

    // Batch processing utilities
    m.def("batchProcessSensoryInput", [](std::shared_ptr<AgentBrain> agent, const std::vector<SensoryPercept>& percepts) {
        if (!agent) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid agent for batch processing");
            return;
        }
        for (const auto& percept : percepts) {
            agent->processSensoryInput(percept);
        }
    }, py::arg("agent"), py::arg("percepts"),
       "Process multiple sensory inputs in batch");

    m.def("batchDecodeMotorCommands", [](std::shared_ptr<AgentBrain> agent, size_t count) {
        if (!agent) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid agent for batch processing");
            return std::vector<MotorCommand>();
        }
        std::vector<MotorCommand> commands;
        for (size_t i = 0; i < count; ++i) {
            commands.push_back(agent->decodeMotorCommand());
        }
        return commands;
    }, py::arg("agent"), py::arg("count"),
       "Decode multiple motor commands in batch");

    // Configuration value utilities
    m.def("setConfigValue", [](std::shared_ptr<Config> config, const std::string& key, const std::string& value) {
        if (!config) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid configuration for setting value");
            return;
        }
        config->set(key, value);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
       "Set configuration value with error checking");

    m.def("getConfigValue", [](std::shared_ptr<Config> config, const std::string& key) {
        if (!config) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid configuration for getting value");
            return py::none();
        }
        auto val = config->getOr<std::string>(key, "");
        return val;
    }, py::arg("config"), py::arg("key"),
       "Get configuration value with error checking");

    // Simulation state utilities
    m.def("resetSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid simulation components");
            return;
        }
        brain->reset();
        world->reset();
        agent->reset();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Reset all simulation components");

    // Statistics and analysis helpers
    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid brain for statistics");
            return py::dict();
        }
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        return stats;
    }, py::arg("brain"),
       "Get comprehensive brain statistics");
}

} // namespace nlm

