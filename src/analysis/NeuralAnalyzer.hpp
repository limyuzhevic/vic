#ifndef NEURAL_ANALYZER_HPP
#define NEURAL_ANALYZER_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <complex>
#include <unordered_set>
#include <queue>
#include <set>

namespace nlm {
namespace analysis {

// Neural activity data structures
struct SpikeTrain {
    std::string neuron_id;
    std::vector<double> spike_times;      // Times of spikes
    std::vector<double> spike_rates;      // Instantaneous firing rates
    std::vector<double> isi_histogram;    // Inter-spike interval distribution
    double mean_firing_rate;              // Average firing rate (Hz)
    double cv_firing_rate;                // Coefficient of variation
    double burst_index;                   // Measure of bursting behavior
};

struct NeuralActivity {
    std::string region_id;
    std::string neuron_id;
    std::vector<double> membrane_potentials;
    std::vector<double> synaptic_inputs;
    std::vector<double> outputs;
    std::vector<double> timestamps;
};

struct NeuralDynamics {
    std::string experiment_id;
    std::string simulation_id;
    double simulation_time;
    double dt;
    std::vector<NeuralActivity> activities;
    std::map<std::string, std::vector<double>> global_signals;
};

// Neural activity analyzer
class NeuralActivityAnalyzer {
private:
    std::vector<SpikeTrain> spike_trains_;
    std::vector<NeuralDynamics> dynamics_;
    std::map<std::string, std::vector<std::complex<double>>> fft_results_;
    double sampling_frequency_;

public:
    NeuralActivityAnalyzer(double sampling_frequency = 1000.0) 
        : sampling_frequency_(sampling_frequency) {}
    
    void setSamplingFrequency(double freq) { sampling_frequency_ = freq; }

    // Data management
    bool addSpikeTrain(const SpikeTrain& spike_train);
    bool addNeuralDynamics(const NeuralDynamics& dynamics);
    std::vector<SpikeTrain> getSpikeTrains() const { return spike_trains_; }
    std::vector<NeuralDynamics> getDynamics() const { return dynamics_; }

    // Basic statistics
    double computeFiringRate(const SpikeTrain& spike_train) const;
    double computeMeanFiringRate(const std::string& region_id = "") const;
    double computeCVFiringRate(const SpikeTrain& spike_train) const;
    std::vector<double> computeISIHistogram(const SpikeTrain& spike_train,
                                           size_t bins = 50) const;

    // Temporal patterns
    double computeSynchrony(const SpikeTrain& spike_train1,
                           const SpikeTrain& spike_train2) const;
    std::vector<double> computeCrossCorrelation(const SpikeTrain& spike_train1,
                                               const SpikeTrain& spike_train2,
                                               int max_lag = 100) const;
    double computePhaseSynchronization(const SpikeTrain& spike_train1,
                                      const SpikeTrain& spike_train2) const;

    // Frequency analysis
    std::pair<std::vector<double>, std::vector<std::complex<double>>> performFFT(const SpikeTrain& spike_train) const;
    std::map<std::string, double> analyzePowerSpectrum(const SpikeTrain& spike_train) const;
    std::map<std::string, double> analyzeBifurcation(const SpikeTrain& spike_train) const;

    // Pattern recognition
    bool detectBursting(const SpikeTrain& spike_train, double threshold = 2.0) const;
    std::vector<std::vector<double>> performDimensionalityReduction(const NeuralDynamics& dynamics) const;
    std::vector<std::vector<double>> extractFeatures(const NeuralDynamics& dynamics) const;

    // Advanced analysis
    std::map<std::string, double> analyzeRecurrentDynamics(const NeuralDynamics& dynamics) const;
    std::map<std::string, double> analyzeAttractorStability(const NeuralDynamics& dynamics) const;
    std::map<std::string, double> analyzeCriticality(const NeuralDynamics& dynamics) const;

    // Network topology analysis
    std::map<std::string, double> analyzeSmallWorldness(const NeuralDynamics& dynamics) const;
    std::map<std::string, double> analyzeClusteringCoefficient(const NeuralDynamics& dynamics) const;
    std::map<std::string, double> analyzePathLength(const NeuralDynamics& dynamics) const;

    // Computational methods
    std::vector<double> computeGrangerCausality(const SpikeTrain& spike_train1,
                                               const SpikeTrain& spike_train2) const;
    std::vector<double> computeTransferEntropy(const SpikeTrain& spike_train1,
                                              const SpikeTrain& spike_train2) const;
    std::map<std::string, double> analyzeRandomMatrixTheory(const NeuralDynamics& dynamics) const;

