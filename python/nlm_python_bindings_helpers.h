// NLM Python Bindings Helper Templates
// This file provides helper templates and forward declarations for improved
// Python bindings structure and maintainability.

#ifndef NLM_PYTHON_BINDINGS_HELPERS_H
#define NLM_PYTHON_BINDINGS_HELPERS_H

#include <pybind11/pybind11.h>
#include <memory>
#include <string>
#include <pybind11/stl.h>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

namespace binding_helpers {

// ===========================================================================
// Template Helper Classes
// ===========================================================================

template<typename T>
class shared_ptr_class {
public:
    static void bind(py::module_& m, const std::string& name);
};

template<typename EnumType>
class enum_class {
public:
    static void bind(py::module_& m, const std::string& name);
};

template<typename Class, typename T>
class property_class {
public:
    static void bind_readonly(py::class_<Class>& cls, const std::string& name, 
                             T (Class::*getter)() const);
};

// ===========================================================================
// Factory Functions
// ===========================================================================

py::object create_object(const std::string& className, py::args args);

// ===========================================================================
// Module Initialization
// ===========================================================================

void bind_core_types(py::module_& m);
void bind_config(py::module_& m);
void bind_random(py::module_& m);
void bind_logger(py::module_& m);
void bind_simulation_clock(py::module_& m);

void bind_brain_system(py::module_& m);
void bind_neuron(py::module_& m);
void bind_synapse(py::module_& m);
void bind_neural_population(py::module_& m);
void bind_neural_region(py::module_& m);

void bind_dynamics(py::module_& m);
void bind_plasticity(py::module_& m);

void bind_memory(py::module_& m);
void bind_cognition(py::module_& m);

void bind_neuromodulation(py::module_& m);
void bind_development(py::module_& m);

void bind_sensory(py::module_& m);
void bind_motor(py::module_& m);
void bind_environment(py::module_& m);

void bind_agent(py::module_& m);
void bind_world(py::module_& m);

void bind_experiments(py::module_& m);
void bind_visualization(py::module_& m);

// ===========================================================================
// Convenience Functions
// ===========================================================================

void add_brain_convenience_methods(py::class_<std::shared_ptr<Brain>>& brain_class);
void add_world_convenience_methods(py::class_<std::shared_ptr<SimpleWorld>>& world_class);
void add_config_convenience_methods(py::class_<std::shared_ptr<Config>>& config_class);

} // namespace binding_helpers

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_HELPERS_H
