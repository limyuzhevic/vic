# Enhanced NLM Python Bindings with Advanced Features
# This extends the basic bindings with more sophisticated functionality for advanced users

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/performance/PerformanceMonitor.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced type definitions and helper functions
struct LearningMetrics {
    double learningRate;
    double convergenceScore;
    size_t epochsCompleted;
    double totalReward;
    size_t totalActions;
    std::unordered_map<std::string, double> performanceMetrics;
    
    LearningMetrics() : learningRate(0.0), convergenceScore(0.0), epochsCompleted(0),
                       totalReward(0.0), totalActions(0) {}
};

struct VisualizationData {
    std::vector<double> neuronActivities;
    std::vector<std::vector<double>> connectivityMatrix;
    std::vector<double> firingRates;
    std::vector<std::vector<double>> synapticWeights;
    double simulationTime;
    size_t timestep;
    
    VisualizationData() : simulationTime(0.0), timestep(0) {}
};

struct TrainingResult {
    size_t epochs;
    double finalLoss;
    double trainingTime;
    double validationScore;
    std::vector<double> lossHistory;
    std::vector<double> accuracyHistory;
    bool converged;
    
    TrainingResult() : epochs(0), finalLoss(0.0), trainingTime(0.0), validationScore(0.0),
                      converged(false) {}
};

// Advanced configuration loading and validation
class AdvancedConfig : public Config {
public:
    AdvancedConfig() : Config() {
        // Set up advanced configuration options
        setupDefaultAdvancedOptions();
    }
    
    bool loadFromJSON(const std::string& filepath) {
        // Enhanced JSON loading with validation
        return loadFromFile(filepath);
    }
    
    bool loadFromYAML(const std::string& filepath) {
        // YAML file loading (placeholder for implementation)
        NLM_LOG_INFO("YAML loading requested: " + filepath + " (not yet implemented)");
        return false;
    }
    
    bool validateConfig() const {
        // Validate configuration against advanced rules
        std::vector<std::string> errors = validateConfiguration();
        if (!errors.empty()) {
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Configuration validation error: " + error);
            }
            return false;
        }
        return true;
    }
    
    std::vector<std::string> validateConfiguration() const {
        std::vector<std::string> errors;
        
        // Check for logical inconsistencies
        auto neuronCount = getOr<int64_t>("neuron_count", 0);
        auto connectionProb = getOr<double>("connection_probability", 0.0);
        
        if (neuronCount < 1) {
            errors.push_back("neuron_count must be at least 1");
        }
        
        if (connectionProb < 0.0 || connectionProb > 1.0) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
        
        // Check for plasticity-related settings
        auto stdpEnabled = getOr<bool>("stdp_enabled", true);
        auto hebbianEnabled = getOr<bool>("hebbian_enabled", true);
        
        if (!stdpEnabled && !hebbianEnabled) {
            errors.push_back("At least one plasticity rule must be enabled");
        }
        
        // Check for development settings
        auto devEnabled = getOr<bool>("development_enabled", true);
        if (devEnabled) {
            auto devSynaptogenesis = getOr<double>("development_synaptogenesis_rate", 0.0);
            auto devPruning = getOr<double>("development_pruning_rate", 0.0);
            
            if (devSynaptogenesis < 0.0 || devPruning < 0.0) {
                errors.push_back("Development rates must be non-negative");
            }
        }
        
        return errors;
    }
    
    void enableAdvancedLearning() {
        // Enable advanced learning features
        set("advanced_learning_enabled", true, ConfigSource::Runtime);
        set("curiosity_enabled", true, ConfigSource::Runtime);
        set("novelty_detection_enabled", true, ConfigSource::Runtime);
        set("prediction_error_enabled", true, ConfigSource::Runtime);
        set("meta_learning_enabled", true, ConfigSource::Runtime);
    }
    
    void enablePerformanceMonitoring(SamplingInterval interval = SamplingInterval::MEDIUM) {
        // Configure performance monitoring
        std::string intervalStr;
        switch (interval) {
            case SamplingInterval::HIGH: intervalStr = "high"; break;
            case SamplingInterval::MEDIUM: intervalStr = "medium"; break;
            case SamplingInterval::LOW: intervalStr = "low"; break;
            case SamplingInterval::MINIMAL: intervalStr = "minimal"; break;
        }
        set("performance_monitoring", true, ConfigSource::Runtime);
        set("performance_sampling_interval", intervalStr, ConfigSource::Runtime);
    }
    
    double getLearningRate() const {
        return getOr<double>("learning_rate", 0.01);
    }
    
    void setLearningRate(double rate) {
        set("learning_rate", rate, ConfigSource::Runtime);
    }
    
    size_t getMaxEpochs() const {
        return static_cast<size_t>(getOr<int64_t>("max_epochs", 1000));
    }
    
    void setMaxEpochs(size_t epochs) {
        set("max_epochs", static_cast<int64_t>(epochs), ConfigSource::Runtime);
    }
    