    // Time series analysis
    std::vector<double> computeAutocorrelation(const SpikeTrain& spike_train,
                                              int max_lag = 100) const;
    std::vector<double> computePartialAutocorrelation(const SpikeTrain& spike_train,
                                                     int max_lag = 20) const;
    std::map<std::string, double> analyzeNonlinearity(const SpikeTrain& spike_train) const;

    // Visualization helpers
    std::string generateSpikeRasterPlot(const SpikeTrain& spike_train) const;
    std::string generateFiringRatePlot(const std::string& region_id) const;
    std::string generateHeatmap(const NeuralDynamics& dynamics) const;

    // Statistical validation
    double computeConfidenceIntervals(const std::vector<double>& data,
                                    double confidence_level = 0.95) const;
    bool performGoodnessOfFitTest(const std::vector<double>& observed,
                                const std::vector<double>& expected) const;

    // Export and import
    bool exportResults(const std::string& filename) const;
    static std::unique_ptr<NeuralActivityAnalyzer> importResults(const std::string& filename);
};

// Network topology analyzer
class NetworkTopologyAnalyzer {
private:
    std::map<std::string, std::vector<std::string>> adjacency_matrix_;
    std::map<std::string, std::map<std::string, double>> connection_weights_;
    std::map<std::string, std::vector<double>> node_attributes_;

public:
    // Graph construction
    bool buildNetworkFromConnectivity(const std::string& connectivity_file);
    bool buildNetworkFromSynapses(const std::map<std::string, std::vector<std::string>>& synapses);
    bool addNode(const std::string& node_id, const std::map<std::string, double>& attributes);
    bool addEdge(const std::string& source, const std::string& target, double weight);

    // Topological measures
    double computeClusteringCoefficient(const std::string& node_id) const;
    double computeGlobalClusteringCoefficient() const;
    double computeCharacteristicPathLength() const;
    double computeAveragePathLength() const;
    double computeSmallWorldness() const;

    // Centrality measures
    std::map<std::string, double> computeBetweennessCentrality() const;
    std::map<std::string, double> computeClosenessCentrality() const;
    std::map<std::string, double> computeEigenvectorCentrality() const;
    std::map<std::string, double> computePageRank() const;

    // Community detection
    std::vector<std::vector<std::string>> detectCommunities() const;
    std::map<std::string, std::vector<std::string>> detectModularityCommunities() const;
    double computeModularity() const;

    // Network dynamics
    std::map<std::string, double> computeDynamicalStability() const;
    std::map<std::string, double> computeSynchronizationProbability() const;
    std::map<std::string, double> computeInformationFlow() const;

    // Network robustness and resilience
    double computeRobustnessToNodeRemoval() const;
    double computeRobustnessToEdgeRemoval() const;
    std::map<std::string, double> computeAttackTolerance() const;

    // Network measures
    double computeNodeDegree(const std::string& node_id) const;
    double computeEdgeDensity() const;
    double computeNetworkSize() const;
    size_t getNodeCount() const { return adjacency_matrix_.size(); }
    size_t getEdgeCount() const;

    // Visualization
    std::string generateAdjacencyMatrix() const;
    std::string generateCommunityPlot() const;
    std::string generateCentralityHeatmap() const;

    // Export
    bool exportNetwork(const std::string& filename) const;
    static std::unique_ptr<NetworkTopologyAnalyzer> importNetwork(const std::string& filename);
};

// Plasticity dynamics analyzer
class PlasticityDynamicsAnalyzer {
private:
    std::vector<std::pair<double, double>> plasticity_history_;  // (time, weight_change)
    std::map<std::string, std::vector<std::pair<double, double>>> synaptic_plasticity_;
    std::map<std::string, std::vector<double>> plasticity_rules_;  // Rule type per synapse
    double simulation_time_;

public:
    PlasticityDynamicsAnalyzer() : simulation_time_(0.0) {}

    // Data management
    bool recordPlasticityChange(const std::string& synapse_id, double weight_change, double time);
    bool recordPlasticityRule(const std::string& synapse_id, const std::string& rule_type);
    bool setSimulationTime(double time) { simulation_time_ = time; return true; }

