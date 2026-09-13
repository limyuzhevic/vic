#include "ResearchWorkflow.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <filesystem>

namespace nlm {

namespace workflow_utils {

StatisticalResult performTTest(const std::vector<double>& data1, 
                              const std::vector<double>& data2,
                              const std::string& testName) {
    StatisticalResult result;
    result.testType = testName + "_t_test";
    
    if (data1.size() < 2 || data2.size() < 2) {
        result.interpretation = "Insufficient data for t-test";
        result.recommendation = "Collect more data points";
        return result;
    }
    
    double mean1 = calculateMean(data1);
    double mean2 = calculateMean(data2);
    double stddev1 = calculateStdDev(data1, mean1);
    double stddev2 = calculateStdDev(data2, mean2);
    
    double pooledStddev = std::sqrt((stddev1 * stddev1 + stddev2 * stddev2) / 2.0);
    
    if (pooledStddev == 0.0) {
        result.statistic = 0.0;
        result.pValue = 1.0;
    } else {
        result.statistic = (mean1 - mean2) / pooledStddev;
        // Simplified p-value calculation - in practice would use t-distribution
        result.pValue = 2.0 * (1.0 - std::erf(std::abs(result.statistic) / std::sqrt(2.0)));
    }
    
    // Interpretation
    if (result.pValue < 0.01) {
        result.interpretation = "Highly significant difference between groups";
    } else if (result.pValue < 0.05) {
        result.interpretation = "Statistically significant difference between groups";
    } else if (result.pValue < 0.10) {
        result.interpretation = "Marginally significant difference between groups";
    } else {
        result.interpretation = "No statistically significant difference between groups";
    }
    
    // Effect size
    result.statistic = calculateEffectSize(mean1, mean2, stddev1, stddev2);
    
    if (std::abs(result.statistic) < 0.2) {
        result.recommendation = "Small effect size. Consider increasing sample size for better precision.";
    } else if (std::abs(result.statistic) < 0.5) {
        result.recommendation = "Medium effect size. Results are practically meaningful.";
    } else {
        result.recommendation = "Large effect size. Results are highly meaningful.";
    }
    
    return result;
}

StatisticalResult performANOVA(const std::vector<std::vector<double>>& datasets,
                               const std::string& testName) {
    StatisticalResult result;
    result.testType = testName + "_anova";
    
    if (datasets.size() < 2) {
        result.interpretation = "Insufficient groups for ANOVA";
        result.recommendation = "Include at least 2 groups";
        return result;
    }
    
    double grandMean = 0.0;
    size_t totalCount = 0;
    for (const auto& data : datasets) {
        grandMean += calculateMean(data) * data.size();
        totalCount += data.size();
    }
    grandMean /= totalCount;
    
    // Between-group variance
    double betweenSS = 0.0;
    for (const auto& data : datasets) {
        double groupMean = calculateMean(data);
        betweenSS += data.size() * (groupMean - grandMean) * (groupMean - grandMean);
    }
    
    // Within-group variance
    double withinSS = 0.0;
    for (const auto& data : datasets) {
        double groupMean = calculateMean(data);
        for (double value : data) {
            withinSS += (value - groupMean) * (value - groupMean);
        }
    }
    
    // F-statistic
    double dfBetween = datasets.size() - 1;
    double dfWithin = totalCount - datasets.size();
    
    if (withinSS == 0.0 || dfWithin == 0) {
        result.statistic = 0.0;
        result.pValue = 1.0;
    } else {
        double msBetween = betweenSS / dfBetween;
        double msWithin = withinSS / dfWithin;
        result.statistic = msBetween / msWithin;
        
        // Simplified p-value using chi-square approximation
        result.pValue = std::exp(-result.statistic * dfBetween / 2.0);
    }
    
    // Interpretation
    if (result.pValue < 0.01) {
        result.interpretation = "Highly significant differences among groups";
    } else if (result.pValue < 0.05) {
        result.interpretation = "Significant differences among groups";
    } else if (result.pValue < 0.10) {
        result.interpretation = "Marginally significant differences among groups";
    } else {
        result.interpretation = "No significant differences among groups";
    }
    
    result.recommendation = "Consider post-hoc tests to identify specific group differences";
    
    return result;
}

StatisticalResult performRegression(const std::vector<double>& x,
                                    const std::vector<double>& y,
                                    const std::string& testName) {
    StatisticalResult result;
    result.testType = testName + "_regression";
    
    if (x.size() != y.size() || x.size() < 2) {
        result.interpretation = "Insufficient data for regression analysis";
        result.recommendation = "Collect at least 2 data points";
        return result;
    }
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    double xStddev = calculateStdDev(x, xMean);
    double yStddev = calculateStdDev(y, yMean);
    
    double numerator = 0.0, denominator = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        numerator += (x[i] - xMean) * (y[i] - yMean);
        denominator += (x[i] - xMean) * (x[i] - xMean);
    }
    
    if (denominator == 0.0) {
        result.statistic = 0.0;
        result.pValue = 1.0;
    } else {
        double slope = numerator / denominator;
        double intercept = yMean - slope * xMean;
        
        // Calculate predicted values and residuals
        std::vector<double> predicted(y.size());
        double ssTotal = 0.0, ssResidual = 0.0;
        
        for (size_t i = 0; i < y.size(); ++i) {
            predicted[i] = slope * x[i] + intercept;
            ssTotal += (y[i] - yMean) * (y[i] - yMean);
            ssResidual += (y[i] - predicted[i]) * (y[i] - predicted[i]);
        }
        
        if (ssTotal == 0.0) {
            result.statistic = (ssResidual == 0.0) ? 0.0 : -1.0;
            result.pValue = 1.0;
        } else {
            double rSquared = 1.0 - ssResidual / ssTotal;
            result.statistic = rSquared; // Use R-squared as the statistic
            result.pValue = std::pow(result.statistic, x.size() / 2.0); // Simplified
        }
        
        // Correlation coefficient
        result.statistic = calculateCorrelation(x, y);
    }
    
