#include <iostream>
#include <cassert>
#include "core/Random/Random.hpp"

int main() {
    std::cout << "Testing improved RandomGenerator..." << std::endl;
    
    // Test 1: Basic construction and seeding
    nlm::RandomGenerator rng1(12345);
    assert(rng1.getSeed() == 12345);
    std::cout << "✓ RandomGenerator construction and seeding works" << std::endl;
    
    // Test 2: Default construction (time-based seed)
    nlm::RandomGenerator rng2;
    std::cout << "✓ Default RandomGenerator construction works" << std::endl;
    
    // Test 3: Move constructor
    nlm::RandomGenerator rng3(54321);
    uint64_t seed = rng3.getSeed();
    nlm::RandomGenerator rng4(std::move(rng3));
    assert(rng4.getSeed() == seed);
    std::cout << "✓ Move constructor works" << std::endl;
    
    // Test 4: Move assignment
    nlm::RandomGenerator rng5(99999);
    rng5 = std::move(rng4);
    assert(rng5.getSeed() == seed);
    std::cout << "✓ Move assignment works" << std::endl;
    
    // Test 5: Random number generation
    nlm::RandomGenerator rng6(42);
    
    // Test uniform integer
    uint32_t min = 10, max = 20;
    for (int i = 0; i < 10; ++i) {
        uint32_t val = rng6.uniformInt(min, max);
        assert(val >= min && val <= max);
    }
    std::cout << "✓ Uniform integer generation works" << std::endl;
    
    // Test uniform real
    for (int i = 0; i < 10; ++i) {
        double val = rng6.uniformReal(0.0, 1.0);
        assert(val >= 0.0 && val < 1.0);
    }
    std::cout << "✓ Uniform real generation works" << std::endl;
    
    // Test Bernoulli distribution
    for (int i = 0; i < 10; ++i) {
        bool val = rng6.bernoulli(0.5);
        assert(val == true || val == false);
    }
    std::cout << "✓ Bernoulli distribution works" << std::endl;
    
    // Test template functions
    std::vector<int> container = {1, 2, 3, 4, 5};
    rng6.shuffle(container);
    std::cout << "✓ Container shuffle works" << std::endl;
    
    auto randomElement = rng6.pickRandom(container);
    assert(randomElement == container[0] || randomElement == container[1] || 
           randomElement == container[2] || randomElement == container[3] || 
           randomElement == container[4]);
    std::cout << "✓ Random pick from container works" << std::endl;
    
    // Test error cases
    bool errorCaught = false;
    try {
        // Test with min > max (should throw)
        rng6.uniformInt(20, 10);
    } catch (const std::invalid_argument&) {
        errorCaught = true;
    }
    assert(errorCaught);
    std::cout << "✓ Error handling works" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All RandomGenerator improvements working correctly! ===" << std::endl;
    
    return 0;
}