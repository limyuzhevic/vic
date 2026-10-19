// Performance Monitoring and Profiling
// Example usage and demonstration of profiling capabilities

#include "core/Profiler.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrateBasicProfiling() {
    std::cout << "=== Basic Profiling Example ===" << std::endl;
    
    nlm::Profiler profiler;
    
    // Profile different operations
    profiler.start("neuron_computation");
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate work
    profiler.stop("neuron_computation");
    
    profiler.start("synapse_computation");
    std::this_thread::sleep_for(std::chrono::milliseconds(25)); // Simulate more work
    profiler.stop("synapse_computation");
    
    profiler.start("plasticity_computation");
    std::this_thread::sleep_for(std::chrono::milliseconds(15)); // Simulate work
    profiler.stop("plasticity_computation");
    
    std::cout << profiler.getReport() << std::endl;
}

void demonstrateScopedProfiling() {
    std::cout << "=== Scoped Profiling Example ===" << std::endl;
    
    // Enable global profiling
    nlm::perf::enable(true);
    
    // This will automatically profile the scope
    {
        nlm::perf::ScopedProfiler profiler("brain_step");
        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Simulate brain step
        
        // Nested profiling
        {
            nlm::perf::ScopedProfiler neuronUpdate("neuron_update");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        {
            nlm::perf::ScopedProfiler spikeUpdate("spike_processing");
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    
    nlm::perf::printReport();
}

void demonstrateGlobalProfiler() {
    std::cout << "=== Global Profiler Example ===" << std::endl;
    
    // Use the global profiler instance
    nlm::Profiler& globalProfiler = nlm::getGlobalProfiler();
    globalProfiler.enable(true);
    
    // Simulate multiple operations
    {
        nlm::Timer timer("main_simulation_loop");
        
        for (int i = 0; i < 5; ++i) {
            nlm::perf::ScopedProfiler step("simulation_step");
            std::this_thread::sleep_for(std::chrono::milliseconds(5 + (i * 2)));
        }
    }
    
    std::cout << globalProfiler.getReport() << std::endl;
}

void demonstratePerformanceComparison() {
    std::cout << "=== Performance Comparison Example ===" << std::endl;
    
    nlm::perf::enable(true);
    
    // Test optimized vs unoptimized approaches
    std::cout << "Testing optimized approach..." << std::endl;
    {
        nlm::perf::ScopedProfiler optimized("optimized_algorithm");
        // Simulate optimized algorithm
        for (int i = 0; i < 10000; ++i) {
            double x = i * 0.1;
            double y = x * x;
            volatile double z = y / (x + 1); // Prevent optimization
        }
    }
    
    std::cout << "Testing unoptimized approach..." << std::endl;
    {
        nlm::perf::ScopedProfiler unoptimized("unoptimized_algorithm");
        // Simulate unoptimized algorithm
        for (int i = 0; i < 10000; ++i) {
            double x = static_cast<double>(rand()) / RAND_MAX;
            double y = x * x;
            double z = y / (x + 1);
        }
    }
    
    nlm::perf::printReport();
}

void demonstrateRealWorldScenario() {
    std::cout << "=== Real-World Neural Network Simulation ===" << std::endl;
    
    nlm::perf::enable(true);
    
    // Simulate a complete neural network simulation
    std::cout << "Initializing neural network..." << std::endl;
    {
        nlm::perf::ScopedProfiler init("network_initialization");
        // Network initialization code
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "Running simulation steps..." << std::endl;
    for (int step = 0; step < 10; ++step) {
        {
            nlm::perf::ScopedProfiler stepProfiler("simulation_step");
            std::cout << "  Step " << step << ": ";
            
            // Different phases of the brain step
            {
                nlm::perf::ScopedProfiler neuronPhase("neuron_phase");
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                std::cout << "neuron ";
            }
            
            {
                nlm::perf::ScopedProfiler spikePhase("spike_phase");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::cout << "spike ";
            }
            
            {
                nlm::perf::ScopedProfiler plasticityPhase("plasticity_phase");
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                std::cout << "plasticity ";
            }
            
            {
                nlm::perf::ScopedProfiler memoryPhase("memory_phase");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::cout << "memory";
            }
            
            std::cout << " complete" << std::endl;
        }
    }
    
    std::cout << "\n=== Performance Report ===" << std::endl;
    nlm::perf::printReport();
}

int main() {
    std::cout << "=== NLM Performance Monitoring Examples ===" << std::endl;
    std::cout << "This demonstrates the profiling and performance monitoring" << std::endl;
    std::cout << "capabilities added to the neural network simulation system." << std::endl;
    std::cout << std::endl;
    
    demonstrateBasicProfiling();
    std::cout << std::endl;
    
    demonstrateScopedProfiling();
    std::cout << std::endl;
    
    demonstrateGlobalProfiler();
    std::cout << std::endl;
    
    demonstratePerformanceComparison();
    std::cout << std::endl;
    
    demonstrateRealWorldScenario();
    
    std::cout << "\n=== All Examples Completed ===" << std::endl;
    return 0;
}