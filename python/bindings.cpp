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
        
        This module enables creating, configuring, and simulating artificial neural brains
        with memory systems, neuromodulation, development, and sensorimotor interaction.
        
        Features:
        - LIF neuron dynamics with event-driven spike propagation
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Four memory systems (working, episodic, semantic, procedural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages (initial, critical period, maturation, adult, aging)
        - Sensorimotor loop with world interaction
        - Checkpoint save/load and replay capabilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Basic type definitions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Neuron ID class
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Synapse ID class
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // Region ID class
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // Population ID class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // Neuron Type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote depolarization")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that hyperpolarize postsynaptic targets")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons affecting neural plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons receiving external stimuli")
        .value("Motor", NeuronType::Motor, "Motor neurons controlling actions")
        .value("Internal", NeuronType::Internal, "Internal neurons processing internal signals")
        .export_values();

    // Synapse Type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting neural plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with direct ion flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction coupling between cells")
        .export_values();

    // Developmental Stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period with heightened learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase with reduced plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable characteristics")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining performance")
        .export_values();

    // Firing State enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, below threshold")
        .value("Active", FiringState::Active, "Neuron currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by external input")
        .export_values();

    // Action Type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left by 90 degrees")
        .value("TurnRight", ActionType::TurnRight, "Turn right by 90 degrees")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with current object")
        .value("Eat", ActionType::Eat, "Eat current object")
        .value("Drink", ActionType::Drink, "Drink current object")
        .value("Rest", ActionType::Rest, "Rest/stop moving")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    // Motor Command enumeration
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

    // World Object Type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object")
        .value("Resource", WorldObjectType::Resource, "Collectible resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Impassable barrier")
        .value("Marker", WorldObjectType::Marker, "Landmarks/target")
        .export_values();

    // Enhanced Config class with more functionality
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        The Config class manages brain configuration parameters including neuron counts,
        connection probabilities, plasticity rules, neuromodulation settings, and more.
        
        Common use:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
        )pbdoc")
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
        // Add configuration parameter setters/getters for convenience
        .def("setNeuronCount", &Config::setNeuronCount, py::arg("count"),
             "Set neuron count")
        .def("setRegionCount", &Config::setRegionCount, py::arg("count"),
             "Set region count")
        .def("setConnectionProbability", &Config::setConnectionProbability, py::arg("prob"),
             "Set connection probability")
        .def("setRandomSeed", &Config::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible experiments")
        .def("setSimulationTimestep", &Config::setSimulationTimestep, py::arg("dt"),
             "Set simulation timestep")
        .def("getNeuronCount", &Config::getNeuronCount,
             "Get neuron count")
        .def("getRegionCount", &Config::getRegionCount,
             "Get region count")
        .def("getConnectionProbability", &Config::getConnectionProbability,
             "Get connection probability")
        .def("getRandomSeed", &Config::getRandomSeed,
             "Get random seed")
        .def("getSimulationTimestep", &Config::getSimulationTimestep,
             "Get simulation timestep");

    // Enhanced SensoryInput class with more functionality
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class representing sensory information from the environment.
        Subclasses include Vision, Audio, and InternalSignals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getTimestampMS", [](const SensoryInput& self) {
            return self.getTimestamp() * 1000.0;  // Convert to milliseconds
        }, "Get timestamp in milliseconds");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Represents visual input with width, height, and color channels.
        Data is stored as a flattened vector of RGB values.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataGrid", [](Vision& self, const std::vector<std::vector<float>>& grid) {
            // Convert 2D grid to flattened vector
            std::vector<float> data;
            data.reserve(grid.size() * grid[0].size());
            for (const auto& row : grid) {
                data.insert(data.end(), row.begin(), row.end());
            }
            self.setData(data);
        }, py::arg("grid"), "Set vision data as 2D grid of grayscale values")
        .def("getDataAsGrid", [](const Vision& self) {
            std::vector<std::vector<float>> grid;
            size_t width = self.getDimensions();
            size_t height = self.getHeight();
            
            // For simplicity, assume grayscale if only one channel
            if (self.getChannels() == 1) {
                const auto& data = self.getData();
                grid.resize(height, std::vector<float>(width));
                for (size_t y = 0; y < height; ++y) {
                    for (size_t x = 0; x < width; ++x) {
                        grid[y][x] = data[y * width + x];
                    }
                }
            }
            return grid;
        }, "Get vision data as 2D grid")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDimensions", &Vision::getDimensions)
        .def("setBrightness", [](Vision& self, float brightness) {
            auto& data = self.getData();
            std::transform(data.begin(), data.end(), data.begin(),
                         [brightness](float val) { return brightness; });
        }, py::arg("brightness"), "Set all pixels to a specific brightness (0.0-1.0)")
        .def("addNoise", [](Vision& self, float intensity) {
            auto& data = self.getData();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-intensity, intensity);
            for (auto& val : data) {
                val += dis(gen);
                val = std::max(0.0f, std::min(1.0f, val));  // Clamp to [0, 1]
            }
        }, py::arg("intensity"), "Add Gaussian noise to vision data");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Represents audio input with sample rate and time series data.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setFrequencyData", [](Audio& self, const std::vector<float>& frequencies) {
            // Convert frequency data to time domain for simplicity
            std::vector<float> timeData;
            timeData.reserve(frequencies.size());
            for (float freq : frequencies) {
                timeData.push_back(sin(freq));  // Simple sine wave representation
            }
            self.setData(timeData);
        }, py::arg("frequencies"), "Set audio from frequency data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getFrequency", [](const Audio& self, size_t index) {
            if (index >= self.getNumSamples()) {
                throw py::index_error("Sample index out of range");
            }
            // Simple frequency approximation
            return index * self.getSampleRate() / self.getNumSamples();
        }, py::arg("index"), "Get frequency at sample index");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Represents internal neural states such as hunger, energy, stress, etc.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            auto& signals = self.getData();
            if (index < signals.size()) {
                signals[index] = value;
            } else {
                // Resize if needed
                signals.resize(index + 1, 0.0f);
                signals[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set signal at specific index")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& signals = self.getData();
            if (index >= signals.size()) {
                throw py::index_error("Signal index out of range");
            }
            return signals[index];
        }, py::arg("index"), "Get signal value at index");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents an action to be taken by the agent in the environment.
        Can be specified by type and optional parameters.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("addParameter", [](Action& self, float value) {
            auto& params = self.getParameters();
            params.push_back(value);
        }, py::arg("value"), "Add a parameter to the action")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get parameter value at index")
        .def("setMovement", [](Action& self, float x, float y, float angle = 0.0f) {
            self.setType(ActionType::Custom);
            std::vector<float> params = {x, y, angle};
            self.setParameters(params);
        }, py::arg("x"), py::arg("y"), py::arg("angle") = 0.0f,
             "Set movement action with position and angle parameters")
        .def("setLook", [](Action& self, float direction) {
            self.setType(ActionType::Look);
            self.setParameters({direction});
        }, py::arg("direction"), "Set look action with direction parameter");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the world with position, type, and properties.
    )pbdoc")
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
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("getObjectName", [](const WorldObject& self) {
            std::string names[] = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
            return names[static_cast<int>(self.type)];
        }, "Get human-readable object name");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the physical state of an agent in the world including position,
        orientation, velocity, and physiological states.
    )pbdoc")
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
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getHeading", [](const AgentBody& self) {
            return self.orientation;  // degrees
        }, "Get heading direction in degrees")
        .def("setHeading", [](AgentBody& self, float heading) {
            self.orientation = heading;  // degrees
            while (self.orientation >= 360.0f) self.orientation -= 360.0f;
            while (self.orientation < 0.0f) self.orientation += 360.0f;
        }, py::arg("heading"), "Set heading direction in degrees")
        .def("isExhausted", [](const AgentBody& self) {
            return self.energy < 10.0f;
        }, "Check if agent is exhausted (low energy)")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 50.0f;
        }, "Check if agent is healthy")
        .def("updatePosition", [](AgentBody& self, float dt) {
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            // Wrap around world boundaries (conceptual)
            if (self.x < 0) self.x += 1000.0f;  // Arbitrary large world
            if (self.x >= 1000.0f) self.x -= 1000.0f;
            if (self.y < 0) self.y += 1000.0f;
            if (self.y >= 1000.0f) self.y -= 1000.0f;
        }, py::arg("dt"), "Update position based on velocity over timestep");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Represents the outcome of an agent's action in the environment, including
        reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result was positive (reward > 0)")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result was negative (reward < 0)")
        .def("toString", [](const ActionResult& self) {
            std::stringstream ss;
            ss << "ActionResult(reward=" << self.reward
               << ", success=" << (self.success ? "true" : "false")
               << ", message=\"" << self.message << "\")";
            return ss.str();
        }, "Get string representation");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Container for all sensory inputs from the world including vision,
        touch, internal signals, and proprioception.
    )pbdoc")
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
        .def("getTimestampMS", [](const SensoryPercept& self) {
            return self.getTimestamp() * 1000.0;
        }, "Get timestamp in milliseconds")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if vision data is available")
        .def("hasInternalSignals", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are available")
        .def("getSignalSum", [](const SensoryPercept& self) {
            const auto& signals = self.getInternal();
            float sum = 0.0f;
            for (float val : signals) sum += val;
            return sum;
        }, "Get sum of all internal signals")
        .def("getSignalAverage", [](const SensoryPercept& self) {
            const auto& signals = self.getInternal();
            if (signals.empty()) return 0.0f;
            float sum = 0.0f;
            for (float val : signals) sum += val;
            return sum / signals.size();
        }, "Get average of all internal signals")
        .def("clone", &SensoryPercept::clone)
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            // Merge other percept into this one
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            // Keep timestamp from self
            return self;
        }, py::arg("other"), "Merge another percept into this one");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        A simple 2D world that simulates an agent's interaction with its environment.
        Contains terrain, objects, and provides sensory input to agents.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's current body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not blocked)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy for agent")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setSimulationTime", &SimpleWorld::setSimulationTime, py::arg("time"),
             "Set simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            return self.getAgentBody().orientation;
        }, "Get agent orientation in degrees")
        .def("getVisionData", [](const SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getVision();
        }, "Get vision data directly from world")
        .def("isAgentAt", [](const SimpleWorld& self, float x, float y) {
            const auto& body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return dx * dx + dy * dy < 0.01f;  // 0.1m radius
        }, py::arg("x"), py::arg("y"), "Check if agent is at specific position")
        .def("getNearbyObjects", [](const SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            // This would need access to world objects - placeholder for future enhancement
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects within radius (future enhancement)")
        .def("hasResourceAt", [](const SimpleWorld& self, float x, float y) {
            // Check for resource at position
            auto percept = self.getSensoryPercept();
            // This would need actual implementation
            return false;
        }, py::arg("x"), py::arg("y"), "Check if resource exists at position (placeholder)")
        .def("hasHazardAt", [](const SimpleWorld& self, float x, float y) {
            // Check for hazard at position
            auto percept = self.getSensoryPercept();
            // This would need actual implementation
            return false;
        }, py::arg("x"), py::arg("y"), "Check if hazard exists at position (placeholder)");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The Brain class represents an artificial neural system that processes information,
        learns from experience, and produces actions. It implements LIF neurons with
        multiple plasticity mechanisms and memory systems.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with step number only")
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
        // Enhanced brain methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             py::return_value_policy::reference_internal,
             "Get semantic memory system")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             py::return_value_policy::reference_internal,
             "Get procedural memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getNeuralWorkingMemory", &Brain::getNeuralWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get neural working memory system")
        .def("getNeuralEpisodicMemory", &Brain::getNeuralEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get neural episodic memory system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get planner/cognition system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attention system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getPredictionError", &Brain::getPredictionError,
             "Get prediction error signal")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of firing neurons")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get E/I balance ratio")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get planner system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attention system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        The AgentBrain class provides a high-level interface for connecting a neural brain
        to an environment. It handles sensory processing, motor decoding, neuromodulation,
        and development.
    )pbdoc")
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Enhanced AgentBrain methods
        .def("getRewardModulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get reward modulation level (alias for neuromodulation level)")
        .def("getCuriosityStrength", &AgentBrain::getCuriosityLevel,
             "Get curiosity strength (alias for curiosity level)")
        .def("getNoveltySensitivity", &AgentBrain::getNoveltyLevel,
             "Get novelty sensitivity (alias for novelty level)")
        .def("getPredictionAccuracy", [](const AgentBrain& self) {
            float error = self.getPredictionError();
            return std::max(0.0f, 1.0f - error);
        }, "Get prediction accuracy (derived from prediction error)")
        .def("getLearningRate", [](const AgentBrain& self) {
            // Combine neuromodulation levels to get effective learning rate
            float neuromod = self.getNeuromodulationLevel();
            float curiosity = self.getCuriosityLevel();
            return neuromod * (1.0f + curiosity * 0.5f);
        }, "Get effective learning rate based on neuromodulation")
        .def("getExplorationRate", [](const AgentBrain& self) {
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            return curiosity * (1.0f + novelty * 0.3f);
        }, "Get exploration rate based on curiosity and novelty");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard NLM parameters");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions
    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
        "Run a brain simulation for specified steps and return spike count");

    m.def("runAgentEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1f);
            agent->processSensoryInput(world->getSensoryPercept());
            agent->getBrain()->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.0f, 0.0f);  // Default reward
        }
        return std::make_pair(agent->getBrain()->getTotalSpikeCount(), 
                            agent->getNeuromodulationLevel());
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
        "Run a complete agent episode and return (spike_count, neuromodulation_level)");

    m.def("createQuickTestBrain", []() {
        auto config = std::make_shared<Config>();
        config->setNeuronCount(100);
        config->setRegionCount(1);
        config->setConnectionProbability(0.15f);
        config->setRandomSeed(42);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a small test brain with quick settings");

    m.def("createQuickTestAgent", [](std::shared_ptr<SimpleWorld> world) {
        auto brain = createQuickTestBrain();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("world"), "Create a quick test agent with world");

    // Error and utility functions
    m.def("validateBrainState", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw py::value_error("Brain pointer is null");
        if (!brain->getConfig()) throw py::value_error("Brain configuration is missing");
        if (brain->getTotalNeuronCount() == 0) throw py::value_error("Brain has no neurons");
        return true;
    }, py::arg("brain"), "Validate brain state and return True if valid");

    m.def("validateWorldState", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) throw py::value_error("World pointer is null");
        if (world->getWidth() <= 0 || world->getHeight() <= 0) 
            throw py::value_error("World dimensions are invalid");
        return true;
    }, py::arg("world"), "Validate world state and return True if valid");

    m.def("getSystemInfo", []() {
        std::stringstream ss;
        ss << "NLM Python Bindings v1.0.0" << std::endl;
        ss << "C++ Compiler: __VERSION__" << std::endl;
        ss << "Build configuration: " << (defined(DEBUG) ? "Debug" : "Release") << std::endl;
        return ss.str();
    }, "Get system information about the NLM bindings");

    m.def("testIntegration", []() {
        // Quick integration test
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::runtime_error("Brain initialization failed");
        }
        return brain->getTotalNeuronCount() > 0;
    }, "Run quick integration test and return True if successful");

    // Version and metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings for neural simulation";
}

} // namespace nlm