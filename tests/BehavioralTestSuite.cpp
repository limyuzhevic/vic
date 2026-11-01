#include "BehavioralTestSuite.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

BehavioralTestSuite::BehavioralTestSuite() = default;

BehavioralTestSuite::~BehavioralTestSuite() = default;

void BehavioralTestSuite::addTest(const std::string& name, const std::string& description,
                                 std::function<bool(BehavioralTestResults&)> testFunction) {
    tests_[name] = std::make_pair(description, testFunction);
}

bool BehavioralTestSuite::runTest(const std::string& name, BehavioralTestResults& results) {
    auto it = tests_.find(name);
    if (it == tests_.end()) {
        return false;
    }
    
    return it->second.second(results);
}

std::vector<BehavioralTestResults> BehavioralTestSuite::runAllTests() {
    std::vector<BehavioralTestResults> allResults;
    
    for (auto& test : tests_) {
        BehavioralTestResults results;
        results.testName = test.first;
        results.testDescription = test.second.first;
        
        bool success = runTest(test.first, results);
        results.sampleSize = success ? 100 : 0; // Placeholder
        results.successCount = success ? 90 : 0; // Placeholder
        results.failureCount = success ? 10 : 0; // Placeholder
        
        allResults.push_back(results);
        results_[test.first] = results;
    }
    
    return allResults;
}

std::vector<std::string> BehavioralTestSuite::getTestNames() const {
    std::vector<std::string> names;
    for (const auto& test : tests_) {
        names.push_back(test.first);
    }
    return names;
}

const BehavioralTestResults* BehavioralTestSuite::getTestResult(const std::string& name) const {
    auto it = results_.find(name);
    if (it == results_.end()) {
        return nullptr;
    }
    return &it->second;
}

void BehavioralTestSuite::clearResults() {
    results_.clear();
}

std::string BehavioralTestSuite::getSuiteSummary() const {
    std::stringstream ss;
    ss << "=== Behavioral Test Suite Summary ===" << std::endl;
    ss << "Total Tests: " << tests_.size() << std::endl;
    ss << "Average Accuracy: " << (getAverageAccuracy() * 100.0f) << "%" << std::endl;
    ss << "Average Selectivity: " << (getAverageSelectivity() * 100.0f) << "%" << std::endl;
    ss << "Average Learning Progress: " << (getAverageLearningProgress() * 100.0f) << "%" << std::endl;
    
    ss << std::endl;
    ss << "Detailed Results:" << std::endl;
    for (const auto& result : results_) {
        ss << result.first << ": " << (result.second.accuracy * 100.0f) << "% accuracy, "
           << (result.second.selectivity * 100.0f) << "% selectivity, "
           << (result.second.learningProgress * 100.0f) << "% learning progress" << std::endl;
    }
    
    return ss.str();
}

float BehavioralTestSuite::getAverageAccuracy() const {
    if (results_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& result : results_) {
        sum += result.second.accuracy;
    }
    
    return sum / static_cast<float>(results_.size());
}

float BehavioralTestSuite::getAverageSelectivity() const {
    if (results_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& result : results_) {
        sum += result.second.selectivity;
    }
    
    return sum / static_cast<float>(results_.size());
}

float BehavioralTestSuite::getAverageLearningProgress() const {
    if (results_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& result : results_) {
        sum += result.second.learningProgress;
    }
    
    return sum / static_cast<float>(results_.size());
}

float BehavioralTestSuite::calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0f;
    
    float xSum = std::accumulate(x.begin(), x.end(), 0.0f);
    float ySum = std::accumulate(y.begin(), y.end(), 0.0f);
    
    float xMean = xSum / static_cast<float>(x.size());
    float yMean = ySum / static_cast<float>(y.size());
    
    float numerator = 0.0f;
    float xVariance = 0.0f;
    float yVariance = 0.0f;
    
    for (size_t i = 0; i < x.size(); ++i) {
        float xDiff = x[i] - xMean;
        float yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        xVariance += xDiff * xDiff;
        yVariance += yDiff * yDiff;
    }
    
    float denominator = std::sqrt(xVariance * yVariance);
    if (denominator == 0.0f) return 0.0f;
    
    return numerator / denominator;
}

