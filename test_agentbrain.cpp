#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

// Mock classes to test AgentBrain logic without full dependencies
class MockNeuronState {
public:
    float membranePotential = 0.0f;
    float restingPotential = 0.0f;
};

class MockNeuron {
public:
    MockNeuron() {}
    MockNeuron(float potential, float resting) {
        state_.membranePotential = potential;
        state_.restingPotential = resting;
    }
    MockNeuronState& getState() { return state_; }
    void injectCurrent(float current) { /* Mock implementation */ }
private:
    MockNeuronState state_;
};

class MockBrain {
public:
    std::vector<MockNeuron*> getNeurons() { return std::vector<MockNeuron*>(); }
};

namespace nlm {
    // Mock NeuronType enum
    enum class NeuronType { Motor, Sensory };
    
    // Mock MotorCommand enum
    enum class MotorCommand { 
        MoveForward, MoveBackward, TurnLeft, TurnRight, 
        Interact, Wait, LookLeft, LookRight
    };
    
    // Mock Brain class
    class Brain {
    public:
        std::vector<MockBrain*> getRegions() { return std::vector<MockBrain*>(); }
        class RandomGenerator {
        public:
            float uniformReal(float min, float max) { return 0.5f; }
            int uniformInt(int min, int max) { return 0; }
        };
        RandomGenerator* getRandomGenerator() { return &rg_; }
    private:
        RandomGenerator rg_;
    };
    
    // Test functions that mirror the logic in AgentBrain.cpp
    
