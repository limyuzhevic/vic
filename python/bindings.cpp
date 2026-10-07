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

    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID.value;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID.value;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID.value;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID.value;

    // Simulation utility functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, size_t numSteps, bool verbose = false) {
        float totalReward = 0.0f;
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            auto action = brain->produceAction();
            if (verbose) {
                std::cout << "Step " << step << ": " 
                          << brain->getFiringNeuronCount() << " firing neurons\n";
            }
        }
        return totalReward;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("verbose") = false,
        "Run a simple simulation for the specified number of steps");

    m.def("create_demo_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        return std::make_shared<Brain>(config);
    }, "Create a demo brain with standard configuration");

    m.def("create_agent_world_simulation", [](size_t worldWidth = 20, size_t worldHeight = 20,
                                                size_t visionWidth = 8, size_t visionHeight = 8) {
        auto brain = create_demo_brain();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->setRandomSeed(42);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Create a complete agent-world simulation setup");

    // Configuration utilities
    m.def("create_default_config_with_params", 
        [](size_t neuronCount, size_t regionCount, float connectionProbability) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", static_cast<int64_t>(regionCount));
            config->set("connection_probability", connectionProbability);
            return config;
        }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
        py::arg("connectionProbability") = 0.1f,
        "Create a default configuration with custom parameters");

    m.def("load_config_from_file", &Config::loadFromFile,
        py::arg("filepath"), "Load configuration from a JSON file");

    // Monitoring and statistics utilities
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Get comprehensive statistics about the brain");

    m.def("get_agent_metrics", [](std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, double> metrics;
        metrics["curiosity_level"] = agent->getCuriosityLevel();
        metrics["novelty_level"] = agent->getNoveltyLevel();
        metrics["prediction_error"] = agent->getPredictionError();
        metrics["neuromodulation_level"] = agent->getNeuromodulationLevel();
        return metrics;
    }, py::arg("agent"), "Get metrics about agent brain performance");

    m.def("monitor_simulation", [](std::shared_ptr<Brain> brain, size_t numSteps,
                                   bool logProgress = true) {
        std::vector<std::map<std::string, double>> history;
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (logProgress && step % 100 == 0) {
                std::cout << "Step " << step << "/" << numSteps 
                          << ": " << brain->getFiringNeuronCount() 
                          << " firing neurons\n";
            }
            
            std::map<std::string, double> snapshot;
            snapshot["step"] = step;
            snapshot["firing_neurons"] = brain->getFiringNeuronCount();
            snapshot["active_neurons"] = brain->getActiveNeuronCount();
            snapshot["average_firing_rate"] = brain->getAverageFiringRate();
            snapshot["total_spikes"] = brain->getTotalSpikeCount();
            history.push_back(snapshot);
        }
        return history;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("logProgress") = true,
        "Monitor brain activity over time and return history");

    // Persistence utilities
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    m.def("checkpoint_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->save(filepath)) {
            return std::make_pair(true, "Checkpoint saved successfully");
        } else {
            return std::make_pair(false, "Failed to save checkpoint");
        }
    }, py::arg("brain"), py::arg("filepath"), "Create a checkpoint of the brain state");

    // Development utilities
    m.def("advance_development", [](std::shared_ptr<Brain> brain, double timestep) {
        auto* agent = dynamic_cast<AgentBrain*>(brain.get());
        if (agent) {
            agent->updateDevelopment(timestep);
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("timestep"), "Advance brain development by timestep");

    m.def("set_development_stage", [](std::shared_ptr<Brain> brain, 
                                     nlm::DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"), "Set the developmental stage of the brain");

    m.def("get_development_info", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> info;
        info["stage"] = developmentalStageToString(brain->getDevelopmentalStage());
        info["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        info["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        return info;
    }, py::arg("brain"), "Get information about brain development state");

    // Enhanced enum utilities
    m.def("get_enum_values", []() {
        py::list values;
        values.append("NeuronType: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal");
        values.append("SynapseType: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction");
        values.append("ActionType: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom");
        values.append("MotorCommand: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait");
        values.append("WorldObjectType: Empty, Resource, Hazard, Wall, Marker");
        values.append("DevelopmentalStage: Initial, CriticalPeriod, Maturation, Adult, Aging");
        values.append("FiringState: Resting, Active, Refractory, Inhibited");
        return values;
    }, "Get all available enum values for reference");

    m.def("get_enum_info", [](const std::string& enumName) {
        std::map<std::string, std::string> info;
        if (enumName == "NeuronType") {
            info["Excitatory"] = "Excitatory neurons that increase firing probability";
            info["Inhibitory"] = "Inhibitory neurons that decrease firing probability";
            info["Modulatory"] = "Modulatory neurons that affect plasticity";
            info["Sensory"] = "Sensory processing neurons";
            info["Motor"] = "Motor output neurons";
            info["Internal"] = "Internal regulatory neurons";
        } else if (enumName == "SynapseType") {
            info["Excitatory"] = "Excitatory synapses that increase postsynaptic firing";
            info["Inhibitory"] = "Inhibitory synapses that decrease postsynaptic firing";
            info["Modulatory"] = "Modulatory synapses that affect plasticity";
            info["Electrical"] = "Electrical synapses that allow direct ion flow";
            info["GapJunction"] = "Gap junction synapses for rapid signaling";
        }
        return info;
    }, py::arg("enumName"), "Get detailed information about an enum type");

    // Quick experiment utilities
    m.def("run_phase3_demo", [](size_t numEpisodes, size_t stepsPerEpisode) {
        std::cout << "Running Phase 3 demo with " << numEpisodes 
                  << " episodes of " << stepsPerEpisode << " steps each.\n";
        // This would call the Phase3Demo implementation
        // For now, just return a placeholder
        return std::make_pair(true, "Phase 3 demo completed (placeholder implementation)");
    }, py::arg("numEpisodes") = 5, py::arg("stepsPerEpisode") = 200,
        "Run a Phase 3 world interaction demo");

    m.def("run_phase4_demo", [](size_t numTrials) {
        std::cout << "Running Phase 4 demo with " << numTrials 
                  << " trials.\n";
        // This would call the Phase4Demo implementation
        return std::make_pair(true, "Phase 4 demo completed (placeholder implementation)");
    }, py::arg("numTrials") = 50, "Run a Phase 4 cognitive experiments demo");

    m.def("run_phase6_integration", [](size_t neuronCount, size_t maxSteps) {
        std::cout << "Running Phase 6 integration test with " << neuronCount 
                  << " neurons for " << maxSteps << " steps.\n";
        // This would call the Phase6Demo implementation
        return std::make_pair(true, "Phase 6 integration completed (placeholder implementation)");
    }, py::arg("neuronCount") = 500, py::arg("maxSteps") = 2000,
        "Run a Phase 6 final integration demo");

    // Python helper functions for common patterns
    m.def("reset_all", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                        std::shared_ptr<SimpleWorld> world) {
        if (brain) brain->reset();
        if (agent) agent->reset();
        if (world) world->reset();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
        "Reset brain, agent, and world to initial state");

    m.def("enable_all_features", [](std::shared_ptr<AgentBrain> agent) {
        if (agent) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
    }, py::arg("agent"), "Enable all learning features in agent brain");

    m.def("get_simulation_summary", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                       std::shared_ptr<SimpleWorld> world) {
        std::ostringstream summary;
        summary << "=== SIMULATION SUMMARY ===\n";
        if (brain) {
            summary << "Brain: " << brain->getTotalNeuronCount() << " neurons, "
                    << brain->getTotalSynapseCount() << " synapses\n";
            summary << "  Firing: " << brain->getFiringNeuronCount() << " neurons\n";
            summary << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
            summary << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        }
        if (agent) {
            summary << "Agent: curiosity=" << agent->getCuriosityLevel() << ", "
                    << "novelty=" << agent->getNoveltyLevel() << ", "
                    << "prediction error=" << agent->getPredictionError() << "\n";
        }
        if (world) {
            summary << "World: size=" << world->getWidth() << "x" << world->getHeight() 
                    << ", time=" << world->getSimulationTime() << "s\n";
        }
        return summary.str();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
        "Get a human-readable summary of the current simulation state");
}

} // namespace nlm
