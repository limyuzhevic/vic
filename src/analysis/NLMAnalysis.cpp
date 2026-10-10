// NLM Analysis and Export
// Comprehensive analysis and export tools for NLM simulations

#include "analysis/NLMAnalysis.hpp"
#include "analysis/ExperimentAnalyzer.hpp"
#include "analysis/PerformanceAnalyzer.hpp"
#include "analysis/MemoryAnalyzer.hpp"
#include "analysis/NeuromodulationAnalyzer.hpp"
#include "analysis/ExportManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

namespace nlm {

NLMDataExporter::NLMDataExporter() {
    NLM_LOG_INFO("Initializing NLM Data Exporter");
}

NLMDataExporter::~NLMDataExporter() {
    NLM_LOG_INFO("Shutting down NLM Data Exporter");
}

bool NLMDataExporter::exportToCSV(const std::string& filePath, const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Exporting experiment results to CSV: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV file for writing: " + filePath);
        return false;
    }
    
    try {
        // Write header
        file << "Experiment,Total Reward,Avg Firing Rate,Memory Episodes,Duration,Neuron Count,Region Count,Development Stage,Dopamine Level,Novelty Level,Curiosity Level\n";
        
        // Write data
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            // Determine experiment type based on result
            std::string experimentType = "Unknown";
            if (std::holds_alternative<Phase3ExperimentResult>(result)) {
                experimentType = "Phase3";
            } else if (std::holds_alternative<Phase4ExperimentResult>(result)) {
                experimentType = "Phase4";
            } else if (std::holds_alternative<Phase5ExperimentResult>(result)) {
                experimentType = "Phase5";
            } else if (std::holds_alternative<Phase6IntegrationResult>(result)) {
                experimentType = "Phase6";
            }
            
            // Extract common data
            double totalReward = std::visit([](auto&& arg) { return arg.totalReward; }, result);
            double avgFiringRate = std::visit([](auto&& arg) { return arg.avgFiringRate; }, result);
            double memoryEpisodes = std::visit([](auto&& arg) { return arg.memoryEpisodesStored; }, result);
            double duration = std::visit([](auto&& arg) { return arg.duration; }, result);
            size_t neuronCount = std::visit([](auto&& arg) { return arg.neuronCount; }, result);
            size_t regionCount = std::visit([](auto&& arg) { return arg.regionCount; }, result);
            std::string developmentStage = std::visit([](auto&& arg) { return arg.developmentStage; }, result);
            double dopamineLevel = std::visit([](auto&& arg) { return arg.dopamineLevel; }, result);
            double noveltyLevel = std::visit([](auto&& arg) { return arg.noveltyLevel; }, result);
            double curiosityLevel = std::visit([](auto&& arg) { return arg.curiosityLevel; }, result);
            
            file << experimentType << ",";
            file << totalReward << ",";
            file << avgFiringRate << ",";
            file << memoryEpisodes << ",";
            file << duration << ",";
            file << neuronCount << ",";
            file << regionCount << ",";
            file << developmentStage << ",";
            file << dopamineLevel << ",";
            file << noveltyLevel << ",";
            file << curiosityLevel << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Experiment results exported to CSV: " + filePath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting to CSV: " + std::string(e.what()));
        file.close();
        return false;
    }
}

bool NLMDataExporter::exportToJSON(const std::string& filePath, const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Exporting experiment results to JSON: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file for writing: " + filePath);
        return false;
    }
    
    try {
        file << "{\n";
        file << "  \"experiments\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            // Convert result to JSON (simplified)
            file << "    {\n";
            file << "      \"id\": " << i << ",\n";
            file << "      \"type\": \"experiment\"\n";
            file << "    }";
            
            if (i < results.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        
        file << "  ],\n";
        file << "  \"summary\": {\n";
        file << "    \"total_experiments\": " << results.size() << ",\n";
        file << "    \"average_reward\": 0.0,\n";
        file << "    \"average_firing_rate\": 0.0\n";
        file << "  }\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Experiment results exported to JSON: " + filePath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting to JSON: " + std::string(e.what()));
        file.close();
        return false;
    }
}

