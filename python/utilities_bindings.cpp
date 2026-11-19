// Utilities bindings implementation
#include "utilities_bindings.h"

namespace nlm {

void bindUtilities(py::module_& m) {
    // Error handling utilities
    m.def("throw_error", [](const std::string& message) {
        throw std::runtime_error(message);
    }, py::arg("message"), "Throw a runtime error with the given message");

    m.def("check_condition", [](bool condition, const std::string& message) {
        if (!condition) {
            throw std::invalid_argument(message);
        }
    }, py::arg("condition"), py::arg("message"), "Check a condition and throw if false");

    // Logging utilities
    m.def("log_message", [](const std::string& message, int level = 0) {
        // This would log to the configured logger
        // For now, just print to stdout
        std::cout << "[NLM " << level << "] " << message << std::endl;
    }, py::arg("message"), py::arg("level") = 0,
         "Log a message with specified level (0=INFO, 1=WARNING, 2=ERROR)");

    // Performance utilities
    m.def("time_operation", [](std::function<void()> func, const std::string& name = "Operation") {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << name << " took " << duration.count() << " ms" << std::endl;
        return duration.count();
    }, py::arg("func"), py::arg("name") = "Operation",
         "Time an operation and return its duration in milliseconds");

    // Debug utilities
    m.def("debug_info", [](const std::shared_ptr<Brain>& brain) {
        py::dict info;
        info["neuron_count"] = brain->getTotalNeuronCount();
        info["synapse_count"] = brain->getTotalSynapseCount();
        info["active_neurons"] = brain->getActiveNeuronCount();
        info["firing_neurons"] = brain->getFiringNeuronCount();
        info["average_firing_rate"] = brain->getAverageFiringRate();
        info["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        info["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        return info;
    }, py::arg("brain"), "Get debug information about a brain object");

    // Validation utilities
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            try {
                auto value = config->get(key, "");
                // Basic validation - could be extended with domain-specific checks
                if (value.empty() && !key.empty()) {
                    errors.push_back("Key \"" + key + "\" has empty value");
                }
            } catch (const std::exception& e) {
                errors.push_back("Error accessing key \"" + key + "\": " + e.what());
            }
        }
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    // Batch processing utilities
    m.def("batch_process", [](std::function<void(int)> processor, int start, int end, int step = 1) {
        for (int i = start; i <= end; i += step) {
            processor(i);
        }
    }, py::arg("processor"), py::arg("start"), py::arg("end"), py::arg("step") = 1,
         "Process a range of values using the provided function");

    // Statistics utilities
    m.def("compute_statistics", [](const std::vector<float>& data) {
        if (data.empty()) {
            throw std::invalid_argument("Cannot compute statistics on empty data");
        }
        py::dict stats;
        float sum = 0.0f;
        float min = data[0];
        float max = data[0];
        for (float val : data) {
            sum += val;
            if (val < min) min = val;
            if (val > max) max = val;
        }
        stats["count"] = static_cast<int>(data.size());
        stats["mean"] = sum / data.size();
        stats["min"] = min;
        stats["max"] = max;
        stats["range"] = max - min;
        return stats;
    }, py::arg("data"), "Compute basic statistics on a vector of floats");

    // Environment utilities
    m.def("get_environment_info", []() {
        py::dict info;
        info["platform"] = "Linux"; // Could be detected dynamically
        info["python_version"] = PYBIND11_VERSION; // Or actual Python version
        info["architecture"] = "x86_64";
        return info;
    }, "Get information about the current environment");

    // Progress tracking utilities
    m.def("progress_tracker", [](std::function<void(float)> callback, const std::string& name = "Progress") {
        // This is a utility function for creating progress tracking callbacks
        // Users can wrap their own functions with this
        return [callback, name](int current, int total) {
            float progress = (total > 0) ? (static_cast<float>(current) / total) : 0.0f;
            callback(progress);
        };
    }, py::arg("callback"), py::arg("name") = "Progress",
         "Create a progress tracking callback function");
}

} // namespace nlm