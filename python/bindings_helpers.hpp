#pragma once

#include <pybind11/pybind11.h>
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function declarations
py::dict getBrainStats(const Brain& brain);
py::dict getIntegrationStatus(const Brain& brain);
std::shared_ptr<Brain> createAdvancedBrain(const std::shared_ptr<Config>& config, 
                                           const py::dict& options);
void demonstrateAdvancedLearning(std::shared_ptr<Brain>& brain, 
                                 std::shared_ptr<SimpleWorld>& world,
                                 std::shared_ptr<AgentBrain>& agent,
                                 size_t steps);

} // namespace nlm
