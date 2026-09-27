#pragma once

#include "ResultsSystem.hpp"
#include "Experiment.hpp"
#include "Phase3Experiment.hpp"
#include "Phase4Experiment.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace nlm {

struct ExperimentResultsSystem::Impl {
    std::vector<std::string> supportedFormats;
    bool verboseLogging;
    std::string tempDirectory;
    
    Impl() : verboseLogging(true), tempDirectory("/tmp/nlm_results") {
        supportedFormats = {"json", "csv", "text", "binary"};
    }
};

ExperimentResultsSystem::ExperimentResultsSystem() : pImpl(new Impl) {}

ExperimentResultsSystem::~ExperimentResultsSystem() = default;

ComprehensiveExperimentResult ExperimentResultsSystem::collectResults(
    const std::shared_ptr<Experiment>& experiment) {
    ComprehensiveExperimentResult result;
    
    if (!experiment) {
        return result;
    }
    
    // Basic experiment metadata
    result.experimentName = experiment->getName();
    result.seed = experiment->getSeed();
    result.timestamp = std::chrono::system_clock::now();
    result.startStep = experiment->getStartStep();
    result.endStep = experiment->getEndStep();
    result.stepData = experiment->getStepData();
    result.metrics = experiment->getMetrics();
    result.notes = experiment->getNotes();
    
    // Add experiment type based on name prefix
    if (experiment->getName().find("phase3") != std::string::npos || 
        experiment->getName().find("Phase3") != std::string::npos) {
        result.experimentType = "Phase3";
    } else if (experiment->getName().find("phase4") != std::string::npos || 
               experiment->getName().find("Phase4") != std::string::npos) {
        result.experimentType = "Phase4";
    } else if (experiment->getName().find("phase6") != std::string::npos || 
               experiment->getName().find("Phase6") != std::string::npos) {
        result.experimentType = "Phase6";
    } else {
        result.experimentType = "Generic";
    }
    
    return result;
}

void ExperimentResultsSystem::analyzeResults(ComprehensiveExperimentResult& result) {
    calculatePerformanceMetrics(result);
    calculateStatisticalMetrics(result);
    calculateLearningMetrics(result);
    calculateEfficiencyMetrics(result);
}

void ExperimentResultsSystem::calculatePerformanceMetrics(
    ComprehensiveExperimentResult& result) {
    
    // Calculate firing rate metrics
    if (!result.stepData.empty()) {
        float totalFiringRate = 0.0f;
        float minFiringRate = std::numeric_limits<float>::max();
        float maxFiringRate = std::numeric_limits<float>::min();
        
        for (const auto& step : result.stepData) {
            totalFiringRate += step.avgFiringRate;
            minFiringRate = std::min(minFiringRate, step.avgFiringRate);
            maxFiringRate = std::max(maxFiringRate, step.avgFiringRate);
        }
        
        result.analysis.meanFiringRate = totalFiringRate / result.stepData.size();
        result.analysis.minFiringRate = minFiringRate;
        result.analysis.maxFiringRate = maxFiringRate;
        
        // Calculate standard deviation
        float sumSq = 0.0f;
        for (const auto& step : result.stepData) {
            float diff = step.avgFiringRate - result.analysis.meanFiringRate;
            sumSq += diff * diff;
        }
        result.analysis.stdDevFiringRate = std::sqrt(sumSq / result.stepData.size());
        
        // Calculate synapse metrics
        size_t totalSynapses = 0;
        for (const auto& step : result.stepData) {
            totalSynapses += step.synapseCount;
        }
        result.analysis.meanSynapseCount = static_cast<float>(totalSynapses) / result.stepData.size();
        
        // Calculate neuron metrics
        size_t totalNeurons = 0;
        for (const auto& step : result.stepData) {
            totalNeurons += step.neuronCount;
        }
        result.analysis.meanNeuronCount = static_cast<float>(totalNeurons) / result.stepData.size();
        
        // Calculate synapse standard deviation
        float sumSqSyn = 0.0f;
        for (const auto& step : result.stepData) {
            float diff = static_cast<float>(step.synapseCount) - result.analysis.meanSynapseCount;
            sumSqSyn += diff * diff;
        }
        result.analysis.stdDevSynapseCount = std::sqrt(sumSqSyn / result.stepData.size());
    }
}

