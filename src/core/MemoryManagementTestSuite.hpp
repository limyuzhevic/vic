#pragma once

/**
 * @file MemoryManagementTestSuite.hpp
 * @brief Comprehensive test suite for memory management safety
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the comprehensive test suite for memory management
 * safety in the NLM computational neuroscience platform. The test suite
 * includes tests for memory leak detection, resource cleanup, exception
 * safety, reference counting, PImpl idiom validation, and stress testing.
 * 
 * Key features:
 * - Memory leak detection using leak detection tools
 * - Resource cleanup validation for all RAII objects
 * - Exception safety testing for constructors and destructors
 * - Reference counting validation for smart pointers
 * - PImpl idiom testing for proper implementation hiding
 * - Stress testing with large numbers of objects
 * - Thread safety testing for concurrent access
 * - Integration testing with system resources
 * - Performance testing for memory allocation patterns
 * - Regression testing for memory management changes
 * - Automated reporting and result aggregation
 * 
 * The test suite follows modern testing best practices:
 * - Isolation (each test is independent)
 * - Reproducibility (tests can be run multiple times)
 * - Automated reporting (detailed test results)
 * - Configurable test options
 * - Integration with build systems
 * - Parallel test execution support
 * 
 * @note This test suite should be run regularly to ensure
 *       memory management reliability and prevent regressions.
 */

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <random>
#include "core/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "core/EnhancedConfig.hpp"

namespace nlm {

/**
 * @brief Test result structure
 */
struct TestResult {
    std::string testName;
    std::string testDescription;
    bool passed;
    std::string errorMessage;
    double executionTime;
    size_t memoryUsed;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::any> metadata;
    
    TestResult(const std::string& name, const std::string& description)
        : testName(name), testDescription(description), passed(false),
          executionTime(0.0), memoryUsed(0), timestamp(std::chrono::system_clock::now()) {}
    
    void setPassed(bool passed) {
        this->passed = passed;
    }
    
    void setError(const std::string& error) {
        this->errorMessage = error;
    }
    
    void setPerformance(double time, size_t memory) {
        this->executionTime = time;
        this->memoryUsed = memory;
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << testName << ": " << (passed ? "PASSED" : "FAILED");
        if (!errorMessage.empty()) {
            oss << " (" + errorMessage + ")";
        }
        oss << " (" + std::to_string(executionTime) + "s, " + std::to_string(memoryUsed) + " bytes)";
        return oss.str();
    }
};

/**
 * @brief Memory usage snapshot
 */
struct MemorySnapshot {
    std::chrono::system_clock::time_point timestamp;
    size_t totalAllocated;
    size_t totalDeallocated;
    size_t currentUsage;
    size_t peakUsage;
    std::unordered_map<std::string, size_t> allocationsByType;
    
    MemorySnapshot() : totalAllocated(0), totalDeallocated(0),
                      currentUsage(0), peakUsage(0) {}
};

/**
 * @brief Memory management test base class
 */
class MemoryManagementTest {
public:
    MemoryManagementTest(const std::string& name, const std::string& description);
    virtual ~MemoryManagementTest() = default;
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    virtual void setup() = 0;
    virtual void cleanup() = 0;
    virtual TestResult run() = 0;
    
    virtual bool setupRequires() const { return false; }
    virtual bool cleanupRequires() const { return false; }
    
protected:
    std::string name_;
    std::string description_;
    MemorySnapshot startSnapshot_;
    MemorySnapshot endSnapshot_;
    
    void recordStartSnapshot();
    void recordEndSnapshot();
    bool checkMemoryLeaks() const;
    size_t getMemoryDelta() const;
};

/**
 * @brief Memory leak detection test
 */
class MemoryLeakTest : public MemoryManagementTest {
public:
    MemoryLeakTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    std::vector<std::unique_ptr<Neuron>> neurons_;
    std::vector<std::unique_ptr<Synapse>> synapses_;
    std::vector<std::unique_ptr<Neuromodulator>> neuromodulators_;
    std::vector<std::unique_ptr<PlasticityRule>> plasticityRules_;
    