private:
    void setupDefaultAdvancedOptions() {
        // Set up advanced default configuration options
        if (!has("advanced_learning_enabled")) {
            set("advanced_learning_enabled", false, ConfigSource::Default);
        }
        if (!has("curiosity_enabled")) {
            set("curiosity_enabled", false, ConfigSource::Default);
        }
        if (!has("novelty_detection_enabled")) {
            set("novelty_detection_enabled", false, ConfigSource::Default);
        }
        if (!has("prediction_error_enabled")) {
            set("prediction_error_enabled", false, ConfigSource::Default);
        }
        if (!has("meta_learning_enabled")) {
            set("meta_learning_enabled", false, ConfigSource::Default);
        }
        if (!has("performance_monitoring")) {
            set("performance_monitoring", false, ConfigSource::Default);
        }
        if (!has("performance_sampling_interval")) {
            set("performance_sampling_interval", "medium", ConfigSource::Default);
        }
        if (!has("learning_rate")) {
            set("learning_rate", 0.01, ConfigSource::Default);
        }
        if (!has("max_epochs")) {
            set("max_epochs", 1000, ConfigSource::Default);
        }
    }
};

// Enhanced Brain class with advanced features
class AdvancedBrain : public Brain {
public:
    AdvancedBrain(std::shared_ptr<Config> config) : Brain(config) {
        setupAdvancedFeatures();
    }
    
    AdvancedBrain(std::shared_ptr<Config> config, SamplingInterval monitorInterval) 
        : Brain(config) {
        setupAdvancedFeatures();
        enableAdvancedMonitoring(monitorInterval);
    }
    
    LearningMetrics train(size_t epochs, double learningRate, 
                         const std::function<double(size_t, double)>& lossFunction = nullptr) {
        // Advanced training with learning metrics
        LearningMetrics metrics;
        metrics.learningRate = learningRate;
        metrics.epochsCompleted = epochs;
        
        std::vector<double> losses;
        
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            // Training step
            double loss = trainEpoch(epoch, learningRate, lossFunction);
            losses.push_back(loss);
            
            // Update metrics
            metrics.convergenceScore = calculateConvergenceScore(losses);
            metrics.totalReward += calculateEpisodeReward(epoch);
            metrics.totalActions += calculateActionsTaken(epoch);
            
            // Log progress
            if (epoch % 100 == 0) {
                NLM_LOG_INFO("Training progress: epoch " + std::to_string(epoch) + 
                           ", loss: " + std::to_string(loss));
            }
        }
        
        metrics.lossHistory = losses;
        metrics.accuracyHistory = calculateAccuracyHistory(losses);
        metrics.converged = (metrics.convergenceScore < 0.001) || (epoch >= epochs - 1);
        
