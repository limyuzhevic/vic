// NeuronTestSuite.cpp - Neuron test suite implementation

#include "tests/TestRunner.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

class NeuronTestSuite : public TestSuite {
public:
    NeuronTestSuite() : TestSuite("Neuron Tests") {}
    
    void runAllTests() override {
        // Test neuron creation
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                assert(neuron.getId() == NeuronId(1));
                assert(neuron.getType() == NeuronType::Internal);
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testNeuronCreation", "Neuron", true, "Neuron created successfully", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testNeuronCreation", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test neuron type
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                neuron.setType(NeuronType::Excitatory);
                assert(neuron.getType() == NeuronType::Excitatory);
                
                neuron.setType(NeuronType::Inhibitory);
                assert(neuron.getType() == NeuronType::Inhibitory);
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testNeuronType", "Neuron", true, "Neuron type handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testNeuronType", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test membrane potential
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                assert(neuron.getMembranePotential() == -70.0f);
                
                neuron.setMembranePotential(-65.0f);
                assert(neuron.getMembranePotential() == -65.0f);
                
                neuron.addToMembranePotential(5.0f);
                assert(neuron.getMembranePotential() == -60.0f);
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testMembranePotential", "Neuron", true, "Membrane potential handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testMembranePotential", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test firing state
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                assert(!neuron.isFiring());
                assert(!neuron.isRefractory());
                
                neuron.setFiringState(FiringState::Active);
                assert(neuron.isFiring());
                
                neuron.setRefractoryPeriod(10);
                assert(neuron.isRefractory());
                assert(!neuron.isFiring());
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testFiringState", "Neuron", true, "Firing state handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testFiringState", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test refractory decrement
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                neuron.setRefractoryPeriod(5);
                assert(neuron.isRefractory());
                
                neuron.decrementRefractory();
                assert(neuron.isRefractory());
                
                for (int i = 0; i < 4; ++i) {
                    neuron.decrementRefractory();
                }
                assert(!neuron.isRefractory());
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testRefractoryDecrement", "Neuron", true, "Refractory decrement correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testRefractoryDecrement", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test current injection
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                assert(neuron.getTotalCurrent() == 0.0f);
                
                neuron.injectCurrent(5.0f);
                assert(neuron.getTotalCurrent() == 5.0f);
                
                neuron.clearTotalCurrent();
                assert(neuron.getTotalCurrent() == 0.0f);
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testCurrentInjection", "Neuron", true, "Current injection handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testCurrentInjection", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test synaptic input
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                
                neuron.receiveExcitatoryInput(5.0f);
                assert(neuron.getTotalCurrent() == 5.0f);
                
                neuron.clearTotalCurrent();
                neuron.receiveInhibitoryInput(3.0f);
                assert(neuron.getTotalCurrent() == -3.0f);
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testSynapticInput", "Neuron", true, "Synaptic input handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testSynapticInput", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test spike history
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                assert(neuron.getSpikeHistory().empty());
                
                neuron.recordSpike(0.0f);
                assert(!neuron.getSpikeHistory().empty());
                
                neuron.reset();
                assert(neuron.getSpikeHistory().empty());
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testSpikeHistory", "Neuron", true, "Spike history handling correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testSpikeHistory", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test random initialization
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                neuron.initializeRandom();
                
                assert(neuron.getMembranePotential() >= -80.0f && neuron.getMembranePotential() <= -60.0f);
                assert(neuron.getThreshold() >= -50.0f && neuron.getThreshold() <= -30.0f);
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testRandomInitialization", "Neuron", true, "Random initialization correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testRandomInitialization", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
        
        // Test reset
        {
            auto start = std::chrono::high_resolution_clock::now();
            try {
                Neuron neuron(NeuronId(1));
                neuron.injectCurrent(5.0f);
                neuron.recordSpike(0.0f);
                
                neuron.reset();
                
                assert(neuron.getMembranePotential() == -70.0f);
                assert(neuron.getTotalCurrent() == 0.0f);
                assert(neuron.getSpikeHistory().empty());
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testReset", "Neuron", true, "Reset functionality correct", duration));
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                addResult(TestResult("testReset", "Neuron", false, std::string("Exception: ") + e.what(), duration));
            }
        }
    }
};

} // namespace tests
} // namespace nlm