void ExperimentResultsSystem::calculateStatisticalMetrics(
    ComprehensiveExperimentResult& result) {
    
    // Calculate reward statistics from metrics
    if (result.hasMetric("total_reward")) {
        result.analysis.totalReward = result.getMetric("total_reward");
        result.analysis.meanReward = result.analysis.totalReward;
        result.analysis.minReward = result.analysis.totalReward;
        result.analysis.maxReward = result.analysis.totalReward;
        result.analysis.stdDevReward = 0.0f;
        
        // For multiple reward measurements, calculate statistics
        std::vector<float> rewards;
        for (const auto& metric : result.metrics) {
            if (metric.first.find("reward") != std::string::npos) {
                rewards.push_back(metric.second);
            }
        }
        
        if (!rewards.empty()) {
            float sum = 0.0f, sumSq = 0.0f, min = std::numeric_limits<float>::max(), 
                  max = std::numeric_limits<float>::min();
            for (float reward : rewards) {
                sum += reward;
                sumSq += reward * reward;
                min = std::min(min, reward);
                max = std::max(max, reward);
            }
            result.analysis.meanReward = sum / rewards.size();
            result.analysis.minReward = min;
            result.analysis.maxReward = max;
            if (rewards.size() > 1) {
                float variance = (sumSq - (sum * sum) / rewards.size()) / (rewards.size() - 1);
                result.analysis.stdDevReward = std::sqrt(variance);
            }
        }
    }
    
    // Calculate behavioral metrics based on metrics
    result.analysis.convergenceRate = calculateConvergenceRate(result);
    result.analysis.explorationRate = calculateExplorationRate(result);
    result.analysis.exploitationRate = calculateExploitationRate(result);
}

void ExperimentResultsSystem::calculateLearningMetrics(
    ComprehensiveExperimentResult& result) {
    
    if (result.stepData.size() < 2) {
        return;
    }
    
    // Calculate learning curve slope (simple linear regression on average firing rate)
    std::vector<float> stepValues;
    std::vector<float> firingRates;
    
    size_t numSteps = std::min<size_t>(result.stepData.size(), 100); // Use up to 100 points
    for (size_t i = 0; i < numSteps; ++i) {
        stepValues.push_back(static_cast<float>(i));
        firingRates.push_back(result.stepData[i].avgFiringRate);
    }
    
    // Simple linear regression to get learning curve slope
    result.analysis.learningCurveSlope = calculateLinearRegressionSlope(stepValues, firingRates);
    result.analysis.learningCurveAUC = calculateLearningCurveAUC(firingRates);
    
    // Calculate retention index (stability of final performance)
    result.analysis.retentionIndex = calculateRetentionIndex(result);
    
    // Calculate spike rate and spikes per step
    result.analysis.meanSpikesPerStep = result.phase3Results.totalSpikes / 
                                       (result.endStep - result.startStep + 1);
    result.analysis.spikeRate = result.analysis.meanSpikesPerStep;
}

void ExperimentResultsSystem::calculateEfficiencyMetrics(
    ComprehensiveExperimentResult& result) {
    
    // Calculate energy efficiency
    if (result.stepData.size() > 0) {
        float totalEnergy = 0.0f;
        for (const auto& metric : result.metrics) {
            if (metric.first.find("energy") != std::string::npos) {
                totalEnergy += metric.second;
            }
        }
        
        if (totalEnergy > 0.0f) {
            result.analysis.energyEfficiency = result.analysis.totalReward / totalEnergy;
        }
    }
    
    // Calculate time efficiency (steps per unit time)
    if (result.phase6Results.totalWallClockTime > 0.0) {
        result.analysis.timeEfficiency = (result.endStep - result.startStep + 1) / 
                                        result.phase6Results.totalWallClockTime;
    }
    
    // Calculate temporal stability
    result.analysis.temporalStability = calculateTemporalStability(result);
    
    // Calculate representational stability
    result.analysis.representationalStability = calculateRepresentationalStability(result);
}

