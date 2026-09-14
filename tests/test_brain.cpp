// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "TestFramework.h"
#include <cassert>
#include <iostream>

namespace test_brain {

class TestBrainCreation : public TestBase {
public:
    std::string getName() const override {
        return "testBrainCreation";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainInitialization : public TestBase {
public:
    std::string getName() const override {
        return "testBrainInitialization";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        bool success = brain.initialize();
        assert(success);
        
        // Verify brain initialized with expected properties
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainStep : public TestBase {
public:
    std::string getName() const override {
        return "testBrainStep";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Just verify it doesn't crash
        assert(brain.getTotalNeuronCount() == 100);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainReset : public TestBase {
public:
    std::string getName() const override {
        return "testBrainReset";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        brain.step(0);
        brain.reset();
        
        // Verify reset worked
        assert(brain.getTotalNeuronCount() == 50);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainRegions : public TestBase {
public:
    std::string getName() const override {
        return "testBrainRegions";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 3);
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            assert(region != nullptr);
        }
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainActionProduction : public TestBase {
public:
    std::string getName() const override {
        return "testBrainActionProduction";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBrainStatus : public TestBase {
public:
    std::string getName() const override {
        return "testBrainStatus";
    }
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify statistics are accessible
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

// Function to register all brain tests
void registerBrainTests(TestRunner& runner) {
    runner.registerTest<TestBrainCreation>();
    runner.registerTest<TestBrainInitialization>();
    runner.registerTest<TestBrainStep>();
    runner.registerTest<TestBrainReset>();
    runner.registerTest<TestBrainRegions>();
    runner.registerTest<TestBrainActionProduction>();
    runner.registerTest<TestBrainStatus>();
}

// Keep backward compatibility
void runAll() {
    std::cout << "Running Brain tests..." << std::endl;
    TestRunner runner;
    registerBrainTests(runner);
    runner.runAll();
}

} // namespace test_brain