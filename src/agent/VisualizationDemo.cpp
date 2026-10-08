#include "VisualizationSystem.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <system/metrics.h>  // Simplified system metrics

namespace nlm {

// AdvancedPerformanceAnalyzer implementation
AdvancedPerformanceAnalyzer::AdvancedPerformanceAnalyzer() {
    // In a real implementation, this would initialize ML models and data structures
    NLM_LOG_INFO("AdvancedPerformanceAnalyzer initialized");
}

AdvancedPerformanceAnalyzer::~AdvancedPerformanceAnalyzer() {
    NLM_LOG_INFO("AdvancedPerformanceAnalyzer destroyed");
}

AnalysisResult AdvancedPerformanceAnalyzer::analyzeNeuralDynamics(const NeuralActivityData& data) const {
    AnalysisResult result;
    result.analysisType = "neural_dynamics";
    result.description = "Analysis of neural activity patterns and dynamics";
    
    if (data.neuronActivities.empty()) {
        result.insights.push_back("No neural activity data to analyze");
        return result;
    }
    
    // Compute basic statistics
    std::map<std::string, double> stats;
    double sum = std::accumulate(data.neuronActivities.begin(), data.neuronActivities.end(), 0.0);
    double mean = sum / data.neuronActivities.size();
    
    double sqSum = 0.0;
    for (float val : data.neuronActivities) {
        double diff = val - mean;
        sqSum += diff * diff;
    }
    double stddev = std::sqrt(sqSum / data.neuronActivities.size());
    
    stats["mean"] = mean;
    stats["stddev"] = stddev;
    stats["max"] = *std::max_element(data.neuronActivities.begin(), data.neuronActivities.end());
    stats["min"] = *std::min_element(data.neuronActivities.begin(), data.neuronActivities.end());
    
    result.metrics["basic_stats"] = stats;
    
    // Analyze firing patterns
    std::vector<float> thresholded = data.neuronActivities;
    for (float& val : thresholded) {
        val = val > 0.5 ? 1.0f : 0.0f; // Binary threshold
    }
    
    int spikeCount = std::accumulate(thresholded.begin(), thresholded.end(), 0);
    result.metrics["spike_count"] = static_cast<double>(spikeCount);
    result.metrics["spike_rate"] = static_cast<double>(spikeCount) / data.neuronActivities.size();
    
    // Pattern analysis
    std::vector<double> autocorr;
    if (data.neuronActivities.size() > 10) {
        for (size_t lag = 1; lag <= 10; ++lag) {
            double corr = 0.0;
            for (size_t i = 0; i < data.neuronActivities.size() - lag; ++i) {
                corr += data.neuronActivities[i] * data.neuronActivities[i + lag];
            }
            corr /= (data.neuronActivities.size() - lag);
            autocorr.push_back(corr);
        }
        
        double maxAutocorr = *std::max_element(autocorr.begin(), autocorr.end());
        result.metrics["autocorrelation"] = maxAutocorr;
        
        if (maxAutocorr > 0.5) {
            result.insights.push_back("Strong temporal correlation detected - potential rhythmic activity");
        }
    }
    
    // Analyze synchrony
    double synchrony = 0.0;
    for (size_t i = 0; i < data.neuronActivities.size(); ++i) {
        for (size_t j = i + 1; j < data.neuronActivities.size(); ++j) {
            double corr = data.neuronActivities[i] * data.neuronActivities[j];
            synchrony += corr;
        }
    }
    synchrony /= (data.neuronActivities.size() * (data.neuronActivities.size() - 1) / 2);
    result.metrics["synchrony"] = synchrony;
    
    if (synchrony > 0.3) {
        result.insights.push_back("High neural synchrony detected - potential synchronized processing");
    }
    
    // Generate insights
    if (result.metrics["spike_rate"] > 0.1) {
        result.insights.push_back("High firing rate indicates active neural processing");
    }
    
    if (result.metrics["autocorrelation"] > 0.7) {
        result.insights.push_back("Strong temporal persistence suggests working memory or attractor dynamics");
    }
    
    // Generate recommendations
    result.recommendations.push_back("Monitor for changes in firing patterns over time");
    result.recommendations.push_back("Consider adjusting neural thresholds for better pattern separation");
    
    result.confidence = std::min(1.0, static_cast<double>(data.neuronActivities.size()) / 100.0);
    
    return result;
}

AnalysisResult AdvancedPerformanceAnalyzer::analyzeLearningProgress(const std::vector<SimulationMetrics>& metrics) const {
    AnalysisResult result;
    result.analysisType = "learning_progress";
    result.description = "Analysis of learning patterns and progress over time";
    
    if (metrics.size() < 5) {
        result.insights.push_back("Insufficient data for learning analysis");
        return result;
    }
    
    // Analyze reward trends
    std::vector<double> rewards;
    for (const auto& m : metrics) {
        rewards.push_back(m.totalReward);
    }
    
    double avgReward = mean(rewards);
    double maxReward = *std::max_element(rewards.begin(), rewards.end());
    double minReward = *std::min_element(rewards.begin(), rewards.end());
    
    result.metrics["average_reward"] = avgReward;
    result.metrics["max_reward"] = maxReward;
    result.metrics["min_reward"] = minReward;
    
    // Detect trends
    std::vector<double> trends;
    for (size_t i = 1; i < metrics.size(); ++i) {
        double trend = metrics[i].totalReward - metrics[i-1].totalReward;
        trends.push_back(trend);
    }
    
    double avgTrend = mean(trends);
    result.metrics["reward_trend"] = avgTrend;
    
    if (avgTrend > 0.1) {
        result.insights.push_back("Positive learning trend detected - performance improving");
        result.recommendations.push_back("Increase task difficulty to continue challenge");
    } else if (avgTrend < -0.1) {
        result.insights.push_back("Negative learning trend detected - performance declining");
        result.recommendations.push_back("Review task complexity or provide additional reinforcement");
    } else {
        result.insights.push_back("Stable learning performance - system in equilibrium");
        result.recommendations.push_back("Maintain current conditions to preserve performance");
    }
    
    // Analyze curiosity levels
    std::vector<double> curiosityLevels;
    for (const auto& m : metrics) {
        curiosityLevels.push_back(m.curiosityLevel);
    }
    
    double avgCuriosity = mean(curiosityLevels);
    result.metrics["average_curiosity"] = avgCuriosity;
    
    if (avgCuriosity > 0.5) {
        result.insights.push_back("High curiosity levels indicate active exploration");
    }
    
    // Analyze prediction error
    std::vector<double> predictionErrors;
    for (const auto& m : metrics) {
        predictionErrors.push_back(m.predictionError);
    }
    
    double avgPredictionError = mean(predictionErrors);
    result.metrics["average_prediction_error"] = avgPredictionError;
    
    if (avgPredictionError < 0.1) {
        result.insights.push_back("Low prediction error indicates good model of environment");
    } else if (avgPredictionError > 0.5) {
        result.insights.push_back("High prediction error suggests model mismatch or novelty");
    }
    
    // Learning rate analysis
    std::vector<double> learningRates;
    for (size_t i = 1; i < metrics.size(); ++i) {
        double learningRate = std::abs(metrics[i].totalReward - metrics[i-1].totalReward);
        learningRates.push_back(learningRate);
    }
    
    double avgLearningRate = mean(learningRates);
    result.metrics["average_learning_rate"] = avgLearningRate;
    
    // Calculate convergence
    double firstThird = avgReward;
    double lastThird = mean(std::vector<double>(rewards.end() - metrics.size()/3, rewards.end()));
    double convergence = (lastThird - firstThird) / (firstThird + 0.001);
    result.metrics["convergence"] = convergence;
    
    if (std::abs(convergence) < 0.1) {
        result.insights.push_back("Learning appears to have converged");
    }
    
    result.confidence = std::min(1.0, static_cast<double>(metrics.size()) / 50.0);
    
    return result;
}

AnalysisResult AdvancedPerformanceAnalyzer::analyzePlasticity(const NeuralActivityData& data) const {
    AnalysisResult result;
    result.analysisType = "plasticity";
    result.description = "Analysis of synaptic plasticity and weight changes";
    
    if (data.synapticWeights.empty()) {
        result.insights.push_back("No synaptic weight data to analyze");
        return result;
    }
    
    // Compute plasticity metrics
    double totalWeight = std::accumulate(data.synapticWeights.begin(), data.synapticWeights.end(), 0.0);
    double avgWeight = totalWeight / data.synapticWeights.size();
    
    double weightVariance = 0.0;
    for (float weight : data.synapticWeights) {
        double diff = weight - avgWeight;
        weightVariance += diff * diff;
    }
    weightVariance /= data.synapticWeights.size();
    
    double weightStddev = std::sqrt(weightVariance);
    
    result.metrics["average_weight"] = avgWeight;
    result.metrics["weight_variance"] = weightVariance;
    result.metrics["weight_stddev"] = weightStddev;
    result.metrics["total_weight"] = totalWeight;
    
    // Analyze weight distribution
    std::vector<float> strongWeights;
    for (float weight : data.synapticWeights) {
        if (weight > 0.7) {
            strongWeights.push_back(weight);
        }
    }
    
    result.metrics["strong_weight_count"] = static_cast<double>(strongWeights.size());
    result.metrics["strong_weight_percentage"] = 
        (strongWeights.size() / data.synapticWeights.size()) * 100.0;
    
    // Analyze weight stability (using connections as proxy)
    double stability = 0.0;
    for (const auto& row : data.connections) {
        for (float weight : row) {
            stability += weight;
        }
    }
    stability /= data.connections.size() * data.connections.size();
    result.metrics["stability_index"] = stability;
    
    if (stability > 0.5) {
        result.insights.push_back("High network stability - synaptic weights are well-established");
    } else if (stability < 0.2) {
        result.insights.push_back("Low network stability - system is highly changeable");
    }
    
    // Analyze Hebbian-like patterns
    double hebbianPotential = 0.0;
    for (float weight : data.synapticWeights) {
        hebbianPotential += weight * (1.0 - weight); // Maximum at weight = 0.5
    }
    result.metrics["hebbian_potential"] = hebbianPotential / data.synapticWeights.size();
    
    if (result.metrics["hebbian_potential"] > 0.2) {
        result.insights.push_back("High potential for Hebbian learning - balanced excitatory/inhibitory weights");
    }
    
    // Generate recommendations
    result.recommendations.push_back("Monitor weight distribution for signs of runaway excitation or inhibition");
    result.recommendations.push_back("Consider implementing homeostatic mechanisms for weight regulation");
    
    result.confidence = std::min(1.0, static_cast<double>(data.synapticWeights.size()) / 50.0);
    
    return result;
}

AnalysisResult AdvancedPerformanceAnalyzer::analyzeConnectivity(const NeuralActivityData& data) const {
    AnalysisResult result;
    result.analysisType = "connectivity";
    result.description = "Analysis of network connectivity and topology";
    
    if (data.connections.empty()) {
        result.insights.push_back("No connectivity data to analyze");
        return result;
    }
    
    size_t totalConnections = data.connections.size() * data.connections.size();
    if (totalConnections == 0) {
        return result;
    }
    
    // Compute connectivity density
    double totalWeight = 0.0;
    for (const auto& row : data.connections) {
        totalWeight += std::accumulate(row.begin(), row.end(), 0.0);
    }
    
    double density = totalWeight / totalConnections;
    result.metrics["connectivity_density"] = density;
    
    // Analyze clustering
    double clustering = 0.0;
    for (size_t i = 0; i < data.connections.size(); ++i) {
        for (size_t j = 0; j < data.connections.size(); ++j) {
            double neighborConnections = 0.0;
            for (size_t k = 0; k < data.connections.size(); ++k) {
                if (data.connections[j][k] > 0.5) {
                    neighborConnections++;
                }
            }
            
            if (neighborConnections > 1) {
                double triangleCount = 0.0;
                for (size_t l = 0; l < data.connections.size(); ++l) {
                    if (data.connections[i][l] > 0.5 && data.connections[l][k] > 0.5) {
                        triangleCount++;
                    }
                }
                clustering += triangleCount / (neighborConnections * (neighborConnections - 1));
            }
        }
    }
    
    clustering /= data.connections.size();
    result.metrics["clustering_coefficient"] = clustering;
    
    if (clustering > 0.3) {
        result.insights.push_back("High clustering - network exhibits small-world properties");
    }
    
    // Analyze modularity
    double modularity = 0.0;
    // Simplified modularity calculation
    std::map<size_t, std::vector<size_t>> communities;
    for (size_t i = 0; i < data.connections.size(); ++i) {
        communities[i] = std::vector<size_t>();
    }
    
    // Assign nodes to communities (simplified)
    for (size_t i = 0; i < data.connections.size(); ++i) {
        double maxConnection = 0.0;
        size_t community = i;
        for (size_t j = 0; j < data.connections.size(); ++j) {
            if (data.connections[i][j] > maxConnection) {
                maxConnection = data.connections[i][j];
                community = j;
            }
        }
        communities[community].push_back(i);
    }
    
    // Compute modularity score
    for (const auto& pair : communities) {
        const auto& community = pair.second;
        double communityStrength = 0.0;
        for (size_t i : community) {
            for (size_t j : community) {
                if (i != j) {
                    communityStrength += data.connections[i][j];
                }
            }
        }
        
        double totalStrength = 0.0;
        for (const auto& otherPair : communities) {
            for (size_t i : pair.second) {
                for (size_t j : otherPair.second) {
                    totalStrength += data.connections[i][j];
                }
            }
        }
        
        if (totalStrength > 0) {
            modularity += (communityStrength / totalStrength) - 
                         (community.size() * community.size()) / (data.connections.size() * data.connections.size());
        }
    }
    
    result.metrics["modularity"] = modularity;
    
    if (modularity > 0.3) {
        result.insights.push_back("High modularity - network has well-defined functional modules");
    }
    
    // Analyze shortest paths
    double avgShortestPath = 0.0;
    // Simplified BFS calculation
    for (size_t i = 0; i < data.connections.size(); ++i) {
        std::vector<double> distances(data.connections.size(), -1);
        std::queue<size_t> queue;
        queue.push(i);
        distances[i] = 0;
        
        while (!queue.empty()) {
            size_t current = queue.front();
            queue.pop();
            
            for (size_t neighbor = 0; neighbor < data.connections.size(); ++neighbor) {
                if (distances[neighbor] < 0 && data.connections[current][neighbor] > 0.5) {
                    distances[neighbor] = distances[current] + 1;
                    queue.push(neighbor);
                }
            }
        }
        
        for (size_t j = 0; j < data.connections.size(); ++j) {
            if (distances[j] >= 0) {
                avgShortestPath += distances[j];
            }
        }
    }
    
    if (data.connections.size() > 0) {
        avgShortestPath /= (data.connections.size() * data.connections.size());
        result.metrics["average_shortest_path"] = avgShortestPath;
        
        if (avgShortestPath < 3) {
            result.insights.push_back("Short path lengths - efficient information transfer");
        }
    }
    
    // Generate recommendations
    if (density < 0.1) {
        result.recommendations.push_back("Increase connectivity for better information integration");
    } else if (density > 0.8) {
        result.recommendations.push_back("Consider reducing connectivity to prevent saturation");
    }
    
    if (clustering < 0.1) {
        result.recommendations.push_back("Enhance local connectivity for specialized processing");
    }
    
    result.confidence = std::min(1.0, static_cast<double>(data.connections.size()) / 50.0);
    
    return result;
}

void AdvancedPerformanceAnalyzer::trainModel(const std::vector<NeuralActivityData>& trainingData) {
    // In a real implementation, this would train ML models
    NLM_LOG_INFO("Training ML model with " + std::to_string(trainingData.size()) + " samples");
    
    // Simplified model training
    if (!trainingData.empty()) {
        model_ = std::make_unique<MLModel>(trainingData);
    }
}

std::vector<double> AdvancedPerformanceAnalyzer::predict(const NeuralActivityData& data) const {
    if (model_) {
        return model_->predict(data);
    }
    
    NLM_LOG_WARNING("No model trained for prediction");
    return std::vector<double>();
}

double AdvancedPerformanceAnalyzer::evaluateModel(const std::vector<NeuralActivityData>& testData) const {
    if (!model_) {
        NLM_LOG_WARNING("No model to evaluate");
        return 0.0;
    }
    
    // Simplified evaluation
    return 0.85; // Placeholder accuracy
}

void AdvancedPerformanceAnalyzer::setModelPath(const std::string& path) {
    modelPath_ = path;
}

void AdvancedPerformanceAnalyzer::setHyperparameters(const nlohmann::json& params) {
    hyperparameters_ = params;
}

// MLModel implementation (simplified)
AdvancedPerformanceAnalyzer::MLModel::MLModel(const std::vector<NeuralActivityData>& trainingData) {
    // Simplified model initialization
    trainingSamples_ = trainingData;
}

std::vector<double> AdvancedPerformanceAnalyzer::MLModel::predict(const NeuralActivityData& data) const {
    // Simplified prediction - compute some basic features
    std::vector<double> features;
    
    if (!trainingSamples_.empty()) {
        // Compute similarity to training samples
        double similarity = 0.0;
        for (const auto& sample : trainingSamples_) {
            double sampleSimilarity = 0.0;
            for (size_t i = 0; i < std::min(data.neuronActivities.size(), sample.neuronActivities.size()); ++i) {
                sampleSimilarity += std::abs(data.neuronActivities[i] - sample.neuronActivities[i]);
            }
            sampleSimilarity /= std::min(data.neuronActivities.size(), sample.neuronActivities.size());
            similarity += (1.0 - sampleSimilarity); // Inverse similarity
        }
        
        similarity /= trainingSamples_.size();
        features.push_back(similarity);
    }
    
    return features;
}

// AdvancedSimulationMonitor implementation
AdvancedSimulationMonitor::AdvancedSimulationMonitor(std::shared_ptr<AgentBrain> agent)
    : agent_(agent)
    , monitoring_(false)
    , monitoringInterval_(1.0)
    , lastUpdateTime_(std::chrono::steady_clock::now()) {
    
    if (!agent_) {
        throw AgentBrainException("Cannot create AdvancedSimulationMonitor with null agent");
    }
    
    NLM_LOG_INFO("AdvancedSimulationMonitor initialized");
}

AdvancedSimulationMonitor::~AdvancedSimulationMonitor() {
    stopMonitoring();
    NLM_LOG_INFO("AdvancedSimulationMonitor destroyed");
}

void AdvancedSimulationMonitor::startMonitoring(double intervalSeconds) {
    if (monitoring_) {
        NLM_LOG_WARNING("Monitoring already started");
        return;
    }
    
    monitoringInterval_ = intervalSeconds;
    monitoring_ = true;
    monitoringThread_ = std::thread(&AdvancedSimulationMonitor::monitoringLoop, this);
    
    NLM_LOG_INFO("AdvancedSimulationMonitor started with interval: " + std::to_string(intervalSeconds) + "s");
}

void AdvancedSimulationMonitor::stopMonitoring() {
    if (!monitoring_) {
        return;
    }
    
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    NLM_LOG_INFO("AdvancedSimulationMonitor stopped");
}

bool AdvancedSimulationMonitor::isMonitoring() const {
    return monitoring_;
}

AdvancedSimulationMonitor::SystemMetrics AdvancedSimulationMonitor::getSystemMetrics() const {
    SystemMetrics metrics;
    
    // Get current system metrics
    metrics.cpuUsage = 45.0; // Placeholder - would use actual system monitoring
    metrics.memoryUsage = 512.0; // MB placeholder
    metrics.memoryPressure = 0.3; // Placeholder
    metrics.temperature = 45.0; // Celsius placeholder
    metrics.timestamp = std::chrono::steady_clock::now();
    
    // Get actual system info
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime_);
    metrics.cpuUsage = 45.0 + 20.0 * std::sin(elapsed.count() / 10.0); // Simulate variation
    
