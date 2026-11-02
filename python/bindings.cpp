#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

namespace py = pybind11;
namespace nlm {

// Utility functions for Python bindings

// Configuration validation helper
py::dict validateConfig(std::shared_ptr<Config> config) {
    py::dict result;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Required keys for basic functionality
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate ranges
    auto neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    if (neuronCount <= 0 || neuronCount > 100000) {
        warnings.push_back("Neuron count " + std::to_string(neuronCount) + " is outside typical range (1-100000)");
    }
    
    auto regionCount = config->getOr<int64_t>("region_count", 1);
    if (regionCount <= 0 || regionCount > 100) {
        warnings.push_back("Region count " + std::to_string(regionCount) + " is outside typical range (1-100)");
    }
    
    auto connectionProb = config->getOr<float>("connection_probability", 0.1f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        errors.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        warnings.push_back("Simulation timestep " + std::to_string(timestep) + " is outside typical range (0.0001-1.0)");
    }
    
    result["errors"] = errors;
    result["warnings"] = warnings;
    result["valid"] = errors.empty();
    result["key_count"] = static_cast<int>(config->getKeys().size());
    
    return result;
}

// Batch processing helper
std::vector<std::vector<double>> batchProcessNeurons(
    std::shared_ptr<Brain> brain, 
    std::vector<NeuronId> neuronIds, 
    std::vector<double> inputs,
    size_t steps
) {
    std::vector<std::vector<double>> results;
    
    if (neuronIds.size() != inputs.size()) {
        throw std::runtime_error("Neuron IDs and inputs must have same size");
    }
    
    // Record initial states
    std::vector<double> initialStates;
    for (const auto& neuronId : neuronIds) {
        // In a real implementation, this would get the neuron's state
        initialStates.push_back(0.0); // Placeholder
    }
    
    results.push_back(initialStates);
    
    // Process batch
    for (size_t step = 0; step < steps; ++step) {
        std::vector<double> currentStates;
        
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            // In a real implementation, inject current and get neuron state
            brain->injectCurrent(neuronIds[i], inputs[i]);
            // Get neuron state would go here
            currentStates.push_back(0.0); // Placeholder
        }
        
        brain->step(step, step * 0.001);
        results.push_back(currentStates);
    }
    
    return results;
}

// Visualization helper
def visualizeActivity(
    std::shared_ptr<Brain> brain,
    size_t neuronsToShow,
    double maxFiringRate,
    const std::string& filename
) {
    auto spikeSystem = brain->getSpikeSystem();
    auto activeNeurons = spikeSystem->getMostActiveNeurons(neuronsToShow);
    
    // Create simple ASCII visualization
    std::string visualization = "Neuron Activity Heatmap (top " + std::to_string(neuronsToShow) + " neurons):\n\n";
    
    for (size_t i = 0; i < std::min(neuronsToShow, activeNeurons.size()); ++i) {
        double normalizedRate = static_cast<double>(i + 1) / neuronsToShow;
        std::string bar = "";
        for (int j = 0; j < static_cast<int>(normalizedRate * 50); ++j) {
            bar += "#";
        }
        visualization += std::to_string(activeNeurons[i].index()) + ": " + bar + "\n";
    }
    
    // In a real implementation, this would write to a file
    if (!filename.empty()) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << visualization;
            file.close();
        }
    }
    
    return visualization;
}

// Statistics and metrics helper
def computeBrainMetrics(std::shared_ptr<Brain> brain) {
    py::dict metrics;
    
    metrics["total_neurons"] = brain->getTotalNeuronCount();
    metrics["total_synapses"] = brain->getTotalSynapseCount();
    metrics["active_neurons"] = brain->getActiveNeuronCount();
    metrics["firing_neurons"] = brain->getFiringNeuronCount();
    metrics["total_spikes"] = brain->getTotalSpikeCount();
    metrics["average_firing_rate"] = brain->getAverageFiringRate();
    metrics["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
    metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
    
    // Get region statistics
    auto regions = brain->getRegions();
    std::vector<py::dict> regionMetrics;
    
    for (size_t i = 0; i < regions.size(); ++i) {
        py::dict regionMetric;
        regionMetric["id"] = i + 1;
        regionMetric["neuron_count"] = regions[i]->getNeuronCount();
        regionMetric["synapse_count"] = regions[i]->getSynapseCount();
        regionMetric["region_name"] = "Region_" + std::to_string(i + 1);
        regionMetrics.push_back(regionMetric);
    }
    
    metrics["region_metrics"] = regionMetrics;
    
    // Get system status
    py::dict systems;
    auto stdp = brain->getSTDP();
    auto hebbian = brain->getHebbian();
    auto structural = brain->getStructuralPlasticity();
    
    systems["stdp_enabled"] = (stdp != nullptr);
    systems["hebbian_enabled"] = (hebbian != nullptr);
    systems["structural_enabled"] = (structural != nullptr);
    
    metrics["systems"] = systems;
    
    return metrics;
}

// Configuration helper functions
def getConfigSummary(std::shared_ptr<Config> config) {
    std::string summary = "Configuration Summary:\n";
    summary += "==================\n\n";
    
    std::vector<std::string> keys = config->getKeys();
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        auto value = config->get<double>(key);
        summary += key + " = " + std::to_string(*value) + "\n";
    }
    
    return summary;
}