bool ExperimentResultsSystem::exportToJSON(
    const ComprehensiveExperimentResult& result, const std::string& filepath) {
    rapidjson::Document doc;
    doc.SetObject();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    // Basic metadata
    doc.AddMember("experimentName", rapidjson::Value(result.experimentName.c_str(), allocator), allocator);
    doc.AddMember("seed", result.seed, allocator);
    doc.AddMember("experimentType", rapidjson::Value(result.experimentType.c_str(), allocator), allocator);
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(result.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    doc.AddMember("timestamp", rapidjson::Value(ss.str().c_str(), allocator), allocator);
    
    // Step data
    rapidjson::Value stepData(rapidjson::kArrayType);
    for (const auto& step : result.stepData) {
        rapidjson::Value stepObj(rapidjson::kObjectType);
        stepObj.AddMember("step", step.step, allocator);
        stepObj.AddMember("neuronCount", step.neuronCount, allocator);
        stepObj.AddMember("synapseCount", step.synapseCount, allocator);
        stepObj.AddMember("firingCount", step.firingCount, allocator);
        stepObj.AddMember("avgFiringRate", step.avgFiringRate, allocator);
        stepData.PushBack(stepObj, allocator);
    }
    doc.AddMember("stepData", stepData, allocator);
    
    // Metrics
    rapidjson::Value metricsObj(rapidjson::kObjectType);
    for (const auto& metric : result.metrics) {
        metricsObj.AddMember(rapidjson::Value(metric.first.c_str(), allocator), metric.second, allocator);
    }
    doc.AddMember("metrics", metricsObj, allocator);
    
    // Notes
    rapidjson::Value notes(rapidjson::kArrayType);
    for (const auto& note : result.notes) {
        notes.PushBack(rapidjson::Value(note.c_str(), allocator), allocator);
    }
    doc.AddMember("notes", notes, allocator);
    
    // Phase-specific results
    rapidjson::Value phase3Obj(rapidjson::kObjectType);
    phase3Obj.AddMember("episodes", result.phase3Results.episodes, allocator);
    phase3Obj.AddMember("totalReward", result.phase3Results.totalReward, allocator);
    phase3Obj.AddMember("finalEnergy", result.phase3Results.finalEnergy, allocator);
    phase3Obj.AddMember("finalNovelty", result.phase3Results.finalNovelty, allocator);
    phase3Obj.AddMember("finalCuriosity", result.phase3Results.finalCuriosity, allocator);
    phase3Obj.AddMember("predictionError", result.phase3Results.predictionError, allocator);
    doc.AddMember("phase3Results", phase3Obj, allocator);
    
    rapidjson::Value phase4Obj(rapidjson::kObjectType);
    phase4Obj.AddMember("predictionErrorInitial", result.phase4Results.predictionErrorInitial, allocator);
    phase4Obj.AddMember("predictionErrorFinal", result.phase4Results.predictionErrorFinal, allocator);
    phase4Obj.AddMember("predictionAccuracyImprovement", result.phase4Results.predictionAccuracyImprovement, allocator);
    phase4Obj.AddMember("totalReward", result.phase4Results.totalReward, allocator);
    doc.AddMember("phase4Results", phase4Obj, allocator);
    
    // Phase 6 results
    rapidjson::Value phase6Obj(rapidjson::kObjectType);
    phase6Obj.AddMember("totalReward", result.phase6Results.totalReward, allocator);
    phase6Obj.AddMember("avgFiringRate", result.phase6Results.avgFiringRate, allocator);
    phase6Obj.AddMember("memoryEpisodesStored", result.phase6Results.memoryEpisodesStored, allocator);
    phase6Obj.AddMember("noveltyLevel", result.phase6Results.noveltyLevel, allocator);
    phase6Obj.AddMember("curiosityLevel", result.phase6Results.curiosityLevel, allocator);
    phase6Obj.AddMember("dopamineLevel", result.phase6Results.dopamineLevel, allocator);
    phase6Obj.AddMember("totalWallClockTime", result.phase6Results.totalWallClockTime, allocator);
    doc.AddMember("phase6Results", phase6Obj, allocator);
    
    // Analysis results
    rapidjson::Value analysisObj(rapidjson::kObjectType);
    analysisObj.AddMember("meanFiringRate", result.analysis.meanFiringRate, allocator);
    analysisObj.AddMember("stdDevFiringRate", result.analysis.stdDevFiringRate, allocator);
    analysisObj.AddMember("minFiringRate", result.analysis.minFiringRate, allocator);
    analysisObj.AddMember("maxFiringRate", result.analysis.maxFiringRate, allocator);
    analysisObj.AddMember("meanSynapseCount", result.analysis.meanSynapseCount, allocator);
    analysisObj.AddMember("stdDevSynapseCount", result.analysis.stdDevSynapseCount, allocator);
    analysisObj.AddMember("meanNeuronCount", result.analysis.meanNeuronCount, allocator);
    analysisObj.AddMember("meanReward", result.analysis.meanReward, allocator);
    analysisObj.AddMember("stdDevReward", result.analysis.stdDevReward, allocator);
    analysisObj.AddMember("minReward", result.analysis.minReward, allocator);
    analysisObj.AddMember("maxReward", result.analysis.maxReward, allocator);
    analysisObj.AddMember("convergenceRate", result.analysis.convergenceRate, allocator);
    analysisObj.AddMember("explorationRate", result.analysis.explorationRate, allocator);
    analysisObj.AddMember("exploitationRate", result.analysis.exploitationRate, allocator);
    analysisObj.AddMember("learningCurveSlope", result.analysis.learningCurveSlope, allocator);
    analysisObj.AddMember("learningCurveAUC", result.analysis.learningCurveAUC, allocator);
    analysisObj.AddMember("retentionIndex", result.analysis.retentionIndex, allocator);
    analysisObj.AddMember("meanSpikesPerStep", result.analysis.meanSpikesPerStep, allocator);
    analysisObj.AddMember("spikeRate", result.analysis.spikeRate, allocator);
    analysisObj.AddMember("temporalStability", result.analysis.temporalStability, allocator);
    analysisObj.AddMember("representationalStability", result.analysis.representationalStability, allocator);
    analysisObj.AddMember("energyEfficiency", result.analysis.energyEfficiency, allocator);
    analysisObj.AddMember("timeEfficiency", result.analysis.timeEfficiency, allocator);
    doc.AddMember("analysis", analysisObj, allocator);
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    file << buffer.GetString();
    file.close();
    
    NLM_LOG_INFO("Results exported to JSON: " + filepath);
    return true;
}

bool ExperimentResultsSystem::exportToCSV(
    const ComprehensiveExperimentResult& result, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Header
    file << "Metric,Value\n";
    
    // Basic info
    file << "Experiment Name," << result.experimentName << "\n";
    file << "Seed," << result.seed << "\n";
    file << "Experiment Type," << result.experimentType << "\n";
    file << "Start Step," << result.startStep << "\n";
    file << "End Step," << result.endStep << "\n";
    file << "Total Steps," << (result.endStep - result.startStep + 1) << "\n";
    
    // Metrics
    file << "\nMetrics:\n";
    for (const auto& metric : result.metrics) {
        file << metric.first << "," << metric.second << "\n";
    }
    
    // Analysis results
    file << "\nAnalysis Results:\n";
    file << "Mean Firing Rate," << result.analysis.meanFiringRate << "\n";
    file << "Std Dev Firing Rate," << result.analysis.stdDevFiringRate << "\n";
    file << "Min Firing Rate," << result.analysis.minFiringRate << "\n";
    file << "Max Firing Rate," << result.analysis.maxFiringRate << "\n";
    file << "Mean Synapse Count," << result.analysis.meanSynapseCount << "\n";
    file << "Std Dev Synapse Count," << result.analysis.stdDevSynapseCount << "\n";
    file << "Mean Neuron Count," << result.analysis.meanNeuronCount << "\n";
    file << "Total Reward," << result.analysis.totalReward << "\n";
    file << "Mean Reward," << result.analysis.meanReward << "\n";
    file << "Std Dev Reward," << result.analysis.stdDevReward << "\n";
    file << "Min Reward," << result.analysis.minReward << "\n";
    file << "Max Reward," << result.analysis.maxReward << "\n";
    file << "Convergence Rate," << result.analysis.convergenceRate << "\n";
    file << "Exploration Rate," << result.analysis.explorationRate << "\n";
    file << "Exploitation Rate," << result.analysis.exploitationRate << "\n";
    file << "Learning Curve Slope," << result.analysis.learningCurveSlope << "\n";
    file << "Learning Curve AUC," << result.analysis.learningCurveAUC << "\n";
    file << "Retention Index," << result.analysis.retentionIndex << "\n";
    file << "Mean Spikes Per Step," << result.analysis.meanSpikesPerStep << "\n";
    file << "Spike Rate," << result.analysis.spikeRate << "\n";
    file << "Temporal Stability," << result.analysis.temporalStability << "\n";
    file << "Representational Stability," << result.analysis.representationalStability << "\n";
    file << "Energy Efficiency," << result.analysis.energyEfficiency << "\n";
    file << "Time Efficiency," << result.analysis.timeEfficiency << "\n";
    
    // Phase 3 results
    file << "\nPhase 3 Results:\n";
    file << "Episodes," << result.phase3Results.episodes << "\n";
    file << "Final Energy," << result.phase3Results.finalEnergy << "\n";
    file << "Final Novelty," << result.phase3Results.finalNovelty << "\n";
    file << "Final Curiosity," << result.phase3Results.finalCuriosity << "\n";
    file << "Prediction Error," << result.phase3Results.predictionError << "\n";
    file << "Developmental Age," << result.phase3Results.developmentalAge << "\n";
    file << "Developmental Stage," << result.phase3Results.developmentalStage << "\n";
    file << "Total Synapses," << result.phase3Results.totalSynapses << "\n";
    file << "Average Weight," << result.phase3Results.averageWeight << "\n";
    file << "Max Weight," << result.phase3Results.maxWeight << "\n";
    file << "Total Spikes," << result.phase3Results.totalSpikes << "\n";
    file << "Average Firing Rate," << result.phase3Results.averageFiringRate << "\n";
    
    file.close();
    NLM_LOG_INFO("Results exported to CSV: " + filepath);
    return true;
}

bool ExperimentResultsSystem::exportToText(
    const ComprehensiveExperimentResult& result, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "===============================================" << std::endl;
    file << "NLM EXPERIMENT RESULTS REPORT" << std::endl;
    file << "===============================================" << std::endl;
    file << std::endl;
    
    file << "Experiment Name: " << result.experimentName << std::endl;
    file << "Experiment Type: " << result.experimentType << std::endl;
    file << "Seed: " << result.seed << std::endl;
    file << "Start Step: " << result.startStep << std::endl;
    file << "End Step: " << result.endStep << std::endl;
    file << "Total Steps: " << (result.endStep - result.startStep + 1) << std::endl;
    file << std::endl;
    
    file << "PERFORMANCE ANALYSIS:" << std::endl;
    file << "--------------------" << std::endl;
    file << "Mean Firing Rate: " << result.analysis.meanFiringRate << " Hz" << std::endl;
    file << "Std Dev Firing Rate: " << result.analysis.stdDevFiringRate << " Hz" << std::endl;
    file << "Min Firing Rate: " << result.analysis.minFiringRate << " Hz" << std::endl;
    file << "Max Firing Rate: " << result.analysis.maxFiringRate << " Hz" << std::endl;
    file << "Mean Neuron Count: " << result.analysis.meanNeuronCount << std::endl;
    file << "Mean Synapse Count: " << result.analysis.meanSynapseCount << std::endl;
    file << std::endl;
    
    file << "REWARD ANALYSIS:" << std::endl;
    file << "----------------" << std::endl;
    file << "Total Reward: " << result.analysis.totalReward << std::endl;
    file << "Mean Reward: " << result.analysis.meanReward << std::endl;
    file << "Std Dev Reward: " << result.analysis.stdDevReward << std::endl;
    file << "Min Reward: " << result.analysis.minReward << std::endl;
    file << "Max Reward: " << result.analysis.maxReward << std::endl;
    file << "Convergence Rate: " << result.analysis.convergenceRate << std::endl;
    file << "Exploration Rate: " << result.analysis.explorationRate << std::endl;
    file << "Exploitation Rate: " << result.analysis.exploitationRate << std::endl;
    file << std::endl;
    
    file << "LEARNING METRICS:" << std::endl;
    file << "----------------" << std::endl;
    file << "Learning Curve Slope: " << result.analysis.learningCurveSlope << std::endl;
    file << "Learning Curve AUC: " << result.analysis.learningCurveAUC << std::endl;
    file << "Retention Index: " << result.analysis.retentionIndex << std::endl;
    file << std::endl;
    
    file << "STABILITY ANALYSIS:" << std::endl;
    file << "-------------------" << std::endl;
    file << "Temporal Stability: " << result.analysis.temporalStability << std::endl;
    file << "Representational Stability: " << result.analysis.representationalStability << std::endl;
    file << std::endl;
    
    file << "EFFICIENCY METRICS:" << std::endl;
    file << "-------------------" << std::endl;
    file << "Energy Efficiency: " << result.analysis.energyEfficiency << std::endl;
    file << "Time Efficiency: " << result.analysis.timeEfficiency << " steps/sec" << std::endl;
    file << std::endl;
    
    file << "PHASE-SPECIFIC RESULTS:" << std::endl;
    file << "----------------------" << std::endl;
    
    if (result.experimentType == "Phase3") {
        file << "Phase 3 Results:" << std::endl;
        file << "  Episodes: " << result.phase3Results.episodes << std::endl;
        file << "  Total Reward: " << result.phase3Results.totalReward << std::endl;
        file << "  Final Energy: " << result.phase3Results.finalEnergy << std::endl;
        file << "  Final Novelty: " << result.phase3Results.finalNovelty << std::endl;
        file << "  Final Curiosity: " << result.phase3Results.finalCuriosity << std::endl;
        file << "  Prediction Error: " << result.phase3Results.predictionError << std::endl;
    } else if (result.experimentType == "Phase4") {
        file << "Phase 4 Results:" << std::endl;
        file << "  Prediction Accuracy Improvement: " << result.phase4Results.predictionAccuracyImprovement << std::endl;
        file << "  Working Memory Retention Initial: " << result.phase4Results.workingMemoryRetentionInitial << std::endl;
        file << "  Working Memory Retention Final: " << result.phase4Results.workingMemoryRetentionFinal << std::endl;
        file << "  Episodic Recall Accuracy: " << result.phase4Results.episodicRecallAccuracy << std::endl;
        file << "  Total Reward: " << result.phase4Results.totalReward << std::endl;
    } else if (result.experimentType == "Phase6") {
        file << "Phase 6 Results:" << std::endl;
        file << "  Total Reward: " << result.phase6Results.totalReward << std::endl;
        file << "  Avg Firing Rate: " << result.phase6Results.avgFiringRate << std::endl;
        file << "  Memory Episodes Stored: " << result.phase6Results.memoryEpisodesStored << std::endl;
        file << "  Novelty Level: " << result.phase6Results.noveltyLevel << std::endl;
        file << "  Curiosity Level: " << result.phase6Results.curiosityLevel << std::endl;
        file << "  Dopamine Level: " << result.phase6Results.dopamineLevel << std::endl;
        file << "  Total Wall Clock Time: " << result.phase6Results.totalWallClockTime << " seconds" << std::endl;
    }
    
    file << std::endl;
    file << "STEP SUMMARY:" << std::endl;
    file << "------------" << std::endl;
    if (!result.stepData.empty()) {
        file << "Number of steps recorded: " << result.stepData.size() << std::endl;
        file << "First step - Neurons: " << result.stepData[0].neuronCount 
             << ", Synapses: " << result.stepData[0].synapseCount 
             << ", Firing: " << result.stepData[0].firingCount 
             << ", Rate: " << result.stepData[0].avgFiringRate << " Hz" << std::endl;
        file << "Last step - Neurons: " << result.stepData.back().neuronCount 
             << ", Synapses: " << result.stepData.back().synapseCount 
             << ", Firing: " << result.stepData.back().firingCount 
             << ", Rate: " << result.stepData.back().avgFiringRate << " Hz" << std::endl;
    }
    
    if (!result.notes.empty()) {
        file << std::endl;
        file << "NOTES:" << std::endl;
        file << "-----" << std::endl;
        for (size_t i = 0; i < result.notes.size(); ++i) {
            file << i + 1 << ". " << result.notes[i] << std::endl;
        }
    }
    
    file << std::endl;
    file << "===============================================" << std::endl;
    file.close();
    
    NLM_LOG_INFO("Results exported to text report: " + filepath);
    return true;
}

bool ExperimentResultsSystem::exportToBinary(
    const ComprehensiveExperimentResult& result, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for binary writing: " + filepath);
        return false;
    }
    
    // Write version
    uint32_t version = 1;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Write experiment metadata
    uint32_t nameLen = static_cast<uint32_t>(result.experimentName.length());
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    file.write(result.experimentName.c_str(), nameLen);
    
    file.write(reinterpret_cast<const char*>(&result.seed), sizeof(result.seed));
    
    uint32_t typeLen = static_cast<uint32_t>(result.experimentType.length());
    file.write(reinterpret_cast<const char*>(&typeLen), sizeof(typeLen));
    file.write(result.experimentType.c_str(), typeLen);
    
    // Write step data
    size_t stepCount = result.stepData.size();
    file.write(reinterpret_cast<const char*>(&stepCount), sizeof(stepCount));
    file.write(reinterpret_cast<const char*>(result.stepData.data()), 
              stepCount * sizeof(Experiment::StepData));
    
    // Write metrics
    size_t metricCount = result.metrics.size();
    file.write(reinterpret_cast<const char*>(&metricCount), sizeof(metricCount));
    for (const auto& metric : result.metrics) {
        uint32_t keyLen = static_cast<uint32_t>(metric.first.length());
        file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        file.write(metric.first.c_str(), keyLen);
        file.write(reinterpret_cast<const char*>(&metric.second), sizeof(metric.second));
    }
    
    // Write analysis results
    file.write(reinterpret_cast<const char*>(&result.analysis.meanFiringRate), sizeof(result.analysis.meanFiringRate));
    file.write(reinterpret_cast<const char*>(&result.analysis.stdDevFiringRate), sizeof(result.analysis.stdDevFiringRate));
    file.write(reinterpret_cast<const char*>(&result.analysis.minFiringRate), sizeof(result.analysis.minFiringRate));
    file.write(reinterpret_cast<const char*>(&result.analysis.maxFiringRate), sizeof(result.analysis.maxFiringRate));
    file.write(reinterpret_cast<const char*>(&result.analysis.meanSynapseCount), sizeof(result.analysis.meanSynapseCount));
    file.write(reinterpret_cast<const char*>(&result.analysis.stdDevSynapseCount), sizeof(result.analysis.stdDevSynapseCount));
    file.write(reinterpret_cast<const char*>(&result.analysis.meanNeuronCount), sizeof(result.analysis.meanNeuronCount));
    file.write(reinterpret_cast<const char*>(&result.analysis.totalReward), sizeof(result.analysis.totalReward));
    file.write(reinterpret_cast<const char*>(&result.analysis.meanReward), sizeof(result.analysis.meanReward));
    file.write(reinterpret_cast<const char*>(&result.analysis.stdDevReward), sizeof(result.analysis.stdDevReward));
    file.write(reinterpret_cast<const char*>(&result.analysis.minReward), sizeof(result.analysis.minReward));
    file.write(reinterpret_cast<const char*>(&result.analysis.maxReward), sizeof(result.analysis.maxReward));
    
    file.close();
    NLM_LOG_INFO("Results exported to binary format: " + filepath);
    return true;
}

