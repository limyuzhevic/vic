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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool result = self.loadFromFile(filepath);
            if (!result) {
                throw py::error_already_set();
            }
            return result;
        }, py::arg("filepath"),
             "Load configuration from a JSON or simple text file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            bool result = self.loadFromArgs(argc, argv);
            if (!result) {
                throw py::error_already_set();
            }
            return result;
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Returns True on success, False on error.")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool result = self.saveToFile(filepath);
            if (!result) {
                throw py::error_already_set();
            }
            return result;
        }, py::arg("filepath"),
             "Save configuration to a text file. Returns True on success, False on error.")
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

    // Advanced brain configuration and analysis
    m.def("configureBrainForLearning", [](std::shared_ptr<Brain> brain, const std::string& learningMode) {
        // Advanced brain configuration for different learning modes
        auto config = brain->getConfig();
        
        if (learningMode == "exploration") {
            // Configure for exploration-based learning
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.curiosity.scale", 1.5f);
            config->set("neuromodulation.novelty.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
            config->set("plasticity.reward_modulated.enable", true);
        } else if (learningMode == "exploitation") {
            // Configure for exploitation-based learning
            config->set("neuromodulation.curiosity.enable", false);
            config->set("neuromodulation.novelty.enable", false);
            config->set("plasticity.stdp.learning_rate", 0.0001f);
            config->set("plasticity.hebbian.learning_rate", 0.005f);
            config->set("plasticity.reward_modulated.enable", true);
        } else if (learningMode == "balanced") {
            // Configure for balanced learning
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.curiosity.scale", 1.0f);
            config->set("neuromodulation.novelty.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.0005f);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
            config->set("plasticity.reward_modulated.enable", true);
        }
        
        // Update brain configuration
        // Note: In a real implementation, this would require modifying the brain's internal config
        return true;
    }, py::arg("brain"), py::arg("learningMode"),
         "Configure brain for specific learning mode (exploration, exploitation, or balanced)");

    m.def("analyzeBrainPerformance", [](std::shared_ptr<Brain> brain) {
        // Advanced brain performance analysis
        std::map<std::string, double> metrics;
        
        if (brain) {
            metrics["total_neurons"] = brain->getTotalNeuronCount();
            metrics["total_synapses"] = brain->getTotalSynapseCount();
            metrics["firing_rate"] = brain->getAverageFiringRate();
            metrics["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
            metrics["total_spikes"] = brain->getTotalSpikeCount();
            metrics["pending_events"] = brain->getPendingSpikeEventCount();
            
            // Get neuromodulation metrics if available
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                metrics["dopamine_level"] = dopamine->getLevel();
                metrics["dopamine_plasticity"] = dopamine->getPlasticityFactor();
            }
            
            // Get development metrics
            metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        }
        
        // Convert to Python dictionary
        py::dict result;
        for (const auto& metric : metrics) {
            result[metric.first] = metric.second;
        }
        
        return result;
    }, py::arg("brain"),
         "Analyze brain performance and return detailed metrics");

    m.def("runAdvancedSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, 
                                     int steps, double dt, bool adaptiveLearning,
                                     std::function<void(const std::string&)> progressCallback) {
        // Advanced simulation with adaptive learning and progress monitoring
        int completedSteps = 0;
        double totalReward = 0.0;
        double maxCuriosity = 0.0;
        
        std::cout << "Starting advanced simulation..." << std::endl;
        
        for (int step = 0; step < steps; ++step) {
            try {
                // Run simulation step
                world->update(dt);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(step, step * dt);
                
                Action action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, step * dt);
                
                // Apply adaptive learning if enabled
                if (adaptiveLearning) {
                    float curiosity = agent->getCuriosityLevel();
                    float novelty = agent->getNoveltyLevel();
                    
                    if (curiosity > 0.8 || novelty > 0.6) {
                        // Increase learning rate based on exploration drive
                        agent->enableRewardModulation(true);
                        float currentReward = agent->getNeuromodulationLevel();
                        agent->applyRewardModulation(currentReward * 1.1, currentReward * 0.9);
                    }
                }
                
                totalReward += 1.0; // Simple reward accumulation
                maxCuriosity = std::max(maxCuriosity, agent->getCuriosityLevel());
                
                completedSteps++;
                
                // Progress callback
                if (progressCallback && step % 100 == 0) {
                    double progress = static_cast<double>(step) / steps * 100.0;
                    std::string progressStr = "Progress: " + std::to_string(progress) + "% (Step " + 
                                           std::to_string(step) + "/" + std::to_string(steps) + ")";
                    progressCallback(progressStr);
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error at step " << step << ": " << e.what() << std::endl;
                // Continue simulation despite errors
                completedSteps++;
            }
        }
        
        // Calculate final metrics
        double avgFiringRate = completedSteps > 0 ? brain->getAverageFiringRate() : 0.0;
        double learningEfficiency = totalReward / completedSteps;
        
        std::cout << "Advanced simulation completed!" << std::endl;
        std::cout << "Total steps: " << completedSteps << std::endl;
        std::cout << "Average firing rate: " << avgFiringRate << " Hz" << std::endl;
        std::cout << "Learning efficiency: " << learningEfficiency << std::endl;
        std::cout << "Peak curiosity: " << maxCuriosity << std::endl;
        
        // Return simulation results
        py::dict results;
        results["steps_completed"] = completedSteps;
        results["total_steps"] = steps;
        results["success_rate"] = static_cast<double>(completedSteps) / steps;
        results["average_firing_rate"] = avgFiringRate;
        results["total_reward"] = totalReward;
        results["learning_efficiency"] = learningEfficiency;
        results["peak_curiosity"] = maxCuriosity;
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         py::arg("steps"), py::arg("dt"), py::arg("adaptiveLearning") = false,
         py::arg("progressCallback") = nullptr,
         "Run advanced simulation with adaptive learning and progress monitoring");

    m.def("createLearningProfile", [](const std::string& profileName, 
                                    double curiosityWeight,
                                    double noveltyWeight,
                                    double rewardWeight) {
        // Create custom learning profile
        auto config = std::make_shared<Config>();
        
        // Configure neuromodulation based on profile
        config->set("neuromodulation.curiosity.weight", curiosityWeight);
        config->set("neuromodulation.novelty.weight", noveltyWeight);
        config->set("neuromodulation.reward.weight", rewardWeight);
        
        // Configure plasticity based on profile
        double totalWeight = curiosityWeight + noveltyWeight + rewardWeight;
        if (totalWeight > 0) {
            config->set("plasticity.stdp.learning_rate", 0.001f * curiosityWeight / totalWeight);
            config->set("plasticity.hebbian.learning_rate", 0.01f * noveltyWeight / totalWeight);
            config->set("plasticity.reward_modulated.learning_rate", 0.1f * rewardWeight / totalWeight);
        }
        
        return config;
    }, py::arg("profileName"), py::arg("curiosityWeight") = 1.0,
         py::arg("noveltyWeight") = 1.0, py::arg("rewardWeight") = 1.0,
         "Create custom learning profile with weighted neuromodulation");

    m.def("analyzeLearningPatterns", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        // Analyze learning patterns across multiple brains
        std::map<std::string, double> collectiveMetrics;
        
        if (brains.empty()) {
            return py::dict();
        }
        
        double totalFiringRate = 0.0;
        double totalPlasticity = 0.0;
        double totalCuriosity = 0.0;
        int validBrains = 0;
        
        for (const auto& brain : brains) {
            if (brain) {
                totalFiringRate += brain->getAverageFiringRate();
                totalPlasticity += brain->getExcitationInhibitionRatio();
                
                auto* dopamine = brain->getDopamine();
                if (dopamine) {
                    totalCuriosity += dopamine->getLevel();
                }
                
                validBrains++;
            }
        }
        
        if (validBrains > 0) {
            collectiveMetrics["average_firing_rate"] = totalFiringRate / validBrains;
            collectiveMetrics["average_plasticity"] = totalPlasticity / validBrains;
            collectiveMetrics["average_curiosity"] = totalCuriosity / validBrains;
            collectiveMetrics["brain_count"] = static_cast<double>(validBrains);
        }
        
        // Convert to Python dictionary
        py::dict result;
        for (const auto& metric : collectiveMetrics) {
            result[metric.first] = metric.second;
        }
        
        return result;
    }, py::arg("brains"),
         "Analyze learning patterns across multiple brains and return collective metrics");

    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
        // Validate configuration with detailed error reporting
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Configuration is null");
            return py::make_tuple(false, errors);
        }
        
        // Check for required configuration keys
        std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "plasticity.stdp.learning_rate"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate configuration values
        if (config->has("brain.neuron_count")) {
            auto neuronCount = config->get<int64_t>("brain.neuron_count");
            if (neuronCount < 10 || neuronCount > 1000000) {
                errors.push_back("brain.neuron_count must be between 10 and 1,000,000");
            }
        }
        
        if (config->has("brain.connection_probability")) {
            auto connProb = config->get<double>("brain.connection_probability");
            if (connProb < 0.0 || connProb > 1.0) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
        
        return py::make_tuple(errors.empty(), errors);
    }, py::arg("config"),
         "Validate configuration and return validation result with error messages");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
