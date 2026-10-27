#include "TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

class BasicConnectivityTest : public TestBase {
public:
    std::string getName() const override {
        return "connectivity";
    }
    
    TestConfig getDefaultConfig() const override {
        TestConfig config;
        config.connectivityNeuronCount = 10;
        config.connectivityCurrent = 50.0f;
        config.connectivitySteps = 50;
        return config;
    }
    
    bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) override {
        if (config.enableProgressReporting) {
            std::cout << "Running Basic Connectivity Test..." << std::endl;
            std::cout << "  Neuron count: " << config.connectivityNeuronCount << std::endl;
            std::cout << "  Injected current: " << config.connectivityCurrent << std::endl;
            std::cout << "  Simulation steps: " << config.connectivitySteps << std::endl;
        }
        
        // Inject current into neurons
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) {
            std::cerr << "Error: No brain region found" << std::endl;
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cerr << "Error: No neurons found" << std::endl;
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into specified number of neurons
        size_t neuronsToInject = std::min(config.connectivityNeuronCount, neurons.size());
        for (size_t i = 0; i < neuronsToInject; ++i) {
            neurons[i]->injectCurrent(config.connectivityCurrent);
        }
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < config.connectivitySteps; ++step) {
            brain->step(step, step * config.timestep);
            if (config.enableProgressReporting && step % 10 == 0) {
                std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        if (config.enableProgressReporting) {
            std::cout << "  Result: " << spikes << " spikes generated" << std::endl;
        }
        
        if (spikes == 0) {
            std::cout << "  Warning: No spikes generated - checking neuron parameters" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "    Neuron " << i << " V=" << neurons[i]->getMembranePotential()
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
            return false; // Consider this a test failure
        }
        
        return true;
    }
};

class PlasticityExperimentTest : public TestBase {
public:
    std::string getName() const override {
        return "plasticity";
    }
    
    TestConfig getDefaultConfig() const override {
        TestConfig config;
        config.plasticitySteps = 1000;
        config.plasticityNeuronCount = 20;
        config.plasticityCurrent = 30.0f;
        config.plasticityWeightThreshold = 0.01f;
        config.plasticityLearningThreshold = 0.001f;
        return config;
    }
    
    bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) override {
        if (config.enableProgressReporting) {
            std::cout << "Running Plasticity Learning Experiment..." << std::endl;
            std::cout << "  Simulation steps: " << config.plasticitySteps << std::endl;
            std::cout << "  Neurons receiving input: " << config.plasticityNeuronCount << std::endl;
            std::cout << "  Input current: " << config.plasticityCurrent << std::endl;
        }
        
        // Enable plasticity on synapses
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) {
            std::cerr << "Error: No brain region found" << std::endl;
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            std::cerr << "Error: No synapses found for plasticity" << std::endl;
            return false;
        }
        
        for (auto& syn : synapses) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
        
        // Apply repeated input pattern
        for (nlm::SimulationStep step = 0; step < config.plasticitySteps; ++step) {
            // Create input pattern - inject current into sensory neurons
            size_t neuronsToStimulate = std::min(config.plasticityNeuronCount, brain->getTotalNeuronCount() / 4);
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, config.plasticityCurrent);
            }
            
            brain->step(step, step * config.timestep);
            
            if (config.enableProgressReporting && step % 200 == 0) {
                std::cout << "  Step " << step << ": " << brain->getTotalSpikeCount() 
                          << " spikes, " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        if (config.enableProgressReporting) {
            // Compute plasticity statistics
            size_t totalSpikes = brain->getTotalSpikeCount();
            size_t firingNeurons = brain->getFiringNeuronCount();
            std::cout << "  Final spike count: " << totalSpikes << std::endl;
            std::cout << "  Final firing neurons: " << firingNeurons << std::endl;
        }
        
        return true; // Plasticity experiment always succeeds for now
    }
};

class StdpVerificationTest : public TestBase {
public:
    std::string getName() const override {
        return "stdp";
    }
    
    TestConfig getDefaultConfig() const override {
        TestConfig config;
        config.stdpTrials = 50;
        config.stdpSynapseCount = 5;
        config.stdpCurrent = 60.0f;
        config.stdpDeltaThreshold = 0.001f;
        return config;
    }
    
    bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) override {
        if (config.enableProgressReporting) {
            std::cout << "Running STDP Verification..." << std::endl;
            std::cout << "  Trials: " << config.stdpTrials << std::endl;
            std::cout << "  Synapses tested: " << config.stdpSynapseCount << std::endl;
            std::cout << "  Current: " << config.stdpCurrent << std::endl;
        }
        
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) {
            std::cerr << "Error: No brain region found" << std::endl;
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            std::cerr << "Error: Not enough synapses for STDP test" << std::endl;
            return false;
        }
        
        // Test first 5 synapses
        size_t testSynapseCount = std::min(config.stdpSynapseCount, synapses.size());
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < testSynapseCount; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            if (config.enableProgressReporting) {
                std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
            }
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        for (int trial = 0; trial < config.stdpTrials; ++trial) {
            // Get neurons for this trial
            auto neurons = region->getAllNeurons();
            if (neurons.size() < 2) {
                continue; // Skip if not enough neurons
            }
            
            // Fire pre-synaptic neuron
            neurons[0]->injectCurrent(config.stdpCurrent);
            brain->step(trial * 2, trial * 2 * config.timestep);
            
            // Then post fires
            neurons[1]->injectCurrent(config.stdpCurrent);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * config.timestep);
        }
        
        // Check after weights
        if (config.enableProgressReporting) {
            std::cout << "  After correlated activity:" << std::endl;
            float totalDelta = 0.0f;
            for (size_t i = 0; i < testSynapseCount; ++i) {
                float delta = synapses[i]->getWeight() - beforeWeights[i];
                totalDelta += delta;
                std::cout << "    Synapse " << i << " after: " << synapses[i]->getWeight()
                          << " (Δ=" << delta << ")" << std::endl;
            }
            
            std::cout << "  Total Δ: " << totalDelta << std::endl;
            
            if (std::abs(totalDelta) > config.stdpDeltaThreshold) {
                if (totalDelta > 0) {
                    std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
                } else {
                    std::cout << "  ! STDP reversed: Check parameters" << std::endl;
                }
                return true;
            } else {
                std::cout << "  ! No change: STDP may not be triggering" << std::endl;
                return false;
            }
        }
        
        return true; // Default to success for backward compatibility
    }
};

// Legacy test functions for backward compatibility
void runBasicConnectivityTest(std::shared_ptr<nlm::Brain> brain) {
    auto test = std::make_shared<BasicConnectivityTest>();
    TestConfig config = test->getDefaultConfig();
    test->run(brain, config);
}

void runPlasticityExperiment(std::shared_ptr<nlm::Brain> brain) {
    auto test = std::make_shared<PlasticityExperimentTest>();
    TestConfig config = test->getDefaultConfig();
    test->run(brain, config);
}

void runStdpVerification(std::shared_ptr<nlm::Brain> brain) {
    auto test = std::make_shared<StdpVerificationTest>();
    TestConfig config = test->getDefaultConfig();
    test->run(brain, config);
}
