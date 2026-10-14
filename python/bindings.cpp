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

    // ========== CONVENIENCE FACTORY FUNCTIONS ==========
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create configuration from JSON file");

    m.def("createConfigFromArgs", [](int argc, char** argv) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromArgs(argc, argv);
        return config;
    }, py::arg("argc"), py::arg("argv"), "Create configuration from command line arguments");

    // ========== SIMULATION CONTEXT MANAGER ==========
    
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for NLM simulation lifecycle)pbdoc")
        .def(py::init<>())
        .def("create_brain", &SimulationContext::create_brain, py::arg("config"), "Create and initialize brain")
        .def("create_agent_brain", &SimulationContext::create_agent_brain, py::arg("brain"), "Create agent brain interface")
        .def("create_world", &SimulationContext::create_world, "Create simple world")
        .def("initialize", &SimulationContext::initialize, py::arg("world"), "Initialize agent with world")
        .def("step", &SimulationContext::step, py::arg("steps"), py::arg("autoreset") = false, "Run simulation steps")
        .def("run_episode", &SimulationContext::run_episode, py::arg("world"), py::arg("max_steps"), py::arg("autoreset") = false, "Run complete episode")
        .def("get_statistics", &SimulationContext::get_statistics, "Get simulation statistics")
        .def("get_performance", &SimulationContext::get_performance, "Get performance metrics")
        .def("get_status_summary", &SimulationContext::get_status_summary, "Get status summary")
        .def("reset", &SimulationContext::reset, "Reset all components")
        .def("cleanup", &SimulationContext::cleanup, "Cleanup and reset resources");
    
    // ========== ADVANCED UTILITY FUNCTIONS ==========
    
    m.def("run_silent_brain", [](std::shared_ptr<Config> config, size_t steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain;
    }, py::arg("config"), py::arg("steps"), "Run brain simulation without world interaction");

    m.def("run_agent_episode", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                                size_t max_steps, bool enable_learning = true) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (enable_learning) {
                auto reward = percept.getInternal();
                agent->applyRewardModulation(reward.empty() ? 0.0f : reward[0], 0.0f);
            }
        }
        
        return brain;
    }, py::arg("config"), py::arg("world"), py::arg("max_steps"), py::arg("enable_learning") = true,
       "Run complete agent episode with brain-world interaction");

    m.def("benchmark_config", [](const std::vector<std::shared_ptr<Config>>& configs, size_t steps) {
        struct BenchmarkResult {
            std::string config_id;
            double initialization_time;
            double step_time;
            double total_time;
            size_t total_spikes;
            float avg_firing_rate;
        };
        
        std::vector<BenchmarkResult> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto& config = configs[i];
            auto brain = std::make_shared<Brain>(config);
            
            // Measure initialization
            auto start = std::chrono::high_resolution_clock::now();
            brain->initialize();
            auto end = std::chrono::high_resolution_clock::now();
            double init_time = std::chrono::duration<double>(end - start).count();
            
            // Measure steps
            start = std::chrono::high_resolution_clock::now();
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            end = std::chrono::high_resolution_clock::now();
            double step_time = std::chrono::duration<double>(end - start).count();
            
            // Calculate statistics
            double total_time = init_time + step_time;
            size_t total_spikes = brain->getTotalSpikeCount();
            float avg_firing_rate = brain->getAverageFiringRate();
            
            results.push_back({
                "config_" + std::to_string(i),
                init_time,
                step_time / steps,
                total_time,
                total_spikes,
                avg_firing_rate
            });
        }
        
        return results;
    }, py::arg("configs"), py::arg("steps"), "Benchmark multiple configurations");

    // ========== CONFIGURATION HELPERS ==========
    
    m.def("get_config_presets", []() {
        std::vector<std::pair<std::string, std::shared_ptr<Config>>> presets;
        
        // Minimal configuration
        auto minimal = std::make_shared<Config>();
        minimal->set("neuron_count", 100);
        minimal->set("region_count", 1);
        presets.emplace_back("minimal", minimal);
        
        // Standard configuration
        auto standard = std::make_shared<Config>();
        standard->set("neuron_count", 1000);
        standard->set("region_count", 4);
        standard->set("connection_probability", 0.1f);
        standard->set("stdp_learning_rate", 0.001f);
        presets.emplace_back("standard", standard);
        
        // Development configuration
        auto development = std::make_shared<Config>();
        development->set("neuron_count", 2000);
        development->set("region_count", 6);
        development->set("connection_probability", 0.15f);
        development->set("stdp_learning_rate", 0.002f);
        development->set("plasticity.structural.enable", true);
        development->set("neuromod.curiosity.enable", true);
        development->set("neuromod.novelty.enable", true);
        presets.emplace_back("development", development);
        
        // Research configuration
        auto research = std::make_shared<Config>();
        research->set("neuron_count", 5000);
        research->set("region_count", 8);
        research->set("connection_probability", 0.08f);
        research->set("stdp_learning_rate", 0.001f);
        research->set("plasticity.structural.enable", true);
        research->set("plasticity.hebbian.enable", true);
        research->set("neuromod.curiosity.enable", true);
        research->set("neuromod.novelty.enable", true);
        research->set("neuromod.dopamine.scale", 1.5f);
        presets.emplace_back("research", research);
        
        return presets;
    }, "Get pre-configured settings for common use cases");

    // ========== VALIDATION HELPERS ==========
    
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        // Check neuron count reasonable
        auto neuron_count_opt = config->get<size_t>("neuron_count");
        if (neuron_count_opt) {
            size_t neuron_count = *neuron_count_opt;
            if (neuron_count < 10) {
                warnings.push_back("Neuron count very low (< 10) - simulation may be too simple");
            } else if (neuron_count > 100000) {
                warnings.push_back("Neuron count very high (> 100K) - may impact performance");
            }
        }
        
        // Check region count reasonable
        auto region_count_opt = config->get<size_t>("region_count");
        if (region_count_opt) {
            size_t region_count = *region_count_opt;
            if (region_count == 0) {
                errors.push_back("Region count must be at least 1");
            }
        }
        
        // Check connection probability
        auto conn_prob_opt = config->get<float>("connection_probability");
        if (conn_prob_opt) {
            float prob = *conn_prob_opt;
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("Connection probability must be between 0 and 1");
            }
        }
        
        // Check timestep
        auto timestep_opt = config->get<double>("simulation_timestep");
        if (timestep_opt) {
            double timestep = *timestep_opt;
            if (timestep <= 0.0) {
                errors.push_back("Simulation timestep must be positive");
            } else if (timestep > 0.1) {
                warnings.push_back("Large timestep (> 0.1s) may affect simulation accuracy");
            }
        }
        
        return std::make_tuple(warnings, errors);
    }, py::arg("config"), "Validate configuration and return warnings/errors");

    // ========== STATISTICS AND MONITORING ==========
    
    m.def("collect_statistics", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent, size_t steps) {
        struct SimulationStatistics {
            std::vector<double> step_times;
            std::vector<size_t> spike_counts;
            std::vector<float> firing_rates;
            std::vector<float> e_i_ratios;
            std::vector<size_t> active_neurons;
            std::vector<float> neuromod_levels;
            std::vector<float> curiosity_levels;
            std::vector<float> novelty_levels;
            std::vector<size_t> action_counts;
            double total_time;
            size_t total_steps;
        };
        
        SimulationStatistics stats;
        stats.step_times.reserve(steps);
        stats.spike_counts.reserve(steps);
        stats.firing_rates.reserve(steps);
        stats.e_i_ratios.reserve(steps);
        stats.active_neurons.reserve(steps);
        stats.neuromod_levels.reserve(steps);
        stats.curiosity_levels.reserve(steps);
        stats.novelty_levels.reserve(steps);
        stats.action_counts.reserve(steps);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            // Update world
            if (world) world->update(0.1);
            
            // Process sensory input
            if (agent && world) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
            }
            
            // Brain step
            brain->step(step);
            
            // Record statistics
            stats.step_times.push_back(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - step_start).count());
            stats.spike_counts.push_back(brain->getFiringNeuronCount());
            stats.firing_rates.push_back(brain->getAverageFiringRate());
            stats.e_i_ratios.push_back(brain->getExcitationInhibitionRatio());
            stats.active_neurons.push_back(brain->getActiveNeuronCount());
            
            if (agent) {
                stats.neuromod_levels.push_back(agent->getNeuromodulationLevel());
                stats.curiosity_levels.push_back(agent->getCuriosityLevel());
                stats.novelty_levels.push_back(agent->getNoveltyLevel());
            }
            
            // Action statistics
            if (agent) {
                auto action = agent->decodeMotorCommand();
                size_t action_idx = static_cast<size_t>(action);
                if (action_idx < stats.action_counts.size()) {
                    stats.action_counts[action_idx]++;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        stats.total_time = std::chrono::duration<double>(end_time - start_time).count();
        stats.total_steps = steps;
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), "Collect detailed simulation statistics");

    // ========== LEGACY COMPATIBILITY ==========
    
    // Keep original functions for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (legacy)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (legacy)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (legacy)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (legacy)");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
