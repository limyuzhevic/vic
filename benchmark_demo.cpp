#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "benchmarks/BenchmarkSuite.hpp"

// Simple demonstration of benchmarking capabilities
class BenchmarkDemo {
public:
    static void demonstrateCoreBenchmarks() {
        std::cout << "=== NLM Core Benchmarks Demo ===\n" << std::endl;
        
        // Create benchmark suite
        nlm::benchmark::ComprehensiveBenchmarkSuite suite("benchmark_results");
        
        // Run core benchmarks
        std::cout << "Running brain performance benchmarks...\n";
        std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        suite.runBrainBenchmarks(neuronCounts);
        
        std::cout << "Running memory allocation benchmarks...\n";
        std::vector<size_t> objectCounts = {1000, 10000, 100000};
        suite.runMemoryBenchmarks(objectCounts);
        
        std::cout << "Running SIMD effectiveness benchmarks...\n";
        std::vector<float> testData(10000, 1.0f);
        suite.runSIMDBenchmarks(testData);
        
        std::cout << "Core benchmarks completed.\n" << std::endl;
    }
    
    static void demonstrateScalabilityBenchmarks() {
        std::cout << "=== NLM Scalability Benchmarks Demo ===\n" << std::endl;
        
        // Create benchmark suite
        nlm::benchmark::ComprehensiveBenchmarkSuite suite("benchmark_results");
        
        // Run scalability benchmarks
        std::cout << "Running neuronal count scalability tests...\n";
        std::vector<size_t> neuronCounts = {100, 1000, 10000, 50000};
        suite.runScalabilityBenchmarks();
        
        std::cout << "Running region count scalability tests...\n";
        std::vector<size_t> regionCounts = {1, 5, 10, 20, 50};
        
        std::cout << "Running connection density scalability tests...\n";
        std::vector<double> densities = {0.01, 0.1, 0.5, 0.9};
        
        std::cout << "Scalability benchmarks completed.\n" << std::endl;
    }
    
    static void demonstrateAdvancedMetrics() {
        std::cout << "=== NLM Advanced Metrics Demo ===\n" << std::endl;
        
        // Create benchmark suite
        nlm::benchmark::ComprehensiveBenchmarkSuite suite("benchmark_results");
        
        // Run integration benchmarks
        std::cout << "Running end-to-end integration benchmarks...\n";
        std::vector<size_t> networkSizes = {100, 1000, 5000, 10000};
        suite.runIntegrationBenchmarks(networkSizes);
        
        // Get results and calculate advanced metrics
        std::vector<nlm::benchmark::BenchmarkMetrics> results = suite.getResults();
        
        if (!results.empty()) {
            std::cout << "Benchmark Results Summary:\n";
            for (const auto& result : results) {
                std::cout << "  - " << result.name << ": "
                          << "OPS: " << result.opsPerSecond << " "
                          << "FLOPs/s: " << result.flopsPerSecond << " "
                          << "Cache Hit: " << result.cacheHitRatio * 100 << "%" << std::endl;
            }
        }
        
        std::cout << "Advanced metrics analysis completed.\n" << std::endl;
    }
    
    static void demonstrateCIIntegration() {
        std::cout << "=== NLM CI/CD Integration Demo ===\n" << std::endl;
        
        // Create benchmark suite with CI enabled
        nlm::benchmark::ComprehensiveBenchmarkSuite suite("benchmark_results");
        suite.setCIEnabled(true);
        suite.setCIBaseline("baseline.json");
        
        // Update baseline
        std::cout << "Updating CI baseline...\n";
        suite.updateCIBaseline();
        
        // Run benchmarks
        std::cout << "Running benchmarks for CI integration...\n";
        suite.runAllBenchmarkSuites();
        
        // Run CI integration
        bool success = suite.runCIIntegration();
        
        if (success) {
            std::cout << "CI integration completed successfully.\n" << std::endl;
        } else {
            std::cout << "CI integration detected performance regressions.\n" << std::endl;
        }
        
        // Export results for CI
        std::cout << "Exporting results for CI...\n";
        suite.exportForCI();
        
        std::cout << "CI/CD integration demo completed.\n" << std::endl;
    }
    