    // Interpretation
    if (std::abs(result.statistic) < 0.3) {
        result.interpretation = "Weak linear relationship";
    } else if (std::abs(result.statistic) < 0.7) {
        result.interpretation = "Moderate linear relationship";
    } else {
        result.interpretation = "Strong linear relationship";
    }
    
    if (result.pValue < 0.01) {
        result.recommendation = "Regression model is highly significant";
    } else if (result.pValue < 0.05) {
        result.recommendation = "Regression model is statistically significant";
    } else {
        result.recommendation = "Regression model is not statistically significant";
    }
    
    return result;
}

double calculateEffectSize(double mean1, double mean2, double stddev1, double stddev2) {
    double pooledStddev = std::sqrt((stddev1 * stddev1 + stddev2 * stddev2) / 2.0);
    if (pooledStddev == 0.0) {
        return 0.0;
    }
    return (mean1 - mean2) / pooledStddev;
}

double calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double calculateStdDev(const std::vector<double>& data, double mean) {
    if (data.size() < 2) return 0.0;
    
    double sumSquares = 0.0;
    for (double value : data) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (data.size() - 1));
}

double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    double xStddev = calculateStdDev(x, xMean);
    double yStddev = calculateStdDev(y, yMean);
    
    if (xStddev == 0.0 || yStddev == 0.0) return 0.0;
    
    double covariance = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        covariance += (x[i] - xMean) * (y[i] - yMean);
    }
    
    return covariance / (x.size() * xStddev * yStddev);
}

double calculateCoefficientOfVariation(const std::vector<double>& data, double mean) {
    if (mean == 0.0) return 0.0;
    double stddev = calculateStdDev(data, mean);
    return stddev / std::abs(mean) * 100.0; // Percentage
}

std::string formatStatisticalResult(const StatisticalResult& result) {
    std::ostringstream oss;
    oss << "Test: " << result.testType << "\n";
    oss << "  Statistic: " << result.statistic << "\n";
    oss << "  p-value: " << result.pValue << "\n";
    oss << "  Interpretation: " << result.interpretation << "\n";
    oss << "  Recommendation: " << result.recommendation << "\n";
    return oss.str();
}

std::string formatConfidenceInterval(double mean, double stddev, double n, double confidence) {
    std::ostringstream oss;
    
    if (n < 2) {
        return "Insufficient data for confidence interval";
    }
    
    // t-value for 95% confidence (simplified)
    double tValue = 2.0;
    
    double margin = tValue * stddev / std::sqrt(n);
    double lower = mean - margin;
    double upper = mean + margin;
    
    oss << "95% CI: [" << lower << ", " << upper << "] (n=" << n << ")";
    return oss.str();
}

std::string identifyLearningPhase(const std::vector<double>& metricValues,
                                   double stabilityThreshold) {
    if (metricValues.size() < 10) {
        return "Insufficient data for learning phase analysis";
    }
    
    // Look for stability in the latter part of the curve
    size_t startIndex = metricValues.size() * 0.7; // Last 30%
    if (startIndex >= metricValues.size() - 1) {
        startIndex = metricValues.size() / 2; // Fallback to middle
    }
    
    double firstPartMean = 0.0, secondPartMean = 0.0;
    size_t firstPartSize = 0, secondPartSize = 0;
    
    for (size_t i = 0; i < metricValues.size(); ++i) {
        if (i < startIndex) {
            firstPartMean += metricValues[i];
            firstPartSize++;
        } else {
            secondPartMean += metricValues[i];
            secondPartSize++;
        }
    }
    
    if (firstPartSize == 0 || secondPartSize == 0) {
        return "Could not determine learning phase";
    }
    
    firstPartMean /= firstPartSize;
    secondPartMean /= secondPartSize;
    
    double difference = std::abs(secondPartMean - firstPartMean);
    double relativeChange = difference / (std::abs(firstPartMean) + 1e-10);
    
    if (relativeChange < stabilityThreshold) {
        return "STABLE - Learning has reached a plateau";
    } else {
        return "UNSTABLE - Learning is still ongoing or oscillatory";
    }
}

std::string detectPlateau(const std::vector<double>& metricValues,
                          double plateauThreshold,
                          int plateauLength) {
    if (metricValues.size() < static_cast<size_t>(plateauLength)) {
        return "No plateau detected - insufficient data";
    }
    
    for (size_t i = 0; i <= metricValues.size() - static_cast<size_t>(plateauLength); ++i) {
        double windowMean = calculateMean(std::vector<double>(metricValues.begin() + i, 
                                                             metricValues.begin() + i + plateauLength));
        
        double maxDiff = 0.0;
        for (size_t j = i; j < i + static_cast<size_t>(plateauLength); ++j) {
            double diff = std::abs(metricValues[j] - windowMean);
            if (diff > maxDiff) maxDiff = diff;
        }
        
        if (maxDiff < plateauThreshold) {
            return "Plateau detected at step " + std::to_string(i);
        }
    }
    
    return "No plateau detected in the time series";
}

