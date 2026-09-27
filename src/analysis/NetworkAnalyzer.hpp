#ifndef NETWORK_ANALYZER_HPP
#define NETWORK_ANALYZER_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>
#include <queue>
#include <unordered_map>

namespace nlm {
namespace analysis {

// Network structure data
struct Connection {
    std::string source_id;
    std::string target_id;
    double weight;
    double delay;
    double conductance;
    std::map<std::string, double> properties;
};

struct NeuralConnection {
    std::string source_region;
    std::string target_region;
    std::vector<std::string> source_neurons;
    std::vector<std::string> target_neurons;
    std::vector<Connection> connections;
    double total_weight;
    double connection_strength;
};

struct NetworkTopology {
    std::string network_id;
    std::string network_type;
    std::map<std::string, std::map<std::string, double>> adjacency_matrix;
    std::vector<std::string> node_ids;
    std::vector<std::string> region_ids;
    double global_efficiency;
    double transitivity;
    double assortativity;
};

// Network topology analyzer
class NetworkTopologyAnalyzer {
private:
    std::vector<NeuralConnection> connections_;
    std::map<std::string, NetworkTopology> topologies_;
    std::map<std::string, std::vector<std::string>> adjacency_list_;

public:
    // Data management
    bool addConnection(const NeuralConnection& connection);
    bool addTopology(const NetworkTopology& topology);
    std::vector<NeuralConnection> getConnections() const { return connections_; }

    // Basic topology measures
    double computeNodeDegree(const std::string& node_id) const;
    double computeOutDegree(const std::string& node_id) const;
    double computeInDegree(const std::string& node_id) const;
    double computeClusteringCoefficient(const std::string& node_id) const;
    double computeGlobalClusteringCoefficient() const;
    double computeCharacteristicPathLength() const;
    double computeAveragePathLength() const;
    double computeSmallWorldness() const;

    // Advanced topology measures
    std::map<std::string, double> computeBetweennessCentrality() const;
    std::map<std::string, double> computeClosenessCentrality() const;
    std::map<std::string, double> computeEigenvectorCentrality() const;
    std::map<std::string, double> computePageRank() const;
    std::map<std::string, double> computeLRCentrality() const;
    std::map<std::string, double> computeStressCentrality() const;

    // Community detection and modularity
    std::vector<std::vector<std::string>> detectCommunities() const;
    std::map<std::string, std::vector<std::string>> detectModularityCommunities() const;
    double computeModularity() const;
    double computeModularity(const std::vector<std::vector<std::string>>& communities) const;

    // Network dynamics analysis
    std::map<std::string, double> computeDynamicalStability() const;
    std::map<std::string, double> computeSynchronizationProbability() const;
    std::map<std::string, double> computeInformationFlow() const;
    std::map<std::string, double> computeNetworkCapacity() const;

    // Network robustness
    double computeRobustnessToNodeRemoval() const;
    double computeRobustnessToEdgeRemoval() const;
    std::map<std::string, double> computeAttackTolerance() const;
    std::map<std::string, double> computeRecoveryCapability() const;

    // Network efficiency
    double computeGlobalEfficiency() const;
    double computeLocalEfficiency(const std::string& node_id) const;
    double computeNetworkDiameter() const;
    std::vector<double> computeDiameterDistribution() const;

    // Network entropy and uncertainty
    double computeNetworkEntropy() const;
    double computeDegreeDistributionEntropy() const;
    double computeBetweennessDistributionEntropy() const;

    // Time-dependent topology
    std::map<std::string, double> analyzeDynamicTopology(const std::string& time_window) const;
    std::vector<double> computeTopologyEvolution() const;
    std::map<std::string, double> computeTopologyVariability() const;

    // Visualization and export
    std::string generateAdjacencyMatrix() const;
    std::string generateCommunityPlot() const;
    std::string generateCentralityHeatmap() const;
    std::string generateNetworkFlowChart() const;

    // File operations
    bool exportNetwork(const std::string& filename) const;
    static std::unique_ptr<NetworkTopologyAnalyzer> importNetwork(const std::string& filename);
};

// Plasticity analyzer
class PlasticityAnalyzer {
private:
    std::map<std::string, std::vector<std::pair<double, double>>> plasticity_traces_;
    std::map<std::string, std::vector<double>> plasticity_rates_;
    std::map<std::string, std::vector<double>> weight_histories_;
    std::map<std::string, std::string> plasticity_rule_types_;
    double simulation_time_;

public:
    PlasticityAnalyzer() : simulation_time_(0.0) {}

    // Data management
    bool recordPlasticityEvent(const std::string& synapse_id,
                              double time,
                              double weight_change,
                              const std::string& rule_type = "STDP");
    bool setPlasticityRule(const std::string& synapse_id,
                         const std::string& rule_type);
    bool addWeightHistory(const std::string& synapse_id, double weight, double time);

    // Plasticity rate analysis
    double computeAveragePlasticityRate(const std::string& synapse_id) const;
    double computePlasticityVariance(const std::string& synapse_id) const;
    std::vector<double> computePlasticityHistogram(const std::string& synapse_id,
                                                  size_t bins = 50) const;

