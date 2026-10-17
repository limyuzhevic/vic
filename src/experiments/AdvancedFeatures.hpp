#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

// Abstract base class for exploration strategies
class ExplorationStrategy {
public:
    virtual ~ExplorationStrategy() = default;
    virtual std::string name() const = 0;
    virtual MotorCommand explore(const std::vector<float>& actionValues, 
                                std::mt19937& rng) = 0;
    virtual void update(double reward, double predictionError) {}
    virtual void reset() {}
};

// Epsilon-greedy exploration strategy
class EpsilonGreedyStrategy : public ExplorationStrategy {
private:
    double epsilon_;
    std::mt19937& rng_;
    std::uniform_real_distribution<double> uniform_;
    std::discrete_distribution<size_t> actionDist_;

public:
    explicit EpsilonGreedyStrategy(double epsilon, std::mt19937& rng) 
        : epsilon_(epsilon), rng_(rng), uniform_(0.0, 1.0) {}
    
    std::string name() const override { return "EpsilonGreedy"; }
    
    MotorCommand explore(const std::vector<float>& actionValues, 
                        std::mt19937& rng) override {
        if (actionValues.empty()) {
            return MotorCommand::Wait;
        }
        
        if (uniform_(rng_) < epsilon_) {
            std::uniform_int_distribution<size_t> uniform(0, actionValues.size() - 1);
            size_t randomAction = uniform(rng_);
            return static_cast<MotorCommand>(randomAction);
        } else {
            // Find best action (assuming indices match motor commands)
            auto maxIt = std::max_element(actionValues.begin(), actionValues.end());
            size_t bestAction = std::distance(actionValues.begin(), maxIt);
            return static_cast<MotorCommand>(std::min(bestAction, 
                static_cast<size_t>(MotorCommand::Interact)));
        }
    }
};

// UCB1 exploration strategy
class UCB1Strategy : public ExplorationStrategy {
private:
    double c_;
    std::vector<double> actionCounts_;
    std::vector<double> actionValues_;
    std::vector<double> actionSums_;

public:
    UCB1Strategy(double c = 2.0) : c_(c) {}
    
    std::string name() const override { return "UCB1"; }
    
    MotorCommand explore(const std::vector<float>& actionValues, 
                        std::mt19937& rng) override {
        if (actionValues.empty()) {
            return MotorCommand::Wait;
        }
        
        size_t bestAction = 0;
        double bestValue = -std::numeric_limits<double>::max();
        
        for (size_t i = 0; i < actionValues.size(); ++i) {
            double totalCount = 0.0;
            for (double count : actionCounts_) totalCount += count;
            
            double explorationTerm = 0.0;
            if (totalCount > 0) {
                explorationTerm = c_ * std::sqrt(std::log(totalCount) / 
                    (actionCounts_[i] + 1e-9));
            }
            
            double ucbValue = actionValues_[i] + explorationTerm;
            
            if (ucbValue > bestValue) {
                bestValue = ucbValue;
                bestAction = i;
            }
        }
        
        return static_cast<MotorCommand>(bestAction);
    }
    
    void update(double reward, double predictionError) override {
        // Update action statistics (simplified)
        if (!actionCounts_.empty()) {
            actionCounts_[0] += 1;
            actionSums_[0] += reward;
            actionValues_[0] = actionSums_[0] / actionCounts_[0];
        }
    }
};

// Thompson Sampling exploration strategy
class ThompsonSamplingStrategy : public ExplorationStrategy {
private:
    std::vector<double> alpha_;
    std::vector<double> beta_;
    std::vector<std::beta_distribution<double>> betaDists_;

public:
    explicit ThompsonSamplingStrategy(size_t numActions) {
        alpha_.resize(numActions, 1.0);
        beta_.resize(numActions, 1.0);
        betaDists_.reserve(numActions);
        for (size_t i = 0; i < numActions; ++i) {
            betaDists_.emplace_back(alpha_[i], beta_[i]);
        }
    }
    
    std::string name() const override { return "ThompsonSampling"; }
    