    return metrics;
}

AdvancedSimulationMonitor::BrainMetrics AdvancedSimulationMonitor::getBrainMetrics() const {
    BrainMetrics metrics;
    
    if (!agent_) {
        return metrics;
    }
    
    // Get brain metrics from agent
    auto* brain = agent_->getBrain();
    if (brain) {
        metrics.totalNeurons = brain->getTotalNeuronCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.totalSpikeCount = brain->getTotalSpikeCount();
        
        // Get neuromodulation levels
        metrics.dopamineLevel = agent_->getNeuromodulationLevel();
        metrics.curiosityLevel = agent_->getCuriosityLevel();
        metrics.noveltyLevel = agent_->getNoveltyLevel();
        metrics.predictionError = agent_->getPredictionError();
        
        // Get development metrics
        metrics.developmentProgress = agent_->getDevelopmentalStage() == DevelopmentalStage::Adult ? 1.0 : 0.5;
        
        // Get energy and health
        metrics.energyLevel = agent_->getVariable("energy_level", 1.0);
        metrics.healthLevel = agent_->getVariable("health_level", 1.0);
    }
    
    metrics.timestamp = std::chrono::steady_clock::now();
    return metrics;
}

void AdvancedSimulationMonitor::setPerformanceThresholds(const std::map<std::string, double>& thresholds) {
    alertRules_.clear();
    for (const auto& pair : thresholds) {
        addAlertRule(pair.first, "Performance threshold exceeded: " + pair.first);
    }
}

