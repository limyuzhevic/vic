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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    m.def("createSimpleConfig", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.region_count", static_cast<int64_t>(1));
        config->set("brain.connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create a simple configuration with sensible defaults");

    // High-level convenience functions
    m.def("run_simple_simulation", [](int steps, int neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        return std::make_pair(
            brain->getTotalNeuronCount(),
            brain->getTotalSpikeCount()
        );
    }, py::arg("steps") = 100, py::arg("neuron_count") = 1000,
       "Run a simple simulation with default configuration");

    // Agent creation with learning enabled
    m.def("create_advanced_agent", [](std::shared_ptr<Brain> brain,
                                     bool enable_reward_modulation = true,
                                     bool enable_structural_plasticity = false,
                                     bool enable_development = false,
                                     bool enable_curiosity = false) {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (enable_reward_modulation) agent->enableRewardModulation(true);
        if (enable_structural_plasticity) agent->enableStructuralPlasticity(true);
        if (enable_development) agent->enableDevelopment(true);
        if (enable_curiosity) agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("reward_modulation") = true,
       py::arg("structural_plasticity") = false,
       py::arg("development") = false,
       py::arg("curiosity") = false,
       "Create an agent brain with learning subsystems enabled");

    // Performance monitoring utilities
    m.def("create_performance_monitor", [](std::shared_ptr<Brain> brain) {
        struct PerformanceMonitor {
            std::shared_ptr<Brain> brain_;
            std::vector<double> brain_times_;
            std::vector<double> step_times_;
            int total_steps_ = 0;
            
            PerformanceMonitor(std::shared_ptr<Brain> brain) : brain_(brain) {}
            
            void record_step(double brain_time, double step_time) {
                brain_times_.push_back(brain_time);
                step_times_.push_back(step_time);
                total_steps_++;
            }
            
            py::dict get_performance_report() {
                py::dict report;
                if (!brain_times_.empty()) {
                    double sum_brain = std::accumulate(brain_times_.begin(), brain_times_.end(), 0.0);
                    double sum_step = std::accumulate(step_times_.begin(), step_times_.end(), 0.0);
                    
                    report["average_brain_time_ms"] = (sum_brain / brain_times_.size()) * 1000.0;
                    report["average_step_time_ms"] = (sum_step / step_times_.size()) * 1000.0;
                    report["total_steps"] = total_steps_;
                    report["spike_rate"] = brain_->getTotalSpikeCount() / static_cast<double>(total_steps_);
                    report["neurons_per_step"] = brain_->getTotalNeuronCount();
                }
                return report;
            }
            
            std::string get_performance_summary() {
                std::stringstream ss;
                ss << "Performance Monitor:" << std::endl;
                ss << "Total steps: " << total_steps_ << std::endl;
                if (!brain_times_.empty()) {
                    double avg_brain = std::accumulate(brain_times_.begin(), brain_times_.end(), 0.0) / brain_times_.size();
                    double avg_step = std::accumulate(step_times_.begin(), step_times_.end(), 0.0) / step_times_.size();
                    ss << "Average brain time: " << avg_brain * 1000.0 << " ms" << std::endl;
                    ss << "Average step time: " << avg_step * 1000.0 << " ms" << std::endl;
                    ss << "Spike rate: " << brain_->getTotalSpikeCount() / static_cast<double>(total_steps_) << " spikes/step" << std::endl;
                }
                return ss.str();
            }
        };
        
        return std::make_shared<PerformanceMonitor>(brain);
    }, py::arg("brain"), "Create a performance monitor for tracking simulation metrics");

    // Model export utilities
    m.def("export_model", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                            const std::string& format) {
        if (format == "binary") {
            return brain->save(filepath);
        } else if (format == "json") {
            // Export as JSON (simplified implementation)
            auto config = brain->getConfig();
            std::ofstream file(filepath);
            if (!file.is_open()) return false;
            
            file << "{\n";
            file << "  \\"brain_info\\": {\n";
            file << "    \\"neuron_count\\": " << brain->getTotalNeuronCount() << ",\n";
            file << "    \\"synapse_count\\": " << brain->getTotalSynapseCount() << ",\n";
            file << "    \\"total_spikes\\": " << brain->getTotalSpikeCount() << ",\n";
            file << "    \\"firing_rate\\": " << brain->getAverageFiringRate() << "\n";
            file << "  },\n";
            file << "  \\"config\\": {\n";
            for (const auto& key : config->getKeys()) {
                file << "    \\"" << key << "\\": ";
                auto value = config->get<std::string>(key);
                if (value.has_value()) {
                    file << "\\"" << value.value() << "\\"";
                }
                file << ",\n";
            }
            file << "  }\n";
            file << "}\n";
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("filepath"), py::arg("format") = "binary",
       "Export brain model to file in specified format (binary or json)");

    // Model import utilities
    m.def("import_model", [](const std::string& filepath, const std::string& format) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath + ".config")) {
            config = std::make_shared<Config>(); // Use default if config file not found
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (format == "binary") {
            brain->load(filepath);
            return brain;
        } else if (format == "json") {
            // Import from JSON (simplified implementation)
            std::ifstream file(filepath);
            if (!file.is_open()) return std::shared_ptr<Brain>(nullptr);
            
            // Parse JSON (simplified - would need proper JSON parser)
            // For now, just return a default brain
            brain->initialize();
            return brain;
        }
        return std::shared_ptr<Brain>(nullptr);
    }, py::arg("filepath"), py::arg("format") = "binary",
       "Import brain model from file (binary or json format)");

    // Convenience function for creating example learning environments
    m.def("create_learning_environment", []() {
        auto world = std::make_shared<SimpleWorld>";
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure for learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete learning environment with agent, world, and brain");

    // Batch simulation utilities
    m.def("batch_simulation", [](const std::vector<std::pair<int, int>>& configs) {
        std::vector<std::pair<size_t, size_t>> results;
        
        for (const auto& config : configs) {
            auto brain_config = std::make_shared<Config>();
            brain_config->set("brain.neuron_count", static_cast<int64_t>(config.first));
            brain_config->set("simulation_timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(brain_config);
            brain->initialize();
            
            for (int step = 0; step < config.second; ++step) {
                brain->step(step);
            }
            
            results.push_back(std::make_pair(
                brain->getTotalNeuronCount(),
                brain->getTotalSpikeCount()
            ));
        }
        
        return results;
    }, py::arg("configs"),
       "Run multiple simulations with different configurations and return results");

    // Educational utility functions
    m.def("get_neuron_type_summary", []() {
        std::stringstream ss;
        ss << "Neuron Type Functions:" << std::endl;
        ss << "  - Excitatory: Positive input, promotes firing" << std::endl;
        ss << "  - Inhibitory: Negative input, suppresses firing" << std::endl;
        ss << "  - Sensory: Responds to external stimuli" << std::endl;
        ss << "  - Motor: Generates output actions" << std::endl;
        ss << "  - Modulatory: Influences plasticity and learning" << std::endl;
        ss << "  - Internal: Hidden layer processing" << std::endl;
        return ss.str();
    }, "Get educational summary of neuron types");

    m.def("get_plasticity_summary", []() {
        std::stringstream ss;
        ss << "Plasticity Mechanisms:" << std::endl;
        ss << "  - STDP: Spike-Timing Dependent Plasticity" << std::endl;
        ss << "  - Hebbian: Co-activation strengthens connections" << std::endl;
        ss << "  - Structural: Creates/destroys synapses" << std::endl;
        ss << "  - Reward-modulated: Reinforcement learning" << std::endl;
        return ss.str();
    }, "Get educational summary of plasticity mechanisms");

    m.def("get_brain_regions_summary", []() {
        std::stringstream ss;
        ss << "Brain Regions and Functions:" << std::endl;
        ss << "  - Cortex: Main processing and computation" << std::endl;
        ss << "  - Subcortical: Lower-level processing" << std::endl;
        ss << "  - Brainstem: Basic life functions" << std::endl;
        ss << "  - Cerebellum: Coordination and timing" << std::endl;
        return ss.str();
    }, "Get educational summary of brain regions");

    // Version and info utilities
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM - Neural Learning Machine Python Bindings";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__website__") = "https://github.com/nlm-project/nlm";

    // Python module metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        A Python interface to the NLM C++ neural simulation framework.
        
        Provides high-level abstractions for creating, training, and simulating
        artificial neural networks with biologically-inspired learning mechanisms.
        
        Features:
        - High-performance spiking neural networks
        - Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
        - Advanced neuromodulation systems
        - Memory and cognition modules
        - World simulation and agent interaction
        - Configuration management and model persistence
        
        Quick Start:
        >>> import pynlm
        >>> brain = pynlm.createSimpleConfig()
        >>> brain = pynlm.createBrain(brain)
        >>> brain.initialize()
        
        For comprehensive documentation, visit:
        https://nlm.readthedocs.io
        
        Examples and tutorials available in the NLM documentation.
    )pbdoc";
}
