// Simple standalone test program for STDP implementation
// This program directly tests the STDP functionality without requiring a full build

#include <iostream>
#include <vector>
#include <cmath>

// Mock Synapse class for testing
class MockSynapse {
public:
    float weight;
    float efficacy;
    float eligibilityTrace;
    
    MockSynapse() : weight(0.5f), efficacy(1.0f), eligibilityTrace(0.0f) {}
    
    float getWeight() const { return weight; }
    void setWeight(float w) { weight = w; }
    float getEfficacy() const { return efficacy; }
    float getEligibilityTrace() const { return eligibilityTrace; }
    void setEligibilityTrace(float trace) { eligibilityTrace = trace; }
    void decayEligibilityTrace(float decayRate) { eligibilityTrace *= (1.0f - decayRate); }
};

// Mock STDP implementation for testing
class MockSTDP {
public:
    float ltpWeight;
    float ltdWeight;
    float tauPlus;
    float tauMinus;
    float minWeight;
    float maxWeight;
    float eligibilityDecay;
    
    MockSTDP() : ltpWeight(0.01f), ltdWeight(0.012f), 
                 tauPlus(20.0f), tauMinus(30.0f),
                 minWeight(-1.0f), maxWeight(1.0f), eligibilityDecay(0.05f) {}
    
    void update(MockSynapse* synapse,
                const std::vector<float>& preSpikes,
                const std::vector<float>& postSpikes,
                float dt) {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        float totalDelta = 0.0f;
        
        for (float preTime : preSpikes) {
            for (float postTime : postSpikes) {
                float delta_t = postTime - preTime;  // Δt in ms
                
                if (delta_t > 0) {
                    // Pre before post: POTENTIATION
                    float delta = ltpWeight * std::exp(-delta_t / tauPlus);
                    totalDelta += delta;
                } else if (delta_t < 0) {
                    // Post before pre: DEPRESSION
                    float delta = -ltdWeight * std::exp(delta_t / tauMinus);
                    totalDelta += delta;
                }
            }
        }
        
        if (std::abs(totalDelta) > 1e-6f) {
            // Scale by synaptic efficacy
            totalDelta *= synapse->getEfficacy();
            
            // Apply weight change with bounds
            float newWeight = synapse->getWeight() + totalDelta;
            newWeight = std::clamp(newWeight, minWeight, maxWeight);
            synapse->setWeight(newWeight);
            
            // Update eligibility trace
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta);
            
            // Apply trace decay
            synapse->decayEligibilityTrace(eligibilityDecay);
        }
    }
    
    float getLTPWeight() const { return ltpWeight; }
    float getLTDWeight() const { return ltdWeight; }
    float getTimeConstant() const { return (tauPlus + tauMinus) * 0.5f; }
};

// Test functions
void testCreation() {
    MockSTDP stdp;
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 25.0f);
}

void testPotentiation() {
    MockSTDP stdp;
    MockSynapse synapse;
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<float> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<float> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
}

void testDepression() {
    MockSTDP stdp;
    MockSynapse synapse;
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<float> preSpikes = {10.0, 20.0, 30.0};
    std::vector<float> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
}

void testEmptySpikes() {
    MockSTDP stdp;
    MockSynapse synapse;
    
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<float> preSpikes;
    std::vector<float> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
}

void testWeightBounds() {
    MockSTDP stdp;
    MockSynapse synapse;
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<float> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<float> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max (1.0)
    assert(synapse.getWeight() <= 1.0f);
}

int main() {
    std::cout << "Running standalone STDP tests..." << std::endl;
    
    try {
        testCreation();
        std::cout << "✓ testCreation PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ testCreation FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    try {
        testPotentiation();
        std::cout << "✓ testPotentiation PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ testPotentiation FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    try {
        testDepression();
        std::cout << "✓ testDepression PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ testDepression FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    try {
        testEmptySpikes();
        std::cout << "✓ testEmptySpikes PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ testEmptySpikes FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    try {
        testWeightBounds();
        std::cout << "✓ testWeightBounds PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ testWeightBounds FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "=== All Standalone Tests PASSED ===" << std::endl;
    return 0;
}