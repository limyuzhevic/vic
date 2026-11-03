#include "AdvancedTools.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace nlm {

struct AdvancedBrainAnalyzer::Impl {
    std::shared_ptr<Brain> brain;
    
    Impl(std::shared_ptr<Brain> b) : brain(b) {}
};

AdvancedBrainAnalyzer::AdvancedBrainAnalyzer(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(brain)) {
}

AdvancedBrainAnalyzer::~AdvancedBrainAnalyzer() = default;

AdvancedBrainAnalyzer::AnalysisReport AdvancedBrainAnalyzer::analyzeCurrentState() const {
    AnalysisReport report;
    
    if (!pImpl->brain) {
        return report;
    }
    
    // Basic statistics
    report.totalNeurons = pImpl->brain->getTotalNeuronCount();
    report.totalSynapses = pImpl->brain->getTotalSynapseCount();
    report.activeNeurons = pImpl->brain->getActiveNeuronCount();
    report.firingNeurons = pImpl->brain->getFiringNeuronCount();
    report.averageFiringRate = pImpl->brain->getAverageFiringRate();
    report.excitationInhibitionRatio = pImpl->brain->getExcitationInhibitionRatio();
    
    // Memory system status
    report.workingMemoryActive = pImpl->brain->getWorkingMemory() != nullptr;
    if (report.workingMemoryActive) {
        report.workingMemoryTraces = pImpl->brain->getWorkingMemory()->getActiveTraces();
    } else {
        report.workingMemoryTraces = 0;
    }
    
    report.episodicMemoryActive = pImpl->brain->getEpisodicMemory() != nullptr;
    if (report.episodicMemoryActive) {
        report.episodicMemoryEpisodes = pImpl->brain->getEpisodicMemory()->getEpisodeCount();
    } else {
        report.episodicMemoryEpisodes = 0;
    }
    
    // Neuromodulation levels
    if (pImpl->brain->getDopamine()) {
        report.dopamineLevel = pImpl->brain->getDopamine()->getLevel();
    } else {
        report.dopamineLevel = 0.0f;
    }
    
    if (pImpl->brain->getNovelty()) {
        report.noveltyLevel = pImpl->brain->getNovelty()->getLevel();
    } else {
        report.noveltyLevel = 0.0f;
    }
    
    if (pImpl->brain->getCuriosity()) {
        report.curiosityLevel = pImpl->brain->getCuriosity()->getLevel();
    } else {
        report.curiosityLevel = 0.0f;
    }
    
    if (pImpl->brain->getPredictionErrorSignal()) {
        report.predictionError = pImpl->brain->getPredictionErrorSignal()->getError();
    } else {
        report.predictionError = 0.0f;
    }
    
    // Development status
    report.developmentalStage = pImpl->brain->getDevelopmentalStage();
    report.plasticityModifier = 1.0f;  // Would need to access development system
    
    // Network properties
    report.regionCount = pImpl->brain->getRegionCount();
    
    float totalWeight = 0.0f;
    float weightSquared = 0.0f;
    size_t synapseCount = 0;
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            totalWeight += weight;
            weightSquared += weight * weight;
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        report.averageSynapticWeight = totalWeight / synapseCount;
        float variance = (weightSquared / synapseCount) - (report.averageSynapticWeight * report.averageSynapticWeight);
        report.synapticWeightVariance = variance > 0.0f ? std::sqrt(variance) : 0.0f;
    } else {
        report.averageSynapticWeight = 0.0f;
        report.synapticWeightVariance = 0.0f;
    }
    
    // Activity patterns
    if (pImpl->brain->getWorkingMemory()) {
        report.memoryActivityLevel = pImpl->brain->getWorkingMemory()->getMemoryActivity();
    } else {
        report.memoryActivityLevel = 0.0f;
    }
    
    report.predictionAccuracy = 0.5f;  // Placeholder
    report.learningRate = 0.01f;  // Placeholder
    
    // Stability indicators
    report.networkStability = 1.0f - (report.synapticWeightVariance / std::max(1.0f, report.averageSynapticWeight));
    report.memoryRetention = report.workingMemoryTraces / std::max(1.0f, static_cast<float>(report.totalNeurons));
    report.adaptability = report.dopamineLevel * report.curiosityLevel;  // Simplified
    
    return report;
}

