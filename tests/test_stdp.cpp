// STDP Tests - Enhanced with comprehensive validation
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace test_stdp {

// STDP validation test classes
class STDPPotentiatingValidationTest : public test_framework::TestBase {
private:
    float ltpWeight_;
    float ltdWeight_;
    float timeConstant_;
public:
    STDPPotentiatingValidationTest(const std::string& name, float ltp, float ltd, float tc)
        : TestBase(name, test_framework::TestCategory::Unit), 
          ltpWeight_(ltp), ltdWeight_(ltd), timeConstant_(tc) {}
    
    void run() override {
        nlm::STDP stdp;
        stdp.configure(ltpWeight_, ltdWeight_, timeConstant_);
        
        // Create a synapse
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        // Pre-before-post (positive dt) - should potentiate
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        // Weight should have increased due to pre-before-post
        assert(finalWeight > initialWeight);
        assert(finalWeight <= 1.0f);  // Should respect upper bound
        
        // Test with different spike timings
        std::fill(preSpikes.begin(), preSpikes.end(), 0.0);
        std::fill(postSpikes.begin(), postSpikes.end(), 1.0);  // Post immediately after pre
        
        float weightBefore = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float weightAfter = synapse.getWeight();
        
        assert(weightAfter > weightBefore);
        
        std::cout << "    STDP potentiating validation test for " << getName() << " passed" << std::endl;
    }
};

class STDPDepressingValidationTest : public test_framework::TestBase {
public:
    STDPDepressingValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::STDP stdp;
        
        // Create a synapse
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        // Post-before-pre (negative dt) - should depress
        std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        // Weight should have decreased due to post-before-pre
        assert(finalWeight < initialWeight);
        assert(finalWeight >= 0.0f);  // Should respect lower bound
        
        // Test with larger time gaps
        std::fill(preSpikes.begin(), preSpikes.end(), 20.0);
        std::fill(postSpikes.begin(), postSpikes.end(), 15.0);
        
        float weightBefore = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float weightAfter = synapse.getWeight();
        
        assert(weightAfter < weightBefore);
        
        std::cout << "    STDP depressing validation test for " << getName() << " passed" << std::endl;
    }
};

class STDPWeightBoundsValidationTest : public test_framework::TestBase {
public:
    STDPWeightBoundsValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::STDP stdp;
        
        // Test with aggressive configuration
        stdp.configure(0.5f, 0.6f, 10.0f);  // High potentiation
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.99f);  // Near max
        
        // Very strong potentiation
        std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Weight should be clamped to max
        assert(synapse.getWeight() <= 1.0f);
        assert(synapse.getWeight() >= 0.0f);
        
        // Test depression with very low initial weight
        stdp.configure(0.01f, 0.02f, 5.0f);  // Low depression
        synapse.setWeight(0.01f);
        
        std::fill(preSpikes.begin(), preSpikes.end(), 10.0);
        std::fill(postSpikes.begin(), postSpikes.end(), 5.0);
        
        float weightBefore = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float weightAfter = synapse.getWeight();
        
        // Weight should not go negative
        assert(weightAfter >= 0.0f);
        
        std::cout << "    STDP weight bounds validation test for " << getName() << " passed" << std::endl;
    }
};

class STDPTimeConstantValidationTest : public test_framework::TestBase {
public:
    STDPTimeConstantValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::STDP stdp;
        
        // Test time constant range validation
        stdp.setTimeConstant(5.0f);
        assert(stdp.getTimeConstant() == 5.0f);
        
        stdp.setTimeConstant(50.0f);
        assert(stdp.getTimeConstant() == 50.0f);
        
        // Test extreme values
        stdp.setTimeConstant(1.0f);
        assert(stdp.getTimeConstant() == 1.0f);
        
        stdp.setTimeConstant(1000.0f);
        assert(stdp.getTimeConstant() == 1000.0f);
        
        // Test configuration method
        stdp.configure(0.01f, 0.02f, 15.0f);
        assert(stdp.getTimeConstant() == 15.0f);
        
        std::cout << "    STDP time constant validation test for " << getName() << " passed" << std::endl;
    }
};

class STDPEmptySpikesValidationTest : public test_framework::TestBase {
public:
    STDPEmptySpikesValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        float initialWeight = synapse.getWeight();
        
        // Test with empty pre spikes - should not change
        std::vector<nlm::Timestamp> preSpikes;
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == initialWeight);
        
        // Test with empty post spikes - should not change
        preSpikes = {1.0, 2.0};
        postSpikes = {};
        
        float weightBefore = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == weightBefore);
        
        // Test with both empty
        preSpikes = {};
        postSpikes = {};
        
        float weightBefore2 = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == weightBefore2);
        
        std::cout << "    STDP empty spikes validation test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests (Original tests)
void testSTDPCreation() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.035f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    std::cout << "    testSTDPName passed" << std::endl;
}

void testSTDPPotentiation() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPDepression() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
    
    std::cout << "    testSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikes passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    
    // Run original unit tests
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    
    std::cout << "\n=== Running STDP Validation Tests ===" << std::endl;
    
    // Run validation tests
    std::vector<std::tuple<float, float, float>> stdpConfigs = {
        {0.01f, 0.012f, 20.0f},
        {0.05f, 0.06f, 30.0f},
        {0.03f, 0.035f, 25.0f},
        {0.1f, 0.12f, 15.0f},
        {0.02f, 0.03f, 50.0f}
    };
    
    for (size_t i = 0; i < stdpConfigs.size(); ++i) {
        auto& config = stdpConfigs[i];
        STDPPotentiatingValidationTest test(
            "potentiating_validation_" + std::to_string(i),
            std::get<0>(config), std::get<1>(config), std::get<2>(config)
        );
        test.run();
    }
    
    // Run other validation tests
    STDPDepressingValidationTest depressingTest("depressing_validation");
    depressingTest.run();
    
    STDPWeightBoundsValidationTest weightBoundsTest("weight_bounds_validation");
    weightBoundsTest.run();
    
    STDPTimeConstantValidationTest timeConstantTest("time_constant_validation");
    timeConstantTest.run();
    
    STDPEmptySpikesValidationTest emptySpikesTest("empty_spikes_validation");
    emptySpikesTest.run();
    
    std::cout << "\n=== All STDP Tests Completed ===" << std::endl;
}

} // namespace test_stdp