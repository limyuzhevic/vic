// TestTypes.hpp - Test result types and utilities
#ifndef TESTTYPES_HPP
#define TESTTYPES_HPP

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <variant>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm::test {

// Test result enumeration
enum class TestStatus {
    PASSED = 0,
    FAILED = 1,
    SKIPPED = 2,
    ERROR = 3,
    TIMEOUT = 4
};

// Test metadata
struct TestMetadata {
    std::string name;
    std::string category;
    std::string description;
    std::string tags;
    int timeoutMs;
    int priority;
    std::time_t creationTime;
    
    TestMetadata(const std::string& testName, const std::string& testCategory,
                 const std::string& testDesc = "")
        : name(testName), category(testCategory), description(testDesc),
          tags(""), timeoutMs(5000), priority(0) {
        creationTime = std::time(nullptr);
    }
};

// Test measurement data
struct TestMetrics {
    std::chrono::duration<double> executionTime;
    std::chrono::duration<double> setupTime;
    std::chrono::duration<double> teardownTime;
    size_t memoryUsage;
    double cpuTime;
    
    TestMetrics()
        : executionTime(0), setupTime(0), teardownTime(0),
          memoryUsage(0), cpuTime(0.0) {}
    
    double getOperationsPerSecond(size_t operations = 1) const {
        if (executionTime.count() == 0) return 0.0;
        return operations / executionTime.count();
    }
};

// Test result with comprehensive data
struct TestResult {
    std::string testName;
    std::string suiteName;
    std::string category;
    TestStatus status;
    TestMetrics metrics;
    std::string errorMessage;
    std::string stackTrace;
    std::map<std::string, std::string> customData;
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& name, const std::string& suite, 
               const std::string& cat = "General")
        : testName(name), suiteName(suite), category(cat), status(TestStatus::ERROR),
          lineNumber(0) {}
    
    // Serialization support
    std::string toJson() const {
        std::stringstream ss;
        ss << "{";
        ss << "\"testName\": \"" << escapeString(testName) << "\",";
        ss << "\"suiteName\": \"" << escapeString(suiteName) << "\",";
        ss << "\"category\": \"" << escapeString(category) << "\",";
        ss << "\"status\": " << static_cast<int>(status) << ",";
        ss << "\"executionTime\": " << metrics.executionTime.count() << ",";
        ss << "\"setupTime\": " << metrics.setupTime.count() << ",";
        ss << "\"teardownTime\": " << metrics.teardownTime.count() << ";";
        ss << "\"memoryUsage\": " << metrics.memoryUsage << ",";
        ss << "\"cpuTime\": " << metrics.cpuTime << ",";
        ss << "\"errorMessage\": \"" << escapeString(errorMessage) << "\",";
        ss << "\"lineNumber\": " << lineNumber << ";";
        ss << "\"fileName\": \"" << escapeString(fileName) << "\"";
        ss << "}";
        return ss.str();
    }
    
private:
    std::string escapeString(const std::string& str) const {
        std::stringstream ss;
        for (char c : str) {
            switch (c) {
                case '\\': ss << "\\\\"; break;
                case '\"': ss << "\\\""; break;
                case '\n': ss << "\\n"; break;
                case '\r': ss << "\\r"; break;
                case '\t': ss << "\\t"; break;
                default: ss << c; break;
            }
        }
        return ss.str();
    }
};

// Parameterized test data
struct ParameterSet {
    std::string name;
    std::map<std::string, std::variant<int, double, std::string, bool, std::vector<int>, std::vector<double>>> parameters;
    
    ParameterSet(const std::string& setName) : name(setName) {}
    
    template<typename T>
    void addParameter(const std::string& key, const T& value) {
        parameters[key] = value;
    }
    
    template<typename T>
    T getParameter(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return std::get<T>(it->second);
        }
        throw std::runtime_error("Parameter not found: " + key);
    }
    
    bool hasParameter(const std::string& key) const {
        return parameters.find(key) != parameters.end();
    }
};

// Test result summary for reporting
struct TestSummary {
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    size_t skippedTests;
    size_t erroredTests;
    size_t timeoutTests;
    double totalExecutionTime;
    double averageExecutionTime;
    double minExecutionTime;
    double maxExecutionTime;
    
    TestSummary()
        : totalTests(0), passedTests(0), failedTests(0), skippedTests(0),
          erroredTests(0), timeoutTests(0), totalExecutionTime(0.0),
          averageExecutionTime(0.0), minExecutionTime(0.0), maxExecutionTime(0.0) {}
    
    double getSuccessRate() const {
        if (totalTests == 0) return 0.0;
        return static_cast<double>(passedTests) / totalTests * 100.0;
    }
    
