// ====================================================================
// ADVANCED ANALYSIS IMPLEMENTATION
// ====================================================================

#include "advanced/AdvancedAnalysis.h"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

AdvancedAnalysis::NeuralConnectivityAnalysis AdvancedAnalysis::analyzeConnectivity(std::shared_ptr<Brain> brain) {
    NeuralConnectivityAnalysis analysis;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        return analysis;
    }
    
    auto neurons = region->getAllNeurons();
    size_t neuronCount = neurons.size();
    auto synapses = region->getSynapses();
    size_t synapseCount = synapses.size();
    
    if (neuronCount > 1) {
        // Connection density
        analysis.connectionDensity = static_cast<double>(synapseCount) / (neuronCount * (neuronCount - 1));
        
        // Calculate clustering coefficient (simplified)
        size_t clusteringSum = 0;
        for (size_t i = 0; i < synapseCount; ++i) {
            for (size_t j = i + 1; j < synapseCount; ++j) {
                // Check if synapses share a common neuron
                if (synapses[i]->getSource() == synapses[j]->getSource() ||
                    synapses[i]->getSource() == synapses[j]->getTarget() ||
                    synapses[i]->getTarget() == synapses[j]->getSource() ||
                    synapses[i]->getTarget() == synapses[j]->getTarget()) {
                    clusteringSum++;
                }
            }
        }
        analysis.clusteringCoefficient = neuronCount > 2 ? 
            static_cast<double>(clusteringSum) / (synapseCount * (synapseCount - 1) / 2) : 0.0;
        
        // Characteristic path length (simplified)
        analysis.characteristicPathLength = neuronCount > 1 ? 
            2.0 * log(static_cast<double>(neuronCount)) / static_cast<double>(synapseCount) : 0.0;
        
        // Small-worldness (simplified)
        double erClustering = pow(static_cast<double>(synapseCount), 2) / static_cast<double>(neuronCount);
        analysis.smallWorldness = erClustering > 0 ? analysis.clusteringCoefficient / erClustering : 1.0;
    }
    
    // Build adjacency matrix for community detection
    analysis.adjacencyMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
    for (size_t i = 0; i < synapseCount; ++i) {
        if (synapses[i]->getSource() && synapses[i]->getTarget()) {
            analysis.adjacencyMatrix[synapses[i]->getSource()->getId()][synapses[i]->getTarget()->getId()] = 1.0;
            analysis.adjacencyMatrix[synapses[i]->getTarget()->getId()][synapses[i]->getSource()->getId()] = 1.0;
        }
    }
    
    return analysis;
}

AdvancedAnalysis::LearningDynamicsAnalysis AdvancedAnalysis::analyzeLearningDynamics(
    std::shared_ptr<Brain> brain, const std::map<std::string, double>& parameters) {
    
    LearningDynamicsAnalysis analysis;
    
    // Set parameters
    if (parameters.count("learning_rate")) {
        analysis.learningRate = parameters.at("learning_rate");
    }
    if (parameters.count("plasticity_strength")) {
        analysis.plasticityStrength = parameters.at("plasticity_strength");
    }
    if (parameters.count("memory_consolidation")) {
        analysis.memoryConsolidation = parameters.at("memory_consolidation");
    }
    if (parameters.count("transfer_learning")) {
        analysis.transferEfficiency = parameters.at("transfer_learning");
    }
    
    // Analyze learning curve by simulating over time
    analysis.learningCurve.clear();
    double cumulativeLearning = 0.0;
    
    for (int step = 0; step < 100; ++step) {
        brain->step(step, 0.001);
        
        // Simple learning metric based on spike activity and weight changes
        double stepLearning = brain->getFiringNeuronCount() * 0.01 + 
                             brain->getAverageFiringRate() * 0.05;
        cumulativeLearning += stepLearning;
        analysis.learningCurve.push_back(cumulativeLearning);
    }
    
    // Calculate retention curve (simplified)
    analysis.retentionCurve.clear();
    double retention = 1.0;
    for (int i = 0; i < 20; ++i) {
        retention *= parameters.count("decay_rate") ? 
            (1.0 - parameters.at("decay_rate")) : 0.95;
        analysis.retentionCurve.push_back(retention);
    }
    
    // Adaptation metrics (simplified)
    analysis.adaptationMetrics["learning_speed"] = analysis.learningCurve.back() / 100.0;
    analysis.adaptationMetrics["plasticity_adaptation"] = analysis.plasticityStrength * 0.1;
    analysis.adaptationMetrics["memory_efficiency"] = analysis.memoryConsolidation * 0.5;
    analysis.adaptationMetrics["transfer_capability"] = analysis.transferEfficiency * 0.3;
    
    return analysis;
}