    MotorCommand explore(const std::vector<float>& actionValues, 
                        std::mt19937& rng) override {
        std::vector<double> sampledValues;
        
        for (size_t i = 0; i < actionValues.size(); ++i) {
            std::beta_distribution<double> dist(alpha_[i], beta_[i]);
            double sample = dist(rng);
            sampledValues.push_back(sample * actionValues[i]);
        }
        
        auto maxIt = std::max_element(sampledValues.begin(), sampledValues.end());
        size_t bestAction = std::distance(sampledValues.begin(), maxIt);
        return static_cast<MotorCommand>(bestAction);
    }
    
    void update(double reward, double predictionError) override {
        // Update beta distribution parameters
        for (size_t i = 0; i < alpha_.size(); ++i) {
            if (reward > 0) {
                alpha_[i] += 1.0;
            } else {
                beta_[i] += 1.0;
            }
        }
    }
};

// Learning metrics collection and analysis
struct LearningMetrics {
    // Performance metrics
    double totalReward = 0.0;
    double averageReward = 0.0;
    double rewardVariance = 0.0;
    double rewardStdDev = 0.0;
    
    // Exploration metrics
    double explorationRate = 0.0;
    double noveltyLevel = 0.0;
    double curiosityLevel = 0.0;
    
    // Behavioral metrics
    double actionEfficiency = 0.0;
    double predictionAccuracy = 0.0;
    double predictionError = 0.0;
    
    // Neural metrics
    size_t totalSpikes = 0;
    double averageFiringRate = 0.0;
    double synapticStrength = 0.0;
    double networkConnectivity = 0.0;
    
    // Development metrics
    double developmentalAge = 0.0;
    double developmentalStage = 0.0;
    double learningVelocity = 0.0;
    
    // Episode metrics
    int episodeCount = 0;
    int stepCount = 0;
    int successfulEpisodes = 0;
    int failedEpisodes = 0;
    
    // Time tracking
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double totalTimeSeconds = 0.0;
    double stepsPerSecond = 0.0;
    
    // History tracking
    std::vector<double> rewardHistory;
    std::vector<double> noveltyHistory;
    std::vector<double> curiosityHistory;
    std::vector<double> predictionErrorHistory;
    std::vector<size_t> spikeCounts;
    
    LearningMetrics() {
        startTime = std::chrono::steady_clock::now();
    }
    
    void reset() {
        totalReward = 0.0;
        averageReward = 0.0;
        rewardVariance = 0.0;
        rewardStdDev = 0.0;
        explorationRate = 0.0;
        noveltyLevel = 0.0;
        curiosityLevel = 0.0;
        actionEfficiency = 0.0;
        predictionAccuracy = 0.0;
        predictionError = 0.0;
        totalSpikes = 0;
        averageFiringRate = 0.0;
        synapticStrength = 0.0;
        networkConnectivity = 0.0;
        developmentalAge = 0.0;
        developmentalStage = 0.0;
        learningVelocity = 0.0;
        episodeCount = 0;
        stepCount = 0;
        successfulEpisodes = 0;
        failedEpisodes = 0;
        
        rewardHistory.clear();
        noveltyHistory.clear();
        curiosityHistory.clear();
        predictionErrorHistory.clear();
        spikeCounts.clear();
        
        startTime = std::chrono::steady_clock::now();
    }
    
    void update(double reward, double novelty, double curiosity, 
                double predictionErr, size_t spikes, int steps) {
        totalReward += reward;
        rewardHistory.push_back(reward);
        noveltyHistory.push_back(novelty);
        curiosityHistory.push_back(curiosity);
        predictionErrorHistory.push_back(predictionErr);
        spikeCounts.push_back(spikes);
        
        stepCount += steps;
        
        // Calculate running statistics
        if (!rewardHistory.empty()) {
            double sum = std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0);
            averageReward = sum / rewardHistory.size();
            
            double sqSum = 0.0;
            for (double r : rewardHistory) {
                sqSum += (r - averageReward) * (r - averageReward);
            }
            rewardVariance = sqSum / rewardHistory.size();
            rewardStdDev = std::sqrt(rewardVariance);
        }
        
