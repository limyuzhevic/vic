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

    // Advanced convenience functions

    // Training loop utilities
    m.def("run_training_loop", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                int max_episodes, int steps_per_episode, bool verbose = false) {
        std::vector<float> episode_rewards;
        for (int episode = 0; episode < max_episodes; ++episode) {
            world->reset();
            float episode_reward = 0.0f;
            for (int step = 0; step < steps_per_episode; ++step) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                // Note: brain.step() should be called separately from AgentBrain
                auto action = agent->decodeMotorCommand();
                if (action) {
                    auto result = world->applyMotorCommand(action, world->getSimulationTime());
                    agent->applyRewardModulation(result.reward, agent->getPredictionError());
                    episode_reward += result.reward;
                }
            }
            episode_rewards.push_back(episode_reward);
            if (verbose) {
                std::cout << "Episode " << episode << ": Reward = " << episode_reward << std::endl;
            }
        }
        return episode_rewards;
    }, py::arg("agent"), py::arg("world"), py::arg("max_episodes"), 
            py::arg("steps_per_episode"), py::arg("verbose") = false,
            "Run complete training loop for agent in world");

    m.def("run_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                           int steps, bool enable_exploration = true) {
        world->reset();
        std::vector<std::pair<float, float>> history; // (reward, curiosity)
        for (int step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Enable exploration if requested
            if (enable_exploration) {
                float curiosity = agent->getCuriosityLevel();
                if (curiosity > 0.7f) { // High curiosity = explore more
                    // Simple exploration behavior
                }
            }
            
            auto action = agent->decodeMotorCommand();
            if (action) {
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(result.reward, agent->getPredictionError());
                agent->updateDevelopment(0.1f);
                history.emplace_back(result.reward, agent->getCuriosityLevel());
            }
        }
        return history;
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), 
            py::arg("enable_exploration") = true,
            "Run single training episode");

    // Monitoring and statistics utilities
    m.def("monitor_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                                   std::shared_ptr<SimpleWorld> world, int duration) {
        std::map<std::string, std::vector<float>> metrics;
        for (int step = 0; step < duration; ++step) {
            brain->step(step);
            if (agent) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
            }
            
            // Collect metrics
            metrics["firing_rate"].push_back(brain->getAverageFiringRate());
            metrics["total_spikes"].push_back(brain->getTotalSpikeCount());
            if (agent) {
                metrics["curiosity"].push_back(agent->getCuriosityLevel());
                metrics["novelty"].push_back(agent->getNoveltyLevel());
                metrics["prediction_error"].push_back(agent->getPredictionError());
            }
        }
        return metrics;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("duration"),
            "Monitor simulation and collect metrics over time");

    m.def("get_detailed_stats", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world) {
        py::dict stats;
        
        // Brain statistics
        stats["brain"] = py::dict(
            "total_neurons", brain->getTotalNeuronCount(),
            "total_synapses", brain->getTotalSynapseCount(),
            "active_neurons", brain->getActiveNeuronCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount(),
            "average_firing_rate", brain->getAverageFiringRate(),
            "excitation_inhibition_ratio", brain->getExcitationInhibitionRatio(),
            "developmental_stage", brain->getDevelopmentalStage().value,
            "plasticity_level", brain->getPlasticityLevel(),
            "excitability_level", brain->getExcitabilityLevel(),
            "learning_rate", brain->getLearningRate()
        );
        
        // Agent statistics
        if (agent) {
            stats["agent"] = py::dict(
                "curiosity_level", agent->getCuriosityLevel(),
                "novelty_level", agent->getNoveltyLevel(),
                "neuromodulation_level", agent->getNeuromodulationLevel(),
                "prediction_error", agent->getPredictionError(),
                "expected_reward", agent->getExpectedReward(),
                "developmental_age", agent->getDevelopmentalAge(),
                "structural_plasticity_enabled", agent->isStructuralPlasticityEnabled(),
                "reward_modulation_enabled", agent->isRewardModulationEnabled(),
                "curiosity_enabled", agent->isCuriosityEnabled(),
                "development_enabled", agent->isDevelopmentEnabled()
            );
        }
        
        // World statistics
        if (world) {
            stats["world"] = py::dict(
                "width", world->getWidth(),
                "height", world->getHeight(),
                "simulation_time", world->getSimulationTime(),
                "agent_x", world->getAgentBody().x,
                "agent_y", world->getAgentBody().y,
                "agent_orientation", world->getAgentBody().orientation
            );
        }
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
            "Get detailed statistics about simulation state");

    // Validation and convenience functions
    m.def("validate_configuration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        if (!config->get<int64_t>("neuron_count").has_value() || 
            config->get<int64_t>("neuron_count").value() <= 0) {
            errors.push_back("neuron_count must be a positive integer");
        }
        if (!config->get<int64_t>("region_count").has_value() || 
            config->get<int64_t>("region_count").value() <= 0) {
            errors.push_back("region_count must be a positive integer");
        }
        if (config->get<double>("connection_probability").value_or(0.0) < 0.0 || 
            config->get<double>("connection_probability").value_or(0.0) > 1.0) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
        if (errors.empty()) {
            return std::make_pair(true, std::string("Configuration is valid"));
        } else {
            std::string error_msg = "Configuration errors:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            return std::make_pair(false, error_msg);
        }
    }, py::arg("config"), "Validate configuration and return (is_valid, message)");

    m.def("create_preset_config", [](const std::string& preset_name) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (preset_name == "minimal") {
            config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
        } else if (preset_name == "standard") {
            config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        } else if (preset_name == "advanced") {
            config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
            config->set("enable_working_memory", true, nlm::ConfigSource::Default);
            config->set("enable_episodic_memory", true, nlm::ConfigSource::Default);
            config->set("enable_curiosity", true, nlm::ConfigSource::Default);
            config->set("enable_development", true, nlm::ConfigSource::Default);
        } else {
            throw std::runtime_error("Unknown preset: " + preset_name);
        }
        return config;
    }, py::arg("preset_name"), "Create configuration with preset parameters");

    m.def("export_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "NLM Simulation Export\n";
        file << "====================\n\n";
        file << "Brain Statistics:\n";
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  Developmental Stage: " << brain->getDevelopmentalStage().value << "\n\n";
        
        if (agent) {
            file << "Agent Statistics:\n";
            file << "  Curiosity Level: " << agent->getCuriosityLevel() << "\n";
            file << "  Novelty Level: " << agent->getNoveltyLevel() << "\n";
            file << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n\n";
        }
        
        if (world) {
            file << "World Statistics:\n";
            file << "  Width: " << world->getWidth() << "\n";
            file << "  Height: " << world->getHeight() << "\n";
            file << "  Simulation Time: " << world->getSimulationTime() << "\n";
            file << "  Agent Position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")\n";
        }
        
        file << "\nExport completed successfully!";
        file.close();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"),
            "Export simulation statistics to file");

    // Enhanced convenience functions
    m.def("create_brain_with_features", [](const std::string& preset_name, bool enable_features) {
        auto config = pynlm::create_preset_config(preset_name);
        auto brain = pynlm::createBrain(config);
        brain->initialize();
        
        if (enable_features) {
            // These would be additional setup calls that make sense
            // For now, just return the basic brain
        }
        
        return brain;
    }, py::arg("preset_name"), py::arg("enable_features") = true,
            "Create brain with preset configuration and optional features");

    m.def("create_agent_with_monitoring", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                            bool enable_monitoring = true) {
        auto agent = pynlm::createAgentBrain(brain);
        agent->initialize(world);
        
        if (enable_monitoring) {
            // Set up monitoring callbacks
            agent->setMonitoringCallback([](const std::string& event, float value) {
                // Simple callback implementation
            });
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enable_monitoring") = true,
            "Create agent with optional monitoring");

    m.def("create_world_with_obstacles", [](int width, int height, int num_obstacles) {
        auto world = pynlm::createSimpleWorld();
        world->configure(width, height, 8, 8);
        
        // Add random obstacles
        std::mt19937 rng(42); // Fixed seed for reproducibility
        std::uniform_int_distribution<int> x_dist(1, width-2);
        std::uniform_int_distribution<int> y_dist(1, height-2);
        
        for (int i = 0; i < num_obstacles; ++i) {
            float x = static_cast<float>(x_dist(rng));
            float y = static_cast<float>(y_dist(rng));
            world->addObject(nlm::WorldObject(x, y, nlm::WorldObjectType::Obstacle, 1.0f, 0.3f));
        }
        
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("num_obstacles") = 10,
            "Create world with obstacles for navigation");

    // Improved documentation strings
    // Visualization and monitoring tools
    m.class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize,
             "Initialize visualization system")
        .def("update", &VisualizationInterface::update,
             "Update visualization data")
        .def("render", &VisualizationInterface::render,
             "Render current visualization")
        .def("close", &VisualizationInterface::close,
             "Close visualization system")
        .def("isActive", &VisualizationInterface::isActive,
             "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate,
             py::arg("hz"), "Set visualization update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate,
             "Get current visualization update rate (Hz)")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork,
             py::arg("brain"), "Visualize brain network structure")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity,
             py::arg("brain"), "Visualize current neural activity")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes,
             py::arg("spikes"), "Create spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights,
             py::arg("brain"), "Visualize synaptic weight matrix");

    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create visualization interface");

    // Enhanced monitoring and data collection
    m.def("collect_time_series", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world, int duration, 
                                   const std::string& data_type = "comprehensive") {
        py::dict time_series;
        
        if (data_type == "comprehensive" || data_type == "all") {
            // Neuron activity
            std::vector<float> firing_rates;
            std::vector<float> spike_counts;
            std::vector<float> excitation_inhibition;
            
            for (int step = 0; step < duration; ++step) {
                brain->step(step);
                firing_rates.push_back(brain->getAverageFiringRate());
                spike_counts.push_back(static_cast<float>(brain->getTotalSpikeCount()));
                excitation_inhibition.push_back(brain->getExcitationInhibitionRatio());
                
                if (agent) {
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                }
            }
            
            time_series["firing_rate"] = firing_rates;
            time_series["total_spikes"] = spike_counts;
            time_series["excitation_inhibition_ratio"] = excitation_inhibition;
            
            if (agent) {
                std::vector<float> curiosity_levels;
                std::vector<float> novelty_levels;
                std::vector<float> prediction_errors;
                
                for (int step = 0; step < duration; ++step) {
                    if (agent) {
                        curiosity_levels.push_back(agent->getCuriosityLevel());
                        novelty_levels.push_back(agent->getNoveltyLevel());
                        prediction_errors.push_back(agent->getPredictionError());
                    }
                }
                
                time_series["curiosity_level"] = curiosity_levels;
                time_series["novelty_level"] = novelty_levels;
                time_series["prediction_error"] = prediction_errors;
            }
            
            if (world) {
                std::vector<float> agent_positions_x;
                std::vector<float> agent_positions_y;
                std::vector<float> simulation_times;
                
                for (int step = 0; step < duration; ++step) {
                    if (world) {
                        agent_positions_x.push_back(world->getAgentBody().x);
                        agent_positions_y.push_back(world->getAgentBody().y);
                        simulation_times.push_back(world->getSimulationTime());
                    }
                }
                
                time_series["agent_position_x"] = agent_positions_x;
                time_series["agent_position_y"] = agent_positions_y;
                time_series["simulation_time"] = simulation_times;
            }
        }
        
        return time_series;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("duration"),
            py::arg("data_type") = "comprehensive",
            "Collect time series data during simulation");

    m.def("analyze_activity_patterns", [](const py::dict& time_series, const std::string& pattern_type) {
        py::dict analysis;
        
        if (time_series.contains("firing_rate")) {
            auto& firing_rates = time_series["firing_rate"].cast<std::vector<float>>();
            
            if (pattern_type == "stationary" || pattern_type == "all") {
                float mean_rate = 0.0f, std_rate = 0.0f;
                for (float rate : firing_rates) {
                    mean_rate += rate;
                }
                mean_rate /= firing_rates.size();
                
                for (float rate : firing_rates) {
                    std_rate += (rate - mean_rate) * (rate - mean_rate);
                }
                std_rate = std::sqrt(std_rate / firing_rates.size());
                
                analysis["stationary_mean"] = mean_rate;
                analysis["stationary_std"] = std_rate;
            }
            
            if (pattern_type == "transients" || pattern_type == "all") {
                std::vector<float> transients;
                for (size_t i = 1; i < firing_rates.size(); ++i) {
                    float change = std::abs(firing_rates[i] - firing_rates[i-1]);
                    if (change > 0.1f) { // Significant change threshold
                        transients.push_back(change);
                    }
                }
                analysis["transient_count"] = static_cast<int>(transients.size());
                if (!transients.empty()) {
                    float max_transient = 0.0f;
                    for (float t : transients) {
                        max_transient = std::max(max_transient, t);
                    }
                    analysis["max_transient"] = max_transient;
                }
            }
        }
        
        return analysis;
    }, py::arg("time_series"), py::arg("pattern_type") = "stationary",
            "Analyze activity patterns in time series data");

    m.def("create_time_series_plot", [](const py::dict& time_series, const std::string& title = "Simulation Data",
                                       const std::string& output_file = "") {
        // Python-like plotting using matplotlib if available, otherwise basic text output
        try {
            if (!time_series.empty()) {
                std::cout << "Time Series Plot: " << title << std::endl;
                std::cout << "===============================" << std::endl;
                
                for (auto it = time_series.begin(); it != time_series.end(); ++it) {
                    std::cout << it->first << ": ";
                    auto& data = it->second.cast<std::vector<float>>();
                    std::cout << "[";
                    for (size_t i = 0; i < std::min(size_t(10), data.size()); ++i) {
                        std::cout << data[i];
                        if (i < std::min(size_t(10), data.size()) - 1) std::cout << ", ";
                    }
                    if (data.size() > 10) std::cout << "..."; // Truncate long data
                    std::cout << "] (" << data.size() << " points)" << std::endl;
                }
                
                if (!output_file.empty()) {
                    std::cout << "Plot data would be saved to: " << output_file << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Warning: Could not create plot: " << e.what() << std::endl;
        }
        
        return true;
    }, py::arg("time_series"), py::arg("title") = "Simulation Data",
            py::arg("output_file") = "",
            "Create visualization of time series data");

    // Advanced error handling and user feedback
    m.def("safe_simulation_step", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                    std::shared_ptr<SimpleWorld> world, int step_num, 
                                    float timestep = 0.1f, bool verbose = false) {
        py::dict result;
        result["success"] = true;
        result["errors"] = py::list();
        result["warnings"] = py::list();
        
        try {
            // Validate inputs
            if (!brain) {
                result["errors"].cast<py::list>().append("Brain is null");
                result["success"] = false;
                return result;
            }
            
            if (step_num < 0) {
                result["warnings"].cast<py::list>().append("Step number is negative, using absolute value");
            }
            
            // Update world
            if (world) {
                world->update(timestep);
                if (verbose) {
                    std::cout << "World updated successfully" << std::endl;
                }
            }
            
            // Process sensory input
            if (agent && world) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                if (verbose) {
                    std::cout << "Sensory input processed" << std::endl;
                }
            }
            
            // Brain step
            brain->step(step_num);
            if (verbose) {
                std::cout << "Brain step completed: " << brain->getFiringNeuronCount() 
                          << " neurons firing" << std::endl;
            }
            
            // Motor action
            if (agent) {
                auto action = agent->decodeMotorCommand();
                if (action) {
                    if (world) {
                        auto action_result = world->applyMotorCommand(action, world->getSimulationTime());
                        agent->applyRewardModulation(action_result.reward, agent->getPredictionError());
                        agent->updateDevelopment(timestep);
                    }
                    if (verbose) {
                        std::cout << "Action decoded and applied" << std::endl;
                    }
                } else {
                    result["warnings"].cast<py::list>().append("No action was decoded");
                }
            }
            
        } catch (const std::exception& e) {
            result["success"] = false;
            result["errors"].cast<py::list>().append(std::string("Exception: ") + e.what());
        }
        
        return result;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("step_num"),
            py::arg("timestep") = 0.1f, py::arg("verbose") = false,
            "Perform simulation step with error handling and feedback");

    m.def("validate_simulation_setup", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                         std::shared_ptr<SimpleWorld> world) {
        py::dict validation_results;
        validation_results["valid"] = true;
        validation_results["warnings"] = py::list();
        validation_results["errors"] = py::list();
        
        // Check brain
        if (!brain) {
            validation_results["errors"].cast<py::list>().append("Brain is not initialized");
            validation_results["valid"] = false;
        } else {
            // Check brain configuration
            auto config = brain->getConfig();
            if (!config->get<int64_t>("neuron_count").has_value() || 
                config->get<int64_t>("neuron_count").value() <= 0) {
                validation_results["errors"].cast<py::list>().append("Brain has invalid neuron_count");
                validation_results["valid"] = false;
            }
            
            if (!config->get<int64_t>("region_count").has_value() || 
                config->get<int64_t>"region_count").value() <= 0) {
                validation_results["warnings"].cast<py::list>().append("Brain has minimal region count");
            }
        }
        
        // Check agent
        if (agent) {
            if (!agent->getBrain()) {
                validation_results["warnings"].cast<py::list>().append("Agent has no associated brain");
            }
        }
        
        // Check world
        if (world) {
            if (world->getWidth() <= 0 || world->getHeight() <= 0) {
                validation_results["errors"].cast<py::list>().append("World has invalid dimensions");
                validation_results["valid"] = false;
            }
            
            auto agent_body = world->getAgentBody();
            if (agent_body.x < 0 || agent_body.x >= world->getWidth() ||
                agent_body.y < 0 || agent_body.y >= world->getHeight()) {
                validation_results["warnings"].cast<py::list>().append("Agent is outside world bounds");
            }
        }
        
        return validation_results;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
            "Validate simulation setup and return validation results");

    // Utility and helper functions
    m.def("run_safe_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world, int duration,
                                   bool verbose = false, float timestep = 0.1f) {
        py::dict simulation_stats;
        simulation_stats["steps_completed"] = 0;
        simulation_stats["successful_steps"] = 0;
        simulation_stats["errors_encountered"] = 0;
        simulation_stats["warnings_encountered"] = 0;
        simulation_stats["final_stats"] = py::dict();
        
        brain->initialize();
        
        if (agent && world) {
            agent->initialize(world);
        }
        
        for (int step = 0; step < duration; ++step) {
            auto step_result = pynlm::safe_simulation_step(brain, agent, world, step, timestep, verbose);
            
            simulation_stats["steps_completed"] = step + 1;
            if (step_result["success"].cast<bool>()) {
                simulation_stats["successful_steps"] = simulation_stats["successful_steps"].cast<int>() + 1;
            } else {
                simulation_stats["errors_encountered"] = simulation_stats["errors_encountered"].cast<int>() + 1;
            }
            
            simulation_stats["warnings_encountered"] = simulation_stats["warnings_encountered"].cast<int>() + 
                step_result["warnings"].cast<py::list>().size();
            
            // Collect final stats at the end
            if (step == duration - 1) {
                simulation_stats["final_stats"] = pynlm::get_detailed_stats(brain, agent, world);
            }
        }
        
        return simulation_stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("duration"),
            py::arg("verbose") = false, py::arg("timestep") = 0.1f,
            "Run simulation with comprehensive error handling and statistics");

    m.def("export_to_csv", [](const py::dict& data, const std::string& filename,
                             const std::string& delimiter = ",") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Export time series data to CSV
        for (auto it = data.begin(); it != data.end(); ++it) {
            file << it->first; // Key
            auto& time_series = it->second.cast<std::vector<float>>();
            for (size_t i = 0; i < time_series.size(); ++i) {
                file << delimiter << time_series[i];
            }
            file << "\n";
        }
        
        file.close();
        return true;
    }, py::arg("data"), py::arg("filename"), py::arg("delimiter") = ",",
            "Export data to CSV file");

    // Advanced visualization presets
    m.def("create_preset_visualization", [](const std::string& preset_name) {
        auto viz = pynlm::createVisualization();
        
        if (preset_name == "network") {
            // Configure for network visualization
            viz->setUpdateRate(5.0); // Slower update for network view
            // Would set up specific visualization modes here
        } else if (preset_name == "activity") {
            // Configure for activity visualization
            viz->setUpdateRate(30.0); // Faster for activity
        } else if (preset_name == "spikes") {
            // Configure for spike visualization
            viz->setUpdateRate(10.0);
        }
        
        return viz;
    }, py::arg("preset_name"), "Create visualization with preset configuration");

    // Monitoring and data aggregation
    m.def("create_data_aggregator", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                       const std::vector<std::shared_ptr<AgentBrain>>& agents,
                                       const std::vector<std::shared_ptr<SimpleWorld>>& worlds) {
        struct DataAggregator {
            std::vector<std::shared_ptr<Brain>> brains;
            std::vector<std::shared_ptr<AgentBrain>> agents;
            std::vector<std::shared_ptr<SimpleWorld>> worlds;
            
            py::dict aggregate_stats() {
                py::dict stats;
                stats["brain_count"] = brains.size();
                stats["agent_count"] = agents.size();
                stats["world_count"] = worlds.size();
                
                // Aggregate brain stats
                if (!brains.empty()) {
                    py::dict brain_stats;
                    for (const auto& brain : brains) {
                        // Aggregate brain statistics
                    }
                    stats["brains"] = brain_stats;
                }
                
                return stats;
            }
        };
        
        return std::make_shared<DataAggregator>(DataAggregator{brains, agents, worlds});
    }, py::arg("brains"), py::arg("agents"), py::arg("worlds"),
            "Create data aggregator for multiple simulation components");

    // Enhanced configuration management
    m.def("create_config_from_template", [](const std::string& template_name, 
                                           const py::dict& customizations) {
        auto config = pynlm::create_preset_config(template_name);
        
        // Apply customizations
        for (auto item : customizations) {
            std::string key = item.first.cast<std::string>();
            
            // Try to apply customization based on key type
            if (key == "neuron_count") {
                int64_t value = customizations[key].cast<int64_t>();
                config->set("neuron_count", value, nlm::ConfigSource::User);
            } else if (key == "region_count") {
                int64_t value = customizations[key].cast<int64_t>();
                config->set("region_count", value, nlm::ConfigSource::User);
            } else if (key == "connection_probability") {
                double value = customizations[key].cast<double>();
                config->set("connection_probability", value, nlm::ConfigSource::User);
            } else if (key == "enable_working_memory") {
                bool value = customizations[key].cast<bool>();
                config->set("enable_working_memory", value, nlm::ConfigSource::User);
            } else if (key == "enable_curiosity") {
                bool value = customizations[key].cast<bool>();
                config->set("enable_curiosity", value, nlm::ConfigSource::User);
            }
        }
        
        return config;
    }, py::arg("template_name"), py::arg("customizations"),
            "Create configuration from template with customizations");

    // Session and experiment management
    m.def("save_experiment_session", [](const std::string& session_name,
                                        std::shared_ptr<Brain> brain,
                                        std::shared_ptr<AgentBrain> agent,
                                        std::shared_ptr<SimpleWorld> world,
                                        const py::dict& metadata) {
        std::string filename = session_name + "_session.json";
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create session file: " + filename);
        }
        
        // Create session JSON
        py::dict session;
        session["name"] = session_name;
        session["timestamp"] = std::chrono::system_clock::now();
        session["metadata"] = metadata;
        session["brain_stats"] = pynlm::get_detailed_stats(brain, agent, world);
        
        // Would serialize to JSON here
        file << "Session data would be serialized here" << std::endl;
        
        file.close();
        return filename;
    }, py::arg("session_name"), py::arg("brain"), py::arg("agent"), py::arg("world"),
            py::arg("metadata"), "Save experiment session with metadata");

    m.def("load_experiment_session", [](const std::string& session_name) {
        std::string filename = session_name + "_session.json";
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot load session file: " + filename);
        }
        
        // Would deserialize session here
        file << "Session data would be deserialized here" << std::endl;
        
        file.close();
        return true;
    }, py::arg("session_name"), "Load experiment session");

    // Progress tracking and callbacks
    m.def("set_simulation_progress_callback", [](std::function<void(int, int)> callback) {
        // Would set up progress callback system here
        return callback;
    }, py::arg("callback"), "Set progress callback for simulation monitoring");

    m.def("create_progress_tracker", [](int total_steps, bool verbose = false) {
        struct ProgressTracker {
            int total_steps;
            bool verbose;
            int completed_steps;
            std::chrono::steady_clock::time_point start_time;
            
            ProgressTracker(int total, bool verbose = false) 
                : total_steps(total), verbose(verbose), completed_steps(0),
                  start_time(std::chrono::steady_clock::now()) {}
            
            void update(int step) {
                completed_steps = step;
                if (verbose && step % 100 == 0) {
                    auto now = std::chrono::steady_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
                    std::cout << "Progress: " << step << "/" << total_steps 
                              << " steps (" << duration.count() << " seconds)" << std::endl;
                }
            }
            
            double get_progress_percentage() const {
                return static_cast<double>(completed_steps) / total_steps * 100.0;
            }
        };
        
        return std::make_shared<ProgressTracker>(total_steps, verbose);
    }, py::arg("total_steps"), py::arg("verbose") = false,
            "Create progress tracker for long simulations");

    // Learning rate schedulers
    m.def("create_learning_rate_scheduler", [](const std::string& scheduler_type, 
                                              double initial_rate, double final_rate,
                                              int total_steps) {
        struct LearningRateScheduler {
            std::string type;
            double initial_rate;
            double final_rate;
            int total_steps;
            int current_step;
            
            LearningRateScheduler(std::string t, double init, double final, int total)
                : type(std::move(t)), initial_rate(init), final_rate(final), 
                  total_steps(total), current_step(0) {}
            
            double get_rate(int step) {
                current_step = step;
                if (type == "linear") {
                    return initial_rate + (final_rate - initial_rate) * 
                           (static_cast<double>(step) / total_steps);
                } else if (type == "exponential") {
                    double decay = std::pow(final_rate / initial_rate, 
                                           static_cast<double>(step) / total_steps);
                    return initial_rate * decay;
                } else if (type == "cosine") {
                    double progress = static_cast<double>(step) / total_steps;
                    return final_rate + 0.5 * (initial_rate - final_rate) * 
                           (1.0 + std::cos(M_PI * progress));
                } else { // constant
                    return initial_rate;
                }
            }
        };
        
        return std::make_shared<LearningRateScheduler>(scheduler_type, initial_rate, final_rate, total_steps);
    }, py::arg("scheduler_type"), py::arg("initial_rate"), py::arg("final_rate"),
            py::arg("total_steps"), "Create learning rate scheduler");

    // Error recovery and checkpoint management
    m.def("create_checkpoint_manager", [](const std::string& checkpoint_dir,
                                         int save_interval, int max_checkpoints) {
        struct CheckpointManager {
            std::string directory;
            int save_interval;
            int max_checkpoints;
            int next_checkpoint_id;
            
            CheckpointManager(std::string dir, int interval, int max)
                : directory(std::move(dir)), save_interval(interval), 
                  max_checkpoints(max), next_checkpoint_id(0) {}
            
            bool should_save(int step) {
                if (step % save_interval == 0) {
                    save_checkpoint(step);
                    return true;
                }
                return false;
            }
            
            void save_checkpoint(int step) {
                // Would implement checkpoint saving here
                next_checkpoint_id++;
                if (next_checkpoint_id > max_checkpoints) {
                    // Remove oldest checkpoint
                }
            }
        };
        
        return std::make_shared<CheckpointManager>(checkpoint_dir, save_interval, max_checkpoints);
    }, py::arg("checkpoint_dir"), py::arg("save_interval"), py::arg("max_checkpoints"),
            "Create checkpoint manager for simulation recovery");

    // Final documentation updates
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration. This function creates a configuration with sensible default values for most parameters. Use this as a starting point and modify the configuration as needed for your specific simulation requirements.");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with the specified configuration. The brain contains neural populations, synapses, and all the computational machinery for simulation.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world environment. The world provides the spatial context for the agent's sensory input and action execution.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. This creates the connection between the neural brain and the external world, handling sensory input and motor output.");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
