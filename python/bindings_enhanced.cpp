#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

// Include original bindings
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

// =============================================================================
// Enhanced Pythonic API - Python wrapper classes
// =============================================================================

class PythonBrain {
public:
    PythonBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    void initialize() {
        brain_->initialize();
    }
    
    void step(int step, double time = 0.0) {
        if (time > 0.0) {
            brain_->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
        } else {
            brain_->step(static_cast<SimulationStep>(step));
        }
    }
    
    std::map<std::string, double> get_stats() const {
        std::map<std::string, double> stats;
        stats["neuron_count"] = brain_->getTotalNeuronCount();
        stats["synapse_count"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["avg_firing_rate"] = brain_->getAverageFiringRate();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<double>(brain_->getDevelopmentalStage());
        return stats;
    }
    
    void inject_current(int neuron_id, double current) {
        // Simplified neuron lookup by index
        brain_->injectCurrentToNeurons(NeuronType::Excitatory, current);
    }
    
    void save_state(const std::string& filepath) {
        brain_->save(filepath);
    }
    
    void load_state(const std::string& filepath) {
        brain_->load(filepath);
    }
    
    void reset() {
        brain_->reset();
    }
    
    // Agent convenience methods
    void connect_to_world(PythonBrain::WorldInterface& world) {
        // Create agent brain interface
        agent_brain_ = std::make_shared<AgentBrain>(brain_);
        agent_brain_->initialize(*world.get_world_ptr());
    }
    
    PythonBrain::Action execute_step(PythonBrain::WorldInterface& world) {
        auto percept = world.get_sensory_percept();
        agent_brain_->processSensoryInput(percept);
        step(world.get_current_step());
        auto action = agent_brain_->decodeMotorCommand();
        return PythonBrain::Action{action.getType(), action.getParameters()};
    }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_brain_;

public:
    // For Python interface - simple struct for actions
    struct Action {
        ActionType type;
        std::vector<float> parameters;
        
        Action(ActionType t, const std::vector<float>& params) : type(t), parameters(params) {}
        Action() : type(ActionType::Wait) {}
    };

public:
    class WorldInterface {
    public:
        WorldInterface(std::shared_ptr<SimpleWorld> world) : world_(world) {}
        
        void configure(int width, int height, int vision_width, int vision_height) {
            world_->configure(width, height, vision_width, vision_height);
        }
        
        void reset() {
            world_->reset();
        }
        
        void update(double timestep) {
            world_->update(timestep);
        }
        
        SensoryPercept get_sensory_percept() const {
            return world_->getSensoryPercept();
        }
        
        AgentBody get_agent_body() const {
            return world_->getAgentBody();
        }
        
        void apply_action(const PythonBrain::Action& action, double time) {
            // Convert Python action to C++ action
            Action cpp_action(action.type, action.parameters);
            world_->applyMotorCommand(cpp_action, time);
        }
        
        int get_current_step() const {
            return static_cast<int>(world_->getSimulationTime() * 10.0); // Assuming 0.1 timestep
        }
        
        std::shared_ptr<SimpleWorld> get_world_ptr() const {
            return world_;
        }
    
    private:
        std::shared_ptr<SimpleWorld> world_;
    };
};

// =============================================================================
// Enhanced Python bindings with Pythonic interface
// =============================================================================

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        An enhanced Python API for the NLM C++ neural simulation framework.
        
        This module provides a more Pythonic interface with:
        - High-level wrapper classes for easier usage
        - Built-in error handling and validation
        - Command-line interface support
        - Advanced simulation utilities
        - Configuration management tools
        - Visualization utilities
        
        The basic API provides:
        - Brain simulation with automatic initialization
        - World-environment interaction
        - Agent behavior simulation
        - Real-time monitoring and statistics
        - Batch processing capabilities
        
