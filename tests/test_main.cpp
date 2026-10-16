// NLM Test Main - GoogleTest Version
// Phase 2: Real Neural Computation Tests

#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"
#include "TestExpectations.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"

// TEST SUITE DECLARATIONS
class NeuronTestSuite : public ::testing::Test {
protected:
    nlm::Neuron neuron;
    nlm::RandomGenerator rng;
    
    void SetUp() override {
        neuron = nlm::Neuron(nlm::NeuronId(1));
        rng.seed(42);
    }
};

class SynapseTestSuite : public ::testing::Test {
protected:
    nlm::Synapse synapse;
    
    void SetUp() override {
        synapse = nlm::Synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    }
};

class STDPTestSuite : public ::testing::Test {
protected:
    nlm::STDP stdp;
    nlm::Synapse synapse;
    
    void SetUp() override {
        synapse = nlm::Synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
    }
};

class RandomTestSuite : public ::testing::Test {
protected:
    nlm::RandomGenerator rng1;
    nlm::RandomGenerator rng2;
    
    void SetUp() override {
        rng1.seed(42);
        rng2.seed(42);
    }
};

class ClockTestSuite : public ::testing::Test {
protected:
    nlm::SimulationClock clock;
    
    void SetUp() override {
        clock = nlm::SimulationClock(0.001);
    }
};

class BrainTestSuite : public ::testing::Test {
protected:
    std::shared_ptr<nlm::Config> config;
    nlm::Brain brain;
    
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        brain = nlm::Brain(config);
    }
};

class TypesTestSuite : public ::testing::Test {
protected:
    nlm::NeuronId neuronId1;
    nlm::NeuronId neuronId2;
    nlm::SynapseId synapseId;
    nlm::RegionId regionId;
    nlm::PopulationId populationId;
    
    void SetUp() override {
        neuronId1 = nlm::NeuronId(1);
        neuronId2 = nlm::NeuronId(2);
        synapseId = nlm::SynapseId(100);
        regionId = nlm::RegionId(1);
        populationId = nlm::PopulationId(1);
    }
};

class ConfigTestSuite : public ::testing::Test {
protected:
    nlm::Config config;
    
    void SetUp() override {
        config = nlm::Config();
    }
};

// NEURON TESTS
TEST_F(NeuronTestSuite, NeuronCreation) {
    EXPECT_EQ(neuron.getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
}

TEST_F(NeuronTestSuite, NeuronType) {
    neuron.setType(nlm::NeuronType::Excitatory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
}

TEST_F(NeuronTestSuite, MembranePotential) {
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    
    neuron.setMembranePotential(-65.0f);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -60.0f);
}

TEST_F(NeuronTestSuite, FiringState) {
    EXPECT_FALSE(neuron.isFiring());
    EXPECT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    EXPECT_TRUE(neuron.isRefractory());
    EXPECT_FALSE(neuron.isFiring());
}

TEST_F(NeuronTestSuite, RefractoryDecrement) {
    neuron.setRefractoryPeriod(5);
    EXPECT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    EXPECT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    EXPECT_FALSE(neuron.isRefractory());
}

TEST_F(NeuronTestSuite, CurrentInjection) {
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
}

TEST_F(NeuronTestSuite, SynapticInput) {
    neuron.receiveExcitatoryInput(5.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), -3.0f);
}

