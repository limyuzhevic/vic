#pragma once

#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;
namespace nlm {

// Template for common ID type bindings (NeuronId, SynapseId, RegionId, PopulationId)
template<typename T>
void bindIdType(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const T& id) {
            std::string className = name;
            // Convert PascalCase to readable format
            size_t pos = 0;
            while ((pos = className.find('_', pos)) != std::string::npos) {
                className.replace(pos, 1, " ");
                pos++;
            }
            return "<" + className + ": " + std::to_string(id.value) + ">";
        });
}

// Simplified template for ID types without hash/repr (if PopulationId differs)
template<typename T>
void bindIdTypeSimple(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=);
}

// Template for enum bindings
template<typename T>
void bindEnumType(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<T>(m, name.c_str(), doc.c_str())
        .export_values();
}

// Template for simple class with init and single method
template<typename T>
void bindSimpleClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>());
}

// Template for class with read-only properties
template<typename T>
void bindReadOnlyClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str());
}

// Template for class with custom init and methods
template<typename T>
void bindClassWithInit(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>());
}

// Helper to add documentation to methods
template<typename... Args>
void addDoc(Args&&... args) {
    // This is just for consistency - pybind11 automatically uses the docstrings
}

} // namespace nlm