#!/usr/bin/env g++
// debug_demo.cpp - Demonstration of debugging and profiling tools

#include "debug_profiler.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// Example function to profile
void exampleFunction(int iterations) {
    double sum = 0.0;
    for (int i = 0; i < iterations; ++i) {
        sum += std::sin(static_cast<double>(i)) * std::cos(static_cast<double>(i));
    }
    std::cout << "Example function result: " << sum << std::endl;
}

// Example class to demonstrate memory tracking
class ExampleClass {
public:
    ExampleClass(size_t size) : data(new double[size]) {
        MEMORY_TRACK(this->data, size * sizeof(double), "ExampleClass::ExampleClass", __LINE__, __FILE__);
        std::cout << "Created ExampleClass with " << size << " elements" << std::endl;
    }
    
    ~ExampleClass() {
        MEMORY_UNTRACK(this->data);
        delete[] data;
        std::cout << "Destroyed ExampleClass" << std::endl;
    }
    
    void process() {
        double sum = 0.0;
        for (size_t i = 0; i < capacity; ++i) {
            sum += data[i];
        }
        std::cout << "Processed data with sum: " << sum << std::endl;
    }
    
private:
    double* data;
    size_t capacity;
};

int main() {
    std::cout << "=== NLM Debugging and Profiling Demo ===" << std::endl;
    
    // Example 1: Performance profiling
    std::cout << "\n1. Performance Profiling Demo:" << std::endl;
    std::cout << "   Profiling example function..." << std::endl;
    
    PROFILE_SCOPE("example_function");
    exampleFunction(10000);
    
    // Profile multiple operations
    for (int i = 0; i < 5; ++i) {
        PROFILE_SCOPE("loop_iteration");
        exampleFunction(1000);
    }
    
    // Example 2: Memory tracking
    std::cout << "\n2. Memory Tracking Demo:" << std::endl;
    std::cout << "   Creating and destroying objects..." << std::endl;
    
    {
        ExampleClass obj1(1000);
        obj1.process();
        
        ExampleClass obj2(2000);
        obj2.process();
    }
    
    // Example 3: Thread-based profiling
    std::cout << "\n3. Thread-based Profiling Demo:" << std::endl;
    std::cout << "   Running multiple threads..." << std::endl;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i]() {
            PROFILE_SCOPE("worker_thread");
            exampleFunction(5000);
            std::cout << "Thread " << i << " completed" << std::endl;
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Example 4: Display profiling results
    std::cout << "\n4. Profiling Results:" << std::endl;
    std::cout << nlm::gProfiler.getReport() << std::endl;
    
    // Example 5: Display memory debugging results
    std::cout << "\n5. Memory Debugging Results:" << std::endl;
    std::cout << nlm::gMemoryDebugger.getReport() << std::endl;
    
    // Example 6: Save profiling data
    std::cout << "\n6. Saving Profiling Data:" << std::endl;
    nlm::gProfiler.saveToFile("debug_profile.json");
    nlm::gMemoryDebugger.saveToFile("debug_memory.json");
    
    // Example 7: Brain analysis (requires Brain class)
    std::cout << "\n7. Brain Analysis Demo:" << std::endl;
    std::cout << "   Note: Brain analysis requires actual Brain class instance." << std::endl;
    std::cout << "   This would typically be called from within the simulation loop." << std::endl;
    
    // Example 8: Demonstration of debug macros
    std::cout << "\n8. Debug Macro Demo:" << std::endl;
    DEBUG_PRINT("This is a debug message");
    DEBUG_PRINTLN("This is a debug message with line number");
    INFO_PRINT("This is an info message");
    WARNING_PRINT("This is a warning message");
    ERROR_PRINT("This is an error message");
    
    // Example 9: Performance function profiling
    std::cout << "\n9. Performance Function Demo:" << std::endl;
    
    auto profiledResult = nlm::gProfiler.profileFunction("expensive_calculation", []() {
        exampleFunction(20000);
        return 42;
    });
    
    std::cout << "Profiled result: " << profiledResult << std::endl;
    
    // Example 10: Brain profiler demo
    std::cout << "\n10. Brain Profiler Demo:" << std::endl;
    
    {
        auto brainProfiler = nlm::gBrainDebugger.profile("brain_step");
        exampleFunction(10000); // Simulate brain work
    }
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "Profiling and debugging data has been collected and can be analyzed." << std::endl;
    std::cout << "Files saved: debug_profile.json, debug_memory.json" << std::endl;
    
    return 0;
}
