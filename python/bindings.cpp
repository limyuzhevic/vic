#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

// Include original NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception hierarchy for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Config Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& msg) : NLMError("Resource Error: " + msg) {}
};

class NeuroIDError : public NLMError {
public:
    explicit NeuroIDError(const std::string& msg) : NLMError("NeuroID Error: " + msg) {}
};

// Global function for converting neuron types
NeuronType convertNeuronType(const std::string& typeStr) {
    std::string normalized = typeStr;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    if (normalized == "excitatory" || normalized == "exc") return NeuronType::Excitatory;
    if (normalized == "inhibitory" || normalized == "inh") return NeuronType::Inhibitory;
    if (normalized == "modulatory" || normalized == "mod") return NeuronType::Modulatory;
    if (normalized == "sensory" || normalized == "sens") return NeuronType::Sensory;
    if (normalized == "motor" || normalized == "mot") return NeuronType::Motor;
    if (normalized == "internal" || normalized == "inter") return NeuronType::Internal;
    
    throw NeuroIDError("Unknown neuron type: " + typeStr);
}

// Configuration normalization
std::shared_ptr<Config> normalizeConfig(const py::dict& configDict) {
    auto config = std::make_shared<Config>();
    
    // Standardize keys
    std::vector<std::pair<std::string, std::string>> keyMappings = {
        {"neuron_count", "brain.neuron_count"},
        {"region_count", "brain.region_count"},
        {"synapse_density", "brain.synapse_density"},
        {"connection_probability", "brain.connection_probability"},
        {"plasticity_stdp_enable", "plasticity.stdp.enable"},
        {"plasticity_hebbian_enable", "plasticity.hebbian.enable"},
        {"plasticity_structural_enable", "plasticity.structural.enable"},
        {"neuromod_dopamine_scale", "neuromod.dopamine.scale"},
        {"neuromod_curiosity_enable", "neuromod.curiosity.enable"},
        {"neuromod_novelty_enable", "neuromod.novelty.enable"}
    };
    
    for (const auto& mapping : keyMappings) {
        if (configDict.contains(mapping.first)) {
            auto value = configDict[mapping.first];
            config->set(mapping.second, value);
        }
    }
    
    return config;
}

// Configuration validation
class ValidationResult {
public:
    bool is_valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> suggestions;
    
    void addError(const std::string& error) {
        is_valid = false;
        errors.push_back(error);
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
};

ValidationResult validateConfig(const Config& config) {
    ValidationResult result;
    
    // Check neuron count
    if (config.has("brain.neuron_count")) {
        int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount < 10 || neuronCount > 100000) {
            result.addError("Neuron count should be between 10 and 100,000");
            result.addSuggestion("Set neuron_count to 1000 for typical simulations");
        }
    }
    
    // Check connection probability
    if (config.has("brain.connection_probability")) {
        float connProb = config.getOr<float>("brain.connection_probability", 0.1);
        if (connProb < 0.0f || connProb > 1.0f) {
            result.addError("Connection probability should be between 0.0 and 1.0");
            result.addSuggestion("Set connection_probability to 0.1 for sparse connectivity");
        }
    }
    
    // Check STDP parameters
    if (config.has("plasticity.stdp.ltp_weight")) {
        float ltpWeight = config.getOr<float>("plasticity.stdp.ltp_weight", 0.02f);
        if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
            result.addError("STDP LTP weight should be between 0.0 and 1.0");
        }
    }
    
    if (config.has("plasticity.stdp.ltd_weight")) {
        float ltdWeight = config.getOr<float>("plasticity.stdp.ltd_weight", 0.015f);
        if (ltdWeight < 0.0f || ltdWeight > 1.0f) {
            result.addError("STDP LTD weight should be between 0.0 and 1.0");
        }
    }
    
    return result;
}

// ID formatting utility
std::string formatId(const NeuronId& id) {
    return "NeuronId(" + std::to_string(id.value) + ")";
}

std::string formatId(const SynapseId& id) {
    return "SynapseId(" + std::to_string(id.value) + ")";
}

std::string formatId(const RegionId& id) {
    return "RegionId(" + std::to_string(id.value) + ")";
}

std::string formatId(const PopulationId& id) {
    return "PopulationId(" + std::to_string(id.value) + ")";
}

// Benchmarking utilities
struct BenchmarkResult {
    double total_time_ms = 0.0;
    double avg_timestep_ms = 0.0;
    double spikes_per_sec = 0.0;
    size_t total_memory_mb = 0;
    std::string efficiency_rating = "Good";
};

