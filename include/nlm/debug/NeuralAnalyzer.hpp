// ==============================================================================
// Neural Analyzer
// ==============================================================================
#ifndef NEURALANALYZER_HPP
#define NEURALANALYZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <algorithm>
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace nlm {
namespace debug {

/**
 * @class NeuralAnalyzer
 * @brief Neural network analysis tool for NLM
 * 
 * Provides tools for neural network analysis including neural network topology analysis,
 * neural activity analysis, neural connectivity analysis, and neural dynamics analysis.
 */
class NeuralAnalyzer {
public:
    /**
     * @enum AnalysisType
     * @brief Type of analysis
     */
    enum class AnalysisType {
        TOPOLOGY_ANALYSIS,
        ACTIVITY_ANALYSIS,
        CONNECTIVITY_ANALYSIS,
        DYNAMICS_ANALYSIS,
        PLASTICITY_ANALYSIS,
        NEUROMODULATION_ANALYSIS,
        BEHAVIORAL_ANALYSIS,
        LEARNING_ANALYSIS,
        CONSOLIDATION_ANALYSIS,
        COMPARATIVE_ANALYSIS
    };

    /**
     * @enum AnalysisMetric
     * @brief Type of analysis metric
     */
    enum class AnalysisMetric {
        DEGREREE_CENTRALITY,
        BETWEENNESS_CENTRALITY,
        CLUSTERING_COEFFICIENT,
        PATH_LENGTH,
        MODULARITY,
        SMALL_WORLDNESS,
        FIRING_RATE,
        BURST_INDEX,
        PAUSE_INDEX,
        REGULARITY,
        SYNAPTIC_STRENGTH,
        CONNECTION_DENSITY,
        EXCITATORY_INHIBITORY_RATIO,
        PLASTICITY_STRENGTH,
        NEUROMODULATION_IMPACT,
        REWARD_PREDICTION_ERROR,
        NOVELTY_EXPLORATION,
        CURIOSITY_DRIVE,
        CONSOLIDATION_EFFECTIVENESS
    };

    /**
     * @struct NeuralTopologyAnalysis
     * @brief Neural topology analysis
     */
    struct NeuralTopologyAnalysis {
        uint64_t totalNeurons;
        uint64_t totalSynapses;
        uint64_t totalPopulations;
        double averageConnectionsPerNeuron;
        double averageFiringRate;
        double averageExcitatoryInhibitionRatio;
        double averageSynapticStrength;
        double clusteringCoefficient;
        double pathLength;
        double modularity;
        double smallWorldness;
        std::map<std::string, double> layerAnalysis;
        std::vector<std::pair<std::string, double>> hubNodes;
        std::vector<std::pair<std::string, double>> bottleneckNodes;
    };

    /**
     * @struct NeuralActivityAnalysis
     * @brief Neural activity analysis
     */
    struct NeuralActivityAnalysis {
        double averageFiringRate;
        double firingRateStandardDeviation;
        double burstIndex;
        double pauseIndex;
        double regularity;
        double coefficientOfVariation;
        std::vector<double> firingRateHistogram;
        std::map<std::string, double> neuronTypeAnalysis;
        std::chrono::steady_clock::time_point analysisTime;
    };

    /**
     * @struct NeuralConnectivityAnalysis
     * @brief Neural connectivity analysis
     */
    struct NeuralConnectivityAnalysis {
        double connectionDensity;
        double averageConnectionStrength;
        double connectionEfficiency;
        double redundancy;
        double resilience;
        std::map<std::string, double> connectionTypeAnalysis;
        std::vector<std::pair<std::string, std::vector<std::string>>> connectionPatterns;
    };

    /**
     * @struct NeuralDynamicsAnalysis
     * @brief Neural dynamics analysis
     */
    struct NeuralDynamicsAnalysis {
        double temporalCohesion;
        double transientDynamics;
        double steadyStateDynamics;
        double oscillatoryActivity;
        double noiseLevel;
        double synchronizationIndex;
        std::vector<std::pair<std::chrono::steady_clock::time_point, double>> timeSeries;
    };

    NeuralAnalyzer();
    ~NeuralAnalyzer();

    /**
     * @brief Analyze neural topology
     * @param brain Neural network reference
     * @return Neural topology analysis
     */
    NeuralTopologyAnalysis analyzeNeuralTopology(const Brain& brain);

    /**
     * @brief Analyze neural activity
     * @param brain Neural network reference
     * @return Neural activity analysis
     */
    NeuralActivityAnalysis analyzeNeuralActivity(const Brain& brain);

    /**
     * @brief Analyze neural connectivity
     * @param brain Neural network reference
     * @return Neural connectivity analysis
     */
    NeuralConnectivityAnalysis analyzeNeuralConnectivity(const Brain& brain);

    /**
     * @brief Analyze neural dynamics
     * @param brain Neural network reference
     * @return Neural dynamics analysis
     */
    NeuralDynamicsAnalysis analyzeNeuralDynamics(const Brain& brain);

    /**
     * @brief Analyze plasticity
     * @param brain Neural network reference
     * @return Plasticity analysis
     */
    struct PlasticityAnalysis {
        double plasticityStrength;
        double plasticityEfficiency;
        double plasticityStability;
        double learningRate;
        double adaptationRate;
        std::map<std::string, double> ruleAnalysis;
        std::vector<std::pair<std::string, double>> plasticityHotspots;
    };

    PlasticityAnalysis analyzePlasticity(const Brain& brain);

    /**
     * @brief Analyze neuromodulation
     * @param neuromodulator Neuromodulation system reference
     * @return Neuromodulation analysis
     */
    struct NeuromodulationAnalysis {
        double neuromodulationStrength;
        double neuromodulationEfficiency;
        double neuromodulationStability;
        double neuromodulationImpact;
        std::map<std::string, double> neuromodulatorAnalysis;
        std::vector<std::pair<std::string, double>> neuromodulationHotspots;
    };

    NeuromodulationAnalysis analyzeNeuromodulation(const Neuromodulator& neuromodulator);

    /**
     * @brief Perform comprehensive neural analysis
     * @param brain Neural network reference
     * @param neuromodulator Neuromodulation system reference
     * @return Comprehensive analysis
     */
    struct ComprehensiveAnalysis {
        NeuralTopologyAnalysis topologyAnalysis;
        NeuralActivityAnalysis activityAnalysis;
        NeuralConnectivityAnalysis connectivityAnalysis;
        NeuralDynamicsAnalysis dynamicsAnalysis;
        PlasticityAnalysis plasticityAnalysis;
        NeuromodulationAnalysis neuromodulationAnalysis;
        double overallScore;
        std::vector<std::string> insights;
        std::chrono::steady_clock::time_point analysisTime;
    };

    ComprehensiveAnalysis performComprehensiveAnalysis(const Brain& brain, const Neuromodulator& neuromodulator);

    /**
     * @brief Compare neural networks
     * @param brain1 First neural network
     * @param brain2 Second neural network
     * @return Comparison result
     */
    struct NeuralNetworkComparison {
        std::string comparisonType;
        double topologySimilarity;
        double activitySimilarity;
        double connectivitySimilarity;
        double dynamicsSimilarity;
        double plasticitySimilarity;
        double neuromodulationSimilarity;
        double overallSimilarity;
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
    };

    NeuralNetworkComparison compareNeuralNetworks(const Brain& brain1, const Brain& brain2);

    /**
     * @brief Get analysis types
     * @return Vector of analysis types
     */
    std::vector<AnalysisType> getAvailableAnalysisTypes() const;

    /**
     * @brief Get analysis metrics
     * @return Vector of analysis metrics
     */
    std::vector<AnalysisMetric> getAvailableAnalysisMetrics() const;

    /**
     * @brief Get neural network metrics
     * @param brain Neural network reference
     * @param metric Analysis metric
     * @return Metric value
     */
    double getNeuralNetworkMetric(const Brain& brain, AnalysisMetric metric);

    /**
     * @brief Get population metrics
     * @param population Neural population reference
     * @param metric Analysis metric
     * @return Metric value
     */
    double getPopulationMetric(const NeuralPopulation& population, AnalysisMetric metric);

    /**
     * @brief Get neuron metrics
     * @param neuron Neuron reference
     * @param metric Analysis metric
     * @return Metric value
     */
    double getNeuronMetric(const Neuron& neuron, AnalysisMetric metric);

    /**
     * @brief Get synapse metrics
     * @param synapse Synapse reference
     * @param metric Analysis metric
     * @return Metric value
     */
    double getSynapseMetric(const Synapse& synapse, AnalysisMetric metric);

    /**
     * @brief Generate neural topology visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void generateNeuralTopologyVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate neural activity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void generateNeuralActivityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate neural connectivity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void generateNeuralConnectivityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate neural dynamics visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void generateNeuralDynamicsVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate plasticity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void generatePlasticityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate neuromodulation visualization
     * @param neuromodulator Neuromodulation system reference
     * @param filename Output file path
     */
    void generateNeuromodulationVisualization(const Neuromodulator& neuromodulator, const std::string& filename);

    /**
     * @brief Generate comprehensive analysis visualization
     * @param analysis Analysis result
     * @param filename Output file path
     */
    void generateComprehensiveAnalysisVisualization(const ComprehensiveAnalysis& analysis, const std::string& filename);

    /**
     * @brief Generate comparison visualization
     * @param comparison Comparison result
     * @param filename Output file path
     */
    void generateComparisonVisualization(const NeuralNetworkComparison& comparison, const std::string& filename);

    /**
     * @brief Get neural network efficiency
     * @param brain Neural network reference
     * @return Neural network efficiency
     */
    double getNeuralNetworkEfficiency(const Brain& brain);

    /**
     * @brief Get neural network stability
     * @param brain Neural network reference
     * @return Neural network stability
     */
    double getNeuralNetworkStability(const Brain& brain);

    /**
     * @brief Get neural network resilience
     * @param brain Neural network reference
     * @return Neural network resilience
     */
    double getNeuralNetworkResilience(const Brain& brain);

    /**
     * @brief Get neural network adaptability
     * @param brain Neural network reference
     * @return Neural network adaptability
     */
    double getNeuralNetworkAdaptability(const Brain& brain);

protected:
    /**
     * @brief Calculate topology metrics
     * @param brain Neural network reference
     * @return Topology metrics
     */
    NeuralTopologyAnalysis calculateTopologyMetrics(const Brain& brain);

    /**
     * @brief Calculate activity metrics
     * @param brain Neural network reference
     * @return Activity metrics
     */
    NeuralActivityAnalysis calculateActivityMetrics(const Brain& brain);

    /**
     * @brief Calculate connectivity metrics
     * @param brain Neural network reference
     * @return Connectivity metrics
     */
    NeuralConnectivityAnalysis calculateConnectivityMetrics(const Brain& brain);

    /**
     * @brief Calculate dynamics metrics
     * @param brain Neural network reference
     * @return Dynamics metrics
     */
    NeuralDynamicsAnalysis calculateDynamicsMetrics(const Brain& brain);

    /**
     * @brief Calculate plasticity metrics
     * @param brain Neural network reference
     * @return Plasticity metrics
     */
    PlasticityAnalysis calculatePlasticityMetrics(const Brain& brain);

    /**
     * @brief Calculate neuromodulation metrics
     * @param neuromodulator Neuromodulation system reference
     * @return Neuromodulation metrics
     */
    NeuromodulationAnalysis calculateNeuromodulationMetrics(const Neuromodulator& neuromodulator);

    /**
     * @brief Create visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create topology visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createTopologyVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create activity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createActivityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create connectivity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createConnectivityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create dynamics visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createDynamicsVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create plasticity visualization
     * @param brain Neural network reference
     * @param filename Output file path
     */
    void createPlasticityVisualization(const Brain& brain, const std::string& filename);

    /**
     * @brief Create neuromodulation visualization
     * @param neuromodulator Neuromodulation system reference
     * @param filename Output file path
     */
    void createNeuromodulationVisualization(const Neuromodulator& neuromodulator, const std::string& filename);

    /**
     * @brief Calculate network efficiency
     * @param brain Neural network reference
     * @return Network efficiency
     */
    double calculateNetworkEfficiency(const Brain& brain) const;

    /**
     * @brief Calculate network stability
     * @param brain Neural network reference
     * @return Network stability
     */
    double calculateNetworkStability(const Brain& brain) const;

    /**
     * @brief Calculate network resilience
     * @param brain Neural network reference
     * @return Network resilience
     */
    double calculateNetworkResilience(const Brain& brain) const;

    /**
     * @brief Calculate network adaptability
     * @param brain Neural network reference
     * @return Network adaptability
     */
    double calculateNetworkAdaptability(const Brain& brain) const;

    /**
     * @brief Generate Python code
     * @param brain Neural network reference
     * @param neuromodulator Neuromodulation system reference
     * @return Python code
     */
    std::string generatePythonCode(const Brain& brain, const Neuromodulator& neuromodulator);

    /**
     * @brief Execute Python code
     * @param pythonCode Python code
     * @return Python execution result
     */
    std::string executePythonCode(const std::string& pythonCode);

    /**
     * @brief Parse analysis result
     * @param result Analysis result
     * @return Parsed result
     */
    std::map<std::string, double> parseAnalysisResult(const ComprehensiveAnalysis& result);

    /**
     * @brief Generate error report
     * @param brain Neural network reference
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(const Brain& brain) const;

    /**
     * @brief Generate warning report
     * @param brain Neural network reference
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(const Brain& brain) const;

    /**
     * @brief Generate diagnostic report
     * @param brain Neural network reference
     * @return Diagnostic report
     */
    std::vector<std::string> generateDiagnosticReport(const Brain& brain) const;

    /**
     * @brief Generate insights
     * @param analysis Analysis result
     * @return Insights
     */
    std::vector<std::string> generateInsights(const ComprehensiveAnalysis& analysis);

    /**
     * @brief Calculate similarity score
     * @param analysis1 First analysis
     * @param analysis2 Second analysis
     * @return Similarity score
     */
    double calculateSimilarityScore(const ComprehensiveAnalysis& analysis1, const ComprehensiveAnalysis& analysis2);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // NEURALANALYZER_HPP
