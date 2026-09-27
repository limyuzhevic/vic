#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <optional>

namespace py = pybind11;
namespace nlm {

// Forward declarations
class Config;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class SimpleWorld;
class Brain;
class AgentBrain;

/**
 * Template-based ID type binding generator
 * 
 * Generates bindings for strongly-typed ID classes with common patterns:
 * - Default constructor
 * - Constructor from uint64_t value
 * - Value property (read-write)
 * - Index property (read-only)
 * - Equality operators
 * - Hash function
 * - String representation
 * 
 * Usage:
 *   BIND_ID_TYPE(NeuronId, "NeuronId")
 *   BIND_ID_TYPE(SynapseId, "SynapseId")
 *   BIND_ID_TYPE(RegionId, "RegionId")
 *   BIND_ID_TYPE(PopulationId, "PopulationId")
 */

template<typename IDType, const char* Name>
void bindIDType(py::module& m) {
    py::class_<IDType>(m, Name)
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const IDType& id) {
            return "<" + std::string(Name) + ": " + std::to_string(id.value) + ">";
        });
}

/**
 * Simple class binding generator for classes with only getter/setter methods
 * 
 * Template for classes with common pattern:
 * - Constructor
 * - Getters for properties
 * - Setters for properties
 * 
 * Usage:
 *   BIND_SIMPLE_CLASS(ClassName)
 *   BIND_SIMPLE_CLASS(AgentBody)
 */

template<typename T>
void bindSimpleClass(py::module& m, const char* name, const char* doc = "") {
    py::class_<T>(m, name, py::doc(doc))
        .def(py::init<>())
        .def_readwrite("x", &T::x)
        .def_readwrite("y", &T::y);
}

/**
 * Class with constructor and simple methods generator
 * 
 * Template for classes with:
 * - Constructor with parameters
 * - Simple get/set methods
 * 
 * Usage:
 *   BIND_CONSTRUCTOR_CLASS(ClassName, "brief doc", {param1, param2})
 *   BIND_CONSTRUCTOR_CLASS(Action, "Action representation", {"type", "parameters"})
 */
template<typename T, typename... Args>
void bindConstructorClass(py::module& m, const char* name, 
                          const char* doc = "", std::initializer_list<std::pair<const char*, const char*>> methods = {}) {
    auto cls = py::class_<T>(m, name, py::doc(doc))
        .def(py::init<Args...>(), 
             py::arg("param1"), py::arg("param2"), py::arg("param3") = Args{})
        .def("getType", &T::getType)
        .def("setType", &T::setType, py::arg("type"))
        .def("getParameters", &T::getParameters)
        .def("setParameters", &T::setParameters, py::arg("params"))
        .def("getName", &T::getName)
        .def("clone", &T::clone);
    
    // Add additional methods if provided
    for (const auto& method : methods) {
        cls.def(method.first, method.second);
    }
}

/**
 * Enhanced property-based API generator
 * 
 * Automatically generates read-only properties for commonly accessed attributes
 * using pybind11's property system. Supports property type detection and common
 * getter/setter patterns.
 * 
 * Usage:
 *   bindPropertyOnly<ClassName>(m, "Brain", "Brain class with property-based API");
 *   bindPropertyOnly<ClassName>(m, "Neuron", "Neuron class with property-based API");
 */
