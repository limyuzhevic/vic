#pragma once

/**
 * BehavioralTests - Tests for selective pattern learning and behavior
 * 
 * Provides comprehensive behavioral testing for neural systems,
 * evaluating how well the brain learns to distinguish between
 * different stimuli, develop selective responses, and improve
 * over time through experience.
 * 
 * Features:
 * - Pattern discrimination tests
 * - Response selectivity tests
 * - Learning progress tracking
 * - Behavior consistency measurements
 * - Statistical significance testing
 */

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

/**
 * Behavioral test results
 */
struct BehavioralTestResults {
    // Test identification
    std::string testName;
    std::string testDescription;
    double timestamp;
    
    // Performance metrics
    float accuracy;                   // Overall accuracy (0.0-1.0)
    float selectivity;                // Response selectivity (0.0-1.0)
    float learningProgress;           // Learning progress over time (0.0-1.0)
    float consistency;                // Behavioral consistency (0.0-1.0)
    
    // Pattern discrimination metrics
    float patternSeparation;          // Separation between patterns (0.0-1.0)
    float patternOverlap;             // Overlap between patterns (0.0-1.0)
    float discriminationScore;        // Discrimination score (0.0-1.0)
    
    // Response metrics
    float responseBias;               // Response bias (0.0-1.0)
    float reactionTime;               // Average reaction time (seconds)
    float responseLatency;            // Response latency (seconds)
    
    // Statistical metrics
    float confidenceInterval;         // Confidence interval (0.0-1.0)
    float statisticalSignificance;    // Statistical significance (0.0-1.0)
    size_t sampleSize;                // Number of samples
    size_t successCount;              // Number of successful responses
    size_t failureCount;              // Number of failed responses
    
    // Detailed metrics
    std::map<std::string, float> detailedMetrics;  // Additional metrics
    
    BehavioralTestResults()
        : accuracy(0.0f)
        , selectivity(0.0f)
        , learningProgress(0.0f)
        , consistency(0.0f)
        , patternSeparation(0.0f)
        , patternOverlap(0.0f)
        , discriminationScore(0.0f)
        , responseBias(0.0f)
        , reactionTime(0.0f)
        , responseLatency(0.0f)
        , confidenceInterval(0.0f)
        , statisticalSignificance(0.0f)
        , sampleSize(0)
        , successCount(0)
        , failureCount(0)
        , timestamp(0.0) {}
    
    /**
     * Calculate the success rate
     */
    float getSuccessRate() const {
        return sampleSize > 0 ? static_cast<float>(successCount) / static_cast<float>(sampleSize) : 0.0f;
    }
    
    /**
     * Calculate the failure rate
     */
    float getFailureRate() const {
        return sampleSize > 0 ? static_cast<float>(failureCount) / static_cast<float>(sampleSize) : 0.0f;
    }
    
    /**
     * Calculate the total error rate
     */
    float getErrorRate() const {
        return 1.0f - getSuccessRate();
    }
    
    /**
     * Generate a human-readable summary
     */
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Test: " << testName << std::endl;
        ss << "Description: " << testDescription << std::endl;
        ss << "Accuracy: " << (accuracy * 100.0f) << "%" << std::endl;
        ss << "Selectivity: " << (selectivity * 100.0f) << "%" << std::endl;
        ss << "Learning Progress: " << (learningProgress * 100.0f) << "%" << std::endl;
        ss << "Consistency: " << (consistency * 100.0f) << "%" << std::endl;
        ss << "Pattern Separation: " << (patternSeparation * 100.0f) << "%" << std::endl;
        ss << "Discrimination Score: " << (discriminationScore * 100.0f) << "%" << std::endl;
        ss << "Success Rate: " << (getSuccessRate() * 100.0f) << "%" << std::endl;
        ss << "Sample Size: " << sampleSize << std::endl;
        ss << "Statistical Significance: " << (statisticalSignificance * 100.0f) << "%" << std::endl;
        return ss.str();
    }
};

/**
 * Behavioral test suite for neural systems
 */
class BehavioralTestSuite {
public:
    BehavioralTestSuite();
    ~BehavioralTestSuite();
    
    // Disable copying
    BehavioralTestSuite(const BehavioralTestSuite&) = delete;
    BehavioralTestSuite& operator=(const BehavioralTestSuite&) = delete;
    
    /**
     * Add a behavioral test to the suite
     */
    void addTest(const std::string& name, const std::string& description,
                 std::function<bool(BehavioralTestResults&)> testFunction);
    
    /**
     * Run a specific test
     */
    bool runTest(const std::string& name, BehavioralTestResults& results);
    
