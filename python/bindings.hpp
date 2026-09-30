#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindIdentifiers(py::module_ &m);
void bindEnumerations(py::module_ &m);
void bindConfig(py::module_ &m);
void bindSensory(py::module_ &m);
void bindMotor(py::module_ &m);
void bindWorld(py::module_ &m);
void bindBrain(py::module_ &m);
void bindAgent(py::module_ &m);
void bindFactory(py::module_ &m);
void bindVersion(py::module_ &m);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";

    // Bind all components
    bindIdentifiers(m);
    bindEnumerations(m);
    bindConfig(m);
    bindSensory(m);
    bindMotor(m);
    bindWorld(m);
    bindBrain(m);
    bindAgent(m);
    bindFactory(m);
    bindVersion(m);

    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
}
