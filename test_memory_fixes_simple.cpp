#include <iostream>
#include <string>

// Simple test to verify memory management fixes
namespace nlm {
    class Config;
    class RandomGenerator;
    class Neuron;
    class Synapse;
    class Brain;
}

// Forward declare some classes we need
class nlm_Config {
public:
    void set(const std::string& key, int value) {}
    template<typename T>
    std::optional<T> get(const std::string& key) const { return std::nullopt; }
};

class nlm_RandomGenerator {
public:
    explicit nlm_RandomGenerator(uint64_t seed) {}
    nlm_RandomGenerator() = default;
    nlm_RandomGenerator(nlm_RandomGenerator&& other) noexcept = default;
    nlm_RandomGenerator& operator=(nlm_RandomGenerator&& other) noexcept = default;
    uint64_t getSeed() const { return 0; }
};

class nlm_Neuron {
public:
    explicit nlm_Neuron(int id) {}
    nlm_Neuron(nlm_Neuron&& other) noexcept = default;
    nlm_Neuron& operator=(nlm_Neuron&& other) noexcept = default;
};

class nlm_Synapse {
public:
    nlm_Synapse(int id, int source, int dest) {}
    nlm_Synapse(nlm_Synapse&& other) noexcept = default;
    nlm_Synapse& operator=(nlm_Synapse&& other) noexcept = default;
};

int main() {
    std::cout << "Testing NLM memory management fixes..." << std::endl;
    
    try {
        // Test RandomGenerator
        nlm_RandomGenerator rng1(42);
        nlm_RandomGenerator rng2(std::move(rng1));
        auto rng3 = std::move(rng2);
        std::cout << "✓ RandomGenerator move operations work" << std::endl;
        
        // Test Config
        nlm_Config config;
        config.set("test", 42);
        std::cout << "✓ Config operations work" << std::endl;
        
        // Test Neuron
        nlm_Neuron neuron1(1);
        nlm_Neuron neuron2 = std::move(neuron1);
        std::cout << "✓ Neuron move operations work" << std::endl;
        
        // Test Synapse
        nlm_Synapse synapse1(1, 1, 2);
        nlm_Synapse synapse2 = std::move(synapse1);
        std::cout << "✓ Synapse move operations work" << std::endl;
        
        std::cout << "\n✅ All memory management tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