    void createObjects(size_t count);
    void releaseObjects();
};

/**
 * @brief Resource cleanup test
 */
class ResourceCleanupTest : public MemoryManagementTest {
public:
    ResourceCleanupTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testRAIIConstruction() const;
    bool testRAIDestruction() const;
    bool testExceptionSafety() const;
    bool testResourceRelease() const;
    
    struct TestResource {
        std::unique_ptr<int> data;
        bool allocated;
        bool released;
        
        TestResource() : data(nullptr), allocated(false), released(false) {}
        ~TestResource() {
            if (allocated && !released) {
                // Simulate resource leak
                throw std::runtime_error("Resource not properly released");
            }
        }
    };
};

/**
 * @brief Exception safety test
 */
class ExceptionSafetyTest : public MemoryManagementTest {
public:
    ExceptionSafetyTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testConstructorExceptionSafety() const;
    bool testAssignmentExceptionSafety() const;
    bool testDestructorExceptionSafety() const;
    bool testMethodExceptionSafety() const;
    
    struct ExceptionTestObject {
        ExceptionTestObject() {
            // Simulate potential failure during construction
            if (shouldFailConstruction_) {
                throw std::runtime_error("Construction failed");
            }
        }
        
        ExceptionTestObject(const ExceptionTestObject&) {
            // Simulate potential failure during copy construction
            if (shouldFailCopy_) {
                throw std::runtime_error("Copy construction failed");
            }
        }
        
        ExceptionTestObject& operator=(const ExceptionTestObject&) {
            // Simulate potential failure during copy assignment
            if (shouldFailAssignment_) {
                throw std::runtime_error("Copy assignment failed");
            }
            return *this;
        }
        
        ~ExceptionTestObject() {
            // Verify object was properly constructed
            if (!constructedProperly_) {
                throw std::runtime_error("Object not properly constructed");
            }
        }
        
        void setShouldFailConstruction(bool fail) { shouldFailConstruction_ = fail; }
        void setShouldFailCopy(bool fail) { shouldFailCopy_ = fail; }
        void setShouldFailAssignment(bool fail) { shouldFailAssignment_ = fail; }
        void setConstructedProperly(bool ok) { constructedProperly_ = ok; }
        
    private:
        bool shouldFailConstruction_ = false;
        bool shouldFailCopy_ = false;
        bool shouldFailAssignment_ = false;
        bool constructedProperly_ = true;
    };
};

/**
 * @brief Reference counting test
 */
class ReferenceCountingTest : public MemoryManagementTest {
public:
    ReferenceCountingTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testSmartPointerCounting() const;
    bool testSharedPointerLifetime() const;
    bool testWeakPointerValidity() const;
    bool testCircularReferenceHandling() const;
    
    struct SharedObject {
        int value;
        
        SharedObject(int val) : value(val) {}
    };
    
    std::shared_ptr<SharedObject> createSharedObject() const;
    std::weak_ptr<SharedObject> createWeakObject(const std::shared_ptr<SharedObject>& strong);
};

/**
 * @brief PImpl idiom test
 */
class PImplTest : public MemoryManagementTest {
public:
    PImplTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testPImplConstruction() const;
    bool testPImplDestruction() const;
    bool testPImplCopy() const;
    bool testPImplMove() const;
    bool testMemoryLeakage() const;
};

/**
 * @brief Stress test
 */
class StressTest : public MemoryManagementTest {
public:
    StressTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testLargeObjectAllocation() const;
    bool testConcurrentAccess() const;
    bool testMemoryPressure() const;
    bool testResourceExhaustion() const;
    
    std::vector<std::thread> threads_;
    std::atomic<size_t> sharedCounter_;
    std::mutex counterMutex_;
};

/**
 * @brief Thread safety test
 */
class ThreadSafetyTest : public MemoryManagementTest {
public:
    ThreadSafetyTest();
    
