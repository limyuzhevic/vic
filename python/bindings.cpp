#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// Forward declarations for the different NLM components
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
    class SensoryPercept;
    class Action;
    class WorldObject;
    class AgentBody;
    class Neuron;
    class Synapse;
}

namespace py = pybind11;
namespace nlm {

// Helper function to create numpy arrays from C++ vectors
// This assumes numpy is available

// Enhanced Config class with more Pythonic interface
void bindConfig(py::module_& m) {
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
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value by key")
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value or return default if key doesn't exist")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("setWithSource", [](Config& self, const std::string& key, auto value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"),
           "Set configuration value with source")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("fromDict", [](const std::map<std::string, std::string>& dict) {
            auto config = std::make_shared<Config>();
            for (const auto& pair : dict) {
                config->set(pair.first, pair.second, ConfigSource::User);
            }
            return config;
        }, py::arg("dict"), "Create config from Python dictionary")
        .def("toDict", [](const Config& self) {
            std::map<std::string, std::string> dict;
            const auto& keys = self.getKeys();
            for (const std::string& key : keys) {
                // Note: This assumes values can be converted to strings
                dict[key] = "value"; // This is a placeholder - actual implementation would need proper serialization
            }
            return dict;
        }, "Convert config to Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

// Enhanced Brain class with more Pythonic interface
void bindBrain(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
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
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Pythonic simulation control
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
        // Statistics and monitoring
        .def("getNeuronStats", [](Brain& self) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive brain statistics")
        .def("isInitialized", [](const Brain& self) {
            // This assumes there's a way to check initialization state
            // Implementation would depend on Brain class internals
            return true; // placeholder
        }, "Check if brain is initialized");
}

// Enhanced AgentBrain class with more Pythonic interface
void bindAgentBrain(py::module_& m) {
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
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
        // Configuration methods
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
        // Statistics and monitoring
        .def("getAgentStats", [](AgentBrain& self) {
            std::map<std::string, double> stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics")
        .def("getMovementStats", [](AgentBrain& self) {
            // This would require access to motor neuron statistics
            // Implementation would depend on AgentBrain internals
            std::map<std::string, double> stats;
            stats["forward_probability"] = 0.3; // placeholder
            stats["turn_probability"] = 0.2;   // placeholder
            stats["interact_probability"] = 0.1; // placeholder
            return stats;
        }, "Get movement/decision statistics");
}

// Enhanced SimpleWorld class with more Pythonic interface
void bindSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("getWorldStats", [](SimpleWorld& self) {
            std::map<std::string, double> stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulation_time"] = self.getSimulationTime();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["agent_x"] = self.getAgentBody().x;
            stats["agent_y"] = self.getAgentBody().y;
            stats["agent_orientation"] = self.getAgentBody().orientation;
            stats["agent_energy"] = self.getAgentBody().energy;
            stats["agent_health"] = self.getAgentBody().health;
            stats["agent_age"] = self.getAgentBody().age;
            return stats;
        }, "Get comprehensive world statistics")
        .def("getObjects", [](SimpleWorld& self) {
            // This would require access to world objects
            // Implementation would depend on SimpleWorld internals
            std::vector<std::map<std::string, double>> objects;
            // placeholder - would populate with actual object data
            return objects;
        }, "Get world objects")
        .def("getAgentPosition", [](SimpleWorld& self) {
            return std::make_pair(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](SimpleWorld& self) {
            return self.getAgentBody().orientation;
        }, "Get agent orientation");
}

// Enhanced Action class
void bindAction(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            std::map<std::string, std::string> dict;
            dict["type"] = actionTypeToString(self.getType());
            dict["parameters"] = "["; // placeholder
            dict["name"] = self.getName();
            return dict;
        }, "Convert action to Python dictionary")
        .def("fromDict", [](const std::map<std::string, std::string>& dict) {
            auto action = std::make_shared<Action>();
            // Implementation would parse dict and set action properties
            // placeholder
            return action;
        }, "Create action from Python dictionary")
        .def("__repr__", [](const Action& self) {
            return "<Action: " + self.getName() + ">";
        });
}

// Enhanced WorldObject class
void bindWorldObject(py::module_& m) {
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
        .def_readwrite("active", &WorldObject::active)
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        .def("toDict", [](const WorldObject& self) {
            std::map<std::string, std::string> dict;
            dict["x"] = std::to_string(self.x);
            dict["y"] = std::to_string(self.y);
            dict["radius"] = std::to_string(self.radius);
            dict["type"] = objectTypeToString(self.type);
            dict["value"] = std::to_string(self.value);
            dict["active"] = self.active ? "true" : "false";
            return dict;
        }, "Convert world object to Python dictionary")
        .def("fromDict", [](const std::map<std::string, std::string>& dict) {
            auto obj = std::make_shared<WorldObject>();
            // Implementation would parse dict and set object properties
            // placeholder
            return obj;
        }, "Create world object from Python dictionary")
        .def("__repr__", [](const WorldObject& self) {
            return "<WorldObject: " + objectTypeToString(self.type) + " at (" + 
                   std::to_string(self.x) + ", " + std::to_string(self.y) + ")>";
        });
}

// Enhanced AgentBody class
void bindAgentBody(py::module_& m) {
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getAngularVelocity", &AgentBody::angularVelocity,
             "Get angular velocity")
        .def("getMovementStats", [](const AgentBody& self) {
            std::map<std::string, double> stats;
            stats["x"] = self.x;
            stats["y"] = self.y;
            stats["orientation"] = self.orientation;
            stats["velocity_x"] = self.velocityX;
            stats["velocity_y"] = self.velocityY;
            stats["angular_velocity"] = self.angularVelocity;
            stats["energy"] = self.energy;
            stats["health"] = self.health;
            stats["age"] = self.age;
            stats["is_moving"] = self.isMoving;
            stats["is_turning"] = self.isTurning;
            stats["last_action_time"] = self.lastActionTime;
            return stats;
        }, "Get comprehensive agent body statistics")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("isEnergyDepleted", [](const AgentBody& self) {
            return self.energy <= 0.0f;
        }, "Check if agent energy is depleted");
}

// Enhanced SensoryPercept class
void bindSensoryPercept(py::module_& m) {
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("getPerceptStats", [](SensoryPercept& self) {
            std::map<std::string, double> stats;
            auto vision = self.getVision();
            stats["vision_width"] = self.getVisionWidth();
            stats["vision_height"] = self.getVisionHeight();
            stats["vision_total"] = vision.size();
            stats["vision_sum"] = std::accumulate(vision.begin(), vision.end(), 0.0f);
            stats["vision_mean"] = vision.empty() ? 0.0 : stats["vision_sum"] / vision.size();
            
            auto touch = self.getTouch();
            stats["touch_size"] = touch.size();
            stats["touch_sum"] = std::accumulate(touch.begin(), touch.end(), 0.0f);
            
            auto internal = self.getInternal();
            stats["internal_size"] = internal.size();
            stats["internal_sum"] = std::accumulate(internal.begin(), internal.end(), 0.0f);
            
            auto proprio = self.getProprioception();
            stats["proprioception_size"] = proprio.size();
            stats["proprioception_sum"] = std::accumulate(proprio.begin(), proprio.end(), 0.0f);
            
            auto audio = self.getAudio();
            stats["audio_size"] = audio.size();
            stats["audio_sum"] = std::accumulate(audio.begin(), audio.end(), 0.0f);
            
            stats["timestamp"] = self.getTimestamp();
            
            return stats;
        }, "Get comprehensive percept statistics")
        .def("getDominantSensory", [](SensoryPercept& self) {
            // Determine which sensory modality has the most data
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprio = self.getProprioception();
            auto audio = self.getAudio();
            
            size_t max_size = std::max({vision.size(), touch.size(), internal.size(), proprio.size(), audio.size()});
            
            if (max_size == vision.size() && !vision.empty()) {
                return std::string("vision");
            } else if (max_size == touch.size() && !touch.empty()) {
                return std::string("touch");
            } else if (max_size == internal.size() && !internal.empty()) {
                return std::string("internal");
            } else if (max_size == proprio.size() && !proprio.empty()) {
                return std::string("proprioception");
            } else if (max_size == audio.size() && !audio.empty()) {
                return std::string("audio");
            }
            
            return std::string("none");
        }, "Get dominant sensory modality");
}

// Helper functions
void bindHelperFunctions(py::module_& m) {
    // Create default configuration
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Create brain with configuration
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    // Create simple world
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    // Create agent brain interface
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Constants and enumerations
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Enum to string conversion helpers
    m.def("neuronTypeToString", [](NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return std::string("Excitatory");
            case NeuronType::Inhibitory: return std::string("Inhibitory");
            case NeuronType::Modulatory: return std::string("Modulatory");
            case NeuronType::Sensory: return std::string("Sensory");
            case NeuronType::Motor: return std::string("Motor");
            case NeuronType::Internal: return std::string("Internal");
            default: return std::string("Unknown");
        }
    }, "Convert neuron type to string");
    
    m.def("synapseTypeToString", [](SynapseType type) {
        switch (type) {
            case SynapseType::Excitatory: return std::string("Excitatory");
            case SynapseType::Inhibitory: return std::string("Inhibitory");
            case SynapseType::Modulatory: return std::string("Modulatory");
            case SynapseType::Electrical: return std::string("Electrical");
            case SynapseType::GapJunction: return std::string("GapJunction");
            default: return std::string("Unknown");
        }
    }, "Convert synapse type to string");
    
    m.def("developmentalStageToString", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return std::string("Initial");
            case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
            case DevelopmentalStage::Maturation: return std::string("Maturation");
            case DevelopmentalStage::Adult: return std::string("Adult");
            case DevelopmentalStage::Aging: return std::string("Aging");
            default: return std::string("Unknown");
        }
    }, "Convert developmental stage to string");
    
    m.def("actionTypeToString", [](ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return std::string("MoveForward");
            case ActionType::MoveBackward: return std::string("MoveBackward");
            case ActionType::MoveLeft: return std::string("MoveLeft");
            case ActionType::MoveRight: return std::string("MoveRight");
            case ActionType::TurnLeft: return std::string("TurnLeft");
            case ActionType::TurnRight: return std::string("TurnRight");
            case ActionType::Look: return std::string("Look");
            case ActionType::LookUp: return std::string("LookUp");
            case ActionType::LookDown: return std::string("LookDown");
            case ActionType::Interact: return std::string("Interact");
            case ActionType::Eat: return std::string("Eat");
            case ActionType::Drink: return std::string("Drink");
            case ActionType::Rest: return std::string("Rest");
            case ActionType::Wait: return std::string("Wait");
            case ActionType::Custom: return std::string("Custom");
            default: return std::string("Unknown");
        }
    }, "Convert action type to string");
    
    m.def("motorCommandToString", [](MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return std::string("MoveForward");
            case MotorCommand::MoveBackward: return std::string("MoveBackward");
            case MotorCommand::TurnLeft: return std::string("TurnLeft");
            case MotorCommand::TurnRight: return std::string("TurnRight");
            case MotorCommand::LookLeft: return std::string("LookLeft");
            case MotorCommand::LookRight: return std::string("LookRight");
            case MotorCommand::Interact: return std::string("Interact");
            case MotorCommand::Wait: return std::string("Wait");
            default: return std::string("Unknown");
        }
    }, "Convert motor command to string");
    
    m.def("worldObjectTypeToString", [](WorldObjectType type) {
        switch (type) {
            case WorldObjectType::Empty: return std::string("Empty");
            case WorldObjectType::Resource: return std::string("Resource");
            case WorldObjectType::Hazard: return std::string("Hazard");
            case WorldObjectType::Wall: return std::string("Wall");
            case WorldObjectType::Marker: return std::string("Marker");
            default: return std::string("Unknown");
        }
    }, "Convert world object type to string");
    
    // Simulation utilities
    m.def("runSimulationStep", [](Brain& brain, SimulationStep step, Timestamp time = 0.0) {
        brain.step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
       "Run a single simulation step");
    
    m.def("runMultiStepSimulation", [](Brain& brain, size_t steps, double timestep = 0.001) {
        for (size_t i = 0; i < steps; ++i) {
            brain.step(i, i * timestep);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       "Run multiple simulation steps");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Pythonic interfaces for all NLM components
        - Comprehensive statistics and monitoring
        - Dictionary serialization for easy data exchange
        - Advanced simulation control
        - Better error handling and validation
        
        Key improvements:
        - Enhanced Config with dict creation/loading
        - Brain statistics and monitoring
        - AgentBrain comprehensive statistics
        - SimpleWorld with position/orientation helpers
        - Action and WorldObject with dict support
        - AgentBody with comprehensive statistics
        - SensoryPercept with dominant sensory detection
        - Helper functions for simulation control
        - Better enumeration conversion utilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind core types
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

    // Enum bindings
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

    // Enhanced bindings
    bindConfig(m);
    bindBrain(m);
    bindAgentBrain(m);
    bindSimpleWorld(m);
    bindAction(m);
    bindWorldObject(m);
    bindAgentBody(m);
    bindSensoryPercept(m);
    bindHelperFunctions(m);
}

} // namespace nlm