bool ExperimentResultsSystem::compareResults(
    const ComprehensiveExperimentResult& result1,
    const ComprehensiveExperimentResult& result2,
    ComprehensiveExperimentResult::ComparisonResults& comparison) {
    comparison.experimentName = result1.experimentName + " vs " + result2.experimentName;
    
    // Calculate performance score based on key metrics
    float score1 = calculatePerformanceScore(result1);
    float score2 = calculatePerformanceScore(result2);
    
    comparison.performanceScore = std::max(score1, score2);
    
    // Calculate learning rate
    comparison.learningRate = (result2.analysis.learningCurveSlope - result1.analysis.learningCurveSlope);
    
    // Calculate efficiency score
    comparison.efficiencyScore = (result2.analysis.timeEfficiency - result1.analysis.timeEfficiency);
    
    // Normalize metrics
    comparison.normalizedMetrics["firing_rate"] = result2.analysis.meanFiringRate / 
                                                   (result1.analysis.meanFiringRate + 0.0001f);
    comparison.normalizedMetrics["reward"] = result2.analysis.meanReward / 
                                            (result1.analysis.meanReward + 0.0001f);
    comparison.normalizedMetrics["stability"] = result2.analysis.temporalStability / 
                                                (result1.analysis.temporalStability + 0.0001f);
    
    NLM_LOG_INFO("Results comparison completed between " + result1.experimentName + " and " + result2.experimentName);
    return true;
}