std::vector<std::string> AdvancedSimulationMonitor::checkPerformance() const {
    std::vector<std::string> alerts;
    
    // Check system metrics
    auto sysMetrics = getSystemMetrics();
    if (sysMetrics.cpuUsage > 90.0) {
        alerts.push_back("High CPU usage: " + std::to_string(sysMetrics.cpuUsage) + "%");
    }
    
    if (sysMetrics.memoryUsage > 2048.0) {
        alerts.push_back("High memory usage: " + std::to_string(sysMetrics.memoryUsage) + "MB");
    }
    
    // Check brain metrics
    auto brainMetrics = getBrainMetrics();
    if (brainMetrics.averageFiringRate > 100.0) {
        alerts.push_back("High firing rate: " + std::to_string(brainMetrics.averageFiringRate) + " Hz");
    }
    
    if (brainMetrics.predictionError > 0.5) {
        alerts.push_back("High prediction error: " + std::to_string(brainMetrics.predictionError));
    }
    
    return alerts;
}

void AdvancedSimulationMonitor::setAlertCallback(std::function<void(const std::string&, const std::string&)> callback) {
    alertCallback_ = std::move(callback);
}

void AdvancedSimulationMonitor::addAlertRule(const std::string& condition, const std::string& message) {
    alertRules_.push_back(std::make_pair(condition, message));
}

bool AdvancedSimulationMonitor::exportMetrics(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Exporting metrics to " + filename + " in format: " + format);
    
    // Simplified export
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "csv") {
        file << "Timestamp,CPU Usage,Memory Usage,Density,Prediction Error" << std::endl;
        
        auto now = std::chrono::steady_clock::now();
        file << std::chrono::duration<double>(now - startTime_).count() << ",";
        file << getSystemMetrics().cpuUsage << ",";
        file << getSystemMetrics().memoryUsage << ",";
        file << getBrainMetrics().averageFiringRate << ",";
        file << getBrainMetrics().predictionError << std::endl;
    }
    
    return true;
}

bool AdvancedSimulationMonitor::exportSystemReport(const std::string& filename) {
    NLM_LOG_INFO("Exporting system report to " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== NLM System Report ===" << std::endl;
    file << "Generated: " << getSystemMetrics().timestamp.time_since_epoch().count() << std::endl;
    file << "Uptime: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count() << " seconds" << std::endl;
    
    auto sysMetrics = getSystemMetrics();
    auto brainMetrics = getBrainMetrics();
    
    file << "\n--- System Metrics ---" << std::endl;
    file << "CPU Usage: " << sysMetrics.cpuUsage << "%" << std::endl;
    file << "Memory Usage: " << sysMetrics.memoryUsage << " MB" << std::endl;
    file << "Memory Pressure: " << sysMetrics.memoryPressure << std::endl;
    file << "Temperature: " << sysMetrics.temperature << "°C" << std::endl;
    
    file << "\n--- Brain Metrics ---" << std::endl;
    file << "Total Neurons: " << brainMetrics.totalNeurons << std::endl;
    file << "Active Neurons: " << brainMetrics.activeNeurons << std::endl;
    file << "Average Firing Rate: " << brainMetrics.averageFiringRate << " Hz" << std::endl;
    file << "Total Spikes: " << brainMetrics.totalSpikeCount << std::endl;
    file << "Average Synaptic Weight: " << brainMetrics.averageSynapticWeight << std::endl;
    file << "Plasticity Index: " << brainMetrics.plasticityIndex << std::endl;
    file << "Development Progress: " << brainMetrics.developmentProgress << std::endl;
    file << "Curiosity Level: " << brainMetrics.curiosityLevel << std::endl;
    file << "Novelty Level: " << brainMetrics.noveltyLevel << std::endl;
    file << "Prediction Error: " << brainMetrics.predictionError << std::endl;
    file << "Reward Rate: " << brainMetrics.rewardRate << std::endl;
    file << "Energy Level: " << brainMetrics.energyLevel << std::endl;
    file << "Health Level: " << brainMetrics.healthLevel << std::endl;
    
    return true;
}

void AdvancedSimulationMonitor::monitoringLoop() {
    NLM_LOG_INFO("AdvancedSimulationMonitor monitoring loop started");
    
    while (monitoring_) {
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Check performance and generate alerts
            bool hasAlerts = checkAlertConditions();
            
            // Sleep for the remaining time
            auto endTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            auto sleepTime = monitoringInterval_ - elapsed;
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error in monitoring loop: ") + e.what());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    NLM_LOG_INFO("AdvancedSimulationMonitor monitoring loop stopped");
}

bool AdvancedSimulationMonitor::checkAlertConditions() const {
    auto alerts = checkPerformance();
    
    if (!alerts.empty() && alertCallback_) {
        for (const auto& alert : alerts) {
            alertCallback_("performance", alert);
        }
        return true;
    }
    
    return false;
}

// AdvancedResultsExporter implementation
AdvancedResultsExporter::AdvancedResultsExporter() : compressionLevel_(6), includeMetadata_(true) {
    NLM_LOG_INFO("AdvancedResultsExporter initialized");
}

AdvancedResultsExporter::~AdvancedResultsExporter() {
    NLM_LOG_INFO("AdvancedResultsExporter destroyed");
}

bool AdvancedResultsExporter::exportBrainState(const std::string& filename, 
                                             std::shared_ptr<AgentBrain> agent,
                                             const ExportConfig& config) {
    NLM_LOG_INFO("Exporting brain state to " + filename);
    
    if (!agent) {
        NLM_LOG_ERROR("Cannot export null agent");
        return false;
    }
    
    // Use provided config or default
    ExportConfig effectiveConfig = config;
    if (effectiveConfig.format.empty()) {
        effectiveConfig.format = defaultFormat_;
    }
    
    // Serialize brain state
    nlohmann::json data;
    
    // Basic agent state
    data["agent_info"]["timestamp"] = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    data["agent_info"]["type"] = "AgentBrain";
    
    // Add metadata if requested
    if (includeMetadata_ || effectiveConfig.includeMetadata) {
        data["metadata"]["version"] = "1.0";
        data["metadata"]["compression"] = effectiveConfig.compression;
        data["metadata"]["created_by"] = "NLM Visualization System";
    }
    
    // Add brain state
    auto* brain = agent->getBrain();
    if (brain) {
        data["brain_state"]["total_neurons"] = brain->getTotalNeuronCount();
        data["brain_state"]["active_neurons"] = brain->getActiveNeuronCount();
        data["brain_state"]["total_spikes"] = brain->getTotalSpikeCount();
        data["brain_state"]["step_count"] = brain->getTotalStepCount();
    }
    
    // Add advanced agent features
    auto* advancedAgent = dynamic_cast<AdvancedAgentBrain*>(agent.get());
    if (advancedAgent) {
        data["advanced_features"]["has_scripting"] = true;
        data["advanced_features"]["has_commands"] = true;
        data["advanced_features"]["has_monitoring"] = true;
    }
    
    // Export based on format
    return exportToFormat(data, filename, effectiveConfig);
}

bool AdvancedResultsExporter::exportSimulationResults(const std::string& filename,
                                                     const std::vector<SimulationMetrics>& metrics,
                                                     const std::vector<NeuralActivityData>& neuralData,
                                                     const ExportConfig& config) {
    NLM_LOG_INFO("Exporting simulation results to " + filename);
    
    nlohmann::json data;
    
    // Add metadata
    data["type"] = "simulation_results";
    data["metrics_count"] = static_cast<int>(metrics.size());
    data["neural_data_points"] = static_cast<int>(neuralData.size());
    
    // Serialize metrics
    nlohmann::json metricsArray = nlohmann::json::array();
    for (const auto& m : metrics) {
        nlohmann::json metric;
        metric["elapsed_time"] = m.elapsedTime;
        metric["steps_completed"] = m.stepsCompleted;
        metric["average_firing_rate"] = m.averageFiringRate;
        metric["total_reward"] = m.totalReward;
        metric["prediction_error"] = m.predictionError;
        metric["curiosity_level"] = m.curiosityLevel;
        metric["novelty_level"] = m.noveltyLevel;
        metric["dopamine_level"] = m.dopamineLevel;
        metric["energy_level"] = m.energyLevel;
        metric["health_level"] = m.healthLevel;
        metric["cpu_usage"] = m.cpuUsage;
        metric["memory_usage"] = m.memoryUsage;
        
        metricsArray.push_back(metric);
    }
    data["metrics"] = metricsArray;
    
    // Serialize neural data
    nlohmann::json neuralArray = nlohmann::json::array();
    for (const auto& d : neuralData) {
        nlohmann::json neuralDataJson;
        neuralDataJson["timestamp"] = d.timestamp;
        neuralDataJson["step_number"] = static_cast<int>(d.stepNumber);
        neuralDataJson["neuron_activities"] = d.neuronActivities;
        neuralDataJson["synaptic_weights"] = d.synapticWeights;
        
        // Serialize connections (simplified)
        nlohmann::json connectionsArray = nlohmann::json::array();
        for (const auto& row : d.connections) {
            nlohmann::json connectionRow = nlohmann::json::array();
            for (float weight : row) {
                connectionRow.push_back(weight);
            }
            connectionsArray.push_back(connectionRow);
        }
        neuralDataJson["connections"] = connectionsArray;
        
        neuralArray.push_back(neuralDataJson);
    }
    data["neural_data"] = neuralArray;
    
    return exportToFormat(data, filename, config);
}

bool AdvancedResultsExporter::exportAnalysisResults(const std::string& filename,
                                                  const std::vector<AnalysisResult>& results,
                                                  const ExportConfig& config) {
    NLM_LOG_INFO("Exporting analysis results to " + filename);
    
    nlohmann::json data;
    data["type"] = "analysis_results";
    data["results_count"] = static_cast<int>(results.size());
    
    // Serialize results
    nlohmann::json resultsArray = nlohmann::json::array();
    for (const auto& r : results) {
        nlohmann::json resultJson;
        resultJson["analysis_type"] = r.analysisType;
        resultJson["description"] = r.description;
        resultJson["confidence"] = r.confidence;
        
        // Convert metrics map to JSON
        nlohmann::json metricsJson = nlohmann::json::object();
        for (const auto& pair : r.metrics) {
            metricsJson[pair.first] = pair.second;
        }
        resultJson["metrics"] = metricsJson;
        
        // Convert time series map to JSON
        nlohmann::json timeSeriesJson = nlohmann::json::object();
        for (const auto& pair : r.timeSeries) {
            nlohmann::json timeSeriesArray = nlohmann::json::array();
            for (double val : pair.second) {
                timeSeriesArray.push_back(val);
            }
            timeSeriesJson[pair.first] = timeSeriesArray;
        }
        resultJson["time_series"] = timeSeriesJson;
        
        // Convert insights and recommendations
        resultJson["insights"] = r.insights;
        resultJson["recommendations"] = r.recommendations;
        
        resultsArray.push_back(resultJson);
    }
    data["results"] = resultsArray;
    
    return exportToFormat(data, filename, config);
}

bool AdvancedResultsExporter::exportCompleteReport(const std::string& filename,
                                                 std::shared_ptr<AgentBrain> agent,
                                                 const std::vector<AnalysisResult>& analyses,
                                                 const ExportConfig& config) {
    NLM_LOG_INFO("Exporting complete report to " + filename);
    
    nlohmann::json data;
    data["type"] = "complete_report";
    data["timestamp"] = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Add agent info
    if (agent) {
        nlohmann::json agentInfo;
        agentInfo["type"] = "AgentBrain";
        
        auto* brain = agent->getBrain();
        if (brain) {
            agentInfo["brain_state"]["total_neurons"] = brain->getTotalNeuronCount();
            agentInfo["brain_state"]["total_spikes"] = brain->getTotalSpikeCount();
        }
        
        data["agent"] = agentInfo;
    }
    
    // Add analyses
    data["analyses"] = analyses; // Already JSON-serializable
    
    // Add system info
    data["system"] = {
        {"platform", "NLM"},
        {"version", "1.0"},
        {"export_format", config.format}
    };
    
    return exportToFormat(data, filename, config);
}

bool AdvancedResultsExporter::exportToFormat(const nlohmann::json& data, const std::string& filename,
                                           const ExportConfig& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for export: " + filename);
        return false;
    }
    
    try {
        if (config.format == "json") {
            if (config.compression == "gzip") {
                // In a real implementation, would compress the JSON
                file << data.dump(2);
            } else {
                file << data.dump(2);
            }
        } else if (config.format == "csv") {
            // Simplified CSV export - would convert to tabular format
            file << "Placeholder CSV export for " << filename << std::endl;
        } else if (config.format == "parquet") {
            file << "Placeholder Parquet export for " << filename << std::endl;
        } else if (config.format == "protobuf") {
            file << "Placeholder Protobuf export for " << filename << std::endl;
        } else {
            // Default to JSON
            file << data.dump(2);
        }
        
        file.flush();
        
        NLM_LOG_INFO("Successfully exported data to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Export failed: ") + e.what());
        return false;
    }
}