        explorationRate = (reward <= 0.0) ? 1.0 : 0.0;
        noveltyLevel = novelty;
        curiosityLevel = curiosity;
        predictionError = predictionErr;
        totalSpikes += spikes;
        
        if (!spikeCounts.empty()) {
            averageFiringRate = static_cast<double>(totalSpikes) / spikeCounts.size();
        }
    }
    
    double getLearningVelocity() const {
        if (totalTimeSeconds > 0) {
            return stepCount / totalTimeSeconds;
        }
        return 0.0;
    }
    
    double getConfidenceInterval(int confidence = 95) const {
        if (rewardHistory.size() < 2) return 0.0;
        
        double mean = averageReward;
        double sumSquares = 0.0;
        for (double r : rewardHistory) {
            sumSquares += (r - mean) * (r - mean);
        }
        
        double stdError = std::sqrt(sumSquares / (rewardHistory.size() - 1)) /
            std::sqrt(rewardHistory.size());
        
        double zScore = 1.96; // for 95% confidence
        return zScore * stdError;
    }
};

// Performance benchmark for comparing strategies
struct BenchmarkResults {
    std::string strategyName;
    double totalReward;
    double averageReward;
    double rewardStdDev;
    double successRate;
    double explorationEfficiency;
    double learningSpeed;
    double finalPerformance;
    double timeToConverge;
    
    BenchmarkResults() : totalReward(0.0), averageReward(0.0), 
                         rewardStdDev(0.0), successRate(0.0),
                         explorationEfficiency(0.0), learningSpeed(0.0),
                         finalPerformance(0.0), timeToConverge(0.0) {}
};

// Adaptive parameters system
struct AdaptiveParameters {
    double learningRate;
    double explorationFactor;
    double noveltyThreshold;
    double curiosityCoefficient;
    double predictionErrorWeight;
    double developmentRate;
    double synapticPlasticity;
    double neuromodulationStrength;
    
    AdaptiveParameters() 
        : learningRate(0.01), explorationFactor(0.1), 
          noveltyThreshold(0.5), curiosityCoefficient(0.05),
          predictionErrorWeight(0.5), developmentRate(0.001),
          synapticPlasticity(1.0), neuromodulationStrength(1.0) {}
    
    void adapt(double performance, double baseline) {
        // Simple adaptive logic
        learningRate = std::min(learningRate * (1.0 + performance * 0.1), 0.1);
        explorationFactor = std::max(0.05, explorationFactor * (1.0 - performance * 0.05));
        curiosityCoefficient = curiosityCoefficient * (1.0 + baseline * 0.05);
    }
    
    void reset() {
        learningRate = 0.01;
        explorationFactor = 0.1;
        noveltyThreshold = 0.5;
        curiosityCoefficient = 0.05;
        predictionErrorWeight = 0.5;
        developmentRate = 0.001;
        synapticPlasticity = 1.0;
        neuromodulationStrength = 1.0;
    }
};