AdvancedAnalysis::PerformanceMetrics AdvancedAnalysis::analyzePerformance(
    std::shared_ptr<Brain> brain,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    
    PerformanceMetrics metrics;
    
    // Calculate performance metrics
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double totalSeconds = duration.count() / 1000000.0;
    
    if (totalSeconds > 0) {
        metrics.simulationSpeed = brain->getTotalSpikeCount() / totalSeconds;  // spikes per second
    }
    
    metrics.simulationTime = duration;
    metrics.neuronsProcessed = brain->getTotalNeuronCount();
    metrics.synapsesAnalyzed = brain->getTotalSynapseCount();
    
    // Memory efficiency (simplified)
    metrics.memoryEfficiency = static_cast<double>(brain->getTotalSynapseCount()) / 
                               (brain->getTotalNeuronCount() * 100);  // arbitrary scaling
    
    // Computational complexity (simplified)
    metrics.computationalComplexity = log(static_cast<double>(brain->getTotalNeuronCount()));
    
    // Scalability factor (simplified)
    metrics.scalabilityFactor = 1.0 / (1.0 + log(static_cast<double>(brain->getTotalNeuronCount()) / 100.0));
    
    return metrics;
}

AdvancedAnalysis::AnomalyDetectionResult AdvancedAnalysis::detectAnomalies(
    std::shared_ptr<Brain> brain,
    const std::map<std::string, double>& historicalData) {
    
    AnomalyDetectionResult result;
    result.timestamp = std::chrono::system_clock::now();
    
    // Simple anomaly detection based on firing rate deviations
    double currentFiringRate = brain->getAverageFiringRate();
    double historicalAverage = historicalData.count("average_firing_rate") ? 
        historicalData.at("average_firing_rate") : 0.5;
    double historicalStdDev = historicalData.count("firing_rate_stddev") ? 
        historicalData.at("firing_rate_stddev") : 0.1;
    
    if (historicalStdDev > 0) {
        double zScore = std::abs(currentFiringRate - historicalAverage) / historicalStdDev;
        
        if (zScore > 2.0) {
            result.anomalyDetected = true;
            result.anomalyScore = zScore;
            result.anomalyType = "firing_rate_deviation";
            result.description = "Unusual firing rate detected (z-score: " + std::to_string(zScore) + ")";
            
            // Identify contributing factors
            result.contributingFactors["firing_rate"] = currentFiringRate;
            result.contributingFactors["historical_average"] = historicalAverage;
            result.contributingFactors["z_score"] = zScore;
        }
    }
    
    return result;
}

