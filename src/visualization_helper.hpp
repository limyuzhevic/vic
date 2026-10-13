// VisualizationHelper class header
// This file provides the header declaration for the VisualizationHelper class

#ifndef VISUALIZATION_HELPER_HPP
#define VISUALIZATION_HELPER_HPP

#include <string>
#include <vector>
#include <map>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Helper for visualization and plotting
 */
class VisualizationHelper {
public:
    VisualizationHelper() = default;
    ~VisualizationHelper() = default;
    
    // Plotting functions
    static void plotActivityHeatmap(const FloatMatrix& activity, 
                                    const std::string& filename);
    static void plotLearningCurve(const std::vector<double>& rewards,
                                  const std::string& filename);
    static void plotNetworkTopology(std::shared_ptr<Brain> brain,
                                     const std::string& filename);
    static void plotTimeSeries(const std::vector<std::vector<float>>& data,
                              const std::vector<std::string>& labels,
                              const std::string& filename);
    static void plotComparison(const std::vector<FloatMatrix>& datasets,
                              const std::vector<std::string>& labels,
                              const std::string& filename);
    
    // Data processing
    static FloatMatrix normalizeData(const FloatMatrix& data);
    static FloatMatrix smoothData(const FloatMatrix& data, 
                                 size_t windowSize = 5);
    static std::vector<size_t> findPeaks(const FloatArray& data,
                                         double threshold = 0.5);
    
    // Export formats
    static void exportToCSV(const FloatMatrix& data, 
                            const std::string& filename);
    static void exportToJSON(const std::map<std::string, FloatArray>& data,
                            const std::string& filename);
    static void exportToMatlab(const FloatMatrix& data,
                              const std::string& filename);
    
    // Utilities
    static std::string generateColorMap(const std::string& scheme = "viridis");
    static double interpolate(double x, double x1, double x2,
                              double y1, double y2);
    static FloatArray computeHistogram(const FloatArray& data,
                                       size_t bins = 50);
};

#endif // VISUALIZATION_HELPER_HPP