std::string generatePlotData(const std::vector<std::pair<double, double>>& points,
                            const std::string& format) {
    if (format == "CSV") {
        std::ostringstream oss;
        oss << "x,y\n";
        for (const auto& point : points) {
            oss << point.first << "," << point.second << "\n";
        }
        return oss.str();
    } else if (format == "JSON") {
        std::ostringstream oss;
        oss << "{\n  \"data\": [\n";
        for (size_t i = 0; i < points.size(); ++i) {
            oss << "    {\"x\": " << points[i].first << ", \"y\": " << points[i].second;
            if (i < points.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "  ]\n}";
        return oss.str();
    } else {
        return "Unsupported format: " + format;
    }
}

std::string generateHistogramData(const std::vector<double>& data,
                                  const std::vector<double>& bins) {
    if (data.empty() || bins.empty()) {
        return "Invalid data for histogram";
    }
    
    std::vector<size_t> histogram(bins.size() - 1, 0);
    double minValue = *std::min_element(data.begin(), data.end());
    double maxValue = *std::max_element(data.begin(), data.end());
    
    for (double value : data) {
        // Find bin
        for (size_t i = 0; i < bins.size() - 1; ++i) {
            if (value >= bins[i] && value < bins[i + 1]) {
                histogram[i]++;
                break;
            }
        }
        // Handle upper bound
        if (value >= bins.back()) {
            histogram.back()++;
        }
    }
    
    std::ostringstream oss;
    oss << "bins,frequency\n";
    for (size_t i = 0; i < histogram.size(); ++i) {
        oss << bins[i] << "-" << bins[i + 1] << "," << histogram[i] << "\n";
    }
    return oss.str();
}

std::string generateTimeSeriesData(const std::vector<double>& timeSeries,
                                   const std::vector<double>& values) {
    if (timeSeries.size() != values.size() || timeSeries.empty()) {
        return "Invalid time series data";
    }
    
    std::ostringstream oss;
    oss << "time,value\n";
    for (size_t i = 0; i < timeSeries.size(); ++i) {
        oss << timeSeries[i] << "," << values[i] << "\n";
    }
    return oss.str();
}

} // namespace workflow_utils

struct ResearchWorkflow::Impl {
    std::shared_ptr<ResearchConfigManager> configManager;
    std::shared_ptr<ExperimentRunner> experimentRunner;
    std::shared_ptr<ParallelBatchProcessor> parallelProcessor;
    
    // Workflow management
    std::map<std::string, std::vector<std::string>> workflowSteps;
    std::map<std::string, std::map<std::string, WorkflowStepResult>> stepResults;
    std::map<std::string, WorkflowStatus> workflowStatuses;
    std::map<std::string, std::vector<std::string>> workflowConfigurations;
    std::map<std::string, std::vector<std::shared_ptr<Experiment>>> workflowExperiments;
    std::map<std::string, std::chrono::system_clock::time_point> workflowStartTimes;
    std::map<std::string, std::chrono::system_clock::time_point> workflowEndTimes;
    
    // Configuration management
    std::map<std::string, ResearchConfig> configurations;
    
    // Control and synchronization
    std::mutex workflowsMutex;
    std::condition_variable workflowsCondition;
    std::map<std::string, std::thread> workflowThreads;
    std::atomic<bool> stopping;
    
    // Current workflow tracking
    std::string currentWorkflowId;
    std::atomic<bool> workflowRunning;
    
    Impl() : stopping(false), currentWorkflowId(""), workflowRunning(false) {}
    
    ~Impl() {
        stop();
    }
};

ResearchWorkflow::ResearchWorkflow() : pImpl(std::make_unique<Impl>()) {
}

ResearchWorkflow::~ResearchWorkflow() = default;

void ResearchWorkflow::setConfigManager(std::shared_ptr<ResearchConfigManager> configManager) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    pImpl->configManager = configManager;
}

void ResearchWorkflow::setExperimentRunner(std::shared_ptr<ExperimentRunner> runner) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    pImpl->experimentRunner = runner;
}

void ResearchWorkflow::setParallelProcessor(std::shared_ptr<ParallelBatchProcessor> processor) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    pImpl->parallelProcessor = processor;
}

std::string ResearchWorkflow::createSequentialWorkflow(const std::vector<std::string>& configNames) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::string workflowId = "seq_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Validate configurations
    std::vector<std::string> validConfigs;
    for (const std::string& configName : configNames) {
        if (pImpl->configurations.find(configName) != pImpl->configurations.end()) {
            validConfigs.push_back(configName);
        }
    }
    
    if (validConfigs.empty()) {
        throw std::runtime_error("No valid configurations found for sequential workflow");
    }
    
    pImpl->workflowConfigurations[workflowId] = validConfigs;
    pImpl->workflowSteps[workflowId] = {"initialization", "execution", "analysis", "reporting"};
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::NotStarted;
    pImpl->workflowStartTimes[workflowId] = std::chrono::system_clock::now();
    
    // Initialize step results
    for (const std::string& step : pImpl->workflowSteps[workflowId]) {
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)] = WorkflowStepResult();
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)].stepName = step;
    }
    
    return workflowId;
}

std::string ResearchWorkflow::createCrossComparisonWorkflow(const std::vector<std::string>& configNames) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::string workflowId = "cross_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Validate configurations
    std::vector<std::string> validConfigs;
    for (const std::string& configName : configNames) {
        if (pImpl->configurations.find(configName) != pImpl->configurations.end()) {
            validConfigs.push_back(configName);
        }
    }
    
    if (validConfigs.size() < 2) {
        throw std::runtime_error("At least 2 configurations required for cross-comparison workflow");
    }
    
    pImpl->workflowConfigurations[workflowId] = validConfigs;
    pImpl->workflowSteps[workflowId] = {"baseline_setup", "experiment_execution", "comparison", "analysis"};
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::NotStarted;
    pImpl->workflowStartTimes[workflowId] = std::chrono::system_clock::now();
    
    // Initialize step results
    for (const std::string& step : pImpl->workflowSteps[workflowId]) {
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)] = WorkflowStepResult();
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)].stepName = step;
    }
    
    return workflowId;
}