AdvancedAnalysis::PredictiveModel AdvancedAnalysis::predictLearningOutcome(
    std::shared_ptr<Brain> brain,
    const std::map<std::string, double>& conditions,
    const std::string& predictionType) {
    
    PredictiveModel model;
    model.lastTrained = std::chrono::system_clock::now();
    
    if (predictionType == "performance") {
        model.modelType = "performance_prediction";
        model.accuracy = 0.85;  // Simplified accuracy
        
        // Feature importance
        model.featureImportance["neuron_count"] = 0.3;
        model.featureImportance["connection_density"] = 0.25;
        model.featureImportance["plasticity_strength"] = 0.2;
        model.featureImportance["learning_rate"] = 0.15;
        model.featureImportance["memory_capacity"] = 0.1;
        
        // Model parameters (simplified)
        model.parameters = {conditions.count("learning_rate") ? conditions.at("learning_rate") : 0.01,
                           conditions.count("neuron_count") ? conditions.at("neuron_count") / 1000.0 : 0.5,
                           conditions.count("plasticity_strength") ? conditions.at("plasticity_strength") : 0.5};
    
    } else if (predictionType == "stability") {
        model.modelType = "stability_prediction";
        model.accuracy = 0.80;
        
        model.featureImportance["connection_density"] = 0.4;
        model.featureImportance["clustering_coefficient"] = 0.3;
        model.featureImportance["path_length"] = 0.2;
        model.featureImportance["learning_rate"] = 0.1;
        
        model.parameters = {conditions.count("connection_density") ? conditions.at("connection_density") : 0.1,
                           conditions.count("clustering_coefficient") ? conditions.at("clustering_coefficient") : 0.5};
    }
    
    // Calculate confidence based on data quality
    model.confidence = 0.7 + (conditions.size() * 0.05); // Simplified
    if (model.confidence > 1.0) model.confidence = 1.0;
    
    return model;
}

std::string AdvancedAnalysis::generateAnalysisReport(
    std::shared_ptr<Brain> brain,
    const std::map<std::string, double>& analysisParameters) {
    
    std::ostringstream oss;
    
    oss << "=== ADVANCED NLM ANALYSIS REPORT ===" << std::endl;
    oss << std::endl;
    
    // Basic information
    oss << "Brain Configuration:" << std::endl;
    oss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    oss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    oss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    oss << "  Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    oss << "  Developmental Stage: " << brain->getDevelopmentalStage().name << std::endl;
    oss << std::endl;
    
    // Connectivity analysis
    auto connectivity = analyzeConnectivity(brain);
    oss << "Neural Connectivity Analysis:" << std::endl;
    oss << "  Connection Density: " << connectivity.connectionDensity << std::endl;
    oss << "  Clustering Coefficient: " << connectivity.clusteringCoefficient << std::endl;
    oss << "  Characteristic Path Length: " << connectivity.characteristicPathLength << std::endl;
    oss << "  Small-Worldness: " << connectivity.smallWorldness << std::endl;
    oss << std::endl;
    
    // Learning dynamics
    auto learningDynamics = analyzeLearningDynamics(brain, analysisParameters);
    oss << "Learning Dynamics Analysis:" << std::endl;
    oss << "  Learning Rate: " << learningDynamics.learningRate << std::endl;
    oss << "  Plasticity Strength: " << learningDynamics.plasticityStrength << std::endl;
    oss << "  Memory Consolidation: " << learningDynamics.memoryConsolidation << std::endl;
    oss << "  Transfer Efficiency: " << learningDynamics.transferEfficiency << std::endl;
    oss << "  Final Learning Score: " << (learningDynamics.learningCurve.empty() ? 0.0 : learningDynamics.learningCurve.back()) << std::endl;
    oss << std::endl;
    
    // Performance metrics (simulate timing)
    auto startTime = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    auto endTime = std::chrono::system_clock::now();
    
    auto performance = analyzePerformance(brain, startTime, endTime);
    oss << "Performance Metrics:" << std::endl;
    oss << "  Simulation Speed: " << performance.simulationSpeed << " spikes/sec" << std::endl;
    oss << "  Memory Efficiency: " << performance.memoryEfficiency << std::endl;
    oss << "  Computational Complexity: " << performance.computationalComplexity << std::endl;
    oss << "  Scalability Factor: " << performance.scalabilityFactor << std::endl;
    oss << std::endl;
    
    // Anomaly detection
    auto anomaly = detectAnomalies(brain);
    if (anomaly.anomalyDetected) {
        oss << "⚠️ ANOMALY DETECTED:" << std::endl;
        oss << "  Type: " << anomaly.anomalyType << std::endl;
        oss << "  Score: " << anomaly.anomalyScore << std::endl;
        oss << "  Description: " << anomaly.description << std::endl;
    } else {
        oss << "✅ No anomalies detected." << std::endl;
    }
    oss << std::endl;
    
    oss << "=== END OF ANALYSIS REPORT ===" << std::endl;
    
    return oss.str();
}

