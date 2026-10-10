#include "performance/TimeProfiler.hpp"
#include "performance/MemoryProfiler.hpp"
#include "performance/SpikeProfiler.hpp"
#include "performance/ConnectivityProfiler.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "Testing NLM Performance Profiling Tools..." << std::endl;
    
    // Test TimeProfiler
    std::cout << "\n1. Testing TimeProfiler:" << std::endl;
    nlm::TimeProfiler timeProfiler;
    timeProfiler.setEnabled(true);
    
    timeProfiler.startProfile("test_operation_1");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeProfiler.stopProfile("test_operation_1");
    
    timeProfiler.startProfile("test_operation_2");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    timeProfiler.stopProfile("test_operation_2");
    
    timeProfiler.printSummary();
    
    // Test MemoryProfiler
    std::cout << "\n2. Testing MemoryProfiler:" << std::endl;
    nlm::MemoryProfiler memoryProfiler;
    memoryProfiler.setEnabled(true);
    
    memoryProfiler.recordAllocation("memory_test", 1024, 10);
    memoryProfiler.recordAllocation("memory_test", 2048, 5);
    memoryProfiler.recordDeallocation("memory_test", 1024, 3);
    
    memoryProfiler.printSummary();
    
    // Test SpikeProfiler
    std::cout << "\n3. Testing SpikeProfiler:" << std::endl;
    nlm::SpikeProfiler spikeProfiler;
    spikeProfiler.setEnabled(true);
    
    spikeProfiler.recordSpikeActivity("spike_test", 100, 50, std::chrono::microseconds(500));
    spikeProfiler.recordSpikeActivity("spike_test", 200, 80, std::chrono::microseconds(300));
    
    spikeProfiler.printSummary();
    
    // Test ConnectivityProfiler
    std::cout << "\n4. Testing ConnectivityProfiler:" << std::endl;
    nlm::ConnectivityProfiler connectivityProfiler;
    connectivityProfiler.setEnabled(true);
    
    connectivityProfiler.recordConnectivity("connectivity_test", 1000, 15000, 0.15f, 0.02f, 5, 50);
    connectivityProfiler.recordConnectivity("connectivity_test", 2000, 40000, 0.12f, 0.015f, 10, 100);
    
    connectivityProfiler.printSummary();
    
    std::cout << "\nAll profiling tools tested successfully!" << std::endl;
    return 0;
}