    /**
     * Run all tests in the suite
     */
    std::vector<BehavioralTestResults> runAllTests();
    
    /**
     * Get the list of test names
     */
    std::vector<std::string> getTestNames() const;
    
    /**
     * Get the test results for a specific test
     */
    const BehavioralTestResults* getTestResult(const std::string& name) const;
    
    /**
     * Clear all test results
     */
    void clearResults();
    
    /**
     * Get the overall test suite summary
     */
    std::string getSuiteSummary() const;
    
    /**
     * Calculate the average accuracy across all tests
     */
    float getAverageAccuracy() const;
    
    /**
     * Calculate the average selectivity across all tests
     */
    float getAverageSelectivity() const;
    
    /**
     * Calculate the average learning progress across all tests
     */
    float getAverageLearningProgress() const;
    
private:
    std::map<std::string, std::pair<std::string, std::function<bool(BehavioralTestResults&)>>> tests_;
    std::map<std::string, BehavioralTestResults> results_;
    
    // Helper functions
    float calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) const;
    float calculatePatternSeparation(const std::vector<std::vector<float>>& patterns) const;
    float calculateDiscriminationScore(const std::vector<float>& responses, const std::vector<float>& expected) const;
};

/**
 * Pattern discrimination test
 * Tests how well the neural system distinguishes between different patterns
 */
class PatternDiscriminationTest {
public:
    PatternDiscriminationTest();
    ~PatternDiscriminationTest();
    
    /**
     * Add a stimulus pattern to the test
     */
    void addPattern(const std::string& name, const std::vector<float>& pattern);
    
    /**
     * Add a response to a stimulus
     */
    void addResponse(const std::string& stimulus, const std::string& response, float confidence);
    
    /**
     * Run the pattern discrimination test
     */
    BehavioralTestResults runTest() const;
    
    /**
     * Get the patterns used in the test
     */
    std::vector<std::string> getPatternNames() const;
    
    /**
     * Get the responses for a specific stimulus
     */
    std::vector<std::pair<std::string, float>> getResponses(const std::string& stimulus) const;
    
private:
    std::map<std::string, std::vector<float>> patterns_;
    std::map<std::string, std::vector<std::pair<std::string, float>>> responses_;
    
    // Helper functions
    float calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const;
    float calculateResponseSelectivity(const std::vector<std::pair<std::string, float>>& responses) const;
};

/**
 * Response selectivity test
 * Tests how selective the neural system's responses are
 */
class ResponseSelectivityTest {
public:
    ResponseSelectivityTest();
    ~ResponseSelectivityTest();
    
    /**
     * Add a stimulus-response pair
     */
    void addStimulusResponse(const std::string& stimulus, const std::string& response, float strength);
    
    /**
     * Set the expected response for a stimulus
     */
    void setExpectedResponse(const std::string& stimulus, const std::string& expectedResponse);
    
    /**
     * Run the response selectivity test
     */
    BehavioralTestResults runTest() const;
    
    /**
     * Get the stimuli used in the test
     */
    std::vector<std::string> getStimulusNames() const;
    
    /**
     * Get the expected response for a stimulus
     */
    std::string getExpectedResponse(const std::string& stimulus) const;
    
private:
    std::map<std::string, std::vector<std::pair<std::string, float>>> stimulusResponses_;
    std::map<std::string, std::string> expectedResponses_;
    
    // Helper functions
    float calculateResponseConsistency(const std::vector<std::pair<std::string, float>>& responses) const;
    float calculateResponseBias(const std::vector<std::pair<std::string, float>>& responses) const;
};

/**
 * Learning progress test
 * Tests how well the neural system improves over time
 */
class LearningProgressTest {
public:
    LearningProgressTest();
    ~LearningProgressTest();
    
    /**
     * Add a learning point (timestamp, performance)
     */
    void addLearningPoint(double timestamp, float performance);
    
    /**
     * Run the learning progress test
     */
    BehavioralTestResults runTest() const;
    
    /**
     * Get the learning points
     */
    std::vector<std::pair<double, float>> getLearningPoints() const;
    
    /**
     * Calculate the learning rate
     */
    float calculateLearningRate() const;
    
    /**
     * Calculate the learning acceleration
     */
    float calculateLearningAcceleration() const;
    
private:
    std::vector<std::pair<double, float>> learningPoints_;
    
    // Helper functions
    float calculateTrendLine(const std::vector<std::pair<double, float>>& points) const;
    float calculateR2(const std::vector<std::pair<double, float>>& points, float slope, float intercept) const;
};

} // namespace nlm
