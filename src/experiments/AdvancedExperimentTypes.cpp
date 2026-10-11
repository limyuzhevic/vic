#pragma once

// AdvancedExperimentTypes.cpp - Main implementation file for advanced experiment types

#include "AdvancedExperimentTypes.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

// =============================================================================
// COMPLEX CONTINUAL LEARNING EXPERIMENT IMPLEMENTATION
// =============================================================================

ComplexContinualLearningExperiment::LearningCurve ComplexContinualLearningExperiment::run(
    std::shared_ptr<Brain> brain,
    const std::vector<TaskDefinition>& tasks,
    size_t cycles,
    bool enableMetaLearning,
    bool enableTransferLearning,
    const std::string& transferTask) {
    
    LearningCurve curve;
    
    try {
        brain->reset();
        brain->initialize();
        
        // Initialize meta-learning if enabled
        if (enableMetaLearning) {
            auto config = brain->getConfig();
            config->set("enable_meta_learning", true, ConfigSource::Runtime);
            config->set("meta_learning_rate", 0.01f, ConfigSource::Runtime);
        }
        
        // Initialize transfer learning if specified
        if (enableTransferLearning && !transferTask.empty()) {
            auto config = brain->getConfig();
            config->set("enable_transfer_learning", true, ConfigSource::Runtime);
            config->set("transfer_task", transferTask, ConfigSource::Runtime);
        }
        
        size_t totalTasksCompleted = 0;
        
        for (size_t cycle = 0; cycle < cycles; ++cycle) {
            NLM_LOG_INFO("Starting continual learning cycle " + std::to_string(cycle + 1) + "/" + std::to_string(cycles));
            
            for (size_t taskIdx = 0; taskIdx < tasks.size(); ++taskIdx) {
                const auto& task = tasks[taskIdx];
                NLM_LOG_INFO("Executing task: " + task.name + " (Cycle " + std::to_string(cycle + 1) + ")");
                
                float preTrainingPerformance = task.evaluateFunction(brain.get(), std::chrono::system_clock::now());
                
                size_t stepsCompleted = 0;
                for (uint64_t step = 0; step < task.stepsBetweenTasks; ++step) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f * task.difficultyLevel);
                    brain->step(step, step * 0.001);
                    stepsCompleted++;
                    
                    float currentPerformance = task.evaluateFunction(brain.get(), std::chrono::system_clock::now());
                    float learningRate = currentPerformance - preTrainingPerformance;
                    
                    curve.addPoint(currentPerformance, learningRate * task.rewardScaling, 
                                  (1.0f - task.difficultyLevel) * 0.5f, stepsCompleted);
                    
                    preTrainingPerformance = currentPerformance;
                }
                
                totalTasksCompleted += stepsCompleted;
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in complex continual learning: " + std::string(e.what()));
    }
    
    return curve;
}

ComplexContinualLearningExperiment::LearningPatternAnalysis ComplexContinualLearningExperiment::analyzeLearningPatterns(
    const LearningCurve& curve) {
    
    LearningPatternAnalysis analysis;
    
    if (curve.performanceScores.empty()) {
        return analysis;
    }
    
    double totalLearning = 0.0;
    size_t count = 0;
    for (size_t i = 1; i < curve.performanceScores.size(); ++i) {
        double learningRate = curve.performanceScores[i] - curve.performanceScores[i-1];
        totalLearning += learningRate;
        count++;
    }
    
    if (count > 0) {
        analysis.averageLearningRate = totalLearning / count;
    }
    
    std::vector<double> learningRates;
    for (size_t i = 1; i < curve.performanceScores.size(); ++i) {
        double learningRate = curve.performanceScores[i] - curve.performanceScores[i-1];
        learningRates.push_back(learningRate);
    }
    
    if (!learningRates.empty()) {
        double sum = std::accumulate(learningRates.begin(), learningRates.end(), 0.0);
        double mean = sum / learningRates.size();
        
        double sqSum = 0.0;
        for (double rate : learningRates) {
            double diff = rate - mean;
            sqSum += diff * diff;
        }
        
        double variance = sqSum / learningRates.size();
        double stddev = std::sqrt(variance);
        
        if (mean > 0) {
            analysis.learningStability = 1.0 / (1.0 + stddev / mean);
        }
    }
    
    if (!curve.taskCompletions.empty()) {
        size_t totalTasks = *std::max_element(curve.taskCompletions.begin(), curve.taskCompletions.end());
        if (totalTasks > 0) {
            analysis.taskSpecificLearningRates.push_back(curve.performanceScores.back() / totalTasks);
        }
    }
    
    if (!curve.performanceScores.empty()) {
        double finalPerformance = curve.performanceScores.back();
        double initialPerformance = curve.performanceScores.front();
        analysis.overallLearningCurveScore = finalPerformance / (initialPerformance + 0.001);
    }
    
    return analysis;
}