std::string ResearchWorkflow::createAblationWorkflow(const std::string& baselineConfig,
                                                   const std::vector<std::string>& ablationTargets) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::string workflowId = "abl_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Validate baseline configuration
    if (pImpl->configurations.find(baselineConfig) == pImpl->configurations.end()) {
        throw std::runtime_error("Baseline configuration not found: " + baselineConfig);
    }
    
    // Create configuration list: baseline + ablations
    std::vector<std::string> configNames = {baselineConfig};
    configNames.insert(configNames.end(), ablationTargets.begin(), ablationTargets.end());
    
    pImpl->workflowConfigurations[workflowId] = configNames;
    pImpl->workflowSteps[workflowId] = {"baseline_setup", "ablation_setup", "experiment_execution", "comparison"};
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::NotStarted;
    pImpl->workflowStartTimes[workflowId] = std::chrono::system_clock::now();
    
    // Initialize step results
    for (const std::string& step : pImpl->workflowSteps[workflowId]) {
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)] = WorkflowStepResult();
        pImpl->stepResults[workflowId][generateStepId(workflowId, step)].stepName = step;
    }
    
    return workflowId;
}

std::string ResearchWorkflow::executeWorkflow(const std::string& workflowId) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        throw std::runtime_error("Workflow not found: " + workflowId);
    }
    
    if (pImpl->workflowStatuses[workflowId] != WorkflowStatus::NotStarted) {
        throw std::runtime_error("Workflow already started or completed: " + workflowId);
    }
    
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::InProgress;
    pImpl->workflowRunning = true;
    pImpl->currentWorkflowId = workflowId;
    
    // Start workflow thread
    pImpl->workflowThreads[workflowId] = std::thread(&ResearchWorkflow::workflowThread, this, workflowId);
    
    return workflowId;
}

bool ResearchWorkflow::pauseWorkflow(const std::string& workflowId) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return false;
    }
    
    if (pImpl->workflowStatuses[workflowId] != WorkflowStatus::InProgress) {
        return false; // Can only pause running workflows
    }
    
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::InProgress; // Still running but paused
    return true;
}

bool ResearchWorkflow::resumeWorkflow(const std::string& workflowId) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return false;
    }
    
    if (pImpl->workflowStatuses[workflowId] != WorkflowStatus::InProgress) {
        return false;
    }
    
    // Resume the workflow thread
    if (pImpl->workflowThreads.find(workflowId) != pImpl->workflowThreads.end()) {
        pImpl->workflowThreads[workflowId].detach();
        pImpl->workflowThreads.erase(workflowId);
    }
    
    pImpl->workflowThreads[workflowId] = std::thread(&ResearchWorkflow::workflowThread, this, workflowId);
    return true;
}

bool ResearchWorkflow::cancelWorkflow(const std::string& workflowId) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return false;
    }
    
    pImpl->workflowStatuses[workflowId] = WorkflowStatus::Cancelled;
    
    // Stop workflow thread if running
    if (pImpl->workflowThreads.find(workflowId) != pImpl->workflowThreads.end()) {
        pImpl->workflowThreads[workflowId].detach();
        pImpl->workflowThreads.erase(workflowId);
    }
    
    return true;
}

const std::string& ResearchWorkflow::getCurrentWorkflowId() const {
    return pImpl->currentWorkflowId;
}

WorkflowStatus ResearchWorkflow::getWorkflowStatus(const std::string& workflowId) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->workflowStatuses.find(workflowId);
    if (it != pImpl->workflowStatuses.end()) {
        return it->second;
    }
    
    return WorkflowStatus::NotStarted;
}

std::vector<std::string> ResearchWorkflow::getWorkflowIds() const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::vector<std::string> ids;
    for (const auto& pair : pImpl->workflowStatuses) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> ResearchWorkflow::getActiveWorkflowIds() const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::vector<std::string> ids;
    for (const auto& pair : pImpl->workflowStatuses) {
        if (pair.second == WorkflowStatus::InProgress) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

std::vector<std::string> ResearchWorkflow::getWorkflowSteps(const std::string& workflowId) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->workflowSteps.find(workflowId);
    if (it != pImpl->workflowSteps.end()) {
        return it->second;
    }
    
    return {};
}

const WorkflowStepResult& ResearchWorkflow::getStepResult(const std::string& workflowId,
                                                          const std::string& stepName) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::string stepId = generateStepId(workflowId, stepName);
    auto it = pImpl->stepResults[workflowId].find(stepId);
    if (it != pImpl->stepResults[workflowId].end()) {
        return it->second;
    }
    
    static WorkflowStepResult emptyResult;
    return emptyResult;
}

std::map<std::string, WorkflowStepResult> ResearchWorkflow::getAllStepResults(const std::string& workflowId) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->stepResults.find(workflowId);
    if (it != pImpl->stepResults.end()) {
        return it->second;
    }
    
    return {};
}

std::string ResearchWorkflow::runStatisticalAnalysis(const std::string& workflowId,
                                                    const std::string& analysisType,
                                                    const std::string& metricName) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return "Workflow not found: " + workflowId;
    }
    
    // Route to specific analysis functions
    if (analysisType == "prediction_error") {
        return analyzePredictionError(workflowId, metricName);
    } else if (analysisType == "learning_curve") {
        return analyzeLearningCurve(workflowId, metricName);
    } else if (analysisType == "retention") {
        return analyzeRetention(workflowId, metricName);
    } else if (analysisType == "performance") {
        return analyzePerformance(workflowId, metricName);
    } else {
        return "Unknown analysis type: " + analysisType;
    }
}

