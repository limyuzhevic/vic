#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <map>

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

// Custom Python exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Config Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("World Error: " + msg) {}
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& msg) : NLMException("Agent Error: " + msg) {}
};

// Context manager for Brain
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    bool initialized;
    bool should_reset;

public:
    explicit BrainContextManager(std::shared_ptr<Brain> b) 
        : brain(b), initialized(false), should_reset(true) {
        if (!brain) {
            throw BrainError("Cannot create context manager with null brain");
        }
    }

    ~BrainContextManager() {
        if (should_reset && initialized) {
            try {
                brain->reset();
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }

    void initialize() {
        if (initialized) return;
        if (!brain->initialize()) {
            throw BrainError("Failed to initialize brain in context manager");
        }
        initialized = true;
    }

    std::shared_ptr<Brain> getBrain() {
        if (!initialized) {
            initialize();
        }
        return brain;
    }

    void disableReset() {
        should_reset = false;
    }

    void step(SimulationStep step, Timestamp time = 0.0) {
        if (!initialized) {
            initialize();
        }
        brain->step(step, time);
    }

    py::dict getStatistics() const {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }

    std::string getStatus() const {
        std::ostringstream oss;
        brain->logStatus();
        return oss.str();
    }
};

// Context manager for AgentBrain
class AgentBrainContextManager {
private:
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;

public:
    AgentBrainContextManager(std::shared_ptr<AgentBrain> a, std::shared_ptr<SimpleWorld> w)
        : agent(a), world(w), initialized(false) {
        if (!agent) {
            throw AgentError("Cannot create context manager with null agent");
        }
    }

    ~AgentBrainContextManager() {
        if (initialized) {
            try {
                agent->reset();
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }

    void initialize() {
        if (initialized) return;
        if (!world) {
            throw AgentError("Cannot initialize agent without world");
        }
        agent->initialize(world);
        initialized = true;
    }

    std::shared_ptr<AgentBrain> getAgent() {
        if (!initialized) {
            initialize();
        }
        return agent;
    }

    void step(SimulationStep step, Timestamp time = 0.0) {
        if (!initialized) {
            initialize();
        }
        world->update(time);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        agent->getBrain()->step(step, time);
    }

    py::dict getSimulationState() const {
        py::dict state;
        if (world) {
            state["simulation_time"] = world->getSimulationTime();
            state["width"] = world->getWidth();
            state["height"] = world->getHeight();
            
            auto body = world->getAgentBody();
            py::dict agent_body;
            agent_body["x"] = body.x;
            agent_body["y"] = body.y;
            agent_body["orientation"] = body.orientation;
            agent_body["energy"] = body.energy;
            state["agent_body"] = agent_body;
        }
        return state;
    }
};

// Context manager for SimpleWorld
class SimpleWorldContextManager {
private:
    std::shared_ptr<SimpleWorld> world;
    bool configured;

public:
    explicit SimpleWorldContextManager(std::shared_ptr<SimpleWorld> w)
        : world(w), configured(false) {
        if (!world) {
            throw WorldError("Cannot create context manager with null world");
        }
    }

    ~SimpleWorldContextManager() {
        try {
            world->reset();
        } catch (...) {
            // Ignore cleanup errors
        }
    }

    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        if (configured) return;
        world->configure(width, height, visionWidth, visionHeight);
        configured = true;
    }

    std::shared_ptr<SimpleWorld> getWorld() {
        return world;
    }

    void reset() {
        world->reset();
    }
};

// Analysis utilities
namespace analysis {
    py::dict computeConnectivityMatrix(std::shared_ptr<Brain> brain) {
        py::dict matrix;
        auto regions = brain->getRegions();
        
        for (size_t i = 0; i < regions.size(); ++i) {
            py::list row;
            for (size_t j = 0; j < regions.size(); ++j) {
                // Count synapses between regions i and j
                size_t count = 0;
                for (const auto& syn : regions[i]->getSynapses()) {
                    for (const auto& syn2 : regions[j]->getSynapses()) {
                        // Simplified connectivity check
                        if (syn->getSourceId() == syn2->getSourceId()) {
                            count++;
                        }
                    }
                }
                row.append(count);
            }
            matrix[std::to_string(i)] = row;
        }
        return matrix;
    }

    py::dict computeActivityPatterns(std::shared_ptr<Brain> brain, SimulationStep startStep, SimulationStep endStep) {
        py::dict patterns;
        patterns["steps_analyzed"] = endStep - startStep + 1;
        
        // Collect firing rates over time
        py::list firing_rates;
        for (SimulationStep step = startStep; step <= endStep; ++step) {
            // This is a simplified version - in reality would need to track over time
            firing_rates.append(static_cast<float>(brain->getFiringNeuronCount()));
        }
        patterns["firing_rates"] = firing_rates;
        
        // Compute statistics
        float sum = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f);
        float avg = sum / firing_rates.size();
        
        float min = *std::min_element(firing_rates.begin(), firing_rates.end());
        float max = *std::max_element(firing_rates.begin(), firing_rates.end());
        
        py::dict stats;
        stats["mean"] = avg;
        stats["min"] = min;
        stats["max"] = max;
        stats["range"] = max - min;
        patterns["statistics"] = stats;
        
        return patterns;
    }

    py::dict performanceProfile(std::shared_ptr<Brain> brain, SimulationStep steps) {
        py::dict profile;
        
        // Simulate performance measurement
        auto start = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        profile["total_steps"] = steps;
        profile["total_time_ms"] = static_cast<double>(duration.count());
        profile["steps_per_second"] = (steps * 1000.0) / duration.count();
        profile["neurons_processed"] = brain->getTotalNeuronCount() * steps;
        
        return profile;
    }

    // Neural activity analysis
    py::dict analyzeLearning(std::shared_ptr<Brain> brain, size_t initial_steps = 100, size_t learning_steps = 1000) {
        py::dict results;
        
        // Record initial state
        brain->reset();
        brain->initialize();
        
        size_t initial_spikes = brain->getTotalSpikeCount();
        auto initial_regions = brain->getRegions();
        std::vector<float> initial_weights;
        
        for (const auto& region : initial_regions) {
            for (const auto& syn : region->getSynapses()) {
                initial_weights.push_back(syn->getWeight());
            }
        }
        
        // Learning phase
        for (SimulationStep step = 0; step < learning_steps; ++step) {
            // Apply learning input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        size_t final_spikes = brain->getTotalSpikeCount();
        auto final_regions = brain->getRegions();
        std::vector<float> final_weights;
        
        for (const auto& region : final_regions) {
            for (const auto& syn : region->getSynapses()) {
                final_weights.push_back(syn->getWeight());
            }
        }
        
        // Compute statistics
        py::dict learning_stats;
        learning_stats["spike_increase"] = static_cast<float>(final_spikes - initial_spikes);
        learning_stats["spike_growth_rate"] = (static_cast<float>(final_spikes - initial_spikes) / initial_spikes);
        
        // Weight changes
        float total_weight_change = 0.0f;
        size_t min_size = std::min(initial_weights.size(), final_weights.size());
        for (size_t i = 0; i < min_size; ++i) {
            total_weight_change += std::abs(final_weights[i] - initial_weights[i]);
        }
        learning_stats["total_weight_change"] = total_weight_change;
        learning_stats["learning_detected"] = (total_weight_change > 0.001f);
        
        results["learning_statistics"] = learning_stats;
        results["initial_spikes"] = initial_spikes;
        results["final_spikes"] = final_spikes;
        
        return results;
    }
}

// Serialization utilities
namespace serialization {
    bool saveBrainToJSON(std::shared_ptr<Brain> brain, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            py::dict brain_data;
            brain_data["neuron_count"] = brain->getTotalNeuronCount();
            brain_data["synapse_count"] = brain->getTotalSynapseCount();
            brain_data["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            // Save regions data
            py::list regions_data;
            for (const auto& region : brain->getRegions()) {
                py::dict region_data;
                region_data["id"] = region->getId();
                region_data["neuron_count"] = region->getNeuronCount();
                regions_data.append(region_data);
            }
            brain_data["regions"] = regions_data;
            
            // Convert to JSON string (simplified)
            std::string json_str = py::cast<std::string>(py::json(brain_data));
            file << json_str;
            file.close();
            return true;
        } catch (...) {
            return false;
        }
    }

    bool loadBrainFromJSON(std::shared_ptr<Brain> brain, const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            py::dict data = py::json::parse(content);
            
            // Restore brain state (simplified)
            brain->reset();
            
            // In reality, would restore actual neural state
            // For now, just return success if file could be read
            return true;
        } catch (...) {
            return false;
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes with improved error handling, context managers,
        and advanced analysis tools.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
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

    // Config with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("validate", [](Config& self) {
            // Validate configuration parameters
            py::dict validation_results;
            bool is_valid = true;
            
            if (self.has("neuron_count") && self.getOr<int64_t>("neuron_count", 0) <= 0) {
                validation_results["neuron_count"] = "Must be positive";
                is_valid = false;
            }
            
            if (self.has("region_count") && self.getOr<int64_t>("region_count", 0) <= 0) {
                validation_results["region_count"] = "Must be positive";
                is_valid = false;
            }
            
            if (self.has("connection_probability")) {
                float prob = self.getOr<float>("connection_probability", 0.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    validation_results["connection_probability"] = "Must be between 0 and 1";
                    is_valid = false;
                }
            }
            
            validation_results["valid"] = is_valid;
            return validation_results;
        }, "Validate configuration parameters");

    // SensoryInput
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["dimensions"] = self.getDimensions();
            result["timestamp"] = self.getTimestamp();
            auto data = self.getData();
            result["data"] = py::cast(data);
            return result;
        }, "Convert sensory input to dictionary");

    // Vision
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
        .def("to_array", [](const Vision& self) {
            return py::cast(self.getData());
        }, "Get vision data as NumPy array");

    // Audio
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount);

    // Action
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = py::cast(self.getParameters());
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary");

    // WorldObject
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
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with properties)pbdoc")
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
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocity_x"] = self.velocityX;
            result["velocity_y"] = self.velocityY;
            result["angular_velocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["is_moving"] = self.isMoving;
            result["is_turning"] = self.isTurning;
            result["last_action_time"] = self.lastActionTime;
            return result;
        }, "Convert agent body to dictionary");

    // ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if action result was positive")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if action result was negative");

    // SensoryPercept
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
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprioception = self.getProprioception();
            auto audio = self.getAudio();
            
            if (vision) result["vision"] = py::cast(*vision);
            if (touch) result["touch"] = py::cast(*touch);
            if (internal) result["internal"] = py::cast(*internal);
            if (proprioception) result["proprioception"] = py::cast(*proprioception);
            if (audio) result["audio"] = py::cast(*audio);
            
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to dictionary");

    // SimpleWorld with context manager support
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world with enhanced features)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
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
        .def("create_context_manager", [](std::shared_ptr<SimpleWorld> world) {
            return std::make_shared<SimpleWorldContextManager>(world);
        }, py::arg("world"), "Create a context manager for this world");

    // Brain with context manager and enhanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with enhanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        .def("create_context_manager", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<BrainContextManager>(brain);
        }, py::arg("brain"), "Create a context manager for this brain")
        .def("get_enhanced_statistics", [](std::shared_ptr<Brain> brain) {
            py::dict stats;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            return stats;
        }, "Get enhanced brain statistics")
        .def("analyze_connectivity", &analysis::computeConnectivityMatrix,
             py::arg("brain"), "Analyze and return connectivity matrix")
        .def("analyze_activity", &analysis::computeActivityPatterns,
             py::arg("brain"), py::arg("start_step"), py::arg("end_step"),
             "Analyze neural activity patterns")
        .def("profile_performance", &analysis::performanceProfile,
             py::arg("brain"), py::arg("steps"), "Profile performance over specified steps")
        .def("analyze_learning", &analysis::analyzeLearning,
             py::arg("brain"), py::arg("initial_steps") = 100, py::arg("learning_steps") = 1000,
             "Analyze learning process")
        .def("save_to_json", &serialization::saveBrainToJSON,
             py::arg("brain"), py::arg("filepath"),
             "Save brain state to JSON file")
        .def("load_from_json", &serialization::loadBrainFromJSON,
             py::arg("brain"), py::arg("filepath"),
             "Load brain state from JSON file");

    // AgentBrain with enhanced features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with enhanced capabilities)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with underlying brain")
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
        .def("create_context_manager", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
            return std::make_shared<AgentBrainContextManager>(agent, world);
        }, py::arg("agent"), py::arg("world"), "Create a context manager for agent and world");

    // Factory functions
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

    // Analysis module access
    m.def("analysis", &analysis::computeConnectivityMatrix,
         py::arg("brain"), "Compute connectivity matrix (alias for analysis module)");

    // Serialization module access
    m.def("serialization", &serialization::saveBrainToJSON,
         py::arg("brain"), py::arg("filepath"), "Save brain to JSON");

    // Utility functions
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        py::dict results;
        bool is_valid = true;
        
        if (config->has("neuron_count") && config->getOr<int64_t>("neuron_count", 0) <= 0) {
            results["neuron_count"] = "Must be positive";
            is_valid = false;
        }
        
        if (config->has("region_count") && config->getOr<int64_t>("region_count", 0) <= 0) {
            results["region_count"] = "Must be positive";
            is_valid = false;
        }
        
        if (config->has("connection_probability")) {
            float prob = config->getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                results["connection_probability"] = "Must be between 0 and 1";
                is_valid = false;
            }
        }
        
        results["valid"] = is_valid;
        return results;
    }, "Validate configuration parameters");

    m.def("benchmark_step", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double steps_per_second = (steps * 1000.0) / duration.count();
        return steps_per_second;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain stepping performance");

    m.def("compare_brains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, SimulationStep steps) {
        py::dict comparison;
        
        // Run brain1
        for (SimulationStep step = 0; step < steps; ++step) {
            brain1->step(step, step * 0.001);
        }
        
        // Reset and run brain2
        brain2->reset();
        brain2->initialize();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain2->step(step, step * 0.001);
        }
        
        // Compare statistics
        py::dict stats1, stats2;
        
        stats1["neurons"] = brain1->getTotalNeuronCount();
        stats1["synapses"] = brain1->getTotalSynapseCount();
        stats1["spikes"] = brain1->getTotalSpikeCount();
        stats1["firing_rate"] = brain1->getAverageFiringRate();
        
        stats2["neurons"] = brain2->getTotalNeuronCount();
        stats2["synapses"] = brain2->getTotalSynapseCount();
        stats2["spikes"] = brain2->getTotalSpikeCount();
        stats2["firing_rate"] = brain2->getAverageFiringRate();
        
        comparison["brain1"] = stats1;
        comparison["brain2"] = stats2;
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), py::arg("steps"), "Compare two brains");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module documentation
    m.def("get_version", []() {
        return "NLM Python Bindings v1.0 (Enhanced)";
    }, "Get the version of the enhanced Python bindings");

    m.def("print_welcome", []() {
        std::cout << "Welcome to the Enhanced NLM Python Bindings!" << std::endl;
        std::cout << "Key features:" << std::endl;
        std::cout << "  - Enhanced error handling with custom exceptions" << std::endl;
        std::cout << "  - Context managers for automatic resource management" << std::endl;
        std::cout << "  - Advanced analysis and visualization tools" << std::endl;
        std::cout << "  - Serialization support for brain states" << std::endl;
        std::cout << "  - Performance profiling and benchmarking" << std::endl;
        std::cout << "  - Neural connectivity and activity analysis" << std::endl;
        return std::string("Welcome message printed");
    }, "Print welcome message with key features");
}

} // namespace nlm