    // Plasticity analysis
    double computeAveragePlasticityRate() const;
    double computePlasticityVariance() const;
    std::map<std::string, double> computePlasticityStatistics(const std::string& synapse_id) const;
    std::vector<std::pair<double, double>> getPlasticityTrace(const std::string& synapse_id = "") const;

    // STDP analysis
    std::map<std::string, double> analyzeSTDPConvergence(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeSTDPSymmetry(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeSTDPTemporalProfile(const std::string& synapse_id) const;

    // Homeostatic plasticity
    std::map<std::string, double> analyzeHomeostaticPlasticity(const std::string& synapse_id) const;
    std::map<std::string, double> analyzeMetaplasticChanges(const std::string& synapse_id) const;

    // Pattern analysis
    std::map<std::string, double> detectPlasticityRegimes(const std::string& synapse_id) const;
    std::map<std::string, double> identifyPlasticityBounds(const std::string& synapse_id) const;
    std::map<std::string, double> analyzePlasticityCriticality(const std::string& synapse_id) const;

    // Network-level analysis
    double computeNetworkPlasticityDynamism() const;
    std::map<std::string, double> analyzePlasticitySpread(const std::string& source_synapse) const;
    std::map<std::string, double> computePlasticityCorrelation(const std::string& synapse_id1,
                                                              const std::string& synapse_id2) const;

    // Model validation
    bool validatePlasticityModel(const std::string& synapse_id) const;
    std::map<std::string, double> performPlasticityGoodnessOfFit(const std::string& synapse_id) const;

    // Visualization
    std::string generatePlasticityTracePlot(const std::string& synapse_id = "") const;
    std::string generatePlasticityHeatmap() const;
    std::string generatePlasticityTimeSeries() const;

    // Export
    bool exportPlasticityData(const std::string& filename) const;
    static std::unique_ptr<PlasticityDynamicsAnalyzer> importPlasticityData(const std::string& filename);
};

// Analysis result container
struct AnalysisResults {
    std::string experiment_id;
    std::string analysis_type;
    double execution_time;
    std::map<std::string, double> scalar_results;
    std::map<std::string, std::vector<double>> vector_results;
    std::map<std::string, std::vector<std::vector<double>>> matrix_results;
    std::string metadata;
    double confidence_level;
};

// Comprehensive analysis engine
class AnalysisEngine {
private:
    std::unique_ptr<NeuralActivityAnalyzer> neural_analyzer_;
    std::unique_ptr<NetworkTopologyAnalyzer> network_analyzer_;
    std::unique_ptr<PlasticityDynamicsAnalyzer> plasticity_analyzer_;
    std::vector<AnalysisResults> analysis_history_;

public:
    AnalysisEngine() {
        neural_analyzer_ = std::make_unique<NeuralActivityAnalyzer>();
        network_analyzer_ = std::make_unique<NetworkTopologyAnalyzer>();
        plasticity_analyzer_ = std::make_unique<PlasticityDynamicsAnalyzer>();
    }

    // Integrated analysis
    AnalysisResults performComprehensiveAnalysis(const NeuralDynamics& dynamics);
    AnalysisResults analyzeNetworkDynamics(const NeuralDynamics& dynamics);
    AnalysisResults analyzePlasticityDynamics(const NeuralDynamics& dynamics);

    // Batch analysis
    std::vector<AnalysisResults> performBatchAnalysis(const std::vector<NeuralDynamics>& dynamics);
    std::vector<AnalysisResults> performAllAnalyses(const NeuralDynamics& dynamics);

    // Result management
    bool addAnalysisResult(const AnalysisResults& result);
    std::vector<AnalysisResults> getAnalysisHistory() const { return analysis_history_; }
    std::vector<AnalysisResults> getAnalysisResults(const std::string& experiment_id) const;

    // Reporting
    std::string generateAnalysisReport(const AnalysisResults& result) const;
    std::string generateComprehensiveReport(const std::string& experiment_id) const;
    std::string exportAllResults(const std::string& filename) const;

    // Validation
    bool validateAnalysisResults(const AnalysisResults& result) const;
    std::vector<std::string> getValidationErrors(const AnalysisResults& result) const;

    // Configuration
    void setSamplingFrequency(double freq) { neural_analyzer_->setSamplingFrequency(freq); }
    void configureAnalysisParameters(const std::map<std::string, double>& params);
};

} // namespace analysis
} // namespace nlm

#endif // NEURAL_ANALYZER_HPP