    void setup() override;
    void cleanup() override;
    TestResult run() override;
    
private:
    bool testConcurrentReadAccess() const;
    bool testConcurrentWriteAccess() const;
    bool testAtomicOperations() const;
    bool testLockContention() const;
    
    std::vector<int> sharedData_;
    std::mutex dataMutex_;
    std::atomic<int> atomicCounter_;
};

/**
 * @brief Memory management test suite
 */
class MemoryManagementTestSuite {
public:
    MemoryManagementTestSuite();
    ~MemoryManagementTestSuite() = default;
    
    void addTest(std::unique_ptr<MemoryManagementTest> test);
    bool runAllTests();
    bool runSelectedTests(const std::vector<std::string>& testNames);
    bool runTestsWithFilter(const std::function<bool(const MemoryManagementTest&)>& filter);
    
    void setParallelExecution(bool parallel) { parallelExecution_ = parallel; }
    void setVerboseOutput(bool verbose) { verboseOutput_ = verbose; }
    void setFailFast(bool failFast) { failFast_ = failFast; }
    
    std::vector<TestResult> getResults() const { return results_; }
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    double getSuccessRate() const;
    
    std::string getReport() const;
    bool exportReport(const std::string& filepath) const;
    
    static MemoryManagementTestSuite& getInstance();
    
private:
    std::vector<std::unique_ptr<MemoryManagementTest>> tests_;
    std::vector<TestResult> results_;
    bool parallelExecution_;
    bool verboseOutput_;
    bool failFast_;
    
    void runTest(MemoryManagementTest& test);
    void logTestResult(const TestResult& result) const;
    void generateSummaryReport() const;
};

/**
 * @brief Memory management test factory
 */
class MemoryManagementTestFactory {
public:
    static std::unique_ptr<MemoryManagementTest> createTest(const std::string& testType);
    static std::vector<std::string> getAvailableTestTypes();
    
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<MemoryManagementTest>()>> testRegistry_;
};

/**
 * @brief Memory management test utilities
 */
namespace MemoryManagementUtils {

/**
 * @brief Get current memory usage
 * 
 * @return Current memory usage in bytes
 */
size_t getCurrentMemoryUsage();

/**
 * @brief Get peak memory usage
 * 
 * @return Peak memory usage in bytes
 */
size_t getPeakMemoryUsage();

/**
 * @brief Force garbage collection
 */
void forceGarbageCollection();

/**
 * @brief Check for memory leaks
 * 
 * @return true if potential memory leaks detected
 */
bool checkForMemoryLeaks();

/**
 * @brief Get allocation statistics
 * 
 * @return Allocation statistics string
 */
std::string getAllocationStatistics();

/**
 * @brief Validate object lifetime
 * 
 * @tparam T Object type
 * @param obj Object to validate
 * @return true if object lifetime is valid
 */
template<typename T>
bool validateObjectLifetime(const T& obj);

/**
 * @brief Test memory allocation pattern
 * 
 * @param size Object size
 * @param count Number of objects
 * @param pattern Allocation pattern
 * @return true if allocation pattern is valid
 */
bool testMemoryAllocationPattern(size_t size, size_t count, const std::string& pattern);

/**
 * @brief Generate memory management report
 * 
 * @param results Test results
 * @return Memory management report
 */
std::string generateMemoryManagementReport(const std::vector<TestResult>& results);

} // namespace MemoryManagementUtils

// Explicit instantiations
namespace nlm {
    template bool MemoryManagementUtils::validateObjectLifetime(const Neuron&);
    template bool MemoryManagementUtils::validateObjectLifetime(const Synapse&);
    template bool MemoryManagementUtils::validateObjectLifetime(const Neuromodulator&);
    template bool MemoryManagementUtils::validateObjectLifetime(const PlasticityRule&);
}