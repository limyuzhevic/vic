#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NetworkActivity {
    std::vector<float> firingRates; // Firing rates of neurons
    std::vector<float> phases; // Phase information for oscillators
    std::vector<float> coherence; // Network coherence values
    std::vector<std::pair<NeuronId, NeuronId>> connections; // Functional connections
    std::string patternType; // Type of activity pattern
    float synchrony; // Degree of synchrony (0-1)
    float informationFlow; // Information flow strength
    Timestamp timestamp; // Time of measurement
    std::map<std::string, double> additionalMetrics; // Additional metrics
};

struct NetworkData {
    std::vector<NetworkActivity> activities; // Time series of network activities
    std::vector<Timestamp> timestamps; // Corresponding timestamps
    std::string sessionId; // Session identifier
    std::string networkId; // Network identifier
};

class NetworkAnalyzer {
public:
    NetworkAnalyzer();
    ~NetworkAnalyzer() = default;
    
    // Add network data
    void addData(const NetworkData& data);
    
    // Add network activity
    void addActivity(const NetworkActivity& activity);
    
    // Analyze activity patterns
    std::string analyzeActivityPatterns(const NetworkData& data) const;
    
    // Calculate network statistics
    double calculateNetworkStatistics(const NetworkData& data) const;
    
    // Calculate synchronization analysis
    double calculateSynchronization(const NetworkData& data) const;
    
    // Analyze information flow
    double analyzeInformationFlow(const NetworkData& data) const;
    
    // Detect emergent behaviors
    std::vector<std::string> detectEmergentBehaviors(const NetworkData& data) const;
    
    // Analyze phase relationships
    double analyzePhaseRelationships(const NetworkData& data) const;
    
    // Calculate network coherence
    double calculateNetworkCoherence(const NetworkData& data) const;
    
    // Predict network dynamics
    NetworkData predictNetworkDynamics(const NetworkData& data, double timeHorizon) const;
    
    // Compare network behavior
    double compareNetworkBehavior(const NetworkData& data1, const NetworkData& data2) const;
    
    // Get network data
    const NetworkData& getData() const;
    
    // Clear network data
    void clearData();
    
    // Export analysis results
    bool exportResults(const std::string& filename, const std::string& format = "json") const;
    
    // Import analysis results
    bool importResults(const std::string& filename, const std::string& format = "json");
    
    // Generate network analysis report
    std::string generateReport() const;
    
    // Check if analyzer has data
    bool hasData() const;
    
    // Get analysis timestamp
    Timestamp getTimestamp() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm