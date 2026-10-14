// Main test runner for NLM brain simulation
// This file provides the entry point for running all tests including memory tests

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

// Include individual test modules
namespace test_memory {
    void runAllMemoryTests();
}

namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_brain_save_load {
    void runAll();
}

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    double duration;
    std::string message;
};

// Test class
class Test {
public:
    virtual ~Test() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
};

// Test suite implementation
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(Test* test) {
        tests_.push_back(test);
    }
    
    std::string getName() const {
        return name_;
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        std::cout << "Running tests in " << name_ << "..." << std::endl;
        
        for (auto* test : tests_) {
            std::cout << "  Running " << test->getName() << "...";
            std::cout.flush();
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test->run();
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(end - start).count();
                
                TestResult result;
                result.testName = test->getName();
                result.passed = true;
                result.duration = duration;
                result.message = "PASSED";
                
                std::cout << " PASSED (" << std::fixed << std::setprecision(3) 
                          << duration << "s)" << std::endl;
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(end - start).count();
                
                TestResult result;
                result.testName = test->getName();
                result.passed = false;
                result.duration = duration;
                result.message = std::string("FAILED: ") + e.what();
                
                std::cout << " FAILED (" << std::fixed << std::setprecision(3) 
                          << duration << "s)" << std::endl;
                std::cout << "    Error: " << e.what() << std::endl;
                
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    void clear() {
        tests_.clear();
    }
    
    size_t getTestCount() const {
        return tests_.size();
    }
    
private:
    std::string name_;
    std::vector<Test*> tests_;
};

// Test suite manager
class TestManager {
public:
    void addSuite(TestSuite* suite) {
        suites_.push_back(suite);
    }
    
    void runAllTests() {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM Brain Simulation Test Suite" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        int totalTests = 0;
        int passedTests = 0;
        double totalDuration = 0.0;
        
        for (auto* suite : suites_) {
            std::cout << "\n" << suite->getName() << ":" << std::endl;
            auto results = suite->runAllTests();
            
            totalTests += results.size();
            
            for (const auto& result : results) {
                passedTests += result.passed ? 1 : 0;
                totalDuration += result.duration;
            }
            
            suite->clear();
        }
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "  Total Tests:  " << totalTests << std::endl;
        std::cout << "  Passed:       " << passedTests << std::endl;
        std::cout << "  Failed:       " << (totalTests - passedTests) << std::endl;
        std::cout << "  Success Rate: " << std::fixed << std::setprecision(1)
                  << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "  Total Time:   " << std::fixed << std::setprecision(3)
                  << totalDuration << "s" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        if (passedTests == totalTests) {
            std::cout << "All tests passed successfully!" << std::endl;
        } else {
            std::cout << "Some tests failed. Please review the implementation." << std::endl;
        }
    }
    
    void clear() {
        suites_.clear();
    }
    
private:
    std::vector<TestSuite*> suites_;
};

int main() {
    TestManager testManager;
    
    // Create test suites
    TestSuite* memorySuite = new TestSuite("Memory Tests");
    memorySuite->addTest(new class : public Test {
        std::string getName() const override { return "Memory Retention and Learning Progression Tests"; }
        void run() override { test_memory::runAllMemoryTests(); }
    }());
    
    TestSuite* basicSuite = new TestSuite("Basic Tests");
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Types Tests"; }
        void run() override { test_types::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Config Tests"; }
        void run() override { test_config::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Random Tests"; }
        void run() override { test_random::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Clock Tests"; }
        void run() override { test_clock::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Neuron Tests"; }
        void run() override { test_neuron::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Synapse Tests"; }
        void run() override { test_synapse::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "STDP Tests"; }
        void run() override { test_stdp::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Brain Tests"; }
        void run() override { test_brain::runAll(); }
    }());
    basicSuite->addTest(new class : public Test {
        std::string getName() const override { return "Brain Save/Load Tests"; }
        void run() override { test_brain_save_load::runAll(); }
    }());
    
    // Add suites to manager
    testManager.addSuite(memorySuite);
    testManager.addSuite(basicSuite);
    
    // Run all tests
    testManager.runAllTests();
    
    // Cleanup
    testManager.clear();
    
    return 0;
}