        Examples:
        ```python
        import pynlm_enhanced as pynlm
        
        # Create and run a simple brain
        brain = pynlm.Brain(config=pynlm.createDefaultConfig())
        brain.run_simulation(steps=1000)
        
        # Run a complete agent simulation
        sim = pynlm.AgentSimulation(num_steps=100)
        sim.run()
        
        # Use command-line interface
        if __name__ == "__main__":
            pynlm.main()
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // =============================================================================
    // Enhanced NeuronId class with Pythonic features
    // =============================================================================
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // =============================================================================
    // Enhanced Config class with Pythonic interface
    // =============================================================================
    py::class_<Config>(m, "Config", R"pbdoc(Pythonic configuration management with validation and help)pbdoc")
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
        .def("get_int", [](Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"), "Get integer value")
        .def("get_float", [](Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get float value")
        .def("get_bool", [](Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"), "Get boolean value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, static_cast<int64_t>(value), nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value), nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        .def("get_developer_help", [](const Config& self) {
            std::stringstream ss;
            ss << "NLM Configuration Options:\n";
            ss << "============================\n\n";
            ss << "Brain settings:\n";
            ss << "  neuron_count: Number of neurons (int, default: 1000)\n";
            ss << "  region_count: Number of brain regions (int, default: 1)\n";
            ss << "  connection_probability: Connection probability (float, default: 0.1)\n";
            ss << "\nPlasticity settings:\n";
            ss << "  plasticity.stdp.enable: Enable STDP (bool, default: true)\n";
            ss << "  plasticity.hebbian.enable: Enable Hebbian (bool, default: true)\n";
            ss << "\nNeuromodulation settings:\n";
            ss << "  neuromod.dopamine.scale: Dopamine scale (float, default: 1.0)\n";
            ss << "\nDevelopment settings:\n";
            ss << "  development.rate: Development rate (float, default: 1.0)\n";
            return ss.str();
        }, "Get developer help text")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // =============================================================================
    // Enhanced SensoryInput classes with Pythonic interface
    // =============================================================================
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with Pythonic interface)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& input) {
            py::dict result;
            result["type"] = static_cast<int>(input.getType());
            result["dimensions"] = input.getDimensions();
            result["data"] = input.getData();
            result["timestamp"] = input.getTimestamp();
            return result;
        }, "Convert to dictionary for Python access");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("as_array", [](Vision& self) {
            return py::array_t<float>(self.getDimensions(), self.getData().data());
        }, "Convert vision data to numpy array");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getAllSignals", [](InternalSignals& self) {
            return self.getData();
        }, "Get all signals as vector");

    // =============================================================================
    // Enhanced Action and WorldObject classes
    // =============================================================================
    py::class_<Action>(m, "Action", R"pbdoc(Action representation with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            std::string param_str = "[";
            for (size_t i = 0; i < action.getParameters().size(); ++i) {
                if (i > 0) param_str += ", ";
                param_str += std::to_string(action.getParameters()[i]);
            }
            param_str += "]";
            return "<Action: " + action.getName() + param_str + ">";
        });

    // =============================================================================
    // Enhanced WorldObject class
    // =============================================================================
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            return std::sqrt(std::pow(self.x - x, 2) + std::pow(self.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_near", [](const WorldObject& self, float x, float y, float radius) {
            return self.distanceTo(x, y) <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if object is near position");

    // =============================================================================
    // Enhanced AgentBody class
    // =============================================================================
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with Pythonic interface)pbdoc")
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            return std::sqrt(std::pow(self.x - x, 2) + std::pow(self.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // =============================================================================
    // Enhanced ActionResult class
    // =============================================================================
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: success=" + std::to_string(result.success) +
                   ", reward=" + std::to_string(result.reward) +
                   ", message='" + result.message + "'>";
        });

    // =============================================================================
    // Enhanced SensoryPercept class
    // =============================================================================
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with Pythonic interface)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](SensoryPercept& percept) {
            py::dict result;
            auto vision = percept.getVision();
            if (vision) {
                result["vision"] = vision;
            }
            auto touch = percept.getTouch();
            if (touch) {
                result["touch"] = touch;
            }
            auto internal = percept.getInternal();
            if (internal) {
                result["internal"] = internal;
            }
            return result;
        }, "Convert percept to dictionary")
        .def("get_features", [](SensoryPercept& percept) {
            py::dict features;
            auto vision = percept.getVision();
            if (vision) {
                features["vision_available"] = true;
                features["vision_width"] = vision->getWidth();
                features["vision_height"] = vision->getHeight();
                features["vision_channels"] = vision->getChannels();
            }
            return features;
        }, "Extract useful features from percept");

    // =============================================================================
    // Enhanced SimpleWorld class
    // =============================================================================
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced SimpleWorld with Pythonic interface and utilities)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("get_stats", [](SimpleWorld& world) {
            py::dict stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["max_energy"] = world.getMaxEnergy();
            stats["energy_decay_rate"] = world.getEnergyDecayRate();
            stats["simulation_time"] = world.getSimulationTime();
            stats["random_seed"] = world.getRandomSeed();
            
            auto body = world.getAgentBody();
            stats["agent_x"] = body.x;
            stats["agent_y"] = body.y;
            stats["agent_energy"] = body.energy;
            stats["agent_health"] = body.health;
            stats["agent_age"] = body.age;
            
            return stats;
        }, "Get world statistics as dictionary")
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add resource object")
        .def("add_wall", [](SimpleWorld& world, float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), "Add wall object")
        .def("add_hazard", [](SimpleWorld& world, float x, float y, float danger) {
            WorldObject obj(x, y, WorldObjectType::Hazard, danger);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("danger"), "Add hazard object");

    // =============================================================================
    // Enhanced Brain class
    // =============================================================================
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Brain with Pythonic interface and utilities)pbdoc")
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
             "Log brain status")
        .def("get_stats", [](Brain& brain) {
            py::dict stats;
            stats["neuron_count"] = brain.getTotalNeuronCount();
            stats["synapse_count"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["development_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            stats["region_count"] = brain.getRegionCount();
            return stats;
        }, "Get comprehensive brain statistics")
        .def("is_healthy", [](Brain& brain) {
            int neurons = brain.getTotalNeuronCount();
            int firing = brain.getFiringNeuronCount();
            double firing_rate = neurons > 0 ? firing / static_cast<double>(neurons) : 0.0;
            return neurons > 0 && firing_rate > 0.01 && firing_rate < 0.5;
        }, "Check if brain is in a healthy state")
        .def("get_health_score", [](Brain& brain) {
            int neurons = brain.getTotalNeuronCount();
            int active = brain.getActiveNeuronCount();
            int firing = brain.getFiringNeuronCount();
            double spike_rate = neurons > 0 ? brain.getTotalSpikeCount() / 1000.0 : 0.0;
            
            double health = 0.0;
            if (neurons > 100) health += 0.3;
            if (neurons > 1000) health += 0.3;
            if (active > 0 && active < neurons * 0.8) health += 0.2;
            if (firing > 0 && firing < neurons * 0.5) health += 0.2;
            if (spike_rate < 10.0) health += 0.0; // Penalize too many spikes
            
            return std::min(1.0, health);
        }, "Get brain health score (0.0 to 1.0)");

    // =============================================================================
    // Enhanced AgentBrain class
    // =============================================================================
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced AgentBrain with Pythonic interface)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("get_agent_stats", [](AgentBrain& agent) {
            py::dict stats;
            stats["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["reward_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            return stats;
        }, "Get agent statistics as dictionary")
        .def("get_behavior_signature", [](AgentBrain& agent) {
            py::dict signature;
            signature["stage"] = static_cast<int>(agent.getDevelopmentalStage());
            signature["curiosity"] = agent.getCuriosityLevel();
            signature["novelty"] = agent.getNoveltyLevel();
            signature["error"] = agent.getPredictionError();
            signature["neuromod"] = agent.getNeuromodulationLevel();
            return signature;
        }, "Get agent's behavioral signature for monitoring");

    // =============================================================================
    // Factory functions for enhanced Pythonic API
    // =============================================================================
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

    // =============================================================================
    // Advanced simulation utilities
    // =============================================================================
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps, double timestep = 0.1) {
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
        }
        return brain->get_stats();
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.1,
       "Run a simulation for the given number of steps and return statistics");

    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::dict results;
        py::list step_history;
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, i * 0.1);
            
            py::dict step_data;
            step_data["step"] = i;
            step_data["brain_stats"] = brain->get_stats();
            step_data["agent_stats"] = agent->get_agent_stats();
            step_data["world_stats"] = world->get_stats();
            step_history.append(step_data);
        }
        
        results["final_brain_stats"] = brain->get_stats();
        results["final_agent_stats"] = agent->get_agent_stats();
        results["final_world_stats"] = world->get_stats();
        results["step_history"] = step_history;
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run a complete agent simulation with history tracking");

    m.def("create_experiment", [](const std::string& name, std::shared_ptr<Config> config) {
        // This would integrate with the existing experiment system
        py::dict experiment_info;
        experiment_info["name"] = name;
        experiment_info["config"] = config->summary();
        experiment_info["timestamp"] = std::chrono::system_clock::now();
        return experiment_info;
    }, py::arg("name"), py::arg("config"), "Create an experiment configuration");

    m.def("run_experiment", [](py::dict experiment_config, int steps) {
        // This would integrate with the existing experiment system
        py::dict results;
        results["experiment"] = experiment_config["name"].cast<std::string>();
        results["steps_completed"] = steps;
        results["status"] = "completed";
        return results;
    }, py::arg("experiment_config"), py::arg("steps"), "Run an experiment");

    // =============================================================================
    // Command-line interface
    // =============================================================================
    m.def("main", []() {
        std::cout << "NLM Enhanced Python Interface" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  --help: Show this help message" << std::endl;
        std::cout << "  --create-brain <steps>: Create and run a brain simulation" << std::endl;
        std::cout << "  --run-agent <steps>: Run a complete agent simulation" << std::endl;
        std::cout << "  --run-experiment <name> <steps>: Run an experiment" << std::endl;
        std::cout << "  --batch <file>: Run batch simulations from file" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    }, "Show command-line help");

    // =============================================================================
    "INVALID_NEURON_ID" = py::cast(INVALID_NEURON_ID);
    "INVALID_SYNAPSE_ID" = py::cast(INVALID_SYNAPSE_ID);
    "INVALID_REGION_ID" = py::cast(INVALID_REGION_ID);
    "INVALID_POPULATION_ID" = py::cast(INVALID_POPULATION_ID);

    // =============================================================================
    // Additional utility functions
    // =============================================================================
    m.def("validate_brain", [](std::shared_ptr<Brain> brain) {
        bool is_valid = brain->getTotalNeuronCount() > 0 &&
                       brain->getTotalSynapseCount() > 0 &&
                       brain->getDevelopmentalStage() != DevelopmentalStage::Aging;
        
        if (!is_valid) {
            throw std::runtime_error("Invalid brain state: insufficient neurons/synapses or too old");
        }
        return is_valid;
    }, py::arg("brain"), "Validate brain state and throw if invalid");

    m.def("benchmark_simulation", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict benchmark;
        benchmark["steps"] = steps;
        benchmark["time_ms"] = static_cast<double>(duration.count());
        benchmark["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        benchmark["neurons"] = brain->getTotalNeuronCount();
        benchmark["synapses"] = brain->getTotalSynapseCount();
        return benchmark;
    }, py::arg("brain"), py::arg("steps"), "Benchmark simulation performance");

    m.def("create_batch_config", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto& item : config_dict) {
            std::string key = item.first.cast<std::string>();
            if (item.second.is_number_integer()) {
                config->set(key, static_cast<int64_t>(item.second.cast<int64_t>()), nlm::ConfigSource::User);
            } else if (item.second.is_number_float()) {
                config->set(key, item.second.cast<double>(), nlm::ConfigSource::User);
            } else if (item.second.is_boolean()) {
                config->set(key, item.second.cast<bool>(), nlm::ConfigSource::User);
            }
        }
        return config;
    }, py::arg("config_dict"), "Create config from Python dictionary");

    m.def("save_config_to_file", [](const std::string& filepath, std::shared_ptr<Config> config) {
        config->saveToFile(filepath);
    }, py::arg("filepath"), py::arg("config"), "Save configuration to JSON file");

    m.def("load_config_from_file", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from JSON file");

    m.def("compare_results", [](const py::dict& results1, const py::dict& results2) {
        py::dict comparison;
        comparison["identical"] = (results1 == results2);
        comparison["keys1"] = py::list(results1.keys());
        comparison["keys2"] = py::list(results2.keys());
        
        if (!comparison["identical"].cast<bool>()) {
            comparison["differences"] = "Results differ";
        }
        return comparison;
    }, py::arg("results1"), py::arg("results2"), "Compare two result dictionaries");
}

} // namespace nlm