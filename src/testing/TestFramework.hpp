// TestFramework.hpp
// Core testing infrastructure for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace nlm {

class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    virtual bool setup() { return true; }
    virtual void teardown() {}
};

class TestSuite {
public:
    TestSuite(const std::string& name);
    ~TestSuite();
    
    void addTest(const std::string& name, std::function<bool()> testFunc);
    void runAll();
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    const std::string& getName() const { return name_; }
    
private:
    class TestCaseImpl : public TestCase {
    public:
        TestCaseImpl(const std::string& name, std::function<bool()> testFunc);
        std::string getName() const override { return name_; }
        void run() override;
        bool setup() override { return true; }
        void teardown() override {}
    
    private:
        std::string name_;
        std::function<bool()> testFunc_;
    };
    
    std::string name_;
    std::vector<TestCaseImpl*> tests_;
    size_t passed_;
    size_t failed_;
};

class Benchmark {
public:
    struct Result {
        std::string name;
        double timeMs;
        size_t iterations;
        double averageTimeMs;
    };
    
    Benchmark();
    ~Benchmark();
    
    static Result benchmark(std::function<void()> func, size_t iterations);
    static void saveResults(const std::vector<Result>& results, const std::string& filename);
    
private:
    static std::string getCurrentTimestamp();
};

class ProfilerSession {
public:
    ProfilerSession(const std::string& name);
    ~ProfilerSession();
    
    void start();
    void end();
    const std::string& getName() const { return name_; }
    double getDuration() const { return durationMs_; }
    size_t getCallCount() const { return callCount_; }
    double getMinTime() const { return minTimeMs_; }
    double getMaxTime() const { return maxTimeMs_; }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double durationMs_;
    size_t callCount_;
    double minTimeMs_;
    double maxTimeMs_;
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    ProfilerSession* startSession(const std::string& name);
    void endSession(ProfilerSession* session);
    void generateReport(const std::string& filename);
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
private:
    bool enabled_;
    std::vector<ProfilerSession*> sessions_;
    double totalTime_;
    
    static std::string getCurrentTimestamp();
};

class CoverageAnalyzer {
public:
    struct SourceFile {
        std::string filename;
        std::vector<bool> coverage;
        size_t coveredLines;
        size_t totalLines;
    };
    
    CoverageAnalyzer();
    ~CoverageAnalyzer();
    
    void addSourceFile(const std::string& filename, const std::vector<bool>& coverage);
    double getOverallCoverage() const;
    void generateReport(const std::string& filename);
    
private:
    std::vector<SourceFile*> files_;
    size_t totalLines_;
    size_t coveredLines_;
    
    static std::string getCurrentTimestamp();
};

class Assert {
public:
    static bool check(bool condition, const std::string& message, const std::string& file, int line);
    static bool isTrue(bool condition, const std::string& message, const std::string& file, int line);
    static bool isFalse(bool condition, const std::string& message, const std::string& file, int line);
    static bool equals(int a, int b, const std::string& message, const std::string& file, int line);
    static bool notEquals(int a, int b, const std::string& message, const std::string& file, int line);
    static bool equals(double a, double b, double epsilon, const std::string& message, const std::string& file, int line);
    static bool notEquals(double a, double b, double epsilon, const std::string& message, const std::string& file, int line);
    static bool equals(const std::string& a, const std::string& b, const std::string& message, const std::string& file, int line);
    static bool notEquals(const std::string& a, const std::string& b, const std::string& message, const std::string& file, int line);
    static bool isNull(void* ptr, const std::string& message, const std::string& file, int line);
    static bool isNotNull(void* ptr, const std::string& message, const std::string& file, int line);
    static bool isGreaterThan(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isGreaterThanOrEqual(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isLessThan(int a, int b, const std::string& message, const std::string& file, int line);
    static bool isLessThanOrEqual(int a, int b, const std::string& message, const std::string& file, int line);
};

class TestFramework {
public:
    TestFramework();
    ~TestFramework();
    
    TestSuite* createSuite(const std::string& name);
    void runAllTests();
    void setVerbose(bool verbose);
    bool saveResults(const std::string& filename);
    
private:
    bool verbose_;
    std::vector<TestSuite*> suites_;
    size_t overallPassed_;
    size_t overallFailed_;
    
    static std::string getCurrentTimestamp();
};

// Convenience macros for test cases
#define TEST_CASE(name) \
class name##_Test : public TestCase { \
public: \
    std::string getName() const override { return #name; } \

#define TEST_ASSERT(condition) \
if (!(condition)) { \
    throw std::runtime_error("Assertion failed: " #condition); \
}

#define TEST_CHECKPOINT \
if (brain_ == nullptr) { \
    throw std::runtime_error("Brain not initialized"); \
}

// Brain test utilities
class BrainTestHelper {
public:
    static std::shared_ptr<Brain> setupBrain(size_t neuronCount = 100);
    static std::shared_ptr<Brain> setupBrain(
        size_t neuronCount,
        size_t regionCount,
        float connectionProbability,
        float simulationTimestep);
    static void runStandardTestSequence(const std::shared_ptr<Brain>& brain);
};

}} // namespace nlm