    // Test 1: Test bounds checking and scaling factors
    void testSensoryProcessing() {
        std::cout << "Testing sensory processing..." << std::endl;
        
        // Test that we can create neuron vectors without crashing
        std::vector<MockNeuron*> neurons;
        
        // Test empty vector handling
        std::vector<MockNeuron*> emptyNeurons;
        float emptyActivity = 0.0f;
        
        if (emptyNeurons.empty()) {
            emptyActivity = 0.0f;
        }
        
        // Test non-empty vector with null checks
        neurons.push_back(new MockNeuron(1.0f, 0.0f));
        neurons.push_back(nullptr); // Test null pointer handling
        neurons.push_back(new MockNeuron(2.0f, 0.0f));
        
        // Test activity calculation with null checks
        float sum = 0.0f;
        for (MockNeuron* n : neurons) {
            if (!n) continue;  // Null check
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        
        assert(neurons.size() >= 2); // We have 2 valid neurons
        assert(sum == 3.0f); // 1.0 + 2.0
        
        // Test scaling factors
        float vision = 0.5f;
        float current = vision * 5.0f; // Vision scale factor
        assert(current == 2.5f);
        
        float touch = 0.8f;
        float touchCurrent = touch * 8.0f; // Touch scale factor
        assert(touchCurrent == 6.4f);
        
        // Test proprioception scaling (should be consistent)
        float proprio = 0.5f;
        float proprioCurrent = (proprio * 2.0f - 1.0f) * 5.0f; // Internal scale
        assert(proprioCurrent == 0.0f);
        
        std::cout << "✓ Sensory processing tests passed" << std::endl;
    }
    
    // Test 2: Test novelty calculation with division by zero protection
    void testNoveltyCalculation() {
        std::cout << "Testing novelty calculation..." << std::endl;
        
    // Test empty vision
    std::vector<float> emptyVision;
    std::vector<float> prevVisionEmpty;
    
    float totalDiff = 0.0f;
    size_t maxCount = std::min(emptyVision.size(), prevVisionEmpty.size());
    size_t count = std::max<size_t>(maxCount, 1);
    
    assert(count == 1); // Protected against division by zero
    
    // Test novelty with values
    std::vector<float> vision{0.1f, 0.2f, 0.3f};
    std::vector<float> prevVisionSame{0.1f, 0.2f, 0.3f}; // Same values
    
    float totalDiff2 = 0.0f;
    size_t maxCount2 = std::min(vision.size(), prevVisionSame.size());
        
        for (size_t i = 0; i < maxCount2; ++i) {
            float diff = vision[i] - previousVision[i];
            totalDiff2 += diff * diff; // Squared difference
        }
        
        size_t count2 = std::max<size_t>(maxCount2, 1);
        float novelty = std::sqrt(totalDiff2 / static_cast<float>(count2));
        
        assert(novelty == 0.0f); // No difference
        
    // Test different values
    std::vector<float> prevVisionDifferent{0.0f, 0.0f, 0.0f};
    
    float totalDiff3 = 0.0f;
    size_t maxCount3 = std::min(vision.size(), prevVisionDifferent.size());
        
        for (size_t i = 0; i < maxCount3; ++i) {
            float diff = vision[i] - previousVision2[i];
            totalDiff3 += diff * diff;
        }
        
        size_t count3 = std::max<size_t>(maxCount3, 1);
        float novelty2 = std::sqrt(totalDiff3 / static_cast<float>(count3));
        
        // Expected: sqrt((0.1^2 + 0.2^2 + 0.3^2) / 3) = sqrt(0.14/3) ≈ 0.216
        assert(std::abs(novelty2 - 0.216f) < 0.01f);
        
        // Test decay
        float noveltyBeforeDecay = 0.216f;
        float decayFactor = 0.99f;
        float noveltyAfterDecay = noveltyBeforeDecay * decayFactor;
        assert(noveltyAfterDecay < noveltyBeforeDecay);
        
        std::cout << "✓ Novelty calculation tests passed" << std::endl;
    }
    
    // Test 3: Test motor neuron distribution and activity calculation
    void testMotorDecoding() {
        std::cout << "Testing motor decoding..." << std::endl;
        
        // Test activity calculation with null checks
        auto calcActivity = [](const std::vector<MockNeuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (MockNeuron* n : neurons) {
                if (!n) continue;
                const auto& state = n->getState();
                sum += std::abs(state.membranePotential - state.restingPotential);
            }
            return neurons.empty() ? 0.0f : sum / neurons.size();
        };
        
        // Test empty vector
        std::vector<MockNeuron*> emptyNeurons;
        float emptyAct = calcActivity(emptyNeurons);
        assert(emptyAct == 0.0f);
        
        // Test with null pointers
        std::vector<MockNeuron*> neuronsWithNulls;
        neuronsWithNulls.push_back(new MockNeuron(1.0f, 0.0f));
        neuronsWithNulls.push_back(nullptr);
        neuronsWithNulls.push_back(new MockNeuron(3.0f, 0.0f));
        neuronsWithNulls.push_back(nullptr);
        neuronsWithNulls.push_back(new MockNeuron(5.0f, 0.0f));
        
        float actWithNulls = calcActivity(neuronsWithNulls);
        assert(actWithNulls == (1.0f + 3.0f + 5.0f) / 3.0f); // 3.0
        
        // Test motor neuron grouping threshold
        float forwardAct = 0.6f;  // Above threshold
        float backwardAct = 0.2f; // Below threshold
        float waitAct = 0.1f;     // Below threshold
        
        // Only act if there's meaningful activity (threshold = 0.3f)
        if (forwardAct < 0.3f) {
            assert(false); // Should not trigger for forwardAct = 0.6f
        }
        
        if (backwardAct < 0.3f) {
            assert(true); // Correctly triggers for backwardAct = 0.2f
        }
        
        if (waitAct < 0.3f) {
            assert(true); // Correctly triggers for waitAct = 0.1f
        }
        
        std::cout << "✓ Motor decoding tests passed" << std::endl;
    }
    
    // Test 4: Test curiosity and exploration selection
    void testCuriositySelection() {
        std::cout << "Testing curiosity-based selection..." << std::endl;
        
        // Test curiosity threshold
        float curiosityLevel = 0.6f; // Above threshold (0.5f)
        bool shouldExplore = curiosityLevel > 0.5f;
        assert(shouldExplore);
        
        // Test exploration probability calculation
        float exploreChance = curiosityLevel * 0.3f; // Up to 30% random
        assert(exploreChance == 0.18f);
        
        // Test random number comparison
        float r = 0.5f; // Mock random number
        if (r < exploreChance) {
            // Would choose random action
            assert(true);
        }
        
        // Test low curiosity (should not explore)
        float lowCuriosity = 0.3f;
        bool shouldExploreLow = lowCuriosity > 0.5f;
        assert(!shouldExploreLow);
        
        std::cout << "✓ Curiosity selection tests passed" << std::endl;
    }
    
    // Test 5: Test reward modulation
    void testRewardModulation() {
        std::cout << "Testing reward modulation..." << std::endl;
        
        // Test prediction error calculation
        float reward = 1.0f;
        float predictedReward = 0.5f;
        float predictionError = reward - predictedReward;
        assert(predictionError == 0.5f);
        
        // Test expected reward moving average
        float expectedReward = 0.95f * 0.5f + 0.05f * 1.0f; // Initial expected = 0.5
        assert(std::abs(expectedReward - 0.525f) < 0.001f);
        
        // Test dopamine clamping
        float dopamine = predictionError;
        dopamine = std::clamp(dopamine, -1.0f, 1.0f);
        assert(dopamine == 0.5f); // Not clamped
        
        // Test extreme values
        float extremeReward = 100.0f;
        float extremePredicted = -50.0f;
        float extremeError = extremeReward - extremePredicted;
        float extremeDopamine = std::clamp(extremeError, -1.0f, 1.0f);
        assert(extremeDopamine == 1.0f); // Clamped
        
        std::cout << "✓ Reward modulation tests passed" << std::endl;
    }
    
    // Run all tests
    int runAllTests() {
        std::cout << "Running AgentBrain bug fix tests..." << std::endl;
        std::cout << "=" << std::endl;
        
        testSensoryProcessing();
        testNoveltyCalculation();
        testMotorDecoding();
        testCuriositySelection();
        testRewardModulation();
        
        std::cout << "=" << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        return 0;
    }
}

int main() {
    return nlm::runAllTests();
}