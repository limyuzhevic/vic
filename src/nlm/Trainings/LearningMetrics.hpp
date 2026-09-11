#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <limits>

namespace nlm {

/**
 * Learning metrics collection and analysis
 */
class LearningMetrics {
public:
    struct TimeSeriesData {
        std::vector<double> timestamps;
        std::vector<float> values;
        std::string label;
        
        TimeSeriesData(const std::string& lbl) : label(lbl) {}
        
        void addDataPoint(double time, float value) {
            timestamps.push_back(time);
            values.push_back(value);
        }
        
        float getAverage() const {
            if (values.empty()) return 0.0f;
            float sum = std::accumulate(values.begin(), values.end(), 0.0f);
            return sum / values.size();
        }
        
        float getMin() const {
            if (values.empty()) return 0.0f;
            return *std::min_element(values.begin(), values.end());
        }
        
        float getMax() const {
            if (values.empty()) return 0.0f;
            return *std::max_element(values.begin(), values.end());
        }
    };
    
    struct TrainingProgress {
        double elapsedTime;
        SimulationStep currentStep;
        float totalReward;
        float averageFiringRate;
        float noveltyLevel;
        float curiosityLevel;
        float dopamineLevel;
        float predictionError;
        size_t totalSpikes;
        size_t memoryEpisodesStored;
        float weightChange;
        
        TrainingProgress()
            : elapsedTime(0.0), currentStep(0), totalReward(0.0f), averageFiringRate(0.0f),
              noveltyLevel(0.0f), curiosityLevel(0.0f), dopamineLevel(0.0f),
              predictionError(0.0f), totalSpikes(0), memoryEpisodesStored(0), weightChange(0.0f) {}
    };
    
    struct TrainingStatistics {
        float totalTrainingTime;
        size_t totalSteps;
        float avgRewardPerStep;
        float maxReward;
        float minReward;
        float avgFiringRate;
        float noveltyTrend;
        float curiosityTrend;
        float dopamineTrend;
        float learningRate;
        float convergenceScore;
        
        TrainingStatistics()
            : totalTrainingTime(0.0f), totalSteps(0), avgRewardPerStep(0.0f), maxReward(0.0f),
              minReward(0.0f), avgFiringRate(0.0f), noveltyTrend(0.0f), curiosityTrend(0.0f),
              dopamineTrend(0.0f), learningRate(0.0f), convergenceScore(0.0f) {}
        
        std::string toString() const {
            std::stringstream ss;
            ss << "Training Statistics:" << std::endl;
            ss << "  Total steps: " << totalSteps << std::endl;
            ss << "  Training time: " << totalTrainingTime << "s" << std::endl;
            ss << "  Avg reward/step: " << avgRewardPerStep << std::endl;
            ss << "  Reward range: [" << minReward << ", " << maxReward << "]" << std::endl;
            ss << "  Avg firing rate: " << avgFiringRate << std::endl;
            ss << "  Learning rate: " << learningRate << std::endl;
            ss << "  Convergence score: " << convergenceScore << std::endl;
            return ss.str();
        }
    };
    
    LearningMetrics() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void recordStep(const TrainingProgress& progress) {
        progressHistory.push_back(progress);
        
        // Update time series data
        rewardTimeSeries.addDataPoint(progress.elapsedTime, progress.totalReward);
        firingRateTimeSeries.addDataPoint(progress.elapsedTime, progress.averageFiringRate);
        noveltyTimeSeries.addDataPoint(progress.elapsedTime, progress.noveltyLevel);
        curiosityTimeSeries.addDataPoint(progress.elapsedTime, progress.curiosityLevel);
        dopamineTimeSeries.addDataPoint(progress.elapsedTime, progress.dopamineLevel);
        
        // Calculate weight change (simplified - track total change)
        static float lastTotalReward = 0.0f;
        weightChange = progress.totalReward - lastTotalReward;
        lastTotalReward = progress.totalReward;
    }
    