void AdvancedResultsExporter::setDefaultExportFormat(const std::string& format) {
    defaultFormat_ = format;
}

void AdvancedResultsExporter::setCompressionLevel(int level) {
    compressionLevel_ = std::max(0, std::min(9, level));
}

void AdvancedResultsExporter::setIncludeMetadata(bool include) {
    includeMetadata_ = include;
}

void AdvancedResultsExporter::setPostProcessor(std::function<nlohmann::json(const nlohmann::json&)> processor) {
    postProcessor_ = std::move(processor);
}

// VisualizationSystem implementation
VisualizationSystem::VisualizationSystem(std::shared_ptr<AgentBrain> agent)
    : agent_(agent)
    , running_(false)
    , startTime_(std::chrono::steady_clock::now()) {
    
    if (!agent_) {
        throw AgentBrainException("Cannot create VisualizationSystem with null agent");
    }
    
    NLM_LOG_INFO("VisualizationSystem initialized");
    
    // Initialize components
    if (!initializeComponents()) {
        throw AgentBrainException("Failed to initialize visualization system components");
    }
}

VisualizationSystem::~VisualizationSystem() {
    shutdown();
    NLM_LOG_INFO("VisualizationSystem destroyed");
}

bool VisualizationSystem::initialize(const VisualizationConfig& config) {
    visConfig_ = config;
    exportConfig_.format = "json"; // Default export format
    exportConfig_.compression = "gzip";
    exportConfig_.includeMetadata = true;
    
    // Update component configurations
    if (brainVisualizer_) {
        // brainVisualizer_->updateConfig(config);
    }
    
    if (dataPlotter_) {
        // dataPlotter_->updateConfig(config);
    }
    
    NLM_LOG_INFO("VisualizationSystem configured successfully");
    return true;
}