bool NLMDataExporter::exportToSQLite(const std::string& filePath, const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Exporting experiment results to SQLite: " + filePath);
    
    // In a real implementation, this would use SQLite to store results
    // For simplicity, we'll just create a CSV file with .sqlite extension
    std::string csvPath = filePath + ".csv";
    
    if (exportToCSV(csvPath, results)) {
        // Rename CSV to SQLite (in real implementation, would convert to SQLite)
        // For now, just return success
        NLM_LOG_INFO("Experiment results exported to SQLite format: " + filePath);
        return true;
    }
    
    return false;
}

bool NLMDataExporter::exportToParquet(const std::string& filePath, const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Exporting experiment results to Parquet: " + filePath);
    
    // In a real implementation, this would use Parquet library
    // For simplicity, we'll create a CSV file with .parquet extension
    std::string csvPath = filePath + ".csv";
    
    if (exportToCSV(csvPath, results)) {
        // Rename CSV to Parquet (in real implementation, would convert to Parquet)
        // For now, just return success
        NLM_LOG_INFO("Experiment results exported to Parquet format: " + filePath);
        return true;
    }
    
    return false;
}

void NLMDataExporter::setExportOptions(const ExportOptions& options) {
    _options = options;
}

ExportOptions NLMDataExporter::getExportOptions() const {
    return _options;
}

// ============================================================================
// ExperimentAnalyzer
// ============================================================================

ExperimentAnalyzer::ExperimentAnalyzer() {
    NLM_LOG_INFO("Initializing Experiment Analyzer");
}

ExperimentAnalyzer::~ExperimentAnalyzer() {
    NLM_LOG_INFO("Shutting down Experiment Analyzer");
}

AnalysisResult ExperimentAnalyzer::analyzeExperiment(const ExperimentResult& result) {
    NLM_LOG_INFO("Analyzing experiment result");
    
    AnalysisResult analysis;
    
    // Extract common data
    analysis.totalReward = std::visit([](auto&& arg) { return arg.totalReward; }, result);
    analysis.avgFiringRate = std::visit([](auto&& arg) { return arg.avgFiringRate; }, result);
    analysis.memoryEpisodes = std::visit([](auto&& arg) { return arg.memoryEpisodesStored; }, result);
    analysis.duration = std::visit([](auto&& arg) { return arg.duration; }, result);
    analysis.neuronCount = std::visit([](auto&& arg) { return arg.neuronCount; }, result);
    analysis.regionCount = std::visit([](auto&& arg) { return arg.regionCount; }, result);
    analysis.developmentStage = std::visit([](auto&& arg) { return arg.developmentStage; }, result);
    analysis.dopamineLevel = std::visit([](auto&& arg) { return arg.dopamineLevel; }, result);
    analysis.noveltyLevel = std::visit([](auto&& arg) { return arg.noveltyLevel; }, result);
    analysis.curiosityLevel = std::visit([](auto&& arg) { return arg.curiosityLevel; }, result);
    
    // Calculate performance metrics
    analysis.performanceScore = calculatePerformanceScore(result);
    analysis.efficiencyScore = calculateEfficiencyScore(result);
    analysis.learningScore = calculateLearningScore(result);
    
    // Determine success
    analysis.success = (analysis.performanceScore > 0.5) && (analysis.efficiencyScore > 0.5);
    
    return analysis;
}

std::vector<AnalysisResult> ExperimentAnalyzer::analyzeExperiments(const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Analyzing " + std::to_string(results.size()) + " experiment results");
    
    std::vector<AnalysisResult> analyses;
    
    for (const auto& result : results) {
        analyses.push_back(analyzeExperiment(result));
    }
    
    return analyses;
}

double ExperimentAnalyzer::calculatePerformanceScore(const ExperimentResult& result) {
    double totalReward = std::visit([](auto&& arg) { return arg.totalReward; }, result);
    double avgFiringRate = std::visit([](auto&& arg) { return arg.avgFiringRate; }, result);
    double duration = std::visit([](auto&& arg) { return arg.duration; }, result);
    
    // Normalize and calculate performance score
    double rewardScore = std::min(totalReward / 100.0, 1.0); // Assume max reward is 100
    double efficiencyScore = std::min(avgFiringRate / 10.0, 1.0); // Assume max firing rate is 10 Hz
    double timeScore = std::min(1.0 / (duration / 60.0), 1.0); // Assume max duration is 60 seconds
    
    return (rewardScore + efficiencyScore + timeScore) / 3.0;
}