    TrainingStatistics computeStatistics() const {
        TrainingStatistics stats;
        
        if (progressHistory.empty()) return stats;
        
        stats.totalSteps = progressHistory.size();
        
        // Calculate total training time
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        stats.totalTrainingTime = static_cast<float>(duration.count());
        
        // Calculate average metrics
        float sumReward = 0.0f, sumFiring = 0.0f, sumNovelty = 0.0f, sumCuriosity = 0.0f, sumDopamine = 0.0f;
        float maxReward = std::numeric_limits<float>::lowest(), minReward = std::numeric_limits<float>::max();
        
        for (const auto& progress : progressHistory) {
            sumReward += progress.totalReward;
            sumFiring += progress.averageFiringRate;
            sumNovelty += progress.noveltyLevel;
            sumCuriosity += progress.curiosityLevel;
            sumDopamine += progress.dopamineLevel;
            
            maxReward = std::max(maxReward, progress.totalReward);
            minReward = std::min(minReward, progress.totalReward);
        }
        
        stats.avgRewardPerStep = sumReward / progressHistory.size();
        stats.maxReward = maxReward;
        stats.minReward = minReward;
        stats.avgFiringRate = sumFiring / progressHistory.size();
        stats.noveltyTrend = calculateTrend(noveltyTimeSeries);
        stats.curiosityTrend = calculateTrend(curiosityTimeSeries);
        stats.dopamineTrend = calculateTrend(dopamineTimeSeries);
        
        // Estimate learning rate
        if (progressHistory.size() > 10) {
            float firstHalfAvg = 0.0f, secondHalfAvg = 0.0f;
            size_t half = progressHistory.size() / 2;
            
            for (size_t i = 0; i < half; ++i) {
                firstHalfAvg += progressHistory[i].totalReward;
            }
            firstHalfAvg /= half;
            
            for (size_t i = half; i < progressHistory.size(); ++i) {
                secondHalfAvg += progressHistory[i].totalReward;
            }
            secondHalfAvg /= (progressHistory.size() - half);
            
            stats.learningRate = secondHalfAvg - firstHalfAvg;
        }
        
        // Calculate convergence score (decreasing reward volatility)
        float rewardVolatility = 0.0f;
        for (const auto& progress : progressHistory) {
            rewardVolatility += progress.weightChange * progress.weightChange;
        }
        rewardVolatility /= progressHistory.size();
        stats.convergenceScore = 1.0f / (1.0f + rewardVolatility);
        
        return stats;
    }
    
    std::string getProgressReport() const {
        std::stringstream ss;
        ss << "=== LEARNING PROGRESS REPORT ===" << std::endl;
        ss << "Total training steps: " << progressHistory.size() << std::endl;
        if (!progressHistory.empty()) {
            const auto& lastProgress = progressHistory.back();
            ss << "Current metrics:" << std::endl;
            ss << "  Reward: " << lastProgress.totalReward << std::endl;
            ss << "  Firing rate: " << lastProgress.averageFiringRate << std::endl;
            ss << "  Novelty: " << lastProgress.noveltyLevel << std::endl;
            ss << "  Curiosity: " << lastProgress.curiosityLevel << std::endl;
            ss << "  Dopamine: " << lastProgress.dopamineLevel << std::endl;
            ss << "  Prediction error: " << lastProgress.predictionError << std::endl;
        }
        
        auto stats = computeStatistics();
        ss << std::endl << stats.toString();
        
        return ss.str();
    }
    
    // Export metrics to CSV for analysis
    std::string exportToCSV() const {
        std::stringstream ss;
        ss << "Step,Time,TotalReward,AvgFiringRate,Novelty,Curiosity,Dopamine,PredictionError,WeightChange" << std::endl;
        
        for (size_t i = 0; i < progressHistory.size(); ++i) {
            const auto& progress = progressHistory[i];
            ss << i << "," << progress.elapsedTime << "," << progress.totalReward << ",";
            ss << progress.averageFiringRate << "," << progress.noveltyLevel << "," << progress.curiosityLevel << ",";
            ss << progress.dopamineLevel << "," << progress.predictionError << "," << progress.weightChange << std::endl;
        }
        
        return ss.str();
    }
    
    // Generate visualization data
    struct PlotData {
        std::vector<double> xData;
        std::vector<float> yReward;
        std::vector<float> yFiring;
        std::vector<float> yNovelty;
    };
    
    PlotData getPlotData() const {
        PlotData data;
        for (size_t i = 0; i < progressHistory.size(); ++i) {
            const auto& progress = progressHistory[i];
            data.xData.push_back(progress.elapsedTime);
            data.yReward.push_back(progress.totalReward);
            data.yFiring.push_back(progress.averageFiringRate);
            data.yNovelty.push_back(progress.noveltyLevel);
        }
        return data;
    }
    
private:
    float calculateTrend(const TimeSeriesData& series) const {
        if (series.values.size() < 2) return 0.0f;
        
        float firstHalf = 0.0f, secondHalf = 0.0f;
        size_t half = series.values.size() / 2;
        
        for (size_t i = 0; i < half && i < series.values.size(); ++i) {
            firstHalf += series.values[i];
        }
        firstHalf /= (half == 0 ? 1 : half);
        
        for (size_t i = half; i < series.values.size(); ++i) {
            secondHalf += series.values[i];
        }
        secondHalf /= (series.values.size() - half);
        
        return secondHalf - firstHalf;
    }
    
