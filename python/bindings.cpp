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

// Add comprehensive error handling and validation exceptions
    class PyNLMException : public std::runtime_error {
    public:
        explicit PyNLMException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class InvalidArgumentException : public PyNLMException {
    public:
        explicit InvalidArgumentException(const std::string& msg) : PyNLMException("InvalidArgument: " + msg) {}
    };
    
    class ConfigurationException : public PyNLMException {
    public:
        explicit ConfigurationException(const std::string& msg) : PyNLMException("Configuration: " + msg) {}
    };
    
    class InitializationException : public PyNLMException {
    public:
        explicit InitializationException(const std::string& msg) : PyNLMException("Initialization: " + msg) {}
    };

    // Convenience factory functions with validation
    m.def("createBrain", []() {
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw InitializationException("Failed to initialize brain");
            }
            return brain;
        } catch (const std::exception& e) {
            throw PyNLMException(std::string("Failed to create brain: ") + e.what());
        }
    }, "Create a brain with default configuration. Automatically initializes it.");

    m.def("createBrainWithConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw InvalidArgumentException("Config cannot be null");
        }
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw InitializationException("Failed to initialize brain with provided config");
            }
            return brain;
        } catch (const std::exception& e) {
            throw PyNLMException(std::string("Failed to create brain: ") + e.what());
        }
    }, "Create a brain with custom configuration. Automatically initializes it.")
        .def("createDefaultConfig", []() {
            return std::make_shared<Config>();
        }, "Create a default configuration object.");

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
        .def("initialize", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InitializationException("Brain cannot be null");
            }
            if (brain->initialize()) {
                return true;
            } else {
                throw InitializationException("Failed to initialize brain");
            }
        }, "Initialize the brain with configuration. Returns True on success.")
        .def("step", [](std::shared_ptr<Brain> brain, SimulationStep currentStep) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->step(currentStep);
        }, py::arg("currentStep"),
              "Perform a simulation step")
        .def("stepWithTime", [](std::shared_ptr<Brain> brain, SimulationStep currentStep, Timestamp currentTime) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->step(currentStep, currentTime);
        }, py::arg("currentStep"), py::arg("currentTime"),
              "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](std::shared_ptr<Brain> brain, const SensoryInput& input) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->receiveSensoryInput(input);
        }, py::arg("input"),
              "Inject sensory input into the brain")
        .def("injectCurrent", [](std::shared_ptr<Brain> brain, NeuronId neuron, MembranePotential current) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
              "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](std::shared_ptr<Brain> brain, NeuronType type, MembranePotential current) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
              "Inject current into all neurons of a specific type")
        .def("produceAction", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->produceAction();
        }, "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->reset();
        }, "Reset brain state")
        .def("save", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            if (brain->save(filepath)) {
                return true;
            } else {
                throw PyNLMException("Failed to save brain to file: " + filepath);
            }
        }, py::arg("filepath"), "Save brain state to file. Returns True on success.")
        .def("load", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            if (brain->load(filepath)) {
                return true;
            } else {
                throw PyNLMException("Failed to load brain from file: " + filepath);
            }
        }, py::arg("filepath"), "Load brain state from file. Returns True on success.")
        .def("addRegion", [](std::shared_ptr<Brain> brain, const std::string& name = "") {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->addRegion(name);
        }, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", [](std::shared_ptr<Brain> brain, RegionId id) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            auto* region = brain->getRegion(id);
            if (!region) {
                throw PyNLMException("Region not found: " + std::to_string(id.value()));
            }
            return region;
        }, py::arg("id"),
              "Get a region by ID. Raises exception if region not found.")
        .def("getRegionCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getRegionCount();
        }, "Get the number of regions")
        .def("getRegionIds", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getRegionIds();
        }, "Get all region IDs")
        .def("getRegions", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getRegions();
        }, "Get all regions")
        .def("getTotalNeuronCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getTotalNeuronCount();
        }, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getTotalSynapseCount();
        }, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getActiveNeuronCount();
        }, "Get count of active neurons")
        .def("getFiringNeuronCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getFiringNeuronCount();
         }, "Get count of currently firing neurons")
        .def("getAverageFiringRate", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getAverageFiringRate();
        }, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getExcitationInhibitionRatio();
        }, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getTotalSpikeCount();
        }, "Get total spike count")
        .def("getDevelopmentalStage", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getDevelopmentalStage();
        }, "Get current developmental stage")
        .def("setDevelopmentalStage", [](std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->setDevelopmentalStage(stage);
        }, py::arg("stage"), "Set developmental stage")
        .def("getConfig", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getConfig();
        }, "Get the configuration")
        .def("getRandomGenerator", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return brain->getRandomGenerator();
        }, "Get random generator")
        .def("logStatus", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            brain->logStatus();
        }, "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](std::shared_ptr<AgentBrain> agent, const SimpleWorld& world) {
            if (!agent) {
                throw InitializationException("AgentBrain cannot be null");
            }
            agent->initialize(world);
        }, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getSensoryInputSize();
        }, "Get expected sensory input size")
        .def("getMotorOutputSize", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getMotorOutputSize();
        }, "Get expected motor output size")
        .def("processSensoryInput", [](std::shared_ptr<AgentBrain> agent, const SensoryPercept& percept) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->processSensoryInput(percept);
        }, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->decodeMotorCommand();
        }, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](std::shared_ptr<AgentBrain> agent, float reward, float predictedReward) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](std::shared_ptr<AgentBrain> agent, double timestep) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->updateDevelopment(timestep);
        }, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getDevelopmentalStage();
        }, "Get current developmental stage")
        .def("getNeuromodulationLevel", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getNeuromodulationLevel();
        }, "Get current neuromodulation level")
        .def("getCuriosityLevel", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getCuriosityLevel();
        }, "Get curiosity level")
        .def("getNoveltyLevel", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getNoveltyLevel();
        }, "Get novelty level")
        .def("getPredictionError", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getPredictionError();
        }, "Get prediction error")
        .def("reset", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->reset();
        }, "Reset agent for new episode")
        .def("getBrain", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->getBrain();
        }, "Get the underlying brain")
        .def("enableRewardModulation", [](std::shared_ptr<AgentBrain> agent, bool enable) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->enableRewardModulation(enable);
        }, py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", [](std::shared_ptr<AgentBrain> agent, bool enable) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->enableStructuralPlasticity(enable);
        }, py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", [](std::shared_ptr<AgentBrain> agent, bool enable) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->enableDevelopment(enable);
        }, py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", [](std::shared_ptr<AgentBrain> agent, bool enable) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            agent->enableCuriosity(enable);
        }, py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->isRewardModulationEnabled();
        }, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->isStructuralPlasticityEnabled();
        }, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", [](std::shared_ptr<AgentBrain> agent) {
            if (!agent) {
                throw InvalidArgumentException("AgentBrain cannot be null");
            }
            return agent->isDevelopmentEnabled();
        }, "Check if development is enabled")
         }, "Check if curiosity is enabled");

    // Add additional convenience functions for Python API
    m.def("simulateEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw InvalidArgumentException("brain, world, and agent cannot be null");
        }
        if (numSteps <= 0) {
            throw InvalidArgumentException("numSteps must be positive");
        }
        
        try {
            for (int step = 0; step < numSteps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get percept and process it
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Brain thinks
                brain->step(step);
                
                // Get action and execute it
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            return true;
        } catch (const std::exception& e) {
            throw PyNLMException(std::string("Episode simulation failed: ") + e.what());
        }
    }, "Run a complete simulation episode. Returns True on success.");

    // Error checking utility function
    auto errorCheck = [](bool condition, const std::string& message) {
        if (!condition) {
            throw PyNLMException(message);
        }
    };

    m.def("runMultipleSimulations", [](std::function<std::shared_ptr<Brain>()> brainFactory, 
                                      std::function<std::shared_ptr<SimpleWorld>()> worldFactory,
                                      int numSimulations, int stepsPerSimulation) {
        std::vector<std::shared_ptr<Brain>> brains;
        std::vector<std::shared_ptr<SimpleWorld>> worlds;
        
        try {
            for (int sim = 0; sim < numSimulations; ++sim) {
                auto brain = brainFactory();
                auto world = worldFactory();
                auto agent = std::make_shared<AgentBrain>(brain);
                agent->initialize(*world);
                
                brains.push_back(brain);
                worlds.push_back(world);
                
                // Run episode
                for (int step = 0; step < stepsPerSimulation; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
            return brains;
        } catch (const std::exception& e) {
            throw PyNLMException(std::string("Multiple simulation failed: ") + e.what());
        }
    }, "Run multiple independent simulations with automatic brain/world creation.");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
