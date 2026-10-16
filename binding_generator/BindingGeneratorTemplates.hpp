#ifndef BINDING_GENERATOR_TEMPLATES_HPP
#define BINDING_GENERATOR_TEMPLATES_HPP

#include <string>
#include <vector>
#include <map>
#include "BindingGenerator.hpp"

namespace NlmBindingTemplates {
    
    // Template strings for generating pybind11 bindings
    // These templates provide reusable patterns for different types of bindings
    
    // Base template for simple class bindings
    const std::string SIMPLE_CLASS_TEMPLATE = R"(
py::class_<%%CLASS_NAME%%>(m, "%%CLASS_NAME%%", %%DOCSTRING%%)
    %%CONSTRUCTORS%%
    %%METHODS%%
    %%PROPERTIES%%
    %%REPR%%";

    // Template for advanced class bindings with error handling
    const std::string ADVANCED_CLASS_TEMPLATE = R"(
py::class_<%%CLASS_NAME%%>(m, "%%CLASS_NAME%%", %%DOCSTRING%%)
    %%CONSTRUCTORS%%
    %%METHODS%%
    %%ADVANCED_METHODS%%
    %%ERROR_HANDLING%%
    %%CONVENIENCE_METHODS%%
    %%PROPERTIES%%
    %%TYPE_CONVERSIONS%%
    %%REPR%%";

    // Template for enum bindings
    const std::string ENUM_TEMPLATE = R"(
py::enum_<%%ENUM_CLASS%%>(m, "%%ENUM_NAME%%", %%DOCSTRING%%)
    %%ENUM_VALUES%%
    .export_values();";

    // Template for value type bindings (like ID classes)
    const std::string VALUE_TYPE_TEMPLATE = R"(
py::class_<%%CLASS_NAME%%>(m, "%%CLASS_NAME%%", %%DOCSTRING%%)
    %%CONSTRUCTORS%%
    %%OPERATORS%%
    %%HELPER_FUNCTIONS%%
    %%REPR%%";

    // Template for container types
    const std::string CONTAINER_TEMPLATE = R"(
py::class_<%%CONTAINER_TYPE%%>(m, "%%CONTAINER_NAME%%")
    .def(py::init<>())
    .def(py::init<const std::vector<%%ELEMENT_TYPE%%>&>(), py::arg("data"))
    .def("size", &%%CONTAINER_TYPE%%::size)
    .def("empty", &%%CONTAINER_TYPE%%::empty)
    .def("begin", &%%CONTAINER_TYPE%%::begin)
    .def("end", &%%CONTAINER_TYPE%%::end)
    .def("front", &%%CONTAINER_TYPE%%::front)
    .def("back", &%%CONTAINER_TYPE%%::back)
    .def("push_back", &%%CONTAINER_TYPE%%::push_back, py::arg("value"))
    .def("clear", &%%CONTAINER_TYPE%%::clear)
    .def("__len__", &%%CONTAINER_TYPE%%::size)
    .def("__iter__", [](const %%CONTAINER_TYPE%%& container) {
        return py::make_iterator(container.begin(), container.end());
    }, py::keep_alive<0, 1>())
    .def("__getitem__", [](const %%CONTAINER_TYPE%%& container, size_t index) {
        auto it = container.begin();
        std::advance(it, index);
        return *it;
    });";

    // Template for factory functions
    const std::string FACTORY_TEMPLATE = R"(
m.def("create%%CLASS_NAME%%", [](%%CTOR_ARGS%%) -> std::shared_ptr<%%CLASS_NAME%%> {
    return std::make_shared<%%CLASS_NAME%%>(%%CTOR_CALL%%);
}, %%ARGUMENTS%%,
     "Create a new %%CLASS_NAME%% instance");";

    // Template for type conversion functions
    const std::string TYPE_CONVERSION_TEMPLATE = R"(
// Type conversion helpers for %%CLASS_NAME%%
m.def("%%CONVERSION_NAME%%", [](const py::object& obj) -> %%TARGET_TYPE%% {
    return obj.cast<%%TARGET_TYPE%%>();
}, "Convert Python object to %%TARGET_TYPE%%");";

    // Template for error handling wrappers
    const std::string ERROR_HANDLING_TEMPLATE = R"(
// Error handling wrapper for %%METHOD_NAME%%
m.def("%%SAFE_METHOD_NAME%%", [](%%CLASS_NAME%%& self%%METHOD_ARGS%%) {
    try {
        return self.%%METHOD_NAME%%(%%METHOD_CALL_ARGS%%);
    } catch (const std::exception& e) {
        py::register_exception_translator([](std::exception_ptr p) {
            if (p) {
                try {
                    std::rethrow_exception(p);
                } catch (const std::exception& ex) {
                    PyErr_SetString(PyExc_RuntimeError, ex.what());
                }
            }
        });
        throw;
    }
}, %%ARGUMENTS%%);";

