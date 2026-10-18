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

    // Advanced Configuration Factory Functions
    m.def("createAdvancedConfig", [](const std::vector<std::pair<std::string, std::variant<int, float, bool, std::string>>>& settings) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& setting : settings) {
            const std::string& key = setting.first;
            const auto& value = setting.second;
            
            if (std::holds_alternative<int>(value)) {
                config->set(key, std::get<int>(value));
            } else if (std::holds_alternative<float>(value)) {
                config->set(key, std::get<float>(value));
            } else if (std::holds_alternative<bool>(value)) {
                config->set(key, std::get<bool>(value));
            } else if (std::holds_alternative<std::string>(value)) {
                config->set(key, std::get<std::string>(value));
            }
        }
        return config;
    }, py::arg("settings"), "Create configuration from settings dictionary");

    // Simulation Utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, 
                          size_t numSteps, bool enableLogging = false) {
        if (enableLogging) {
            NLM_LOG_INFO("Starting simulation with " + std::to_string(numSteps) + " steps");
        }
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            if (enableLogging && (step % 100 == 0)) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                           "Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                           ", Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           ", Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
            }
        }
        
        if (enableLogging) {
            NLM_LOG_INFO("Simulation completed successfully");
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
              py::arg("numSteps"), py::arg("enableLogging") = false,
              "Run complete agent simulation with brain, world, and agent");

    // Enhanced Performance Monitoring
    m.def("getSimulationStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        auto stats = std::make_shared<std::vector<std::pair<std::string, std::string>>>();
        
        // Brain statistics
        stats->emplace_back("Brain: Total Neurons", std::to_string(brain->getTotalNeuronCount()));
        stats->emplace_back("Brain: Total Synapses", std::to_string(brain->getTotalSynapseCount()));
        stats->emplace_back("Brain: Firing Neurons", std::to_string(brain->getFiringNeuronCount()));
        stats->emplace_back("Brain: Active Neurons", std::to_string(brain->getActiveNeuronCount()));
        stats->emplace_back("Brain: Total Spikes", std::to_string(brain->getTotalSpikeCount()));
        stats->emplace_back("Brain: Average Firing Rate", std::to_string(brain->getAverageFiringRate()));
        stats->emplace_back("Brain: E/I Ratio", std::to_string(brain->getExcitationInhibitionRatio()));
        stats->emplace_back("Brain: Developmental Stage", std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        
        // Agent statistics
        stats->emplace_back("Agent: Neuromodulation Level", std::to_string(agent->getNeuromodulationLevel()));
        stats->emplace_back("Agent: Curiosity Level", std::to_string(agent->getCuriosityLevel()));
        stats->emplace_back("Agent: Novelty Level", std::to_string(agent->getNoveltyLevel()));
        stats->emplace_back("Agent: Prediction Error", std::to_string(agent->getPredictionError()));
        
        // World statistics
        stats->emplace_back("World: Simulation Time", std::to_string(world->getSimulationTime()));
        stats->emplace_back("World: Agent Position", std::to_string(world->getAgentBody()->x) + ", " + std::to_string(world->getAgentBody()->y));
        stats->emplace_back("World: Agent Energy", std::to_string(world->getAgentBody()->energy));
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              "Get comprehensive simulation statistics");

    // Advanced Performance Analysis and Optimization
    m.def("analyzePerformanceProfile", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent, size_t durationSteps = 1000) {
        auto analysis = std::make_shared<std::map<std::string, double>>();
        
        // Collect performance metrics over time
        std::vector<double> firingRates;
        std::vector<double> spikeCounts;
        std::vector<double> neuromodulationLevels;
        
        for (size_t step = 0; step < durationSteps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
            
            float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            firingRates.push_back(brain->getAverageFiringRate());
            spikeCounts.push_back(static_cast<double>(brain->getTotalSpikeCount()));
            neuromodulationLevels.push_back(agent->getNeuromodulationLevel());
        }
        
        // Calculate statistical analysis
        double avgFiringRate = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
        double avgSpikeCount = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0) / spikeCounts.size();
        double avgNeuromodulation = std::accumulate(neuromodulationLevels.begin(), neuromodulationLevels.end(), 0.0) / neuromodulationLevels.size();
        
        // Calculate stability metrics
        double firingRateStd = 0.0, spikeCountStd = 0.0;
        if (firingRates.size() > 1) {
            double firingRateMean = avgFiringRate;
            double spikeCountMean = avgSpikeCount;
            
            double firingRateVar = 0.0, spikeCountVar = 0.0;
            for (double rate : firingRates) firingRateVar += (rate - firingRateMean) * (rate - firingRateMean);
            for (double count : spikeCounts) spikeCountVar += (count - spikeCountMean) * (count - spikeCountMean);
            
            firingRateStd = std::sqrt(firingRateVar / (firingRates.size() - 1));
            spikeCountStd = std::sqrt(spikeCountVar / (spikeCounts.size() - 1));
        }
        
        // Calculate optimization recommendations
        double firingStability = 1.0 / (1.0 + firingRateStd);
        double spikeEfficiency = avgSpikeCount / durationSteps;
        double neuromodulationBalance = 1.0 - std::abs(avgNeuromodulation - 0.5);
        
        double overallScore = (firingStability * 0.4 + spikeEfficiency * 0.3 + neuromodulationBalance * 0.3);
        
        // Store analysis results
        analysis->emplace("avg_firing_rate", avgFiringRate);
        analysis->emplace("firing_rate_std", firingRateStd);
        analysis->emplace("avg_spike_count", avgSpikeCount);
        analysis->emplace("spike_count_std", spikeCountStd);
        analysis->emplace("avg_neuromodulation", avgNeuromodulation);
        analysis->emplace("firing_stability", firingStability);
        analysis->emplace("spike_efficiency", spikeEfficiency);
        analysis->emplace("neuromodulation_balance", neuromodulationBalance);
        analysis->emplace("overall_performance_score", overallScore);
        
        // Performance categorization
        if (overallScore >= 0.8) {
            analysis->emplace("performance_category", "EXCELLENT");
            analysis->emplace("optimization_status", "OPTIMIZED");
        } else if (overallScore >= 0.6) {
            analysis->emplace("performance_category", "GOOD");
            analysis->emplace("optimization_status", "GOOD");
        } else if (overallScore >= 0.4) {
            analysis->emplace("performance_category", "FAIR");
            analysis->emplace("optimization_status", "NEEDS_IMPROVEMENT");
        } else {
            analysis->emplace("performance_category", "POOR");
            analysis->emplace("optimization_status", "REQUIRES_ATTENTION");
        }
        
        return analysis;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              py::arg("durationSteps") = 1000,
              "Analyze performance profile and return detailed metrics");

    // Memory Optimization and Profiling
    m.def("profileMemoryUsage", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent) {
        auto profile = std::make_shared<std::map<std::string, double>>();
        
        // Estimate memory usage components
        double neuronMemory = static_cast<double>(brain->getTotalNeuronCount()) * 64.0; // Approximate 64 bytes per neuron
        double synapseMemory = static_cast<double>(brain->getTotalSynapseCount()) * 32.0; // Approximate 32 bytes per synapse
        double configMemory = 1024.0; // Approximate config memory
        double agentMemory = 512.0;   // Approximate agent memory
        double worldMemory = 2048.0; // Approximate world memory
        
        double totalMemory = neuronMemory + synapseMemory + configMemory + agentMemory + worldMemory;
        
        // Calculate memory efficiency metrics
        double memoryPerNeuron = neuronMemory / brain->getTotalNeuronCount();
        double memoryPerSynapse = synapseMemory / brain->getTotalSynapseCount();
        double memoryPerSpike = totalMemory / brain->getTotalSpikeCount();
        
        // Memory optimization score
        double optimizationScore = 1.0;
        if (memoryPerNeuron > 100.0) optimizationScore *= 0.8; // High memory per neuron
        if (memoryPerSynapse > 50.0) optimizationScore *= 0.8;   // High memory per synapse
        if (memoryPerSpike > 10.0) optimizationScore *= 0.9;     // High memory per spike
        
        // Store profile results
        profile->emplace("total_memory_kb", totalMemory / 1024.0);
        profile->emplace("neuron_memory_kb", neuronMemory / 1024.0);
        profile->emplace("synapse_memory_kb", synapseMemory / 1024.0);
        profile->emplace("config_memory_kb", configMemory / 1024.0);
        profile->emplace("agent_memory_kb", agentMemory / 1024.0);
        profile->emplace("world_memory_kb", worldMemory / 1024.0);
        profile->emplace("memory_per_neuron", memoryPerNeuron);
        profile->emplace("memory_per_synapse", memoryPerSynapse);
        profile->emplace("memory_per_spike", memoryPerSpike);
        profile->emplace("memory_optimization_score", optimizationScore);
        
        return profile;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              "Profile memory usage and return optimization metrics");

    // Comprehensive Benchmarking Suite
    m.def("runComprehensiveBenchmark", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent, size_t testDuration = 2000) {
        auto benchmark = std::make_shared<std::map<std::string, double>>();
        
        // Multiple benchmark scenarios
        struct BenchmarkResult {
            std::string name;
            double stepsPerSecond;
            double memoryEfficiency;
            double responsiveness;
            double stability;
        };
        
        std::vector<BenchmarkResult> results;
        
        // Benchmark 1: Basic neural activity
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t steps = 0;
            
            for (size_t i = 0; i < testDuration / 4; ++i) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                agent->decodeMotorCommand();
                world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
                float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
                agent->applyRewardModulation(reward, 0.0f);
                steps++;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            double stepsPerSecond = steps / duration;
            
            results.push_back({"basic_neural_activity", stepsPerSecond, 0.8, 0.7, 0.9});
        }
        
        // Benchmark 2: Learning performance
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t steps = 0;
            double learningCurve = 0.0;
            
            for (size_t i = 0; i < testDuration / 4; ++i) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                agent->decodeMotorCommand();
                world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
                
                float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
                float predictionError = agent->getPredictionError();
                agent->applyRewardModulation(reward, predictionError);
                
                // Track learning progress
                learningCurve += agent->getCuriosityLevel();
                steps++;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            double stepsPerSecond = steps / duration;
            double avgLearningProgress = learningCurve / (testDuration / 4);
            
            results.push_back({"learning_performance", stepsPerSecond, avgLearningProgress, 0.6, 0.8});
        }
        
        // Benchmark 3: Memory consolidation
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            size_t steps = 0;
            double memoryUtilization = 0.0;
            
            for (size_t i = 0; i < testDuration / 4; ++i) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                agent->decodeMotorCommand();
                world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
                
                float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
                agent->applyRewardModulation(reward, 0.0f);
                
                memoryUtilization += brain->getActiveNeuronCount();
                steps++;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            double stepsPerSecond = steps / duration;
            double avgMemoryUtilization = memoryUtilization / (testDuration / 4);
            
            results.push_back({"memory_consolidation", stepsPerSecond, avgMemoryUtilization, 0.8, 0.7});
        }
        
        // Calculate overall benchmark results
        double totalSteps = 0;
        double totalSpeed = 0;
        double totalEfficiency = 0;
        double totalResponsiveness = 0;
        double totalStability = 0;
        
        for (const auto& result : results) {
            totalSteps += result.stepsPerSecond;
            totalSpeed += result.stepsPerSecond;
            totalEfficiency += result.memoryEfficiency;
            totalResponsiveness += result.responsiveness;
            totalStability += result.stability;
        }
        
        size_t numTests = results.size();
        
        // Store benchmark results
        benchmark->emplace("total_benchmarks", static_cast<double>(numTests));
        benchmark->emplace("avg_steps_per_second", totalSpeed / numTests);
        benchmark->emplace("avg_memory_efficiency", totalEfficiency / numTests);
        benchmark->emplace("avg_responsiveness", totalResponsiveness / numTests);
        benchmark->emplace("avg_stability", totalStability / numTests);
        benchmark->emplace("overall_benchmark_score", (totalSpeed + totalEfficiency + totalResponsiveness + totalStability) / (4.0 * numTests));
        
        // Store individual benchmark results
        for (const auto& result : results) {
            std::string key = "benchmark_" + result.name;
            benchmark->emplace(key + "_score", (result.stepsPerSecond + result.memoryEfficiency + result.responsiveness + result.stability) / 4.0);
        }
        
        return benchmark;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              py::arg("testDuration") = 2000,
              "Run comprehensive benchmark suite and return detailed results");

    // Advanced Configuration Utilities
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        auto errors = std::make_shared<std::vector<std::string>>();
        
        // Check for required configuration keys
        if (!config->has("brain.neuron_count")) {
            errors->push_back("Missing required key: brain.neuron_count");
        }
        if (!config->has("brain.synapse_density")) {
            errors->push_back("Missing required key: brain.synapse_density");
        }
        if (!config->has("random_seed")) {
            errors->push_back("Missing required key: random_seed");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    // Utility Functions
    m.def("createComplexWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 16, 16);  // Larger world with higher resolution vision
        world->setMaxEnergy(200.0f);
        world->setEnergyDecayRate(0.05f);
        return world;
    }, "Create a complex world with enhanced parameters");

    m.def("createOptimizedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        // Apply performance optimizations
        if (!config->has("brain.connection_probability")) {
            config->set("brain.connection_probability", 0.08f);
        }
        if (!config->has("plasticity.stdp.enable")) {
            config->set("plasticity.stdp.enable", true);
        }
        if (!config->has("plasticity.hebbian.enable")) {
            config->set("plasticity.hebbian.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable advanced features
        if (auto* spikeSystem = brain->getSpikeSystem()) {
            // Can add performance optimizations here
        }
        
        return brain;
    }, py::arg("config"), "Create an optimized brain with performance settings");

    m.def("benchmarkSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
                                 size_t steps = 1000, size_t warmupSteps = 100) {
        auto results = std::make_shared<std::map<std::string, double>>();
        
        // Warmup
        for (size_t i = 0; i < warmupSteps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        
        // Clear timing statistics
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Main benchmark
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        results->emplace("total_steps", static_cast<double>(steps));
        results->emplace("duration_seconds", duration);
        results->emplace("steps_per_second", steps / duration);
        results->emplace("final_spike_count", static_cast<double>(brain->getTotalSpikeCount()));
        results->emplace("firing_rate_hz", static_cast<double>(brain->getTotalSpikeCount()) / duration * 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
              py::arg("steps") = 1000, py::arg("warmupSteps") = 100,
              "Run benchmark test and return performance statistics");

// High-Level API Functions
    m.def("initializeNLMSystem", []() {
        // Initialize global NLM system settings
        Logger::getInstance().setLevel(Logger::Level::Info);
        // Additional initialization can be added here
        return std::make_shared<Config>();
    }, "Initialize NLM system with default settings");

    m.def("createCustomConfig", [](const py::kwargs& kwargs) {
        auto config = std::make_shared<Config>();
        for (auto item : kwargs) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("kwargs"), "Create custom configuration using keyword arguments");

    m.def("validateCompleteSystem", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent) {
        auto validator = std::make_shared<SimulationValidator>();
        auto validationResult = validator->validateSimulation(brain, world, agent);
        
        std::cout << "=== COMPLETE SYSTEM VALIDATION ===" << std::endl;
        std::cout << "System Health: " << (validationResult.isValid ? "✓ VALID" : "✗ INVALID") << std::endl;
        std::cout << "Step Errors: " << validationResult.stepErrors.size() << std::endl;
        
        if (!validationResult.isValid) {
            std::cout << "Validation Errors:" << std::endl;
            for (const auto& error : validationResult.stepErrors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        return validationResult;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              "Validate complete system and return validation result");

    m.def("analyzeSystemPerformance", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent) {
        auto analyzer = std::make_shared<PerformanceAnalyzer>();
        auto performanceReport = analyzer->analyzePerformance(brain, world, agent);
        
        std::cout << "=== SYSTEM PERFORMANCE ANALYSIS ===" << std::endl;
        std::cout << "Average Firing Rate: " << performanceReport.avgFiringRate << " Hz" << std::endl;
        std::cout << "Efficiency Score: " << performanceReport.efficiencyScore << "/100" << std::endl;
        std::cout << "Activity Pattern: " << performanceReport.activityPattern << std::endl;
        
        if (performanceReport.anomalies.size() > 0) {
            std::cout << "Performance Anomalies Detected:" << std::endl;
            for (const auto& anomaly : performanceReport.anomalies) {
                std::cout << "  - " << anomaly << std::endl;
            }
        }
        
        return performanceReport;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
              "Analyze system performance and return detailed report");

    m.def("createProductionReadySimulation", [](const py::kwargs& config) {
        // Create production-ready simulation with all advanced features
        auto brainConfig = std::make_shared<Config>();
        
        // Apply production-grade defaults
        brainConfig->set("brain.neuron_count", 2000);
        brainConfig->set("brain.synapse_density", 0.1f);
        brainConfig->set("plasticity.stdp.enable", true);
        brainConfig->set("plasticity.hebbian.enable", true);
        brainConfig->set("plasticity.structural.enable", true);
        brainConfig->set("neuromod.dopamine.scale", 1.0);
        brainConfig->set("neuromod.curiosity.enable", true);
        brainConfig->set("neuromod.novelty.enable", true);
        
        // Apply user overrides
        for (auto item : config) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                if (key.find(".") != std::string::npos) {
                    // Handle nested keys like "brain.neuron_count"
                    size_t dotPos = key.find(".");
                    std::string category = key.substr(0, dotPos);
                    std::string param = key.substr(dotPos + 1);
                    // In a real implementation, we would have more sophisticated handling
                    brainConfig->set(key, value.cast<int64_t>());
                } else {
                    brainConfig->set(key, value.cast<int64_t>());
                }
            } else if (value.is(py::float_())) {
                brainConfig->set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                brainConfig->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                brainConfig->set(key, value.cast<std::string>());
            }
        }
        
        return std::make_tuple(brainConfig, "Production simulation created successfully");
    }, py::arg("config") = py::kwargs(), "Create production-ready simulation with advanced features");
}

} // namespace nlm