template<typename T>
void bindPropertyOnly(py::module& m, const char* name, const char* doc = "") {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Read-only properties for common attribute patterns
    // Based on typical getter method names or member variable access
    
    // ID types
    if constexpr (requires { typename T::IdType; }) {
        cls.def_readonly("id", &T::getId);
    }
    
    // Float properties (common in neural models)
    if constexpr (requires { &T::getMembranePotential; }) {
        cls.def_readonly("membranePotential", &T::getMembranePotential);
    }
    if constexpr (requires { &T::getWeight; }) {
        cls.def_readonly("weight", &T::getWeight);
    }
    if constexpr (requires { &T::getThreshold; }) {
        cls.def_readonly("threshold", &T::getThreshold);
    }
    if constexpr (requires { &T::getRate; }) {
        cls.def_readonly("rate", &T::getRate);
    }
    if constexpr (requires { &T::getDelay; }) {
        cls.def_readonly("delay", &T::getDelay);
    }
    if constexpr (requires { &T::getEfficacy; }) {
        cls.def_readonly("efficacy", &T::getEfficacy);
    }
    
    // String properties
    if constexpr (requires { &T::getName; }) {
        cls.def_readonly("name", &T::getName);
    }
    if constexpr (requires { &T::getDescription; }) {
        cls.def_readonly("description", &T::getDescription);
    }
    
    // Count/size properties
    if constexpr (requires { &T::getCount; }) {
        cls.def_readonly("count", &T::getCount);
    }
    if constexpr (requires { &T::getSize; }) {
        cls.def_readonly("size", &T::getSize);
    }
    if constexpr (requires { &T::getTotal; }) {
        cls.def_readonly("total", &T::getTotal);
    }
    if constexpr (requires { &T::getActiveCount; }) {
        cls.def_readonly("activeCount", &T::getActiveCount);
    }
    
    // State properties
    if constexpr (requires { &T::getState; }) {
        cls.def_readonly("state", &T::getState);
    }
    if constexpr (requires { &T::getFiringState; }) {
        cls.def_readonly("firingState", &T::getFiringState);
    }
    
    // Timestamp properties
    if constexpr (requires { &T::getTimestamp; }) {
        cls.def_readonly("timestamp", &T::getTimestamp);
    }
    
    // Simulation properties
    if constexpr (requires { &T::getStep; }) {
        cls.def_readonly("step", &T::getStep);
    }
    if constexpr (requires { &T::getTime; }) {
        cls.def_readonly("time", &T::getTime);
    }
    
    // Progress/completion properties
    if constexpr (requires { &T::getProgress; }) {
        cls.def_readonly("progress", &T::getProgress);
    }
    if constexpr (requires { &T::getCompletion; }) {
        cls.def_readonly("completion", &T::getCompletion);
    }
    
    // Add helper method to access raw member if needed
    cls.def("__repr__", [](const T& self) {
        return "<" + std::string(name) + ">";
    });
}

/**
 * Enhanced property-based API generator for read-write properties
 * 
 * Supports both getting and setting for commonly accessed attributes
 * with proper validation and property metadata.
 * 
 * Usage:
 *   bindEnhancedPropertyOnly<ClassName>(m, "Config", "Configuration class with property-based API");
 */
template<typename T>
void bindEnhancedPropertyOnly(py::module& m, const char* name, const char* doc = "") {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Read-write properties for common attribute patterns
    
    // String properties with setter
    if constexpr (requires { &T::getName; } && requires { &T::setName; }) {
        cls.def_readwrite("name", &T::getName, &T::setName);
    }
    if constexpr (requires { &T::getDescription; } && requires { &T::setDescription; }) {
        cls.def_readwrite("description", &T::getDescription, &T::setDescription);
    }
    
    // Float properties with setter
    if constexpr (requires { &T::getValue; } && requires { &T::setValue; }) {
        cls.def_readwrite("value", &T::getValue, &T::setValue);
    }
    if constexpr (requires { &T::getWeight; } && requires { &T::setWeight; }) {
        cls.def_readwrite("weight", &T::getWeight, &T::setWeight);
    }
    if constexpr (requires { &T::getRate; } && requires { &T::setRate; }) {
        cls.def_readwrite("rate", &T::getRate, &T::setRate);
    }
    if constexpr (requires { &T::getThreshold; } && requires { &T::setThreshold; }) {
        cls.def_readwrite("threshold", &T::getThreshold, &T::setThreshold);
    }
    if constexpr (requires { &T::getDelay; } && requires { &T::setDelay; }) {
        cls.def_readwrite("delay", &T::getDelay, &T::setDelay);
    }
    if constexpr (requires { &T::getMembranePotential; } && requires { &T::setMembranePotential; }) {
        cls.def_readwrite("membranePotential", &T::getMembranePotential, &T::setMembranePotential);
    }
    if constexpr (requires { &T::getEfficacy; } && requires { &T::setEfficacy; }) {
        cls.def_readwrite("efficacy", &T::getEfficacy, &T::setEfficacy);
    }
    
    // Integer properties with validation
    if constexpr (requires { &T::getCount; } && requires { &T::setCount; }) {
        cls.def_readwrite("count", &T::getCount, &T::setCount);
    }
    if constexpr (requires { &T::getSize; } && requires { &T::setSize; }) {
        cls.def_readwrite("size", &T::getSize, &T::setSize);
    }
    
    // Boolean properties
    if constexpr (requires { &T::getEnabled; } && requires { &T::setEnabled; }) {
        cls.def_readwrite("enabled", &T::getEnabled, &T::setEnabled);
    }
    if constexpr (requires { &T::getActive; } && requires { &T::setActive; }) {
        cls.def_readwrite("active", &T::getActive, &T::setActive);
    }
    
    // Add some computed properties as read-only
    cls.def("__repr__", [](const T& self) {
        return "<" + std::string(name) + ">";
    });
    
    // Add property validation helper
    cls.def("validateProperties", []() {
        return true;  // Placeholder for property validation
    });
}

