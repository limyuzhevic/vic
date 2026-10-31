// NLM Test Framework
// Provides configurable test scenarios and proper separation from production code

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <algorithm>

// Forward declarations
namespace nlm {
    class Brain;
    class Config;
}

namespace test {

// Test result enumeration
enum class TestResult {
    PASS,
    FAIL,
    ERROR,
    SKIP
};

// Test severity levels
enum class TestSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Test case structure
struct TestCase {
    std::string name;
    std::string description;
    std::function<bool()> testFunction;
    TestSeverity severity = TestSeverity::INFO;
    std::vector<std::string> tags;
    bool enabled = true;
    double timeoutSeconds = 30.0;  // Default timeout

    TestCase(const std::string& name, const std::string& description,
             std::function<bool()> func, TestSeverity severity = TestSeverity::INFO)
        : name(name), description(description), testFunction(func), severity(severity) {}

    TestCase(const std::string& name, const std::string& description,
             std::function<bool()> func, TestSeverity severity,
             const std::vector<std::string>& tags)
        : name(name), description(description), testFunction(func), severity(severity), tags(tags) {}
};

// Test statistics
struct TestStatistics {
    size_t totalTests = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    size_t errorTests = 0;
    size_t skippedTests = 0;
    double totalTimeSeconds = 0.0;
    std::string startTime;
    std::string endTime;

    double getPassRate() const {
        return totalTests > 0 ? (static_cast<double>(passedTests) / totalTests) * 100.0 : 0.0;
    }

    std::string getSummary() const {
        std::stringstream ss;
        ss << "Tests: " << totalTests
           << " | Passed: " << passedTests
           << " | Failed: " << failedTests
           << " | Errors: " << errorTests
           << " | Skipped: " << skippedTests
           << " | Pass Rate: " << std::fixed << std::setprecision(1) << getPassRate() << "%";
        return ss.str();
    }
};

// Test environment setup
class TestEnvironment {
protected:
    std::shared_ptr<nlm::Config> defaultConfig;
    std::string outputDirectory;
    bool verboseOutput = false;

public:
    virtual ~TestEnvironment() = default;

    virtual bool initialize() {
        defaultConfig = std::make_shared<nlm::Config>();
        defaultConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        defaultConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        defaultConfig->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        defaultConfig->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        defaultConfig->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        return true;
    }

    virtual void cleanup() {}

    std::shared_ptr<nlm::Config> getDefaultConfig() const { return defaultConfig; }

    void setOutputDirectory(const std::string& dir) { outputDirectory = dir; }
    const std::string& getOutputDirectory() const { return outputDirectory; }

    void setVerbose(bool verbose) { verboseOutput = verbose; }
    bool isVerbose() const { return verboseOutput; }
};

// Test runner base class
class TestRunner {
protected:
    std::shared_ptr<TestEnvironment> environment;
    std::vector<TestCase> testCases;
    TestStatistics statistics;
    std::vector<std::string> failedTestNames;
    std::vector<std::string> errorTestNames;

public:
    explicit TestRunner(std::shared_ptr<TestEnvironment> env = nullptr) : environment(env) {
        if (!environment) {
            environment = std::make_shared<TestEnvironment>();
            environment->initialize();
        }
    }

    virtual ~TestRunner() = default;

    void addTest(const TestCase& testCase) {
        testCases.push_back(testCase);
    }

    void addTests(const std::vector<TestCase>& tests) {
        testCases.insert(testCases.end(), tests.begin(), tests.end());
    }

    virtual void setup() {}
    virtual void teardown() {}

