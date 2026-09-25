#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../benchmark/Benchmark.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations for helper function implementations
std::shared_ptr<Config> createStandardConfig();
std::shared_ptr<Brain> createTestBrain();
BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps);

} // namespace nlm