float BehavioralTestSuite::calculatePatternSeparation(const std::vector<std::vector<float>>& patterns) const {
    if (patterns.size() < 2) return 0.0f;
    
    float totalSeparation = 0.0f;
    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            float separation = 0.0f;
            for (size_t k = 0; k < patterns[i].size() && k < patterns[j].size(); ++k) {
                separation += std::abs(patterns[i][k] - patterns[j][k]);
            }
            totalSeparation += separation / static_cast<float>(patterns[i].size());
        }
    }
    
    float numPairs = static_cast<float>(patterns.size() * (patterns.size() - 1) / 2);
    return numPairs > 0 ? totalSeparation / numPairs : 0.0f;
}

float BehavioralTestSuite::calculateDiscriminationScore(const std::vector<float>& responses, const std::vector<float>& expected) const {
    if (responses.size() != expected.size() || responses.empty()) return 0.0f;
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < responses.size(); ++i) {
        totalDifference += std::abs(responses[i] - expected[i]);
    }
    
    return 1.0f - (totalDifference / static_cast<float>(responses.size()));
}

PatternDiscriminationTest::PatternDiscriminationTest() = default;

PatternDiscriminationTest::~PatternDiscriminationTest() = default;

void PatternDiscriminationTest::addPattern(const std::string& name, const std::vector<float>& pattern) {
    patterns_[name] = pattern;
}

void PatternDiscriminationTest::addResponse(const std::string& stimulus, const std::string& response, float confidence) {
    responses_[stimulus].push_back(std::make_pair(response, confidence));
}

BehavioralTestResults PatternDiscriminationTest::runTest() const {
    BehavioralTestResults results;
    results.testName = "Pattern Discrimination Test";
    results.testDescription = "Tests how well the neural system distinguishes between different patterns";
    
    if (patterns_.empty()) return results;
    
    // Calculate pattern similarity
    std::vector<std::vector<float>> patternVectors;
    std::vector<std::string> patternNames;
    
    for (const auto& pattern : patterns_) {
        patternVectors.push_back(pattern.second);
        patternNames.push_back(pattern.first);
    }
    
    results.patternSeparation = calculatePatternSeparation(patternVectors);
    
    // Calculate response consistency
    float totalSelectivity = 0.0f;
    size_t patternCount = 0;
    
    for (const auto& responsePair : responses_) {
        float selectivity = calculateResponseSelectivity(responsePair.second);
        totalSelectivity += selectivity;
        patternCount++;
    }
    
    results.selectivity = patternCount > 0 ? totalSelectivity / static_cast<float>(patternCount) : 0.0f;
    
    // Calculate accuracy (simplified)
    results.accuracy = 0.5f; // Placeholder
    results.sampleSize = responses_.size();
    
    return results;
}

std::vector<std::string> PatternDiscriminationTest::getPatternNames() const {
    std::vector<std::string> names;
    for (const auto& pattern : patterns_) {
        names.push_back(pattern.first);
    }
    return names;
}

std::vector<std::pair<std::string, float>> PatternDiscriminationTest::getResponses(const std::string& stimulus) const {
    auto it = responses_.find(stimulus);
    if (it == responses_.end()) {
        return std::vector<std::pair<std::string, float>>();
    }
    return it->second;
}

float PatternDiscriminationTest::calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        totalDifference += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return 1.0f - (totalDifference / static_cast<float>(pattern1.size()));
}

float PatternDiscriminationTest::calculateResponseSelectivity(const std::vector<std::pair<std::string, float>>& responses) const {
    if (responses.empty()) return 0.0f;
    
    std::map<std::string, float> responseStrengths;
    for (const auto& response : responses) {
        responseStrengths[response.first] += response.second;
    }
    
    float maxStrength = 0.0f;
    for (const auto& strength : responseStrengths) {
        maxStrength = std::max(maxStrength, strength.second);
    }
    
    return maxStrength > 0.0f ? maxStrength / static_cast<float>(responses.size()) : 0.0f;
}

ResponseSelectivityTest::ResponseSelectivityTest() = default;

ResponseSelectivityTest::~ResponseSelectivityTest() = default;

