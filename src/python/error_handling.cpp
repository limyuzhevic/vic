// Error Handling Module - error_handling.cpp
#ifndef ERROR_HANDLING_MODULE_H
#define ERROR_HANDLING_MODULE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) 
        : NLMException("Simulation Error: " + message) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) 
        : NLMException("Validation Error: " + message) {}
};

// Enhanced error checking utilities
bool validate_config(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw ConfigurationError("Configuration is null");
    }
    
    // Check for required brain configuration
    if (!config->has("brain.neuron_count")) {
        throw ConfigurationError("Missing required parameter: brain.neuron_count");
    }
    
    // Validate neuron count is reasonable
    auto neuron_count = config->getOr<int64_t>("brain.neuron_count", 1000);
    if (neuron_count < 100 || neuron_count > 100000) {
        throw ValidationError("Neuron count must be between 100 and 100000");
    }
    
    return true;
}

// Enhanced logging utilities
void log_experiment_results(const std::string& experiment_name,
                           const std::map<std::string, double>& results) {
    std::cout << "=== Experiment Results: " << experiment_name << " ===" << std::endl;
    for (const auto& pair : results) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << "=== End Results ===" << std::endl;
}

// Batch operation utilities
class BatchProcessor {
public:
    std::vector<std::map<std::string, double>> process_episodes(
        const std::shared_ptr<NLMExperiment>& experiment,
        size_t num_episodes) {
        std::vector<std::map<std::string, double>> all_results;
        
        for (size_t i = 0; i < num_episodes; ++i) {
            std::cout << "Processing episode " << (i + 1) << "/" << num_episodes << std::endl;
            
            // Reset for new episode
            experiment->brain->reset();
            experiment->world->reset();
            experiment->agent->reset();
            experiment->setup();
            
            auto result = experiment->runEpisode();
            all_results.push_back(result);
        }
        
        return all_results;
    }
    
    std::map<std::string, double> aggregate_results(
        const std::vector<std::map<std::string, double>>& results) {
        std::map<std::string, double> aggregated;
        size_t count = results.size();n        
        if (count == 0) return aggregated;
        
        std::map<std::string, double> sum;
        for (const auto& result : results) {
            for (const auto& pair : result) {
                sum[pair.first] += pair.second;
            }
        }
        
        for (const auto& pair : sum) {
            aggregated[pair.first] = pair.second / count;
        }
        
        return aggregated;
    }
};

void bind_error_handling(pybind11::module_ &m) {
    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ValidationError>(m, "ValidationError");
    
    // Utility functions
    m.def("validate_config", &validate_config,
          py::arg("config"),
          "Validate configuration for required parameters and constraints");
    
    m.def("log_experiment_results", &log_experiment_results,
          py::arg("experiment_name"), py::arg("results"),
          "Log experiment results in a formatted way");
    
    // Batch processor class
    py::class_<BatchProcessor>(m, "BatchProcessor",
        "Utility class for batch processing and aggregating experiment results.")
        .def(py::init<>())
        .def("process_episodes", &BatchProcessor::process_episodes,
             py::arg("experiment"), py::arg("num_episodes"),
             "Process multiple episodes and return all results")
        .def("aggregate_results", &BatchProcessor::aggregate_results,
             py::arg("results"),
             "Aggregate and average results from multiple episodes");
}

} // namespace nlm

#endif // ERROR_HANDLING_MODULE_H
