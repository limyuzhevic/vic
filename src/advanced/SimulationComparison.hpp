#ifndef NLM_SIMULATION_COMPARISON_HPP
#define NLM_SIMULATION_COMPARISON_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

struct ComparisonResult {
    std::string configName;
    size_t neuronCount;
    size_t synapseCount;
    double totalTime;
    double stepsPerSecond;
    double avgFiringRate;
    size_t totalSpikes;
};

class SimulationComparator {
public:
    static void compareConfigurations(const std::string& configList);
    static ComparisonResult runSingleSimulation(const std::string& configPath);
    static void printComparisonResults(const std::vector<ComparisonResult>& results);
    static std::vector<std::string> parseConfigList(const std::string& configList);
};

} // namespace nlm

#endif // NLM_SIMULATION_COMPARISON_HPP