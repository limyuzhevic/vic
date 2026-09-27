// Spike System Test Implementation
// Comprehensive tests for spike propagation mechanisms

#include "brain/SpikeSystem.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace nlm {
namespace test {

class SpikeSystemTest : public BaseTest {
public:
    SpikeSystemTest() : testName("Spike System Tests") {}
    
    void run() override {
        std::cout << "Running Spike System tests..." << std::endl;
        
        testSpikeSystemCreation();
        testSpikePropagation();
        testSpikeTimingValidation();
        testRefractoryPeriodEnforcement();
        testSpikeRecordTracking();
        testSpikeTimingAccuracy();
        testMultiSpikeSynchronization();
        testSpikeSystemReset();
        
        std::cout << "Spike System tests completed successfully!" << std::endl;
    }
    
private:
    void testSpikeSystemCreation() {
        nlm::SpikeSystem spikeSystem;
        
        assert(spikeSystem.getMaxSpikeHistory() > 0);
        assert(spikeSystem.getMaxConcurrentSpikes() > 0);
        assert(spikeSystem.getSpikeThreshold() > 0.0f);
        
        std::cout << "  ✓ testSpikeSystemCreation passed" << std::endl;
    }
    
    void testSpikePropagation() {
        nlm::SpikeSystem spikeSystem;
        
        // Create test neurons
        nlm::Neuron preNeuron(nlm::NeuronId(1));
        nlm::Neuron postNeuron(nlm::NeuronId(2));
        
        preNeuron.setMembranePotential(0.0f);  // Above threshold
        postNeuron.setMembranePotential(-70.0f);
        
        // Simulate spike propagation
        spikeSystem.propagateSpike(&preNeuron, &postNeuron);
        
        assert(postNeuron.isFiring());
        assert(postNeuron.getMembranePotential() > preNeuron.getMembranePotential());
        
        std::cout << "  ✓ testSpikePropagation passed" << std::endl;
    }
    
    void testSpikeTimingValidation() {
        nlm::SpikeSystem spikeSystem;
        
        nlm::Neuron neuron1(nlm::NeuronId(1));
        nlm::Neuron neuron2(nlm::NeuronId(2));
        nlm::Neuron neuron3(nlm::NeuronId(3));
        
        // Set different spike times
        neuron1.setLastSpikeTime(0.0f);
        neuron2.setLastSpikeTime(10.0f);
        neuron3.setLastSpikeTime(20.0f);
        
        // Validate spike timing
        assert(neuron1.getLastSpikeTime() < neuron2.getLastSpikeTime());
        assert(neuron2.getLastSpikeTime() < neuron3.getLastSpikeTime());
        
        std::cout << "  ✓ testSpikeTimingValidation passed" << std::endl;
    }
    
    void testRefractoryPeriodEnforcement() {
        nlm::SpikeSystem spikeSystem;
        
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setMembranePotential(0.0f);
        neuron.setRefractoryPeriod(5);
        
        // Record spike
        neuron.recordSpike(0.0f);
        assert(neuron.isFiring());
        
        // Try to record another spike during refractory period
        neuron.recordSpike(1.0f);
        assert(neuron.isRefractory());
        assert(!neuron.isFiring());
        
        // Wait for refractory period to end (simulated)
        neuron.decrementRefractory();
        neuron.decrementRefractory();
        neuron.decrementRefractory();
        neuron.decrementRefractory();
        neuron.decrementRefractory();  // All decremented
        
        assert(!neuron.isRefractory());
        
        std::cout << "  ✓ testRefractoryPeriodEnforcement passed" << std::endl;
    }
    
    void testSpikeRecordTracking() {
        nlm::SpikeSystem spikeSystem;
        
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getSpikeHistory().empty());
        
        // Record multiple spikes
        neuron.recordSpike(0.0f);
        neuron.recordSpike(10.0f);
        neuron.recordSpike(20.0f);
        
        assert(neuron.getSpikeHistory().size() == 3);
        assert(neuron.getSpikeHistory()[0] == 0.0f);
        assert(neuron.getSpikeHistory()[1] == 10.0f);
        assert(neuron.getSpikeHistory()[2] == 20.0f);
        
        // Clear spike history
        neuron.clearSpikeHistory();
        assert(neuron.getSpikeHistory().empty());
        
        std::cout << "  ✓ testSpikeRecordTracking passed" << std::endl;
    }
    
    void testSpikeTimingAccuracy() {
        nlm::SpikeSystem spikeSystem;
        
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        float expectedTime = 15.5f;
        neuron.recordSpike(expectedTime);
        
        float actualTime = neuron.getLastSpikeTime();
        
        assert(std::abs(actualTime - expectedTime) < 0.001f);
        
        std::cout << "  ✓ testSpikeTimingAccuracy passed" << std::endl;
    }
    
    void testMultiSpikeSynchronization() {
        nlm::SpikeSystem spikeSystem;
        
        std::vector<nlm::Neuron*> neurons;
        for (int i = 0; i < 5; ++i) {
            neurons.push_back(new nlm::Neuron(nlm::NeuronId(i)));
        }
        
        // Synchronize spikes
        for (auto neuron : neurons) {
            neuron->recordSpike(0.0f);
        }
        
        bool allFiring = true;
        for (auto neuron : neurons) {
            if (!neuron->isFiring()) {
                allFiring = false;
                break;
            }
        }
        
        assert(allFiring);
        
        // Cleanup
        for (auto neuron : neurons) {
            delete neuron;
        }
        
        std::cout << "  ✓ testMultiSpikeSynchronization passed" << std::endl;
    }
    
    void testSpikeSystemReset() {
        nlm::SpikeSystem spikeSystem;
        
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.recordSpike(0.0f);
        neuron.recordSpike(10.0f);
        
        spikeSystem.resetSpikeSystem(&neuron);
        
        assert(!neuron.isFiring());
        assert(!neuron.isRefractory());
        assert(neuron.getSpikeHistory().empty());
        
        std::cout << "  ✓ testSpikeSystemReset passed" << std::endl;
    }
};

}} // namespace nlm::test