void VisualizationSystem::shutdown() {
    if (!running_) {
        return;
    }
    
    stopAll();
    
    // Shutdown components
    if (brainVisualizer_) {
        // brainVisualizer_->shutdown();
    }
    
    if (dataPlotter_) {
        // dataPlotter_->shutdown();
    }
    
    if (simulationMonitor_) {
        // simulationMonitor_->shutdown();
    }
    
    if (performanceAnalyzer_) {
        // performanceAnalyzer_->shutdown();
    }
    
    if (resultExporter_) {
        // resultExporter_->shutdown();
    }
    
    NLM_LOG_INFO("VisualizationSystem shutdown completed");
}

void VisualizationSystem::startAll() {
    if (running_) {
        return;
    }
    
    running_ = true;
    
    // Start components
    if (brainVisualizer_) {
        // brainVisualizer_->start();
    }
    
    if (dataPlotter_) {
        // dataPlotter_->start();
    }
    
    if (simulationMonitor_) {
        // simulationMonitor_->startMonitoring(1.0); // Default 1 second interval
    }
    
    NLM_LOG_INFO("VisualizationSystem started");
}

void VisualizationSystem::stopAll() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Stop components
    if (brainVisualizer_) {
        // brainVisualizer_->stop();
    }
    
    if (dataPlotter_) {
        // dataPlotter_->stop();
    }
    
    if (simulationMonitor_) {
        // simulationMonitor_->stopMonitoring();
    }
    
    NLM_LOG_INFO("VisualizationSystem stopped");
}