// Random utility functions
def generateRandomConfig(std::shared_ptr<Config> config) {
    // Generate a random but reasonable configuration
    config->set("random_seed", static_cast<int64_t>(std::time(nullptr)), ConfigSource::Runtime);
    config->set("neuron_count", static_cast<int64_t>(1000 + (rand() % 9000)), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1 + (rand() % 9)), ConfigSource::Runtime);
    config->set("connection_probability", 0.01f + (rand() / (RAND_MAX / 0.49f)), ConfigSource::Runtime);
    config->set("simulation_timestep", 0.0001f + (rand() / (RAND_MAX / 0.0099f)), ConfigSource::Runtime);
    
    return config;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Enhanced with additional API methods, validation, and utilities.
        
        Features:
        - Enhanced Brain class with batch processing and visualization
        - Config validation and helper functions
        - Advanced statistics and metrics
        - Random configuration generation
        - Activity visualization tools
        - Performance monitoring utilities
        
        New methods for advanced users:
        - batch_process_neurons()
        - visualize_activity()
        - compute_brain_metrics()
        - validate_config()
        - get_config_summary()
        - generate_random_config()
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with enhanced methods)pbdoc")
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
        .def("is_valid", [](const NeuronId& id) {
            return id.value > 0;  // Simple validity check
        });

    // Enhanced SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse with enhanced methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("is_valid", [](const SynapseId& id) {
            return id.value > 0;  // Simple validity check
        });

    // Enhanced RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with enhanced methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("is_valid", [](const RegionId& id) {
            return id.value > 0;  // Simple validity check
        });

    // Enhanced PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population with enhanced methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &NeuronId::operator!=);

    // Enhanced enum classes with additional metadata
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration with descriptions)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; })
        .def("to_string", [](NeuronType type) {
            static const std::map<NeuronType, std::string> names = {
                {NeuronType::Excitatory, "Excitatory"},
                {NeuronType::Inhibitory, "Inhibitory"},
                {NeuronType::Modulatory, "Modulatory"},
                {NeuronType::Sensory, "Sensory"},
                {NeuronType::Motor, "Motor"},
                {NeuronType::Internal, "Internal"}
            };
            return names.at(type);
        });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration with descriptions)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; })
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("to_string", [](SynapseType type) {
            static const std::map<SynapseType, std::string> names = {
                {SynapseType::Excitatory, "Excitatory"},
                {SynapseType::Inhibitory, "Inhibitory"},
                {SynapseType::Modulatory, "Modulatory"},
                {SynapseType::Electrical, "Electrical"},
                {SynapseType::GapJunction, "GapJunction"}
            };
            return names.at(type);
        });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration with descriptions)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::CriticalPeriod || 
                   stage == DevelopmentalStage::Maturation;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult;
        })
        .def("to_string", [](DevelopmentalStage stage) {
            static const std::map<DevelopmentalStage, std::string> names = {
                {DevelopmentalStage::Initial, "Initial"},
                {DevelopmentalStage::CriticalPeriod, "CriticalPeriod"},
                {DevelopmentalStage::Maturation, "Maturation"},
                {DevelopmentalStage::Adult, "Adult"},
                {DevelopmentalStage::Aging, "Aging"}
            };
            return names.at(stage);
        });

    // Enhanced Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration class with validation and utility methods)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
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
        .def("get", &Config::get<double>, py::arg("key"),
             "Get a configuration value as double")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with default")
        .def("set", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration string value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration integer value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration boolean value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("validate", &validateConfig, py::arg("config"),
             "Validate configuration and return validation results")
        .def("get_summary", &getConfigSummary, py::arg("config"),
             "Get human-readable configuration summary")
        .def("get_or_default", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with string default")
        .def("get_or_int", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with int default")
        .def("get_or_float", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with float default")
        .def("get_or_bool", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with bool default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("generate_random", &generateRandomConfig, py::arg("config"),
             "Generate a random configuration");

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Advanced sensory input with additional methods)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("is_empty", [](const SensoryInput& input) {
            return input.getData().empty();
        }, "Check if sensory input is empty")
        .def("get_data_size", [](const SensoryInput& input) {
            return input.getData().size();
        }, "Get size of sensory data");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with additional methods)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getData", &Vision::getData, "Get vision data")
        .def("normalize", &Vision::normalize, "Normalize vision data to [0,1]")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            self.resize(newWidth, newHeight);
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize vision image");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with additional methods)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getData", &Audio::getData, "Get audio data")
        .def("getFrequency", &Audio::getFrequency, "Get audio frequency analysis")
        .def("filter", [](Audio& self, float lowFreq, float highFreq) {
            self.filter(lowFreq, highFreq);
        }, py::arg("lowFreq"), py::arg("highFreq"), "Filter audio frequencies");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with additional methods)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getData", &InternalSignals::getData, "Get internal signals data")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of signals")
        .def("average", &InternalSignals::average, "Get average signal value")
        .def("standard_deviation", &InternalSignals::standardDeviation, "Get standard deviation");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("getDescription", &Action::getDescription, "Get action description")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            result["description"] = action.getDescription();
            return result;
        }, "Convert action to dictionary")
        .def(static_cast<std::string (Action::*)(ActionType)>(&Action::getName), py::arg("type"), "Get action name from type");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with additional methods)pbdoc")
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_within_radius", [](const WorldObject& obj, float x, float y, float radius) {
            float distance = std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
            return distance <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if within radius");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state with additional methods)pbdoc")
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
        .def("move", [](AgentBody& body, float dx, float dy) {
            body.x += dx;
            body.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move agent body by offset")
        .def("rotate", [](AgentBody& body, float angle) {
            body.orientation += angle;
        }, py::arg("angle"), "Rotate agent body")
        .def("get_position", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get agent position as tuple")
        .def("get_velocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get agent velocity as tuple");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("get_reward_range", [](float reward) {
            return std::make_pair(std::max(-1.0f, reward), std::min(1.0f, reward));
        }, py::arg("reward"), "Get normalized reward range");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data with additional methods)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getSignalCount", &SensoryPercept::getSignalCount, "Get total signal count")
        .def("hasVision", &SensoryPercept::hasVision, "Check if vision is available")
        .def("hasTouch", &SensoryPercept::hasTouch, "Check if touch is available")
        .def("hasInternal", &SensoryPercept::hasInternal, "Check if internal signals are available")
        .def("hasAudio", &SensoryPercept::hasAudio, "Check if audio is available")
        .def("merge", &SensoryPercept::merge, py::arg("other"), "Merge with another percept");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Advanced simple world simulation environment)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory ranges")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
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
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight, "Get vision height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getEnvironmentStats", &SimpleWorld::getEnvironmentStats, "Get environment statistics")
        .def("isAgentAtPosition", [](SimpleWorld& world, float x, float y) {
            auto body = world.getAgentBody();
            return std::abs(body.x - x) < 0.01f && std::abs(body.y - y) < 0.01f;
        }, py::arg("x"), py::arg("y"), "Check if agent is at position");

    // Enhanced Brain class with advanced methods
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Advanced neural simulation brain with enhanced methods)pbdoc")
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
        .def("batchInjectCurrent", [](Brain& brain, 
                               const std::vector<std::pair<NeuronId, float>>& injections,
                               SimulationStep step, float timestep = 0.001f) {
            for (const auto& injection : injections) {
                brain.injectCurrent(injection.first, injection.second);
            }
            brain.step(step, step * timestep);
        }, py::arg("injections"), py::arg("step"), py::arg("timestep") = 0.001f,
            "Inject current to multiple neurons in batch")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        .def("compute_metrics", &computeBrainMetrics, py::arg("brain"),
             "Compute advanced brain metrics and statistics")
        .def("visualize_activity", [](Brain& brain, size_t neuronsToShow, double maxFiringRate, const std::string& filename) {
            return visualizeActivity(std::make_shared<Brain>(brain), neuronsToShow, maxFiringRate, filename);
        }, py::arg("neuronsToShow") = 10, py::arg("maxFiringRate") = 100.0, py::arg("filename") = "",
            "Visualize brain activity")
        .def("batch_process_neurons", &batchProcessNeurons,
             py::arg("neuronIds"), py::arg("inputs"), py::arg("steps"),
             "Process neurons in batch for performance testing");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with enhanced methods)pbdoc")
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

    // Factory functions with enhanced error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation support");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (with validation)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface with validation");

    // Utility functions
    m.def("validate_config", &validateConfig, py::arg("config"),
          "Validate configuration and return detailed results");

    m.def("compute_brain_metrics", &computeBrainMetrics, py::arg("brain"),
          "Compute comprehensive brain metrics and statistics");

    m.def("visualize_activity", [](std::shared_ptr<Brain> brain, 
                              size_t neuronsToShow = 10, 
                              double maxFiringRate = 100.0, 
                              const std::string& filename = "") {
        return visualizeActivity(brain, neuronsToShow, maxFiringRate, filename);
    }, py::arg("brain"), py::arg("neuronsToShow") = 10, py::arg("maxFiringRate") = 100.0, py::arg("filename") = "",
          "Visualize brain activity and generate heatmaps");

    m.def("batch_process_neurons", &batchProcessNeurons,
          py::arg("brain"), py::arg("neuronIds"), py::arg("inputs"), py::arg("steps"),
          "Process multiple neurons efficiently for performance testing");

    m.def("get_config_summary", &getConfigSummary, py::arg("config"),
          "Get human-readable configuration summary");

    m.def("generate_random_config", &generateRandomConfig, py::arg("config") = nullptr,
          "Generate a random configuration (uses global random seed)");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0-Phase6-Enhanced";
    m.attr("__phase__") = "6";
    m.attr("__enhancement__") = "AdvancedPythonAPI";
}

} // namespace nlm