    TestStatistics runAllTests() {
        statistics.startTime = getCurrentTimestamp();
        setup();

        for (const auto& testCase : testCases) {
            if (!testCase.enabled) {
                statistics.skippedTests++;
                continue;
            }

            std::cout << "Running test: " << testCase.name << std::endl;
            if (environment->isVerbose()) {
                std::cout << "  Description: " << testCase.description << std::endl;
                std::cout << "  Severity: " << severityToString(testCase.severity) << std::endl;
            }

            statistics.totalTests++;
            auto startTime = std::chrono::high_resolution_clock::now();

            TestResult result = runSingleTest(testCase);

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            statistics.totalTimeSeconds += duration.count();

            switch (result) {
                case TestResult::PASS:
                    statistics.passedTests++;
                    std::cout << "  PASSED (" << std::fixed << std::setprecision(3) << duration.count() << "s)" << std::endl;
                    break;
                case TestResult::FAIL:
                    statistics.failedTests++;
                    failedTestNames.push_back(testCase.name);
                    std::cout << "  FAILED" << std::endl;
                    break;
                case TestResult::ERROR:
                    statistics.errorTests++;
                    errorTestNames.push_back(testCase.name);
                    std::cout << "  ERROR" << std::endl;
                    break;
                case TestResult::SKIP:
                    statistics.skippedTests++;
                    std::cout << "  SKIPPED" << std::endl;
                    break;
            }
        }

        teardown();
        statistics.endTime = getCurrentTimestamp();
        return statistics;
    }

protected:
    TestResult runSingleTest(const TestCase& testCase) {
        try {
            // Implement timeout handling
            auto future = std::async(std::launch::async, testCase.testFunction);
            auto status = future.wait_for(std::chrono::duration<double>(testCase.timeoutSeconds));

            if (status == std::future_status::timeout) {
                std::cout << "    TIMEOUT after " << testCase.timeoutSeconds << " seconds" << std::endl;
                return TestResult::FAIL;
            }

            bool result = future.get();
            return result ? TestResult::PASS : TestResult::FAIL;
        } catch (const std::exception& e) {
            std::cout << "    EXCEPTION: " << e.what() << std::endl;
            return TestResult::ERROR;
        } catch (...) {
            std::cout << "    UNKNOWN EXCEPTION" << std::endl;
            return TestResult::ERROR;
        }
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string severityToString(TestSeverity severity) {
        switch (severity) {
            case TestSeverity::INFO: return "INFO";
            case TestSeverity::WARNING: return "WARNING";
            case TestSeverity::ERROR: return "ERROR";
            case TestSeverity::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    std::string resultToString(TestResult result) {
        switch (result) {
            case TestResult::PASS: return "PASS";
            case TestResult::FAIL: return "FAIL";
            case TestResult::ERROR: return "ERROR";
            case TestResult::SKIP: return "SKIP";
            default: return "UNKNOWN";
        }
    }

    void logTestResult(const TestCase& testCase, TestResult result) {
        std::cout << "  " << testCase.name << ": " << resultToString(result);
        if (!testCase.tags.empty()) {
            std::cout << " [Tags: ";
            for (size_t i = 0; i < testCase.tags.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << testCase.tags[i];
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }

    // For subclasses to override
    virtual bool runTest(const TestCase& testCase) {
        return testCase.testFunction();
    }
};

// Test configuration
struct TestConfig {
    std::string configFile = "configs/default_test.cfg";
    std::string outputDirectory = "./test_results";
    bool verbose = false;
    std::string logLevel = "INFO";
    std::vector<std::string> enabledTags;
    std::vector<std::string> disabledTags;
    double timeoutMultiplier = 1.0;  // Multiplier for default timeouts
    bool saveResults = false;        // Save detailed results to files

    TestConfig() = default;
};

// Test utilities
class TestUtils {
public:
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static std::string generateRandomSeed() {
        auto now = std::chrono::system_clock::now();
        auto seed = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        return std::to_string(seed);
    }

    static bool directoryExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
    }

    static bool createDirectory(const std::string& path) {
        if (directoryExists(path)) {
            return true;
        }
        return mkdir(path.c_str(), 0755) == 0;
    }

    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    static bool stringContains(const std::string& str, const std::string& substring) {
        return str.find(substring) != std::string::npos;
    }

    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    static bool stringEndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
};

} // namespace test