    std::vector<TrainingProgress> progressHistory;
    TimeSeriesData rewardTimeSeries{ "Reward" };
    TimeSeriesData firingRateTimeSeries{ "Firing Rate" };
    TimeSeriesData noveltyTimeSeries{ "Novelty" };
    TimeSeriesData curiosityTimeSeries{ "Curiosity" };
    TimeSeriesData dopamineTimeSeries{ "Dopamine" };
    std::chrono::high_resolution_clock::time_point startTime;
};

/**
 * Training loop controller for NLM brain training
 */
class TrainingController {
public:
    struct TrainingConfig {
        uint64_t maxSteps;
        double timestep;
        bool verbose;
        std::string savePath;
        bool autoSave;
        float rewardThreshold;
        float convergenceThreshold;
        size_t patience;  // Steps without improvement before early stopping
        
        TrainingConfig()
            : maxSteps(10000), timestep(0.001), verbose(false), savePath(""), 
              autoSave(false), rewardThreshold(0.0f), convergenceThreshold(0.01f), patience(1000) {}
    };
    
    TrainingController(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                      std::shared_ptr<SimpleWorld> world)
        : brain_(brain), agent_(agent), world_(world), metrics_(),
          bestRewardSoFar(std::numeric_limits<float>::lowest()),
          stepsWithoutImprovement(0), episodeCount(0) {}
    
    TrainingResult runTraining(const TrainingConfig& config) {
        TrainingResult result;
        result.success = false;
        result.maxStepsReached = false;
        
        if (!brain_ || !agent_ || !world_) {
            result.errorMessage = "Invalid brain, agent, or world for training";
            return result;
        }
        
        std::cout << "=== NLM TRAINING LOOP ===" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Max steps: " << config.maxSteps << std::endl;
        std::cout << "  Timestep: " << config.timestep << std::endl;
        std::cout << "  Verbose: " << (config.verbose ? "yes" : "no") << std::endl;
        std::cout << "  Auto-save: " << (config.autoSave ? "yes" : "no") << std::endl;
        std::cout << std::endl;
        
        // Initialize training state
        resetTrainingState();
        
        try {
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(config.maxSteps); ++step) {
                // Record start time for this step
                auto stepStart = std::chrono::high_resolution_clock::now();
                
                // Run one training step
                float reward = runTrainingStep(step, config.timestep);
                
                // Update metrics
                LearningMetrics::TrainingProgress progress;
                progress.elapsedTime = getElapsedTime();
                progress.currentStep = step;
                progress.totalReward = totalReward;
                progress.averageFiringRate = brain_->getAverageFiringRate();
                progress.noveltyLevel = agent_ ? agent_->getNoveltyLevel() : 0.0f;
                progress.curiosityLevel = agent_ ? agent_->getCuriosityLevel() : 0.0f;
                progress.dopamineLevel = agent_ ? agent_->getNeuromodulationLevel() : 0.0f;
                progress.predictionError = agent_ ? agent_->getPredictionError() : 0.0f;
                progress.totalSpikes = brain_->getTotalSpikeCount();
                progress.memoryEpisodesStored = (world_ ? world_->getSimulationTime() : 0.0) / 10.0f; // Rough estimate
                progress.weightChange = reward - (step > 0 ? lastStepReward : 0.0f);
                
                metrics_.recordStep(progress);
                
                lastStepReward = reward;
                
                // Check for improvement
                if (reward > bestRewardSoFar) {
                    bestRewardSoFar = reward;
                    stepsWithoutImprovement = 0;
                    
                    // Auto-save if enabled
                    if (config.autoSave && !config.savePath.empty()) {
                        std::string saveFile = config.savePath + "_best_" + std::to_string(step) + ".bin";
                        if (brain_->save(saveFile)) {
                            std::cout << "Saved best model at step " << step << " to " << saveFile << std::endl;
                        }
                    }
                } else {
                    stepsWithoutImprovement++;
                }
                
                // Check for early stopping
                if (config.patience > 0 && stepsWithoutImprovement >= config.patience) {
                    std::cout << "Early stopping triggered after " << stepsWithoutImprovement 
                             << " steps without improvement" << std::endl;
                    result.earlyStopped = true;
                    break;
                }
                
                // Verbose output
                if (config.verbose && (step % 100 == 0 || step == 0)) {
                    std::cout << "Step " << step << ": reward = " << reward 
                             << ", total = " << totalReward 
                             << ", firing = " << brain_->getAverageFiringRate() 
                             << ", spikes = " << brain_->getTotalSpikeCount()
                             << ", time = " << getElapsedTime() << "s" << std::endl;
                }
                
                // Record step duration for timing calculations
                auto stepEnd = std::chrono::high_resolution_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::duration<double>>(stepEnd - stepStart);
                stepTimes.push_back(stepDuration.count());
            }
            
            if (step >= static_cast<SimulationStep>(config.maxSteps)) {
                result.maxStepsReached = true;
                std::cout << "Reached maximum steps (" << config.maxSteps << ")" << std::endl;
            }
            
            // Training completed successfully
            result.success = true;
            result.finalMetrics = metrics_.computeStatistics();
            result.totalSteps = config.maxSteps;
            
            // Save final state if requested
            if (!config.savePath.empty()) {
                std::string finalFile = config.savePath + "_final.bin";
                if (brain_->save(finalFile)) {
                    std::cout << "Saved final model to " << finalFile << std::endl;
                }
            }
            
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Training error: ") + e.what();
            std::cerr << "ERROR: " << result.errorMessage << std::endl;
        }
        