std::string ComplexContinualLearningExperiment::generateLearningReport(
    const LearningCurve& curve,
    const LearningPatternAnalysis& analysis) {
    
    std::ostringstream report;
    report << "=== Complex Continual Learning Experiment Report ===\n\n";
    report << "Learning Curve Summary:\n";
    report << "  Total Time Points: " << curve.timestamps.size() << "\n";
    report << "  Initial Performance: " << curve.performanceScores.front() << "\n";
    report << "  Final Performance: " << curve.performanceScores.back() << "\n";
    report << "  Average Performance: " << std::accumulate(curve.performanceScores.begin(), 
                                   curve.performanceScores.end(), 0.0) / curve.performanceScores.size() << "\n";
    report << "  Total Reward Accumulated: " << std::accumulate(curve.rewardRates.begin(), 
                                   curve.rewardRates.end(), 0.0) << "\n";
    report << "  Average Exploration Rate: " << std::accumulate(curve.explorationRates.begin(), 
                                   curve.explorationRates.end(), 0.0) / curve.explorationRates.size() << "\n\n";
    
    report << "Learning Pattern Analysis:\n";
    report << "  Average Learning Rate: " << analysis.averageLearningRate << "\n";
    report << "  Learning Stability: " << analysis.learningStability << "\n";
    report << "  Overall Learning Curve Score: " << analysis.overallLearningCurveScore << "\n";
    
    if (!analysis.taskSpecificLearningRates.empty()) {
        report << "  Task-Specific Learning Rates: [";
        for (size_t i = 0; i < analysis.taskSpecificLearningRates.size(); ++i) {
            report << analysis.taskSpecificLearningRates[i];
            if (i < analysis.taskSpecificLearningRates.size() - 1) report << ", ";
        }
        report << "]\n";
    }
    
    return report.str();
}

