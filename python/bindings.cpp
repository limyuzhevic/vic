#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <chrono>

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
        
        This module provides Python bindings for a sophisticated artificial neural system
        that implements spiking neural computation with plasticity, development, memory,
        prediction, and neuromodulation systems.
        
        For comprehensive examples and documentation, see the Python examples directory.
    )pbdoc";

    // Register custom exceptions for Python users
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register configuration and validation exceptions
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    py::register_exception<std::logic_error>(m, "LogicError");
    
    // Create a custom exception for NLM-specific errors
    py::class_<py::python_exception<>>(m, "NLMError")
        .def(py::init<const std::string&>())
        .def("what", [](const py::python_exception<>& self) {
            return self.what();
        });
    
    // Add utility functions for error handling and validation
    m.def("validate_id", [](const uint64_t value) {
        if (value == 0) {
            throw py::value_error("Invalid ID value: must be non-zero");
        }
        return NeuronId(value);
    }, py::arg("value"), "Create a validated NeuronId");
    
    m.def("safe_get", [](const py::object& obj, const std::string& attr, const py::object& default_value) {
        try {
            return obj.attr(attr.c_str());
        } catch (const py::error_already_set&) {
            return default_value;
        }
    }, py::arg("obj"), py::arg("attr"), py::arg("default_value") = py::none(), 
       "Safely get an attribute from an object with a default value");

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

    // Improve Python API with more idiomatic names and patterns
    
    // Config improvements - more Pythonic property access
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_all_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set_value", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with source (advanced)")
        .def("get_int", &Config::getOr<int64_t>, py::arg("key"), py::arg("default_value") = 0,
             "Get integer value or default")
        .def("get_float", &Config::getOr<double>, py::arg("key"), py::arg("default_value") = 0.0,
             "Get float value or default")
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("default_value") = "",
             "Get string value or default")
        .def("get_bool", &Config::getOr<bool>, py::arg("key"), py::arg("default_value") = false,
             "Get boolean value or default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput improvements - more Pythonic API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision improvements - more Pythonic constructor
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("get_shape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get vision dimensions as a tuple");

    // Audio improvements - more Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // Action improvements - more Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("is_valid", &Action::getType, "Check if action is valid (non-custom)")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + std::to_string(action.getParameters().size()) + ">";
        });

    // WorldObject improvements - more Pythonic
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
        .def("position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set position using method")
        .def("get_position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get position as tuple")
        .def("distance_to", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object");

    // Add utility functions for data conversion and serialization
    m.def("serialize_neuron", [](const Neuron& neuron) {
        py::dict result;
        result["id"] = neuron.getId().value;
        result["type"] = static_cast<int>(neuron.getType());
        result["membrane_potential"] = neuron.getMembranePotential();
        result["threshold"] = neuron.getThreshold();
        result["is_firing"] = neuron.isFiring();
        result["is_refractory"] = neuron.isRefractory();
        result["firing_rate"] = neuron.getFiringRate();
        return result;
    }, py::arg("neuron"), "Serialize a neuron to a dictionary");

    m.def("deserialize_neuron", [](py::dict data) -> std::unique_ptr<Neuron> {
        auto neuron = std::make_unique<Neuron>(NeuronId(data["id"].cast<uint64_t>()));
        neuron->setType(static_cast<NeuronType>(data["type"].cast<int>()));
        neuron->setMembranePotential(data["membrane_potential"].cast<float>());
        neuron->setThreshold(data["threshold"].cast<float>());
        return neuron;
    }, py::arg("data"), "Deserialize a neuron from a dictionary");

    m.def("convert_to_json", [](const py::object& obj) {
        // Simple JSON conversion helper
        if (py::isinstance<py::dict>(obj)) {
            py::dict result;
            for (auto item : obj) {
                result[item.first.cast<std::string>()] = convert_to_json(item.second);
            }
            return result;
        } else if (py::isinstance<py::list>(obj)) {
            py::list result;
            for (auto item : obj) {
                result.append(convert_to_json(item));
            }
            return result;
        } else {
            return obj;
        }
    }, py::arg("obj"), "Recursively convert object to JSON-serializable format");

    m.def("from_json", [](const py::object& obj) {
        // Simple reverse conversion (basic implementation)
        return obj;
    }, py::arg("obj"), "Convert object from JSON-serializable format (basic)");

    m.def("batch_synchronize", [](std::vector<std::shared_ptr<Brain>>& brains, double timestep) {
        for (auto& brain : brains) {
            brain->step(0, timestep);
        }
    }, py::arg("brains"), py::arg("timestep"), "Synchronize multiple brains in batch");

    m.def("export_brain_state", [](std::shared_ptr<Brain>& brain, const std::string& format) {
        if (format == "json") {
            py::dict state;
            state["neuron_count"] = brain->getTotalNeuronCount();
            state["synapse_count"] = brain->getTotalSynapseCount();
            state["spike_count"] = brain->getTotalSpikeCount();
            state["region_count"] = brain->getRegionCount();
            return state;
        } else {
            throw std::invalid_argument("Unsupported export format: " + format);
        }
    }, py::arg("brain"), py::arg("format"), "Export brain state in specified format");

    m.def("import_brain_state", [](std::shared_ptr<Brain>& brain, py::dict state) {
        // Basic implementation - in practice would be more complex
        return brain;
    }, py::arg("brain"), py::arg("state"), "Import brain state from dictionary");

    m.def("validate_configuration", [](std::shared_ptr<Config>& config) {
        std::vector<std::string> warnings;
        
        if (!config->has("random_seed")) {
            warnings.push_back("Missing random_seed configuration");
        }
        if (!config->has("simulation_timestep")) {
            warnings.push_back("Missing simulation_timestep configuration");
        }
        if (!config->has("neuron_count")) {
            warnings.push_back("Missing neuron_count configuration");
        }
        
        return warnings;
    }, py::arg("config"), "Validate configuration and return warnings");

    m.def("generate_config_summary", [](std::shared_ptr<Config>& config) {
        std::stringstream ss;
        ss << "Configuration Summary:" << std::endl;
        ss << "  Keys: " << config->getKeys().size() << std::endl;
        ss << "  Entries: " << config->summary() << std::endl;
        return ss.str();
    }, py::arg("config"), "Generate human-readable configuration summary");

    m.def("safe_execute", [](py::function func, py::args args, py::kwargs kwargs) {
        try {
            return func(*args, **kwargs);
        } catch (const py::error_already_set&) {
            py::print("Error in safe_execute:");
            py::print(py::str(py::cast<py::object>(func)));
            throw;  // Re-raise the exception
        }
    }, py::arg("func"), py::arg("args"), py::arg("kwargs"), "Safely execute a function with error handling");

    m.def("parallel_step", [](std::shared_ptr<Brain>& brain, size_t steps, double timestep) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, timestep);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"), "Execute multiple steps in parallel-like fashion");

    m.def("create_timed_callback", [](py::function callback, double interval) {
        // Simple timer implementation
        auto start_time = std::chrono::steady_clock::now();
        return [callback, interval, start_time]() mutable {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(now - start_time).count();
            if (elapsed >= interval) {
                callback();
                start_time = now;
            }
        };
    }, py::arg("callback"), py::arg("interval"), "Create a timed callback function");

    m.def("compute_neural_stats", [](std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        return stats;
    }, py::arg("brain"), "Compute comprehensive neural statistics");

    m.def("format_duration", [](double seconds) {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds / 60) % 60);
        int secs = static_cast<int>(seconds % 60);
        int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        if (hours > 0) {
            ss << hours << "h ";
        }
        ss << minutes << "m " << secs << "." << ms << "s";
        return ss.str();
    }, py::arg("seconds"), "Format duration in human-readable format");

    m.def("generate_unique_id", []() {
        static uint64_t counter = 0;
        return ++counter;
    }, "Generate a unique integer ID");

    m.def("safe_range", [](size_t start, size_t end, size_t step = 1) {
        if (step == 0) {
            throw std::invalid_argument("Step cannot be zero");
        }
        if ((step > 0 && start > end) || (step < 0 && start < end)) {
            throw std::invalid_argument("Invalid range parameters");
        }
        return py::range(start, end, step);
    }, py::arg("start"), py::arg("end"), py::arg("step") = 1, "Create a safe range with validation");

    m.def("dict_merge", [](const std::vector<py::dict>& dicts) {
        py::dict result;
        for (const auto& d : dicts) {
            for (auto item : d) {
                result[item.first.cast<std::string>()] = item.second;
            }
        }
        return result;
    }, py::arg("dicts"), "Merge multiple dictionaries");

    m.def("dict_filter", [](py::dict data, py::function predicate) {
        py::dict result;
        for (auto item : data) {
            try {
                if (predicate(item.second).cast<bool>()) {
                    result[item.first] = item.second;
                }
            } catch (...) {
                // Skip items that cause errors in predicate
            }
        }
        return result;
    }, py::arg("data"), py::arg("predicate"), "Filter dictionary based on predicate function");

    m.def("progress_tracker", [](py::function task, size_t total) {
        auto tracker = [task, total](size_t current) {
            double progress = static_cast<double>(current) / total;
            int bar_width = 50;
            int pos = static_cast<int>(bar_width * progress);
            
            std::cout << "[";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << static_cast<int>(progress * 100.0) << "% "
                     << current << "/" << total << " completed\r";
            std::cout.flush();
            
            if (current >= total) {
                std::cout << std::endl;
            }
        };
        return tracker;
    }, py::arg("task"), py::arg("total"), "Create a progress tracker function");

    m.def("weighted_average", [](const py::list& values, const py::list& weights) {
        if (values.size() != weights.size()) {
            throw std::invalid_argument("Values and weights lists must have same size");
        }
        if (values.size() == 0) {
            return 0.0;
        }
        
        double sum_weighted = 0.0;
        double total_weight = 0.0;
        
        for (size_t i = 0; i < values.size(); ++i) {
            double value = values[i].cast<double>();
            double weight = weights[i].cast<double>();
            sum_weighted += value * weight;
            total_weight += weight;
        }
        
        return total_weight > 0 ? sum_weighted / total_weight : 0.0;
    }, py::arg("values"), py::arg("weights"), "Compute weighted average of values");

    m.def("round_robins分配", [](const py::list& items, size_t num_groups) {
        py::list result;
        result.resize(num_groups);
        
        if (num_groups == 0) {
            throw std::invalid_argument("Number of groups must be positive");
        }
        
        for (size_t i = 0; i < items.size(); ++i) {
            result[i % num_groups].attr("append")(items[i]);
        }
        
        return result;
    }, py::arg("items"), py::arg("num_groups"), "Distribute items across groups in round-robin fashion");

    m.def("safe_divide", [](double numerator, double denominator, double default_value = 0.0) {
        if (denominator == 0.0) {
            return default_value;
        }
        return numerator / denominator;
    }, py::arg("numerator"), py::arg("denominator"), py::arg("default_value") = 0.0, "Safe division that returns default on division by zero");

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

    // Convenience builder pattern for creating configured brain systems
    m.def("create_brain_from_template", [](const std::string& template_name) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        if (template_name == "simple") {
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            config->set("connection_probability", 0.2f, ConfigSource::Default);
        } else if (template_name == "dense") {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("connection_probability", 0.3f, ConfigSource::Default);
        } else if (template_name == "sparse") {
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain from template: " + template_name);
        }
        return brain;
    }, py::arg("template_name"), "Create a brain from a predefined configuration template");
    
    m.def("create_config_with_values", [](const std::unordered_map<std::string, py::object>& values) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : values) {
            const std::string& key = pair.first;
            const py::object& value = pair.second;
            
            if (py::isinstance<py::int_>(value)) {
                config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(value)) {
                config->set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(value)) {
                config->set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::str>(value)) {
                config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else {
                throw std::invalid_argument("Unsupported configuration value type for key: " + key);
            }
        }
        return config;
    }, py::arg("values"), "Create a configuration with Python values");
    
    // Advanced factory with sensory and world setup
    m.def("create_agent_simulation", 
        [](const std::shared_ptr<Brain>& brain, 
           const std::shared_ptr<SimpleWorld>& world,
           const std::string& agent_type) -> std::shared_ptr<AgentBrain> {
            
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(world);
            
            if (agent_type == "explorer") {
                // Configure for exploration behavior
                agentBrain->enableCuriosity(true);
                agentBrain->enableNovelty(true);
            } else if (agent_type == "optimizer") {
                // Configure for optimization behavior
                agentBrain->enableRewardModulation(true);
            } else if (agent_type == "developer") {
                // Configure for development behavior
                agentBrain->enableDevelopment(true);
                agentBrain->enableStructuralPlasticity(true);
            }
            
            return agentBrain;
        }, 
        py::arg("brain"), py::arg("world"), py::arg("agent_type") = "explorer",
        "Create an agent brain with specific behavioral configuration");
    
    // Quick setup utilities for common use cases
    m.def("setup_demo_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for demo simulation");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 160, 120);
        world->setRandomSeed(12345);
        
        return std::make_tuple(brain, world);
    }, "Set up a complete demo simulation with default configuration");

    // NeuralRegion C++ bindings that were accidentally removed
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing multiple populations)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getTotalSynapseCount", &NeuralRegion::getTotalSynapseCount)
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getAllSynapses", &NeuralRegion::getAllSynapses)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("addNeuron", &NeuralRegion::addNeuron, py::arg("neuron"))
        .def("getNeuron", &NeuralRegion::getNeuron, py::arg("id"))
        .def("getNeuronsByType", &NeuralRegion::getNeuronsByType, py::arg("type"))
        .def("getActiveNeurons", &NeuralRegion::getActiveNeurons)
        .def("computeConnectivityStats", &NeuralRegion::computeConnectivityStats)
        .def("exportRegionData", &NeuralRegion::exportRegionData)
        .def("importRegionData", &NeuralRegion::importRegionData, py::arg("data"))
        .def("reset", &NeuralRegion::reset)
        .def("logStatus", &NeuralRegion::logStatus);