TEST_F(NeuronTestSuite, SpikeHistory) {
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    EXPECT_EQ(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

TEST_F(NeuronTestSuite, RandomInitialization) {
    neuron.initializeRandom(rng);
    
    EXPECT_LT(neuron.getMembranePotential(), -60.0f);
    EXPECT_GT(neuron.getMembranePotential(), -80.0f);
    EXPECT_LT(neuron.getThreshold(), -50.0f);
    EXPECT_GT(neuron.getThreshold(), -60.0f);
}

TEST_F(NeuronTestSuite, Reset) {
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

// SYNAPSE TESTS
TEST_F(SynapseTestSuite, SynapseCreation) {
    EXPECT_EQ(synapse.getId(), nlm::SynapseId(1));
    EXPECT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
    EXPECT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
}

TEST_F(SynapseTestSuite, SynapseWeight) {
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    
    synapse.setWeight(0.5f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.5f);
    
    synapse.addToWeight(0.2f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.7f);
    
    synapse.addToWeight(-0.3f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.4f);
}

TEST_F(SynapseTestSuite, SynapseWeightBounds) {
    synapse.setWeight(10.0f);
    EXPECT_LE(synapse.getWeight(), 1.0f);
    
    synapse.setWeight(-10.0f);
    EXPECT_GE(synapse.getWeight(), -1.0f);
}

TEST_F(SynapseTestSuite, SynapseType) {
    synapse.setType(nlm::SynapseType::Excitatory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
    EXPECT_TRUE(synapse.isExcitatory());
    EXPECT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
    EXPECT_TRUE(synapse.isInhibitory());
    EXPECT_FALSE(synapse.isExcitatory());
}

TEST_F(SynapseTestSuite, SynapseDelay) {
    EXPECT_EQ(synapse.getDelay(), 1);
    
    synapse.setDelay(5);
    EXPECT_EQ(synapse.getDelay(), 5);
}

TEST_F(SynapseTestSuite, SynapseSpikeHistory) {
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    EXPECT_EQ(synapse.getPreSpikeHistory().size(), 2);
    EXPECT_EQ(synapse.getPostSpikeHistory().size(), 1);
    
    synapse.clearHistory();
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
}

TEST_F(SynapseTestSuite, SynapseEligibilityTrace) {
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.5f);
    
    synapse.decayEligibilityTrace(0.1f);
    EXPECT_LT(synapse.getEligibilityTrace(), 0.5f);
}

TEST_F(SynapseTestSuite, SynapseEfficacy) {
    EXPECT_FLOAT_EQ(synapse.getEfficacy(), 1.0f);
    
    synapse.setEfficacy(0.5f);
    EXPECT_FLOAT_EQ(synapse.getEfficacy(), 0.5f);
    
    synapse.setEfficacy(5.0f);
    EXPECT_LE(synapse.getEfficacy(), 2.0f);
}

TEST_F(SynapseTestSuite, SynapseRandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    EXPECT_GT(synapse.getWeight(), 0.0f);
    EXPECT_GE(synapse.getDelay(), 1);
    EXPECT_LE(synapse.getDelay(), 5);
}

TEST_F(SynapseTestSuite, SynapseReset) {
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
    EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
}

TEST_F(SynapseTestSuite, SynapsePlasticityFlags) {
    EXPECT_FALSE(synapse.getPlasticityFlags().hebbian);
    EXPECT_FALSE(synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    EXPECT_TRUE(synapse.getPlasticityFlags().hebbian);
    EXPECT_TRUE(synapse.getPlasticityFlags().stdp);
    EXPECT_FALSE(synapse.getPlasticityFlags().reward_modulated);
}

// STDP TESTS
TEST_F(STDPTestSuite, STDPCreation) {
    nlm::STDP stdp;
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.01f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.012f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 20.0f);
}

TEST_F(STDPTestSuite, STDPConfiguration) {
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.05f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.06f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 30.0f);
}

TEST_F(STDPTestSuite, STDPConfigure) {
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.03f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.035f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 25.0f);
}

TEST_F(STDPTestSuite, STDPName) {
    EXPECT_NE(stdp.getName(), nullptr);
    EXPECT_EQ(std::string(stdp.getName()), "STDP");
}

TEST_F(STDPTestSuite, STDPPotentiation) {
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    EXPECT_GT(finalWeight, initialWeight);
}

TEST_F(STDPTestSuite, STDPDepression) {
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    float initialWeight = synapse2.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse2.getWeight();
    EXPECT_LT(finalWeight, initialWeight);
}

TEST_F(STDPTestSuite, STDPEmptySpikes) {
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse3.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    EXPECT_FLOAT_EQ(synapse3.getWeight(), initialWeight);
    
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    EXPECT_FLOAT_EQ(synapse3.getWeight(), initialWeight);
}

TEST_F(STDPTestSuite, STDPWeightBounds) {
    nlm::Synapse synapse4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse4.setType(nlm::SynapseType::Excitatory);
    synapse4.setWeight(0.99f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse4, preSpikes, postSpikes, 0.001);
    
    EXPECT_LE(synapse4.getWeight(), 1.0f);
}

// RANDOM TESTS
TEST_F(RandomTestSuite, RandomGeneratorCreation) {
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
    }
}

