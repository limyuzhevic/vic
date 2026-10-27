// Benchmark test main
#include "test_performance.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Performance Benchmark Test ===" << std::endl;
    std::cout << "Testing system performance and scalability..." << std::endl;
    
    try {
        test_performance::testPerformanceBenchmark();
        std::cout << "\n✓ Performance benchmark passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Benchmark failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Benchmark failed with unknown exception" << std::endl;
        return 1;
    }
}
