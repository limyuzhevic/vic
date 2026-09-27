#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>

// Forward declarations
class AdvancedConfig;

void init_advanced_config(py::module& m);

PYBIND11_MODULE(pynlm_advanced, m) {
    init_advanced_config(m);
}