std::string ResearchWorkflow::generateStatisticalReport(const std::string& workflowId) const {
    std::ostringstream oss;
    oss << "NLM Research Workflow - Statistical Report\n";
    oss << "===========================================" << "\n\n";
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return "Workflow not found: " + workflowId;
    }
    
    oss << "Workflow ID: " << workflowId << "\n";
    oss << "Status: " << static_cast<int>(pImpl->workflowStatuses.at(workflowId)) << "\n\n";
    
    // Get all step results
    auto stepResults = getAllStepResults(workflowId);
    oss << "Step Results:\n";
    for (const auto& stepPair : stepResults) {
        const WorkflowStepResult& result = stepPair.second;
        oss << "  " << result.stepName << ": " << static_cast<int>(result.status) << "\n";
    }
    
    return oss.str();
}

std::string ResearchWorkflow::generateComparisonReport(const std::string& workflowId) const {
    std::ostringstream oss;
    oss << "NLM Research Workflow - Comparison Report\n";
    oss << "=========================================" << "\n\n";
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return "Workflow not found: " + workflowId;
    }
    
    // Generate comparison results for all config pairs
    auto configs = getWorkflowConfigurations(workflowId);
    oss << "Configurations:\n";
    for (const std::string& configName : configs) {
        oss << "  - " << configName << "\n";
    }
    oss << "\n";
    
    // TODO: Implement actual comparison logic
    oss << "Comparison results would be generated here...\n";
    
    return oss.str();
}

bool ResearchWorkflow::generateVisualizations(const std::string& workflowId,
                                             const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->workflowStatuses.find(workflowId) == pImpl->workflowStatuses.end()) {
        return false;
    }
    
    // Route to specific visualization functions
    if (config.metricsToVisualize.empty() || 
        std::find(config.metricsToVisualize.begin(), config.metricsToVisualize.end(), "learning_curve") != config.metricsToVisualize.end()) {
        return createLearningCurvePlot(workflowId, config);
    } else if (std::find(config.metricsToVisualize.begin(), config.metricsToVisualize.end(), "comparison") != config.metricsToVisualize.end()) {
        return createComparisonPlot(workflowId, config);
    } else if (std::find(config.metricsToVisualize.begin(), config.metricsToVisualize.end(), "distribution") != config.metricsToVisualize.end()) {
        return createMetricsDistributionPlot(workflowId, config);
    }
    
    return false;
}

bool ResearchWorkflow::saveVisualization(const std::string& workflowId,
                                        const std::string& filename) const {
    std::ostringstream oss;
    oss << "Visualization saved to: " << filename << " for workflow " << workflowId;
    
    // TODO: Implement actual file saving
    return true;
}

std::shared_ptr<Experiment> ResearchWorkflow::getExperimentResult(const std::string& workflowId,
                                                                 const std::string& stepName) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->workflowExperiments.find(workflowId);
    if (it != pImpl->workflowExperiments.end()) {
        for (const auto& experiment : it->second) {
            if (experiment->getName() == stepName) {
                return experiment;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Experiment>> ResearchWorkflow::getAllWorkflowResults(const std::string& workflowId) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->workflowExperiments.find(workflowId);
    if (it != pImpl->workflowExperiments.end()) {
        return it->second;
    }
    
    return {};
}

bool ResearchWorkflow::saveWorkflowResults(const std::string& workflowId, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Research Workflow Results\n";
        file << "# Workflow: " << workflowId << "\n\n";
        
        auto experiments = getAllWorkflowResults(workflowId);
        file << "Experiments: " << experiments.size() << "\n";
        
        for (const auto& experiment : experiments) {
            file << "Experiment: " << experiment->getName() << "\n";
            file << "  Seed: " << experiment->getSeed() << "\n";
            file << "  Metrics:\n";
            
            const auto& metrics = experiment->getMetrics();
            for (const auto& metric : metrics) {
                file << "    " << metric.first << ": " << metric.second << "\n";
            }
            file << "  Steps:\n";
            
            const auto& steps = experiment->getStepData();
            for (const auto& step : steps) {
                file << "    Step " << step.step << ": " 
                     << "Neurons: " << step.neuronCount << " "
                     << "Synapses: " << step.synapseCount << " "
                     << "Firing: " << step.firingCount << " "
                     << "Avg Rate: " << step.avgFiringRate << "\n";
            }
            file << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ResearchWorkflow::addConfiguration(const ResearchConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    if (pImpl->configurations.find(config.name) != pImpl->configurations.end()) {
        return false; // Already exists
    }
    
    pImpl->configurations[config.name] = config;
    return true;
}

bool ResearchWorkflow::removeConfiguration(const std::string& configName) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    return pImpl->configurations.erase(configName) > 0;
}

const ResearchConfig* ResearchWorkflow::getConfiguration(const std::string& configName) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->configurations.find(configName);
    if (it != pImpl->configurations.end()) {
        return &it->second;
    }
    
    return nullptr;
}

std::vector<std::string> ResearchWorkflow::getConfigurationNames() const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->configurations) {
        names.push_back(pair.first);
    }
    return names;
}

double ResearchWorkflow::getWorkflowProgress(const std::string& workflowId) const {
    return calculateWorkflowProgress(workflowId);
}

std::chrono::duration<double> ResearchWorkflow::getEstimatedTimeRemaining(const std::string& workflowId) const {
    // Calculate based on completed steps vs total steps
    auto steps = getWorkflowSteps(workflowId);
    if (steps.empty()) {
        return std::chrono::duration<double>(0.0);
    }
    
    size_t completed = getCompletedSteps(workflowId);
    double progress = static_cast<double>(completed) / steps.size();
    
    // Estimate remaining time (assuming each step takes ~1 minute for setup)
    double estimatedRemaining = (1.0 - progress) * 60.0; // seconds
    
    return std::chrono::duration<double>(estimatedRemaining);
}

