// Convenience functions for NLM Python bindings
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for convenience functions
std::shared_ptr<Config> createBeginnerConfig();
std::shared_ptr<Config> createResearchConfig();
std::shared_ptr<Config> createHighDensityConfig();
void runSimulationSteps(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                       std::shared_ptr<AgentBrain> agent, size_t steps,
                       double worldTimestep, bool enableLearning,
                       bool enableDevelopment, bool enableCuriosity);
std::string generateBrainSnapshot(std::shared_ptr<Brain> brain);
void runLearningDemo(std::shared_ptr<Config> config, size_t steps, const std::string& configName);
void runPatternLearningDemo(std::shared_ptr<Config> config, size_t steps, const std::string& configName);

} // namespace nlm
