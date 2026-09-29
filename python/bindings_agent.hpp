#pragma once

#include <pybind11/pybind11.h>
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"

namespace py = pybind11;
namespace nlm {

void bind_agent(py::module_& m);

} // namespace nlm
