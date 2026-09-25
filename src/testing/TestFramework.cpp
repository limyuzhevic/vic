// TestFramework.cpp
// Implementation of core testing infrastructure for NLM

#include "TestFramework.hpp"
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <sstream>

namespace nlm {

    // TestFramework implementation
    TestFramework::TestFramework() : verbose_(true) {}
    
    TestFramework::~TestFramework() {
        for (auto& suite : suites_) {
            delete suite;
        }
        suites_.clear();
    }
    
    TestSuite* TestFramework::createSuite(const std::string& name) {
        TestSuite* suite = new TestSuite(name);
        suites_.push_back(suite);
        return suite;
    }
    
    void TestFramework::runAllTests() {
        if (verbose_) {
            std::cout << "=== NLM Test Framework ===\n";
            std::cout << "Running " << suites_.size() << " test suites\n\n";
        }
        
        overallPassed_ = 0;
        overallFailed_ = 0;
        
        for (auto* suite : suites_) {
            suite->runAll();
            overallPassed_ += suite->getPassedCount();
            overallFailed_ += suite->getFailedCount();
            
            if (verbose_) {
                std::cout << "\n";
            }
        }
        
        if (verbose_) {
            std::cout << "\n=== Test Summary ===\n";
            std::cout << "Total: " << (overallPassed_ + overallFailed_) << " tests\n";
            std::cout << "Passed: " << overallPassed_ << "\n";
            std::cout << "Failed: " << overallFailed_ << "\n";
            
            if (overallFailed_ == 0) {
                std::cout << "\nAll tests passed! ✓\n";
            } else {
                std::cout << "\nSome tests failed! ✗\n";
            }
        }
    }
    
    void TestFramework::setVerbose(bool verbose) {
        verbose_ = verbose;
    }
    
    bool TestFramework::saveResults(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== NLM Test Results ===\n";
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        file << "Overall Results:\n";
        file << "  Passed: " << overallPassed_ << "\n";
        file << "  Failed: " << overallFailed_ << "\n";
        file << "  Total: " << (overallPassed_ + overallFailed_) << "\n\n";
        
        for (auto* suite : suites_) {
            file << "Suite: " << suite->getName() << "\n";
            file << "  Passed: " << suite->getPassedCount() << "\n";
            file << "  Failed: " << suite->getFailedCount() << "\n";
            file << "  Total: " << (suite->getPassedCount() + suite->getFailedCount()) << "\n\n";
        }
        
        file.close();
        return true;
    }
    
    std::string TestFramework::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // TestSuite implementation
    TestSuite::TestSuite(const std::string& name) : name_(name), passed_(0), failed_(0) {}
    
    TestSuite::~TestSuite() {
        for (auto& test : tests_) {
            delete test;
        }
        tests_.clear();
    }
    
    TestCase* TestSuite::addTest(const std::string& name, std::function<bool()> testFunc) {
        TestCase* test = new TestCaseImpl(name, testFunc);
        tests_.push_back(test);
        return test;
    }
    