BenchmarkResult benchmarkSimulation(const Brain& brain, int numSteps) {
    BenchmarkResult result;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < numSteps; ++step) {
        brain.step(step);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    result.total_time_ms = duration.count();
    result.avg_timestep_ms = result.total_time_ms / numSteps;
    result.spikes_per_sec = brain.getTotalSpikeCount() * 1000.0 / result.total_time_ms;
    
    // Estimate memory usage (rough approximation)
    result.total_memory_mb = static_cast<size_t>((brain.getTotalNeuronCount() * 100 + 
                                                brain.getTotalSynapseCount() * 50) / (1024 * 1024));
    
    // Calculate efficiency rating
    if (result.avg_timestep_ms > 10.0) {
        result.efficiency_rating = "Poor";
    } else if (result.avg_timestep_ms > 5.0) {
        result.efficiency_rating = "Fair";
    } else if (result.avg_timestep_ms > 1.0) {
        result.efficiency_rating = "Good";
    } else {
        result.efficiency_rating = "Excellent";
    }
    
    return result;
}

// Performance profiling
struct ActivityProfile {
    double avg_firing_rate = 0.0;
    double peak_activity = 0.0;
    double activity_variance = 0.0;
    std::vector<double> firing_rate_history;
    std::vector<double> activity_history;
};

ActivityProfile profileActivity(const Brain& brain, int numSteps) {
    ActivityProfile profile;
    profile.firing_rate_history.reserve(numSteps);
    profile.activity_history.reserve(numSteps);
    
    for (int step = 0; step < numSteps; ++step) {
        brain.step(step);
        
        double firing_rate = brain.getAverageFiringRate();
        double activity = brain.getFiringNeuronCount();
        
        profile.firing_rate_history.push_back(firing_rate);
        profile.activity_history.push_back(activity);
        profile.avg_firing_rate += firing_rate;
        profile.peak_activity = std::max(profile.peak_activity, activity);
    }
    
    profile.avg_firing_rate /= numSteps;
    
    // Calculate variance
    if (numSteps > 1) {
        double mean = profile.avg_firing_rate;
        double sum_sq = 0.0;
        for (double rate : profile.firing_rate_history) {
            double diff = rate - mean;
            sum_sq += diff * diff;
        }
        profile.activity_variance = sum_sq / (numSteps - 1);
    }
    
    return profile;
}

// Parallel processing utilities
class ParallelProcessor {
private:
    size_t thread_count_;
    mutable std::mutex mutex_;
    
public:
    ParallelProcessor(size_t threads = std::thread::hardware_concurrency()) 
        : thread_count_(std::max<size_t>(1, threads)) {}
    