        // Generate final report
        result.progressReport = metrics_.getProgressReport();
        result.finalMetricsReport = result.finalMetrics.toString();
        
        std::cout << std::endl << "=== TRAINING SUMMARY ===" << std::endl;
        std::cout << result.progressReport << std::endl;
        std::cout << "Final metrics:" << std::endl;
        std::cout << result.finalMetricsReport << std::endl;
        
        return result;
    }
    
    LearningMetrics& getMetrics() {
        return metrics_;
    }
    
    const LearningMetrics& getMetrics() const {
        return metrics_;
    }
    
private:
    float runTrainingStep(SimulationStep step, double timestep) {
        // Reset world for new episode
        if (world_) {
            world_->reset();
        }
        
        float episodeReward = 0.0f;
        
        // Run simulation loop
        for (int substep = 0; substep < 100; ++substep) {
            // Update world
            if (world_) {
                world_->update(timestep);
            }
            
            // Get sensory input from world
            SensoryPercept percept;
            if (world_) {
                percept = world_->getSensoryPercept();
            }
            
            // Process sensory input in brain
            if (agent_) {
                agent_->processSensoryInput(percept);
            }
            
            // Run brain step
            brain_->step(step, step * timestep);
            
            // Decode motor command
            auto action = agent_ ? agent_->decodeMotorCommand() : nullptr;
            
            // Apply motor command to world
            if (action && world_) {
                world_->applyMotorCommand(action.get(), step * timestep);
            }
            
            // Get reward from world
            float reward = 0.0f;
            if (world_) {
                auto worldPercept = world_->getSensoryPercept();
                const auto& internal = worldPercept.getInternal();
                if (!internal.empty()) {
                    reward = internal[0];  // First internal signal as reward
                }
            }
            
            // Apply reward modulation
            if (agent_) {
                agent_->applyRewardModulation(reward, 0.0f);  // Simplified: no predicted reward
            }
            
            episodeReward += reward;
            totalReward += reward;
            
            // Update development
            if (agent_) {
                agent_->updateDevelopment(timestep);
            }
        }
        
        return episodeReward;
    }
    
    void resetTrainingState() {
        totalReward = 0.0f;
        lastStepReward = 0.0f;
        bestRewardSoFar = std::numeric_limits<float>::lowest();
        stepsWithoutImprovement = 0;
        episodeCount = 0;
        stepTimes.clear();
        metrics_ = LearningMetrics();
    }
    
    double getElapsedTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime);
        return duration.count();
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    LearningMetrics metrics_;
    
    float totalReward;
    float lastStepReward;
    float bestRewardSoFar;
    size_t stepsWithoutImprovement;
    size_t episodeCount;
    std::vector<double> stepTimes;
    std::chrono::high_resolution_clock::time_point startTime;
};

/**
 * Training result structure
 */
struct TrainingResult {
    bool success;
    bool earlyStopped;
    bool maxStepsReached;
    std::string errorMessage;
    std::string progressReport;
    std::string finalMetricsReport;
    
    LearningMetrics::TrainingStatistics finalMetrics;
    uint64_t totalSteps;
    
    TrainingResult()
        : success(false), earlyStopped(false), maxStepsReached(false), totalSteps(0) {}
};

} // namespace nlm
