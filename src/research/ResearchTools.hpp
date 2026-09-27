#ifndef RESEARCH_TOOLS_HPP
#define RESEARCH_TOOLS_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <set>
#include <functional>
#include <limits>

namespace nlm {
namespace research {

// Experimental design structures
struct ExperimentParameters {
    std::string name;
    std::string description;
    std::map<std::string, double> variable_ranges;  // min-max ranges for parameters
    std::map<std::string, double> fixed_values;    // fixed parameter values
    std::vector<std::string> conditions;           // experimental conditions
    double dt;                                      // time step
    double duration;                                // simulation duration
    size_t repetitions;                             // number of repetitions
};

// Data collection structures
struct RecordedData {
    std::string experiment_id;
    std::string data_type;                           // Type of data recorded
    std::vector<double> values;                      // Recorded values
    std::vector<double> timestamps;                  // Corresponding timestamps
    std::map<std::string, std::vector<double>> metadata;
};

// Statistical analysis results
struct StatisticalResults {
    std::string test_name;
    double mean;                                     // Mean value
    double std_dev;                                  // Standard deviation
    double median;                                   // Median value
    double variance;                                 // Variance
    double min;                                      // Minimum value
    double max;                                      // Maximum value
    double confidence_interval;                      // Confidence interval
    size_t sample_size;                              // Number of samples
    std::map<std::string, double> additional_stats;
};

// Model comparison results
struct ModelComparison {
    std::string model_a_name;
    std::string model_b_name;
    double performance_a;                            // Model A performance metric
    double performance_b;                            // Model B performance metric
    double difference;                               // Performance difference
    double confidence;                                // Confidence in difference
    std::map<std::string, double> metrics_comparison;
};

// Advanced statistical analysis class
class StatisticalAnalyzer {
private:
    std::map<std::string, std::vector<double>> data_datasets_;
    std::map<std::string, std::vector<std::vector<double>>> multivariate_data_;
    std::mt19937 rng_;

public:
    StatisticalAnalyzer() : rng_(std::random_device{}()) {}

    // Data management
    bool addDataset(const std::string& name, const std::vector<double>& data);
    bool addMultivariateDataset(const std::string& name, 
                               const std::vector<std::vector<double>>& data);
    std::vector<double> getDataset(const std::string& name) const;

    // Statistical tests
    StatisticalResults computeMeanStdDev(const std::string& dataset_name) const;
    StatisticalResults computePercentiles(const std::string& dataset_name, 
                                         const std::vector<double>& percentiles) const;
    StatisticalResults computeDistributionFit(const std::string& dataset_name,
                                            const std::string& distribution_type) const;

    // Correlation and regression
    double computePearsonCorrelation(const std::string& dataset1,
                                    const std::string& dataset2) const;
    double computeSpearmanCorrelation(const std::string& dataset1,
                                     const std::string& dataset2) const;
    StatisticalResults performLinearRegression(const std::string& x_dataset,
                                             const std::string& y_dataset) const;

    // Hypothesis testing
    StatisticalResults performTTest(const std::string& dataset1,
                                   const std::string& dataset2) const;
    StatisticalResults performANOVA(const std::vector<std::string>& dataset_names) const;
    StatisticalResults performKruskalWallis(const std::vector<std::string>& dataset_names) const;

    // Time series analysis
    StatisticalResults computeAutocorrelation(const std::string& dataset_name,
                                            int max_lag = 20) const;
    StatisticalResults computeSpectralDensity(const std::string& dataset_name,
                                            double sampling_freq = 1.0) const;
    StatisticalResults detectTrends(const std::string& dataset_name) const;

    // Clustering and dimensionality reduction
    StatisticalResults performKMeansClustering(const std::string& dataset_name,
                                              size_t k_clusters) const;
    StatisticalResults performPCA(const std::string& dataset_name) const;

    // Model validation
    StatisticalResults computeGoodnessOfFit(const std::string& observed,
                                           const std::string& predicted) const;
    StatisticalResults computeCrossValidation(const std::string& dataset_name,
                                            size_t k_folds) const;