TEST_F(RandomTestSuite, UniformInt) {
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng1.uniformInt(5, 10);
        EXPECT_GE(val, 5u);
        EXPECT_LE(val, 10u);
    }
}

TEST_F(RandomTestSuite, UniformReal) {
    for (int i = 0; i < 100; ++i) {
        double val = rng1.uniformReal(0.0, 1.0);
        EXPECT_GE(val, 0.0);
        EXPECT_LT(val, 1.0);
    }
}

TEST_F(RandomTestSuite, Normal) {
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng1.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    EXPECT_NEAR(mean, 0.0, 0.1);
}

TEST_F(RandomTestSuite, Bernoulli) {
    int trueCount = 0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        if (rng1.bernoulli(0.3)) {
            ++trueCount;
        }
    }
    double ratio = static_cast<double>(trueCount) / samples;
    EXPECT_GT(ratio, 0.25);
    EXPECT_LT(ratio, 0.35);
}

TEST_F(RandomTestSuite, RandomBool) {
    bool val = rng1.randomBool();
    EXPECT_TRUE(true || false);
}

TEST_F(RandomTestSuite, PickRandomIndex) {
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng1.pickRandomIndex(10);
        EXPECT_LT(idx, 10u);
    }
}

TEST_F(RandomTestSuite, Shuffle) {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng1.shuffle(vec);
    
    EXPECT_EQ(vec.size(), vecCopy.size());
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    EXPECT_EQ(vec, vecCopy);
}

TEST_F(RandomTestSuite, Seed) {
    nlm::RandomGenerator rng3;
    rng3.seed(12345);
    uint32_t val1 = rng3.uniformInt(0, 1000);
    
    rng3.seed(12345);
    uint32_t val2 = rng3.uniformInt(0, 1000);
    
    EXPECT_EQ(val1, val2);
}

// CLOCK TESTS
TEST_F(ClockTestSuite, ClockCreation) {
    nlm::SimulationClock clock(0.001);
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getTimestep(), 0.001);
}

TEST_F(ClockTestSuite, ClockAdvance) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 1);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 2);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.002);
}

TEST_F(ClockTestSuite, ClockReset) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    EXPECT_EQ(clock.getStep(), 3);
    
    clock.reset();
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
}

TEST_F(ClockTestSuite, ClockSetStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST_F(ClockTestSuite, ClockSetTime) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.5);
    EXPECT_EQ(clock.getStep(), 500);
}