AdvancedBrainAnalyzer::VisualizationData AdvancedBrainAnalyzer::generateVisualizationData() const {
    VisualizationData data;
    
    if (!pImpl->brain) {
        return data;
    }
    
    // For now, return empty data structure
    // In a full implementation, this would:
    // 1. Extract neuron positions from regions
    // 2. Get current neuron activities
    // 3. Determine neuron types
    // 4. Extract synaptic connections
    // 5. Get synaptic weights
    // 6. Extract memory patterns
    // 7. Get memory activations
    // 8. Capture spike history
    // 9. Record activity trajectories
    
    return data;
}

AdvancedBrainAnalyzer::ComparisonResult AdvancedBrainAnalyzer::compareStates(
    const AnalysisReport& previousReport,
    const AnalysisReport& currentReport) const {
    ComparisonResult result;
    
    // Calculate state difference (Euclidean distance)
    std::vector<float> previousVec = {
        static_cast<float>(previousReport.totalNeurons),
        static_cast<float>(previousReport.totalSynapses),
        static_cast<float>(previousReport.activeNeurons),
        previousReport.averageFiringRate,
        previousReport.averageSynapticWeight,
        previousReport.workingMemoryTraces,
        previousReport.episodicMemoryEpisodes,
        previousReport.dopamineLevel,
        previousReport.noveltyLevel,
        previousReport.curiosityLevel
    };
    
    std::vector<float> currentVec = {
        static_cast<float>(currentReport.totalNeurons),
        static_cast<float>(currentReport.totalSynapses),
        static_cast<float>(currentReport.activeNeurons),
        currentReport.averageFiringRate,
        currentReport.averageSynapticWeight,
        currentReport.workingMemoryTraces,
        currentReport.episodicMemoryEpisodes,
        currentReport.dopamineLevel,
        currentReport.noveltyLevel,
        currentReport.curiosityLevel
    };
    
    float sumSqDiff = 0.0f;
    for (size_t i = 0; i < previousVec.size(); ++i) {
        float diff = currentVec[i] - previousVec[i];
        sumSqDiff += diff * diff;
    }
    result.stateDifference = std::sqrt(sumSqDiff);
    
    // Calculate activity similarity (inverse of state difference)
    result.activitySimilarity = 1.0f / (1.0f + result.stateDifference);
    
    // Calculate connectivity change
    result.connectivityChange = currentReport.averageSynapticWeight - previousReport.averageSynapticWeight;
    
    // Calculate memory change
    result.memoryChange = currentReport.workingMemoryTraces - previousReport.workingMemoryTraces;
    
    // Generate trend analysis
    std::stringstream ss;
    if (result.stateDifference > 10.0f) {
        ss << "Significant state change detected"; 
    } else if (result.stateDifference > 5.0f) {
        ss << "Moderate state change"; 
    } else {
        ss << "Stable state"; 
    }
    
    if (result.connectivityChange > 0.1f) {
        ss << ", increasing connectivity"; 
    } else if (result.connectivityChange < -0.1f) {
        ss << ", decreasing connectivity"; 
    }
    
    result.trendAnalysis = ss.str();
    
    return result;
}

