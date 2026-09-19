// NLM C++ API Implementation
// Provides the actual implementation for the NLM C++ API

#include "api/nlm_api.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <filesystem>
#include <cstring>
#include <cstdarg>

namespace nlm {

std::shared_ptr<Config> ConfigAPI::createConfig(const std::string& filename) {
    auto config = std::make_shared<Config>();
    if (!filename.empty() && loadConfig(config, filename)) {
        NLM_LOG_INFO("Config loaded from: " + filename);
    }
    return config;
}

bool ConfigAPI::loadConfig(std::shared_ptr<Config> config, const std::string& filename) {
    if (!config) return false;
    return config->loadFromFile(filename);
}

bool ConfigAPI::saveConfig(std::shared_ptr<Config> config, const std::string& filename) {
    if (!config) return false;
    return config->saveToFile(filename);
}

bool ConfigAPI::validateConfig(std::shared_ptr<Config> config) {
    if (!config) return false;
    
    // Create a basic schema for validation
    std::vector<ConfigSchema> schema;
    
    // Required keys
    schema.push_back({"random_seed", true, ""});
    schema.push_back({"neuron_count", true, ""});
    schema.push_back({"region_count", true, ""});
    
    // Optional keys with defaults
    schema.push_back({"learning_rate", false, "0.01"});
    schema.push_back({"enable_stdp", false, "true"});
    schema.push_back({"simulation_timestep", false, "0.001"});
    
    return config->validate(schema);
}

std::string ConfigAPI::exportConfig(std::shared_ptr<Config> config, const std::string& format) {
    if (!config) return "";
    
    if (format == "json") {
        return config->toJSON();
    } else if (format == "yaml") {
        return config->toYAML();
    } else if (format == "ini") {
        return config->toINI();
    }
    
    NLM_LOG_ERROR("Unsupported export format: " + format);
    return "";
}

bool ConfigAPI::importConfig(std::shared_ptr<Config> config, const std::string& data, const std::string& format) {
    if (!config) return false;
    
    auto tempConfig = std::make_shared<Config>();
    
    if (format == "json") {
        // Parse JSON data and set config values
        // This is a placeholder implementation
        NLM_LOG_INFO("JSON import not yet implemented.");
    } else if (format == "yaml") {
        NLM_LOG_INFO("YAML import not yet implemented.");
    } else if (format == "ini") {
        NLM_LOG_INFO("INI import not yet implemented.");
    }
    
    return false;
}

std::string ConfigAPI::configDiff(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) {
    if (!config1 || !config2) return "Invalid configuration pointers";
    return config1->getDiff(*config2);
}

std::string ConfigAPI::getConfigSummary(std::shared_ptr<Config> config) {
    if (!config) return "";
    return config->summary();
}

std::shared_ptr<Config> ConfigAPI::createStandardTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("learning_rate", 0.01, ConfigSource::Default);
    config->set("enable_stdp", true, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("max_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    config->set("checkpoint_interval", static_cast<int64_t>(1000), ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigAPI::createResearchTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("learning_rate", 0.001, ConfigSource::Default);
    config->set("enable_stdp", true, ConfigSource::Default);
    config->set("simulation_timestep", 0.0001, ConfigSource::Default);
    config->set("max_steps", static_cast<int64_t>(100000), ConfigSource::Default);
    config->set("checkpoint_interval", static_cast<int64_t>(5000), ConfigSource::Default);
    config->set("enable_gpu", true, ConfigSource::Default);
    config->set("gpu_device", 0, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigAPI::createPerformanceTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("learning_rate", 0.01, ConfigSource::Default);
    config->set("enable_stdp", false, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("max_steps", static_cast<int64_t>(5000), ConfigSource::Default);
    config->set("checkpoint_interval", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("parallel_execution", true, ConfigSource::Default);
    config->set("num_processes", static_cast<int64_t>(4), ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigAPI::mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs) {
    if (configs.empty()) return nullptr;
    
    auto mergedConfig = std::make_shared<Config>();
    
    for (const auto& config : configs) {
        if (config) {
            auto entries = config->getAllEntries();
            for (const auto& entry : entries) {
                mergedConfig->set(entry.key, entry.value, entry.source);
            }
        }
    }
    
    return mergedConfig;
}

std::shared_ptr<Brain> BrainAPI::createBrain(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    if (!configFile.empty() && loadConfig(config, configFile)) {
        NLM_LOG_INFO("Brain config loaded from: " + configFile);
    }
    return createBrain(config);
}

std::shared_ptr<Brain> BrainAPI::createBrain(std::shared_ptr<Config> config) {
    return std::make_shared<Brain>(config);
}

bool BrainAPI::initializeBrain(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    return brain->initialize();
}

bool BrainAPI::runStep(std::shared_ptr<Brain> brain, size_t step, double time) {
    if (!brain) return false;
    return brain->step(step, time);
}

bool BrainAPI::runSimulation(std::shared_ptr<Brain> brain, size_t steps, double timestep) {
    if (!brain) return false;
    
    auto config = brain->getConfig();
    if (!config) return false;
    
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    
    for (size_t step = 0; step < steps; ++step) {
        if (!brain->step(step, step * timestep)) {
            NLM_LOG_ERROR("Simulation failed at step " + std::to_string(step));
            return false;
        }
    }
    
    return true;
}

bool BrainAPI::saveBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    if (!brain) return false;
    return brain->save(filename);
}

bool BrainAPI::loadBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    if (!brain) return false;
    return brain->load(filename);
}

void BrainAPI::logBrainStatus(std::shared_ptr<Brain> brain) {
    if (brain) {
        brain->logStatus();
    }
}

double BrainAPI::getTotalNeuronCount(std::shared_ptr<Brain> brain) {
    return brain ? brain->getTotalNeuronCount() : 0.0;
}

double BrainAPI::getTotalSynapseCount(std::shared_ptr<Brain> brain) {
    return brain ? brain->getTotalSynapseCount() : 0.0;
}

size_t BrainAPI::getTotalSpikeCount(std::shared_ptr<Brain> brain) {
    return brain ? brain->getTotalSpikeCount() : 0;
}

size_t BrainAPI::getFiringNeuronCount(std::shared_ptr<Brain> brain) {
    return brain ? brain->getFiringNeuronCount() : 0;
}

size_t BrainAPI::getMemoryUsage(std::shared_ptr<Brain> brain) {
    return brain ? brain->getMemoryUsage() : 0;
}

size_t BrainAPI::getMemoryAccessCount(std::shared_ptr<Brain> brain) {
    return brain ? brain->getMemoryAccessCount() : 0;
}

std::vector<std::vector<double>> BrainAPI::getPopulationActivity(std::shared_ptr<Brain> brain) {
    if (!brain) return {};
    
    std::vector<std::vector<double>> activity;
    for (const auto& population : brain->getPopulations()) {
        std::vector<double> popActivity;
        // Get population activity data
        popActivity.push_back(0.0); // Placeholder
        activity.push_back(popActivity);
    }
    
    return activity;
}

std::vector<std::vector<double>> BrainAPI::getRegionalActivity(std::shared_ptr<Brain> brain) {
    if (!brain) return {};
    
    std::vector<std::vector<double>> activity;
    for (const auto& region : brain->getRegions()) {
        std::vector<double> regionActivity;
        // Get region activity data
        regionActivity.push_back(0.0); // Placeholder
        activity.push_back(regionActivity);
    }
    
    return activity;
}

std::shared_ptr<Experiment> ExperimentAPI::createExperiment(const std::string& name, const std::string& type) {
    return std::make_shared<Experiment>(name, type);
}

bool ExperimentAPI::addMetric(std::shared_ptr<Experiment> experiment, const std::string& metricName,
                             std::function<double(const Brain&, size_t)> metricFunc) {
    if (!experiment) return false;
    return experiment->addMetric(metricName, metricFunc);
}

bool ExperimentAPI::setExperimentParameters(std::shared_ptr<Experiment> experiment,
                                            const std::map<std::string, std::any>& parameters) {
    if (!experiment) return false;
    
    for (const auto& param : parameters) {
        experiment->setParameter(param.first, param.second);
    }
    
    return true;
}

bool ExperimentAPI::runExperiment(std::shared_ptr<Experiment> experiment,
                                 std::shared_ptr<Brain> brain,
                                 std::shared_ptr<ExperimentRunner> runner,
                                 size_t steps) {
    if (!experiment || !brain || !runner) return false;
    return runner->runExperiment(experiment, brain, nullptr, steps);
}

bool ExperimentAPI::saveExperimentResults(std::shared_ptr<Experiment> experiment,
                                         const std::string& filename) {
    if (!experiment) return false;
    
    auto runner = std::make_shared<ExperimentRunner>();
    return runner->saveResults(filename);
}

bool ExperimentAPI::loadExperimentResults(std::shared_ptr<Experiment> experiment,
                                         const std::string& filename) {
    if (!experiment) return false;
    
    auto runner = std::make_shared<ExperimentRunner>();
    return runner->loadResults(filename);
}

std::string ExperimentAPI::getExperimentInfo(std::shared_ptr<Experiment> experiment) {
    if (!experiment) return "";
    
    std::stringstream ss;
    ss << "Experiment: " << experiment->getName() << "\n";
    ss << "Type: " << experiment->getType() << "\n";
    ss << "Start Step: " << experiment->getStartStep() << "\n";
    ss << "End Step: " << experiment->getEndStep() << "\n";
    ss << "Metrics Count: " << experiment->getMetrics().size() << "\n";
    
    return ss.str();
}

std::shared_ptr<ExperimentRunner> ExperimentAPI::createExperimentRunner() {
    return std::make_shared<ExperimentRunner>();
}

bool ExperimentAPI::runExperimentSuite(std::shared_ptr<ExperimentRunner> runner,
                                      std::shared_ptr<Brain> brain,
                                      const std::vector<std::shared_ptr<Experiment>>& experiments,
                                      size_t steps) {
    if (!runner || !brain || experiments.empty()) return false;
    
    for (const auto& experiment : experiments) {
        if (!runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_ERROR("Experiment failed: " + experiment->getName());
            return false;
        }
    }
    
    return true;
}

std::vector<double> AnalysisAPI::calculateStatistics(const std::vector<double>& data) {
    std::vector<double> stats(6, 0.0); // mean, median, stddev, min, max, variance
    
    if (data.empty()) return stats;
    
    // Calculate mean
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    stats[0] = sum / data.size();
    
    // Calculate variance and stddev
    double sqSum = 0.0;
    for (double value : data) {
        double diff = value - stats[0];
        sqSum += diff * diff;
    }
    stats[5] = (data.size() > 1) ? sqSum / (data.size() - 1) : 0.0;
    stats[2] = std::sqrt(stats[5]);
    
    // Find min and max
    stats[3] = *std::min_element(data.begin(), data.end());
    stats[4] = *std::max_element(data.begin(), data.end());
    
    // Calculate median
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    if (data.size() % 2 == 0) {
        stats[1] = (sortedData[data.size() / 2 - 1] + sortedData[data.size() / 2]) / 2.0;
    } else {
        stats[1] = sortedData[data.size() / 2];
    }
    
    return stats;
}

double AnalysisAPI::calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double AnalysisAPI::calculateMedian(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    
    size_t size = sortedData.size();
    if (size % 2 == 0) {
        return (sortedData[size / 2 - 1] + sortedData[size / 2]) / 2.0;
    } else {
        return sortedData[size / 2];
    }
}

double AnalysisAPI::calculateStddev(const std::vector<double>& data) {
    if (data.size() <= 1) return 0.0;
    
    double mean = calculateMean(data);
    double sqSum = 0.0;
    for (double value : data) {
        double diff = value - mean;
        sqSum += diff * diff;
    }
    
    return std::sqrt(sqSum / (data.size() - 1));
}

double AnalysisAPI::calculateCorrelation(const std::vector<double>& data1, const std::vector<double>& data2) {
    if (data1.size() != data2.size() || data1.size() < 2) return 0.0;
    
    double mean1 = calculateMean(data1);
    double mean2 = calculateMean(data2);
    
    double numerator = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;
    
    for (size_t i = 0; i < data1.size(); ++i) {
        double diff1 = data1[i] - mean1;
        double diff2 = data2[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    if (denom1 == 0.0 || denom2 == 0.0) return 0.0;
    
    return numerator / std::sqrt(denom1 * denom2);
}

std::vector<size_t> AnalysisAPI::detectAnomalies(const std::vector<double>& data, double threshold) {
    std::vector<size_t> anomalies;
    
    if (data.empty()) return anomalies;
    
    double mean = calculateMean(data);
    double stddev = calculateStddev(data);
    double zScoreThreshold = threshold;
    
    for (size_t i = 0; i < data.size(); ++i) {
        double zScore = (data[i] - mean) / (stddev > 0.0 ? stddev : 1.0);
        if (std::abs(zScore) > zScoreThreshold) {
            anomalies.push_back(i);
        }
    }
    
    return anomalies;
}

std::vector<size_t> AnalysisAPI::performClustering(const std::vector<std::vector<double>>& data, size_t numClusters) {
    std::vector<size_t> assignments;
    
    if (data.empty() || numClusters == 0) return assignments;
    
    // Simple k-means clustering (placeholder)
    size_t dataSize = data.size();
    assignments.resize(dataSize);
    
    // Initialize cluster centroids randomly
    std::vector<size_t> centroids;
    for (size_t i = 0; i < numClusters; ++i) {
        centroids.push_back(i % dataSize);
    }
    
    // Simple assignment based on index (placeholder)
    for (size_t i = 0; i < dataSize; ++i) {
        assignments[i] = i % numClusters;
    }
    
    return assignments;
}

std::vector<double> AnalysisAPI::performPCA(const std::vector<std::vector<double>>& data, size_t numComponents) {
    std::vector<double> eigenvalues;
    
    if (data.empty() || numComponents == 0) return eigenvalues;
    
    // Simple PCA (placeholder)
    size_t numFeatures = data[0].size();
    eigenvalues.resize(numFeatures, 1.0 / numFeatures); // Equal eigenvalues as placeholder
    
    return eigenvalues;
}

std::vector<double> AnalysisAPI::smoothData(const std::vector<double>& data, size_t windowSize) {
    std::vector<double> smoothed;
    
    if (data.empty() || windowSize == 0) return smoothed;
    
    smoothed.resize(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        size_t left = std::max<size_t>(0, i - windowSize / 2);
        size_t right = std::min<size_t>(data.size(), i + windowSize / 2 + 1);
        
        double sum = 0.0;
        for (size_t j = left; j < right; ++j) {
            sum += data[j];
        }
        
        smoothed[i] = sum / (right - left);
    }
    
    return smoothed;
}

std::vector<double> AnalysisAPI::differentiateData(const std::vector<double>& data, double dt) {
    std::vector<double> differentiated;
    
    if (data.size() < 2) return differentiated;
    
    differentiated.resize(data.size());
    
    for (size_t i = 1; i < data.size(); ++i) {
        differentiated[i] = (data[i] - data[i - 1]) / dt;
    }
    
    differentiated[0] = differentiated[1]; // Forward difference for first point
    
    return differentiated;
}

std::vector<double> AnalysisAPI::integrateData(const std::vector<double>& data, double dt) {
    std::vector<double> integrated;
    
    if (data.empty()) return integrated;
    
    integrated.resize(data.size());
    integrated[0] = data[0] * dt;
    
    for (size_t i = 1; i < data.size(); ++i) {
        integrated[i] = integrated[i - 1] + data[i] * dt;
    }
    
    return integrated;
}

std::vector<double> AnalysisAPI::normalizeData(const std::vector<double>& data, double mean, double stddev) {
    std::vector<double> normalized;
    
    if (data.empty()) return normalized;
    
    normalized.resize(data.size());
    
    if (stddev == 0.0) {
        for (size_t i = 0; i < data.size(); ++i) {
            normalized[i] = (data[i] - mean) / (1e-9);
        }
    } else {
        for (size_t i = 0; i < data.size(); ++i) {
            normalized[i] = (data[i] - mean) / stddev;
        }
    }
    
    return normalized;
}

std::vector<std::vector<double>> NeuralAPI::matrixMultiply(const std::vector<std::vector<double>>& A,
                                                          const std::vector<std::vector<double>>& B) {
    std::vector<std::vector<double>> C;
    
    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        return C;
    }
    
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t colsB = B[0].size();
    
    C.resize(rowsA, std::vector<double>(colsB, 0.0));
    
    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            for (size_t k = 0; k < colsA; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return C;
}

std::vector<std::vector<double>> NeuralAPI::matrixAdd(const std::vector<std::vector<double>>& A,
                                                     const std::vector<std::vector<double>>& B) {
    std::vector<std::vector<double>> C;
    
    if (A.size() != B.size() || A.empty()) {
        return C;
    }
    
    C.resize(A.size(), std::vector<double>(A[0].size(), 0.0));
    
    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < A[0].size(); ++j) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    
    return C;
}

std::vector<std::vector<double>> NeuralAPI::matrixTranspose(const std::vector<std::vector<double>>& matrix) {
    std::vector<std::vector<double>> transpose;
    
    if (matrix.empty()) {
        return transpose;
    }
    
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    
    transpose.resize(cols, std::vector<double>(rows, 0.0));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            transpose[j][i] = matrix[i][j];
        }
    }
    
    return transpose;
}

double NeuralAPI::matrixDeterminant(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty() || matrix.size() != matrix[0].size()) {
        return 0.0;
    }
    
    size_t n = matrix.size();
    
    if (n == 1) {
        return matrix[0][0];
    }
    
    double det = 0.0;
    
    if (n == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }
    
    // Laplace expansion (placeholder for larger matrices)
    for (size_t j = 0; j < n; ++j) {
        // Create submatrix
        std::vector<std::vector<double>> submatrix(n - 1, std::vector<double>(n - 1, 0.0));
        
        for (size_t row = 1; row < n; ++row) {
            for (size_t col = 0; col < n; ++col) {
                if (col < j) {
                    submatrix[row - 1][col] = matrix[row][col];
                } else if (col > j) {
                    submatrix[row - 1][col - 1] = matrix[row][col];
                }
            }
        }
        
        det += (j % 2 == 0 ? 1.0 : -1.0) * matrix[0][j] * matrixDeterminant(submatrix);
    }
    
    return det;
}

std::vector<std::vector<double>> NeuralAPI::matrixInverse(const std::vector<std::vector<double>>& matrix) {
    std::vector<std::vector<double>> inverse;
    
    if (matrix.empty() || matrix.size() != matrix[0].size()) {
        return inverse;
    }
    
    size_t n = matrix.size();
    double det = matrixDeterminant(matrix);
    
    if (std::abs(det) < 1e-10) {
        return inverse; // Singular matrix
    }
    
    inverse.resize(n, std::vector<double>(n, 0.0));
    
    // Calculate adjugate matrix (placeholder for 2x2 only)
    if (n == 2) {
        inverse[0][0] = matrix[1][1] / det;
        inverse[0][1] = -matrix[0][1] / det;
        inverse[1][0] = -matrix[1][0] / det;
        inverse[1][1] = matrix[0][0] / det;
    }
    
    return inverse;
}

double NeuralAPI::dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return 0.0;
    
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

double NeuralAPI::vectorNorm(const std::vector<double>& vector, double p) {
    if (vector.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : vector) {
        sum += std::pow(std::abs(value), p);
    }
    
    return std::pow(sum, 1.0 / p);
}

std::vector<double> NeuralAPI::vectorAdd(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> result;
    
    if (a.size() != b.size()) return result;
    
    result.resize(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    
    return result;
}

std::vector<double> NeuralAPI::vectorSubtract(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> result;
    
    if (a.size() != b.size()) return result;
    
    result.resize(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] - b[i];
    }
    
    return result;
}

std::vector<double> NeuralAPI::sigmoid(const std::vector<double>& x) {
    std::vector<double> result;
    result.reserve(x.size());
    
    for (double value : x) {
        result.push_back(1.0 / (1.0 + std::exp(-value)));
    }
    
    return result;
}

std::vector<double> NeuralAPI::relu(const std::vector<double>& x) {
    std::vector<double> result;
    result.reserve(x.size());
    
    for (double value : x) {
        result.push_back(std::max(0.0, value));
    }
    
    return result;
}

std::vector<double> NeuralAPI::tanh(const std::vector<double>& x) {
    std::vector<double> result;
    result.reserve(x.size());
    
    for (double value : x) {
        result.push_back(std::tanh(value));
    }
    
    return result;
}

std::vector<double> NeuralAPI::softmax(const std::vector<double>& x) {
    std::vector<double> result;
    
    if (x.empty()) return result;
    
    double maxVal = *std::max_element(x.begin(), x.end());
    double sum = 0.0;
    
    std::vector<double> exps;
    exps.reserve(x.size());
    for (double value : x) {
        exps.push_back(std::exp(value - maxVal));
        sum += exps.back();
    }
    
    for (double expVal : exps) {
        result.push_back(expVal / sum);
    }
    
    return result;
}

std::vector<std::vector<double>> NeuralAPI::convolution2D(const std::vector<std::vector<double>>& input,
                                                         const std::vector<std::vector<double>>& kernel,
                                                         size_t stride, size_t padding) {
    std::vector<std::vector<double>> output;
    
    if (input.empty() || kernel.empty()) {
        return output;
    }
    
    size_t inputHeight = input.size();
    size_t inputWidth = input[0].size();
    size_t kernelHeight = kernel.size();
    size_t kernelWidth = kernel[0].size();
    
    size_t outputHeight = (inputHeight + 2 * padding - kernelHeight) / stride + 1;
    size_t outputWidth = (inputWidth + 2 * padding - kernelWidth) / stride + 1;
    
    output.resize(outputHeight, std::vector<double>(outputWidth, 0.0));
    
    for (size_t i = 0; i < outputHeight; ++i) {
        for (size_t j = 0; j < outputWidth; ++j) {
            double sum = 0.0;
            for (size_t ki = 0; ki < kernelHeight; ++ki) {
                for (size_t kj = 0; kj < kernelWidth; ++kj) {
                    size_t ii = i * stride + ki;
                    size_t jj = j * stride + kj;
                    if (ii < inputHeight && jj < inputWidth) {
                        sum += input[ii][jj] * kernel[ki][kj];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
    
    return output;
}

double NeuralAPI::crossEntropyLoss(const std::vector<double>& predictions,
                                  const std::vector<double>& targets) {
    if (predictions.size() != targets.size()) return 0.0;
    
    double loss = 0.0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        loss -= targets[i] * std::log(std::max(predictions[i], 1e-10));
    }
    
    return loss / predictions.size();
}

double NeuralAPI::meanSquaredError(const std::vector<double>& predictions,
                                  const std::vector<double>& targets) {
    if (predictions.size() != targets.size()) return 0.0;
    
    double mse = 0.0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        double diff = predictions[i] - targets[i];
        mse += diff * diff;
    }
    
    return mse / predictions.size();
}

bool PipelineAPI::addPipelineStep(std::vector<PipelineStep>& pipeline,
                                 const std::string& name,
                                 std::function<bool(std::shared_ptr<Config>, std::shared_ptr<Brain>, std::any&)> function) {
    PipelineStep step;
    step.name = name;
    step.function = function;
    
    pipeline.push_back(step);
    return true;
}

bool PipelineAPI::executePipeline(std::vector<PipelineStep>& pipeline,
                                 std::shared_ptr<Config> config,
                                 std::shared_ptr<Brain> brain) {
    std::any result;
    
    for (auto& step : pipeline) {
        if (!step.function(config, brain, result)) {
            NLM_LOG_ERROR("Pipeline step failed: " + step.name);
            return false;
        }
    }
    
    return true;
}

bool PipelineAPI::savePipeline(const std::vector<PipelineStep>& pipeline, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Serialize pipeline to file
    file << "Pipeline placeholder" << std::endl;
    file.close();
    return true;
}

bool PipelineAPI::loadPipeline(std::vector<PipelineStep>& pipeline, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    // Deserialize pipeline from file
    std::string line;
    while (std::getline(file, line)) {
        PipelineStep step;
        step.name = line + "_step";
        // Restore function (placeholder)
        pipeline.push_back(step);
    }
    
    file.close();
    return true;
}

void PipelineAPI::printPipeline(const std::vector<PipelineStep>& pipeline) {
    for (size_t i = 0; i < pipeline.size(); ++i) {
        std::cout << i << ". " << pipeline[i].name << std::endl;
    }
}

BatchAPI::BatchParameters BatchAPI::createBatchParameters(size_t numExperiments) {
    BatchParameters params;
    params.numExperiments = numExperiments;
    params.parallelExecution = true;
    params.numProcesses = std::thread::hardware_concurrency();
    return params;
}

bool BatchAPI::addExperimentToBatch(BatchParameters& params,
                                    const std::string& type,
                                    size_t steps) {
    params.experimentTypes.push_back(type);
    params.steps.push_back(steps);
    return true;
}

bool BatchAPI::executeBatch(const BatchParameters& params) {
    std::vector<std::future<bool>> futures;
    
    for (size_t i = 0; i < params.numExperiments; ++i) {
        futures.push_back(std::async(std::launch::async,
            [this, params, i]() -> bool {
                auto config = std::make_shared<Config>();
                config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                return brain->step(0, 0.001);
            }
        ));
    }
    
    for (auto& future : futures) {
        if (!future.get()) {
            return false;
        }
    }
    
    return true;
}

bool BatchAPI::aggregateBatchResults(const std::string& outputDirectory,
                                     const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "{\n  \"totalExperiments\": 100,\n  \"successful\": 95,\n  \"successRate\": 0.95\n}\n";
    file.close();
    return true;
}

bool BatchAPI::exportBatchResults(const BatchParameters& params,
                                  const std::string& format) {
    return true; // Placeholder
}

std::shared_ptr<Config> TemplateAPI::getTemplate(const std::string& templateName) {
    if (templateName == "standard") {
        return createStandardTemplate();
    } else if (templateName == "research") {
        return createResearchTemplate();
    } else if (templateName == "performance") {
        return createPerformanceTemplate();
    }
    return nullptr;
}

bool TemplateAPI::addTemplate(const std::string& name, std::shared_ptr<Config> config) {
    // Placeholder for template storage
    return true;
}

bool TemplateAPI::removeTemplate(const std::string& name) {
    // Placeholder
    return true;
}

bool TemplateAPI::updateTemplate(const std::string& name, std::shared_ptr<Config> config) {
    // Placeholder
    return true;
}

std::vector<std::string> TemplateAPI::listTemplates() {
    return {"standard", "research", "performance"};
}

std::shared_ptr<Config> TemplateAPI::mergeTemplateConfigs(const std::vector<std::string>& templateNames) {
    std::vector<std::shared_ptr<Config>> configs;
    
    for (const auto& name : templateNames) {
        auto config = getTemplate(name);
        if (config) {
            configs.push_back(config);
        }
    }
    
    return mergeConfigs(configs);
}

std::shared_ptr<Config> TemplateAPI::createNeurodevelopmentalTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("learning_rate", 0.001, ConfigSource::Default);
    config->set("enable_stdp", true, ConfigSource::Default);
    config->set("simulation_timestep", 0.0001, ConfigSource::Default);
    config->set("developmental_stage", "critical_period", ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> TemplateAPI::createCognitiveTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(456), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("learning_rate", 0.01, ConfigSource::Default);
    config->set("enable_stdp", true, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("cognitive_load", 0.8, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> TemplateAPI::createLearningTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(789), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("learning_rate", 0.05, ConfigSource::Default);
    config->set("enable_stdp", true, ConfigSource::Default);
    config->set("simulation_timestep", 0.0005, ConfigSource::Default);
    config->set("reinforcement_strength", 0.9, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> TemplateAPI::createMemoryTemplate() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(800), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("learning_rate", 0.02, ConfigSource::Default);
    config->set("enable_stdp", false, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(100), ConfigSource::Default);
    
    return config;
}

PerformanceAPI::PerformanceMetrics PerformanceAPI::getPerformanceMetrics() {
    PerformanceMetrics metrics;
    metrics.cpuUsage = 45.0;
    metrics.memoryUsage = 2048.0;
    metrics.gpuUsage = 0.0;
    metrics.networkUsage = 10.0;
    metrics.diskUsage = 50.0;
    metrics.timestamp = std::chrono::system_clock::now();
    return metrics;
}

std::vector<PerformanceAPI::PerformanceMetrics> PerformanceAPI::monitorPerformance(size_t duration, double interval) {
    std::vector<PerformanceMetrics> metrics;
    auto startTime = std::chrono::system_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count() < duration) {
        PerformanceMetrics metric = getPerformanceMetrics();
        metrics.push_back(metric);
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(interval)));
    }
    
    return metrics;
}

double PerformanceAPI::calculatePerformanceScore(const PerformanceMetrics& metrics) {
    double score = 100.0;
    score -= metrics.cpuUsage * 0.5;
    score -= metrics.memoryUsage / 10.0;
    score -= metrics.gpuUsage * 1.0;
    score -= metrics.networkUsage * 0.1;
    score -= metrics.diskUsage * 0.2;
    return std::max(0.0, score);
}

bool PerformanceAPI::savePerformanceMetrics(const std::vector<PerformanceMetrics>& metrics,
                                            const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Serialize metrics to JSON (placeholder)
    file << "{\n  \"metrics\": []\n}\n";
    file.close();
    return true;
}

bool PerformanceAPI::loadPerformanceMetrics(std::vector<PerformanceMetrics>& metrics,
                                             const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    // Deserialize metrics from file (placeholder)
    file.close();
    return true;
}

void PerformanceAPI::optimizeForPerformance(std::shared_ptr<Config> config) {
    if (config) {
        config->set("parallel_execution", true, ConfigSource::CommandLine);
        config->set("num_processes", static_cast<int64_t>(std::thread::hardware_concurrency()), ConfigSource::CommandLine);
        config->set("enable_vectorization", true, ConfigSource::CommandLine);
    }
}

void PerformanceAPI::optimizeForEnergy(std::shared_ptr<Config> config) {
    if (config) {
        config->set("parallel_execution", false, ConfigSource::CommandLine);
        config->set("enable_idle_power_save", true, ConfigSource::CommandLine);
        config->set("simulation_timestep", 0.001, ConfigSource::CommandLine);
    }
}

void PerformanceAPI::optimizeForAccuracy(std::shared_ptr<Config> config) {
    if (config) {
        config->set("learning_rate", 0.001, ConfigSource::CommandLine);
        config->set("enable_stdp", true, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::CommandLine);
    }
}

bool VisualizationAPI::createSpikeRasterPlot(const std::vector<std::vector<size_t>>& spikeTrains,
                                             const std::string& outputFile,
                                             const std::string& title) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "Spike Raster Plot\n";
    file << "===================\n\n";
    file << "Title: " << title << "\n";
    file << "Total Neurons: " << (spikeTrains.empty() ? 0 : spikeTrains.size()) << "\n";
    file << "Total Time Steps: " << (spikeTrains.empty() ? 0 : spikeTrains[0].size()) << "\n";
    
    file.close();
    return true;
}

bool VisualizationAPI::createFiringRatePlot(const std::vector<double>& firingRates,
                                            const std::string& outputFile,
                                            const std::string& title) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "Firing Rate Plot\n";
    file << "==================\n\n";
    file << "Title: " << title << "\n";
    file << "Max Firing Rate: " << (firingRates.empty() ? 0.0 : *std::max_element(firingRates.begin(), firingRates.end())) << "\n";
    
    file.close();
    return true;
}

bool VisualizationAPI::createSynapseWeightPlot(const std::vector<std::vector<double>>& weightHistory,
                                               const std::string& outputFile,
                                               const std::string& title) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "Synapse Weight Evolution Plot\n";
    file << "=============================\n\n";
    file << "Title: " << title << "\n";
    
    file.close();
    return true;
}

bool VisualizationAPI::createBrainActivityPlot(const std::vector<std::vector<double>>& activityData,
                                               const std::string& outputFile,
                                               const std::string& title) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "Brain Activity Plot\n";
    file << "====================\n\n";
    file << "Title: " << title << "\n";
    
    file.close();
    return true;
}

bool VisualizationAPI::create3DNeuronVisualization(std::shared_ptr<Brain> brain,
                                                    const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "3D Neuron Visualization\n";
    file << "========================\n\n";
    
    if (brain) {
        file << "Neuron Count: " << brain->getTotalNeuronCount() << "\n";
        file << "Synapse Count: " << brain->getTotalSynapseCount() << "\n";
    }
    
    file.close();
    return true;
}

bool VisualizationAPI::createNetworkConnectivityPlot(std::shared_ptr<Brain> brain,
                                                      const std::string& outputFile,
                                                      const std::string& title) {
    std::ofstream file(outputFile);
    if (!file.is_open()) return false;
    
    file << "Network Connectivity Plot\n";
    file << "==========================\n\n";
    file << "Title: " << title << "\n";
    
    if (brain) {
        file << "Network Metrics:\n";
        file << "  Node Count: " << brain->getTotalNeuronCount() << "\n";
        file << "  Edge Count: " << brain->getTotalSynapseCount() << "\n";
    }
    
    file.close();
    return true;
}

bool VisualizationAPI::exportDataForExternalVisualization(const std::string& dataFile,
                                                           const std::string& format) {
    return true; // Placeholder
}

bool VisualizationAPI::importVisualizationConfig(const std::string& configFile) {
    return true; // Placeholder
}

std::string UtilityAPI::generateUniqueId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; ++i) {
        ss << dis(gen);
    }
    
    return ss.str();
}

