#ifndef NLM_ADVANCED_VISUALIZATION_HPP
#define NLM_ADVANCED_VISUALIZATION_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

class AdvancedVisualization {
public:
    static void createNeuronConnectivityPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
    static void createSpikingActivityPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
    static void createPlasticityEvolutionPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
    static void createPerformanceCharts(const std::vector<std::pair<double, double>>& data, const std::string& outputFile);
    static void createNetworkTopology(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
    static void createHeatmapVisualization(const std::shared_ptr<Brain>& brain, const std::string& outputFile);
};

} // namespace nlm

#endif // NLM_ADVANCED_VISUALIZATION_HPP