// Global helper functions for Python bindings
// These provide convenient factory and utility functions for Python users

#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../benchmark/Benchmark.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace py = pybind11;
namespace nlm {

// Factory functions for common objects
std::shared_ptr<Config> createStandardConfig();
std::shared_ptr<Brain> createTestBrain();
BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps);

} // namespace nlm
