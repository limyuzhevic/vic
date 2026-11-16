// ID Class Template for Python Bindings
// Template to eliminate code duplication in ID class definitions

#ifndef NLM_BINDINGS_ID_TEMPLATE_HPP
#define NLM_BINDINGS_ID_TEMPLATE_HPP

#include <pybind11/pybind11.h>
#include <string>
#include <cstdint>

template<typename IDType, typename Tag>
pybind11::class_<IDType> registerIdClass(pybind11::module_& m, const std::string& name) {
    return m.def_class_<IDType>(name)
        .def(py::init<>())
        .def(py::init<uint64_t>(), pybind11::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

#endif // NLM_BINDINGS_ID_TEMPLATE_HPP