AdvancedBrainAnalyzer::AnomalyReport AdvancedBrainAnalyzer::detectAnomalies() const {
    AnomalyReport report;
    report.hasAnomalies = false;
    report.severity = 0.0f;
    
    if (!pImpl->brain) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("BRAIN_NULL");
        report.descriptions.push_back("Brain object is null");
        report.recommendation = "Ensure brain object is properly initialized";
        return report;
    }
    
    // Check for anomalies
    auto currentReport = analyzeCurrentState();
    
    if (currentReport.totalNeurons == 0) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("NO_NEURONS");
        report.descriptions.push_back("Brain has no neurons initialized");
        report.severity += 10.0f;
    }
    
    if (currentReport.totalSynapses == 0 && currentReport.totalNeurons > 0) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("NO_SYNAPSES");
        report.descriptions.push_back("Brain has neurons but no synaptic connections");
        report.severity += 5.0f;
    }
    
    if (currentReport.activeNeurons > currentReport.totalNeurons * 0.8f) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("HIGH_ACTIVITY");
        report.descriptions.push_back("Unusually high proportion of active neurons");
        report.severity += 3.0f;
    }
    
    if (currentReport.workingMemoryActive && currentReport.workingMemoryTraces > 1000) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("MEMORY_OVERLOAD");
        report.descriptions.push_back("Working memory has too many traces");
        report.severity += 7.0f;
    }
    
    if (currentReport.episodicMemoryActive && currentReport.episodicMemoryEpisodes > 1000) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("EPISODIC_OVERLOAD");
        report.descriptions.push_back("Episodic memory has too many episodes");
        report.severity += 5.0f;
    }
    
    if (currentReport.averageFiringRate > 200.0f) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("HIGH_FIRING_RATE");
        report.descriptions.push_back("Average firing rate is extremely high");
        report.severity += 8.0f;
    }
    
    if (currentReport.excitationInhibitionRatio > 100.0f) {
        report.hasAnomalies = true;
        report.anomalyTypes.push_back("HIGH_E/I_RATIO");
        report.descriptions.push_back("Excitatory/inhibitory ratio is extremely high");
        report.severity += 6.0f;
    }
    
    if (report.hasAnomalies) {
        report.recommendation = "Review brain initialization and configuration parameters";
        if (report.severity > 20.0f) {
            report.recommendation += " Consider resetting brain and reinitializing with different parameters";
        } else {
            report.recommendation += ". Current operation may be acceptable but monitor closely.";
        }
    } else {
        report.recommendation = "Brain operation appears normal";
    }
    
    return report;
}