bool VisualizationSystem::isRunning() const {
    return running_;
}

void VisualizationSystem::updateVisualizationConfig(const VisualizationConfig& config) {
    visConfig_ = config;
    
    // Update component configurations
    if (brainVisualizer_) {
        // brainVisualizer_->updateConfig(config);
    }
}

void VisualizationSystem::updateExportConfig(const ExportConfig& config) {
    exportConfig_ = config;
    
    // Update exporter configuration
    if (resultExporter_) {
        // resultExporter_->updateConfig(config);
    }
}

void VisualizationSystem::pauseVisualization(bool pause) {
    // Implement pause functionality
    NLM_LOG_INFO("Visualization system " + std::string(pause ? "paused" : "resumed"));
}

void VisualizationSystem::captureScreenshot(const std::string& filename) {
    if (brainVisualizer_) {
        // brainVisualizer_->saveScreenshot(filename);
    }
    
    NLM_LOG_INFO("Screenshot captured: " + filename);
}

void VisualizationSystem::startRecording(const std::string& filename) {
    // Implement recording functionality
    NLM_LOG_INFO("Recording started: " + filename);
}

void VisualizationSystem::stopRecording() {
    // Implement stop recording functionality
    NLM_LOG_INFO("Recording stopped");
}

std::map<std::string, std::string> VisualizationSystem::getSystemStatus() const {
    std::map<std::string, std::string> status;
    
    status["running"] = running_ ? "yes" : "no";
    status["uptime"] = std::to_string(std::chrono::duration<double>(
        std::chrono::steady_clock::now() - startTime_).count()) + " seconds";
    
    status["components"] = std::to_string(components_.size());
    
    return status;
}

std::vector<std::string> VisualizationSystem::getActiveComponents() const {
    std::vector<std::string> components;
    
    if (brainVisualizer_) components_.push_back("brain_visualizer");
    if (dataPlotter_) components_.push_back("data_plotter");
    if (simulationMonitor_) components_.push_back("simulation_monitor");
    if (performanceAnalyzer_) components_.push_back("performance_analyzer");
    if (resultExporter_) components_.push_back("result_exporter");
    
    return components_;
}

double VisualizationSystem::getSystemUptime() const {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration<double>(now - startTime_).count();
    return uptime;
}

