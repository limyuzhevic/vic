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

    // Convenience function for running a complete simulation
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                    std::shared_ptr<AgentBrain> agent, int num_steps, bool verbose = false) {
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << "Simulation completed successfully!" << std::endl;
            std::cout << "Final statistics:" << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("verbose") = false,
       "Run a complete agent simulation with the given brain, world, and agent");

    // Convenience function for creating a brain with custom configuration
    m.def("create_brain_with_config", [](const std::string& config_file) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_file)) {
            throw std::runtime_error("Failed to load config file: " + config_file);
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_file"), "Create a brain with configuration loaded from file");

    // Convenience function for running learning experiments
    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain, int steps, bool enable_plasticity = true) {
        if (enable_plasticity) {
            // Enable plasticity on all synapses
            for (const auto& region : brain->getRegions()) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
                }
            }
        }
        
        // Run the experiment
        for (int step = 0; step < steps; ++step) {
            // Inject sensory-like input to stimulate learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
        
        std::cout << "Learning experiment completed!" << std::endl;
    }, py::arg("brain"), py::arg("steps"), py::arg("enable_plasticity") = true,
       "Run a learning experiment with the given brain");

    // Convenience function for brain analysis and statistics
    m.def("analyze_brain", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== Brain Analysis ===" << std::endl;
        std::cout << "Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        std::cout << "\nPer Region:" << std::endl;
        for (const auto& region : brain->getRegions()) {
            std::cout << "  " << region->getName() << ": "
                     << region->getTotalNeuronCount() << " neurons, "
                     << region->getSynapseCount() << " synapses, "
                     << "avg weight: " << region->getAverageSynapticWeight() << std::endl;
        }
    }, py::arg("brain"), "Analyze and print brain statistics");

    // Convenience function for running a simulation with checkpointing
    m.def("run_simulation_with_checkpoints", [](std::shared_ptr<Brain> brain, int steps, 
                                               const std::string& checkpoint_prefix = "checkpoint") {
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Save checkpoint every 100 steps
            if (step % 100 == 0) {
                std::string checkpoint_file = checkpoint_prefix + "_step_" + std::to_string(step) + ".bin";
                brain->save(checkpoint_file);
                std::cout << "Checkpoint saved: " << checkpoint_file << std::endl;
            }
        }
        
        // Final checkpoint
        brain->save(checkpoint_prefix + "_final.bin");
        std::cout << "Simulation completed with checkpoints" << std::endl;
    }, py::arg("brain"), py::arg("steps"), py::arg("checkpoint_prefix") = "checkpoint",
       "Run simulation with periodic checkpointing");

    // Convenience function for comparing two brains
    m.def("compare_brains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        std::cout << "=== Brain Comparison ===" << std::endl;
        std::cout << "Neuron Count - Brain 1: " << brain1->getTotalNeuronCount() << 
                 ", Brain 2: " << brain2->getTotalNeuronCount() << std::endl;
        std::cout << "Synapse Count - Brain 1: " << brain1->getTotalSynapseCount() << 
                 ", Brain 2: " << brain2->getTotalSynapseCount() << std::endl;
        std::cout << "Total Spikes - Brain 1: " << brain1->getTotalSpikeCount() << 
                 ", Brain 2: " << brain2->getTotalSpikeCount() << std::endl;
        std::cout << "Average Firing Rate - Brain 1: " << brain1->getAverageFiringRate() << 
                 ", Brain 2: " << brain2->getAverageFiringRate() << std::endl;
        std::cout << "E/I Ratio - Brain 1: " << brain1->getExcitationInhibitionRatio() << 
                 ", Brain 2: " << brain2->getExcitationInhibitionRatio() << std::endl;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and print statistics");

    m.def("create_brain_with_config", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a brain with configuration");

    // Advanced simulation control
    m.def("run_simulation_with_control", [](std::shared_ptr<Brain> brain, int steps, double timestep = 0.001,
                                           const std::vector<std::function<bool(int)>>& event_handlers = {}) {
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            // Execute event handlers
            for (const auto& handler : event_handlers) {
                if (handler(step)) {
                    break; // Stop on first true handler
                }
            }
        }
        
        return "Simulation completed: " + std::to_string(steps) + " steps at " + std::to_string(timestep) + "s timestep";
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       py::arg("event_handlers") = std::vector<std::function<bool(int)>>(),
       "Run simulation with advanced control options and event handlers");

    // Advanced memory management and inspection
    m.def("inspect_memory_systems", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== Memory System Inspection ===" << std::endl;
        
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Active traces: " << workingMem->getActiveTraces() << std::endl;
            std::cout << "  Capacity: " << workingMem->getCapacity() << std::endl;
            std::cout << "  Load: " << (workingMem->getActiveTraces() / workingMem->getCapacity() * 100) << "%" << std::endl;
        }
        
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            std::cout << "Episodic Memory:" << std::endl;
            std::cout << "  Episodes stored: " << episodicMem->getEpisodeCount() << std::endl;
            std::cout << "  Max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
            auto* recentEpisodes = episodicMem->getRecentEpisodes(5);
            if (recentEpisodes) {
                std::cout << "  Recent episodes (last 5):" << std::endl;
                for (size_t i = 0; i < 5 && recentEpisodes[i]; ++i) {
                    std::cout << "    Episode " << i << ": " << recentEpisodes[i]->activeNeurons.size() << " active neurons" << std::endl;
                }
            }
        }
        
        auto* assocMem = brain->getAssociativeMemory();
        if (assocMem) {
            std::cout << "Associative Memory:" << std::endl;
            std::cout << "  Associations stored: " << assocMem->getAssociationCount() << std::endl;
        }
    }, py::arg("brain"), "Advanced inspection of all memory systems");

    // Advanced statistics and analysis
    m.def("analyze_brain_advanced", [](std::shared_ptr<Brain> brain, bool include_detailed = true) {
        std::cout << "=== Advanced Brain Analysis ===" << std::endl;
        
        // Basic statistics
        std::cout << "Basic Statistics:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        if (include_detailed) {
            // Regional analysis
            std::cout << "\nRegional Analysis:" << std::endl;
            for (const auto& region : brain->getRegions()) {
                std::cout << "  " << region->getName() << ":" << std::endl;
                std::cout << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
                std::cout << "    Synapses: " << region->getSynapseCount() << std::endl;
                std::cout << "    Active: " << region->getActiveNeuronCount() << std::endl;
                std::cout << "    Firing: " << region->getFiringNeuronCount() << std::endl;
                std::cout << "    Average weight: " << region->getAverageSynapticWeight() << std::endl;
                
                // Population breakdown
                std::cout << "    Populations:" << std::endl;
                for (size_t i = 0; i < region->getPopulationCount(); ++i) {
                    auto* pop = region->getPopulation(PopulationId(i + 1));
                    if (pop) {
                        std::cout << "      " << pop->getNeuronType() << ": " << pop->getNeuronCount() << " neurons" << std::endl;
                    }
                }
            }
        }
    }, py::arg("brain"), py::arg("include_detailed") = true,
       "Comprehensive brain analysis with optional detailed regional breakdown");

    // Advanced configuration management
    m.def("configure_brain_advanced", [](std::shared_ptr<Brain> brain, const std::string& config_str) {
        std::cout << "=== Advanced Brain Configuration ===" << std::endl;
        std::cout << "Current configuration:" << std::endl;
        
        auto config = brain->getConfig();
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (config->has(key)) {
                std::cout << "  " << key << ": " << config->summary() << std::endl;
            }
        }
        
        std::cout << "\nNote: Use Brain.getConfig() for detailed configuration access in Python";
    }, py::arg("brain"), py::arg("config_str") = "",
       "Advanced brain configuration and inspection");

    // Performance monitoring and profiling
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, int monitor_steps = 100) {
        std::cout << "=== Performance Monitoring ===" << std::endl;
        std::cout << "Monitoring " << monitor_steps << " simulation steps..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < monitor_steps; ++step) {
            brain->step(step);
            
            if (step % 10 == 0) {
                auto current_time = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
                double steps_per_second = (step + 1) / (elapsed / 1000.0);
                
                std::cout << "  Step " << step << ": " << steps_per_second << " steps/sec, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        double avg_performance = monitor_steps / (total_time / 1000.0);
        
        std::cout << "\nPerformance Summary:" << std::endl;
        std::cout << "  Total time: " << total_time << " ms" << std::endl;
        std::cout << "  Average performance: " << avg_performance << " steps/sec" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }, py::arg("brain"), py::arg("monitor_steps") = 100,
       "Monitor brain performance over a specified number of steps");

    // Advanced scripting and automation
    m.def("run_automation_script", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& commands) {
        std::cout << "=== Automation Script Execution ===" << std::endl;
        
        for (size_t i = 0; i < commands.size(); ++i) {
            std::cout << "Command " << i + 1 << ": " << commands[i] << std::endl;
            
            // Simple command parsing for automation
            if (commands[i] == "step_once") {
                brain->step(brain->getPendingSpikeEventCount() + 1);
            } else if (commands[i].find("step_") == 0 && commands[i].size() > 5) {
                // Extract step number
                try {
                    int step = std::stoi(commands[i].substr(5));
                    brain->step(step);
                } catch (const std::exception& e) {
                    std::cout << "  Error: Invalid step command" << std::endl;
                }
            } else if (commands[i] == "reset") {
                brain->reset();
            } else if (commands[i].find("save_") == 0 && commands[i].size() > 5) {
                std::string filepath = commands[i].substr(5);
                if (!brain->save(filepath)) {
                    std::cout << "  Error: Failed to save to " << filepath << std::endl;
                }
            } else if (commands[i].find("load_") == 0 && commands[i].size() > 5) {
                std::string filepath = commands[i].substr(5);
                if (!brain->load(filepath)) {
                    std::cout << "  Error: Failed to load from " << filepath << std::endl;
                }
            } else if (commands[i] == "stats") {
                brain->logStatus();
            } else {
                std::cout << "  Unknown command (use: step_once, step_<n>, reset, save_<file>, load_<file>, stats)" << std::endl;
            }
        }
        
        std::cout << "\nAutomation script completed!";
    }, py::arg("brain"), py::arg("commands") = std::vector<std::string>(),
       "Run automation script with simple command parsing");

    // Multi-agent coordination (basic)
    m.def("create_agent_manager", [](std::vector<std::shared_ptr<Brain>> brains,
                                    std::vector<std::shared_ptr<SimpleWorld>> worlds,
                                    std::vector<std::shared_ptr<AgentBrain>> agents) {
        std::cout << "=== Agent Manager Creation ===" << std::endl;
        std::cout << "Created agent manager with " << brains.size() << " agents" << std::endl;
        
        // Basic agent manager for coordinating multiple agents
        class AgentManager {
        public:
            std::vector<std::shared_ptr<Brain>> brains;
            std::vector<std::shared_ptr<SimpleWorld>> worlds;
            std::vector<std::shared_ptr<AgentBrain>> agents;
            
            AgentManager(std::vector<std::shared_ptr<Brain>> b, std::vector<std::shared_ptr<SimpleWorld>> w,
                        std::vector<std::shared_ptr<AgentBrain>> a)
                : brains(b), worlds(w), agents(a) {}
                
            void run_synchronized_simulation(int steps, double timestep = 0.1) {
                std::cout << "Running synchronized simulation with " << brains.size() << " agents..." << std::endl;
                
                for (int step = 0; step < steps; ++step) {
                    std::cout << "Step " << step << ": ";
                    
                    for (size_t i = 0; i < brains.size(); ++i) {
                        worlds[i]->update(timestep);
                        
                        auto percept = worlds[i]->getSensoryPercept();
                        agents[i]->processSensoryInput(percept);
                        
                        brains[i]->step(step);
                        
                        auto motor_cmd = agents[i]->decodeMotorCommand();
                        worlds[i]->applyMotorCommand(motor_cmd, worlds[i]->getSimulationTime());
                        
                        if (i < brains.size() - 1) std::cout << ", ";
                    }
                    
                    std::cout << std::endl;
                }
                
                std::cout << "Synchronized simulation completed!" << std::endl;
            }
        };
        
        return std::make_shared<AgentManager>(brains, worlds, agents);
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"),
       "Create agent manager for coordinating multiple agents");

    // Advanced batch operations
    m.def("batch_process_brains", [](const std::vector<std::string>& brain_files, 
                                   const std::string& output_prefix,
                                   int steps_per_brain = 100) {
        std::cout << "=== Batch Brain Processing ===" << std::endl;
        std::cout << "Processing " << brain_files.size() << " brain files..." << std::endl;
        
        int processed = 0;
        int successful = 0;
        
        for (const auto& filepath : brain_files) {
            processed++;
            std::cout << "Processing " << processed << "/" << brain_files.size() << ": " << filepath << "..." << std::endl;
            
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                std::cout << "  Failed to load config: " << filepath << std::endl;
                continue;
            }
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cout << "  Failed to initialize brain" << std::endl;
                continue;
            }
            
            // Run simulation
            for (int step = 0; step < steps_per_brain; ++step) {
                brain->step(step);
            }
            
            // Save results
            std::string output_file = output_prefix + "_processed_" + 
                                    std::to_string(successful + 1) + ".bin";
            if (brain->save(output_file)) {
                successful++;
                std::cout << "  Saved to: " << output_file << std::endl;
            } else {
                std::cout << "  Failed to save results" << std::endl;
            }
        }
        
        std::cout << "\nBatch processing completed: " << successful << "/" << processed << " successful" << std::endl;
    }, py::arg("brain_files"), py::arg("output_prefix"), py::arg("steps_per_brain") = 100,
       "Batch process multiple brains and save results");

    // Experimental setup and management
    m.def("setup_experiment", [](const std::string& experiment_name,
                                 std::shared_ptr<Brain> brain,
                                 const std::map<std::string, std::string>& parameters) {
        std::cout << "=== Experiment Setup ===" << std::endl;
        std::cout << "Experiment: " << experiment_name << std::endl;
        std::cout << "Parameters:" << std::endl;
        
        for (const auto& param : parameters) {
            std::cout << "  " << param.first << ": " << param.second << std::endl;
        }
        
        // Apply experiment parameters to brain configuration
        auto config = brain->getConfig();
        for (const auto& param : parameters) {
            // Parse parameter type and set appropriate config value
            if (param.first.find("neuron_count") != std::string::npos) {
                try {
                    size_t count = std::stoul(param.second);
                    config->set("neuron_count", count);
                } catch (...) {
                    std::cout << "  Warning: Invalid parameter format for " << param.first << std::endl;
                }
            } else if (param.first.find("plasticity") != std::string::npos) {
                // Handle plasticity parameters
                config->set(param.first, std::stof(param.second));
            }
        }
        
        // Re-initialize brain with new parameters
        brain->reset();
        brain->initialize();
        
        std::cout << "\nExperiment " << experiment_name << " setup complete!" << std::endl;
        
        return "Experiment " + experiment_name + " ready for execution";
    }, py::arg("experiment_name"), py::arg("brain"), py::arg("parameters") = std::map<std::string, std::string>(),
       "Setup and configure an experiment with the given parameters");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