    std::string getReport() const {
        std::stringstream ss;
        ss << "=== Test Summary ===" << std::endl;
        ss << "Total Tests: " << totalTests << std::endl;
        ss << "Passed: " << passedTests << std::endl;
        ss << "Failed: " << failedTests << std::endl;
        ss << "Skipped: " << skippedTests << std::endl;
        ss << "Errored: " << erroredTests << std::endl;
        ss << "Timeout: " << timeoutTests << std::endl;
        ss << "Success Rate: " << std::fixed << std::setprecision(2) << getSuccessRate() << "%" << std::endl;
        ss << "Total Time: " << std::fixed << std::setprecision(6) << totalExecutionTime << "s" << std::endl;
        if (totalTests > 0) {
            ss << "Average Time: " << std::fixed << std::setprecision(6) << averageExecutionTime << "s" << std::endl;
            ss << "Min Time: " << std::fixed << std::setprecision(6) << minExecutionTime << "s" << std::endl;
            ss << "Max Time: " << std::fixed << std::setprecision(6) << maxExecutionTime << "s" << std::endl;
        }
        return ss.str();
    }
};

// Test discovery interface
class TestDiscovery {
public:
    virtual ~TestDiscovery() = default;
    
    virtual std::vector<TestMetadata> discoverTests() = 0;
    virtual std::vector<ParameterSet> discoverParameterSets() = 0;
    virtual bool loadTest(const std::string& testPath) = 0;
    
    // File-based discovery
    static std::vector<TestMetadata> discoverFromDirectory(const std::string& directoryPath) {
        std::vector<TestMetadata> tests;
        
        // In a real implementation, this would scan for test files
        // For now, we return an empty vector
        return tests;
    }
};

// Test output formatter
class TestOutputFormatter {
public:
    virtual ~TestOutputFormatter() = default;
    
    virtual void startTests() = 0;
    virtual void addTestResult(const TestResult& result) = 0;
    virtual void endTests() = 0;
    virtual std::string getOutput() const = 0;
    
    // Standard formatters
    class ConsoleFormatter : public TestOutputFormatter {
    private:
        std::stringstream output;
        
    public:
        void startTests() override {
            output << "=== Test Suite ===" << std::endl;
        }
        
        void addTestResult(const TestResult& result) override {
            output << "Test: " << result.testName << std::endl;
            output << "Suite: " << result.suiteName << std::endl;
            output << "Category: " << result.category << std::endl;
            output << "Status: " << testStatusToString(result.status) << std::endl;
            output << "Time: " << std::fixed << std::setprecision(6) << result.metrics.executionTime.count() << "s" << std::endl;
            
            if (!result.errorMessage.empty()) {
                output << "Error: " << result.errorMessage << std::endl;
            }
            output << std::endl;
        }
        
        void endTests() override {
            output << "=== Tests Complete ===" << std::endl;
        }
        
        std::string getOutput() const override {
            return output.str();
        }
        
    private:
        std::string testStatusToString(TestStatus status) const {
            switch (status) {
                case TestStatus::PASSED: return "PASSED";
                case TestStatus::FAILED: return "FAILED";
                case TestStatus::SKIPPED: return "SKIPPED";
                case TestStatus::ERROR: return "ERROR";
                case TestStatus::TIMEOUT: return "TIMEOUT";
                default: return "UNKNOWN";
            }
        }
    };
    
    class JsonFormatter : public TestOutputFormatter {
    private:
        std::stringstream output;
        
    public:
        void startTests() override {
            output << "[{" << std::endl;
        }
        
        void addTestResult(const TestResult& result) override {
            output << "  {" << std::endl;
            output << "    \"testName\": \"" << result.testName << "\"," << std::endl;
            output << "    \"suiteName\": \"" << result.suiteName << "\"," << std::endl;
            output << "    \"category\": \"" << result.category << "\"," << std::endl;
            output << "    \"status\": " << static_cast<int>(result.status) << "," << std::endl;
            output << "    \"executionTime\": " << result.metrics.executionTime.count() << "," << std::endl;
            output << "    \"errorMessage\": \"" << escapeString(result.errorMessage) << "\"" << std::endl;
            output << "  }";
            output << (output.str().find_last_of('\n') != output.str().find_last_of('\n') ? "," : "") << std::endl;
        }
        
        void endTests() override {
            output << "}]" << std::endl;
        }
        
        std::string getOutput() const override {
            return output.str();
        }
        
    private:
        std::string escapeString(const std::string& str) const {
            std::stringstream ss;
            for (char c : str) {
                switch (c) {
                    case '\\': ss << "\\\\"; break;
                    case '\"': ss << "\\\""; break;
                    case '\n': ss << "\\n"; break;
                    case '\r': ss << "\\r"; break;
                    case '\t': ss << "\\t"; break;
                    default: ss << c; break;
                }
            }
            return ss.str();
        }
    };
};

} // namespace nlm::test

#endif // TESTTYPES_HPP