    // Monte Carlo simulation
    StatisticalResults monteCarloSimulation(std::function<double()> function,
                                           size_t iterations) const;

    // Reproducibility tools
    void setRandomSeed(unsigned int seed) { rng_.seed(seed); }
    unsigned int getRandomSeed() const { return rng_.seed(); }

    // Batch processing
    std::vector<StatisticalResults> analyzeMultipleDatasets(const std::vector<std::string>& dataset_names);

    // Serialization
    bool saveResults(const std::string& filename) const;
    static std::unique_ptr<StatisticalAnalyzer> loadResults(const std::string& filename);
};

// Neural activity analyzer
class NeuralActivityAnalyzer {
private:
    std::vector<RecordedData> neural_data_;
    std::map<std::string, std::vector<std::vector<double>>> spike_trains_;

public:
    // Data collection
    bool recordNeuralActivity(const std::string& experiment_id,
                             const std::string& neuron_id,
                             const std::vector<double>& spikes,
                             const std::vector<double>& timestamps);
    bool recordPopulationActivity(const std::string& experiment_id,
                                  const std::string& population_id,
                                  const std::vector<double>& activity,
                                  const std::vector<double>& timestamps);

    // Neural dynamics analysis
    StatisticalResults analyzeFiringRate(const std::string& neuron_id,
                                        double window_size = 10.0) const;
    StatisticalResults analyzeFiringPattern(const std::string& neuron_id) const;
    StatisticalResults analyzeSynchrony(const std::string& neuron_id1,
                                      const std::string& neuron_id2) const;
    StatisticalResults analyzeReconstruction(const std::string& neuron_id) const;

    // Frequency domain analysis
    StatisticalResults analyzePowerSpectrum(const std::string& neuron_id,
                                          double sampling_freq = 1000.0) const;
    StatisticalResults analyzeCoherence(const std::string& neuron_id1,
                                      const std::string& neuron_id2) const;

    // Network-level analysis
    StatisticalResults analyzeNetworkTopology(const std::string& population_id) const;
    StatisticalResults analyzeSmallWorldness(const std::string& population_id) const;
    StatisticalResults analyzeClusteringCoefficient(const std::string& population_id) const;

    // Dynamic analysis
    StatisticalResults analyzeStateSpace(const std::string& neuron_id,
                                        const std::string& variable) const;
    StatisticalResults analyzeAttractors(const std::string& population_id) const;
    StatisticalResults analyzeCriticality(const std::string& population_id) const;

    // Emergent behavior analysis
    StatisticalResults analyzeEmergentRhythms(const std::string& population_id) const;
    StatisticalResults analyzePhaseTransitions(const std::string& population_id) const;
    StatisticalResults analyzeBistability(const std::string& population_id) const;

    // Experimental design
    bool designExperiment(const ExperimentParameters& params);
    bool runControlledExperiment(const ExperimentParameters& params);

    // Data visualization (for research)
    std::string generateAnalysisReport(const std::string& analysis_name) const;
    bool exportDataForVisualization(const std::string& dataset_name,
                                   const std::string& output_format) const;

    // Statistical validation
    StatisticalResults computeReliability(const std::string& dataset_name,
                                         double alpha = 0.05) const;
    StatisticalResults computeEffectSize(const std::string& dataset1,
                                       const std::string& dataset2) const;

    // Reproducibility tools
    void saveExperimentData(const std::string& experiment_id) const;
    static std::unique_ptr<NeuralActivityAnalyzer> loadExperimentData(const std::string& experiment_id);
};

// Model calibration class
class ModelCalibrator {
private:
    std::map<std::string, std::vector<double>> model_parameters_;
    std::vector<RecordedData> experimental_data_;
    std::map<std::string, double> parameter_bounds_;
    std::function<double(const std::map<std::string, double>&)> error_function_;

public:
    // Calibration setup
    bool setModelParameters(const std::map<std::string, std::vector<double>>& parameters);
    bool setExperimentalData(const std::vector<RecordedData>& data);
    bool setParameterBounds(const std::map<std::string, double>& bounds);
    bool setErrorFunction(std::function<double(const std::map<std::string, double>&)> func);

