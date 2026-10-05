#pragma once

#include "BindingManager.h"
#include <algorithm>
#include <sstream>

namespace nlm {
    
    // Implementation of type_conversion utilities
    namespace type_conversion {
        int stringToInt(const std::string& str) {
            try {
                return std::stoi(str);
            } catch (const std::exception& e) {
                throw ConfigError("Cannot convert string to int: " + str + ". " + e.what());
            }
        }
        
        double stringToDouble(const std::string& str) {
            try {
                return std::stod(str);
            } catch (const std::exception& e) {
                throw ConfigError("Cannot convert string to double: " + str + ". " + e.what());
            }
        }
        
        bool stringToBool(const std::string& str) {
            std::string lower = str;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            if (lower == "true" || lower == "1" || lower == "yes") return true;
            if (lower == "false" || lower == "0" || lower == "no") return false;
            throw ConfigError("Cannot convert string to bool: " + str);
        }
    }
    
    // Helper function to register basic types
    void registerBasicTypes(pybind11::module_& m) {
        m.def("INVALID_NEURON_ID", &INVALID_NEURON_ID);
        m.def("INVALID_SYNAPSE_ID", &INVALID_SYNAPSE_ID);
        m.def("INVALID_REGION_ID", &INVALID_REGION_ID);
        m.def("INVALID_POPULATION_ID", &INVALID_POPULATION_ID);
    }
    
    // Helper function to create context managers
    template<typename T>
    py::class_<T> addContextManagerSupport(pybind11::module_& m, const std::string& name) {
        auto cls = py::class_<T>(m, name.c_str());
        
        // Add context manager protocol
        cls.def(py::self != py::self); // Dummy implementation
        
        return cls;
    }
    
    // Helper function to add properties
    template<typename T, typename PropertyType>
    void addProperty(py::class_<T>& cls, const std::string& name,
                    PropertyType (T::*getter)() const, const std::string& doc = "") {
        cls.def_property_readonly(name.c_str(), getter, doc.c_str());
    }
    
    // Helper function to add method with Pythonic naming
    template<typename T, typename ReturnType, typename... Args>
    void addMethod(py::class_<T>& cls, const std::string& pyName,
                  ReturnType (T::*method)(Args...) const,
                  const std::string& cppName = "", const std::string& doc = "") {
        std::string actualName = cppName.empty() ? pyName : cppName;
        cls.def(pyName.c_str(), method, doc.c_str());
    }
    
    // Documentation generator
    class DocumentationGenerator {
    public:
        static std::string generateModuleDoc(const std::string& name, const std::string& description) {
            std::stringstream ss;
            ss << """NLM Python Bindings Module: """ << name << """

""" << description << ""

This module provides Python bindings for NLM (Neural Learning Machine) functionality
in the """ << name << """ domain.

.. currentmodule:: """ << name << """

This module was automatically generated from C++ bindings using pybind11.
""";
            return ss.str();
        }
        
        static std::string generateClassDoc(const std::string& name, const std::string& description) {
            std::stringstream ss;
            ss << """Class """ << name << """: """ << description << """

Attributes:
    This class provides ...

Methods:
    This class provides methods for ...
""";
            return ss.str();
        }
    };
}