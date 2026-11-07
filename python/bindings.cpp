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

    // Template to reduce repetition in ID class bindings
    template<typename T>
    void bindIdClass(py::module_& m, const std::string& name, const std::string& doc) {
        py::class_<T>(m, name.c_str(), doc.c_str())
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &T::value)
            .def("index", &T::index)
            .def("__eq__", &T::operator==)
            .def("__ne__", &T::operator!=)
            .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const T& id) {
                return "<" + name + ": " + std::to_string(id.value) + ">";
            });
    }

    // Bind individual ID classes using the template
    bindIdClass<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    bindIdClass<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    bindIdClass<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    bindIdClass<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");

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
        .def("getData", &Vision::getData, "Get the vision data as a vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getType", &Vision::getType, "Get the type of sensory input")
        .def("getDimensions", &Vision::getDimensions, "Get the dimensionality")
        .def("clone", &Vision::clone, "Clone the vision data")
        .def("toArray", [](const Vision& self) {
            return self.getData();
        }, "Convert vision data to Python array (convenience method)")
        .def("fromArray", [](Vision& self, py::object array) {
            // Handle numpy array or Python list
            std::vector<float> data;
            if (py::isinstance(array, py::module::get_object("numpy").attr("ndarray"))) {
                // Extract data from numpy array
                py::array_t<float> arr = array.cast<py::array_t<float>>();
                data.assign(arr.begin(), arr.end());
            } else {
                data = array.cast<std::vector<float>>();
            }
            self.setData(data);
        }, py::arg("array"), "Set vision data from Python array or list (convenience method)");

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

    // ===== ADVANCED CONVENIENCE AND BATCH METHODS =====
    
    // Batch operations for efficiency
    m.def("injectCurrentBatch", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
        for (const auto& inj : injections) {
            self.injectCurrent(inj.first, inj.second);
        }
    }, py::arg("brain"), py::arg("injections"),
         "Inject current into multiple neurons at once (batch operation)")
    .def("getComprehensiveStats", [](const Brain& self) {
        return std::vector<float>{
            static_cast<float>(self.getTotalNeuronCount()),
            static_cast<float>(self.getTotalSynapseCount()),
            static_cast<float>(self.getActiveNeuronCount()),
            static_cast<float>(self.getFiringNeuronCount()),
            self.getAverageFiringRate(),
            self.getExcitationInhibitionRatio(),
            static_cast<float>(self.getTotalSpikeCount()),
            static_cast<float>(self.getPendingSpikeEventCount()),
            self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod ? 1.0f : 0.0f
        };
    }, py::arg("brain"),
         "Get comprehensive system statistics (convenience method)")
    .def("createConfigFromDict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value_obj = item.second;
            
            if (py::isinstance(value_obj, py::int_())) {
                config->set(key, static_cast<int64_t>(value_obj.cast<int64_t>()), ConfigSource::User);
            } else if (py::isinstance(value_obj, py::float_())) {
                config->set(key, value_obj.cast<double>(), ConfigSource::User);
            } else if (py::isinstance(value_obj, py::bool_())) {
                config->set(key, value_obj.cast<bool>(), ConfigSource::User);
            } else if (py::isinstance(value_obj, py::str())) {
                config->set(key, value_obj.cast<std::string>(), ConfigSource::User);
            }
        }
        return config;
    }, py::arg("config_dict"),
         "Create configuration from Python dictionary (convenience method)")
    .def("runSimulationSteps", [](Brain& self, SimulationStep startStep, SimulationStep endStep, Timestamp timestep) {
        for (SimulationStep step = startStep; step <= endStep; ++step) {
            self.step(step, timestep * (step - startStep));
        }
    }, py::arg("brain"), py::arg("startStep"), py::arg("endStep"), py::arg("timestep"),
         "Run simulation over a range of steps (convenience method)")
    .def("getSystemActivity", [](Brain& self) {
        return std::vector<float>{
            static_cast<float>(self.getTotalNeuronCount()),
            static_cast<float>(self.getFiringNeuronCount()),
            self.getAverageFiringRate(),
            self.getExcitationInhibitionRatio(),
            static_cast<float>(self.getTotalSpikeCount()) / std::max(1.0f, static_cast<float>(self.getTotalSynapseCount()))
        };
    }, py::arg("brain"),
         "Get normalized system activity metrics (convenience method)")
    .def("enableAllSubsystems", [](AgentBrain& agent) {
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
    }, py::arg("agent"),
         "Enable all learning subsystems (convenience method)")
    .def("getNeuromodulationState", [](AgentBrain& agent) {
        return std::vector<float>{
            agent.getCuriosityLevel(),
            agent.getNoveltyLevel(),
            agent.getPredictionError(),
            agent.getNeuromodulationLevel()
        };
    }, py::arg("agent"),
         "Get neuromodulation state as vector (convenience method)")
    
    // Configuration convenience methods
    .def("getConfigValue", [](const Config& cfg, const std::string& key) {
        // Try to get common types with fallback
        if (cfg.has(key)) {
            // Try int first, then double, then string, then bool
            if (cfg.get<int>(key)) {
                return py::make_tuple("int", cfg.getOr<int>(key, 0));
            } else if (cfg.get<double>(key)) {
                return py::make_tuple("double", cfg.getOr<double>(key, 0.0));
            } else if (cfg.get<bool>(key)) {
                return py::make_tuple("bool", cfg.getOr<bool>(key, false));
            } else if (cfg.get<std::string>(key)) {
                return py::make_tuple("string", cfg.getOr<std::string>(key, ""));
            }
        }
        throw py::key_error("Key '" + key + "' not found in configuration");
    }, py::arg("key"),
         "Get configuration value with type detection (returns tuple of (type, value))")
    
    // Simulation convenience methods
    .def("runSimulation", [](Brain& self, SimulationStep startStep, SimulationStep endStep, Timestamp timestep) {
        for (SimulationStep step = startStep; step <= endStep; ++step) {
            self.step(step, timestep * (step - startStep));
        }
    }, py::arg("startStep"), py::arg("endStep"), py::arg("timestep"),
         "Run simulation over a range of steps (convenience method)")
    .def("stepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
        self.step(step, time);
    }, py::arg("step"), py::arg("time"),
         "Perform simulation step with specific time (convenience method)")
    
    // Debug and diagnostic methods
    .def("getDebugInfo", &Brain::logStatus,
         "Get debug information (same as logStatus but returns empty string)")
    .def("getSystemStats", [](const Brain& self) {
        return std::vector<float>{
            static_cast<float>(self.getTotalNeuronCount()),
            static_cast<float>(self.getTotalSynapseCount()),
            static_cast<float>(self.getActiveNeuronCount()),
            static_cast<float>(self.getFiringNeuronCount()),
            self.getAverageFiringRate(),
            self.getExcitationInhibitionRatio(),
            static_cast<float>(self.getTotalSpikeCount()),
            static_cast<float>(self.getPendingSpikeEventCount())
        };
    }, "Get system statistics as a vector of floats (convenience method)")
    
    // Memory system access convenience methods
    .def("getWorkingMemory", [](Brain& self) {
        return self.getWorkingMemory();
    }, "Get working memory system (convenience method)")
    .def("getEpisodicMemory", [](Brain& self) {
        return self.getEpisodicMemory();
    }, "Get episodic memory system (convenience method)")
    .def("getAssociativeMemory", [](Brain& self) {
        return self.getAssociativeMemory();
    }, "Get associative memory system (convenience method)")
    .def("getPredictionSystem", [](Brain& self) {
        return self.getPredictionSystem();
    }, "Get prediction system (convenience method)")
    .def("getPlanner", [](Brain& self) {
        return self.getPlanner();
    }, "Get neural planner (convenience method)")
    .def("getConceptFormation", [](Brain& self) {
        return self.getConceptFormation();
    }, "Get concept formation system (convenience method)")
    .def("getAttention", [](Brain& self) {
        return self.getAttention();
    }, "Get attentional selection system (convenience method)")
    .def("getDevelopmentSystem", [](Brain& self) {
        return self.getDevelopmentSystem();
    }, "Get development system (convenience method)")
    .def("getDopamine", [](Brain& self) {
        return self.getDopamine();
    }, "Get dopamine neuromodulator (convenience method)")
    .def("getCuriosity", [](Brain& self) {
        return self.getCuriosity();
    }, "Get curiosity neuromodulator (convenience method)")
    .def("getNovelty", [](Brain& self) {
        return self.getNovelty();
    }, "Get novelty neuromodulator (convenience method)")
    .def("getPredictionErrorSignal", [](Brain& self) {
        return self.getPredictionErrorSignal();
    }, "Get prediction error signal (convenience method)")
    .def("getRandomGenerator", [](Brain& self) {
        return self.getRandomGenerator();
    }, "Get random number generator (convenience method)")
    
    // Legacy compatibility methods
    .def("getNeuronCount", &Brain::getTotalNeuronCount,
         "Get total neuron count (legacy alias for getTotalNeuronCount)")
    .def("getSynapseCount", &Brain::getTotalSynapseCount,
         "Get total synapse count (legacy alias for getTotalSynapseCount)")
    .def("getActiveNeurons", &Brain::getActiveNeuronCount,
         "Get active neuron count (legacy alias for getActiveNeuronCount)")
    .def("getFiringNeurons", &Brain::getFiringNeuronCount,
         "Get firing neuron count (legacy alias for getFiringNeuronCount)")
    .def("getAverageFiring", &Brain::getAverageFiringRate,
         "Get average firing rate (legacy alias for getAverageFiringRate)")
    .def("getTotalSpikes", &Brain::getTotalSpikeCount,
         "Get total spike count (legacy alias for getTotalSpikeCount)")
    .def("getE/I_Ratio", &Brain::getExcitationInhibitionRatio,
         "Get excitation/inhibition ratio (legacy alias for getExcitationInhibitionRatio)")
    .def("getRegionCount", &Brain::getRegionCount,
         "Get region count (legacy alias for getRegionCount)");

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

    // ===== ADVANCED AGENT BRAIN CONVENIENCE METHODS =====
    
    // Advanced agent configuration and setup
    m.def("setupLearningAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, const py::dict& config) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Apply learning configurations
        if (config.contains("reward_modulation")) {
            agent->enableRewardModulation(config["reward_modulation"].cast<bool>());
        }
        if (config.contains("structural_plasticity")) {
            agent->enableStructuralPlasticity(config["structural_plasticity"].cast<bool>());
        }
        if (config.contains("development")) {
            agent->enableDevelopment(config["development"].cast<bool>());
        }
        if (config.contains("curiosity")) {
            agent->enableCuriosity(config["curiosity"].cast<bool>());
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("config") = py::dict(),
         "Setup an agent brain with learning configurations (convenience method)")
    .def("createExperimentSetup", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableAllSubsystems(); // Convenience method to enable all learning
        return std::make_pair(brain, agent);
    }, py::arg("brain"), py::arg("world"),
         "Create complete experiment setup with brain and agent (convenience method)")
    .def("simulateEpisode", [](AgentBrain& agent, SimpleWorld& world, SimulationStep steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            agent.updateDevelopment(0.1); // Fixed timestep for agent development
        }
        return std::make_tuple(agent.getBrain()->getTotalSpikeCount(), 
                              agent.getCuriosityLevel());
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
         "Run a complete simulation episode (convenience method)")
    .def("batchProcessPercepts", [](AgentBrain& agent, const std::vector<SensoryPercept>& percepts) {
        for (const auto& percept : percepts) {
            agent.processSensoryInput(percept);
        }
    }, py::arg("agent"), py::arg("percepts"),
         "Process multiple percepts in batch (convenience method)")
    .def("batchApplyRewards", [](AgentBrain& agent, const std::vector<float>& rewards) {
        float cumulative_reward = 0.0f;
        for (const auto& reward : rewards) {
            cumulative_reward += reward;
            agent.applyRewardModulation(reward, 0.0f);
        }
        return cumulative_reward;
    }, py::arg("agent"), py::arg("rewards"),
         "Apply multiple rewards and return cumulative reward (convenience method)")
    .def("getAdvancedStats", [](AgentBrain& agent) {
        return std::vector<float>{
            agent.getCuriosityLevel(),
            agent.getNoveltyLevel(),
            agent.getPredictionError(),
            agent.getNeuromodulationLevel(),
            agent.isRewardModulationEnabled() ? 1.0f : 0.0f,
            agent.isStructuralPlasticityEnabled() ? 1.0f : 0.0f,
            agent.isDevelopmentEnabled() ? 1.0f : 0.0f,
            agent.isCuriosityEnabled() ? 1.0f : 0.0f
        };
    }, py::arg("agent"),
         "Get comprehensive advanced agent statistics (convenience method)")