void ResponseSelectivityTest::addStimulusResponse(const std::string& stimulus, const std::string& response, float strength) {
    stimulusResponses_[stimulus].push_back(std::make_pair(response, strength));
}

void ResponseSelectivityTest::setExpectedResponse(const std::string& stimulus, const std::string& expectedResponse) {
    expectedResponses_[stimulus] = expectedResponse;
}

BehavioralTestResults ResponseSelectivityTest::runTest() const {
    BehavioralTestResults results;
    results.testName = "Response Selectivity Test";
    results.testDescription = "Tests how selective the neural system's responses are";
    
    if (stimulusResponses_.empty()) return results;
    
    // Calculate consistency
    float totalConsistency = 0.0f;
    size_t stimulusCount = 0;
    
    for (const auto& stimulusResponse : stimulusResponses_) {
        float consistency = calculateResponseConsistency(stimulusResponse.second);
        totalConsistency += consistency;
        stimulusCount++;
    }
    
    results.consistency = stimulusCount > 0 ? totalConsistency / static_cast<float>(stimulusCount) : 0.0f;
    
    // Calculate bias
    float totalBias = 0.0f;
    for (const auto& stimulusResponse : stimulusResponses_) {
        float bias = calculateResponseBias(stimulusResponse.second);
        totalBias += bias;
    }
    
    results.responseBias = stimulusCount > 0 ? totalBias / static_cast<float>(stimulusCount) : 0.0f;
    
    // Calculate accuracy
    size_t correctCount = 0;
    size_t totalCount = 0;
    
    for (const auto& stimulusResponse : stimulusResponses_) {
        auto it = expectedResponses_.find(stimulusResponse.first);
        if (it != expectedResponses_.end()) {
            totalCount++;
            // Simplified accuracy calculation
            float responseStrength = 0.0f;
            for (const auto& response : stimulusResponse.second) {
                if (response.first == it->second) {
                    responseStrength += response.second;
                }
            }
            if (responseStrength > 0.0f) {
                correctCount++;
            }
        }
    }
    
    results.accuracy = totalCount > 0 ? static_cast<float>(correctCount) / static_cast<float>(totalCount) : 0.0f;
    results.sampleSize = totalCount;
    results.successCount = correctCount;
    results.failureCount = totalCount - correctCount;
    
    return results;
}

std::vector<std::string> ResponseSelectivityTest::getStimulusNames() const {
    std::vector<std::string> names;
    for (const auto& stimulusResponse : stimulusResponses_) {
        names.push_back(stimulusResponse.first);
    }
    return names;
}

std::string ResponseSelectivityTest::getExpectedResponse(const std::string& stimulus) const {
    auto it = expectedResponses_.find(stimulus);
    if (it == expectedResponses_.end()) {
        return std::string();
    }
    return it->second;
}

float ResponseSelectivityTest::calculateResponseConsistency(const std::vector<std::pair<std::string, float>>& responses) const {
    if (responses.empty()) return 0.0f;
    
    std::map<std::string, float> responseStrengths;
    for (const auto& response : responses) {
        responseStrengths[response.first] += response.second;
    }
    
    float totalStrength = 0.0f;
    for (const auto& strength : responseStrengths) {
        totalStrength += strength.second;
    }
    
    return totalStrength > 0.0f ? responseStrengths.rbegin()->second / totalStrength : 0.0f;
}

float ResponseSelectivityTest::calculateResponseBias(const std::vector<std::pair<std::string, float>>& responses) const {
    if (responses.empty()) return 0.0f;
    
    std::map<std::string, float> responseStrengths;
    for (const auto& response : responses) {
        responseStrengths[response.first] += response.second;
    }
    
    float maxStrength = 0.0f;
    for (const auto& strength : responseStrengths) {
        maxStrength = std::max(maxStrength, strength.second);
    }
    
    return maxStrength / static_cast<float>(responses.size());
}

LearningProgressTest::LearningProgressTest() = default;

LearningProgressTest::~LearningProgressTest() = default;

void LearningProgressTest::addLearningPoint(double timestamp, float performance) {
    learningPoints_.push_back(std::make_pair(timestamp, performance));
}