/**
 * Automatic property binding generator
 * 
 * Automatically detects and binds properties for any class
 * using reflection-like techniques and common patterns.
 * 
 * Usage:
 *   autoBindProperties<ClassName>(m, "Brain");
 */
template<typename T>
void autoBindProperties(py::module& m, const char* name, const char* doc = "") {
    // Call both read-only and read-write binding generators
    bindPropertyOnly<T>(m, name, doc);
    bindEnhancedPropertyOnly<T>(m, name, doc);
}

/**
 * Context manager generator for classes that support context management
 * 
 * Creates Python context manager support using pybind11's context manager protocol.
 * 
 * Usage:
 *   BIND_CONTEXT_MANAGER(ClassName)
 *   BIND_CONTEXT_MANAGER(SimpleWorld)
 */

template<typename T>
void bindContextManager(py::module& m, const char* name) {
    py::class_<T>(m, name)
        .def(py::init<>())
        .def("__enter__", [](T& self) { return &self; })
        .def("__exit__", [](T& self, py::object, py::object, py::object) {
            // Exit cleanup if needed
        });
}

/**
 * Iterator support generator for collections
 * 
 * Creates Python iterator support for std::vector and similar containers.
 * 
 * Usage:
 *   BIND_ITERATOR(std::vector<float>, "float_vector")
 *   BIND_ITERATOR(std::vector<NeuronId>, "neuron_id_vector")
 */
template<typename ContainerType>
void bindIterator(py::module& m, const char* name) {
    py::class_<ContainerType>(m, name)
        .def(py::init<>())
        .def(py::init<typename ContainerType::value_type>(), py::arg("value"))
        .def("__iter__", [](const ContainerType& self) {
            return py::make_iterator(self.begin(), self.end());
        }, py::return_value_policy::reference_internal)
        .def("append", &ContainerType::push_back, py::arg("value"))
        .def("clear", &ContainerType::clear)
        .def("size", &ContainerType::size)
        .def("empty", &ContainerType::empty);
}

/**
 * Error handling helper for bindings
 * 
 * Registers Python exceptions for C++ exceptions in bindings.
 * 
 * Usage:
 *   BIND_EXCEPTIONS(m)
 */
inline void bindExceptions(py::module& m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "RuntimeError");
}

/**
 * Convenience macro to generate complete ID type bindings in one line
 * 
 * Equivalent to calling bindIDType directly but more concise.
 * 
 * Usage: BIND_ID_TYPE(NeuronId, "NeuronId")
 */
#define BIND_ID_TYPE(ID_TYPE, NAME) \
    template<> \
    inline void bindIDType<ID_TYPE, NAME>(py::module& m)

/**
 * Convenience macro to generate simple class bindings
 * 
 * Equivalent to calling bindSimpleClass directly but more concise.
 * 
 * Usage: BIND_SIMPLE_CLASS(AgentBody)
 */
#define BIND_SIMPLE_CLASS(CLASS_NAME) \
    template<> \
    inline void bindSimpleClass<CLASS_NAME>(py::module& m, const char* name, const char* doc)

/**
 * Convenience macro to generate property-only bindings
 * 
 * Equivalent to calling bindPropertyOnly directly but more concise.
 * 
 * Usage: BIND_PROPERTY_ONLY(ClassName, "value", "name")
 */
#define BIND_PROPERTY_ONLY(CLASS_NAME, ...) \
    template<> \
    inline void bindPropertyOnly<CLASS_NAME>(py::module& m, const char* name, const char* doc, const char* firstProp, __VA_ARGS__)

} // namespace nlm