std::string ResearchWorkflow::getCurrentWorkflowProgressReport(const std::string& workflowId) const {
    std::ostringstream oss;
    
    auto steps = getWorkflowSteps(workflowId);
    size_t completed = getCompletedSteps(workflowId);
    
    oss << "Progress Report for Workflow: " << workflowId << "\n";
    oss << "=======================================" << "\n";
    oss << "Completed Steps: " << completed << "/" << steps.size() << " (" 
        << (steps.empty() ? 0.0 : (completed * 100.0 / steps.size())) << "%)\n";
    
    oss << "\nSteps:\n";
    for (size_t i = 0; i < steps.size(); ++i) {
        const WorkflowStepResult& result = getStepResult(workflowId, steps[i]);
        oss << "  [" << (i < completed ? "✓" : "○") << "] " << steps[i];
        if (result.status != WorkflowStatus::NotStarted) {
            oss << " (" << static_cast<int>(result.status) << ")";
        }
        oss << "\n";
    }
    
    auto remainingTime = getEstimatedTimeRemaining(workflowId);
    oss << "\nEstimated Time Remaining: " << remainingTime.count() << " seconds\n";
    
    return oss.str();
}

std::string ResearchWorkflow::generateProgressReport(const std::string& workflowId) const {
    return getCurrentWorkflowProgressReport(workflowId);
}

std::string ResearchWorkflow::generateDetailedReport(const std::string& workflowId) const {
    std::ostringstream oss;
    oss << "NLM Research Workflow - Detailed Report\n";
    oss << "==========================================" << "\n\n";
    
    oss << "Workflow Information:\n";
    oss << "  ID: " << workflowId << "\n";
    oss << "  Status: " << static_cast<int>(getWorkflowStatus(workflowId)) << "\n";
    
    auto configs = getWorkflowConfigurations(workflowId);
    oss << "  Configurations: " << configs.size() << "\n";
    for (const auto& configName : configs) {
        const ResearchConfig* config = getConfiguration(configName);
        if (config) {
            oss << "    - " << config->name << ": " << config->description << "\n";
        }
    }
    
    oss << "\nStep Results:\n";
    auto stepResults = getAllStepResults(workflowId);
    for (const auto& stepPair : stepResults) {
        const WorkflowStepResult& result = stepPair.second;
        oss << "  " << result.stepName << ": " << static_cast<int>(result.status) << "\n";
        if (!result.description.empty()) {
            oss << "    Description: " << result.description << "\n";
        }
        if (result.executionTime.count() > 0) {
            oss << "    Execution Time: " << result.executionTime.count() << " seconds\n";
        }
        if (!result.errorMessage.empty()) {
            oss << "    Error: " << result.errorMessage << "\n";
        }
    }
    
    oss << "\nExperiment Results:\n";
    auto experiments = getAllWorkflowResults(workflowId);
    for (const auto& experiment : experiments) {
        oss << "  " << experiment->getName() << " (Seed: " << experiment->getSeed() << ")\n";
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            oss << "    " << metric.first << ": " << metric.second << "\n";
        }
    }
    
    return oss.str();
}

std::string ResearchWorkflow::generateCompleteReport(const std::string& workflowId) const {
    std::ostringstream oss;
    oss << "=== COMPLETE NLM RESEARCH WORKFLOW REPORT ===\n";
    oss << "Workflow: " << workflowId << "\n";
    oss << "Generated: " << workflow_utils::formatTimePoint(std::chrono::system_clock::now()) << "\n\n";
    
    oss << generateDetailedReport(workflowId) << "\n";
    
    oss << "=== Statistical Analysis ===\n";
    // Add statistical analysis results
    oss << "Statistical tests would be included here based on the experiment data.\n\n";
    
    oss << "=== Conclusions and Recommendations ===\n";
    oss << "Analysis of results and research conclusions would appear here.\n";
    
    return oss.str();
}

void ResearchWorkflow::workflowThread(const std::string& workflowId) {
    try {
        // Execute workflow steps sequentially
        auto steps = getWorkflowSteps(workflowId);
        
        for (size_t i = 0; i < steps.size() && !pImpl->stopping; ++i) {
            std::string stepName = steps[i];
            
            // Update workflow status
            {
                std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
                pImpl->workflowStatuses[workflowId] = WorkflowStatus::InProgress;
            }
            
            // Execute the step
            bool success = executeStep(workflowId, stepName);
            
            // Update step result
            {
                std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
                
                std::string stepId = generateStepId(workflowId, stepName);
                if (pImpl->stepResults[workflowId].find(stepId) != pImpl->stepResults[workflowId].end()) {
                    pImpl->stepResults[workflowId][stepId].status = success ? WorkflowStatus::Completed : WorkflowStatus::Failed;
                    pImpl->stepResults[workflowId][stepId].executionTime = 
                        std::chrono::duration<double>(10.0); // Simulated execution time
                }
            }
        }
        
        // Complete workflow
        {
            std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
            pImpl->workflowStatuses[workflowId] = WorkflowStatus::Completed;
            pImpl->workflowEndTimes[workflowId] = std::chrono::system_clock::now();
            pImpl->currentWorkflowId = "";
            pImpl->workflowRunning = false;
        }
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
        
        pImpl->workflowStatuses[workflowId] = WorkflowStatus::Failed;
        pImpl->workflowEndTimes[workflowId] = std::chrono::system_clock::now();
        
        // Record error in current step
        auto steps = getWorkflowSteps(workflowId);
        if (!steps.empty()) {
            std::string currentStep = steps.back();
            std::string stepId = generateStepId(workflowId, currentStep);
            if (pImpl->stepResults[workflowId].find(stepId) != pImpl->stepResults[workflowId].end()) {
                pImpl->stepResults[workflowId][stepId].errorMessage = e.what();
            }
        }
        
        pImpl->currentWorkflowId = "";
        pImpl->workflowRunning = false;
    }
}