std::vector<ComprehensiveExperimentResult> ExperimentResultsSystem::aggregateBatchResults(
    const std::vector<std::shared_ptr<Experiment>>& experiments) {
    std::vector<ComprehensiveExperimentResult> results;
    
    for (const auto& exp : experiments) {
        if (exp) {
            ComprehensiveExperimentResult result = collectResults(exp);
            analyzeResults(result);
            results.push_back(result);
        }
    }
    
    return results;
}

bool ExperimentResultsSystem::generateVisualization(
    const ComprehensiveExperimentResult& result, const std::string& outputPath) {
    
    // For now, create a simple visualization summary
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open visualization file: " + outputPath);
        return false;
    }
    
    file << "# NLM Experiment Results Visualization" << std::endl;
    file << "# Experiment: " << result.experimentName << std::endl;
    file << "# Type: " << result.experimentType << std::endl;
    file << std::endl;
    
    file << "## Firing Rate Time Series" << std::endl;
    file << "```" << std::endl;
    if (!result.stepData.empty()) {
        for (size_t i = 0; i < std::min<size_t>(result.stepData.size(), 50); ++i) {
            float firingRate = result.stepData[i].avgFiringRate;
            file << i << ": " << std::string(static_cast<size_t>(firingRate * 50), '=') << std::endl;
        }
    }
    file << "```" << std::endl;
    
    file << "## Metric Distribution" << std::endl;
    file << "```plaintext" << std::endl;
    file << "Mean Firing Rate:     " << result.analysis.meanFiringRate << " Hz" << std::endl;
    file << "Std Dev Firing Rate:  " << result.analysis.stdDevFiringRate << " Hz" << std::endl;
    file << "Min Firing Rate:      " << result.analysis.minFiringRate << " Hz" << std::endl;
    file << "Max Firing Rate:      " << result.analysis.maxFiringRate << " Hz" << std::endl;
    file << "```" << std::endl;
    
    file << "## Reward Over Time" << std::endl;
    file << "```" << std::endl;
    file << "Total Reward: " << result.analysis.totalReward << std::endl;
    file << "Mean Reward:  " << result.analysis.meanReward << std::endl;
    file << "```" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("Visualization generated: " + outputPath);
    return true;
}