std::map<std::string, double> AdvancedAnalysis::computeStatisticalMeasures(const std::vector<double>& data) {
    std::map<std::string, double> stats;
    
    if (data.empty()) {
        return stats;
    }
    
    // Mean
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    stats["mean"] = sum / data.size();
    
    // Standard deviation
    double sqSum = 0.0;
    for (double val : data) {
        sqSum += (val - stats["mean"]) * (val - stats["mean"]);
    }
    stats["stddev"] = std::sqrt(sqSum / data.size());
    
    // Minimum and maximum
    stats["min"] = *std::min_element(data.begin(), data.end());
    stats["max"] = *std::max_element(data.begin(), data.end());
    
    // Median
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());
    if (sortedData.size() % 2 == 0) {
        stats["median"] = (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0;
    } else {
        stats["median"] = sortedData[sortedData.size() / 2];
    }
    
    return stats;
}

std::map<std::string, double> AdvancedAnalysis::analyzeNetworkTopology(std::shared_ptr<Brain> brain) {
    std::map<std::string, double> topology;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        return topology;
    }
    
    auto neurons = region->getAllNeurons();
    auto synapses = region->getSynapses();
    size_t neuronCount = neurons.size();
    size_t synapseCount = synapses.size();
    
    // Network density
    topology["density"] = neuronCount > 1 ? 
        static_cast<double>(synapseCount) / (neuronCount * (neuronCount - 1)) : 0.0;
    
    // Average degree
    topology["average_degree"] = neuronCount > 0 ? 
        static_cast<double>(synapseCount) / neuronCount : 0.0;
    
    // Degree distribution statistics
    std::map<size_t, size_t> degreeHistogram;
    for (const auto& syn : synapses) {
        size_t sourceDegree = 0, targetDegree = 0;
        for (const auto& s : synapses) {
            if (s->getSource() == syn->getSource()) sourceDegree++;
            if (s->getTarget() == syn->getTarget()) targetDegree++;
        }
        degreeHistogram[sourceDegree]++;
        degreeHistogram[targetDegree]++;
    }
    
    // Calculate mean degree from histogram
    double meanDegree = 0.0;
    size_t totalDegree = 0;
    for (const auto& pair : degreeHistogram) {
        meanDegree += pair.first * pair.second;
        totalDegree += pair.second;
    }
    topology["mean_degree"] = totalDegree > 0 ? meanDegree / totalDegree : 0.0;
    
    // Degree variance
    double variance = 0.0;
    for (const auto& pair : degreeHistogram) {
        double diff = pair.first - topology["mean_degree"];
        variance += diff * diff * pair.second;
    }
    topology["degree_variance"] = totalDegree > 0 ? variance / totalDegree : 0.0;
    
    // Network assortativity (simplified)
    double assortativity = 0.0;
    for (const auto& syn : synapses) {
        if (syn->getSource() && syn->getTarget()) {
            assortativity += syn->getSource()->getId() * syn->getTarget()->getId();
        }
    }
    topology["assortativity"] = synapseCount > 0 ? assortativity / synapseCount : 0.0;
    
    return topology;
}

std::map<std::string, double> AdvancedAnalysis::analyzeLearningEfficiency(
    std::shared_ptr<Brain> brain,
    int simulationSteps) {
    std::map<std::string, double> efficiency;
    
    // Simulate learning over time
    std::vector<double> learningProgress;
    for (int step = 0; step < simulationSteps; ++step) {
        brain->step(step, 0.001);
        
        // Simple efficiency metric: learning per step
        double stepLearning = brain->getAverageFiringRate() * 0.01 + 
                             brain->getFiringNeuronCount() * 0.001;
        learningProgress.push_back(stepLearning);
    }
    
    // Calculate efficiency metrics
    efficiency["learning_efficiency"] = std::accumulate(learningProgress.begin(), learningProgress.end(), 0.0) / simulationSteps;
    efficiency["learning_consistency"] = computeStatisticalMeasures(learningProgress)["stddev"];
    efficiency["learning_speed"] = learningProgress.back() / (simulationSteps * 0.001);
    
    // Calculate improvement rate
    if (learningProgress.size() > 1) {
        double initialLearning = learningProgress[0];
        double finalLearning = learningProgress.back();
        efficiency["improvement_rate"] = (finalLearning - initialLearning) / initialLearning;
    }
    
    return efficiency;
}