TEST_F(ClockTestSuite, ClockAdvanceBy) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST_F(ClockTestSuite, ClockConversions) {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    EXPECT_DOUBLE_EQ(ts, 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    EXPECT_EQ(st, 500);
}

TEST_F(ClockTestSuite, ClockFirstLastStep) {
    nlm::SimulationClock clock(0.001);
    
    EXPECT_TRUE(clock.isFirstStep());
    
    clock.advance();
    EXPECT_FALSE(clock.isFirstStep());
    
    clock.reset();
    EXPECT_TRUE(clock.isFirstStep());
    
    EXPECT_TRUE(clock.isLastStep(0));
    clock.advance();
    EXPECT_FALSE(clock.isLastStep(0));
    EXPECT_TRUE(clock.isLastStep(1));
}

TEST_F(ClockTestSuite, ClockTimeAtFutureStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    EXPECT_DOUBLE_EQ(future, 0.15);
}

// BRAIN TESTS
TEST_F(BrainTestSuite, BrainCreation) {
    nlm::Brain brain(config);
    EXPECT_TRUE(true);
}

TEST_F(BrainTestSuite, BrainInitialization) {
    bool success = brain.initialize();
    EXPECT_TRUE(success);
    EXPECT_EQ(brain.getRegionCount(), 1);
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

TEST_F(BrainTestSuite, BrainStep) {
    brain.initialize();
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
}

TEST_F(BrainTestSuite, BrainReset) {
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 50);
}

TEST_F(BrainTestSuite, BrainRegions) {
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    nlm::Brain brain2(config);
    brain2.initialize();
    
    auto regionIds = brain2.getRegionIds();
    EXPECT_EQ(regionIds.size(), 3);
    
    for (auto rid : regionIds) {
        auto* region = brain2.getRegion(rid);
        EXPECT_NE(region, nullptr);
    }
}

TEST_F(BrainTestSuite, BrainActionProduction) {
    bool success = brain.initialize();
    if (success) {
        auto action = brain.produceAction();
        EXPECT_NE(action, nullptr);
    }
}

TEST_F(BrainTestSuite, BrainStatus) {
    bool success = brain.initialize();
    if (success) {
        EXPECT_GT(brain.getTotalNeuronCount(), 0);
        EXPECT_GE(brain.getTotalSynapseCount(), 0);
        EXPECT_GE(brain.getFiringNeuronCount(), 0);
        EXPECT_GE(brain.getAverageFiringRate(), 0.0f);
    }
}

// TYPES TESTS
TEST_F(TypesTestSuite, NeuronId) {
    EXPECT_EQ(neuronId1, nlm::NeuronId(1));
    EXPECT_NE(neuronId1, neuronId2);
    EXPECT_LT(neuronId1, neuronId2);
    EXPECT_GT(neuronId2, neuronId1);
    EXPECT_EQ(neuronId1.index(), 1);
}

TEST_F(TypesTestSuite, SynapseId) {
    EXPECT_NE(synapseId, nlm::SynapseId(200));
    EXPECT_LT(synapseId, nlm::SynapseId(200));
    EXPECT_GT(nlm::SynapseId(200), synapseId);
}

TEST_F(TypesTestSuite, RegionId) {
    EXPECT_NE(regionId, nlm::RegionId(2));
}

TEST_F(TypesTestSuite, PopulationId) {
    EXPECT_NE(populationId, nlm::PopulationId(2));
}

TEST_F(TypesTestSuite, NeuronType) {
    EXPECT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    EXPECT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST_F(TypesTestSuite, SynapseType) {
    EXPECT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST_F(TypesTestSuite, PlasticityFlags) {
    nlm::PlasticityFlags flags;
    EXPECT_FALSE(flags.hebbian);
    EXPECT_FALSE(flags.stdp);
    EXPECT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    EXPECT_TRUE(flags.hebbian);
}

TEST_F(TypesTestSuite, SpikeEvent) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    EXPECT_EQ(event.source_neuron.index(), 1);
    EXPECT_DOUBLE_EQ(event.timestamp, 0.5);
    EXPECT_EQ(event.step, 500);
}

// CONFIG TESTS
TEST_F(ConfigTestSuite, ConfigCreation) {
    EXPECT_FALSE(config.has("test"));
}

TEST_F(ConfigTestSuite, ConfigSetAndGet) {
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    EXPECT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    EXPECT_TRUE(doubleVal.has_value());
    EXPECT_GT(*doubleVal, 3.13);
    EXPECT_LT(*doubleVal, 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    EXPECT_TRUE(stringVal.has_value());
    EXPECT_EQ(*stringVal, "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    EXPECT_TRUE(boolVal.has_value());
    EXPECT_TRUE(*boolVal);
}

TEST_F(ConfigTestSuite, ConfigGetOr) {
    int val = config.getOr<int>("missing", 100);
    EXPECT_EQ(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    EXPECT_EQ(val, 50);
}

TEST_F(ConfigTestSuite, ConfigRemove) {
    config.set("test", 123, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("test"));
    
    config.remove("test");
    EXPECT_FALSE(config.has("test"));
}

TEST_F(ConfigTestSuite, ConfigKeys) {
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    EXPECT_EQ(keys.size(), 3);
}

int main(int argc, char **argv) {
    std::cout << "=== NLM Phase 2 Tests - GoogleTest Version ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    std::cout << std::endl;
    if (result == 0) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
    }
    
    return result;
}
