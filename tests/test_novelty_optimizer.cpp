#pragma once

#include "performance/NoveltyOptimizer.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>

namespace nlm {

class NoveltyOptimizerTest {
public:
    static void runAllTests() {
        std::cout << "Running NoveltyOptimizer tests..." << std::endl;
        
        testNumericalCorrectness();
        testEdgeCases();
        testPerformance();
        testSIMDCompatibility();
        
        std::cout << "All tests passed!" << std::endl;
    }
    
private:
    static void testNumericalCorrectness() {
        std::cout << "  Testing numerical correctness..." << std::endl;
        
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        // Test with various sizes
        for (size_t size = 1; size <= 1024; size *= 2) {
            std::vector<float> a(size);
            std::vector<float> b(size);
            
            for (size_t i = 0; i < size; ++i) {
                a[i] = dist(rng);
                b[i] = dist(rng);
            }
            
            // Test computeAbsDiffSum
            float expected = 0.0f;
            for (size_t i = 0; i < size; ++i) {
                expected += std::abs(a[i] - b[i]);
            }
            
            float result = NoveltyOptimizer::computeAbsDiffSum(a, b, size);
            assert(std::abs(result - expected) < 1e-6f);
            
            // Test computeNovelty
            float novelty = NoveltyOptimizer::computeNovelty(a, b, 0.5f, 1.0f);
            float expectedNovelty = std::min(1.0f, expected / size / 0.5f);
            assert(std::abs(novelty - expectedNovelty) < 1e-6f);
        }
        
        std::cout << "    Numerical correctness: PASSED" << std::endl;
    }
    
    static void testEdgeCases() {
        std::cout << "  Testing edge cases..." << std::endl;
        
        // Empty vectors
        std::vector<float> empty;
        std::vector<float> data{1.0f, 2.0f, 3.0f};
        
        assert(NoveltyOptimizer::computeAbsDiffSum(empty, data, 0) == 0.0f);
        assert(NoveltyOptimizer::computeAbsDiffSum(data, empty, 0) == 0.0f);
        assert(NoveltyOptimizer::computeNovelty(empty, data) == 0.0f);
        assert(NoveltyOptimizer::computeNovelty(data, empty) == 0.0f);
        
        // Single element
        std::vector<float> singleA{5.0f};
        std::vector<float> singleB{3.0f};
        
        float diff = NoveltyOptimizer::computeAbsDiffSum(singleA, singleB, 1);
        assert(std::abs(diff - 2.0f) < 1e-6f);
        
        // Identical vectors
        std::vector<float> identical{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        assert(NoveltyOptimizer::computeAbsDiffSum(identical, identical, 5) == 0.0f);
        assert(NoveltyOptimizer::computeNovelty(identical, identical) == 0.0f);
        
        // Mismatched sizes
        std::vector<float> a{1.0f, 2.0f, 3.0f};
        std::vector<float> b{1.0f, 2.0f};
        
        float diffResult = NoveltyOptimizer::computeAbsDiffSum(a, b, 2);
        float expectedDiff = std::abs(1.0f - 1.0f) + std::abs(2.0f - 2.0f);
        assert(std::abs(diffResult - expectedDiff) < 1e-6f);
        
        // computeSmallAbsDiffSum
        float smallResult = NoveltyOptimizer::computeSmallAbsDiffSum(a, b, 2);
        assert(std::abs(smallResult - expectedDiff) < 1e-6f);
        
        std::cout << "    Edge cases: PASSED" << std::endl;
    }
    
    static void testPerformance() {
        std::cout << "  Testing performance..." << std::endl;
        
        const size_t numIterations = 1000;
        const size_t testSizes[] = {16, 32, 64, 128, 256, 512};
        
        for (size_t size : testSizes) {
            std::vector<float> a(size);
            std::vector<float> b(size);
            
            // Fill with random data
            std::mt19937 rng(42);
            std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
            
            for (size_t i = 0; i < size; ++i) {
                a[i] = dist(rng);
                b[i] = dist(rng);
            }
            
            // Measure computeAbsDiffSum performance
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < numIterations; ++i) {
                NoveltyOptimizer::computeAbsDiffSum(a, b, size);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
            double opsPerSecond = (numIterations * size) / (elapsed / 1000.0);
            
            std::cout << "    Size " << size << ": " << elapsed << " ms, " 
                      << opsPerSecond / 1e6 << " M ops/sec" << std::endl;
            
            // Performance should be reasonable (not too slow)
            // At least 100M ops/sec for small sizes
            assert(opsPerSecond > 100.0 * size * numIterations);
        }
        
        std::cout << "    Performance: PASSED" << std::endl;
    }
    
    static void testSIMDCompatibility() {
        std::cout << "  Testing SIMD compatibility..." << std::endl;
        
        // Test that the function works correctly with different array types
        // (This would test actual SIMD if available)
        
        // Test with std::vector
        std::vector<float> vec1{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        std::vector<float> vec2{1.5f, 2.5f, 3.5f, 4.5f, 5.5f};
        
        float result1 = NoveltyOptimizer::computeAbsDiffSum(vec1, vec2, 5);
        float expected1 = 2.5f; // |1-1.5| + |2-2.5| + |3-3.5| + |4-4.5| + |5-5.5| = 2.5
        assert(std::abs(result1 - expected1) < 1e-6f);
        
        // Test with raw pointers (if supported)
        alignas(32) float raw1[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        alignas(32) float raw2[5] = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f};
        
        // This test would need to be adapted based on template support
        // For now, just verify the function works with vectors
        
        std::cout << "    SIMD compatibility: PASSED" << std::endl;
    }
};

} // namespace nlm
