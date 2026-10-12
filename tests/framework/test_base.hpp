#ifndef TEST_BASE_HPP
#define TEST_BASE_HPP

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

// NLM testing framework
namespace test {

class TestResult {
public:
    enum class Status { PASSED, FAILED, SKIPPED };
    
    TestResult(const std::string& testName) 
        : name(testName), status(Status::SKIPPED), elapsedMs(0.0) {}
    
    std::string getName() const { return name; }
    Status getStatus() const { return status; }
    double getElapsedMs() const { return elapsedMs; }
    std::string getMessage() const { return message; }
    
    void setStatus(Status s) { status = s; }
    void setElapsedMs(double ms) { elapsedMs = ms; }
    void setMessage(const std::string& msg) { message = msg; }
    
private:
    std::string name;
    Status status;
    double elapsedMs;
    std::string message;
};

class TestSuite {
public:
    virtual ~TestSuite() {}
    virtual void runTests() = 0;
    virtual std::string getName() const = 0;
};

class ParameterizedTestBase {
protected:
    template<typename T>
    static std::string toString(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    static std::string generateTestName(const std::string& base, int param) {
        return base + "_param_" + std::to_string(param);
    }
};

// Macro for test registration
#define TEST_CASE(SuiteName, TestName) \
    class Test_##SuiteName##_##TestName : public ::test::TestSuite { \
    public: \
        Test_##SuiteName##_##TestName() : TestSuite() { \
            /* Register with test framework */ \
        } \
        void runTests() override { \
            TestName(); \
        } \
        static void TestName()

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Assertion failed: " << #expected \
                      << " == " << #actual << std::endl; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while(0)

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Assertion failed: " << #cond << std::endl; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while(0)

#define ASSERT_FALSE(cond) \
    do { \
        if ((cond)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Assertion failed: !" << #cond << std::endl; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while(0)

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Expectation failed: " << #expected \
                      << " != " << #actual << std::endl; \
            throw std::runtime_error("Expectation failed"); \
        } \
    } while(0)

#define SUCCEED() std::cout << "Test passed: " << __FILE__ << ":" << __LINE__ << std::endl

// Performance testing utilities
class Stopwatch {
public:
    Stopwatch() { startTime = std::chrono::high_resolution_clock::now(); }
    
    double elapsedMs() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return static_cast<double>(duration.count());
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

// Memory leak detection
class MemoryTracker {
public:
    static void* trackAllocation(void* ptr, const std::string& label) {
        allocations[ptr] = label;
        return ptr;
    }
    
    static void trackDeallocation(void* ptr) {
        allocations.erase(ptr);
    }
    
    static size_t getLeakCount() {
        return allocations.size();
    }
    
    static void reportLeaks() {
        if (!allocations.empty()) {
            std::cerr << "MEMORY LEAK DETECTED: " << allocations.size() << " allocations not freed:" << std::endl;
            for (const auto& pair : allocations) {
                std::cerr << "  " << pair.first << " (" << pair.second << ")" << std::endl;
            }
        }
    }
    
private:
    static std::unordered_map<void*, std::string> allocations;
};

std::unordered_map<void*, std::string> test::MemoryTracker::allocations;

// Test data generators
class TestDataGenerator {
public:
    static std::vector<float> generateRandomVector(size_t size, float min = -1.0f, float max = 1.0f) {
        std::vector<float> data(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        for (size_t i = 0; i < size; ++i) {
            data[i] = dist(gen);
        }
        return data;
    }
    
    static std::vector<int> generateRandomIntVector(size_t size, int min = 0, int max = 100) {
        std::vector<int> data(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        for (size_t i = 0; i < size; ++i) {
            data[i] = dist(gen);
        }
        return data;
    }
};

} // namespace test

#endif // TEST_BASE_HPP
