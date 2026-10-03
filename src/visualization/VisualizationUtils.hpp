#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Utility functions for visualization

// Format ASCII art for neural network visualization
std::string formatNeuralNetworkASCII(const std::vector<NeuronVisualizationData>& neurons,
                                    const std::vector<SynapseVisualizationData>& synapses,
                                    const std::string& colorScheme = "default",
                                    int detailLevel = 1);

// Format memory information for display
std::string formatMemoryASCII(const MemoryVisualizationData& memory,
                             int detailLevel = 1);

// Format neuromodulation information for display
std::string formatNeuromodulationASCII(const NeuromodulationVisualizationData& neuromodulator,
                                       int detailLevel = 1);

// Format cognitive information for display
std::string formatCognitiveASCII(const CognitiveVisualizationData& cognitive,
                                 int detailLevel = 1);

// Format development information for display
std::string formatDevelopmentASCII(const DevelopmentVisualizationData& development,
                                   int detailLevel = 1);

// Format performance metrics for display
std::string formatPerformanceASCII(const PerformanceMetricsVisualizationData& metrics,
                                   int detailLevel = 1);

// Export data to various formats
bool exportToCSV(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                const std::vector<SynapseVisualizationData>& synapses);

bool exportToJSON(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                 const std::vector<SynapseVisualizationData>& synapses);

bool exportToHTML(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                 const std::vector<SynapseVisualizationData>& synapses);

// Generate progress bar
std::string generateProgressBar(float progress, int width = 50);

// Generate statistics summary
std::string generateStatisticsSummary(const std::vector<NeuronVisualizationData>& neurons,
                                      const std::vector<SynapseVisualizationData>& synapses,
                                      const PerformanceMetricsVisualizationData& metrics);

// Generate color palette based on scheme
std::map<std::string, std::string> getColorPalette(const std::string& scheme);

} // namespace nlm