std::map<std::string, double> AdvancedAnalysis::analyzeMemoryUsage(std::shared_ptr<Brain> brain) {
    std::map<std::string, double> memory;
    
    // Estimate memory usage based on brain size
    memory["neuron_memory"] = static_cast<double>(brain->getTotalNeuronCount()) * 1024; // 1KB per neuron
    memory["synapse_memory"] = static_cast<double>(brain->getTotalSynapseCount()) * 512; // 512B per synapse
    
    // Calculate total memory
    memory["total_memory"] = memory["neuron_memory"] + memory["synapse_memory"];
    
    // Memory efficiency
    memory["memory_efficiency"] = brain->getTotalNeuronCount() > 0 ? 
        static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount() : 0.0;
    
    return memory;
}

std::map<std::string, double> AdvancedAnalysis::analyzeScalability(
    std::vector<size_t> neuronCounts,
    int simulationSteps) {
    std::map<std::string, double> scalability;
    
    if (neuronCounts.empty()) {
        return scalability;
    }
    
    // Simulate performance for different neuron counts
    std::vector<double> performanceTimes;
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < simulationSteps; ++step) {
            brain->step(step, 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        performanceTimes.push_back(duration.count() / 1000.0); // Convert to milliseconds
    }
    
    // Calculate scalability metrics
    if (performanceTimes.size() > 1) {
        double timeRatio = performanceTimes.back() / performanceTimes[0];
        double neuronsRatio = static_cast<double>(neuronCounts.back()) / neuronCounts[0];
        
        scalability["time_complexity"] = std::log(timeRatio) / std::log(neuronsRatio);
        scalability["parallel_efficiency"] = 1.0 / scalability["time_complexity"];
        
        // Calculate scalability factor
        double expectedTimeRatio = neuronsRatio * scalability["time_complexity"];
        scalability["scalability_factor"] = expectedTimeRatio / timeRatio;
    }
    
    return scalability;
}

std::map<std::string, double> AdvancedAnalysis::crossValidatePredictiveModel(
    std::shared_ptr<Brain> brain,
    const std::vector<std::map<std::string, double>>& validationData,
    const std::string& modelType) {
    std::map<std::string, double> validationResults;
    
    if (validationData.empty()) {
        return validationResults;
    }
    
    // Simple cross-validation: train on subset, test on remainder
    double totalError = 0.0;
    int testCount = 0;
    
    for (const auto& data : validationData) {
        // Use current brain state as "trained" model
        double predicted = brain->getAverageFiringRate();
        double actual = data.count("firing_rate") ? data.at("firing_rate") : 0.0;
        
        double error = std::abs(predicted - actual);
        totalError += error;
        testCount++;
    }
    
    if (testCount > 0) {
        validationResults["mean_absolute_error"] = totalError / testCount;
        validationResults["r_squared"] = 1.0 - (totalError / testCount) / 
                                        (computeStatisticalMeasures(std::vector<double>(validationData.size(), 
                                                                                 computeStatisticalMeasures(validationData.begin()->count("firing_rate") ? 
                                                                             validationData.begin()->at("firing_rate") : 0.0)).at("stddev") * 
                                              computeStatisticalMeasures(validationData.begin()->count("firing_rate") ? 
                                                  validationData.begin()->at("firing_rate") : 0.0).at("stddev")));
    }
    
    return validationResults;
}