bool ResearchWorkflow::executeStep(const std::string& workflowId, const std::string& stepName) {
    if (!pImpl->experimentRunner || !pImpl->configManager) {
        return false;
    }
    
    auto configs = getWorkflowConfigurations(workflowId);
    if (configs.empty()) {
        return false;
    }
    
    // For each configuration, run the experiment
    for (const std::string& configName : configs) {
        const ResearchConfig* config = getConfiguration(configName);
        if (!config) continue;
        
        // Convert research config to core config
        nlm::Config coreConfig = pImpl->configManager->toCoreConfig();
        
        // Create experiment
        auto experiment = std::make_shared<Experiment>(configName);
        experiment->setSeed(config->random_seed);
        
        // TODO: In a real implementation, we would:
        // 1. Create a brain and environment based on the configuration
        // 2. Run the experiment using the experiment runner
        // 3. Record the results
        
        // For now, simulate experiment execution
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Record some metrics
        experiment->recordMetric("total_reward", 100.0 + config->random_seed * 10.0);
        experiment->recordMetric("final_firing_rate", 0.5 + config->random_seed * 0.01);
        
        // Store experiment result
        {
            std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
            pImpl->workflowExperiments[workflowId].push_back(experiment);
        }
    }
    
    return true;
}

void ResearchWorkflow::updateStepProgress(const std::string& workflowId, const std::string& stepName, double progress) {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    std::string stepId = generateStepId(workflowId, stepName);
    if (pImpl->stepResults[workflowId].find(stepId) != pImpl->stepResults[workflowId].end()) {
        pImpl->stepResults[workflowId][stepId].progress = progress;
    }
}

std::string ResearchWorkflow::analyzePredictionError(const std::string& workflowId,
                                                    const std::string& metricName) const {
    std::ostringstream oss;
    oss << "Prediction Error Analysis for Workflow: " << workflowId << "\n";
    oss << "Metric: " << metricName << "\n\n";
    
    auto experiments = getAllWorkflowResults(workflowId);
    if (experiments.empty()) {
        oss << "No experiment data available for analysis\n";
        return oss.str();
    }
    
    // Collect metric values across experiments
    std::vector<double> metricValues;
    for (const auto& experiment : experiments) {
        if (experiment->hasMetric(metricName)) {
            metricValues.push_back(experiment->getMetric(metricName));
        }
    }
    
    if (metricValues.empty()) {
        oss << "No data found for metric: " << metricName << "\n";
        return oss.str();
    }
    
    // Perform statistical analysis
    double mean = workflow_utils::calculateMean(metricValues);
    double stddev = workflow_utils::calculateStdDev(metricValues, mean);
    
    oss << "Statistics:\n";
    oss << "  Mean: " << mean << "\n";
    oss << "  Standard Deviation: " << stddev << "\n";
    oss << "  Coefficient of Variation: " << workflow_utils::calculateCoefficientOfVariation(metricValues, mean) << "%\n\n";
    
    // Identify learning phase
    std::string learningPhase = workflow_utils::identifyLearningPhase(metricValues);
    oss << "Learning Phase Analysis:\n" << learningPhase << "\n\n";
    
    // Detect plateau
    std::string plateau = workflow_utils::detectPlateau(metricValues);
    oss << "Plateau Detection:\n" << plateau << "\n\n";
    
    return oss.str();
}

std::string ResearchWorkflow::analyzeLearningCurve(const std::string& workflowId,
                                                   const std::string& metricName) const {
    std::ostringstream oss;
    oss << "Learning Curve Analysis for Workflow: " << workflowId << "\n";
    oss << "Metric: " << metricName << "\n\n";
    
    auto experiments = getAllWorkflowResults(workflowId);
    if (experiments.empty()) {
        oss << "No experiment data available for analysis\n";
        return oss.str();
    }
    
    // Group experiments by configuration (if available)
    std::map<std::string, std::vector<double>> configMetrics;
    for (const auto& experiment : experiments) {
        // Use experiment name as proxy for configuration
        if (experiment->hasMetric(metricName)) {
            configMetrics[experiment->getName()].push_back(experiment->getMetric(metricName));
        }
    }
    
    oss << "Results by Configuration:\n";
    for (const auto& pair : configMetrics) {
        const auto& values = pair.second;
        double mean = workflow_utils::calculateMean(values);
        double stddev = workflow_utils::calculateStdDev(values, mean);
        
        oss << "  " << pair.first << ": n=" << values.size() << ", mean=" << mean 
            << ", stddev=" << stddev << "\n";
    }
    
    oss << "\nOverall Statistics:\n";
    std::vector<double> allValues;
    for (const auto& pair : configMetrics) {
        allValues.insert(allValues.end(), pair.second.begin(), pair.second.end());
    }
    
    if (!allValues.empty()) {
        double mean = workflow_utils::calculateMean(allValues);
        double stddev = workflow_utils::calculateStdDev(allValues, mean);
        
        oss << "  Total n=" << allValues.size() << ", mean=" << mean 
            << ", stddev=" << stddev << "\n";
        
        // Calculate learning rate (simplified)
        std::sort(allValues.begin(), allValues.end());
        double firstQuartile = allValues.size() > 0 ? allValues[allValues.size() / 4] : 0;
        double thirdQuartile = allValues.size() > 0 ? allValues[3 * allValues.size() / 4] : 0;
        double interquartileRange = thirdQuartile - firstQuartile;
        
        oss << "  Interquartile Range: " << interquartileRange << "\n";
        oss << "  Coefficient of Variation: " << workflow_utils::calculateCoefficientOfVariation(allValues, mean) << "%\n";
    }
    
    return oss.str();
}