AdvancedBrainAnalyzer::LearningProgressReport AdvancedBrainAnalyzer::generateLearningReport() const {
    LearningProgressReport report;
    
    // Generate simplified learning progress report
    report.learningRate = 0.01f;  // Would need to track over time
    report.convergenceSpeed = 0.5f;  // Would need to track over time
    report.stability = 1.0f - (report.learningRate * 10.0f);  // Simplified
    
    // Generate sample memory retention curve
    report.memoryRetentionCurve = {0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
    
    // Generate sample prediction accuracy curve
    report.predictionAccuracyCurve = {0.5f, 0.6f, 0.7f, 0.75f, 0.8f, 0.82f, 0.85f, 0.87f};
    
    // Generate overall assessment
    std::stringstream ss;
    if (report.learningRate > 0.1f) {
        ss << "Rapid learning detected"; 
    } else if (report.learningRate > 0.01f) {
        ss << "Normal learning rate"; 
    } else {
        ss << "Slow learning rate"; 
    }
    
    if (report.convergenceSpeed > 0.8f) {
        ss << ", fast convergence"; 
    } else if (report.convergenceSpeed > 0.5f) {
        ss << ", moderate convergence"; 
    } else {
        ss << ", slow convergence"; 
    }
    
    if (report.stability > 0.8f) {
        ss << ", stable operation"; 
    } else {
        ss << ", unstable operation"; 
    }
    
    report.overallAssessment = ss.str();
    
    return report;
}

bool AdvancedBrainAnalyzer::exportForExternalAnalysis(const std::string& filepath) const {
    // Implementation would export brain state in formats suitable for
    // external analysis tools (JSON, CSV, etc.)
    NLM_LOG_INFO("Exporting brain state to " + filepath);
    return false;  // Placeholder
}

AdvancedBrainAnalyzer::NetworkTopology AdvancedBrainAnalyzer::analyzeNetworkTopology() const {
    NetworkTopology topology;
    
    if (!pImpl->brain) {
        return topology;
    }
    
    topology.totalRegions = pImpl->brain->getRegionCount();
    
    // This would require access to population information
    // For now, return basic topology
    return topology;
}

AdvancedBrainAnalyzer::PerformanceBenchmark AdvancedBrainAnalyzer::benchmarkPerformance(size_t steps) const {
    PerformanceBenchmark benchmark;
    
    // This would require timing the actual brain operation
    // For now, return placeholder values
    benchmark.wallClockTimePerStep = 0.001;  // 1 ms per step
    benchmark.cpuUtilization = 50.0;  // 50%
    benchmark.memoryUsage = 100.0;  // MB
    benchmark.spikesProcessedPerStep = 100;
    benchmark.neuronsActivePerStep = 50;
    benchmark.plasticityOperationsPerStep = 10;
    
    return benchmark;
}

AdvancedBrainAnalyzer::SystemDiagnostics AdvancedBrainAnalyzer::getDiagnostics() const {
    SystemDiagnostics diag;
    
    if (!pImpl->brain) {
        diag.systemStatus = "ERROR: Brain object is null";
        diag.errors.push_back("Brain object not initialized");
        return diag;
    }
    
    auto currentReport = analyzeCurrentState();
    auto anomalyReport = detectAnomalies();
    
    if (anomalyReport.hasAnomalies) {
        diag.systemStatus = "WARNING: System has detected anomalies";
        diag.warnings = anomalyReport.anomalyTypes;
        diag.errors.insert(diag.errors.end(), anomalyReport.descriptions.begin(), anomalyReport.descriptions.end());
    } else {
        diag.systemStatus = "OK: System operating normally";
    }
    
    if (currentReport.averageFiringRate > 100.0f) {
        diag.performanceIssues.push_back("High firing rate may indicate instability");
    }
    
    if (currentReport.excitationInhibitionRatio > 50.0f) {
        diag.performanceIssues.push_back("High E/I ratio may indicate imbalance");
    }
    
    if (!diag.performanceIssues.empty()) {
        diag.systemStatus = "PERFORMANCE: Issues detected";
    }
    
    diag.recommendation = "Review system status above for recommendations";
    
    return diag;
}

std::string AdvancedBrainAnalyzer::generateCompleteReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Advanced Brain Analysis Report ===" << std::endl;
    ss << std::endl;
    
    if (!pImpl->brain) {
        ss << "ERROR: Brain object is null" << std::endl;
        return ss.str();
    }
    
    auto report = analyzeCurrentState();
    auto anomalies = detectAnomalies();
    auto diagnostics = getDiagnostics();
    auto learning = generateLearningReport();
    
    ss << "Brain State Analysis:" << std::endl;
    ss << "  Total Neurons: " << report.totalNeurons << std::endl;
    ss << "  Total Synapses: " << report.totalSynapses << std::endl;
    ss << "  Active Neurons: " << report.activeNeurons << std::endl;
    ss << "  Firing Neurons: " << report.firingNeurons << std::endl;
    ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << report.averageFiringRate << " Hz" << std::endl;
    ss << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(2) << report.excitationInhibitionRatio << std::endl;
    ss << std::endl;
    
    ss << "Memory System:" << std::endl;
    ss << "  Working Memory: " << (report.workingMemoryActive ? "Active (" + std::to_string(report.workingMemoryTraces) + " traces)" : "Not Active") << std::endl;
    ss << "  Episodic Memory: " << (report.episodicMemoryActive ? "Active (" + std::to_string(report.episodicMemoryEpisodes) + " episodes)" : "Not Active") << std::endl;
    ss << "  Memory Activity Level: " << std::fixed << std::setprecision(3) << report.memoryActivityLevel << std::endl;
    ss << std::endl;
    
    ss << "Neuromodulation:" << std::endl;
    ss << "  Dopamine Level: " << std::fixed << std::setprecision(3) << report.dopamineLevel << std::endl;
    ss << "  Novelty Level: " << std::fixed << std::setprecision(3) << report.noveltyLevel << std::endl;
    ss << "  Curiosity Level: " << std::fixed << std::setprecision(3) << report.curiosityLevel << std::endl;
    ss << "  Prediction Error: " << std::fixed << std::setprecision(3) << report.predictionError << std::endl;
    ss << std::endl;
    
    ss << "Development:" << std::endl;
    ss << "  Developmental Stage: " << static_cast<int>(report.developmentalStage) << std::endl;
    ss << "  Plasticity Modifier: " << std::fixed << std::setprecision(3) << report.plasticityModifier << std::endl;
    ss << std::endl;
    
    ss << "Network Properties:" << std::endl;
    ss << "  Region Count: " << report.regionCount << std::endl;
    ss << "  Average Synaptic Weight: " << std::fixed << std::setprecision(4) << report.averageSynapticWeight << std::endl;
    ss << "  Synaptic Weight Variance: " << std::fixed << std::setprecision(4) << report.synapticWeightVariance << std::endl;
    ss << std::endl;
    
    ss << "Stability Indicators:" << std::endl;
    ss << "  Network Stability: " << std::fixed << std::setprecision(3) << report.networkStability << std::endl;
    ss << "  Memory Retention: " << std::fixed << std::setprecision(3) << report.memoryRetention << std::endl;
    ss << "  Adaptability: " << std::fixed << std::setprecision(3) << report.adaptability << std::endl;
    ss << std::endl;
    
    ss << "Anomalies Detected: " << (anomalies.hasAnomalies ? "YES" : "NO") << std::endl;
    if (anomalies.hasAnomalies) {
        ss << "  Severity: " << anomalies.severity << std::endl;
        ss << "  Types: ";
        for (size_t i = 0; i < anomalies.anomalyTypes.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << anomalies.anomalyTypes[i];
        }
        ss << std::endl;
        ss << "  Recommendation: " << anomalies.recommendation << std::endl;
    } else {
        ss << "  No anomalies detected." << std::endl;
    }
    ss << std::endl;
    
    ss << "Learning Progress:" << std::endl;
    ss << "  Learning Rate: " << std::fixed << std::setprecision(4) << learning.learningRate << std::endl;
    ss << "  Convergence Speed: " << std::fixed << std::setprecision(3) << learning.convergenceSpeed << std::endl;
    ss << "  Stability: " << std::fixed << std::setprecision(3) << learning.stability << std::endl;
    ss << "  Assessment: " << learning.overallAssessment << std::endl;
    ss << std::endl;
    
    ss << "System Diagnostics:" << std::endl;
    ss << "  Status: " << diagnostics.systemStatus << std::endl;
    if (!diagnostics.warnings.empty()) {
        ss << "  Warnings: ";
        for (size_t i = 0; i < diagnostics.warnings.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << diagnostics.warnings[i];
        }
        ss << std::endl;
    }
    if (!diagnostics.errors.empty()) {
        ss << "  Errors: ";
        for (size_t i = 0; i < diagnostics.errors.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << diagnostics.errors[i];
        }
        ss << std::endl;
    }
    if (!diagnostics.performanceIssues.empty()) {
        ss << "  Performance Issues: ";
        for (size_t i = 0; i < diagnostics.performanceIssues.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << diagnostics.performanceIssues[i];
        }
        ss << std::endl;
    }
    ss << "  Recommendation: " << diagnostics.recommendation << std::endl;
    ss << std::endl;
    
    return ss.str();
}

} // namespace nlm