// Advanced progress monitoring with ETA calculations
class AdvancedProgressMonitor {
private:
    int totalSteps_;
    int currentStep_;
    std::chrono::steady_clock::time_point startTime_;
    std::vector<double> stepTimes_;
    double eta_ = 0.0;

public:
    AdvancedProgressMonitor(int totalSteps) 
        : totalSteps_(totalSteps), currentStep_(0) {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    void update(int step) {
        currentStep_ = step;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
        
        if (currentStep_ > 0 && elapsed > 0) {
            double stepsPerSecond = currentStep_ / elapsed;
            double remainingSteps = totalSteps_ - currentStep_;
            eta_ = remainingSteps / stepsPerSecond;
        }
        
        stepTimes_.push_back(elapsed);
    }
    
    void printProgress() const {
        double progress = static_cast<double>(currentStep_) / totalSteps_ * 100.0;
        
        // Progress bar
        int barWidth = 50;
        int progressPos = static_cast<int>(barWidth * progress / 100.0);
        
        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < progressPos) std::cout << "=";
            else if (i == progressPos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << progress << "%";
        
        if (currentStep_ % 10 == 0 || currentStep_ == totalSteps_) {
            std::cout << " ETA: " << std::fixed << std::setprecision(1) << eta_ << "s";
        }
        
        std::cout.flush();
    }
    
    double getETA() const { return eta_; }
    double getProgress() const { return static_cast<double>(currentStep_) / totalSteps_; }
};

// Configuration validator
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        std::string getSummary() const {
            std::stringstream ss;
            ss << "Validation: " << (isValid ? "PASSED" : "FAILED");
            if (!errors.empty()) {
                ss << " (Errors: " << errors.size() << ")";
            }
            if (!warnings.empty()) {
                ss << " (Warnings: " << warnings.size() << ")";
            }
            return ss.str();
        }
    };
    
    static ValidationResult validate(const std::string& param, double value,
                                    double min = -std::numeric_limits<double>::max(),
                                    double max = std::numeric_limits<double>::max()) {
        ValidationResult result;
        
        if (value < min) {
            result.isValid = false;
            result.errors.push_back("Parameter '" + param + "' value " + std::to_string(value) +
                                   " is below minimum " + std::to_string(min));
        }
        
        if (value > max) {
            result.isValid = false;
            result.errors.push_back("Parameter '" + param + "' value " + std::to_string(value) +
                                   " exceeds maximum " + std::to_string(max));
        }
        
        if (value < min * 0.5 || value > max * 0.5) {
            result.warnings.push_back("Parameter '" + param + "' is at extreme value " + std::to_string(value));
        }
        
        return result;
    }
};

// Results comparator for statistical analysis
class ResultsComparator {
public:
    struct ComparisonResult {
        std::string testName;
        double pValue;
        bool isSignificant;
        std::string interpretation;
        
        ComparisonResult(const std::string& name, double p) 
            : testName(name), pValue(p), isSignificant(p < 0.05) {
            if (pValue < 0.01) {
                interpretation = "Highly significant difference (p < 0.01)";
            } else if (pValue < 0.05) {
                interpretation = "Significant difference (p < 0.05)";
            } else if (pValue < 0.1) {
                interpretation = "Marginal significance (p < 0.1)";
            } else {
                interpretation = "No significant difference (p > 0.1)";
            }
        }
    };
    
    static double tTest(const std::vector<double>& sample1, const std::vector<double>& sample2) {
        if (sample1.size() < 2 || sample2.size() < 2) return 1.0;
        
        double mean1 = std::accumulate(sample1.begin(), sample1.end(), 0.0) / sample1.size();
        double mean2 = std::accumulate(sample2.begin(), sample2.end(), 0.0) / sample2.size();
        
        double var1 = 0.0, var2 = 0.0;
        for (double val : sample1) var1 += (val - mean1) * (val - mean1);
        for (double val : sample2) var2 += (val - mean2) * (val - mean2);
        
        var1 /= (sample1.size() - 1);
        var2 /= (sample2.size() - 1);
        
        double pooledVar = ((sample1.size() - 1) * var1 + (sample2.size() - 1) * var2) /
                          (sample1.size() + sample2.size() - 2);
        
        double t = (mean1 - mean2) / std::sqrt(pooledVar * (1.0/sample1.size() + 1.0/sample2.size()));
        
        // Approximate two-tailed p-value
        double p = 2.0 * (1.0 - std::erf(std::abs(t) / std::sqrt(2.0)));
        return p;
    }
    
    static ComparisonResult compare(const std::string& testName,
                                   const std::vector<double>& data1,
                                   const std::vector<double>& data2) {
        double pValue = tTest(data1, data2);
        return ComparisonResult(testName, pValue);
    }
};

// Visualization engine for multiple output formats
class VisualizationEngine {
public:
    struct PlotData {
        std::vector<double> xValues;
        std::vector<double> yValues;
        std::string label;
        std::string color;
    };
    
