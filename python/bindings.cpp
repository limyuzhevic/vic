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

    m.def("runSimulationLoop", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid parameters: brain, world, and agent must be non-null");
        }
        
        if (!agent->isRewardModulationEnabled() || !agent->isCuriosityEnabled()) {
            NLM_LOG_WARNING("Simulation running without neuromodulation enabled. Consider enabling reward modulation and curiosity for better learning.");
        }
        
        for (int step = 0; step < numSteps; ++step) {
            try {
                // 1. Update world
                world->update(0.1);
                
                // 2. Get sensory input from world
                auto percept = world->getSensoryPercept();
                
                // 3. Process sensory input in brain
                agent->processSensoryInput(percept);
                
                // 4. Run brain step
                brain->step(step);
                
                // 5. Get motor command from brain
                auto motor_cmd = agent->decodeMotorCommand();
                
                // 6. Apply motor command to world
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
                
                // 7. Apply reward modulation
                float reward = 0.0f;
                if (auto internal = percept->getInternal()) {
                    reward = internal->empty() ? 0.0f : internal->front();
                }
                agent->applyRewardModulation(reward, 0.0);
                
                // 8. Update development
                agent->updateDevelopment(0.1);
                
                // Log progress every 10 steps
                if (step % 10 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                                std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " +
                                std::to_string(agent->getCuriosityLevel()) + " curiosity");
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error at step " + std::to_string(step) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        NLM_LOG_INFO("Simulation completed successfully after " + std::to_string(numSteps) + " steps");
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        "Run a complete simulation loop with world, brain, and agent interaction");

    m.def("importConfiguration", [](std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        
        if (filepath.empty()) {
            throw std::runtime_error("Filepath cannot be empty");
        }
        
        bool success = config->loadFromFile(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        
        NLM_LOG_INFO("Configuration imported successfully from: " + filepath);
        return success;
    }, py::arg("config"), py::arg("filepath"),
        "Import configuration from a file");

    m.def("exportConfiguration", [](std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        
        if (filepath.empty()) {
            throw std::runtime_error("Filepath cannot be empty");
        }
        
        bool success = config->saveToFile(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save configuration to file: " + filepath);
        }
        
        NLM_LOG_INFO("Configuration exported successfully to: " + filepath);
        return success;
    }, py::arg("config"), py::arg("filepath"),
        "Export configuration to a file");

    m.def("createExperimentConfig", [](std::string experimentType) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Create configurations for different experiment types
        if (experimentType == "novelty_search") {
            // Configuration for novelty-driven exploration experiments
            config->set("brain.neuron_count", 800);
            config->set("brain.synapse_density", 0.15);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.curiosity.novelty_weight", 0.8);
            config->set("neuromod.curiosity.prediction_error_weight", 0.2);
            config->set("development.enable", true);
            config->set("development.initial_plasticity", 1.5);
        } else if (experimentType == "reward_learning") {
            // Configuration for reward-based learning experiments
            config->set("brain.neuron_count", 1200);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("neuromod.dopamine.scale", 2.0);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
            config->set("memory.episodic.max_episodes", 1000);
        } else if (experimentType == "social_learning") {
            // Configuration for social learning experiments
            config->set("brain.neuron_count", 1500);
            config->set("brain.synapse_density", 0.12);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction_error.enable", true);
            config->set("memory.associative.capacity", 500);
        } else if (experimentType == "developmental") {
            // Configuration for developmental studies
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.18);
            config->set("development.enable", true);
            config->set("development.initial_plasticity", 3.0);
            config->set("development.critical_period_length", 500);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.stdp.enable", true);
        } else {
            throw std::invalid_argument("Unknown experiment type: " + experimentType +
                                       ". Supported types: novelty_search, reward_learning, social_learning, developmental");
        }
        
        NLM_LOG_INFO("Created " + experimentType + " configuration");
        return config;
    }, py::arg("experimentType"),
        "Create configuration for specific experiment types");

    m.def("createConfigFromDict", [](std::map<std::string, py::object> configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        for (const auto& pair : configDict) {
            std::string key = pair.first;
            py::object value = pair.second;
            
            // Convert Python types to NLM types
            if (value.is_none()) {
                continue;  // Skip None values
            }
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                auto list = value.cast<std::vector<int>>();
                config->set(key, list);
            } else if (value.is(py::list()) && value.cast<std::vector<float>>().size() > 0) {
                auto list = value.cast<std::vector<float>>();
                config->set(key, list);
            } else {
                NLM_LOG_WARNING("Skipping unsupported config value for key: " + key);
            }
        }
        
        NLM_LOG_INFO("Configuration created from dictionary with " + std::to_string(configDict.size()) + " entries");
        return config;
    }, py::arg("configDict"),
        "Create configuration from a dictionary (Python dict)");

    m.def("runMultiStepExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent, int numEpisodes,
                                         int stepsPerEpisode, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid parameters: brain, world, and agent must be non-null");
        }
        
        if (numEpisodes <= 0 || stepsPerEpisode <= 0) {
            throw std::runtime_error("Number of episodes and steps per episode must be positive");
        }
        
        std::vector<std::pair<int, float>> episodeStats;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            if (verbose) {
                NLM_LOG_INFO("=== Episode " + std::to_string(episode + 1) + " ===");
            }
            
            // Reset for new episode
            world->reset();
            agent->reset();
            brain->reset();
            world->setAgentStart(rand() % world->getWidth(), rand() % world->getHeight());
            
            float episodeTotalReward = 0.0f;
            float episodeMaxCuriosity = 0.0f;
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain
                brain->step(step);
                
                // Get action
                auto motor_cmd = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
                
                // Calculate reward
                float reward = 0.0f;
                if (auto internal = percept->getInternal()) {
                    reward = internal->empty() ? 0.0f : internal->front();
                }
                
                // Apply reward modulation
                agent->applyRewardModulation(reward, 0.0);
                episodeTotalReward += reward;
                
                // Update development
                agent->updateDevelopment(0.1);
                
                // Track max curiosity
                float curiosity = agent->getCuriosityLevel();
                episodeMaxCuriosity = std::max(episodeMaxCuriosity, curiosity);
                
                if (verbose && step % 10 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + ": reward=" + std::to_string(reward) + 
                                ", curiosity=" + std::to_string(curiosity));
                }
            }
            
            // Store episode statistics
            episodeStats.emplace_back(episode + 1, episodeTotalReward);
            
            if (verbose) {
                NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + " completed:");
                NLM_LOG_INFO("  Total reward: " + std::to_string(episodeTotalReward));
                NLM_LOG_INFO("  Max curiosity: " + std::to_string(episodeMaxCuriosity));
                NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
                NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(agent->getDevelopmentalStage())));
            }
        }
        
        // Calculate overall statistics
        float totalReward = 0.0f;
        for (const auto& stats : episodeStats) {
            totalReward += stats.second;
        }
        
        NLM_LOG_INFO("Multi-step experiment completed:");
        NLM_LOG_INFO("  Episodes: " + std::to_string(numEpisodes));
        NLM_LOG_INFO("  Total reward: " + std::to_string(totalReward));
        NLM_LOG_INFO("  Average reward per episode: " + std::to_string(totalReward / numEpisodes));
        
        return episodeStats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("numEpisodes"), py::arg("stepsPerEpisode"), py::arg("verbose") = false,
          "Run a multi-episode experiment and collect statistics");

    m.def("createSimpleDemo", []() {
        NLM_LOG_INFO("=== Simple NLM Demo ===");
        NLM_LOG_INFO("This demo creates a simple brain-world-agent system");
        
        // Create components
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 50);
        config->set("brain.synapse_density", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        world->setAgentStart(3, 3);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable basic features
        agent->enableRewardModulation(true);
        
        NLM_LOG_INFO("Starting simulation for 20 steps...");
        
        for (int step = 0; step < 20; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            float reward = 0.0f;
            if (auto internal = percept->getInternal()) {
                reward = internal->empty() ? 0.0f : internal->front();
            }
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            if (step % 5 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                            std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " +
                            std::to_string(agent->getNeuromodulationLevel()) + " neuromodulation");
            }
        }
        
        NLM_LOG_INFO("Simple demo completed successfully!");
        return std::make_tuple(brain, world, agent);
    }, "Run a simple demo that shows basic brain-world interaction");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        if (filepath.empty()) {
            throw std::runtime_error("Filepath cannot be empty");
        }
        
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to file: " + filepath);
        }
        
        NLM_LOG_INFO("Brain state exported successfully to: " + filepath);
    }, py::arg("brain"), py::arg("filepath"),
        "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        if (filepath.empty()) {
            throw std::runtime_error("Filepath cannot be empty");
        }
        
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from file: " + filepath);
        }
        
        NLM_LOG_INFO("Brain state imported successfully from: " + filepath);
    }, py::arg("brain"), py::arg("filepath"),
        "Import brain state from file");

    m.def("runControlledSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                          std::shared_ptr<AgentBrain> agent, int maxSteps, 
                                          std::function<bool(int, float)> stopCondition) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid parameters: brain, world, and agent must be non-null");
        }
        
        if (maxSteps <= 0) {
            throw std::runtime_error("Max steps must be positive");
        }
        
        for (int step = 0; step < maxSteps; ++step) {
            try {
                // Get current time
                float time = world->getSimulationTime();
                
                // Check stop condition
                if (stopCondition(step, time)) {
                    NLM_LOG_INFO("Stop condition met at step " + std::to_string(step));
                    break;
                }
                
                // Update world
                world->update(0.1);
                
                // Process sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Run brain
                brain->step(step);
                
                // Get action and apply
                auto motor_cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
                
                // Apply neuromodulation
                float reward = 0.0f;
                if (auto internal = percept->getInternal()) {
                    reward = internal->empty() ? 0.0f : internal->front();
                }
                agent->applyRewardModulation(reward, 0.0);
                
                // Update development
                agent->updateDevelopment(0.1);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error at step " + std::to_string(step) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        NLM_LOG_INFO("Controlled simulation completed after " + std::to_string(maxSteps) + " steps");
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("maxSteps"), 
          py::arg("stopCondition") = nullptr,
          "Run a controlled simulation with custom stop condition");

    m.def("analyzeBrainState", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        std::ostringstream analysis;
        analysis << "=== NLM Brain State Analysis ===\n";
        analysis << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        analysis << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        analysis << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        analysis << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
        analysis << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        analysis << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        analysis << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        analysis << "Pending spike events: " << brain->getPendingSpikeEventCount() << "\n";
        analysis << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        NLM_LOG_INFO(analysis.str());
        return analysis.str();
    }, py::arg("brain"),
        "Analyze and log detailed brain state information");

    m.def("benchmarkSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid parameters: brain, world, and agent must be non-null");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Warm up
        for (int i = 0; i < 10; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        
        world->reset();
        agent->reset();
        brain->reset();
        
        // Actual benchmark
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
            
            agent->applyRewardModulation(0.0, 0.0);
            agent->updateDevelopment(0.1);
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(benchmarkEnd - benchmarkStart);
        float stepsPerSecond = (numSteps / (totalTime.count() / 1000.0f));
        
        NLM_LOG_INFO("Benchmark completed: " + std::to_string(numSteps) + " steps in " + 
                    std::to_string(totalTime.count()) + " ms (" + std::to_string(stepsPerSecond) + " steps/sec)");
        
        return std::make_pair(numSteps, stepsPerSecond);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        "Run a benchmark simulation and measure performance");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Development-focused configuration
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15);
        config->set("brain.connection_probability", 0.08);
        config->set("brain.v_thresh", -45.0);  // Higher threshold for slower firing
        config->set("brain.v_rest", -65.0);   // Less hyperpolarized
        config->set("brain.v_reset", -70.0);  // More negative reset
        config->set("brain.tau_mem", 25.0);   // Slower membrane time constant
        config->set("brain.tau_ref", 5.0);    // Longer refractory period
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.002);  // Higher learning rate
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);   // Enable structural changes
        config->set("neuromod.dopamine.scale", 2.0);          // Stronger neuromodulation
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        config->set("development.enable", true);
        config->set("development.initial_plasticity", 2.0);   // High initial plasticity
        config->set("development.plasticity_decay_rate", 0.5); // Slower decay
        
        return config;
    }, "Create a development-focused configuration");

    m.def("createPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Performance-optimized configuration
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.05);  // Sparse connections
        config->set("brain.connection_probability", 0.02);
        config->set("brain.v_thresh", -55.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 15.0);   // Faster membrane time constant
        config->set("brain.tau_ref", 1.0);    // Shorter refractory period
        config->set("plasticity.stdp.enable", false);  // Disable for speed
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);  // Disable for speed
        config->set("neuromod.dopamine.scale", 0.5);   // Lighter neuromodulation
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("development.enable", false);  // Disable development
        
        return config;
    }, "Create a performance-optimized configuration");

    m.def("quickStartDemo", []() {
        NLM_LOG_INFO("=== NLM Quick Start Demo ===");
        NLM_LOG_INFO("Creating brain with default configuration...");
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        NLM_LOG_INFO("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        world->setAgentStart(5.0, 5.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        NLM_LOG_INFO("Starting simulation for 50 steps...");
        
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            float reward = 0.0f;
            if (auto internal = percept->getInternal()) {
                reward = internal->empty() ? 0.0f : internal->front();
            }
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            if (step % 10 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                            std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " +
                            std::to_string(agent->getCuriosityLevel()) + " curiosity");
            }
        }
        
        NLM_LOG_INFO("Demo completed successfully!");
        NLM_LOG_INFO("Statistics:");
        NLM_LOG_INFO("- Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("- Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("- Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("- Final developmental stage: " + std::to_string(static_cast<int>(agent->getDevelopmentalStage())));
        
        return std::make_tuple(brain, world, agent);
    }, "Run a quick start demo showing basic brain-world interaction");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Research-focused configuration with balanced parameters
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.12);
        config->set("brain.connection_probability", 0.06);
        config->set("brain.v_thresh", -52.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 3.0);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.0015);
        config->set("plasticity.stdp.tau_plus", 15.0);
        config->set("plasticity.stdp.tau_minus", 15.0);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.0005);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0001);
        config->set("plasticity.structural.pruning_rate", 0.00001);
        config->set("neuromod.dopamine.scale", 1.5);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.novelty_weight", 0.6);
        config->set("neuromod.curiosity.prediction_error_weight", 0.4);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.decay_rate", 0.1);
        config->set("neuromod.prediction_error.enable", true);
        config->set("neuromod.prediction_error.learning_rate", 0.01);
        config->set("development.enable", true);
        config->set("development.initial_plasticity", 1.0);
        config->set("development.plasticity_decay_rate", 0.8);
        config->set("development.critical_period_length", 200);
        config->set("memory.working.capacity", 200);
        config->set("memory.working.decay_rate", 0.01);
        config->set("memory.episodic.max_episodes", 500);
        config->set("memory.associative.capacity", 100);
        config->set("simulation.timestep", 0.001);
        config->set("simulation.max_steps", 10000);
        
        return config;
    }, "Create a research-oriented configuration with balanced parameters for scientific studies");

    m.def("validateBrainConfiguration", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        // Check neuron count
        if (auto neuronCount = config->get<size_t>("brain.neuron_count")) {
            if (*neuronCount == 0) {
                errors.push_back("Neuron count cannot be zero");
            } else if (*neuronCount > 100000) {
                warnings.push_back("High neuron count (>100,000) may impact performance");
            }
        }
        
        // Check connectivity
        if (auto connProb = config->getOr<float>("brain.connection_probability", 0.1f)) {
            if (*connProb < 0.0f || *connProb > 1.0f) {
                errors.push_back("Connection probability must be between 0.0 and 1.0");
            }
        }
        
        // Check voltage parameters
        if (auto v_thresh = config->getOr<float>("brain.v_thresh", -55.0f)) {
            if (*v_thresh < -100.0f || *v_thresh > -20.0f) {
                warnings.push_back("Threshold potential seems unusual: " + std::to_string(*v_thresh) + " mV");
            }
        }
        
        // Check synaptic parameters
        if (auto stdp_enabled = config->getOr<bool>("plasticity.stdp.enable", true)) {
            if (*stdp_enabled) {
                if (auto stdp_lr = config->getOr<float>("plasticity.stdp.learning_rate", 0.001f)) {
                    if (*stdp_lr <= 0.0f) {
                        warnings.push_back("STDP learning rate should be positive for learning");
                    }
                }
            }
        }
        
        if (!errors.empty()) {
            std::ostringstream errorMsg;
            errorMsg << "Configuration validation failed:\n";
            for (const auto& err : errors) {
                errorMsg << "ERROR: " << err << "\n";
            }
            NLM_LOG_ERROR(errorMsg.str());
            throw std::runtime_error(errorMsg.str());
        }
        
        if (!warnings.empty()) {
            std::ostringstream warningMsg;
            warningMsg << "Configuration validation completed with warnings:\n";
            for (const auto& warn : warnings) {
                warningMsg << "WARNING: " << warn << "\n";
            }
            NLM_LOG_WARNING(warningMsg.str());
        }
        
        NLM_LOG_INFO("Configuration validation passed successfully");
        return warnings.empty();
    }, py::arg("config"),
        "Validate configuration parameters and return true if valid");

    m.def("getExampleConfigurations", []() {
        std::vector<std::pair<std::string, std::shared_ptr<Config>>> examples;
        
        // Quick Start
        auto quickConfig = std::make_shared<Config>();
        quickConfig->set("brain.neuron_count", 100);
        quickConfig->set("brain.synapse_density", 0.1);
        examples.emplace_back("Quick Start", quickConfig);
        
        // Research
        auto researchConfig = std::make_shared<Config>();
        researchConfig->set("brain.neuron_count", 1000);
        researchConfig->set("brain.synapse_density", 0.12);
        researchConfig->set("plasticity.stdp.enable", true);
        researchConfig->set("neuromod.curiosity.enable", true);
        examples.emplace_back("Research", researchConfig);
        
        // Development
        auto devConfig = std::make_shared<Config>();
        devConfig->set("brain.neuron_count", 2000);
        devConfig->set("development.enable", true);
        devConfig->set("development.initial_plasticity", 2.0);
        examples.emplace_back("Development", devConfig);
        
        // Performance
        auto perfConfig = std::make_shared<Config>();
        perfConfig->set("brain.neuron_count", 5000);
        perfConfig->set("plasticity.stdp.enable", false);
        perfConfig->set("neuromod.curiosity.enable", false);
        examples.emplace_back("Performance", perfConfig);
        
        return examples;
    }, "Get a list of example configurations for different use cases");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