    // STDP specific analysis
    std::map<std::string, double> analyzeSTDPSymmetry(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeSTDPTemporalProfile(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeSTDPConvergence(const std::string& synapse_id) const;

    // Homeostatic plasticity analysis
    std::map<std::string, double> analyzeHomeostaticPlasticity(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeMetaplasticChanges(const std::string& synapse_id) const;

    // Pattern analysis
    std::map<std::string, double> detectPlasticityRegimes(const std::string& synapse_id) const;
    std::map<std::string, double> identifyPlasticityBounds(const std::string& synapse_id) const;
    std::map<std::string, double> analyzePlasticityCriticality(const std::string& synapse_id) const;

    // Network-level plasticity
    double computeNetworkPlasticityDynamism() const;
    std::map<std::string, double> analyzePlasticitySpread(const std::string& source_synapse) const;
    std::map<std::string, double> computePlasticityCorrelation(const std::string& synapse_id1,
                                                              const std::string& synapse_id2) const;

    // Model validation
    bool validatePlasticityModel(const std::string& synapse_id) const;
    std::map<std::string, double> performPlasticityGoodnessOfFit(const std::string& synapse_id) const;

    // Visualization
    std::string generatePlasticityTracePlot(const std::string& synapse_id) const;
    std::string generatePlasticityHeatmap() const;
    std::string generatePlasticityTimeSeries() const;

    // Export/import
    bool exportPlasticityData(const std::string& filename) const;
    static std::unique_ptr<PlasticityAnalyzer> importPlasticityData(const std::string& filename);
};

// Emergent behavior analyzer
class EmergentBehaviorAnalyzer {
private:
    std::map<std::string, std::vector<double>> behavioral_traces_;
    std::map<std::string, std::vector<std::vector<double>>> state_trajectories_;
    std::map<std::string, std::vector<double>> emergent_properties_;

public:
    // Data management
    bool recordBehavior(const std::string& behavior_name,
                       const std::vector<double>& trace,
                       double time_step = 0.1);
    bool recordStateTrajectory(const std::string& behavior_name,
                              const std::vector<std::vector<double>>& trajectory);
    bool recordEmergentProperty(const std::string& property_name,
                               const std::vector<double>& property_values);

    // Emergent pattern detection
    std::map<std::string, double> detectRhythms(const std::string& behavior_name) const;
    std::map<std::string, double> detectOscillations(const std::string& behavior_name) const;
    std::map<std::string, double> detectAttractors(const std::string& behavior_name) const;
    std::map<std::string, double> detect bifurcations(const std::string& behavior_name) const;

    // Complexity measures
    std::map<std::string, double> computeComplexity(const std::string& behavior_name) const;
    std::map<std::string, double> computeSelfOrganization(const std::string& behavior_name) const;
    std::map<std::string, double> computeAdaptation(const std::string& behavior_name) const;

    // Pattern recognition
    std::vector<std::vector<double>> performPatternRecognition(const std::string& behavior_name) const;
    std::map<std::string, double> classifyBehavioralPattern(const std::string& behavior_name) const;

    // Correlation analysis
    std::map<std::string, double> correlateBehaviors(const std::string& behavior1,
                                                     const std::string& behavior2) const;
    std::map<std::string, double> analyzeBehavioralCoupling(const std::string& behavior1,
                                                           const std::string& behavior2) const;

    // Validation and quality control
    bool validateBehavioralModel(const std::string& behavior_name) const;
    std::map<std::string, double> performGoodnessOfFit(const std::string& behavior_name) const;

    // Export
    std::string generateBehaviorReport(const std::string& behavior_name) const;
    bool exportBehaviorData(const std::string& filename) const;
};

// Comprehensive analysis orchestrator
class AnalysisOrchestrator {
private:
    std::unique_ptr<NetworkTopologyAnalyzer> network_analyzer_;
    std::unique_ptr<PlasticityAnalyzer> plasticity_analyzer_;
    std::unique_ptr<EmergentBehaviorAnalyzer> behavior_analyzer_;
    std::vector<std::string> configured_analyzers_;

public:
    AnalysisOrchestrator() {
        network_analyzer_ = std::make_unique<NetworkTopologyAnalyzer>();
        plasticity_analyzer_ = std::make_unique<PlasticityAnalyzer>();
        behavior_analyzer_ = std::make_unique<EmergentBehaviorAnalyzer>();
        configured_analyzers_ = {"network", "plasticity", "behavior"};
    }

    // Analysis execution
    std::map<std::string, double> performNetworkAnalysis(const std::string& network_id);
    std::map<std::string, double> performPlasticityAnalysis(const std::string& synapse_id);
    std::map<std::string, double> performBehaviorAnalysis(const std::string& behavior_name);

    // Integrated analysis
    std::map<std::string, double> performIntegratedAnalysis(const std::string& experiment_id);
    std::vector<std::map<std::string, double>> performMultiAnalysis(const std::vector<std::string>& experiment_ids);

    // Configuration and control
    void configureAnalyzer(const std::string& analyzer_type, bool enable);
    std::vector<std::string> getConfiguredAnalyzers() const { return configured_analyzers_; }

    // Results management
    bool saveAnalysisResults(const std::string& filename) const;
    bool loadAnalysisResults(const std::string& filename);
    std::string generateAnalysisReport(const std::string& experiment_id) const;

    // Validation
    bool validateAnalysisConfiguration() const;
    std::vector<std::string> getConfigurationErrors() const;
};

} // namespace analysis
} // namespace nlm

#endif // NETWORK_ANALYZER_HPP