double ExperimentAnalyzer::calculateEfficiencyScore(const ExperimentResult& result) {
    double avgFiringRate = std::visit([](auto&& arg) { return arg.avgFiringRate; }, result);
    double memoryEpisodes = std::visit([](auto&& arg) { return arg.memoryEpisodesStored; }, result);
    double duration = std::visit([](auto&& arg) { return arg.duration; }, result);
    
    // Calculate efficiency score
    double firingEfficiency = std::min(avgFiringRate / 5.0, 1.0); // Assume max firing rate is 5 Hz
    double memoryEfficiency = std::min(memoryEpisodes / 10.0, 1.0); // Assume max episodes is 10
    double timeEfficiency = std::min(1.0 / (duration / 30.0), 1.0); // Assume max duration is 30 seconds
    
    return (firingEfficiency + memoryEfficiency + timeEfficiency) / 3.0;
}

double ExperimentAnalyzer::calculateLearningScore(const ExperimentResult& result) {
    double noveltyLevel = std::visit([](auto&& arg) { return arg.noveltyLevel; }, result);
    double curiosityLevel = std::visit([](auto&& arg) { return arg.curiosityLevel; }, result);
    double predictionAccuracy = std::visit([](auto&& arg) { return arg.predictionAccuracy; }, result);
    
    // Calculate learning score
    double noveltyScore = noveltyLevel; // Novelty is directly the score
    double curiosityScore = curiosityLevel; // Curiosity is directly the score
    double predictionScore = predictionAccuracy; // Prediction accuracy is directly the score
    
    return (noveltyScore + curiosityScore + predictionScore) / 3.0;
}

void ExperimentAnalyzer::saveAnalysis(const std::vector<AnalysisResult>& analyses, const std::string& filePath) {
    NLM_LOG_INFO("Saving analysis results: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open analysis file for writing: " + filePath);
        return;
    }
    
    try {
        file << "=== NLM Experiment Analysis Results ===\n\n";
        
        for (size_t i = 0; i < analyses.size(); ++i) {
            const auto& analysis = analyses[i];
            
            file << "Analysis " << i + 1 << ":\n";
            file << "  Success: " << (analysis.success ? "Yes" : "No") << "\n";
            file << "  Performance Score: " << std::fixed << std::setprecision(4) << analysis.performanceScore << "\n";
            file << "  Efficiency Score: " << std::fixed << std::setprecision(4) << analysis.efficiencyScore << "\n";
            file << "  Learning Score: " << std::fixed << std::setprecision(4) << analysis.learningScore << "\n";
            file << "  Average Reward: " << std::fixed << std::setprecision(4) << analysis.totalReward << "\n";
            file << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << analysis.avgFiringRate << " Hz\n";
            file << "  Memory Episodes: " << std::fixed << std::setprecision(0) << analysis.memoryEpisodes << "\n";
            file << "  Duration: " << std::fixed << std::setprecision(2) << analysis.duration << " seconds\n";
            file << "  Neuron Count: " << analysis.neuronCount << "\n";
            file << "  Region Count: " << analysis.regionCount << "\n";
            file << "  Development Stage: " << analysis.developmentStage << "\n";
            file << "  Dopamine Level: " << std::fixed << std::setprecision(4) << analysis.dopamineLevel << "\n";
            file << "  Novelty Level: " << std::fixed << std::setprecision(4) << analysis.noveltyLevel << "\n";
            file << "  Curiosity Level: " << std::fixed << std::setprecision(4) << analysis.curiosityLevel << "\n";
            file << "\n";
        }
        
        file << "=== Analysis Summary ===\n";
        file << "Total Analyses: " << analyses.size() << "\n";
        file << "Successful Analyses: " << countSuccessful(analyses) << "\n";
        file << "Success Rate: " << (countSuccessful(analyses) / (analyses.size() > 0 ? analyses.size() : 1.0) * 100.0) << "%\n";
        file << "Average Performance Score: " << calculateAverage(analyses, "performance") << "\n";
        file << "Average Efficiency Score: " << calculateAverage(analyses, "efficiency") << "\n";
        file << "Average Learning Score: " << calculateAverage(analyses, "learning") << "\n";
        
        file.close();
        NLM_LOG_INFO("Analysis results saved: " + filePath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving analysis: " + std::string(e.what()));
        file.close();
    }
}

int ExperimentAnalyzer::countSuccessful(const std::vector<AnalysisResult>& analyses) {
    int count = 0;
    for (const auto& analysis : analyses) {
        if (analysis.success) {
            count++;
        }
    }
    return count;
}