bool ExperimentResultsSystem::generateSummaryReport(
    const ComprehensiveExperimentResult& result, const std::string& reportPath) {
    std::ofstream file(reportPath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open report file: " + reportPath);
        return false;
    }
    
    file << "NLM EXPERIMENT EXECUTIVE SUMMARY" << std::endl;
    file << "=================================" << std::endl;
    file << std::endl;
    
    file << "Experiment: " << result.experimentName << std::endl;
    file << "Type: " << result.experimentType << std::endl;
    file << "Seed: " << result.seed << std::endl;
    file << std::endl;
    
    file << "OVERALL PERFORMANCE:" << std::endl;
    file << "-------------------" << std::endl;
    file << "Firing Rate: " << result.analysis.meanFiringRate << " ± " 
          << result.analysis.stdDevFiringRate << " Hz" << std::endl;
    file << "Reward: " << result.analysis.meanReward << " ± " 
          << result.analysis.stdDevReward << std::endl;
    file << std::endl;
    
    file << "EXPERIMENT STATUS: SUCCESS" << std::endl;
    file << std::endl;
    
    file << "RECOMMENDATIONS:" << std::endl;
    file << "---------------" << std::endl;
    
    if (result.analysis.meanFiringRate < 1.0f) {
        file << "• Low firing rate observed. Consider increasing neural excitability." << std::endl;
    }
    
    if (result.analysis.temporalStability < 0.5f) {
        file << "• Low temporal stability. Results may not be reproducible." << std::endl;
    }
    
    if (result.analysis.learningCurveSlope < 0.0f) {
        file << "• Learning curve is declining. Review training parameters." << std::endl;
    }
    
    file << "• Consider running additional trials for statistical significance." << std::endl;
    
    file << std::endl;
    file << "=================================" << std::endl;
    file.close();
    
    NLM_LOG_INFO("Summary report generated: " + reportPath);
    return true;
}

