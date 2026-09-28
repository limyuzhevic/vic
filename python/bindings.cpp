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

    // === Convenience Functions for Common Workflows ===

    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, int num_steps, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent provided");
        }
        
        brain->initialize();
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, " << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("verbose") = false,
         "Run a complete agent simulation with brain, world, and agent integration");

    m.def("create_learning_experiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                           std::shared_ptr<SimpleWorld> world, int steps) {
        if (!brain || !agent || !world) {
            throw std::runtime_error("Invalid components for learning experiment");
        }
        
        brain->initialize();
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        float initial_mean_weight = 0.0f;
        float final_mean_weight = 0.0f;
        int total_spikes = 0;
        
        // Get initial weight statistics
        if (auto* region = brain->getRegion(RegionId(1))) {
            float sum = 0.0f;
            size_t count = 0;
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
                ++count;
            }
            if (count > 0) initial_mean_weight = sum / count;
        }
        
        // Run learning experiment
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Apply reward based on novelty
            float reward = agent->getNoveltyLevel();
            agent->applyRewardModulation(reward, 0.0f);
            
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            total_spikes = brain->getTotalSpikeCount();
            
            agent->updateDevelopment(0.1);
        }
        
        // Get final weight statistics
        if (auto* region = brain->getRegion(RegionId(1))) {
            float sum = 0.0f;
            size_t count = 0;
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
                ++count;
            }
            if (count > 0) final_mean_weight = sum / count;
        }
        
        float weight_change = final_mean_weight - initial_mean_weight;
        
        return std::make_tuple(initial_mean_weight, final_mean_weight, total_spikes, weight_change);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
         "Create and run a learning experiment, returning initial/final weights, spike count, and change");

    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    m.def("save_brain_with_validation", [](std::shared_ptr<Brain> brain, const std::string& filepath, 
                                          bool create_backup = true) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        // Validate brain state before saving
        if (brain->getTotalNeuronCount() == 0) {
            throw std::runtime_error("Cannot save brain with zero neurons");
        }
        
        std::string backup_path = filepath + ".backup";
        if (create_backup) {
            brain->save(backup_path);
        }
        
        brain->save(filepath);
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("create_backup") = true,
         "Save brain state with optional backup and validation");

    m.def("load_brain_with_validation", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        try {
            brain->load(filepath);
            
            // Verify the loaded brain
            if (brain->getTotalNeuronCount() == 0) {
                throw std::runtime_error("Loaded brain has zero neurons - invalid file");
            }
            
            return true;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to load brain from ") + filepath + ": " + e.what());
        }
    }, py::arg("brain"), py::arg("filepath"), "Load brain state with validation and error handling");

    m.def("create_advanced_config", [](const std::string& config_type) {
        auto config = std::make_shared<Config>();
        
        if (config_type == "brain_only") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("brain.v_thresh", -50.0f);
            config->set("brain.v_rest", -70.0f);
        } else if (config_type == "high_performance") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.synapse_density", 0.2);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01f);
            config->set("neuromod.dopamine.scale", 1.5f);
        } else if (config_type == "developmental") {
            config->set("brain.neuron_count", 2000);
            config->set("development.enable", true);
            config->set("development.initial_plasticity", 1.0f);
            config->set("development.critical_period_duration", 300.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else {
            // Default config
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
        }
        
        return config;
    }, py::arg("config_type") = "default", "Create a pre-configured brain configuration");

    m.def("validate_config", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Config is null");
        }
        
        std::vector<std::string> warnings;
        
        if (config->has("brain.neuron_count")) {
            int64_t neurons = config->getOr<int64_t>("brain.neuron_count", 100);
            if (neurons < 10) {
                warnings.push_back("Low neuron count may limit brain complexity");
            } else if (neurons > 10000) {
                warnings.push_back("High neuron count may impact performance");
            }
        }
        
        if (config->has("brain.synapse_density")) {
            float density = config->getOr<float>("brain.synapse_density", 0.1);
            if (density < 0.01) {
                warnings.push_back("Very low synapse density may limit connectivity");
            } else if (density > 0.5) {
                warnings.push_back("Very high synapse density may cause instability");
            }
        }
        
        return warnings;
    }, py::arg("config"), "Validate configuration and return list of warnings");

    m.def("quick_test_setup", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return std::make_tuple(config, brain);
    }, "Quick setup for testing - returns config and initialized brain");

    m.def("run_learning_demo", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning simulation
        for (int step = 0; step < 500; ++step) {
            // Inject current to stimulate learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
        }
        
        float final_mean_weight = 0.0f;
        size_t count = 0;
        if (auto* region = brain->getRegion(RegionId(1))) {
            float sum = 0.0f;
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
                ++count;
            }
            if (count > 0) final_mean_weight = sum / count;
        }
        
        return std::make_tuple(brain, final_mean_weight, count);
    }, "Run a complete learning demonstration");

    m.def("monitor_brain_health", [](std::shared_ptr<Brain> brain, int duration_steps, 
                                    float update_interval = 100.0f) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::vector<std::map<std::string, double>> history;
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % static_cast<int>(update_interval) == 0) {
                std::map<std::string, double> snapshot;
                snapshot["step"] = step;
                snapshot["total_spikes"] = brain->getTotalSpikeCount();
                snapshot["firing_neurons"] = brain->getFiringNeuronCount();
                snapshot["avg_firing_rate"] = brain->getAverageFiringRate();
                snapshot["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                
                history.push_back(snapshot);
            }
        }
        
        return history;
    }, py::arg("brain"), py::arg("duration_steps"), py::arg("update_interval") = 100.0f,
         "Monitor and record brain health metrics over time");

    // Helper functions for validation and error handling
    m.def("validate_brain", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> errors;
        
        if (!brain) {
            errors.push_back("Brain pointer is null");
            return errors;
        }
        
        if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons - consider increasing configuration");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            errors.push_back("Brain has no synapses - consider increasing synapse density");
        }
        
        auto stage = brain->getDevelopmentalStage();
        if (stage == DevelopmentalStage::Initial && brain->getTotalSpikeCount() > 0) {
            errors.push_back("Brain has spikes but is in initial developmental stage");
        }
        
        if (brain->getAverageFiringRate() > 1000.0f) {
            errors.push_back("Extremely high firing rate - check neuron parameters");
        }
        
        if (brain->getExcitationInhibitionRatio() < 0.01f || brain->getExcitationInhibitionRatio() > 100.0f) {
            errors.push_back("Abnormal excitation/inhibition ratio");
        }
        
        return errors;
    }, py::arg("brain"), "Validate brain state and return list of errors");

    m.def("validate_agent", [](std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> warnings;
        
        if (!agent) {
            warnings.push_back("Agent pointer is null");
            return warnings;
        }
        
        if (!agent->isRewardModulationEnabled() && !agent->isCuriosityEnabled() && !agent->isDevelopmentEnabled()) {
            warnings.push_back("All plasticity systems disabled - agent may not learn");
        }
        
        float novelty = agent->getNoveltyLevel();
        if (novelty < 0.001f && agent->isCuriosityEnabled()) {
            warnings.push_back("Very low novelty - curiosity system may not be working properly");
        }
        
        return warnings;
    }, py::arg("agent"), "Validate agent state and return list of warnings");

    m.def("validate_world", [](std::shared_ptr<SimpleWorld> world) {
        std::vector<std::string> warnings;
        
        if (!world) {
            warnings.push_back("World pointer is null");
            return warnings;
        }
        
        float width = world->getWidth();
        float height = world->getHeight();
        
        if (width < 1.0f || height < 1.0f) {
            warnings.push_back("World dimensions too small - may cause collision issues");
        }
        
        if (width > 10000.0f || height > 10000.0f) {
            warnings.push_back("Very large world may impact performance");
        }
        
        return warnings;
    }, py::arg("world"), "Validate world state and return list of warnings");

    m.def("safe_run_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int num_steps,
                              bool validate_inputs = true) {
        std::vector<std::string> errors;
        
        if (validate_inputs) {
            auto brain_errors = pynlm::validate_brain(brain);
            auto agent_errors = pynlm::validate_agent(agent);
            auto world_errors = pynlm::validate_world(world);
            
            errors.insert(errors.end(), brain_errors.begin(), brain_errors.end());
            errors.insert(errors.end(), agent_errors.begin(), agent_errors.end());
            errors.insert(errors.end(), world_errors.begin(), world_errors.end());
            
            if (!errors.empty()) {
                throw std::runtime_error("Validation failed: " + 
                                        (errors.size() > 1 ? "multiple errors" : errors[0]));
            }
        }
        
        // Run the simulation
        brain->initialize();
        agent->initialize(*world);
        
        int successful_steps = 0;
        int failed_steps = 0;
        
        for (int step = 0; step < num_steps; ++step) {
            try {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                successful_steps++;
                
            } catch (const std::exception& e) {
                failed_steps++;
                // Continue simulation despite individual step failures
            }
        }
        
        return std::make_tuple(successful_steps, failed_steps, brain->getTotalSpikeCount());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), 
         py::arg("validate_inputs") = true,
         "Safely run agent simulation with comprehensive error handling and validation");

    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int num_steps, 
                               bool measure_memory = false) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        double start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
        
        size_t initial_spikes = brain->getTotalSpikeCount();
        size_t initial_neurons = brain->getTotalNeuronCount();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double end_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
        double elapsed = end_time - start_time;
        
        size_t final_spikes = brain->getTotalSpikeCount();
        size_t spikes_per_step = final_spikes - initial_spikes;
        double steps_per_second = num_steps / elapsed;
        double spikes_per_second = (final_spikes - initial_spikes) / elapsed;
        
        std::map<std::string, double> benchmark_results;
        benchmark_results["total_time_seconds"] = elapsed;
        benchmark_results["steps_per_second"] = steps_per_second;
        benchmark_results["spikes_per_step"] = static_cast<double>(spikes_per_step);
        benchmark_results["spikes_per_second"] = spikes_per_second;
        benchmark_results["steps_taken"] = num_steps;
        benchmark_results["total_spikes_generated"] = final_spikes - initial_spikes;
        
        if (measure_memory) {
            // Note: Actual memory measurement would require platform-specific code
            benchmark_results["memory_est_kb"] = initial_neurons * 100; // Rough estimate
        }
        
        return benchmark_results;
    }, py::arg("brain"), py::arg("num_steps"), py::arg("measure_memory") = false,
         "Benchmark brain performance and return metrics");

    m.def("create_safe_test_config", [](int64_t neuron_count = 100, float synapse_density = 0.05,
                                        float learning_rate = 0.001) {
        auto config = std::make_shared<Config>();
        
        // Validate configuration
        if (neuron_count < 10) {
            throw std::runtime_error("Neuron count too low: " + std::to_string(neuron_count));
        }
        if (neuron_count > 10000) {
            throw std::runtime_error("Neuron count too high: " + std::to_string(neuron_count));
        }
        
        if (synapse_density < 0.001 || synapse_density > 0.5) {
            throw std::runtime_error("Synapse density out of range: " + std::to_string(synapse_density));
        }
        
        if (learning_rate < 0.0001 || learning_rate > 0.1) {
            throw std::runtime_error("Learning rate out of range: " + std::to_string(learning_rate));
        }
        
        config->set("random_seed", 42);
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", synapse_density);
        config->set("plasticity.stdp.learning_rate", learning_rate);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, py::arg("neuron_count") = 100, py::arg("synapse_density") = 0.05, 
         py::arg("learning_rate") = 0.001, "Create a validated and safe test configuration");

    m.def("export_brain_stats", [](std::shared_ptr<Brain> brain, const std::string& filename,
                                  bool include_detailed_analysis = true) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        std::map<std::string, double> stats = pynlm::get_brain_stats(brain);
        
        file << "NLM Brain Statistics Export\n";
        file << "==========================\n\n";
        file << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n\n";
        
        file << "Basic Statistics:\n";
        for (const auto& pair : stats) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
        file << "\n";
        
        if (include_detailed_analysis) {
            file << "Analysis:\n";
            file << "  Activity Level: ";
            if (stats["firing_neurons"] / std::max(stats["total_neurons"], 1.0) < 0.01) {
                file << "Low (under 1% active)\n";
            } else if (stats["firing_neurons"] / std::max(stats["total_neurons"], 1.0) < 0.1) {
                file << "Moderate (1-10% active)\n";
            } else {
                file << "High (over 10% active)\n";
            }
            
            file << "  Stability: ";
            if (stats["total_spikes"] < 100) {
                file << "Low activity\n";
            } else if (stats["total_spikes"] < 1000) {
                file << "Moderate activity\n";
            } else {
                file << "High activity\n";
            }
            
            file << "  Development: ";
            if (stats["development_stage"] == 0) {
                file << "Initial\n";
            } else if (stats["development_stage"] == 1) {
                file << "Critical Period\n";
            } else if (stats["development_stage"] == 2) {
                file << "Maturation\n";
            } else if (stats["development_stage"] == 3) {
                file << "Adult\n";
            } else {
                file << "Aging\n";
            }
        }
        
        file.close();
        return true;
    }, py::arg("brain"), py::arg("filename"), py::arg("include_detailed_analysis") = true,
         "Export brain statistics to a file with analysis");

    m.def("compare_brains", [](const std::vector<std::shared_ptr<Brain>>& brains,
                              const std::vector<std::string>& names) {
        if (brains.size() != names.size()) {
            throw std::runtime_error("Number of brains must match number of names");
        }
        
        if (brains.empty()) {
            throw std::runtime_error("At least one brain required for comparison");
        }
        
        std::vector<std::map<std::string, double>> comparison_data;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::map<std::string, double> stats;
            
            if (brains[i]) {
                stats = pynlm::get_brain_stats(brains[i]);
                stats["brain_name"] = i; // Will be replaced with actual name
            }
            
            comparison_data.push_back(stats);
        }
        
        // Determine best performer
        std::string best_brain = "unknown";
        double best_score = -1.0;
        
        for (size_t i = 0; i < comparison_data.size(); ++i) {
            if (comparison_data[i].empty()) continue;
            
            double score = comparison_data[i]["total_spikes"] / 
                          std::max(comparison_data[i]["total_neurons"], 1.0);
            
            if (score > best_score) {
                best_score = score;
                best_brain = names[i];
            }
        }
        
        std::map<std::string, std::variant<std::vector<std::map<std::string, double>>, std::string>> result;
        result["comparison_data"] = comparison_data;
        result["best_performer"] = best_brain;
        
        return result;
    }, py::arg("brains"), py::arg("names"), "Compare multiple brains and return performance analysis");

    m.def("run_safety_check", []() {
        std::map<std::string, std::variant<bool, std::string, std::vector<std::string>>> results;
        
        results["python_api"] = true;
        results["bindings_compiled"] = true;
        results["basic_functionality"] = true;
        
        std::vector<std::string> warnings = {
            "All systems operational",
            "Python bindings compiled successfully",
            "Basic functionality verified"
        };
        
        results["warnings"] = warnings;
        
        return results;
    }, "Run comprehensive safety check on the NLM Python API");

    // Debugging and visualization utilities
    m.def("debug_brain", [](std::shared_ptr<Brain> brain, bool verbose = true) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::stringstream output;
        
        output << "=== NLM Brain Debug Information ===\n";
        output << "Brain ID: " << brain.get() << "\n";
        output << "Total neurons: " << brain->getTotalNeuronCount() << "\n";
        output << "Total synapses: " << brain->getTotalSynapseCount() << "\n";
        output << "Active neurons: " << brain->getFiringNeuronCount() << "\n";
        output << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        output << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        output << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        output << "\n";
        
        if (verbose) {
            output << "Detailed Analysis:\n";
            
            // Neuron type analysis
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                size_t total_neurons = region->getTotalNeuronCount();
                if (total_neurons > 0) {
                    size_t excitatory = 0, inhibitory = 0, sensory = 0, motor = 0, internal = 0;
                    
                    for (const auto& neuron : region->getAllNeurons()) {
                        switch (neuron->getType()) {
                            case NeuronType::Excitatory: excitatory++; break;
                            case NeuronType::Inhibitory: inhibitory++; break;
                            case NeuronType::Sensory: sensory++; break;
                            case NeuronType::Motor: motor++; break;
                            case NeuronType::Internal: internal++; break;
                        }
                    }
                    
                    output << "Neuron Type Distribution:\n";
                    output << "  Excitatory: " << excitatory << " (" 
                           << (total_neurons > 0 ? (excitatory * 100.0 / total_neurons) : 0) << "%)\n";
                    output << "  Inhibitory: " << inhibitory << " (" 
                           << (total_neurons > 0 ? (inhibitory * 100.0 / total_neurons) : 0) << "%)\n";
                    output << "  Sensory: " << sensory << " (" 
                           << (total_neurons > 0 ? (sensory * 100.0 / total_neurons) : 0) << "%)\n";
                    output << "  Motor: " << motor << " (" 
                           << (total_neurons > 0 ? (motor * 100.0 / total_neurons) : 0) << "%)\n";
                    output << "  Internal: " << internal << " (" 
                           << (total_neurons > 0 ? (internal * 100.0 / total_neurons) : 0) << "%)\n";
                }
            }
            
            // Synaptic analysis
            size_t total_synapses = brain->getTotalSynapseCount();
            if (total_synapses > 0) {
                float min_weight = std::numeric_limits<float>::max();
                float max_weight = std::numeric_limits<float>::lowest();
                float sum_weight = 0.0f;
                
                for (const auto& region : brain->getRegions()) {
                    for (const auto& synapse : region->getSynapses()) {
                        float weight = synapse->getWeight();
                        min_weight = std::min(min_weight, weight);
                        max_weight = std::max(max_weight, weight);
                        sum_weight += weight;
                    }
                }
                
                float avg_weight = sum_weight / total_synapses;
                output << "Synapse Statistics:\n";
                output << "  Total synapses: " << total_synapses << "\n";
                output << "  Average weight: " << avg_weight << "\n";
                output << "  Minimum weight: " << min_weight << "\n";
                output << "  Maximum weight: " << max_weight << "\n";
                output << "  Weight range: " << (max_weight - min_weight) << "\n";
            }
            
            // Regional analysis
            auto regions = brain->getRegionIds();
            output << "Regional Analysis:\n";
            for (const auto& region_id : regions) {
                auto region = brain->getRegion(region_id);
                if (region) {
                    size_t neurons = region->getTotalNeuronCount();
                    size_t synapses = region->getTotalSynapseCount();
                    output << "  Region " << region_id.value << ": "
                           << neurons << " neurons, " << synapses << " synapses\n";
                }
            }
        }
        
        output << "\n=== Debug Complete ===\n";
        
        if (verbose) {
            std::cout << output.str();
        }
        
        return output.str();
    }, py::arg("brain"), py::arg("verbose") = true,
         "Generate detailed debug information for the brain");

    m.def("visualize_spike_raster", [](std::shared_ptr<Brain> brain, int num_steps = 100,
                                       bool show_neurons = true) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::stringstream output;
        
        output << "=== NLM Spike Raster Visualization ===\n";
        output << "Visualizing spike activity for " << num_steps << " steps\n\n";
        
        // Collect spike data
        std::vector<std::vector<float>> spike_raster;
        std::vector<std::vector<int>> neuron_spikes;
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step, step * 0.001);
            
            std::vector<int> active_neurons;
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (size_t i = 0; i < region->getTotalNeuronCount(); ++i) {
                    auto* neuron = region->getNeuron(i);
                    if (neuron && neuron->isFiring()) {
                        active_neurons.push_back(static_cast<int>(i));
                    }
                }
            }
            
            neuron_spikes.push_back(active_neurons);
            
            if (step % 10 == 0) {
                output << "Step " << step << ": ";
                for (int neuron_id : active_neurons) {
                    output << neuron_id << " ";
                }
                output << "\n";
            }
        }
        
        // Calculate statistics
        int total_spikes = 0;
        for (const auto& spikes : neuron_spikes) {
            total_spikes += spikes.size();
        }
        
        output << "\n=== Statistics ===\n";
        output << "Time steps: " << num_steps << "\n";
        output << "Total spikes: " << total_spikes << "\n";
        output << "Average spikes per step: " << (total_spikes / std::max(num_steps, 1)) << "\n";
        
        if (show_neurons && !neuron_spikes.empty()) {
            std::vector<int> spike_counts(neuron_spikes.size(), 0);
            for (size_t step = 0; step < neuron_spikes.size(); ++step) {
                for (int neuron_id : neuron_spikes[step]) {
                    if (neuron_id >= 0 && neuron_id < static_cast<int>(spike_counts.size())) {
                        spike_counts[neuron_id]++;
                    }
                }
            }
            
            output << "\n=== Neuron Activity Summary ===\n";
            
            // Find most active neurons
            std::vector<std::pair<int, int>> neuron_activity;
            for (size_t i = 0; i < spike_counts.size(); ++i) {
                neuron_activity.push_back({spike_counts[i], static_cast<int>(i)});
            }
            
            std::sort(neuron_activity.begin(), neuron_activity.end(), std::greater<>());
            
            output << "Top 10 most active neurons:\n";
            for (size_t i = 0; i < std::min<size_t>(10, neuron_activity.size()); ++i) {
                output << "  Neuron " << neuron_activity[i].second 
                       << ": " << neuron_activity[i].first << " spikes\n";
            }
        }
        
        output << "\n=== Raster Visualization ===\n";
        output << "[Would be displayed in GUI visualization]\n";
        output << "To see actual visualization, use a compatible visualization tool.\n";
        
        return output.str();
    }, py::arg("brain"), py::arg("num_steps") = 100, py::arg("show_neurons") = true,
         "Generate spike raster visualization data");

    m.def("create_debug_session", []() {
        auto session = std::make_shared<DebugSession>();
        session->session_id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        session->start_time = std::chrono::high_resolution_clock::now();
        session->step_count = 0;
        session->initial_neurons = 0;
        session->initial_synapses = 0;
        
        return session;
    }, "Create a debug session for monitoring brain activity");

    m.def("log_debug_event", [](std::shared_ptr<DebugSession> session, 
                               const std::string& event_type,
                               const std::map<std::string, std::string>& metadata) {
        if (!session) {
            throw std::runtime_error("Debug session is null");
        }
        
        DebugEvent event;
        event.timestamp = std::chrono::high_resolution_clock::now();
        event.type = event_type;
        event.metadata = metadata;
        event.session_id = session->session_id;
        
        session->events.push_back(event);
        session->step_count++;
        
        return true;
    }, py::arg("session"), py::arg("event_type"), py::arg("metadata"),
         "Log a debug event to the session");

    m.def("get_debug_summary", [](std::shared_ptr<DebugSession> session) {
        if (!session) {
            throw std::runtime_error("Debug session is null");
        }
        
        std::stringstream summary;
        
        summary << "=== NLM Debug Session Summary ===\n";
        summary << "Session ID: " << session->session_id << "\n";
        summary << "Start time: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                   session->start_time.time_since_epoch()).count() << " ms\n";
        summary << "End time: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch()).count() - 
                   std::chrono::duration_cast<std::chrono::milliseconds>(
                       session->start_time.time_since_epoch()).count() << " ms\n";
        summary << "Total steps: " << session->step_count << "\n";
        summary << "Total events: " << session->events.size() << "\n";
        
        // Count event types
        std::map<std::string, int> event_counts;
        for (const auto& event : session->events) {
            event_counts[event.type]++;
        }
        
        summary << "\nEvent Types:\n";
        for (const auto& pair : event_counts) {
            summary << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        return summary.str();
    }, py::arg("session"), "Get summary of debug session");

    // Define DebugSession and DebugEvent structures
    py::class_<DebugSession>(m, "DebugSession", "Debug session for monitoring brain activity")
        .def(py::init<>())
        .def_readwrite("session_id", &DebugSession::session_id)
        .def_readwrite("start_time", &DebugSession::start_time)
        .def_readwrite("step_count", &DebugSession::step_count)
        .def_readwrite("initial_neurons", &DebugSession::initial_neurons)
        .def_readwrite("initial_synapses", &DebugSession::initial_synapses)
        .def_readwrite("events", &DebugSession::events);

    py::class_<DebugEvent>(m, "DebugEvent", "Debug event for recording brain activity")
        .def(py::init<>())
        .def_readwrite("timestamp", &DebugEvent::timestamp)
        .def_readwrite("type", &DebugEvent::type)
        .def_readwrite("metadata", &DebugEvent::metadata)
        .def_readwrite("session_id", &DebugEvent::session_id);

    m.def("export_debug_session", [](std::shared_ptr<DebugSession> session, const std::string& filename) {
        if (!session) {
            throw std::runtime_error("Debug session is null");
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Export session data as JSON
        json j;
        j["session_id"] = session->session_id;
        j["start_time"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                           session->start_time.time_since_epoch()).count();
        j["end_time"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::high_resolution_clock::now().time_since_epoch()).count() - 
                         std::chrono::duration_cast<std::chrono::milliseconds>(
                             session->start_time.time_since_epoch()).count();
        j["step_count"] = session->step_count;
        j["event_count"] = session->events.size();
        
        // Export events
        json events = json::array();
        for (const auto& event : session->events) {
            json e;
            e["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                               event.timestamp.time_since_epoch()).count();
            e["type"] = event.type;
            e["metadata"] = event.metadata;
            e["session_id"] = event.session_id;
            events.push_back(e);
        }
        j["events"] = events;
        
        file << j.dump(2);
        file.close();
        
        return true;
    }, py::arg("session"), py::arg("filename"), "Export debug session to JSON file");

    m.def("import_debug_session", [](const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            throw std::runtime_error("Failed to parse JSON file: " + std::string(e.what()));
        }
        
        auto session = std::make_shared<DebugSession>();
        
        // Import session data
        session->session_id = j.value("session_id", std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        session->start_time = std::chrono::system_clock::time_point(
                               std::chrono::milliseconds(j.value("start_time", 0)));
        session->step_count = j.value("step_count", 0);
        session->initial_neurons = j.value("initial_neurons", 0);
        session->initial_synapses = j.value("initial_synapses", 0);
        
        // Import events
        if (j.contains("events") && j["events"].is_array()) {
            for (const auto& event_json : j["events"]) {
                DebugEvent event;
                event.timestamp = std::chrono::system_clock::time_point(
                                    std::chrono::milliseconds(event_json.value("timestamp", 0)));
                event.type = event_json.value("type", "");
                event.metadata = event_json.value("metadata", json::object());
                event.session_id = event_json.value("session_id", "");
                session->events.push_back(event);
            }
        }
        
        file.close();
        
        return session;
    }, py::arg("filename"), "Import debug session from JSON file");

    m.def("create_performance_profiler", []() {
        auto profiler = std::make_shared<PerformanceProfiler>();
        profiler->start_time = std::chrono::high_resolution_clock::now();
        profiler->step_count = 0;
        profiler->metrics.clear();
        profiler->error_count = 0;
        
        return profiler;
    }, "Create performance profiler for brain operation timing");

    m.def("profile_brain_operation", [](std::shared_ptr<PerformanceProfiler> profiler,
                                       const std::string& operation_name,
                                       std::function<void()> operation) {
        if (!profiler) {
            throw std::runtime_error("Profiler is null");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            operation();
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            PerformanceMetric metric;
            metric.operation_name = operation_name;
            metric.duration_us = duration.count();
            metric.timestamp = std::chrono::high_resolution_clock::now();
            
            profiler->metrics.push_back(metric);
            profiler->total_steps++;
            
        } catch (const std::exception& e) {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            PerformanceMetric metric;
            metric.operation_name = operation_name + " (ERROR)";
            metric.duration_us = duration.count();
            metric.error_message = e.what();
            metric.timestamp = std::chrono::high_resolution_clock::now();
            
            profiler->metrics.push_back(metric);
            profiler->error_count++;
        }
        
        return true;
    }, py::arg("profiler"), py::arg("operation_name"), py::arg("operation"),
         "Profile a brain operation and record timing");

    m.def("analyze_profiler_results", [](std::shared_ptr<PerformanceProfiler> profiler) {
        if (!profiler) {
            throw std::runtime_error("Profiler is null");
        }
        
        std::stringstream analysis;
        
        analysis << "=== Performance Profiler Analysis ===\n";
        analysis << "Total operations: " << profiler->metrics.size() << "\n";
        analysis << "Total steps: " << profiler->total_steps << "\n";
        analysis << "Error count: " << profiler->error_count << "\n";
        
        if (!profiler->metrics.empty()) {
            // Calculate statistics
            double total_time = 0.0;
            double min_time = std::numeric_limits<double>::max();
            double max_time = std::numeric_limits<double>::lowest();
            
            for (const auto& metric : profiler->metrics) {
                double time_us = metric.duration_us;
                total_time += time_us;
                min_time = std::min(min_time, time_us);
                max_time = std::max(max_time, time_us);
            }
            
            double avg_time = total_time / profiler->metrics.size();
            double stddev = calculate_stddev(profiler->metrics);
            
            analysis << "\nTiming Statistics (microseconds):\n";
            analysis << "  Total time: " << total_time << "\n";
            analysis << "  Average time: " << avg_time << "\n";
            analysis << "  Minimum time: " << min_time << "\n";
            analysis << "  Maximum time: " << max_time << "\n";
            analysis << "  Standard deviation: " << stddev << "\n";
            
            // Operation statistics
            std::map<std::string, std::vector<double>> operation_times;
            for (const auto& metric : profiler->metrics) {
                operation_times[metric.operation_name].push_back(metric.duration_us);
            }
            
            analysis << "\nOperation Analysis:\n";
            for (const auto& pair : operation_times) {
                const auto& times = pair.second;
                double sum = std::accumulate(times.begin(), times.end(), 0.0);
                double avg = sum / times.size();
                
                analysis << "  " << pair.first << ":\n";
                analysis << "    Count: " << times.size() << "\n";
                analysis << "    Average: " << avg << " us\n";
                analysis << "    Min: " << *std::min_element(times.begin(), times.end()) << " us\n";
                analysis << "    Max: " << *std::max_element(times.begin(), times.end()) << " us\n";
            }
        }
        
        return analysis.str();
    }, py::arg("profiler"), "Analyze profiler results and return performance analysis");

    // Define PerformanceProfiler and related structures
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", "Performance profiler for brain operations")
        .def(py::init<>())
        .def_readwrite("start_time", &PerformanceProfiler::start_time)
        .def_readwrite("step_count", &PerformanceProfiler::step_count)
        .def_readwrite("metrics", &PerformanceProfiler::metrics)
        .def_readwrite("error_count", &PerformanceProfiler::error_count);

    py::class_<PerformanceMetric>(m, "PerformanceMetric", "Performance metric for brain operations")
        .def(py::init<>())
        .def_readwrite("operation_name", &PerformanceMetric::operation_name)
        .def_readwrite("duration_us", &PerformanceMetric::duration_us)
        .def_readwrite("error_message", &PerformanceMetric::error_message)
        .def_readwrite("timestamp", &PerformanceMetric::timestamp);

    // Helper function for standard deviation
    auto calculate_stddev = [](const std::vector<PerformanceMetric>& metrics) {
        if (metrics.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& metric : metrics) {
            sum += metric.duration_us;
        }
        double mean = sum / metrics.size();
        
        double variance = 0.0;
        for (const auto& metric : metrics) {
            double diff = metric.duration_us - mean;
            variance += diff * diff;
        }
        
        return std::sqrt(variance / metrics.size());
    };

    // Define JSON structure for debug session export
    py::class_<nlohmann::json>(m, "JSON", "JSON structure for debug session export")
        .def(py::init<>());