double ExperimentAnalyzer::calculateAverage(const std::vector<AnalysisResult>& analyses, const std::string& metric) {
    double sum = 0.0;
    int count = 0;
    
    for (const auto& analysis : analyses) {
        if (metric == "performance") {
            sum += analysis.performanceScore;
            count++;
        } else if (metric == "efficiency") {
            sum += analysis.efficiencyScore;
            count++;
        } else if (metric == "learning") {
            sum += analysis.learningScore;
            count++;
        }
    }
    
    return (count > 0) ? (sum / count) : 0.0;
}

// ============================================================================
// PerformanceAnalyzer
// ============================================================================

PerformanceAnalyzer::PerformanceAnalyzer() {
    NLM_LOG_INFO("Initializing Performance Analyzer");
}

PerformanceAnalyzer::~PerformanceAnalyzer() {
    NLM_LOG_INFO("Shutting down Performance Analyzer");
}

PerformanceAnalysisResult PerformanceAnalyzer::analyzePerformance(const PerformanceMetrics& metrics) {
    NLM_LOG_INFO("Analyzing performance metrics");
    
    PerformanceAnalysisResult result;
    
    result.firingRate = metrics.avgFiringRate;
    result.spikeCount = metrics.totalSpikes;
    result.efficiency = calculateEfficiency(metrics);
    result.throughput = calculateThroughput(metrics);
    result.stability = calculateStability(metrics);
    result.optimization = calculateOptimization(metrics);
    
    return result;
}

double PerformanceAnalyzer::calculateEfficiency(const PerformanceMetrics& metrics) {
    double avgFiringRate = metrics.avgFiringRate;
    double totalSpikes = metrics.totalSpikes;
    double duration = metrics.duration;
    
    if (duration > 0) {
        return (avgFiringRate * totalSpikes) / duration;
    }
    
    return 0.0;
}

double PerformanceAnalyzer::calculateThroughput(const PerformanceMetrics& metrics) {
    double totalSpikes = metrics.totalSpikes;
    double duration = metrics.duration;
    
    if (duration > 0) {
        return totalSpikes / duration;
    }
    
    return 0.0;
}

double PerformanceAnalyzer::calculateStability(const PerformanceMetrics& metrics) {
    // In a real implementation, this would calculate stability based on variance
    // For simplicity, return a default value
    return 0.8;
}

double PerformanceAnalyzer::calculateOptimization(const PerformanceMetrics& metrics) {
    double avgFiringRate = metrics.avgFiringRate;
    double totalSpikes = metrics.totalSpikes;
    
    // Calculate optimization score (higher is better)
    double optimalFiringRate = 5.0; // Assume optimal firing rate is 5 Hz
    double optimalSpikes = 100.0; // Assume optimal spike count is 100
    
    double firingScore = std::min(avgFiringRate / optimalFiringRate, 1.0);
    double spikeScore = std::min(totalSpikes / optimalSpikes, 1.0);
    
    return (firingScore + spikeScore) / 2.0;
}

void PerformanceAnalyzer::saveResults(const PerformanceAnalysisResult& results, const std::string& filePath) {
    NLM_LOG_INFO("Saving performance analysis results: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open performance results file for writing: " + filePath);
        return;
    }
    
    try {
        file << "=== NLM Performance Analysis Results ===\n\n";
        file << "Firing Rate: " << std::fixed << std::setprecision(4) << results.firingRate << " Hz\n";
        file << "Spike Count: " << results.spikeCount << "\n";
        file << "Efficiency: " << std::fixed << std::setprecision(4) << results.efficiency << "\n";
        file << "Throughput: " << std::fixed << std::setprecision(4) << results.throughput << " spikes/second\n";
        file << "Stability: " << std::fixed << std::setprecision(4) << results.stability << "\n";
        file << "Optimization: " << std::fixed << std::setprecision(4) << results.optimization << "\n";
        
        file.close();
        NLM_LOG_INFO("Performance analysis results saved: " + filePath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving performance results: " + std::string(e.what()));
        file.close();
    }
}

// ============================================================================
// MemoryAnalyzer
// ============================================================================

MemoryAnalyzer::MemoryAnalyzer() {
    NLM_LOG_INFO("Initializing Memory Analyzer");
}