    static void demonstrateComprehensiveBenchmarking() {
        std::cout << "=== Comprehensive NLM Benchmarking Suite Demo ===\n" << std::endl;
        
        // Create comprehensive benchmark suite
        nlm::benchmark::ComprehensiveBenchmarkSuite suite("benchmark_results");
        
        // Run all benchmark suites
        std::cout << "Running comprehensive benchmark suite...\n";
        suite.runAllBenchmarkSuites();
        
        // Generate reports
        std::cout << "Generating comprehensive reports...\n";
        suite.generateAllReports();
        
        // Get results
        std::vector<nlm::benchmark::BenchmarkMetrics> results = suite.getResults();
        
        // Generate performance scorecard
        if (!results.empty()) {
            std::string scorecard = suite.generatePerformanceScorecard(results);
            std::cout << "Performance Scorecard:\n" << scorecard << std::endl;
        }
        
        // Generate analysis
        std::string analysis = suite.analyzePerformance(results);
        std::cout << "Performance Analysis:\n" << analysis << std::endl;
        
        // Identify bottlenecks
        std::string bottlenecks = suite.identifyBottlenecks(results);
        std::cout << "Bottleneck Analysis:\n" << bottlenecks << std::endl;
        
        // Generate recommendations
        std::string recommendations = suite.generateRecommendations(results);
        std::cout << "Recommendations:\n" << recommendations << std::endl;
        
        // Save results
        std::cout << "Saving results...\n";
        suite.saveAllResults();
        
        // Export for CI
        std::cout << "Exporting for CI...\n";
        suite.exportForCI();
        
        std::cout << "Comprehensive benchmarking demo completed.\n" << std::endl;
    }
    
    static void printBenchmarkInfo() {
        std::cout << "NLM Performance Benchmarking Suite\n";
        std::cout << "=================================\n" << std::endl;
        std::cout << "\nAvailable Benchmark Categories:\n";
        std::cout << "  1. Core Benchmarks\n";
        std::cout << "     - Brain Performance (neuron/step timing, spike processing)\n";
        std::cout << "     - Memory Allocation (pool vs heap performance)\n";
        std::cout << "     - SIMD Effectiveness (scalar vs vectorized performance)\n";
        std::cout << "     - Integration (end-to-end simulation performance)\n";
        std::cout << "\n  2. Benchmarking Infrastructure\n";
        std::cout << "     - Performance measurement framework with multiple metrics\n";
        std::cout << "     - Statistical analysis of benchmark results\n";
        std::cout << "     - Performance profiling with flame graphs\n";
        std::cout << "     - Memory usage tracking and analysis\n";
        std::cout << "     - CPU cache efficiency measurement\n";
        std::cout << "\n  3. Scalability Testing\n";
        std::cout << "     - Neuronal count scalability (100 -> 1M neurons)\n";
        std::cout << "     - Region count scalability (1 -> 100 regions)\n";
        std::cout << "     - Connection density scalability (0.01 -> 0.99)\n";
        std::cout << "     - Memory footprint analysis\n";
        std::cout << "     - Time complexity analysis\n";
        std::cout << "\n  4. Comparative Benchmarking\n";
        std::cout << "     - Comparison with reference implementations\n";
        std::cout << "     - Performance regression testing\n";
        std::cout << "     - Optimization effectiveness measurement\n";
        std::cout << "     - Cross-architecture benchmarking\n";
        std::cout << "     - Hardware-specific optimizations validation\n";
        std::cout << "\n  5. Automated Benchmarking\n";
        std::cout << "     - Benchmark automation and scheduling\n";
        std::cout << "     - Result collection and storage\n";
        std::cout << "     - Benchmark reproducibility and versioning\n";
        std::cout << "     - CI/CD integration for performance regression detection\n";
        std::cout << "\n  6. Advanced Metrics\n";
        std::cout << "     - Operations per second (OPS) for neural computation\n";
        std::cout << "     - FLOPs per second for mathematical operations\n";
        std::cout << "     - Memory bandwidth utilization\n";
        std::cout << "     - Cache hit/miss ratios\n";
        std::cout << "     - Thread synchronization overhead\n";
        std::cout << "\nTo run specific demos, use:\n";
        std::cout << "  ./benchmark_demo core    - Run core benchmarks\n";
        std::cout << "  ./benchmark_demo scalability - Run scalability benchmarks\n";
        std::cout << "  ./benchmark_demo advanced  - Run advanced metrics benchmarks\n";
        std::cout << "  ./benchmark_demo ci       - Run CI/CD integration demo\n";
        std::cout << "  ./benchmark_demo comprehensive - Run comprehensive benchmarking\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        BenchmarkDemo::printBenchmarkInfo();
        return 0;
    }
    
    std::string demoType = argv[1];
    
    try {
        if (demoType == "core") {
            BenchmarkDemo::demonstrateCoreBenchmarks();
        } else if (demoType == "scalability") {
            BenchmarkDemo::demonstrateScalabilityBenchmarks();
        } else if (demoType == "advanced") {
            BenchmarkDemo::demonstrateAdvancedMetrics();
        } else if (demoType == "ci") {
            BenchmarkDemo::demonstrateCIIntegration();
        } else if (demoType == "comprehensive") {
            BenchmarkDemo::demonstrateComprehensiveBenchmarking();
        } else if (demoType == "help") {
            BenchmarkDemo::printBenchmarkInfo();
        } else {
            std::cerr << "Unknown demo type: " << demoType << std::endl;
            BenchmarkDemo::printBenchmarkInfo();
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}