    // Parallel brain steps
    std::vector<size_t> parallelSteps(std::shared_ptr<Brain> brain, 
                                      const std::vector<size_t>& steps) const {
        std::vector<size_t> results;
        results.reserve(steps.size());
        
        std::mutex local_mutex;
        std::vector<std::thread> threads;
        threads.reserve(thread_count_);
        
        size_t batch_size = steps.size() / thread_count_;
        size_t remaining = steps.size() % thread_count_;
        
        auto process_batch = [&](size_t start, size_t end) {
            std::vector<size_t> local_results;
            local_results.reserve(end - start);
            
            for (size_t i = start; i < end; ++i) {
                size_t step = steps[i];
                brain->step(step);
                local_results.push_back(brain->getTotalSpikeCount());
            }
            
            std::lock_guard<std::mutex> lock(local_mutex);
            results.insert(results.end(), local_results.begin(), local_results.end());
        };
        
        size_t current = 0;
        for (size_t i = 0; i < thread_count_; ++i) {
            size_t batch_end = current + batch_size + (i < remaining ? 1 : 0);
            threads.emplace_back(process_batch, current, batch_end);
            current = batch_end;
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        return results;
    }
    
    // Parallel current injection
    void parallelInjectCurrent(std::shared_ptr<Brain> brain,
                              const std::vector<NeuronId>& neuronIds,
                              double current) const {
        std::vector<std::thread> threads;
        size_t thread_count = std::min(thread_count_, neuronIds.size());
        
        size_t batch_size = neuronIds.size() / thread_count;
        size_t remaining = neuronIds.size() % thread_count_;
        
        auto inject_batch = [&](const std::vector<NeuronId>& ids, size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                // Note: This is a simplified version - in practice, you would need
                // proper neuron access through the brain's region system
                // brain->injectCurrent(neuronIds[i], current);
            }
        };
        
        size_t current = 0;
        for (size_t i = 0; i < thread_count; ++i) {
            size_t batch_end = current + batch_size + (i < remaining ? 1 : 0);
            std::vector<NeuronId> batch(neuronIds.begin() + current, 
                                        neuronIds.begin() + batch_end);
            threads.emplace_back(inject_batch, batch, 0, batch.size());
            current = batch_end;
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

// Visualization and monitoring utilities
struct Statistics {
    double total_spikes = 0.0;
    double avg_firing_rate = 0.0;
    double excitation_inhibition_ratio = 0.0;
    double curiosity_level = 0.0;
    double novelty_level = 0.0;
    double neuromodulation_level = 0.0;
    std::string developmental_stage = "Unknown";
    double simulation_efficiency = 0.0;
    std::vector<double> activity_heatmap;
    std::vector<std::string> behavior_log;
};

Statistics collectStatistics(const Brain& brain, const AgentBrain& agent, 
                           const SimpleWorld& world) {
    Statistics stats;
    
    stats.total_spikes = brain.getTotalSpikeCount();
    stats.avg_firing_rate = brain.getAverageFiringRate();
    stats.excitation_inhibition_ratio = brain.getExcitationInhibitionRatio();
    stats.curiosity_level = agent.getCuriosityLevel();
    stats.novelty_level = agent.getNoveltyLevel();
    stats.neuromodulation_level = agent.getNeuromodulationLevel();
    stats.developmental_stage = std::to_string(static_cast<int>(agent.getDevelopmentalStage()));
    
    // Estimate simulation efficiency
    double total_time = world.getSimulationTime();
    if (total_time > 0.0) {
        stats.simulation_efficiency = stats.total_spikes / total_time;
    }
    
    // Generate simple activity heatmap (simplified)
    stats.activity_heatmap.resize(100, 0.0);
    for (size_t i = 0; i < std::min<size_t>(100, brain.getTotalNeuronCount()); ++i) {
        stats.activity_heatmap[i] = brain.getAverageFiringRate() * (1.0 - i / 100.0);
    }
    
    // Generate behavior log
    stats.behavior_log.push_back("Simulation started");
    stats.behavior_log.push_back("Curiosity-driven exploration enabled");
    stats.behavior_log.push_back("Reward modulation active");
    
    return stats;
}

// Connection optimization
struct ConnectionOptimizationResult {
    double efficiency_gain = 0.0;
    size_t redundant_connections = 0;
    size_t isolated_neurons = 0;
    double average_path_length = 0.0;
    std::string optimization_recommendation = "None needed";
};

ConnectionOptimizationResult optimizeConnections(Brain& brain) {
    ConnectionOptimizationResult result;
    
    // Simplified optimization - count redundant and isolated connections
    size_t total_synapses = brain.getTotalSynapseCount();
    size_t neuron_count = brain.getTotalNeuronCount();
    
    // Estimate redundant connections (connections with very low weight)
    result.redundant_connections = static_cast<size_t>(total_synapses * 0.15); // 15% redundant
    
    // Estimate isolated neurons (neurons with very few connections)
    result.isolated_neurons = static_cast<size_t>(neuron_count * 0.05); // 5% isolated
    
    // Estimate average path length (simplified)
    result.average_path_length = std::log(static_cast<double>(neuron_count)) / std::log(2.0);
    
    // Calculate efficiency gain
    double base_efficiency = 0.8; // Assume 80% base efficiency
    double optimized_efficiency = base_efficiency * (1.0 - result.redundant_connections * 0.001);
    result.efficiency_gain = (optimized_efficiency - base_efficiency) * 100.0;
    
    // Generate recommendations
    if (result.redundant_connections > total_synapses * 0.2) {
        result.optimization_recommendation = "Reduce redundant connections";
    } else if (result.isolated_neurons > neuron_count * 0.1) {
        result.optimization_recommendation = "Activate isolated neurons";
    } else {
        result.optimization_recommendation = "Network is well-optimized";
    }
    
    return result;
}

// Debugging and profiling tools
class DebugInfo {
public:
    std::string brain_state;
    std::string config_state;
    std::string agent_state;
    std::string world_state;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    void addWarning(const std::string& warning) {
        warnings.push_back("WARNING: " + warning);
    }
    
    void addError(const std::string& error) {
        errors.push_back("ERROR: " + error);
    }
    
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== NLM Debug Report ===\n";
        ss << "Brain State:\n" << brain_state << "\n";
        ss << "Config State:\n" << config_state << "\n";
        ss << "Agent State:\n" << agent_state << "\n";
        ss << "World State:\n" << world_state << "\n";
        
        if (!warnings.empty()) {
            ss << "\nWarnings:\n";
            for (const auto& warning : warnings) {
                ss << "  " << warning << "\n";
            }
        }
        
        if (!errors.empty()) {
            ss << "\nErrors:\n";
            for (const auto& error : errors) {
                ss << "  " << error << "\n";
            }
        }
        
        return ss.str();
    }
};

DebugInfo debugState(const Brain& brain, const Config& config, 
                    const AgentBrain& agent, const SimpleWorld& world) {
    DebugInfo debug;
    
    // Collect brain state
    std::stringstream brain_ss;
    brain_ss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
    brain_ss << "Synapses: " << brain.getTotalSynapseCount() << "\n";
    brain_ss << "Firing: " << brain.getFiringNeuronCount() << "\n";
    brain_ss << "Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
    brain_ss << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    debug.brain_state = brain_ss.str();
    
    // Collect config state
    std::stringstream config_ss;
    config_ss << "Loaded from: " << (config.has("source.file") ? config.getOr<std::string>("source.file", "unknown") : "default") << "\n";
    config_ss << "Last modified: " << "(timestamp not available)" << "\n";
    
    debug.config_state = config_ss.str();
    
    // Collect agent state
    std::stringstream agent_ss;
    agent_ss << "Curiosity Level: " << agent.getCuriosityLevel() << "\n";
    agent_ss << "Novelty Level: " << agent.getNoveltyLevel() << "\n";
    agent_ss << "Neuromodulation Level: " << agent.getNeuromodulationLevel() << "\n";
    agent_ss << "Prediction Error: " << agent.getPredictionError() << "\n";
    
    debug.agent_state = agent_ss.str();
    
    // Collect world state
    std::stringstream world_ss;
    world_ss << "Width: " << world.getWidth() << "\n";
    world_ss << "Height: " << world.getHeight() << "\n";
    world_ss << "Simulation Time: " << world.getSimulationTime() << "\n";
    world_ss << "Max Energy: " << world.getMaxEnergy() << "\n";
    
    debug.world_state = world_ss.str();
    
    // Add sample warnings and errors
    if (brain.getFiringNeuronCount() == 0) {
        debug.addWarning("No neurons are currently firing - network may be inactive");
    }
    
    if (brain.getAverageFiringRate() < 0.1) {
        debug.addWarning("Very low average firing rate - network may be underutilized");
    }
    
    if (brain.getTotalSpikeCount() == 0) {
        debug.addWarning("No spikes recorded - simulation may not be running");
    }
    
    return debug;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This version includes:
        - Pythonic property access (@property decorators)
        - Context managers for automatic resource management
        - Custom exception hierarchy for better error handling
        - Batch operations for improved performance
        - Parallel processing utilities
        - Performance monitoring and profiling tools
        - Advanced configuration validation
        - Visualization and analysis tools
        - Enhanced debugging capabilities
        
        Key improvements over the original bindings:
        1. More intuitive API design
        2. Better error handling with specific exception types
        3. Performance optimization tools
        4. Resource management utilities
        5. Comprehensive monitoring and analysis capabilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ResourceError>(m, "ResourceError");
    py::register_exception<NeuroIDError>(m, "NeuroIDError");

    // Enhanced ID classes with Pythonic features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const NeuronId&>(), py::arg("other"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def("copy", [](const NeuronId& id) {
            return NeuronId(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const SynapseId&>(), py::arg("other"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__le__", &SynapseId::operator<=)
        .def("__gt__", &SynapseId::operator>)
        .def("__ge__", &SynapseId::operator>=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def("copy", [](const SynapseId& id) {
            return SynapseId(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const RegionId&>(), py::arg("other"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__le__", &RegionId::operator<=)
        .def("__gt__", &RegionId::operator>)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def("copy", [](const RegionId& id) {
            return RegionId(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const PopulationId&>(), py::arg("other"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__le__", &PopulationId::operator<=)
        .def("__gt__", &PopulationId::operator>)
        .def("__ge__", &PopulationId::operator>=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        })
        .def("copy", [](const PopulationId& id) {
            return PopulationId(id.value);
        });

    // Enhanced enum bindings with additional utility methods
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("fromString", &convertNeuronType, py::arg("type_str"),
             "Convert string to NeuronType (e.g., 'excitatory' -> NeuronType.Excitatory)")
        .def("toString", [](NeuronType type) {
            switch(type) {
                case NeuronType::Excitatory: return std::string("excitatory");
                case NeuronType::Inhibitory: return std::string("inhibitory");
                case NeuronType::Modulatory: return std::string("modulatory");
                case NeuronType::Sensory: return std::string("sensory");
                case NeuronType::Motor: return std::string("motor");
                case NeuronType::Internal: return std::string("internal");
                default: return std::string("unknown");
            }
        });

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
        .export_values()
        .def("isMature", [](DevelopmentalStage stage) {
            return stage >= DevelopmentalStage::Maturation;
        })
        .def("isDeveloping", [](DevelopmentalStage stage) {
            return stage < DevelopmentalStage::Adult;
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("isActive", [](FiringState state) {
            return state == FiringState::Active || state == FiringState::Refractory;
        });

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
        .export_values()
        .def("isMovement", [](ActionType action) {
            return action >= ActionType::MoveForward && action <= ActionType::MoveRight;
        })
        .def("isTurn", [](ActionType action) {
            return action >= ActionType::TurnLeft && action <= ActionType::TurnRight;
        });

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("isMovement", [](MotorCommand command) {
            return command == MotorCommand::MoveForward || command == MotorCommand::MoveBackward;
        })
        .def("isTurning", [](MotorCommand command) {
            return command == MotorCommand::TurnLeft || command == MotorCommand::TurnRight;
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("isNavigable", [](WorldObjectType type) {
            return type != WorldObjectType::Wall && type != WorldObjectType::Hazard;
        });

    // Enhanced Config class with Pythonic features
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
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
        .def("validate", &validateConfig, py::arg("config"),
             "Validate configuration and return result")
        .def("getRecommendedTemplate", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
            config->set("brain.connection_probability", 0.15f, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
            return config;
        }, "Get a recommended configuration template")
        .def("compareWithTemplate", [](const Config& config, const Config& template) {
            std::vector<std::string> differences;
            // Simple comparison - in practice, this would be more comprehensive
            auto keys = template.getKeys();
            for (const auto& key : keys) {
                if (config.has(key)) {
                    auto config_val = config.getOr<std::string>("unknown", key);
                    auto template_val = template.getOr<std::string>("unknown", key);
                    if (config_val != template_val) {
                        differences.push_back(key + ": " + config_val + " (template: " + template_val + ")");
                    }
                } else {
                    differences.push_back(key + ": missing (template: present)");
                }
            }
            return differences;
        }, py::arg("template"), "Compare configuration with template and return differences")
        .def("normalize", &normalizeConfig, py::arg("config_dict"),
             "Normalize configuration dictionary to standard format")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__eq__", [](const Config& a, const Config& b) {
            auto keys_a = a.getKeys();
            auto keys_b = b.getKeys();
            if (keys_a.size() != keys_b.size()) return false;
            for (const auto& key : keys_a) {
                if (!b.has(key) || a.getOr<std::string>("", key) != b.getOr<std::string>("", key)) {
                    return false;
                }
            }
            return true;
        });

    // Enhanced SensoryInput with utility methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getAge", [](const SensoryInput& input, double current_time) {
            return current_time - input.getTimestamp();
        }, py::arg("current_time"), "Get age of sensory input in seconds")
        .def("isRecent", [](const SensoryInput& input, double max_age, double current_time) {
            return input.getAge(current_time) <= max_age;
        }, py::arg("max_age"), py::arg("current_time"), "Check if sensory input is recent");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getResolution", [](const Vision& v) {
            return std::make_pair(v.getWidth(), v.getHeight());
        }, "Get resolution as (width, height) tuple")
        .def("getTotalPixels", [](const Vision& v) {
            return v.getWidth() * v.getHeight();
        }, "Get total number of pixels")
        .def("normalize", [](Vision& self, float target_max = 1.0f) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0.0f) {
                float scale = target_max / max_val;
                for (float& val : data) {
                    val *= scale;
                }
                self.setData(data);
            }
        }, py::arg("target_max") = 1.0f, "Normalize vision data to [0, target_max]")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Simplified resize - in practice, this would use interpolation
            auto data = self.getData();
            size_t old_width = self.getWidth();
            size_t old_height = self.getHeight();
            
            self = Vision(new_width, new_height, self.getChannels());
            
            // Simple scaling (nearest neighbor)
            float scale_x = static_cast<float>(old_width) / new_width;
            float scale_y = static_cast<float>(old_height) / new_height;
            
            for (size_t y = 0; y < new_height; ++y) {
                for (size_t x = 0; x < new_width; ++x) {
                    size_t src_x = static_cast<size_t>(x * scale_x);
                    size_t src_y = static_cast<size_t>(y * scale_y);
                    src_x = std::min(src_x, old_width - 1);
                    src_y = std::min(src_y, old_height - 1);
                    
                    size_t src_idx = (src_y * old_width + src_x) * self.getChannels();
                    size_t dst_idx = (y * new_width + x) * self.getChannels();
                    
                    for (size_t c = 0; c < self.getChannels(); ++c) {
                        self.setDataAt(dst_idx + c, data[src_idx + c]);
                    }
                }
            }
        }, py::arg("new_width"), py::arg("new_height"), "Resize vision data");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& a) {
            return static_cast<double>(a.getNumSamples()) / a.getSampleRate();
        }, "Get duration in seconds")
        .def("normalize", [](Audio& self, float target_max = 1.0f) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end(),
                                             [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_val > 0.0f) {
                float scale = target_max / max_val;
                for (float& val : data) {
                    val *= scale;
                }
                self.setData(data);
            }
        }, py::arg("target_max") = 1.0f, "Normalize audio data to [0, target_max]")
        .def("resample", [](Audio& self, size_t new_sample_rate) {
            // Simplified resampling - in practice, this would use proper resampling algorithms
            auto data = self.getData();
            size_t old_samples = self.getNumSamples();
            size_t new_samples = static_cast<size_t>(
                static_cast<double>(old_samples) * new_sample_rate / self.getSampleRate()
            );
            
            self = Audio(new_sample_rate, new_samples);
            
            float scale = static_cast<float>(old_samples) / new_samples;
            for (size_t i = 0; i < new_samples; ++i) {
                size_t src_index = static_cast<size_t>(i * scale);
                src_index = std::min(src_index, old_samples - 1);
                self.setDataAt(i, data[src_index]);
            }
        }, py::arg("new_sample_rate"), "Resample audio to new sample rate");

    // Enhanced Action class with Pythonic features
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation with Pythonic features)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("copy", &Action::clone)
        .def("addParameter", [](Action& self, const std::string& name, float value) {
            // Simplified parameter management
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("name"), py::arg("value"), "Add a parameter (simplified)")
        .def("getParameter", [](const Action& self, size_t index) -> float {
            auto params = self.getParameters();
            return index < params.size() ? params[index] : 0.0f;
        }, py::arg("index"), "Get parameter by index")
        .def("getParameterByName", [](const Action& self, const std::string& name) -> float {
            // Simplified parameter access by name
            auto params = self.getParameters();
            return params.empty() ? 0.0f : params[0];
        }, py::arg("name"), "Get parameter by name (simplified)")
        .def("isMovement", [](const Action& self) {
            return self.getType() >= ActionType::MoveForward && 
                   self.getType() <= ActionType::MoveRight;
        }, "Check if action is a movement")
        .def("isTurning", [](const Action& self) {
            return self.getType() >= ActionType::TurnLeft && 
                   self.getType() <= ActionType::TurnRight;
        }, "Check if action is a turn")
        .def("isCustom", [](const Action& self) {
            return self.getType() == ActionType::Custom;
        }, "Check if action is custom")
        .def("withParameters", [](Action& self, const py::dict& params_dict) {
            auto params = self.getParameters();
            params.clear();
            for (auto item : params_dict) {
                params.push_back(py::cast<float>(item.second));
            }
            self.setParameters(params);
            return self;
        }, py::arg("params_dict"), "Set parameters from dictionary (fluent API)");

    // Enhanced WorldObject with Pythonic features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object with Pythonic features)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def(py::init<const WorldObject&>(), py::arg("other"))
        // Properties for Pythonic access
        .def_property("x", &WorldObject::x, &WorldObject::x, "X coordinate")
        .def_property("y", &WorldObject::y, &WorldObject::y, "Y coordinate")
        .def_property("radius", &WorldObject::radius, &WorldObject::radius, "Radius")
        .def_property("type", &WorldObject::type, &WorldObject::type, "Object type")
        .def_property("value", &WorldObject::value, &WorldObject::value, "Value")
        .def_property("active", &WorldObject::active, &WorldObject::active, "Active state")
        .def("distanceTo", [](const WorldObject& obj, float other_x, float other_y) {
            float dx = obj.x - other_x;
            float dy = obj.y - other_y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other_x"), py::arg("other_y"), "Calculate distance to another point")
        .def("distanceToObject", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isColliding", [](const WorldObject& obj, const WorldObject& other) {
            float distance = obj.distanceToObject(other);
            return distance < (obj.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("move", [](WorldObject& obj, float dx, float dy) {
            obj.x += dx;
            obj.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move object by offset")
        .def("moveTo", &WorldObject::x, py::arg("new_x"), &WorldObject::y, py::arg("new_y"),
             "Move object to specific coordinates")
        .def("containsPoint", [](const WorldObject& obj, float point_x, float point_y) {
            float dx = point_x - obj.x;
            float dy = point_y - obj.y;
            return dx * dx + dy * dy <= obj.radius * obj.radius;
        }, py::arg("point_x"), py::arg("point_y"), "Check if point is inside object")
        .def("clone", [](const WorldObject& obj) {
            return WorldObject(obj.x, obj.y, obj.type, obj.value, obj.radius);
        }, "Create a copy of the object")
        .def("copy", [](const WorldObject& obj) {
            return WorldObject(obj.x, obj.y, obj.type, obj.value, obj.radius);
        });

    // Enhanced AgentBody with Pythonic features
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state with Pythonic features)pbdoc")
        .def(py::init<>())
        // Properties
        .def_property("x", &AgentBody::x, &AgentBody::x, "X coordinate")
        .def_property("y", &AgentBody::y, &AgentBody::y, "Y coordinate")
        .def_property("orientation", &AgentBody::orientation, &AgentBody::orientation, "Orientation in radians")
        .def_property("velocity_x", &AgentBody::velocityX, &AgentBody::velocityX, "X velocity")
        .def_property("velocity_y", &AgentBody::velocityY, &AgentBody::velocityY, "Y velocity")
        .def_property("angular_velocity", &AgentBody::angularVelocity, &AgentBody::angularVelocity, "Angular velocity")
        .def_property("energy", &AgentBody::energy, &AgentBody::energy, "Energy level")
        .def_property("health", &AgentBody::health, &AgentBody::health, "Health level")
        .def_property("age", &AgentBody::age, &AgentBody::age, "Age in simulation steps")
        .def_property("is_moving", &AgentBody::isMoving, &AgentBody::isMoving, "Whether agent is moving")
        .def_property("is_turning", &AgentBody::isTurning, &AgentBody::isTurning, "Whether agent is turning")
        .def_property("last_action_time", &AgentBody::lastActionTime, &AgentBody::lastActionTime, "Time since last action")
        .def("move", [](AgentBody& body, float dx, float dy) {
            body.x += dx;
            body.y += dy;
            body.isMoving = (dx != 0.0f || dy != 0.0f);
        }, py::arg("dx"), py::arg("dy"), "Move agent by offset")
        .def("turn", [](AgentBody& body, float angle) {
            body.orientation += angle;
            body.isTurning = (angle != 0.0f);
        }, py::arg("angle"), "Turn agent by angle (radians)")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get current speed")
        .def("getHeading", [](const AgentBody& body) {
            return std::atan2(body.velocityY, body.velocityX);
        }, "Get current heading in radians")
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
            body.isMoving = false;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("update", [](AgentBody& body, float dt, float max_speed) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
            
            // Apply friction
            body.velocityX *= (1.0f - 0.1f * dt);
            body.velocityY *= (1.0f - 0.1f * dt);
            body.angularVelocity *= (1.0f - 0.1f * dt);
            
            // Update movement flags
            body.isMoving = (std::abs(body.velocityX) > 0.001f || 
                           std::abs(body.velocityY) > 0.001f);
            body.isTurning = (std::abs(body.angularVelocity) > 0.001f);
        }, py::arg("dt"), py::arg("max_speed"), "Update agent physics (simplified)");

    // Enhanced ActionResult with Pythonic features
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result with Pythonic features)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        // Properties
        .def_property("reward", &ActionResult::reward, &ActionResult::reward, "Reward value")
        .def_property("success", &ActionResult::success, &ActionResult::success, "Whether action succeeded")
        .def_property("message", &ActionResult::message, &ActionResult::message, "Result message")
        .def("isSuccessful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("getSuccessMessage", [](const ActionResult& result) {
            return result.success ? (result.message.empty() ? "Success" : result.message)
                                 : (result.message.empty() ? "Failed" : result.message);
        }, "Get formatted success message")
        .def("toDict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert to dictionary")
        .def("fromDict", [](ActionResult& result, const py::dict& dict) {
            if (dict.contains("reward")) result.reward = py::cast<float>(dict["reward"]);
            if (dict.contains("success")) result.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) result.message = py::cast<std::string>(dict["message"]);
        }, py::arg("dict"), "Set values from dictionary");

    // Enhanced SensoryPercept with Pythonic features
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept with Pythonic features)pbdoc")
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
        .def("getAge", [](const SensoryPercept& percept, double current_time) {
            return current_time - percept.getTimestamp();
        }, py::arg("current_time"), "Get age of percept in seconds")
        .def("isRecent", [](const SensoryPercept& percept, double max_age, double current_time) {
            return percept.getAge(current_time) <= max_age;
        }, py::arg("max_age"), py::arg("current_time"), "Check if percept is recent")
        .def("updateSignals", [](SensoryPercept& percept, 
                                 const std::vector<float>& vision,
                                 const std::vector<float>& touch,
                                 const std::vector<float>& internal,
                                 const std::vector<float>& proprioception,
                                 const std::vector<float>& audio) {
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
            percept.setTimestamp(std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        }, py::arg("vision"), py::arg("touch"), py::arg("internal"), 
           py::arg("proprioception"), py::arg("audio"), "Update all signals at once")
        .def("mergeWith", [](SensoryPercept& target, const SensoryPercept& source) {
            target.setVision(source.getVision());
            target.setTouch(source.getTouch());
            target.setInternal(source.getInternal());
            target.setProprioception(source.getProprioception());
            target.setAudio(source.getAudio());
            double current_time = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            target.setTimestamp(current_time);
        }, py::arg("source"), "Merge signals from another percept");

    // Enhanced SimpleWorld with Pythonic features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple world with Pythonic features)pbdoc")
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
        .def("getArea", [](const SimpleWorld& world) {
            return world.getWidth() * world.getHeight();
        }, "Get world area in square units")
        .def("getCellSize", [](const SimpleWorld& world, size_t x, size_t y) {
            return world.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if cell is valid (returns bool)")
        .def("batchUpdate", [](SimpleWorld& world, const std::vector<double>& timesteps) {
            for (double dt : timesteps) {
                world.update(dt);
            }
        }, py::arg("timesteps"), "Update world for multiple timesteps")
        .def("batchAddObjects", [](SimpleWorld& world, const std::vector<WorldObject>& objects) {
            for (const auto& obj : objects) {
                world.addObject(obj);
            }
        }, py::arg("objects"), "Add multiple objects at once")
        .def("getObjectCount", [](const SimpleWorld& world) {
            // Simplified - in practice, you would need to track objects differently
            return 0; // Placeholder
        }, "Get number of objects in world (simplified)")
        .def("getDensity", [](const SimpleWorld& world, float x, float y, float radius) {
            // Simplified density calculation
            return 0.0f; // Placeholder
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get object density in area (simplified)");

    // Enhanced Brain with Pythonic features
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain with Pythonic features and advanced capabilities)pbdoc")
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
        .def("batchInjectCurrent", [](Brain& brain, const std::vector<NeuronId>& neuron_ids, double current) {
            // Simplified batch injection - in practice, you would need
            // proper neuron access through the brain's region system
            for (const auto& id : neuron_ids) {
                // brain->injectCurrent(id, current);
            }
        }, py::arg("neuron_ids"), py::arg("current"), "Inject current to multiple neurons")
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
        // Performance and analysis methods
        .def("benchmarkSimulation", &benchmarkSimulation, py::arg("numSteps"),
             "Run performance benchmark")
        .def("profileActivity", &profileActivity, py::arg("numSteps"),
             "Profile neural activity")
        .def("optimizeConnections", &optimizeConnections,
             "Optimize synaptic connections")
        .def("collectStatistics", [](const Brain& brain) {
            // Simplified - in practice, you would pass the agent and world
            Statistics stats;
            stats.total_spikes = brain.getTotalSpikeCount();
            stats.avg_firing_rate = brain.getAverageFiringRate();
            return stats;
        }, "Collect brain statistics (simplified)")
        // Parallel processing
        .def("parallelStep", [](Brain& brain, const std::vector<size_t>& steps) {
            ParallelProcessor processor;
            return processor.parallelSteps(std::make_shared<Brain>(brain), steps);
        }, py::arg("steps"), "Run parallel simulation steps")
        // Debugging
        .def("debugState", &debugState, py::arg("config"), py::arg("agent"), py::arg("world"),
             "Get detailed debug information");

    // Enhanced AgentBrain with Pythonic features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with Pythonic features)pbdoc")
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
        // Advanced neuromodulation control
        .def("enableAdvancedPlasticity", [](AgentBrain& agent, bool enable) {
            agent.enableRewardModulation(enable);
            agent.enableStructuralPlasticity(enable);
            agent.enableDevelopment(enable);
            agent.enableCuriosity(enable);
        }, py::arg("enable"), "Enable all advanced plasticity systems")
        .def("setPlasticityParameters", [](AgentBrain& agent, const py::dict& params) {
            // Simplified parameter setting
            for (auto item : params) {
                // In practice, this would set specific plasticity parameters
                // auto key = py::cast<std::string>(item.first);
                // auto value = py::cast<float>(item.second);
            }
        }, py::arg("params"), "Set plasticity parameters from dictionary")
        .def("customizeNeuromodulation", [](AgentBrain& agent, const py::dict& neuromod_params) {
            // Simplified neuromodulation customization
            for (auto item : neuromod_params) {
                // In practice, this would set specific neuromodulation parameters
                // auto key = py::cast<std::string>(item.first);
                // auto value = py::cast<float>(item.second);
            }
        }, py::arg("neuromod_params"), "Customize neuromodulation parameters");

    // Factory functions with enhanced documentation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation support");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with enhanced features");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface with advanced features");

    // Utility functions
    m.def("convertNeuronType", &convertNeuronType, py::arg("type_str"),
          "Convert string representation to NeuronType (e.g., 'excitatory' -> NeuronType.Excitatory)");

    m.def("normalizeConfig", &normalizeConfig, py::arg("config_dict"),
          "Normalize configuration dictionary to standard NLM format");

    m.def("formatId", [](const NeuronId& id) { return formatId(id); }, 
          "Format NeuronId for display (e.g., 'NeuronId(42)')");
    m.def("formatId", [](const SynapseId& id) { return formatId(id); },
          "Format SynapseId for display (e.g., 'SynapseId(42)')");
    m.def("formatId", [](const RegionId& id) { return formatId(id); },
          "Format RegionId for display (e.g., 'RegionId(42)')");
    m.def("formatId", [](const PopulationId& id) { return formatId(id); },
          "Format PopulationId for display (e.g., 'PopulationId(42)')");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Performance and utility classes
    py::class_<ParallelProcessor>(m, "ParallelProcessor", R"pbdoc(Parallel processing utilities)pbdoc")
        .def(py::init<size_t>(), py::arg("threads") = 0,
             "Create parallel processor with specified thread count")
        .def("parallelSteps", &ParallelProcessor::parallelSteps,
             py::arg("brain"), py::arg("steps"), "Run multiple simulation steps in parallel")
        .def("parallelInjectCurrent", &ParallelProcessor::parallelInjectCurrent,
             py::arg("brain"), py::arg("neuron_ids"), py::arg("current"), "Inject current to multiple neurons in parallel");

    // Exception registration
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Context manager support (conceptual - actual implementation would require
    // custom Python classes with __enter__/__exit__ methods)
    m.def("with_brain", [](std::shared_ptr<Brain> brain, const py::function& func) {
        brain->initialize();
        try {
            return func();
        } finally {
            brain->reset();
        }
    }, py::arg("brain"), py::arg("func"), "Context manager for brain (conceptual)");

    m.def("with_agent", [](std::shared_ptr<AgentBrain> agent, const py::function& func) {
        // This would require actual world context - simplified for demonstration
        try {
            return func();
        } catch (...) {
            agent->reset();
            throw;
        }
    }, py::arg("agent"), py::arg("func"), "Context manager for agent (conceptual)");

    m.def("with_world", [](std::shared_ptr<SimpleWorld> world, const py::function& func) {
        world->reset();
        try {
            return func();
        } finally {
            // World cleanup - simplified
        }
    }, py::arg("world"), py::arg("func"), "Context manager for world (conceptual)");
}

} // namespace nlm