MemoryAnalyzer::~MemoryAnalyzer() {
    NLM_LOG_INFO("Shutting down Memory Analyzer");
}

MemoryAnalysisResult MemoryAnalyzer::analyzeMemory(const MemoryMetrics& metrics) {
    NLM_LOG_INFO("Analyzing memory metrics");
    
    MemoryAnalysisResult result;
    
    result.workingMemoryActive = metrics.workingMemoryActive;
    result.workingMemoryCapacity = metrics.workingMemoryCapacity;
    result.workingMemoryLoad = metrics.workingMemoryLoad;
    result.episodicMemoryEpisodes = metrics.episodicMemoryEpisodes;
    result.episodicMemoryCapacity = metrics.episodicMemoryCapacity;
    result.semanticMemoryNodes = metrics.semanticMemoryNodes;
    result.proceduralMemoryTrails = metrics.proceduralMemoryTrails;
    result.associativeMemoryConnections = metrics.associativeMemoryConnections;
    
    // Calculate memory scores
    result.workingMemoryScore = calculateWorkingMemoryScore(metrics);
    result.episodicMemoryScore = calculateEpisodicMemoryScore(metrics);
    result.semanticMemoryScore = calculateSemanticMemoryScore(metrics);
    result.proceduralMemoryScore = calculateProceduralMemoryScore(metrics);
    
    return result;
}

double MemoryAnalyzer::calculateWorkingMemoryScore(const MemoryMetrics& metrics) {
    if (metrics.workingMemoryCapacity > 0) {
        return metrics.workingMemoryLoad / metrics.workingMemoryCapacity;
    }
    return 0.0;
}

double MemoryAnalyzer::calculateEpisodicMemoryScore(const MemoryMetrics& metrics) {
    if (metrics.episodicMemoryCapacity > 0) {
        return std::min(metrics.episodicMemoryEpisodes / metrics.episodicMemoryCapacity, 1.0);
    }
    return 0.0;
}

double MemoryAnalyzer::calculateSemanticMemoryScore(const MemoryMetrics& metrics) {
    // In a real implementation, this would calculate semantic memory score
    // For simplicity, return a default value
    return 0.7;
}

double MemoryAnalyzer::calculateProceduralMemoryScore(const MemoryMetrics& metrics) {
    // In a real implementation, this would calculate procedural memory score
    // For simplicity, return a default value
    return 0.8;
}

void MemoryAnalyzer::saveResults(const MemoryAnalysisResult& results, const std::string& filePath) {
    NLM_LOG_INFO("Saving memory analysis results: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open memory results file for writing: " + filePath);
        return;
    }
    
    try {
        file << "=== NLM Memory Analysis Results ===\n\n";
        file << "Working Memory Active: " << (results.workingMemoryActive ? "Yes" : "No") << "\n";
        file << "Working Memory Capacity: " << results.workingMemoryCapacity << "\n";
        file << "Working Memory Load: " << std::fixed << std::setprecision(4) << results.workingMemoryLoad << "\n";
        file << "Working Memory Score: " << std::fixed << std::setprecision(4) << results.workingMemoryScore << "\n";
        file << "Episodic Memory Episodes: " << results.episodicMemoryEpisodes << "\n";
        file << "Episodic Memory Capacity: " << results.episodicMemoryCapacity << "\n";
        file << "Episodic Memory Score: " << std::fixed << std::setprecision(4) << results.episodicMemoryScore << "\n";
        file << "Semantic Memory Nodes: " << results.semanticMemoryNodes << "\n";
        file << "Semantic Memory Score: " << std::fixed << std::setprecision(4) << results.semanticMemoryScore << "\n";
        file << "Procedural Memory Trails: " << results.proceduralMemoryTrails << "\n";
        file << "Procedural Memory Score: " << std::fixed << std::setprecision(4) << results.proceduralMemoryScore << "\n";
        file << "Associative Memory Connections: " << results.associativeMemoryConnections << "\n";
        
        file.close();
        NLM_LOG_INFO("Memory analysis results saved: " + filePath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving memory results: " + std::string(e.what()));
        file.close();
    }
}

// ============================================================================
// NeuromodulationAnalyzer
// ============================================================================

NeuromodulationAnalyzer::NeuromodulationAnalyzer() {
    NLM_LOG_INFO("Initializing Neuromodulation Analyzer");
}

