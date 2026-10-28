// Advanced configuration management utilities for Python
#ifndef NLM_PYTHON_ADVANCED_CONFIG_H
#define NLM_PYTHON_ADVANCED_CONFIG_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace py = pybind11;

// Simple JSON implementation for Python bindings
class SimpleJSON {
public:
    static std::string to_string(const std::map<std::string, std::string>& map) {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : map) {
            if (!first) ss << ", ";
            ss << "\"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    static std::string dump(int indent = 0) {
        return "{}";
    }
};

void registerAdvancedConfigUtils(py::module_& m);

#endif // NLM_PYTHON_ADVANCED_CONFIG_H