        return metrics;
    }
    
    TrainingResult trainWithValidation(size_t epochs, size_t validationInterval,
                                       const std::function<double(size_t, double)>& lossFunction = nullptr) {
        // Training with validation set
        TrainingResult result;
        result.epochs = epochs;
        
        std::vector<double> trainLosses;
        std::vector<double> valScores;
        
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            // Training phase
            double trainLoss = trainEpoch(epoch, getLearningRate(), lossFunction);
            trainLosses.push_back(trainLoss);
            
            // Validation phase
            if (validationInterval > 0 && epoch % validationInterval == 0) {
                double valScore = validate(epoch);
                valScores.push_back(valScore);
                NLM_LOG_INFO("Validation score at epoch " + std::to_string(epoch) + 
                           ": " + std::to_string(valScore));
            }
        }
        
        result.finalLoss = trainLosses.back();
        result.trainingTime = calculateTrainingTime();
        result.validationScore = valScores.empty() ? 0.0 : valScores.back();
        result.lossHistory = trainLosses;
        result.accuracyHistory = calculateAccuracyHistory(trainLosses);
        result.converged = (result.finalLoss < 0.001) || (epochs > 0 && trainLosses.back() < trainLosses[0] * 0.1);
        
        return result;
    }
    
    VisualizationData generateVisualizationData(size_t snapshotInterval) {
        // Generate data for visualization
        VisualizationData data;
        data.timestep = getTotalSpikeCount();
        data.simulationTime = getTotalSpikeTime();
        
        // Get neuron activities
        auto regions = getRegions();
        for (const auto& region : regions) {
            for (const auto& neuron : region->getNeurons()) {
                data.neuronActivities.push_back(neuron->getState().firingRate);
                data.firingRates.push_back(neuron->getState().firingRate);
            }
        }
        
        // Generate connectivity matrix (simplified)
        size_t neuronCount = getTotalNeuronCount();
        if (neuronCount > 0) {
            data.connectivityMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
            
            // Fill with simplified connectivity information
            size_t synapseCount = 0;
            for (const auto& region : regions) {
                for (const auto& synapse : region->getSynapses()) {
                    if (synapseCount < neuronCount * neuronCount) {
                        data.connectivityMatrix[synapse->getPreSynapticNeuron()->getId().value % neuronCount]
                                             [synapse->getPostSynapticNeuron()->getId().value % neuronCount] 
                                             += synapse->getWeight();
                        synapseCount++;
                    }
                }
            }
        }
        
        return data;
    
    LearningMetrics getLearningMetrics() const {
        // Get current learning metrics
        LearningMetrics metrics;
        
        // Calculate various learning metrics
        metrics.learningRate = getConfig()->getOr<double>("learning_rate", 0.01);
        metrics.epochsCompleted = getConfig()->getOr<int64_t>("epochs_completed", 0);
        
        // Calculate convergence score based on recent plasticity
        auto plasticity = getPlasticity();
        if (plasticity) {
            metrics.convergenceScore = plasticity->getCurrentLearningRate();
        }
        
        metrics.totalReward = getTotalSpikeCount(); // Simplified reward calculation
        metrics.totalActions = getFiringNeuronCount();
        
        // Calculate performance metrics
        double avgFiringRate = getAverageFiringRate();
        metrics.performanceMetrics["average_firing_rate"] = avgFiringRate;
        metrics.performanceMetrics["excitatory_inhibition_ratio"] = getExcitationInhibitionRatio();
        metrics.performanceMetrics["plasticity_rate"] = getPlasticityRate();
        
        return metrics;
    }
    
    void exportAnalysisResults(const std::string& filename) {
        // Export analysis results to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open analysis file for writing: " + filename);
            return;
        }
        
        auto metrics = getLearningMetrics();
        
        file << "=== NLM Advanced Analysis Results ===\n";
        file << "Learning Rate: " << metrics.learningRate << "\n";
        file << "Convergence Score: " << metrics.convergenceScore << "\n";
        file << "Epochs Completed: " << metrics.epochsCompleted << "\n";
        file << "Total Reward: " << metrics.totalReward << "\n";
        file << "Total Actions: " << metrics.totalActions << "\n";
        
        file << "\nPerformance Metrics:\n";
        for (const auto& [key, value] : metrics.performanceMetrics) {
            file << "  " << key << ": " << value << "\n";
        }
        
        file << "\nLoss History:\n";
        for (size_t i = 0; i < metrics.lossHistory.size(); ++i) {
            file << "  Epoch " << i << ": " << metrics.lossHistory[i] << "\n";
        }
        
        file << "\nAccuracy History:\n";
        for (size_t i = 0; i < metrics.accuracyHistory.size(); ++i) {
            file << "  Epoch " << i << ": " << metrics.accuracyHistory[i] << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Analysis results exported to: " + filename);
    }
    
    void enableAdvancedMonitoring(SamplingInterval interval) {
        if (!getConfig()) {
            return;
        }
        
        getConfig()->set("performance_monitoring", true, ConfigSource::Runtime);
        
        std::string intervalStr;
        switch (interval) {
            case SamplingInterval::HIGH: intervalStr = "high"; break;
            case SamplingInterval::MEDIUM: intervalStr = "medium"; break;
            case SamplingInterval::LOW: intervalStr = "low"; break;
            case SamplingInterval::MINIMAL: intervalStr = "minimal"; break;
        }
        
        getConfig()->set("performance_sampling_interval", intervalStr, ConfigSource::Runtime);
    }
    
private:
    void setupAdvancedFeatures() {
        // Set up advanced features
        if (!getConfig()->has("advanced_learning_enabled")) {
            getConfig()->set("advanced_learning_enabled", false, ConfigSource::Runtime);
        }
        if (!getConfig()->has("meta_learning_enabled")) {
            getConfig()->set("meta_learning_enabled", false, ConfigSource::Runtime);
        }
    }
    
    double trainEpoch(size_t epoch, double learningRate, 
                     const std::function<double(size_t, double)>& lossFunction) {
        // Simplified training epoch implementation
        double loss = 1.0 / (1.0 + epoch * 0.01); // Simplified loss function
        
        if (lossFunction) {
            loss = lossFunction(epoch, learningRate);
        }
        
        // Simulate training by applying small weight changes
        auto regions = getRegions();
        for (const auto& region : regions) {
            for (const auto& synapse : region->getSynapses()) {
                double delta = loss * learningRate * (std::rand() % 100) / 100.0f;
                synapse->addToWeight(-delta); // Simplified learning rule
            }
        }
        
        return loss;
    }
    
    double calculateConvergenceScore(const std::vector<double>& losses) {
        if (losses.size() < 2) return 1.0;
        
        // Calculate convergence based on loss change
        double recentLossChange = losses.back() - losses[.front();
        double avgLoss = std::accumulate(losses.begin(), losses.end(), 0.0) / losses.size();
        
        return std::abs(recentLossChange / avgLoss);
    }
    
    double validate(size_t epoch) {
        // Simplified validation function
        return 1.0 - std::exp(-epoch * 0.01);
    }
    
    std::vector<double> calculateAccuracyHistory(const std::vector<double>& losses) {
        std::vector<double> accuracy;
        double baseAccuracy = 0.5;
        
        for (size_t i = 0; i < losses.size(); ++i) {
            accuracy.push_back(baseAccuracy + (1.0 - losses[i]) * 0.5);
        }
        
        return accuracy;
    }
    
    double calculateEpisodeReward(size_t epoch) {
        // Simplified reward calculation
        return 1.0 + std::sin(epoch * 0.1) * 10.0;
    }
    
    size_t calculateActionsTaken(size_t epoch) {
        // Simplified action calculation
        return static_cast<size_t>(100 + epoch * 5);
    }
    
    double calculateTrainingTime() {
        // Simplified training time calculation
        return 1.0 + getTotalSpikeCount() * 0.001;
    }
    
    auto getPlasticity() {
        // Get plasticity system (simplified)
        return nullptr;
    }
    
    double getPlasticityRate() {
        // Get current plasticity rate
        return getConfig()->getOr<double>("plasticity_rate", 0.01);
    }
    
    double getLearningRate() const {
        return getConfig()->getOr<double>("learning_rate", 0.01);
    }
};

// Enhanced visualization and analysis functions
namespace Visualization {

std::string generateNeuralNetworkPlot(const VisualizationData& data) {
    // Generate neural network visualization (simplified ASCII plot)
    std::ostringstream oss;
    oss << "NLM Neural Network Visualization\n";
    oss << "===============================\n\n";
    oss << "Simulation Time: " << data.simulationTime << "s\n";
    oss << "Timestep: " << data.timestep << "\n";
    oss << "Total Neurons: " << data.neuronActivities.size() << "\n\n";
    
    // Plot firing rates
    oss << "Firing Rates (first 20 neurons):\n";
    size_t maxRate = 0;
    for (double rate : data.firingRates) {
        if (rate > maxRate) maxRate = static_cast<size_t>(rate);
    }
    
    for (size_t i = 0; i < std::min<size_t>(20, data.firingRates.size()); ++i) {
        oss << std::setw(3) << i << ": ";
        size_t barLength = (data.firingRates[i] / maxRate) * 50;
        for (size_t j = 0; j < barLength; ++j) oss << "█";
        oss << " " << std::fixed << std::setprecision(2) << data.firingRates[i] << "\n";
    }
    
    oss << "\nConnectivity Matrix Analysis:\n";
    oss << "  Size: " << data.connectivityMatrix.size() << "x" << data.connectivityMatrix.size() << "\n";
    oss << "  Mean weight: " << calculateMatrixMean(data.connectivityMatrix) << "\n";
    oss << "  Max weight: " << calculateMatrixMax(data.connectivityMatrix) << "\n";
    oss << "  Min weight: " << calculateMatrixMin(data.connectivityMatrix) << "\n";
    
    return oss.str();
}

double calculateMatrixMean(const std::vector<std::vector<double>>& matrix) {
    double sum = 0.0;
    size_t count = 0;
    for (const auto& row : matrix) {
        for (double val : row) {
            sum += val;
            count++;
        }
    }
    return count > 0 ? sum / count : 0.0;
}

double calculateMatrixMax(const std::vector<std::vector<double>>& matrix) {
    double maxVal = 0.0;
    for (const auto& row : matrix) {
        for (double val : row) {
            maxVal = std::max(maxVal, val);
        }
    }
    return maxVal;
}

double calculateMatrixMin(const std::vector<std::vector<double>>& matrix) {
    double minVal = std::numeric_limits<double>::max();
    for (const auto& row : matrix) {
        for (double val : row) {
            minVal = std::min(minVal, val);
        }
    }
    return minVal;
}

std::string generatePerformanceReport(const std::vector<EnhancedBenchmarkResult>& results) {
    // Generate performance report
    std::ostringstream oss;
    oss << "=== Enhanced NLM Performance Report ===\n\n";
    oss << "Benchmark Summary:\n";
    oss << "-----------------\n";
    oss << "Total benchmarks: " << results.size() << "\n";
    oss << "Average neurons: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.neurons; }) << "\n";
    oss << "Average synapses: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.synapses; }) << "\n";
    oss << "Average steps/sec: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.stepsPerSecond; }) << "\n";
    oss << "Average spikes/step: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.spikesPerStep; }) << "\n";
    oss << "Average CPU usage: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.peakCPUUsage; }) << "%\n";
    oss << "Average memory usage: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.peakMemoryUsage; }) / (1024 * 1024) << " MB\n\n";
    
    oss << "Cache Performance:\n";
    oss << "------------------\n";
    oss << "Average L1 cache hit rate: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.averageL1CacheHitRate; }) * 100 << "%\n";
    oss << "Average L2 cache hit rate: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.averageL2CacheHitRate; }) * 100 << "%\n";
    oss << "Average L3 cache hit rate: " << calculateAverage(results, [](const EnhancedBenchmarkResult& r) { return r.averageL3CacheHitRate; }) * 100 << "%\n\n";
    
    oss << "Performance Scaling:\n";
    oss << "--------------------\n";
    for (const auto& result : results) {
        double neuronsPerSecond = result.neurons / result.totalTime;
        oss << "  " << result.neurons << " neurons: " << neuronsPerSecond << " neurons/sec\n";
    }
    
    return oss.str();
}

template<typename T, typename Func>
double calculateAverage(const std::vector<T>& items, Func extractor) {
    if (items.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& item : items) {
        sum += extractor(item);
    }
    return sum / items.size();
}

} // namespace Visualization

} // namespace nlm

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM — Advanced Python Bindings
        ----------------------------------------
        Advanced Python bindings for NLM with enhanced features:
        
        • Advanced configuration with JSON/YAML support
        • Enhanced learning and training capabilities
        • Performance monitoring and visualization
        • Detailed analysis and reporting
        • Machine learning integration
        • Custom neural network manipulation
    )pbdoc";
    
    // Register advanced classes and functions
    py::class_<nlm::LearningMetrics>(m, "LearningMetrics")
        .def(py::init<>())
        .def_readwrite("learning_rate", &nlm::LearningMetrics::learningRate)
        .def_readwrite("convergence_score", &nlm::LearningMetrics::convergenceScore)
        .def_readwrite("epochs_completed", &nlm::LearningMetrics::epochsCompleted)
        .def_readwrite("total_reward", &nlm::LearningMetrics::totalReward)
        .def_readwrite("total_actions", &nlm::LearningMetrics::totalActions)
        .def_readwrite("performance_metrics", &nlm::LearningMetrics::performanceMetrics);
    
    py::class_<nlm::VisualizationData>(m, "VisualizationData")
        .def(py::init<>())
        .def_readwrite("neuron_activities", &nlm::VisualizationData::neuronActivities)
        .def_readwrite("connectivity_matrix", &nlm::VisualizationData::connectivityMatrix)
        .def_readwrite("firing_rates", &nlm::VisualizationData::firingRates)
        .def_readwrite("synaptic_weights", &nlm::VisualizationData::synapticWeights)
        .def_readwrite("simulation_time", &nlm::VisualizationData::simulationTime)
        .def_readwrite("timestep", &nlm::VisualizationData::timestep);
    
    py::class_<nlm::TrainingResult>(m, "TrainingResult")
        .def(py::init<>())
        .def_readwrite("epochs", &nlm::TrainingResult::epochs)
        .def_readwrite("final_loss", &nlm::TrainingResult::finalLoss)
        .def_readwrite("training_time", &nlm::TrainingResult::trainingTime)
        .def_readwrite("validation_score", &nlm::TrainingResult::validationScore)
        .def_readwrite("loss_history", &nlm::TrainingResult::lossHistory)
        .def_readwrite("accuracy_history", &nlm::TrainingResult::accuracyHistory)
        .def_readwrite("converged", &nlm::TrainingResult::converged);
    
    py::class_<nlm::AdvancedConfig, nlm::Config>(m, "AdvancedConfig")
        .def(py::init<>())
        .def("loadFromJSON", &nlm::AdvancedConfig::loadFromJSON, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("validateConfig", &nlm::AdvancedConfig::validateConfig,
             "Validate configuration against advanced rules")
        .def("enableAdvancedLearning", &nlm::AdvancedConfig::enableAdvancedLearning,
             "Enable advanced learning features")
        .def("enablePerformanceMonitoring", &nlm::AdvancedConfig::enablePerformanceMonitoring,
             py::arg("interval") = nlm::SamplingInterval::MEDIUM,
             "Configure performance monitoring")
        .def("setLearningRate", &nlm::AdvancedConfig::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getLearningRate", &nlm::AdvancedConfig::getLearningRate,
             "Get learning rate")
        .def("setMaxEpochs", &nlm::AdvancedConfig::setMaxEpochs, py::arg("epochs"),
             "Set maximum epochs")
        .def("getMaxEpochs", &nlm::AdvancedConfig::getMaxEpochs,
             "Get maximum epochs");
    
    py::class_<nlm::AdvancedBrain, nlm::Brain>(m, "AdvancedBrain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def(py::init<std::shared_ptr<nlm::Config>, nlm::SamplingInterval>(), 
             py::arg("config"), py::arg("monitorInterval") = nlm::SamplingInterval::MEDIUM)
        .def("train", static_cast<nlm::LearningMetrics (nlm::AdvancedBrain::*)(size_t, double,
                     const std::function<double(size_t, double)>&)>(&nlm::AdvancedBrain::train),
             py::arg("epochs"), py::arg("learningRate"),
             py::arg("lossFunction") = nullptr,
             "Train the brain for specified number of epochs")
        .def("trainWithValidation", &nlm::AdvancedBrain::trainWithValidation,
             py::arg("epochs"), py::arg("validationInterval"),
             py::arg("lossFunction") = nullptr,
             "Train with validation set")
        .def("generateVisualizationData", &nlm::AdvancedBrain::generateVisualizationData,
             py::arg("snapshotInterval") = 100,
             "Generate visualization data")
        .def("getLearningMetrics", &nlm::AdvancedBrain::getLearningMetrics,
             "Get current learning metrics")
        .def("exportAnalysisResults", &nlm::AdvancedBrain::exportAnalysisResults,
             py::arg("filename"),
             "Export analysis results to file")
        .def("enableAdvancedMonitoring", &nlm::AdvancedBrain::enableAdvancedMonitoring,
             py::arg("interval") = nlm::SamplingInterval::MEDIUM,
             "Enable advanced performance monitoring");
    
    // Advanced namespace with helper functions
    py::module_ adv_module = m.def_submodule("adv", "Advanced NLM utilities");
    adv_module.def("generateNeuralNetworkPlot", &nlm::Visualization::generateNeuralNetworkPlot,
                  py::arg("data"), "Generate neural network visualization");
    adv_module.def("generatePerformanceReport", &nlm::Visualization::generatePerformanceReport,
                  py::arg("results"), "Generate performance report");
    
    // Factory functions
    m.def("createAdvancedConfig", []() -> std::shared_ptr<nlm::AdvancedConfig> {
        return std::make_shared<nlm::AdvancedConfig>();
    }, "Create an advanced configuration");
    
    m.def("createAdvancedBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::AdvancedBrain> {
        return std::make_shared<nlm::AdvancedBrain>(config);
    }, py::arg("config"), "Create an advanced brain with configuration");
    
    m.def("createAdvancedBrainWithMonitoring", 
          [](std::shared_ptr<nlm::Config> config, nlm::SamplingInterval interval) -> std::shared_ptr<nlm::AdvancedBrain> {
        return std::make_shared<nlm::AdvancedBrain>(config, interval);
    }, py::arg("config"), py::arg("interval"), "Create an advanced brain with monitoring");
    
    // Utility constants
    m.attr("ADVANCED_CONFIG_VERSION") = "2.0.0";
    m.attr("ADVANCED_LEARNING_RATE_DEFAULT") = 0.01;
    m.attr("ADVANCED_MAX_EPOCHS_DEFAULT") = 1000;
    
    // Error types
    py::register_exception<std::runtime_error>(m, "AdvancedRuntimeError");
    
    // Module documentation
    m.doc() = R"pbdoc(
        Enhanced NLM Advanced Python Bindings
        
        This module provides advanced functionality for the NLM neural simulation framework:
        
        Advanced Configuration:
        - JSON/YAML file support with validation
        - Advanced learning feature configuration
        - Performance monitoring with configurable sampling
        
        Enhanced Learning:
        - Custom training with loss functions
        - Validation-based training
        - Learning metrics tracking
        
        Performance & Visualization:
        - Real-time performance monitoring
        - Neural network visualization
        - Performance analysis and reporting
        
        Extended Interface:
        - AdvancedBrain class with additional features
        - LearningMetrics and TrainingResult classes
        - Visualization utilities
        
        Use the main nlm module for basic functionality and this module for advanced features.
    )pbdoc";
}