bool VisualizationSystem::initializeComponents() {
    // Initialize brain visualizer
    if (agent_) {
        brainVisualizer_ = std::make_shared<BrainVisualizer>(agent_);
    if (brainVisualizer_ && !brainVisualizer_->initialize(visConfig_)) {
        NLM_LOG_ERROR("Failed to initialize brain visualizer");
        return false;
    }
    
    // Initialize data plotter
    dataPlotter_ = std::make_unique<AdvancedDataPlotter>();
    if (dataPlotter_ && !dataPlotter_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize data plotter");
        return false;
    }
    
    // Initialize simulation monitor
    simulationMonitor_ = std::make_shared<AdvancedSimulationMonitor>(agent_);
    if (simulationMonitor_ && !simulationMonitor_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize simulation monitor");
        return false;
    }
    
    // Initialize performance analyzer
    performanceAnalyzer_ = std::make_unique<AdvancedPerformanceAnalyzer>();
    if (performanceAnalyzer_ && !performanceAnalyzer_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize performance analyzer");
        return false;
    }
    
    // Initialize result exporter
    resultExporter_ = std::make_unique<AdvancedResultsExporter>();
    if (resultExporter_ && !resultExporter_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize result exporter");
        return false;
    }
    
    NLM_LOG_INFO("All visualization system components initialized");
    return true;
}
}

void VisualizationSystem::shutdownComponents() {
    // Shutdown brain visualizer
    if (brainVisualizer_) {
        // brainVisualizer_->shutdown();
    }
    
    // Shutdown data plotter
    if (dataPlotter_) {
        // dataPlotter_->shutdown();
    }
    
    // Shutdown simulation monitor
    if (simulationMonitor_) {
        // simulationMonitor_->stopMonitoring();
    }
    
    // Shutdown performance analyzer
    if (performanceAnalyzer_) {
        // performanceAnalyzer_->shutdown();
    }
    
    // Shutdown result exporter
    if (resultExporter_) {
        // resultExporter_->shutdown();
    }
    
    NLM_LOG_INFO("All visualization system components shutdown");
}
} // namespace nlm

AdvancedPerformanceAnalyzer::MLModel::MLModel::MLModel(const std::vector<NeuralActivityData>& trainingData) {
    trainingSamples_ = trainingData;
}

std::vector<double> AdvancedPerformanceAnalyzer::MLModel::predict(const NeuralActivityData& data) const {
    std::vector<double> predictions;
    
    if (!trainingSamples_.empty()) {
        NLM_LOG_WARNING("Model not trained, cannot make predictions");
        return std::vector<double>();
    }
    
    std::vector<double> AdvancedPerformanceAnalyzer::MLModel::predict(const NeuralActivityData& data) const {
        return model_->predict(data);
    }
    
    double AdvancedPerformanceAnalyzer::evaluateModel(const std::vector<NeuralActivityData>& testData) const {
        if (!model_) {
        NLM_LOG_WARNING("Model not trained, cannot evaluate predictions");
        return 0.0;
    }
    
    double AdvancedPerformanceAnalyzer::evaluateModel(const std::vector<NeuralActivityData>& testData) const {
        double accuracy = 0.0;
        
        for (const sample : testData) {
            double sampleAccuracy = model_->predict(sample);
            accuracy += sampleAccuracy;
        }
        
        if (!testData.empty()) {
            return 0.0;
        }
    
    double accuracy = accuracy / testData.size();
    return accuracy;
    }
}
} // namespace nlm

bool Config::loadFromJSON(const std::string& content, ConfigSource source) {
    try {
        auto json = nlohmann::json::parse(content);
        
        // Convert JSON to configuration
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            auto configValue = jsonToConfigValue(it.value());
            
            // Check for existing key
            auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (existingIt != pImpl->entries.end()) {
                // Add new entry
                ConfigEntry newEntry;
                newEntry.key = key;
                newEntry.value = configValue;
                newEntry.source = source;
                newEntry.description = "Loaded from JSON configuration";
                
                pImpl->entries.push_back(newEntry);
            } else {
                // Update existing entry
                auto existingIt = pImpl->entries.begin();
                std::advance(existingIt, existingIt + pImpl->entries.end(),
                    [&key](ConfigEntry& e) { return e.key == key; });
                existingIt->value = configValue;
                existingIt->source = source;
            }
        }
        
        } catch (const nlohmann::json::exception& e) {
            NLM_LOG_ERROR(std::string("JSON parsing failed: ") + e.what());
            return false;
        }
    }
}

bool Config::isJSONFile(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == "json";
}

bool Config::isYAMLError(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == "yaml" || extension == "yml";
}

// Config.cpp implementations (simplified)
namespace nlm {

// Template implementations for Config class
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Additional implementations for visualization system
namespace nlm {

// Demonstration function to show the complete system
void demonstrateVisualizationSystem() {
    // Create a mock agent
    auto config = std::make_shared<Config>();
    
    // Initialize configuration
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 500);
    
    // Create advanced agent brain
    auto agentBrain = std::make_shared<AgentBrain>(config);
    
    // Create visualization system
    VisualizationSystem vizSystem(agentBrain);
    
    // Configure visualization
    VisualizationConfig visConfig;
    visConfig.backend = "opengl";
    visConfig.theme = "neural";
    visConfig.realTimeUpdates = true;
    visConfig.updateIntervalMs = 100;
    
    // Initialize visualization
    if (vizSystem.initialize(visConfig)) {
        std::cout << "Visualization system initialized successfully!" << std::endl;
        
        // Start monitoring and visualization
        vizSystem.startAll();
        
        std::cout << "Simulation started. Press Ctrl+C to stop." << std::endl;
        
        // Simulate for a few seconds
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Capture a screenshot
        vizSystem.captureScreenshot("demo_screenshot.png");
        
        // Stop visualization
        vizSystem.stopAll();
        
        std::cout << "Demo completed successfully!" << std::endl;
        
    } else {
        std::cerr << "Failed to initialize visualization system!" << std::endl;
    }
}

} // namespace nlm

int main() {
    std::cout << "NLM Visualization System Demo" << std::endl;
    std::cout << "=" << std::string(30, '=') << std::endl;
    
    try {
        demonstrateVisualizationSystem();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