    void TestSuite::runAll() {
        NLM_LOG_INFO("Running test suite: " + name_);
        NLM_LOG_INFO("Total tests: " + std::to_string(tests_.size()));
        
        for (auto* test : tests_) {
            try {
                if (test->run()) {
                    passed_++;
                    NLM_LOG_INFO("✓ " + test->getName());
                } else {
                    failed_++;
                    NLM_LOG_ERROR("✗ " + test->getName() + ": test failed");
                }
            } catch (const std::exception& e) {
                failed_++;
                NLM_LOG_ERROR("✗ " + test->getName() + ": " + e.what());
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Test results: " + std::to_string(passed_) + " passed, " + 
                   std::to_string(failed_) + " failed");
    }
    
    // TestCaseImpl implementation
    TestSuite::TestCaseImpl::TestCaseImpl(const std::string& name, std::function<bool()> testFunc) 
        : TestCase(name), testFunc_(testFunc) {}
    
    bool TestSuite::TestCaseImpl::run() {
        return testFunc_();
    }
    
    // Benchmark implementation
    Benchmark::Benchmark() {}
    
    Benchmark::~Benchmark() {}
    
    Benchmark::Result Benchmark::run(const std::string& name, std::function<void()> func, size_t iterations) {
        Result result;
        result.name = name;
        result.iterations = iterations;
        
        // Warm up
        for (size_t i = 0; i < 100; ++i) {
            func();
        }
        
        // Actual benchmarking
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double timeMs = static_cast<double>(duration.count()) / 1000.0;
        
        result.timeMs = timeMs;
        result.averageTimeMs = timeMs / iterations;
        
        return result;
    }
    
    void Benchmark::saveResults(const std::vector<Result>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "=== NLM Benchmark Results ===\n";
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        file << std::left << std::setw(30) << "Test Name"
             << std::setw(15) << "Iterations"
             << std::setw(15) << "Total Time (ms)"
             << std::setw(20) << "Average Time (ms)"
             << std::setw(15) << "Performance (ops/ms)" << "\n";
        
        file << std::string(100, '-') << "\n";
        
        for (const auto& result : results) {
            double opsPerMs = static_cast<double>(result.iterations) / (result.timeMs / 1000.0);
            file << std::left << std::setw(30) << result.name
                 << std::setw(15) << result.iterations
                 << std::setw(15) << std::fixed << std::setprecision(6) << result.timeMs
                 << std::setw(20) << std::fixed << std::setprecision(6) << result.averageTimeMs
                 << std::setw(15) << std::fixed << std::setprecision(2) << opsPerMs << "\n";
        }
        
        file.close();
    }
    
    std::string Benchmark::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Profiler implementation
    Profiler::Profiler() : enabled_(true), totalTime_(0.0) {}
    
    Profiler::~Profiler() {
        for (auto& session : sessions_) {
            delete session;
        }
        sessions_.clear();
    }
    
    ProfilerSession* Profiler::startSession(const std::string& name) {
        if (!enabled_) {
            return nullptr;
        }
        
        ProfilerSession* session = new ProfilerSession(name);
        sessions_.push_back(session);
        return session;
    }
    
    void Profiler::endSession(ProfilerSession* session) {
        if (!session || !enabled_) {
            return;
        }
        
        session->end();
        totalTime_ += session->getDuration();
    }
    
    void Profiler::generateReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "=== NLM Profiler Report ===\n";
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        file << std::left << std::setw(40) << "Session Name"
             << std::setw(20) << "Duration (ms)"
             << std::setw(15) << "Calls"
             << std::setw(15) << "Average (ms)"
             << std::setw(15) << "Min (ms)"
             << std::setw(15) << "Max (ms)" << "\n";
        
        file << std::string(120, '-') << "\n";
        
        for (const auto* session : sessions_) {
            double avgTime = session->getCallCount() > 0 ? 
                session->getDuration() / session->getCallCount() : 0.0;
            
            file << std::left << std::setw(40) << session->getName()
                 << std::setw(20) << std::fixed << std::setprecision(6) << session->getDuration()
                 << std::setw(15) << session->getCallCount()
                 << std::setw(15) << std::fixed << std::setprecision(6) << avgTime
                 << std::setw(15) << std::fixed << std::setprecision(6) << session->getMinTime()
                 << std::setw(15) << std::fixed << std::setprecision(6) << session->getMaxTime() << "\n";
        }
        
        file << "\nTotal profiled time: " << std::fixed << std::setprecision(6) << totalTime_ << " ms\n";
        file.close();
    }
    
    std::string Profiler::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // CoverageAnalyzer implementation
    CoverageAnalyzer::CoverageAnalyzer() : totalLines_(0), coveredLines_(0) {}
    
    CoverageAnalyzer::~CoverageAnalyzer() {
        for (auto& file : files_) {
            delete file;
        }
        files_.clear();
    }
    
    void CoverageAnalyzer::addSourceFile(const std::string& filename, 
                                        const std::vector<bool>& coverage) {
        SourceFile* file = new SourceFile();
        file->filename = filename;
        file->coverage = coverage;
        file->coveredLines = std::count(coverage.begin(), coverage.end(), true);
        file->totalLines = coverage.size();
        
        files_.push_back(file);
        totalLines_ += file->totalLines;
        coveredLines_ += file->coveredLines;
    }
    