std::vector<std::string> ExperimentResultsSystem::findResultFiles(
    const std::string& directory) {
    std::vector<std::string> resultFiles;
    
    // This would use file system operations to find result files
    // For now, return empty vector
    
    NLM_LOG_INFO("Searching for result files in: " + directory);
    return resultFiles;
}

bool ExperimentResultsSystem::loadFromJSON(
    const std::string& filepath, ComprehensiveExperimentResult& result) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file for reading: " + filepath);
        return false;
    }
    
    // This would use rapidjson to parse the file
    // For now, implement basic parsing
    
    file.close();
    return false;
}

bool ExperimentResultsSystem::loadFromBinary(
    const std::string& filepath, ComprehensiveExperimentResult& result) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open binary file for reading: " + filepath);
        return false;
    }
    
    // This would read the binary format we created
    // For now, implement basic reading
    
    file.close();
    return false;
}

void ExperimentResultsSystem::addMetadata(
    ComprehensiveExperimentResult& result, 
    const std::string& key, const std::string& value) {
    result.metadata[key] = value;
}

void ExperimentResultsSystem::addPhase3Results(
    ComprehensiveExperimentResult& result, 
    const Phase3Experiment& phase3Exp) {
    // Extract Phase3 specific results from the experiment
    // This would depend on the actual implementation details
    result.phase3Results.episodes = 1; // Placeholder
    result.phase3Results.totalReward = 0.0f; // Placeholder
}