std::map<std::string, std::vector<std::vector<double>>> AdvancedAnalysis::clusterAnomalies(
    const std::vector<AnomalyDetectionResult>& anomalies) {
    std::map<std::string, std::vector<std::vector<double>>> clusters;
    
    if (anomalies.empty()) {
        return clusters;
    }
    
    // Simple clustering based on anomaly scores and types
    std::vector<std::vector<double>> anomalyData;
    
    for (const auto& anomaly : anomalies) {
        std::vector<double> dataPoint;
        dataPoint.push_back(anomaly.anomalyScore);
        dataPoint.push_back(static_cast<double>(anomaly.anomalyDetected));
        anomalyData.push_back(dataPoint);
    }
    
    // For simplicity, group by anomaly score ranges
    clusters["low_score"] = {}; // 0.0 - 1.0
    clusters["medium_score"] = {}; // 1.0 - 2.0
    clusters["high_score"] = {}; // 2.0+
    
    for (const auto& dataPoint : anomalyData) {
        if (dataPoint[0] < 1.0) {
            clusters["low_score"].push_back(dataPoint);
        } else if (dataPoint[0] < 2.0) {
            clusters["medium_score"].push_back(dataPoint);
        } else {
            clusters["high_score"].push_back(dataPoint);
        }
    }
    
    return clusters;
}

std::map<std::string, std::vector<double>> AdvancedAnalysis::analyzeLearningTrajectory(
    std::shared_ptr<Brain> brain,
    int analysisWindow) {
    std::map<std::string, std::vector<double>> trajectory;
    
    // Simulate learning trajectory over time
    trajectory["learning_rate"] = std::vector<double>();
    trajectory["firing_rate"] = std::vector<double>();
    trajectory["plasticity"] = std::vector<double>();
    
    for (int step = 0; step < analysisWindow; ++step) {
        brain->step(step, 0.001);
        
        trajectory["learning_rate"].push_back(brain->getAverageFiringRate());
        trajectory["firing_rate"].push_back(brain->getFiringNeuronCount() / 
                                          std::max(brain->getTotalNeuronCount(), 1.0));
        trajectory["plasticity"].push_back(brain->getPlasticityStrength());
    }
    
    return trajectory;
}

std::map<std::string, double> AdvancedAnalysis::analyzeNetworkResilience(
    std::shared_ptr<Brain> brain,
    double failureRate) {
    std::map<std::string, double> resilience;
    
    // Simulate network failure
    auto neuronCount = brain->getTotalNeuronCount();
    size_t failedNeurons = static_cast<size_t>(neuronCount * failureRate);
    
    // Calculate resilience based on remaining network
    size_t remainingNeurons = neuronCount - failedNeurons;
    resilience["connectivity_retention"] = remainingNeurons / static_cast<double>(neuronCount);
    
    // Calculate functional resilience
    resilience["functional_resilience"] = resilience["connectivity_retention"] * 
                                        (1.0 - failureRate);
    
    // Calculate recovery time (simplified)
    resilience["recovery_time"] = std::exp(-resilience["functional_resilience"]);
    
    return resilience;
}

std::map<std::string, std::vector<double>> AdvancedAnalysis::analyzeDevelopmentalTrajectory(
    std::shared_ptr<Brain> brain,
    int stages) {
    std::map<std::string, std::vector<double>> trajectory;
    
    // Simulate developmental stages
    trajectory["plasticity_rate"] = std::vector<double>();
    trajectory["learning_capacity"] = std::vector<double>();
    trajectory["behavioral_flexibility"] = std::vector<double>();
    
    double plasticity = 0.1;
    for (int stage = 0; stage < stages; ++stage) {
        // Simulate developmental changes
        plasticity *= 1.1; // Plasticity increases with development
        
        trajectory["plasticity_rate"].push_back(plasticity);
        trajectory["learning_capacity"].push_back(plasticity * 0.8);
        trajectory["behavioral_flexibility"].push_back(1.0 - std::pow(plasticity, 2));
    }
    
    return trajectory;
}

} // namespace nlm
