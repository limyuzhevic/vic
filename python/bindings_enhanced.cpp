#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// Enhanced error types for Python bindings
class NLMPythonError : public std::runtime_error {
public:
    NLMPythonError(const std::string& message, const std::string& context = "")
        : std::runtime_error("[NLM Python] " + message + 
                           (context.empty() ? "" : " (context: " + context + ")")) {}
};

class ConfigError : public NLMPythonError {
public:
    ConfigError(const std::string& message, const std::string& context = "")
        : NLMPythonError(message, context) {}
};

class BrainError : public NLMPythonError {
public:
    BrainError(const std::string& message, const std::string& context = "")
        : NLMPythonError(message, context) {}
};

class AgentError : public NLMPythonError {
public:
    AgentError(const std::string& message, const std::string& context = "")
        : NLMPythonError(message, context) {}
};

class WorldError : public NLMPythonError {
public:
    WorldError(const std::string& message, const std::string& context = "")
        : NLMPythonError(message, context) {}
};

// Python helper functions
namespace py_helpers {
    // Convert Python dict to C++ unordered_map
    std::unordered_map<std::string, std::string> dictToMap(py::object dict_obj);
    
    // Convert C++ unordered_map to Python dict
    py::dict mapToDict(const std::unordered_map<std::string, std::string>& map);
    
    // Convert Python list to C++ vector
    template<typename T>
    std::vector<T> listToVector(py::list list_obj);
    
    // Convert C++ vector to Python list
    template<typename T>
    py::list vectorToList(const std::vector<T>& vec);
    
    // Safe Python exception handling
    py::object tryCatch(std::function<py::object()> func);
    
    // Performance timing
    struct Timer {
        std::chrono::high_resolution_clock::time_point start;
        Timer() : start(std::chrono::high_resolution_clock::now()) {}
        double elapsed() const {
            return std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - start
            ).count();
        }
    };
    
    // String utilities
    std::string trim(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
}

// Enhanced Python bindings module
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ==============================
        
        This is the enhanced Python API for NLM (Neural Learning Machine),
        providing access to all the advanced features including:
        
        * Enhanced configuration system with validation
        * Comprehensive error handling and logging
        * Research tools and extensibility
        * Performance profiling and monitoring
        * Visualization and debugging tools
        * Advanced agent management
        * Custom experiment support
        
        Key improvements over the original API:
        
        1. Type-safe configuration with validation
        2. Detailed error messages with context
        3. Research experiment framework
        4. Performance monitoring and profiling
        5. Enhanced visualization capabilities
        6. Better Python integration and usability
        7. Comprehensive documentation and examples
        
        For the original API, use `import pynlm`.
        This enhanced API provides additional features and is recommended for new projects.
    )pbdoc";

    // Register custom exception types
    py::register_exception_translator<ConfigError>([](const ConfigError& e) {
        return PyErr_SetString(PyExc_RuntimeError, e.what());
    });
    
    py::register_exception_translator<BrainError>([](const BrainError& e) {
        return PyErr_SetString(PyExc_RuntimeError, e.what());
    });
    
    py::register_exception_translator<AgentError>([](const AgentError& e) {
        return PyErr_SetString(PyExc_RuntimeError, e.what());
    });
    
    py::register_exception_translator<WorldError>([](const WorldError& e) {
        return PyErr_SetString(PyExc_RuntimeError, e.what());
    });
    
    // Helper functions
    m.def("trim", &py_helpers::trim, "Trim whitespace from string");
    m.def("starts_with", &py_helpers::startsWith, py::arg("str"), py::arg("prefix"),
          "Check if string starts with prefix");
    m.def("ends_with", &py_helpers::endsWith, py::arg("str"), py::arg("suffix"),
          "Check if string ends with suffix");
    
    // Configuration helper functions
    m.def("load_config_from_dict", &Config::loadFromMap,
          py::arg("config_dict"), "Load configuration from Python dictionary");
    
    // Performance monitoring
    m.def("time_function", py_helpers::tryCatch,
          py::arg("func"), "Execute function and return result or raise exception");
    
    // Error handling utilities
    m.def("try_catch", py_helpers::tryCatch,
          py::arg("func"), "Execute function with exception handling");
    
    // Version and information
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "Enhanced NLM Python Bindings";
    m.attr("__author__") = "NLM Research Project";
    m.attr("__license__") = "MIT";
    m.attr("__copyright__") = "Copyright (c) 2026 NLM Research Project";
    
    // Compatibility layer - re-export original API
    py::module original_api = py::module_::import("pynlm");
    m.add_object("original_api", original_api.ptr());
    
    // Enhanced API features
    m.def("get_enhanced_version", []() {
        return std::map<std::string, std::string>{
            {"version", "2.0.0"},
            {"description", "Enhanced NLM Python Bindings"},
            {"features", "Configuration validation, Error handling, Research tools, Performance monitoring, Visualization, Advanced API"}
        };
    }, "Get enhanced API version information");
}

// Helper function implementations
namespace py_helpers {

std::unordered_map<std::string, std::string> dictToMap(py::object dict_obj) {
    std::unordered_map<std::string, std::string> result;
    if (py::isinstance(dict_obj, py::dict())) {
        for (auto item : dict_obj) {
            std::string key = py::cast<std::string>(item.first);
            std::string value = py::cast<std::string>(item.second);
            result[key] = value;
        }
    }
    return result;
}

py::dict mapToDict(const std::unordered_map<std::string, std::string>& map) {
    py::dict result;
    for (const auto& pair : map) {
        result[pair.first] = pair.second;
    }
    return result;
}

template<typename T>
std::vector<T> listToVector(py::list list_obj) {
    std::vector<T> result;
    result.reserve(py::len(list_obj));
    for (auto item : list_obj) {
        result.push_back(py::cast<T>(item));
    }
    return result;
}

template<typename T>
py::list vectorToList(const std::vector<T>& vec) {
    py::list result;
    for (const auto& item : vec) {
        result.append(item);
    }
    return result;
}

py::object tryCatch(std::function<py::object()> func) {
    try {
        return func();
    } catch (const std::exception& e) {
        // Re-raise as Python exception
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return py::none();
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        return py::none();
    }
}

std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace py_helpers