std::string ResearchWorkflow::analyzeRetention(const std::string& workflowId,
                                               const std::string& metricName) const {
    std::ostringstream oss;
    oss << "Retention Analysis for Workflow: " << workflowId << "\n";
    oss << "Metric: " << metricName << "\n\n";
    
    // TODO: Implement retention analysis based on experiment step data
    oss << "Retention analysis requires access to step-by-step experiment data.\n";
    oss << "This would analyze how metrics are maintained over time after the initial learning phase.\n";
    
    return oss.str();
}

std::string ResearchWorkflow::analyzePerformance(const std::string& workflowId,
                                                 const std::string& metricName) const {
    std::ostringstream oss;
    oss << "Performance Analysis for Workflow: " << workflowId << "\n";
    oss << "Metric: " << metricName << "\n\n";
    
    auto experiments = getAllWorkflowResults(workflowId);
    if (experiments.empty()) {
        oss << "No experiment data available for analysis\n";
        return oss.str();
    }
    
    // Calculate performance statistics
    std::vector<double> metricValues;
    for (const auto& experiment : experiments) {
        if (experiment->hasMetric(metricName)) {
            metricValues.push_back(experiment->getMetric(metricName));
        }
    }
    
    if (metricValues.empty()) {
        oss << "No data found for metric: " << metricName << "\n";
        return oss.str();
    }
    
    double mean = workflow_utils::calculateMean(metricValues);
    double stddev = workflow_utils::calculateStdDev(metricValues, mean);
    
    oss << "Performance Statistics:\n";
    oss << "  Mean: " << mean << "\n";
    oss << "  Standard Deviation: " << stddev << "\n";
    oss << "  Coefficient of Variation: " << workflow_utils::calculateCoefficientOfVariation(metricValues, mean) << "%\n";
    
    // Performance classification
    if (metricName.find("reward") != std::string::npos || 
        metricName.find("performance") != std::string::npos) {
        oss << "\nPerformance Classification:\n";
        if (mean > 0.8 * metricValues.back()) {
            oss << "  State: Excellent - High performance maintained\n";
        } else if (mean > 0.5 * metricValues.back()) {
            oss << "  State: Good - Acceptable performance levels\n";
        } else {
            oss << "  State: Needs Improvement - Low performance levels\n";
        }
    }
    
    return oss.str();
}

bool ResearchWorkflow::createLearningCurvePlot(const std::string& workflowId,
                                              const VisualizationConfig& config) const {
    std::ostringstream oss;
    oss << "Learning Curve Plot for Workflow: " << workflowId << "\n";
    oss << "Format: " << config.outputFormat << "\n";
    oss << "Output Path: " << config.outputPath << "\n";
    
    // TODO: Implement actual plotting using a plotting library
    oss << "Plot would be generated showing learning curves for all configurations\n";
    
    return true;
}

bool ResearchWorkflow::createComparisonPlot(const std::string& workflowId,
                                           const VisualizationConfig& config) const {
    std::ostringstream oss;
    oss << "Comparison Plot for Workflow: " << workflowId << "\n";
    oss << "Format: " << config.outputFormat << "\n";
    oss << "Output Path: " << config.outputPath << "\n";
    
    // TODO: Implement actual plotting
    oss << "Plot would be generated comparing configurations side-by-side\n";
    
    return true;
}

bool ResearchWorkflow::createMetricsDistributionPlot(const std::string& workflowId,
                                                    const VisualizationConfig& config) const {
    std::ostringstream oss;
    oss << "Metrics Distribution Plot for Workflow: " << workflowId << "\n";
    oss << "Format: " << config.outputFormat << "\n";
    oss << "Output Path: " << config.outputPath << "\n";
    
    // TODO: Implement actual plotting
    oss << "Plot would be generated showing distribution of metrics across experiments\n";
    
    return true;
}

std::string ResearchWorkflow::generateStepId(const std::string& workflowId, const std::string& stepName) const {
    return workflowId + "_step_" + stepName;
}

size_t ResearchWorkflow::getCompletedSteps(const std::string& workflowId) const {
    auto stepResults = getAllStepResults(workflowId);
    size_t completed = 0;
    for (const auto& stepPair : stepResults) {
        const WorkflowStepResult& result = stepPair.second;
        if (result.status == WorkflowStatus::Completed) {
            completed++;
        }
    }
    return completed;
}

double ResearchWorkflow::calculateWorkflowProgress(const std::string& workflowId) const {
    auto steps = getWorkflowSteps(workflowId);
    if (steps.empty()) return 0.0;
    
    size_t completed = getCompletedSteps(workflowId);
    return static_cast<double>(completed) / steps.size();
}

std::vector<std::string> ResearchWorkflow::getWorkflowConfigurations(const std::string& workflowId) const {
    std::lock_guard<std::mutex> lock(pImpl->workflowsMutex);
    
    auto it = pImpl->workflowConfigurations.find(workflowId);
    if (it != pImpl->workflowConfigurations.end()) {
        return it->second;
    }
    
    return {};
}

void ResearchWorkflow::notifyWorkflowThreads() {
    pImpl->workflowsCondition.notify_all();
}

void ResearchWorkflow::waitForWorkflowThreads(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(pImpl->workflowsMutex);
    pImpl->workflowsCondition.wait_for(lock, timeout, [this]() {
        return pImpl->workflowThreads.empty() && !pImpl->workflowRunning;
    });
}

} // namespace nlm