    double CoverageAnalyzer::getOverallCoverage() const {
        return totalLines_ > 0 ? 
            static_cast<double>(coveredLines_) / totalLines_ * 100.0 : 0.0;
    }
    
    void CoverageAnalyzer::generateReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "=== NLM Code Coverage Report ===\n";
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        file << "Summary:\n";
        file << "  Total lines: " << totalLines_ << "\n";
        file << "  Covered lines: " << coveredLines_ << "\n";
        file << "  Coverage: " << std::fixed << std::setprecision(2) 
             << getOverallCoverage() << "%\n\n";
        
        file << std::left << std::setw(60) << "File"
             << std::setw(15) << "Covered"
             << std::setw(15) << "Total"
             << std::setw(15) << "Coverage %" << "\n";
        
        file << std::string(105, '-') << "\n";
        
        for (const auto* file : files_) {
            double coveragePercent = file->totalLines > 0 ? 
                static_cast<double>(file->coveredLines) / file->totalLines * 100.0 : 0.0;
            
            file << std::left << std::setw(60) << file->filename
                 << std::setw(15) << file->coveredLines
                 << std::setw(15) << file->totalLines
                 << std::setw(15) << std::fixed << std::setprecision(2) << coveragePercent << "\n";
        }
        
        file.close();
    }
    
    std::string CoverageAnalyzer::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Assert implementation
    bool Assert::check(bool condition, const std::string& message, const std::string& file, int line) {
        if (!condition) {
            NLM_LOG_ERROR("Assertion failed: " + message + " at " + file + ":" + std::to_string(line));
            return false;
        }
        return true;
    }
    
    bool Assert::isTrue(bool condition, const std::string& message, const std::string& file, int line) {
        return check(condition, message, file, line);
    }
    
    bool Assert::isFalse(bool condition, const std::string& message, const std::string& file, int line) {
        return check(!condition, message, file, line);
    }
    
    bool Assert::equals(int a, int b, const std::string& message, const std::string& file, int line) {
        return check(a == b, message, file, line);
    }
    
    bool Assert::notEquals(int a, int b, const std::string& message, const std::string& file, int line) {
        return check(a != b, message, file, line);
    }
    
    bool Assert::equals(double a, double b, double epsilon, const std::string& message, 
                       const std::string& file, int line) {
        return check(std::abs(a - b) <= epsilon, message, file, line);
    }
    
    bool Assert::notEquals(double a, double b, double epsilon, const std::string& message,
                          const std::string& file, int line) {
        return check(std::abs(a - b) > epsilon, message, file, line);
    }
    
    bool Assert::equals(const std::string& a, const std::string& b, const std::string& message,
                       const std::string& file, int line) {
        return check(a == b, message, file, line);
    }
    
    bool Assert::notEquals(const std::string& a, const std::string& b, const std::string& message,
                          const std::string& file, int line) {
        return check(a != b, message, file, line);
    }
    
    bool Assert::isNull(void* ptr, const std::string& message, const std::string& file, int line) {
        return check(ptr == nullptr, message, file, line);
    }
    
    bool Assert::isNotNull(void* ptr, const std::string& message, const std::string& file, int line) {
        return check(ptr != nullptr, message, file, line);
    }
    
    bool Assert::isGreaterThan(int a, int b, const std::string& message, const std::string& file, int line) {
        return check(a > b, message, file, line);
    }
    
    bool Assert::isGreaterThanOrEqual(int a, int b, const std::string& message, 
                                      const std::string& file, int line) {
        return check(a >= b, message, file, line);
    }
    
    bool Assert::isLessThan(int a, int b, const std::string& message, const std::string& file, int line) {
        return check(a < b, message, file, line);
    }
    
    bool Assert::isLessThanOrEqual(int a, int b, const std::string& message, 
                                   const std::string& file, int line) {
        return check(a <= b, message, file, line);
    }

}} // namespace nlm