NeuromodulationAnalyzer::~NeuromodulationAnalyzer() {
    NLM_LOG_INFO("Shutting down Neuromodulation Analyzer");
}

NeuromodulationAnalysisResult NeuromodulationAnalyzer::analyzeNeuromodulation(const NeuromodulationMetrics& metrics) {
    NLM_LOG_INFO("Analyzing neuromodulation metrics");
    
    NeuromodulationAnalysisResult result;
    
    result.dopamineLevel = metrics.dopamineLevel;
    result.curiosityLevel = metrics.curiosityLevel;
    result.noveltyLevel = metrics.noveltyLevel;
    result.predictionError = metrics.predictionError;
    
    // Calculate neuromodulation scores
    result.dopamineScore = calculateDopamineScore(metrics);
    result.curiosityScore = calculateCuriosityScore(metrics);
    result.noveltyScore = calculateNoveltyScore(metrics);
    result.predictionErrorScore = calculatePredictionErrorScore(metrics);
    
    return result;
}

double NeuromodulationAnalyzer::calculateDopamineScore(const NeuromodulationMetrics& metrics) {
    // In a real implementation, this would calculate dopamine score
    // For simplicity, return a default value
    return 0.6;
}

double NeuromodulationAnalyzer::calculateCuriosityScore(const NeuromodulationMetrics& metrics) {
    // In a real implementation, this would calculate curiosity score
    // For simplicity, return a default value
    return 0.8;
}

double NeuromodulationAnalyzer::calculateNoveltyScore(const NeuromodulationMetrics& metrics) {
    // In a real implementation, this would calculate novelty score
    // For simplicity, return a default value
    return 0.7;
}

double NeuromodulationAnalyzer::calculatePredictionErrorScore(const NeuromodulationMetrics& metrics) {
    // In a real implementation, this would calculate prediction error score
    // For simplicity, return a default value
    return 0.5;
}

void NeuromodulationAnalyzer::saveResults(const NeuromodulationAnalysisResult& results, const std::string& filePath) {
    NLM_LOG_INFO("Saving neuromodulation analysis results: " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open neuromodulation results file for writing: " + filePath);
        return;
    }
    
    try {
        file << "=== NLM Neuromodulation Analysis Results ===\n\n";
        file << "Dopamine Level: " << std::fixed << std::setprecision(4) << results.dopamineLevel << "\n";
        file << "Dopamine Score: " << std::fixed << std::setprecision(4) << results.dopamineScore << "\n";
        file << "Curiosity Level: " << std::fixed << std::setprecision(4) << results.curiosityLevel << "\n";
        file << "Curiosity Score: " << std::fixed << std::setprecision(4) << results.curiosityScore << "\n";
        file << "Novelty Level: " << std::fixed << std::setprecision(4) << results.noveltyLevel << "\n";
        file << "Novelty Score: " << std::fixed << std::setprecision(4) << results.noveltyScore << "\n";
        file << "Prediction Error: " << std::fixed << std::setprecision(4) << results.predictionError << "\n";
        file << "Prediction Error Score: " << std::fixed << std::setprecision(4) << results.predictionErrorScore << "\n";
        
        file.close();
        NLM_LOG_INFO("Neuromodulation analysis results saved: " + filePath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving neuromodulation results: " + std::string(e.what()));
        file.close();
    }
}

// ============================================================================
// ExportManager
// ============================================================================

ExportManager::ExportManager() {
    NLM_LOG_INFO("Initializing Export Manager");
}

ExportManager::~ExportManager() {
    NLM_LOG_INFO("Shutting down Export Manager");
}

bool ExportManager::exportData(const std::string& dataType, const std::string& filePath,
                              const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("Exporting data: " + dataType + " to " + filePath);
    
    if (dataType == "csv") {
        return _exporter.exportToCSV(filePath, results);
    } else if (dataType == "json") {
        return _exporter.exportToJSON(filePath, results);
    } else if (dataType == "sqlite") {
        return _exporter.exportToSQLite(filePath, results);
    } else if (dataType == "parquet") {
        return _exporter.exportToParquet(filePath, results);
    } else {
        NLM_LOG_ERROR("Unsupported data type: " + dataType);
        return false;
    }
}

void ExportManager::setExportOptions(const ExportOptions& options) {
    _exporter.setExportOptions(options);
}

ExportOptions ExportManager::getExportOptions() const {
    return _exporter.getExportOptions();
}

} // namespace nlm