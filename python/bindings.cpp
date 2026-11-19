// Type bindings - contains simple types and enumerations
#include "type_bindings.h"

namespace nlm {

void bind_types(py::module_& m);
void bind_config(py::module_& m);
void bind_sensory(py::module_& m);
void bind_actions(py::module_& m);
void bind_world(py::module_& m);
void bind_agent(py::module_& m);
void bind_brain(py::module_& m);
void bind_factories(py::module_& m);
void bind_utilities(py::module_& m);

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Comprehensive neural simulation API
        - Advanced factory methods for complex object creation
        - Error handling and input validation
        - Type conversion utilities for interoperability
        - Performance monitoring and debugging tools
        - Statistics and analysis functions
        
        Examples:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all types and enumerations
    bind_types(m);

    // Bind core NLM classes
    bind_config(m);
    bind_brain(m);

    // Bind sensory input classes
    bind_sensory(m);

    // Bind action and agent classes
    bind_actions(m);

    // Bind world and simulation classes
    bind_world(m);

    // Bind factory functions
    bind_factories(m);

    // Bind utility functions
    bind_utilities(m);
}

// Implementation files are included via the headers and separate .cpp files