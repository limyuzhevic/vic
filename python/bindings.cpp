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

    // Add Python-specific configuration loading methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("loadFromDict", [](Config& self, const py::dict& py_dict) {
            // Convert Python dict to config
            for (auto item : py_dict) {
                std::string key = py::str(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    self.set(key, item.second.cast<int64_t>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    self.set(key, item.second.cast<double>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    self.set(key, item.second.cast<bool>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str())) {
                    self.set(key, item.second.cast<std::string>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::list())) {
                    // Handle lists
                    py::list py_list = item.second.cast<py::list>();
                    std::vector<double> vec;
                    for (auto elem : py_list) {
                        vec.push_back(elem.cast<double>());
                    }
                    self.set(key, vec, ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("config_dict"),
           "Load configuration from a Python dictionary")
        .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) -> py::object {
            if (self.has(key)) {
                return py::cast(self.get<py::object>(key).value());
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value") = py::none(),
           "Get a configuration value with optional default")
        .def("getInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("default_value"),
             "Get an integer configuration value")
        .def("getFloat", &Config::getOr<double>, py::arg("key"), py::arg("default_value"),
             "Get a floating-point configuration value")
        .def("getBool", &Config::getOr<bool>, py::arg("key"), py::arg("default_value"),
             "Get a boolean configuration value")
        .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("default_value"),
             "Get a string configuration value")
        .def("set", [](Config& self, const py::dict& py_dict) {
            for (auto item : py_dict) {
                std::string key = py::str(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    self.set(key, item.second.cast<int64_t>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    self.set(key, item.second.cast<double>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    self.set(key, item.second.cast<bool>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str())) {
                    self.set(key, item.second.cast<std::string>(), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::list())) {
                    // Handle lists
                    py::list py_list = item.second.cast<py::list>();
                    std::vector<double> vec;
                    for (auto elem : py_list) {
                        vec.push_back(elem.cast<double>());
                    }
                    self.set(key, vec, ConfigSource::Runtime);
                }
            }
        }, py::arg("config_dict"),
           "Set configuration from a Python dictionary")
        .def("toDict", [](const Config& self) {
            py::dict py_dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value_opt = self.get<py::object>(key);
                if (value_opt) {
                    py_dict[key.c_str()] = *value_opt;
                }
            }
            return py_dict;
        }, "Convert configuration to Python dictionary")
        .def("validate", [](const Config& self) -> bool {
            bool valid = true;
            
            // Required keys
            std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
            
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    std::cerr << "ERROR: Missing required configuration key: " << key << std::endl;
                    valid = false;
                }
            }
            
            // Value validation
            if (self.has("neuron_count")) {
                auto count = self.getOr<int64_t>("neuron_count", 0);
                if (count <= 0) {
                    std::cerr << "ERROR: neuron_count must be positive (got: " << count << ")" << std::endl;
                    valid = false;
                }
            }
            
            if (self.has("region_count")) {
                auto count = self.getOr<int64_t>("region_count", 0);
                if (count <= 0) {
                    std::cerr << "ERROR: region_count must be positive (got: " << count << ")" << std::endl;
                    valid = false;
                }
            }
            
            if (self.has("simulation_timestep")) {
                auto timestep = self.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0) {
                    std::cerr << "ERROR: simulation_timestep must be positive (got: " << timestep << ")" << std::endl;
                    valid = false;
                }
            }
            
            return valid;
        }, "Validate configuration and return True if valid")
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
             "Set the timestamp")
        .def("asVision", [](const SensoryInput& self) {
            return py::cast(dynamic_cast<const Vision&>(self));
        }, "Cast to Vision type (if applicable)")
        .def("asAudio", [](const SensoryInput& self) {
            return py::cast(dynamic_cast<const Audio&>(self));
        }, "Cast to Audio type (if applicable)")
        .def("asInternalSignals", [](const SensoryInput& self) {
            return py::cast(dynamic_cast<const InternalSignals&>(self));
        }, "Cast to InternalSignals type (if applicable)");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getImage", [](const Vision& self) {
            return self.getData(); // Return vector of pixel data
        }, "Get pixel data as vector")
        .def("setImage", [](Vision& self, const std::vector<float>& data, size_t width, size_t height, size_t channels) {
            self.setData(data);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels"),
           "Set image data with dimensions");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getAudioData", [](const Audio& self) {
            return self.getData();
        }, "Get audio samples as vector");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            // This would need to be implemented in the C++ class
            // For now, returning a placeholder
            return 0.0f;
        }, py::arg("index"), "Get signal value at index")
        .def("getAllSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all signal values as vector");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return 0.0f;
        }, py::arg("index"), "Get parameter at index")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter")
        .def("clone", &Action::clone)
        .def("asDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary");

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
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to dictionary");

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
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("getStateDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Get agent body state as dictionary");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("asDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary");

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
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            
            // Add vision data if available
            auto vision = self.getVision();
            if (vision) {
                result["vision"] = vision->getData();
            }
            
            // Add touch data if available
            auto touch = self.getTouch();
            if (touch) {
                result["touch"] = touch->getData();
            }
            
            // Add internal signals if available
            auto internal = self.getInternal();
            if (internal) {
                result["internal"] = internal->getData();
            }
            
            // Add proprioception if available
            auto proprioception = self.getProprioception();
            if (proprioception) {
                result["proprioception"] = proprioception->getData();
            }
            
            // Add audio if available
            auto audio = self.getAudio();
            if (audio) {
                result["audio"] = audio->getData();
            }
            
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to dictionary")
        .def("fromDict", [](SensoryPercept& self, const py::dict& data) {
            // Parse dictionary and set sensory data
            for (auto item : data) {
                std::string key = py::str(item.first);
                if (key == "vision") {
                    std::vector<float> visionData = item.second.cast<std::vector<float>>();
                    auto vision = std::make_shared<Vision>();
                    vision->setData(visionData);
                    self.setVision(vision);
                } else if (key == "touch") {
                    std::vector<float> touchData = item.second.cast<std::vector<float>>();
                    auto touch = std::make_shared<Touch>();
                    touch->setData(touchData);
                    self.setTouch(touch);
                } else if (key == "internal") {
                    std::vector<float> internalData = item.second.cast<std::vector<float>>();
                    auto internal = std::make_shared<InternalSignals>();
                    internal->setData(internalData);
                    self.setInternal(internal);
                } else if (key == "proprioception") {
                    std::vector<float> proprioceptionData = item.second.cast<std::vector<float>>();
                    auto proprioception = std::make_shared<Proprioception>();
                    proprioception->setData(proprioceptionData);
                    self.setProprioception(proprioception);
                } else if (key == "audio") {
                    std::vector<float> audioData = item.second.cast<std::vector<float>>();
                    auto audio = std::make_shared<Audio>();
                    audio->setData(audioData);
                    self.setAudio(audio);
                } else if (key == "timestamp") {
                    double timestamp = item.second.cast<double>();
                    self.setTimestamp(timestamp);
                }
            }
        }, py::arg("data"), "Create sensory percept from dictionary");

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getState", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            return result;
        }, "Get world state as dictionary")
        .def("setState", [](SimpleWorld& self, const py::dict& state) {
            if (state.contains("width")) {
                // Note: world dimensions are set at configure() time
            }
            if (state.contains("maxEnergy")) {
                self.setMaxEnergy(state["maxEnergy"].cast<float>());
            }
            if (state.contains("energyDecayRate")) {
                self.setEnergyDecayRate(state["energyDecayRate"].cast<float>());
            }
            if (state.contains("simulationTime")) {
                // This would require a setter method in the C++ class
            }
            if (state.contains("randomSeed")) {
                self.setRandomSeed(state["randomSeed"].cast<uint64_t>());
            }
        }, py::arg("state"), "Set world state from dictionary");

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
             "Log brain status")
        .def("getState", [](const Brain& self) {
            py::dict result;
            result["totalNeurons"] = self.getTotalNeuronCount();
            result["totalSynapses"] = self.getTotalSynapseCount();
            result["activeNeurons"] = self.getActiveNeuronCount();
            result["firingNeurons"] = self.getFiringNeuronCount();
            result["averageFiringRate"] = self.getAverageFiringRate();
            result["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            result["totalSpikes"] = self.getTotalSpikeCount();
            result["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            return result;
        }, "Get brain state as dictionary")
        .def("setState", [](Brain& self, const py::dict& state) {
            // This would require methods to set brain state
            // For now, just a placeholder
        }, py::arg("state"), "Set brain state from dictionary")
        .def("checkpoint", [](Brain& self, const std::string& filepath) {
            self.save(filepath);
        }, py::arg("filepath"), "Save brain state as checkpoint");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getState", [](const AgentBrain& self) {
            py::dict result;
            result["sensoryInputSize"] = self.getSensoryInputSize();
            result["motorOutputSize"] = self.getMotorOutputSize();
            result["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            result["neuromodulationLevel"] = self.getNeuromodulationLevel();
            result["curiosityLevel"] = self.getCuriosityLevel();
            result["noveltyLevel"] = self.getNoveltyLevel();
            result["predictionError"] = self.getPredictionError();
            result["rewardModulationEnabled"] = self.isRewardModulationEnabled();
            result["structuralPlasticityEnabled"] = self.isStructuralPlasticityEnabled();
            result["developmentEnabled"] = self.isDevelopmentEnabled();
            result["curiosityEnabled"] = self.isCuriosityEnabled();
            return result;
        }, "Get agent brain state as dictionary")
        .def("updateState", [](AgentBrain& self, const py::dict& state) {
            // This would require methods to update agent brain state
            // For now, just a placeholder
        }, py::arg("state"), "Update agent brain state from dictionary")
        .def("checkpoint", [](AgentBrain& self, const std::string& filepath) {
            self.getBrain()->save(filepath);
        }, py::arg("filepath"), "Save agent brain state as checkpoint");

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

    // Python-specific helper functions
    m.def("run_demo", [](int episodes, int steps, const std::string& config_file) {
        // This would call the demo mode from main.cpp
        // For now, just a placeholder
        std::cout << "Running Python demo: " << episodes << " episodes, " << steps << " steps each" << std::endl;
    }, py::arg("episodes") = 5, py::arg("steps") = 200, py::arg("config_file") = "configs/default.cfg",
       "Run NLM demo from Python");

    m.def("run_benchmark", [](const std::string& config_file, int iterations) {
        // This would call the benchmark mode from main.cpp
        // For now, just a placeholder
        std::cout << "Running Python benchmark: " << iterations << " iterations" << std::endl;
    }, py::arg("config_file") = "configs/default.cfg", py::arg("iterations") = 3,
       "Run NLM benchmark from Python");

    m.def("run_test", [](int iterations) {
        // This would call the test mode from main.cpp
        // For now, just a placeholder
        std::cout << "Running Python test: " << iterations << " iterations" << std::endl;
    }, py::arg("iterations") = 1,
       "Run NLM tests from Python");

    // Exception definitions
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Constants
    m.attr("VERSION") = "0.1.0";
    m.attr("PHASE") = "Phase 2: Real Neural Computation";

    // Convenience constants
    py::module sys = py::module::import("sys");
    if (sys.attr("version_info").attr("major").cast<int>() >= 3) {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const std::runtime_error& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        });
    }
}

} // namespace nlm