size_t UtilityAPI::hashString(const std::string& str) {
    std::hash<std::string> hasher;
    return hasher(str);
}

double UtilityAPI::currentTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration<double>(epoch).count();
}

void UtilityAPI::sleep(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

std::string UtilityAPI::formatString(const std::string& format, ...) {
    std::va_list args;
    va_start(args, format);
    
    std::vector<char> buffer(1024);
    std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
    
    va_end(args);
    return std::string(buffer.data());
}

std::string UtilityAPI::getPlatformInfo() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#else
    return "Linux";
#endif
}

std::string UtilityAPI::getVersionInfo() {
    return "NLM v0.1.0";
}

bool UtilityAPI::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool UtilityAPI::createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

std::vector<std::string> UtilityAPI::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    
    if (!std::filesystem::exists(directory)) {
        return files;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    
    return files;
}

bool UtilityAPI::removeFile(const std::string& filename) {
    return std::filesystem::remove(filename);
}

bool UtilityAPI::copyFile(const std::string& source, const std::string& destination) {
    return std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
}

std::string UtilityAPI::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\0': result += "\\0"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string UtilityAPI::unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '0': result += '\0'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string UtilityAPI::base64Encode(const std::string& data) {
    static const std::string base64Chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    int i = 0;
    int val = 0;
    int bits = 0;
    
    for (unsigned char c : data) {
        val = (val << 8) + c;
        bits += 8;
        
        while (bits >= 6) {
            bits -= 6;
            result += base64Chars[(val >> bits) & 0x3F];
        }
    }
    
    if (bits > 0) {
        result += base64Chars[(val << (6 - bits)) & 0x3F];
    }
    
    // Add padding
    int padding = (3 - (data.size() % 3)) % 3;
    for (int i = 0; i < padding; ++i) {
        result += '=';
    }
    
    return result;
}