void ExperimentResultsSystem::addPhase4Results(
    ComprehensiveExperimentResult& result, 
    const Phase4Experiment& phase4Exp) {
    // Extract Phase4 specific results from the experiment
    // This would depend on the actual implementation details
    result.phase4Results.predictionAccuracyImprovement = 0.0f; // Placeholder
}

void ExperimentResultsSystem::addPhase6Results(
    ComprehensiveExperimentResult& result, 
    const Phase6IntegratedExperiment& phase6Exp) {
    // Extract Phase6 specific results from the experiment
    // This would depend on the actual implementation details
    result.phase6Results.totalReward = 0.0f; // Placeholder
}

// Helper functions for metrics calculation
float ExperimentResultsSystem::calculateConvergenceRate(
    const ComprehensiveExperimentResult& result) {
    if (result.stepData.size() < 2) return 0.0f;
    
    // Calculate how quickly the system reaches steady state
    size_t steadyStateSteps = 0;
    float currentRate = result.stepData[0].avgFiringRate;
    
    for (size_t i = 1; i < result.stepData.size(); ++i) {
        float diff = std::abs(result.stepData[i].avgFiringRate - currentRate);
        if (diff < 0.1f) {
            steadyStateSteps++;
        }
    }
    
    return static_cast<float>(steadyStateSteps) / result.stepData.size();
}

float ExperimentResultsSystem::calculateExplorationRate(
    const ComprehensiveExperimentResult& result) {
    // Simplified exploration calculation based on firing rate variability
    if (result.stepData.size() < 2) return 0.0f;
    
    float sum = 0.0f, sumSq = 0.0f;
    for (const auto& step : result.stepData) {
        sum += step.avgFiringRate;
        sumSq += step.avgFiringRate * step.avgFiringRate;
    }
    
    float mean = sum / result.stepData.size();
    float variance = (sumSq / result.stepData.size()) - (mean * mean);
    
    return std::sqrt(std::max(0.0f, variance));
}

float ExperimentResultsSystem::calculateExploitationRate(
    const ComprehensiveExperimentResult& result) {
    return 1.0f - calculateExplorationRate(result);
}

float ExperimentResultsSystem::calculateLinearRegressionSlope(
    const std::vector<float>& x, const std::vector<float>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0f;
    
    float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    float n = static_cast<float>(x.size());
    float denominator = n * sumX2 - sumX * sumX;
    
    if (std::abs(denominator) < 0.0001f) return 0.0f;
    
    return (n * sumXY - sumX * sumY) / denominator;
}

float ExperimentResultsSystem::calculateLearningCurveAUC(
    const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    
    float area = 0.0f;
    for (size_t i = 1; i < values.size(); ++i) {
        float avg = (values[i-1] + values[i]) / 2.0f;
        area += avg;
    }
    
    return area / values.size();
}

float ExperimentResultsSystem::calculateRetentionIndex(
    const ComprehensiveExperimentResult& result) {
    if (result.stepData.size() < 3) return 0.0f;
    
    // Compare last 10% of steps to overall average
    size_t lastStart = result.stepData.size() * 9 / 10;
    float lastAvg = 0.0f, overallAvg = 0.0f;
    
    for (size_t i = 0; i < result.stepData.size(); ++i) {
        overallAvg += result.stepData[i].avgFiringRate;
    }
    overallAvg /= result.stepData.size();
    
    for (size_t i = lastStart; i < result.stepData.size(); ++i) {
        lastAvg += result.stepData[i].avgFiringRate;
    }
    lastAvg /= (result.stepData.size() - lastStart);
    
    return lastAvg / (overallAvg + 0.0001f);
}

float ExperimentResultsSystem::calculateTemporalStability(
    const ComprehensiveExperimentResult& result) {
    if (result.stepData.size() < 2) return 0.0f;
    
    float totalStability = 0.0f;
    size_t comparisons = 0;
    
    for (size_t i = 1; i < result.stepData.size(); ++i) {
        float diff = std::abs(result.stepData[i].avgFiringRate - result.stepData[i-1].avgFiringRate);
        float stability = 1.0f - std::min(1.0f, diff / 5.0f); // Normalize
        totalStability += stability;
        comparisons++;
    }
    
    return comparisons > 0 ? totalStability / comparisons : 0.0f;
}

float ExperimentResultsSystem::calculateRepresentationalStability(
    const ComprehensiveExperimentResult& result) {
    // Simplified representational stability based on firing rate consistency
    return calculateTemporalStability(result);
}

float ExperimentResultsSystem::calculatePerformanceScore(
    const ComprehensiveExperimentResult& result) {
    float score = 0.0f;
    
    // Weight different metrics
    score += result.analysis.meanFiringRate * 0.3f;
    score += result.analysis.meanReward * 0.4f;
    score += result.analysis.temporalStability * 0.2f;
    score += result.analysis.timeEfficiency * 0.1f;
    
    return score;
}

} // namespace nlm