// Performance Profiling Example
// This example demonstrates how to use the NLM Performance Profiling System

#include "PerformanceProfiler.hpp"
#include "CpuProfiler.hpp"
#include "MemoryProfiler.hpp"
#include "ThreadProfiler.hpp"
#include "SimulationProfiler.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

void demonstrateBasicProfiling() {
    std::cout << "=== Basic Performance Profiling Example ===" << std::endl;
    
    // Create and start profiler
    auto profiler = std::make_unique<PerformanceProfiler>();
    profiler->startProfiling();
    
    std::cout << "Profiling started. Performing sample operations..." << std::endl;
    
    // Simulate neural network operations
    for (int i = 0; i < 1000; ++i) {
        // Simulate neuron computation
        double input = 0.0;
        for (int j = 0; j < 100; ++j) {
            input += std::sin(j * 0.1);
        }
        
        double activation = 1.0 / (1.0 + std::exp(-input));
        
        // Simulate spike generation
        if (activation > 0.5) {
            // Generate spike
        }
    }
    
    std::cout << "Operations completed. Stopping profiling..." << std::endl;
    profiler->stopProfiling();
    
    // Get and display performance metrics
    auto metrics = profiler->getCurrentMetrics();
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  CPU Time: " << metrics.cpuTime << "s" << std::endl;
    std::cout << "  Wall Clock Time: " << metrics.wallClockTime << "s" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << " bytes" << std::endl;
    std::cout << "  Active Threads: " << metrics.activeThreads << std::endl;
    std::cout << "  Steps per Second: " << metrics.stepsPerSecond << std::endl;
}

void demonstrateSpecializedProfilers() {
    std::cout << "\n=== Specialized Profilers Example ===" << std::endl;
    
    // CPU Profiler
    auto cpuProfiler = std::make_unique<CpuProfiler>();
    cpuProfiler->startCpuProfiling();
    
    // Simulate CPU-intensive operations
    for (int i = 0; i < 10000; ++i) {
        double result = std::sqrt(i) * std::sin(i * 0.1) * std::cos(i * 0.2);
    }
    
    cpuProfiler->stopCpuProfiling();
    auto cpuMetrics = cpuProfiler->getCurrentMetrics();
    std::cout << "CPU Profiler Results:" << std::endl;
    std::cout << "  CPU Usage: " << cpuMetrics.cpuUsage << std::endl;
    std::cout << "  Average Step Time: " << cpuProfiler->getAverageStepTime() << "s" << std::endl;
    
    // Memory Profiler
    auto memoryProfiler = std::make_unique<MemoryProfiler>();
    memoryProfiler->startMemoryProfiling();
    
    // Simulate memory allocation
    std::vector<std::vector<double>> data;
    for (int i = 0; i < 100; ++i) {
        std::vector<double> row(1000, 0.0);
        for (int j = 0; j < 1000; ++j) {
            row[j] = static_cast<double>(rand()) / RAND_MAX;
        }
        data.push_back(row);
    }
    
    memoryProfiler->stopMemoryProfiling();
    auto memoryMetrics = memoryProfiler->getCurrentMetrics();
    std::cout << "Memory Profiler Results:" << std::endl;
    std::cout << "  Memory Usage: " << memoryMetrics.memoryUsage << " bytes" << std::endl;
    std::cout << "  Peak Memory Usage: " << memoryProfiler->getPeakMemoryUsage() << " bytes" << std::endl;
    std::cout << "  Memory Leak Detected: " << (memoryProfiler->detectMemoryLeak() ? "Yes" : "No") << std::endl;
    
    // Thread Profiler
    auto threadProfiler = std::make_unique<ThreadProfiler>();
    threadProfiler->startThreadProfiling();
    
    // Create multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 1000; ++j) {
                double result = std::sin(j * 0.01) * i;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    threadProfiler->stopThreadProfiling();
    auto threadMetrics = threadProfiler->getCurrentMetrics();
    std::cout << "Thread Profiler Results:" << std::endl;
    std::cout << "  Thread Usage: " << threadMetrics.threadUsage << std::endl;
    std::cout << "  Active Threads: " << threadProfiler->getActiveThreadCount() << std::endl;
    
    // Simulation Profiler
    auto simulationProfiler = std::make_unique<SimulationProfiler>();
    simulationProfiler->startProfiling();
    
    // Simulate neural network simulation
    for (size_t step = 0; step < 100; ++step) {
        // Simulate neural network dynamics
        for (size_t neuron = 0; neuron < 1000; ++neuron) {
            double input = 0.0;
            for (size_t synapse = 0; synapse < 100; ++synapse) {
                input += 0.1 * std::sin(neuron * 0.01 + synapse * 0.001);
            }
            
            double activation = 1.0 / (1.0 + std::exp(-input));
            
            if (activation > 0.5) {
                // Generate spike
            }
        }
        
        // Record simulation metrics
        simulationProfiler->recordStep(step, 0.001, 1000, 10000, 50);
    }
    
    simulationProfiler->stopProfiling();
    auto simulationMetrics = simulationProfiler->getCurrentMetrics();
    std::cout << "Simulation Profiler Results:" << std::endl;
    std::cout << "  Steps per Second: " << simulationMetrics.stepsPerSecond << std::endl;
    std::cout << "  Memory per Step: " << simulationMetrics.memoryPerStep << " bytes" << std::endl;
    std::cout << "  Simulation Efficiency: " << simulationProfiler->getSimulationEfficiency() << std::endl;
}

void demonstratePerformanceReport() {
    std::cout << "\n=== Performance Report Example ===" << std::endl;
    
    PerformanceReport report("NLM Performance Profiling Example");
    
    // Add sample performance metrics
    for (int i = 0; i < 10; ++i) {
        PerformanceMetrics metrics;
        metrics.cpuTime = static_cast<double>(rand()) / RAND_MAX * 2.0;
        metrics.wallClockTime = metrics.cpuTime * 1.5;
        metrics.memoryUsage = static_cast<size_t>(rand()) % 1000000000;
        metrics.activeThreads = rand() % 16 + 1;
        metrics.stepsPerSecond = 1000.0 / (metrics.cpuTime + 0.001);
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        
        report.addPerformanceMetrics(metrics);
    }
    
    // Export report in different formats
    report.exportToText("performance_report.txt");
    report.exportToJSON("performance_report.json");
    report.exportToHTML("performance_report.html");
    
    std::cout << "Performance report generated in multiple formats:" << std::endl;
    std::cout << "  - performance_report.txt (text)" << std::endl;
    std::cout << "  - performance_report.json (JSON)" << std::endl;
    std::cout << "  - performance_report.html (HTML)" << std::endl;
    
    // Calculate performance statistics
    double avgCpuUsage = report.calculateAverageCpuUsage();
    size_t peakMemoryUsage = report.calculatePeakMemoryUsage();
    double avgStepTime = report.calculateAverageStepTime();
    
    std::cout << "\nPerformance Statistics:" << std::endl;
    std::cout << "  Average CPU Usage: " << avgCpuUsage << std::endl;
    std::cout << "  Peak Memory Usage: " << peakMemoryUsage << " bytes" << std::endl;
    std::cout << "  Average Step Time: " << avgStepTime << "s" << std::endl;
}

int main() {
    std::cout << "NLM Performance Profiling Example" << std::endl;
    std::cout << "================================" << std::endl;
    
    demonstrateBasicProfiling();
    demonstrateSpecializedProfilers();
    demonstratePerformanceReport();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    std::cout << "Performance profiling and benchmarking examples completed successfully." << std::endl;
    
    return 0;
}