    static void exportToCSV(const std::string& filename, 
                           const std::vector<std::pair<std::string, std::vector<double>>>& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
            return;
        }
        
        // Write header
        bool first = true;
        for (const auto& pair : data) {
            if (!first) file << ",";
            file << pair.first;
            first = false;
        }
        file << "\n";
        
        // Write data (assuming all vectors have same size)
        if (!data.empty()) {
            size_t size = data.front().second.size();
            for (size_t i = 0; i < size; ++i) {
                first = true;
                for (const auto& pair : data) {
                    if (!first) file << ",";
                    file << pair.second[i];
                    first = false;
                }
                file << "\n";
            }
        }
        
        file.close();
    }
    
    static void exportToJSON(const std::string& filename,
                           const std::map<std::string, std::vector<double>>& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
            return;
        }
        
        file << "{\n";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) file << ",\n";
            file << "  \"" << pair.first << "\": [";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                file << pair.second[i];
                if (i < pair.second.size() - 1) file << ", ";
            }
            file << "]";
            first = false;
        }
        file << "\n}\n";
        file.close();
    }
    
    static void generateHTMLDashboard(const std::string& filename,
                                     const std::vector<PlotData>& plots) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
            return;
        }
        
        file << "<!DOCTYPE html>\n<html>\n<head>\n";
        file << "<title>NLM Phase 3 Dashboard</title>\n";
        file << "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
        file << "</head>\n<body>\n";
        file << "<h1>NLM Phase 3 Experiment Dashboard</h1>\n";
        file << "<div style=\"display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px;\">\n";
        
        for (const auto& plot : plots) {
            file << "  <div>\n";
            file << "    <h3>" << plot.label << "</h3>\n";
            file << "    <canvas id=\"chart" << plot.label << "\" width=\"400\" height=\"300\"></canvas>\n";
            file << "  </div>\n";
        }
        
        file << "</div>\n";
        file << "<script>\n";
        file << "  const colors = ['rgba(75, 192, 192, 0.6)', 'rgba(255, 99, 132, 0.6)', 'rgba(54, 162, 235, 0.6)', 'rgba(255, 206, 86, 0.6)'];\n";
        file << "  window.onload = function() {\n";
        
        int i = 0;
        for (const auto& plot : plots) {
            file << "    const ctx" << i << " = document.getElementById('chart" << plot.label << "').getContext('2d');\n";
            file << "    new Chart(ctx" << i << ", {\n";
            file << "      type: 'line',\n";
            file << "      data: {\n";
            file << "        labels: [";
            for (size_t j = 0; j < plot.xValues.size(); ++j) {
                file << plot.xValues[j];
                if (j < plot.xValues.size() - 1) file << ", ";
            }
            file << "],\n";
            file << "        datasets: [{\n";
            file << "          label: '" << plot.label << "',\n";
            file << "          data: [";
            for (size_t j = 0; j < plot.yValues.size(); ++j) {
                file << plot.yValues[j];
                if (j < plot.yValues.size() - 1) file << ", ";
            }
            file << "],\n";
            file << "          borderColor: '" << plot.color << "',\n";
            file << "          backgroundColor: '" << plot.color << "'\n";
            file << "        }]\n";
            file << "      },\n";
            file << "      options: {\n";
            file << "        responsive: true,\n";
            file << "        plugins: {\n";
            file << "          title: {\n";
            file << "            display: true,\n";
            file << "            text: '" << plot.label << "'\n";
            file << "          }\n";
            file << "        },\n";
            file << "        scales: {\n";
            file << "          x: { title: { display: true, text: 'Episode' } },\n";
            file << "          y: { title: { display: true, text: 'Value' } }\n";
            file << "        }\n";
            file << "      }\n";
            file << "    });\n";
            i++;
        }
        
        file << "  }\n";
        file << "</script>\n";
        file << "</body>\n</html>";
        file.close();
    }
};

} // namespace nlm