    // Template for documentation generation
    const std::string DOCUMENTATION_TEMPLATE = R"(/// %%CLASS_NAME%%: %%DOCSTRING%%
/// 
/// Example usage:
/// ```python
/// # %%EXAMPLE_CODE%%
/// ```
/// 
/// Methods:
/// %%METHODS_DOCSTRING%%

    // Convenience method templates
    const std::string CONVENIENCE_TEMPLATE = R"(
    .def("%%METHOD_NAME%%", [](%%CLASS_NAME%%& self%%TEMPLATE_ARGS%%) %%RETURN_TYPE%% {
        %%IMPLEMENTATION%%
    }, %%ARGUMENTS%%,
         "%%DOCSTRING%% (convenience method)");";

    // Batch operation template
    const std::string BATCH_TEMPLATE = R"(
    .def("%%METHOD_NAME%%", [](%%CLASS_NAME%%& self, const std::vector<%%PARAM_TYPE%%>& values) {
        std::vector<%%RETURN_TYPE%%> results;
        for (const auto& value : values) {
            results.push_back(self.%%METHOD_NAME%%(value));
        }
        return results;
    }, py::arg("values"),
         "Apply method to each element in collection (convenience method)");";

    // Iterator template
    const std::string ITERATOR_TEMPLATE = R"(
    .def("%%ITERATOR_NAME%%", [](%%CLASS_NAME%%& self) {
        return py::make_iterator(self.begin(), self.end());
    }, py::keep_alive<0, 1>(),
         "Get iterator for %%COLLECTION_NAME%% (convenience method)");";

    // Comparison operators template
    const std::string COMPARISON_TEMPLATE = R"(
    .def("%%COMPARATOR_NAME%%", [](const %%CLASS_NAME%%& a, const %%CLASS_NAME%%& b) {
        return a.%%COMPARATOR_IMPL%%(b);
    }, py::arg("other"),
         "%%COMPARATOR_DOCSTRING%%");";

    // Statistics/summary template
    const std::string STATISTICS_TEMPLATE = R"(
    .def("%%STATISTICS_NAME%%", []() {
        py::dict stats;
        %%STATISTICS_IMPLEMENTATION%%
        return stats;
    },
         "Get system statistics (convenience method)");";

    // Configuration template
    const std::string CONFIGURATION_TEMPLATE = R"(
    // Configuration section
    .def("%%CONFIG_METHOD%%", []() {
        py::dict config;
        %%CONFIG_IMPLEMENTATION%%
        return config;
    }, "Get configuration (convenience method)");";

    // Utility function templates
    const std::string UTILITY_TEMPLATE = R"(
    // Utility function
    m.def("%%UTILITY_NAME%%", [](%%ARG_TYPES%%) %%RETURN_TYPE%% {
        %%UTILITY_IMPLEMENTATION%%
    }, %%ARGUMENTS%%,
         "%%UTILITY_DOCSTRING%%");";

    // Error wrapper template
    const std::string ERROR_WRAPPER_TEMPLATE = R"(
    // Safe wrapper with error handling
    m.def("%%SAFE_NAME%%", [](%%ARG_TYPES%%) -> py::object {
        try {
            %%SAFE_IMPLEMENTATION%%
        } catch (const std::exception& e) {
            py::register_exception_translator([](std::exception_ptr p) {
                if (p) {
                    try {
                        std::rethrow_exception(p);
                    } catch (const std::exception& ex) {
                        PyErr_SetString(PyExc_RuntimeError, ex.what());
                    }
                }
            });
            return py::none();
        }
    }, %%ARGUMENTS%%,
         "Safe %%SAFE_NAME%% with error handling");";

    // Main binding template file - combines all templates
    const std::string MAIN_BINDING_TEMPLATE = R"(
// Generated by NLM Binding Generator
// File: generated_bindings/bindings.cpp
// Generated on: %%DATE%%
// Binding level: %%BINDING_LEVEL%%

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

%%INCLUDES%%

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(%%MODULE_NAME%%, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, Neuron, Synapse and related components.

        This is %%BINDING_LEVEL%% binding generation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    %%CORE_BINDINGS%%

    %%ENUM_BINDINGS%%

    %%FACTORY_FUNCTIONS%%

    %%UTILITY_FUNCTIONS%%
}

} // namespace nlm
";

} // namespace NlmBindingTemplates

#endif // BINDING_GENERATOR_TEMPLATES_HPP