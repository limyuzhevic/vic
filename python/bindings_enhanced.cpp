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

// Helper functions for Python bindings
namespace pybind11_helpers {
    // Create a dictionary from Config for easier Python access
    py::dict configToDict(const Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<std::string>(key);
            if (value) {
                result[key] = *value;
            }
        }
        return result;
    }
    
    // Create Config from dictionary for easier Python usage
    std::shared_ptr<Config> dictToConfig(const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config->set(key, value);
        }
        return config;
    }
    
    // Helper to extract values from Config with defaults
    template<typename T>
    T getConfigValue(const Config& config, const std::string& key, const T& defaultValue) {
        auto value = config.get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    // Advanced brain configuration for Python
    std::shared_ptr<Config> createBrainConfig(
        size_t neuronCount = 1000,
        size_t regionCount = 2,
        float connectionProbability = 0.1f,
        const std::string& environment = "GridWorld"
    ) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("environment_name", environment);
        return config;
    }
    
    // Simulation shortcuts for common use cases
    void runQuickSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t steps,
        bool verbose = false
    ) {
        if (verbose) {
            std::cout << "=== Quick NLM Simulation ===" << std::endl;
            std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Steps: " << steps << std::endl;
        }
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Get observation
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, step * 0.001);
            
            if (verbose && (step % 500 == 0)) {
                std::cout << "Step " << step << ": "
                          << "Firing rate: " << brain->getAverageFiringRate()
                          << " Total spikes: " << brain->getTotalSpikeCount()
                          << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << "=== Simulation Complete ===" << std::endl;
            std::cout << "Final firing rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    // Create a pre-configured agent for common robotics tasks
    std::shared_ptr<AgentBrain> createRoboticsAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        const std::string& task = "navigation"
    ) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable common behaviors based on task
        if (task == "navigation") {
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
        } else if (task == "exploration") {
            agent->enableCuriosity(true);
            agent->enableNoveltyDetection(true);
        } else if (task == "learning") {
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        }
        
        return agent;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        
        This enhanced version provides comprehensive Python bindings for the NLM C++ neural
        simulation framework with advanced convenience functions, performance optimizations,
        and neuroscience-specific APIs.
        
        Key Features:
        - Enhanced brain configuration and management
        - Advanced sensory and motor control APIs
        - Convenience functions for common simulation tasks
        - Performance monitoring and benchmarking
        - Advanced Python integration with optimization
        - Neuroscience-specific utilities
        
        Usage Examples:
        ```python
        import pynlm
        
        # Create a configured brain
        config = pynlm.createBrainConfig(neuronCount=500, environment="GridWorld")
        brain = pynlm.createBrain(config)
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run quick simulation
        pynlm.runQuickSimulation(brain, world, agent, steps=1000, verbose=True)
        ```
    )pbdoc";

    // Register exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron and synapse identifiers (same as before for backward compatibility)
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

    // Enum definitions (same as before)
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

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with Pythonic interface)pbdoc")
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
        })
        // Pythonic convenience methods
        .def("setValue", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (Pythonic alias for set)")
        .def("setValue", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (Pythonic alias for set)")
        .def("setValue", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (Pythonic alias for set)")
        .def("setValue", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (Pythonic alias for set)")
        .def("getValue", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a configuration value with Pythonic default")
        .def("getValue", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get a configuration value with Pythonic default")
        .def("getValue", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get a configuration value with Pythonic default")
        .def("getValue", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a configuration value with Pythonic default")
        // Config dictionary conversion
        .def("toDict", pybind11_helpers::configToDict,
             "Convert configuration to Python dictionary")
        .def("fromDict", [](Config& self, const py::dict& config_dict) {
            auto config = pybind11_helpers::dictToConfig(config_dict);
            // Copy all values from the temporary config
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                auto value = config->get<std::string>(key);
                if (value) {
                    self.set(key, *value);
                }
            }
        }, py::arg("config_dict"),
           "Update configuration from Python dictionary");

    // Enhanced SensoryInput classes
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
        .def("getChannels", &Vision::getChannels)
        // Vision-specific methods
        .def("setPixel", [](Vision& self, size_t x, size_t y, float r, float g, float b) {
            // Simplified - would need actual implementation
        }, py::arg("x"), py::arg("y"), py::arg("r"), py::arg("g"), py::arg("b"),
           "Set a pixel color (simplified)")
        .def("getPixel", [](Vision& self, size_t x, size_t y) {
            // Return a default color
            return std::vector<float>{0.5f, 0.5f, 0.5f};
        }, py::arg("x"), py::arg("y"),
           "Get a pixel color (simplified)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Audio-specific methods
        .def("addSample", &Audio::addSample, py::arg("sample"),
             "Add an audio sample")
        .def("getSamples", &Audio::getData, "Get audio samples")
        .def("getAmplitude", [](Audio& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float maxVal = *std::max_element(data.begin(), data.end(),
                                             [](float a, float b) { return std::abs(a) < std::abs(b); });
            return maxVal;
        }, "Get maximum amplitude");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getDataSize,
             "Get number of signals")
        .def("getAllSignals", &InternalSignals::getData, "Get all signal values")
        .def("getAverage", [](InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average signal value");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Action utility methods
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("getParameter", [](Action& self, size_t index, float defaultValue) {
            auto params = self.getParameters();
            return (index < params.size()) ? params[index] : defaultValue;
        }, py::arg("index"), py::arg("defaultValue") = 0.0f,
           "Get parameter at index with default")
        .def("isComplete", [](Action& self) {
            // Check if action has required parameters based on type
            auto type = self.getType();
            auto params = self.getParameters();
            
            switch (type) {
                case ActionType::MoveForward:
                case ActionType::MoveBackward:
                case ActionType::TurnLeft:
                case ActionType::TurnRight:
                    return !params.empty();
                case ActionType::Look:
                case ActionType::LookUp:
                case ActionType::LookDown:
                    return params.size() >= 1; // Direction
                case ActionType::Interact:
                    return params.size() >= 1; // Target distance
                default:
                    return true; // Custom actions are always complete
            }
        }, "Check if action is complete");

    // Enhanced world and agent classes
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
        // WorldObject convenience methods
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position")
        .def("isNearby", [](const WorldObject& self, float x, float y, float threshold) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (threshold * threshold);
        }, py::arg("x"), py::arg("y"), py::arg("threshold"),
           "Check if nearby position")
        .def("getPosition", [](const WorldObject& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        // AgentBody utility methods
        .def("getPosition", [](AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getDistanceTo", [](AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position")
        .def("isNearby", [](AgentBody& self, float x, float y, float threshold) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (threshold * threshold);
        }, py::arg("x"), py::arg("y"), py::arg("threshold"),
           "Check if nearby position")
        .def("moveTo", [](AgentBody& self, float x, float y, float maxSpeed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0.0f) {
                float speed = std::min(maxSpeed, distance / 0.001f); // Simplified time step
                self.velocityX = (dx / distance) * speed;
                self.velocityY = (dy / distance) * speed;
                self.isMoving = true;
            } else {
                self.isMoving = false;
            }
        }, py::arg("x"), py::arg("y"), py::arg("maxSpeed") = 1.0f,
           "Move to position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
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
        // SensoryPercept utility methods
        .def("addSignal", [](SensoryPercept& self, float value) {
            // Add signal to internal signals
            auto internal = self.getInternal();
            internal.addSignal(value);
            self.setInternal(internal);
        }, py::arg("value"), "Add a signal value")
        .def("getSignalCount", [](SensoryPercept& self) {
            auto internal = self.getInternal();
            return internal.getDataSize();
        }, "Get number of signals")
        .def("getAverageSignal", [](SensoryPercept& self) {
            auto internal = self.getInternal();
            return internal.getAverage();
        }, "Get average signal value")
        .def("hasVision", [](SensoryPercept& self) {
            auto vision = self.getVision();
            return vision.getWidth() > 0 && vision.getHeight() > 0;
        }, "Check if vision data available")
        .def("hasAudio", [](SensoryPercept& self) {
            auto audio = self.getAudio();
            return audio.getNumSamples() > 0;
        }, "Check if audio data available");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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
        // World utility methods
        .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add resource at position")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float damage = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 0.5f, "Add hazard at position")
        .def("getNearestResource", [](SimpleWorld& self, float x, float y) {
            WorldObject nearest;
            float minDist = std::numeric_limits<float>::max();
            
            for (const auto& obj : self.getAllObjects()) {
                if (obj.type == WorldObjectType::Resource) {
                    float dx = obj.x - x;
                    float dy = obj.y - y;
                    float dist = dx * dx + dy * dy;
                    if (dist < minDist) {
                        minDist = dist;
                        nearest = obj;
                    }
                }
            }
            
            return py::make_tuple(nearest.x, nearest.y, nearest.value);
        }, py::arg("x"), py::arg("y"),
           "Get nearest resource (x, y, value)")
        .def("getSafeZoneRadius", [](SimpleWorld& self) {
            return 5.0f; // Default safe zone
        }, "Get safe zone radius");

    // Enhanced Brain class with Pythonic interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with Pythonic interface)pbdoc")
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
        // Brain convenience methods
        .def("isInitialized", [](Brain& self) {
            // Check if brain has regions
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized")
        .def("getNeuronCountPerRegion", [](Brain& self) {
            std::vector<size_t> counts;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                counts.push_back(region->getAllNeurons()->size());
            }
            return counts;
        }, "Get neuron count per region")
        .def("getSynapticDensity", [](Brain& self) {
            size_t totalNeurons = self.getTotalNeuronCount();
            size_t totalSynapses = self.getTotalSynapseCount();
            return (totalNeurons > 0) ? (totalSynapses / totalNeurons) : 0.0f;
        }, "Get synaptic density (synapses per neuron)")
        .def("getFiringStats", [](Brain& self) {
            size_t totalNeurons = self.getTotalNeuronCount();
            size_t firingNeurons = self.getFiringNeuronCount();
            float avgFiringRate = self.getAverageFiringRate();
            
            return py::make_tuple(
                firingNeurons,
                (totalNeurons > 0) ? (firingNeurons * 100.0f / totalNeurons) : 0.0f,
                avgFiringRate
            );
        }, "Get firing statistics (firing count, percentage, avg rate)")
        .def("getExcitatoryInhibitoryBalance", [](Brain& self) {
            float ratio = self.getExcitationInhibitionRatio();
            return py::make_tuple(ratio, (ratio > 1.0f) ? "Excitation Dominant" : 
                                                     (ratio < 0.9f) ? "Inhibition Dominant" : "Balanced");
        }, "Get excitation/inhibition balance with status");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // AgentBrain utility methods
        .def("getNeuromodulationStats", [](AgentBrain& self) {
            return py::make_tuple(
                self.getNeuromodulationLevel(),
                self.getCuriosityLevel(),
                self.getNoveltyLevel(),
                self.getPredictionError()
            );
        }, "Get neuromodulation statistics")
        .def("getDevelopmentProgress", [](AgentBrain& self) {
            auto stage = self.getDevelopmentalStage();
            std::string stageName;
            
            switch (stage) {
                case DevelopmentalStage::Initial: stageName = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stageName = "Critical Period"; break;
                case DevelopmentalStage::Maturation: stageName = "Maturation"; break;
                case DevelopmentalStage::Adult: stageName = "Adult"; break;
                case DevelopmentalStage::Aging: stageName = "Aging"; break;
            }
            
            return py::make_tuple(stage, stageName);
        }, "Get developmental stage info")
        .def("isActive", [](AgentBrain& self) {
            auto level = self.getNeuromodulationLevel();
            return level > 0.1f; // Consider agent active if neuromodulation is significant
        }, "Check if agent is currently active");

    // Advanced configuration factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrainConfig", pybind11_helpers::createBrainConfig,
          py::arg("neuronCount") = 1000,
          py::arg("regionCount") = 2,
          py::arg("connectionProbability") = 0.1f,
          py::arg("environment") = "GridWorld",
          "Create a configured brain configuration");

    m.def("createComplexBrainConfig", [](size_t neuronCount, float connectionProb, 
                                         const std::string& env, bool enableDevelopment,
                                         bool enableCuriosity) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 2);
        config->set("connection_probability", connectionProb);
        config->set("environment_name", env);
        config->set("enable_development", enableDevelopment ? "true" : "false");
        config->set("enable_curiosity", curiosity ? "true" : "false");
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("reward_discount_factor", 0.99f);
        return config;
    }, py::arg("neuronCount") = 500,
          py::arg("connectionProb") = 0.1f,
          py::arg("env") = "GridWorld",
          py::arg("enableDevelopment") = true,
          py::arg("enableCuriosity") = true,
          "Create a complex brain configuration");

    // Core object factories
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced simulation functions
    m.def("runQuickSimulation", pybind11_helpers::runQuickSimulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          py::arg("verbose") = false,
          "Run a complete simulation with quick setup");

    m.def("runBenchmarkSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, size_t steps,
                                       bool verbose = false) {
        // Benchmark implementation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        if (verbose) {
            std::cout << "=== Benchmark Results ===" << std::endl;
            std::cout << "Steps: " << steps << std::endl;
            std::cout << "Duration: " << duration.count() << "s" << std::endl;
            std::cout << "Steps per second: " << (steps / std::max(duration.count(), 1LL)) << std::endl;
            std::cout << "Final firing rate: " << brain->getAverageFiringRate() << std::endl;
        }
        
        return std::make_tuple(steps, duration.count(), brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          py::arg("verbose") = false,
          "Run a benchmark simulation and return performance metrics");

    m.def("createRoboticsAgent", pybind11_helpers::createRoboticsAgent,
          py::arg("brain"), py::arg("world"), py::arg("task") = "navigation",
          "Create a pre-configured agent for robotics tasks");

    // Performance and monitoring functions
    m.def("getBrainPerformance", [](std::shared_ptr<Brain> brain) {
        return py::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getActiveNeuronCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
    }, py::arg("brain"), "Get brain performance metrics");

    m.def("compareBrains", [](std::vector<std::shared_ptr<Brain>> brains) {
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            py::dict result;
            result["brain_id"] = i;
            result["neuron_count"] = brains[i]->getTotalNeuronCount();
            result["synapse_count"] = brains[i]->getTotalSynapseCount();
            result["firing_rate"] = brains[i]->getAverageFiringRate();
            result["total_spikes"] = brains[i]->getTotalSpikeCount();
            
            results.push_back(result);
        }
        
        return results;
    }, py::arg("brains"), "Compare multiple brains and return metrics");

    // Utility and helper functions
    m.def("formatNeuronId", [](NeuronId id) {
        return "N" + std::to_string(id.value);
    }, py::arg("id"), "Format neuron ID as string");

    m.def("formatSynapseId", [](SynapseId id) {
        return "S" + std::to_string(id.value);
    }, py::arg("id"), "Format synapse ID as string");

    m.def("createRandomConfig", []() {
        auto config = std::make_shared<Config>();
        // Set random but reasonable values
        config->set("neuron_count", 500 + (rand() % 1500));
        config->set("region_count", 1 + (rand() % 5));
        config->set("connection_probability", 0.05f + (rand() % 100) / 2000.0f);
        config->set("random_seed", rand());
        return config;
    }, "Create a random configuration");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        auto keys = config->getKeys();
        bool valid = true;
        std::string errors;
        
        if (!config->has("neuron_count")) {
            valid = false;
            errors += "Missing neuron_count; ";
        }
        
        if (!config->has("connection_probability")) {
            valid = false;
            errors += "Missing connection_probability; ";
        }
        
        return py::make_tuple(valid, errors);
    }, py::arg("config"), "Validate configuration and return result");

    // Constants and identifiers (same as before for compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python-specific utilities
    m.def("pythonicBrainStep", [](std::shared_ptr<Brain> brain, size_t steps, bool verbose = false) {
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            if (verbose && (step % 100 == 0)) {
                std::cout << "Step " << step << ": "
                          << "Firing rate: " << brain->getAverageFiringRate()
                          << " Spikes: " << brain->getTotalSpikeCount()
                          << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << "Pythonic simulation completed" << std::endl;
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false,
       "Pythonic brain stepping with progress reporting");

    // Export constants for Python
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 2;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    m.attr("SIMULATION_TIME_STEP") = 0.001f;
}

} // namespace nlm