// Damage Recovery Experiment Implementation
std::vector<DamageRecoveryExperiment::RecoveryMetric> DamageRecoveryExperiment::run(
    std::shared_ptr<Brain> brain,
    const DamageProfile& damage,
    const std::vector<std::string>& rehabilitationStrategies,
    uint64_t recoverySteps,
    bool enableNeurogenesis,
    bool enablePlasticityBoost) {
    
    std::vector<RecoveryMetric> recoveryData;
    
    try {
        brain->reset();
        brain->initialize();
        
        damage.applyDamage(brain.get());
        float initialDamageLevel = damage.measureDamage(brain.get());
        NLM_LOG_INFO("Applied damage: " + damage.damageType + " (Severity: " + 
                    std::to_string(initialDamageLevel) + ")");
        
        RecoveryMetric initialMetric;
        initialMetric.timestamp = std::chrono::system_clock::now();
        initialMetric.recoveryProgress = 0.0f;
        initialMetric.functionalCapacity = 1.0f - initialDamageLevel;
        initialMetric.synapticIntegrity = 1.0f - initialDamageLevel;
        initialMetric.behavioralPerformance = brain->getAverageFiringRate();
        recoveryData.push_back(initialMetric);
        
        for (uint64_t step = 0; step < recoverySteps; ++step) {
            RecoveryMetric metric;
            metric.timestamp = std::chrono::system_clock::now();
            
            for (const auto& strategy : rehabilitationStrategies) {
                if (strategy == "neurogenesis" && enableNeurogenesis) {
                    float neurogenesisRate = 0.001f * (1.0f - metric.recoveryProgress);
                    brain->getDevelopmentSystem()->increaseNeurons(static_cast<size_t>(neurosgenesisRate * 1000));
                } else if (strategy == "plasticity_boost" && enablePlasticityBoost) {
                    if (auto* plasticity = brain->getSTDP()) {
                        plasticity->setLearningRate(plasticity->getLearningRate() * 1.1f);
                    }
                }
            }
            
            float currentDamageLevel = damage.measureDamage(brain.get());
            metric.recoveryProgress = 1.0f - (currentDamageLevel / initialDamageLevel);
            metric.functionalCapacity = 1.0f - currentDamageLevel;
            metric.synapticIntegrity = brain->getTotalSynapseCount() / 
                                      static_cast<float>(brain->getTotalNeuronCount() * 10);
            metric.behavioralPerformance = brain->getAverageFiringRate();
            
            metric.subsystemRecoveries["cognition"] = metric.functionalCapacity;
            metric.subsystemRecoveries["motor"] = metric.behavioralPerformance;
            
            recoveryData.push_back(metric);
            brain->step(step, step * 0.001);
            
            if (metric.recoveryProgress > 0.95f) {
                NLM_LOG_INFO("Near-complete recovery achieved at step " + std::to_string(step));
                break;
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in damage recovery experiment: " + std::string(e.what()));
    }
    
    return recoveryData;
}

// Scaling Experiment Implementation
ScalingExperiment::ScalingAnalysis ScalingExperiment::run(
    const std::vector<ScaleLevel>& scaleLevels,
    uint64_t stepsPerScale,
    bool enableMultithreading,
    bool enableSIMD,
    bool enableSparseConnectivity,
    const std::string& hardwareProfile) {
    
    ScalingAnalysis analysis;
    
    try {
        NLM_LOG_INFO("Starting scaling experiment with " + std::to_string(scaleLevels.size()) + 
                    " scale levels");
        
        for (size_t i = 0; i < scaleLevels.size(); ++i) {
            const auto& scale = scaleLevels[i];
            NLM_LOG_INFO("Testing scale level " + std::to_string(i+1) + ": " + 
                        std::to_string(scale.neuronCount) + " neurons");
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(scale.neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(scale.regionCount), ConfigSource::Default);
            config->set("connection_probability", scale.connectionProbability, ConfigSource::Default);
            
            if (enableMultithreading) {
                config->set("thread_count", 4, ConfigSource::Default);
            }
            
            if (enableSIMD) {
                config->set("enable_simd", true, ConfigSource::Default);
            }
            
            if (enableSparseConnectivity) {
                config->set("enable_sparse_connectivity", true, ConfigSource::Default);
            }
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for scale " + std::to_string(scale.neuronCount));
                continue;
            }
            
            float scalePerformance = 0.0f;
            for (uint64_t step = 0; step < stepsPerScale; ++step) {
                brain->step(step, step * 0.001);
                
                if (step % (stepsPerScale / 10) == 0) {
                    scalePerformance += brain->getAverageFiringRate();
                }
            }
            
            scalePerformance /= 10.0f;
            scale.empiricalPerformance = scalePerformance;
            
            scale.theoreticalCapacity = scale.neuronCount * scale.connectionProbability * 0.5f;
            scale.resourceUtilization = scale.neuronCount / 10000.0f;
            scale.timeToConvergence = stepsPerScale / 1000.0f;
            
            analysis.scaleLevels.push_back(scale);
            analysis.performanceScaling.push_back(std::make_pair(scale.neuronCount, scalePerformance));
            analysis.resourceScaling.push_back(std::make_pair(scale.neuronCount, scale.resourceUtilization));
        }
        
        if (analysis.scaleLevels.size() >= 3) {
            analyzeScalingLaws(analysis.scaleLevels, "power_law");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in scaling experiment: " + std::string(e.what()));
    }
    
    return analysis;
}

ScalingExperiment::ScalingAnalysis ScalingExperiment::analyzeScalingLaws(
    const std::vector<ScaleLevel>& scaleLevels,
    const std::string& scalingModel) {
    
    ScalingAnalysis analysis;
    
    if (scaleLevels.size() < 2) {
        return analysis;
    }
    
    std::vector<size_t> neuronCounts;
    std::vector<double> performances;
    
    for (const auto& scale : scaleLevels) {
        neuronCounts.push_back(scale.neuronCount);
        performances.push_back(scale.empiricalPerformance);
    }
    
    if (scalingModel == "power_law") {
        double logNeuronsSum = 0.0, logPerformanceSum = 0.0, logNeuronsPerfSum = 0.0;
        double logNeuronsSqSum = 0.0;
        
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            double logNeurons = std::log(neuronCounts[i]);
            double logPerf = std::log(std::max(performances[i], 0.001));
            
            logNeuronsSum += logNeurons;
            logPerformanceSum += logPerf;
            logNeuronsPerfSum += logNeurons * logPerf;
            logNeuronsSqSum += logNeurons * logNeurons;
        }
        
        double n = neuronCounts.size();
        double b = (n * logNeuronsPerfSum - logNeuronsSum * logPerformanceSum) / 
                   (n * logNeuronsSqSum - logNeuronsSum * logNeuronsSum);
        double a = std::exp((logPerformanceSum - b * logNeuronsSum) / n);
        
        analysis.scalingLawExponent = b;
        analysis.saturationPoint = a / 0.01;
        analysis.efficiencyPeak = a * std::pow(10000.0, b);
        
    } else if (scalingModel == "exponential") {
        double logPerfSum = 0.0, logNeuronsSum = 0.0, logNeuronsPerfSum = 0.0;
        double logNeuronsSqSum = 0.0;
        
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            double logNeurons = std::log(neuronCounts[i]);
            double logPerf = std::log(std::max(performances[i], 0.001));
            
            logPerfSum += logPerf;
            logNeuronsSum += logNeurons;
            logNeuronsPerfSum += logNeurons * logPerf;
            logNeuronsSqSum += logNeurons * logNeurons;
        }
        
        double n = neuronCounts.size();
        double b = (n * logNeuronsPerfSum - logNeuronsSum * logPerfSum) / 
                   (n * logNeuronsSqSum - logNeuronsSum * logNeuronsSum);
        double a = std::exp((logPerfSum - b * logNeuronsSum) / n);
        
        analysis.scalingLawExponent = b;
        analysis.saturationPoint = 50.0f;
        analysis.efficiencyPeak = a * std::exp(b * 50000.0f);
        
    } else {
        double sumNeurons = std::accumulate(neuronCounts.begin(), neuronCounts.end(), 0.0);
        double sumPerf = std::accumulate(performances.begin(), performances.end(), 0.0);
        double sumNeuronsPerf = 0.0, sumNeuronsSq = 0.0;
        
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            sumNeuronsPerf += neuronCounts[i] * performances[i];
            sumNeuronsSq += neuronCounts[i] * neuronCounts[i];
        }
        
        double n = neuronCounts.size();
        double b = (n * sumNeuronsPerf - sumNeurons * sumPerf) / 
                   (n * sumNeuronsSq - sumNeurons * sumNeurons);
        double a = (sumPerf - b * sumNeurons) / n;
        
        analysis.scalingLawExponent = b;
        analysis.saturationPoint = 1000.0f;
        analysis.efficiencyPeak = a + b * 50000.0f;
    }
    
    analysis.scaleLevels = scaleLevels;
    
    return analysis;
}

// Placeholder implementations for other experiment types (simplified for brevity)
template<typename T>
std::vector<T> createAdvancedExperimentSuite(const std::string& suiteName, const std::vector<std::string>& experimentTypes) {
    return std::vector<T>();
}

} // namespace nlm