BehavioralTestResults LearningProgressTest::runTest() const {
    BehavioralTestResults results;
    results.testName = "Learning Progress Test";
    results.testDescription = "Tests how well the neural system improves over time";
    
    if (learningPoints_.size() < 2) return results;
    
    // Calculate learning rate
    float learningRate = calculateLearningRate();
    results.learningProgress = learningRate > 0.0f ? std::min(learningRate / 0.1f, 1.0f) : 0.0f;
    
    // Calculate learning acceleration
    float acceleration = calculateLearningAcceleration();
    results.detailedMetrics["learning_acceleration"] = acceleration;
    
    // Calculate consistency
    std::vector<float> performances;
    for (const auto& point : learningPoints_) {
        performances.push_back(point.second);
    }
    
    float meanPerformance = std::accumulate(performances.begin(), performances.end(), 0.0f) / static_cast<float>(performances.size());
    float stdDev = 0.0f;
    
    for (float performance : performances) {
        stdDev += (performance - meanPerformance) * (performance - meanPerformance);
    }
    
    stdDev = std::sqrt(stdDev / static_cast<float>(performances.size()));
    results.consistency = 1.0f - std::min(stdDev / 1.0f, 1.0f); // Normalize to 0-1
    
    // Calculate accuracy (simplified)
    results.accuracy = learningRate > 0.0f ? std::min(learningRate / 0.05f, 1.0f) : 0.0f;
    
    results.sampleSize = learningPoints_.size();
    
    return results;
}

std::vector<std::pair<double, float>> LearningProgressTest::getLearningPoints() const {
    return learningPoints_;
}

float LearningProgressTest::calculateLearningRate() const {
    if (learningPoints_.size() < 2) return 0.0f;
    
    // Use the first and last points to calculate the learning rate
    double timeDiff = learningPoints_.back().first - learningPoints_.front().first;
    if (timeDiff <= 0.0) return 0.0f;
    
    float performanceDiff = learningPoints_.back().second - learningPoints_.front().second;
    return performanceDiff / static_cast<float>(timeDiff);
}

float LearningProgressTest::calculateLearningAcceleration() const {
    if (learningPoints_.size() < 3) return 0.0f;
    
    // Calculate the derivative of the learning rate
    float rate1 = calculateLearningRate();
    
    // Create a temporary set of points with the last two points
    std::vector<std::pair<double, float>> tempPoints(learningPoints_.begin() + 1, learningPoints_.end());
    
    float rate2 = tempPoints.size() >= 2 ? (tempPoints.back().second - tempPoints.front().second) / 
                                         (tempPoints.back().first - tempPoints.front().first) : 0.0f;
    
    return (rate2 - rate1) / (learningPoints_.back().first - learningPoints_.front().first);
}

float LearningProgressTest::calculateTrendLine(const std::vector<std::pair<double, float>>& points) const {
    if (points.size() < 2) return 0.0f;
    
    double xSum = 0.0, ySum = 0.0;
    double xxSum = 0.0, xySum = 0.0;
    
    for (const auto& point : points) {
        xSum += point.first;
        ySum += point.second;
        xxSum += point.first * point.first;
        xySum += point.first * point.second;
    }
    
    double n = static_cast<double>(points.size());
    double denominator = n * xxSum - xSum * xSum;
    
    if (denominator == 0.0) return 0.0f;
    
    double slope = (n * xySum - xSum * ySum) / denominator;
    
    return static_cast<float>(slope);
}

float LearningProgressTest::calculateR2(const std::vector<std::pair<double, float>>& points, float slope, float intercept) const {
    if (points.size() < 2) return 0.0f;
    
    double yMean = 0.0, ssTotal = 0.0, ssResidual = 0.0;
    
    for (const auto& point : points) {
        yMean += point.second;
    }
    
    yMean /= points.size();
    
    for (const auto& point : points) {
        double predicted = slope * point.first + intercept;
        ssTotal += (point.second - yMean) * (point.second - yMean);
        ssResidual += (point.second - predicted) * (point.second - predicted);
    }
    
    if (ssTotal == 0.0) return 1.0f;
    
    return static_cast<float>(1.0 - ssResidual / ssTotal);
}

} // namespace nlm
