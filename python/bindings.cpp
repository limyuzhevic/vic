#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "types_bindings.hpp"
#include "config_bindings.hpp"
#include "brain_bindings.hpp"
#include "agent_bindings.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    bind_types(m);
    bind_config(m);
    bind_brain(m);
    bind_agent(m);
}

// Include the individual binding functions
void bind_types(py::module_& m);
void bind_config(py::module_& m);
void bind_brain(py::module_& m);
void bind_agent(py::module_& m);
