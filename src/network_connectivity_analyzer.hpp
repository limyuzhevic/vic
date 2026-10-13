// NetworkConnectivityAnalyzer class header
// This file provides the header declaration for the NetworkConnectivityAnalyzer class

#ifndef NETWORK_CONNECTIVITY_ANALYZER_HPP
#define NETWORK_CONNECTIVITY_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Analyze network connectivity
 */
class NetworkConnectivityAnalyzer {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    NetworkConnectivityAnalyzer(std::shared_ptr<Brain> brain);
    
    ~NetworkConnectivityAnalyzer() = default;
    
    // Graph analysis
    FloatMatrix computeAdjacencyMatrix() const;
    double computeClusteringCoefficient() const;
    double computeCharacteristicPathLength() const;
    double computeSmallWorldness() const;
    
    // Topological analysis
    std::vector<size_t> findHubNeurons(double threshold = 0.8) const;
    std::map<size_t, std::vector<size_t>> findCommunities() const;
    FloatArray computeBetweennessCentrality() const;
    FloatArray computeClosenessCentrality() const;
    
    // Dynamic analysis
    FloatMatrix computeTemporalConnectivity(size_t windowSize = 100) const;
    FloatArray computeConnectionStability() const;
    
    // Export
    void exportConnectivityGraph(const std::string& filepath) const;
    std::string getConnectivitySummary() const;
};

#endif // NETWORK_CONNECTIVITY_ANALYZER_HPP