    // Calibration algorithms
    std::map<std::string, double> calibrateUsingGradientDescent(size_t iterations = 1000,
                                                                 double learning_rate = 0.01);
    std::map<std::string, double> calibrateUsingParticleSwarm(size_t iterations = 500,
                                                              size_t swarm_size = 30);
    std::map<std::string, double> calibrateUsingBayesianOptimization(size_t iterations = 100);
    std::map<std::string, double> calibrateUsingGeneticAlgorithm(size_t generations = 100,
                                                                 size_t population_size = 50);

    // Validation and cross-validation
    double validate(const std::map<std::string, double>& parameters) const;
    std::vector<double> crossValidate(const std::map<std::string, double>& parameters,
                                      size_t k_folds = 5) const;

    // Sensitivity analysis
    std::map<std::string, double> computeSensitivity(const std::map<std::string, double>& parameters,
                                                    const std::vector<std::string>& target_variables) const;
    std::map<std::string, std::vector<double>> computeParameterSpace(const std::map<std::string, double>& base_parameters,
                                                                     const std::map<std::string, double>& ranges) const;

    // Model comparison
    ModelComparison compareModels(const std::string& model_a_name,
                                const std::string& model_b_name,
                                const std::map<std::string, double>& data) const;

    // Uncertainty quantification
    std::map<std::string, double> computeUncertainty(const std::map<std::string, double>& parameters) const;
    StatisticalResults computeConfidenceIntervals(const std::map<std::string, double>& parameters) const;

    // Export and reporting
    bool exportCalibratedModel(const std::string& filename) const;
    std::string generateCalibrationReport() const;
    bool saveCalibrationResults(const std::string& filename) const;

    // Reproducibility
    static std::unique_ptr<ModelCalibrator> loadCalibration(const std::string& filename);
};

// Research experiment controller
class ResearchExperimentController {
private:
    std::map<std::string, ExperimentParameters> experiments_;
    std::map<std::string, std::vector<RecordedData>> experimental_results_;
    std::chrono::system_clock::time_point start_time_;
    bool experiment_running_;
    size_t current_experiment_id_;

public:
    ResearchExperimentController() : experiment_running_(false), current_experiment_id_(0) {}

    // Experiment management
    std::string createExperiment(const ExperimentParameters& params);
    bool runExperiment(const std::string& experiment_id);
    bool stopExperiment(const std::string& experiment_id);
    bool pauseExperiment(const std::string& experiment_id);
    bool resumeExperiment(const std::string& experiment_id);

    // Experiment control
    bool setExperimentParameter(const std::string& experiment_id,
                               const std::string& param_name,
                               double value);
    double getExperimentParameter(const std::string& experiment_id,
                                 const std::string& param_name) const;

    // Data management
    bool recordData(const std::string& experiment_id,
                   const RecordedData& data);
    std::vector<RecordedData> getExperimentalData(const std::string& experiment_id) const;
    std::map<std::string, std::vector<RecordedData>> getAllResults() const { return experimental_results_; }

    // Analysis and reporting
    StatisticalResults analyzeExperimentalData(const std::string& experiment_id) const;
    std::string generateExperimentalReport(const std::string& experiment_id) const;

    // Batch processing
    std::vector<std::string> runBatchExperiments(const std::vector<std::string>& experiment_ids);
    bool runAllExperiments();

    // Reproducibility
    bool saveExperimentSetup(const std::string& filename) const;
    bool loadExperimentSetup(const std::string& filename);
    bool exportResults(const std::string& filename) const;

    // Monitoring and control
    double getExperimentProgress(const std::string& experiment_id) const;
    bool isExperimentRunning(const std::string& experiment_id) const;
    std::chrono::duration<double> getExperimentDuration(const std::string& experiment_id) const;

    // Statistical quality control
    bool checkExperimentalQuality(const std::string& experiment_id) const;
    StatisticalResults performGoodnessOfFitTest(const std::string& experiment_id) const;
};

} // namespace research
} // namespace nlm

#endif // RESEARCH_TOOLS_HPP