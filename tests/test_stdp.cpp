// STDP (Spike-Timing Dependent Plasticity) Tests
// Test STDP algorithm implementation

#include <gtest/gtest.h>
#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include <vector>

namespace nlm {

class STDPTest : public ::testing::Test {
protected:
    void SetUp() override {
        stdp = std::make_unique<STDP>();
        synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
        synapse->setWeight(0.5f);
    }
    
    void TearDown() override {
        stdp.reset();
        synapse.reset();
    }
    
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Synapse> synapse;
};

TEST_F(STDPTest, Plasticity) {
    // Test STDP plasticity
    std::vector<Timestamp> preSpikes = {100.0f};
    std::vector<Timestamp> postSpikes = {110.0f};  // Δt = 10ms positive
    
    float initialWeight = synapse->getWeight();
    stdp->update(synapse.get(), preSpikes, postSpikes, 1.0f);
    float newWeight = synapse->getWeight();
    
    EXPECT_GT(newWeight, initialWeight);  // Should potentiate
}

TEST_F(STDPTest, WeightDepression) {
    // Test LTD (post before pre)
    std::vector<Timestamp> preSpikes = {110.0f};
    std::vector<Timestamp> postSpikes = {100.0f};  // Δt = -10ms negative
    
    float initialWeight = synapse->getWeight();
    stdp->update(synapse.get(), preSpikes, postSpikes, 1.0f);
    float newWeight = synapse->getWeight();
    
    EXPECT_LT(newWeight, initialWeight);  // Should depress
}

TEST_F(STDPTest, ErrorConditions) {
    // Test error conditions
    std::vector<Timestamp> empty;
    EXPECT_NO_THROW(stdp->update(synapse.get(), empty, empty, 1.0f));
}

} // namespace nlm

