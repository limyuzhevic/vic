#include "main_bindings.h"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register basic exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register basic type conversion utilities
    register_type_conversions(m);

    // Include all binding modules
    include_basic_types(m);
    include_config(m);
    include_sensory(m);
    include_action(m);
    include_agent(m);
    include_world(m);
    include_brain(m);
    include_agentbrain(m);
    include_factories(m);
}

} // namespace nlm