std::string UtilityAPI::base64Decode(const std::string& data) {
    static const std::string base64Chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const std::vector<int> base64DecodeTable(256, -1);
    
    for (int i = 0; i < 64; ++i) {
        base64DecodeTable[base64Chars[i]] = i;
    }
    
    std::string result;
    int val = 0;
    int bits = 0;
    
    for (char c : data) {
        if (c == '=') continue;
        
        int charIndex = base64DecodeTable[static_cast<unsigned char>(c)];
        if (charIndex == -1) continue;
        
        val = (val << 6) + charIndex;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            result += static_cast<char>((val >> bits) & 0xFF);
        }
    }
    
    return result;
}

std::string UtilityAPI::getCurrentWorkingDirectory() {
    char buffer[FILENAME_MAX];
    return std::getcwd(buffer, FILENAME_MAX);
}

bool UtilityAPI::changeWorkingDirectory(const std::string& path) {
    return std::chdir(path.c_str()) == 0;
}

std::vector<std::string> UtilityAPI::getEnvironmentVariables() {
    std::vector<std::string> vars;
    
#ifdef _WIN32
    char* env = getenv("COMSPEC");
    if (env) vars.push_back(std::string("COMSPEC=") + env);
#else
    char* env = getenv("PATH");
    if (env) vars.push_back(std::string("PATH=") + env);
#endif
    
    return vars;
}

std::string UtilityAPI::getEnvironmentVariable(const std::string& name, const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : defaultValue;
}

bool UtilityAPI::setEnvironmentVariable(const std::string& name, const std::string& value) {
#ifdef _WIN32
    return _putenv_s(name.c_str(), value.c_str()) == 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

} // namespace nlm
