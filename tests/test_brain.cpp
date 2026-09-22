// Brain Tests - Modern Google Test Version
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class BrainTests : public BrainTestFixture {
public:
    void TestBrainCreation() {
        nlm::Brain brain(config);
        EXPECT_EQ(brain.getRegionCount(), 0);
        EXPECT_EQ(brain.getTotalNeuronCount(), 0);
    }
    
    void TestBrainInitialization() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        // Verify brain initialized with expected properties
        EXPECT_EQ(brain.getRegionCount(), 2);
        EXPECT_EQ(brain.getTotalNeuronCount(), 100);
        EXPECT_GT(brain.getTotalSynapseCount(), 0);
    }
    
    void TestBrainStep() {
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Just verify it doesn't crash
        EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    }
    
    void TestBrainReset() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        brain.step(0);
        brain.reset();
        
        // Verify reset worked
        EXPECT_EQ(brain.getTotalNeuronCount(), 50);
    }
    
    void TestBrainRegions() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        auto regionIds = brain.getRegionIds();
        EXPECT_EQ(regionIds.size(), 3);
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            EXPECT_NE(region, nullptr);
        }
    }
    
    void TestBrainActionProduction() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        auto action = brain.produceAction();
        EXPECT_NE(action, nullptr);
    }
    
    void TestBrainStatus() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        nlm::Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        // Verify statistics are accessible
        EXPECT_GT(brain.getTotalNeuronCount(), 0);
        EXPECT_GE(brain.getTotalSynapseCount(), 0);
        EXPECT_GE(brain.getFiringNeuronCount(), 0);
        EXPECT_GE(brain.getAverageFiringRate(), 0.0f);
    }
};

TEST_F(BrainTests, TestBrainCreation) { TestBrainCreation(); }
TEST_F(BrainTests, TestBrainInitialization) { TestBrainInitialization(); }
TEST_F(BrainTests, TestBrainStep) { TestBrainStep(); }
TEST_F(BrainTests, TestBrainReset) { TestBrainReset(); }
TEST_F(BrainTests, TestBrainRegions) { TestBrainRegions(); }
TEST_F(BrainTests, TestBrainActionProduction) { TestBrainActionProduction(); }
TEST_F(BrainTests, TestBrainStatus) { TestBrainStatus(); }

} // namespace test
} // namespace nlm
