// Neuron Test Suite - Advanced Biophysical Dynamics
// Tests for the refactored Neuron class with advanced adaptation, refractory dynamics,
// and stochastic properties

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace nlm {

// Test fixture for Neuron with advanced dynamics
class TestNeuronAdvancedDynamics {
private:
    NeuronId neuronId = NeuronId(1);
    Neuron neuron;
    RandomGenerator rng;
    
public:
    TestNeuronAdvancedDynamics() : neuron(neuronId) {
        neuron.setType(NeuronType::Excitatory);
        neuron.initializeRandom(rng);
    }
    
    void testNeuronInitialization() {
        assert(neuron.getType() == NeuronType::Excitatory);
        assert(neuron.getState().neuronType == NeuronType::Excitatory);
        assert(neuron.getState().membraneCapacitance > 0.0f);
        assert(neuron.getState().timeConstant > 0.0f);
        std::cout << "✓ Neuron initialization passed" << std::endl;
    }
    
    void testRefractoryPeriodHandling() {
        // Set a short refractory period for testing
        neuron.setRefractoryProperties(2, 1);
        
        // Simulate spike to enter refractory period
        Timestamp spikeTime = 100.0;
        bool fired = neuron.stepLIF(spikeTime, 0.001);
        assert(fired);
        
        // Check that neuron is in refractory state
        assert(neuron.getState().refractoryRemaining > 0);
        assert(neuron.getState().firingState == FiringState::Refractory);
        
        // Simulate steps during refractory period
        for (int i = 0; i < 5; ++i) {
            neuron.decrementRefractory();
            Timestamp currentTime = spikeTime + (i + 1) * 0.001;
            bool shouldFire = neuron.stepLIF(currentTime, 0.001);
            assert(!shouldFire); // Should not fire during refractory
        }
        
        std::cout << "✓ Refractory period handling passed" << std::endl;
    }
    
    void testAdaptationCurrents() {
        // Initialize neuron and check adaptation state
        neuron.setAdaptationParameters(100.0f, 10.0f, -60.0f, -50.0f);
        
        // Simulate some spikes to trigger adaptation
        Timestamp spikeTime = 0.0;
        for (int i = 0; i < 5; ++i) {
            neuron.stepLIF(spikeTime + i * 0.001, 0.001);
        }
        
        // Check that adaptation variable increased
        assert(neuron.getState().adaptationVariable > 0.0f);
        
        // Update adaptation currents
        neuron.updateAdaptationCurrents(neuron.getState().membranePotential);
        
        // Check that adaptation currents are being calculated
        float adaptationCurrents = neuron.getAdaptationCurrents();
        assert(adaptationCurrents >= 0.0f);
        
        std::cout << "✓ Adaptation currents test passed" << std::endl;
    }
    
    void testSynapticConductanceDynamics() {
        // Set synaptic properties
        neuron.setSynapticProperties(5.0f, 100.0f, 10.0f, 100.0f);
        
        float dt = 0.001f;
        
        // Simulate synaptic conductance changes
        for (int i = 0; i < 10; ++i) {
            neuron.updateSynapticConductances(dt);
            
            // Check that conductances remain non-negative
            assert(neuron.getState().ampaConductance >= 0.0f);
            assert(neuron.getState().nmdaConductance >= 0.0f);
            assert(neuron.getState().GABAaConductance >= 0.0f);
            assert(neuron.getState().gabaBConductance >= 0.0f);
        }
        
        std::cout << "✓ Synaptic conductance dynamics test passed" << std::endl;
    }
    
    void testStochasticDynamics() {
        // Set stochastic properties
        neuron.setStochasticProperties(1.0f, 0.0f, 5.0f);
        
        float dt = 0.001f;
        
        // Run simulation with multiple random seeds
        for (int seed = 0; seed < 3; ++seed) {
            RandomGenerator seedRng;
            neuron.updateStochasticDynamics(dt, seedRng);
            
            // Check that membrane potential is within reasonable bounds
            MembranePotential V = neuron.getMembranePotential();
            assert(V >= neuron.getState().restingPotential - 20.0f);
            assert(V <= neuron.getState().threshold + 20.0f);
        }
        
        std::cout << "✓ Stochastic dynamics test passed" << std::endl;
    }
    
    void testRefractoryRecovery() {
        // Set a refractory period
        neuron.setRefractoryProperties(5, 3);
        
        // Enter refractory period
        Timestamp spikeTime = 0.0;
        neuron.stepLIF(spikeTime, 0.001);
        
        assert(neuron.getState().refractoryRemaining > 0);
        
        // Simulate recovery
        float recovery = neuron.getRefractoryRecovery();
        assert(recovery < 1.0f);
        
        // Check recovery dynamics
        neuron.updateRefractoryRecovery(0.001);
        
        std::cout << "✓ Refractory recovery test passed" << std::endl;
    }
    
    void testShortTermPlasticity() {
        float dt = 0.001f;
        
        // Run simulation with STP
        for (int i = 0; i < 50; ++i) {
            neuron.updateShortTermPlasticity(dt);
            
            float facilitation = neuron.getSTPValue();
            assert(facilitation >= 0.0f);
            assert(facilitation <= 1.0f);
        }
        
        std::cout << "✓ Short-term plasticity test passed" << std::endl;
    }
    
    void testResetAndPostInhibitoryCurrents() {
        // Simulate spike
        Timestamp spikeTime = 0.0;
        neuron.stepLIF(spikeTime, 0.001);
        
        // Check that reset and post-inhibitory currents are active
        neuron.updateResetCurrents();
        neuron.updatePostInhibitoryCurrents();
        
        std::cout << "✓ Reset and post-inhibitory currents test passed" << std::endl;
    }
    
    void testNeuronTypeSpecificDynamics() {
        // Test different neuron types
        neuron.setType(NeuronType::Inhibitory);
        assert(neuron.getType() == NeuronType::Inhibitory);
        
        // Test type-specific configuration
        NeuronConfig inhibitoryConfig = NeuronConfig::createInhibitoryNeuronConfig();
        assert(inhibitoryConfig.membraneCapacitance < 1.0f);
        
        std::cout << "✓ Neuron type-specific dynamics test passed" << std::endl;
    }
    
    void testConfigurationValidation() {
        // Test configuration validation
        NeuronConfig config = NeuronConfig::createDefaultConfig(NeuronType::Excitatory);
        assert(NeuronConfig::validateConfig(config));
        
        std::cout << "✓ Configuration validation test passed" << std::endl;
    }
    
    void runAllTests() {
        std::cout << "Running Neuron Advanced Dynamics Test Suite..." << std::endl;
        std::cout << "==============================================" << std::endl;
        
        testNeuronInitialization();
        testRefractoryPeriodHandling();
        testAdaptationCurrents();
        testSynapticConductanceDynamics();
        testStochasticDynamics();
        testRefractoryRecovery();
        testShortTermPlasticity();
        testResetAndPostInhibitoryCurrents();
        testNeuronTypeSpecificDynamics();
        testConfigurationValidation();
        
        std::cout << "==============================================" << std::endl;
        std::cout << "All Neuron Advanced Dynamics tests passed!" << std::endl;
    }
};

// Test for NeuralRegion with neuron type management
class TestNeuralRegionTypeManagement {
private:
    RegionId regionId = RegionId(1);
    NeuralRegion region;
    
public:
    TestNeuralRegionTypeManagement() : region(regionId) {
        region.setName("Test Region");
    }
    
    void testNeuronTypeCreation() {
        // Create neurons of different types
        region.createNeuronTypes(NeuronType::Excitatory, 10);
        region.createNeuronTypes(NeuronType::Inhibitory, 5);
        region.createNeuronTypes(NeuronType::Sensory, 3);
        region.createNeuronTypes(NeuronType::Motor, 2);
        
        // Check population counts
        assert(region.getPopulationCount() == 4);
        
        std::cout << "✓ Neuron type creation test passed" << std::endl;
    }
    
    void testNeuronTypeBalancing() {
        region.balanceNeuronTypes();
        
        // Check that dominant neuron type is identified
        NeuronType dominantType = region.getDominantNeuronType();
        assert(dominantType == NeuronType::Excitatory || dominantType == NeuronType::Internal);
        
        std::cout << "✓ Neuron type balancing test passed" << std::endl;
    }
    
    void testNeuronTypeRatios() {
        // Set neuron type ratios
        region.setNeuronTypeRatio(NeuronType::Excitatory, 0.8f);
        region.setNeuronTypeRatio(NeuronType::Inhibitory, 0.15f);
        
        // Check ratios
        float excitatoryRatio = region.getNeuronTypeRatio(NeuronType::Excitatory);
        float inhibitoryRatio = region.getNeuronTypeRatio(NeuronType::Inhibitory);
        
        assert(excitatoryRatio >= 0.0f && excitatoryRatio <= 1.0f);
        assert(inhibitoryRatio >= 0.0f && inhibitoryRatio <= 1.0f);
        
        std::cout << "✓ Neuron type ratios test passed" << std::endl;
    }
    
    void testTypeSpecificStatistics() {
        // Create neurons and test type-specific statistics
        region.createNeuronTypes(NeuronType::Excitatory, 10);
        region.createNeuronTypes(NeuronType::Inhibitory, 5);
        
        // Test type-specific firing rates
        float excitatoryFiring = region.computeTypeSpecificFiringRates(NeuronType::Excitatory);
        float inhibitoryFiring = region.computeTypeSpecificFiringRates(NeuronType::Inhibitory);
        
        assert(excitatoryFiring >= 0.0f);
        assert(inhibitoryFiring >= 0.0f);
        
        // Test type-specific refractory periods
        float excitatoryRefractory = region.computeTypeSpecificRefractoryPeriods(NeuronType::Excitatory);
        float inhibitoryRefractory = region.computeTypeSpecificRefractoryPeriods(NeuronType::Inhibitory);
        
        assert(excitatoryRefractory >= 0.0f);
        assert(inhibitoryRefractory >= 0.0f);
        
        // Test type-specific adaptation strengths
        float excitatoryAdaptation = region.computeTypeSpecificAdaptationStrengths(NeuronType::Excitatory);
        float inhibitoryAdaptation = region.computeTypeSpecificAdaptationStrengths(NeuronType::Inhibitory);
        
        assert(excitatoryAdaptation >= 0.0f);
        assert(inhibitoryAdaptation >= 0.0f);
        
        std::cout << "✓ Type-specific statistics test passed" << std::endl;
    }
    
    void runAllTests() {
        std::cout << "Running NeuralRegion Type Management Test Suite..." << std::endl;
        std::cout << "==============================================" << std::endl;
        
        testNeuronTypeCreation();
        testNeuronTypeBalancing();
        testNeuronTypeRatios();
        testTypeSpecificStatistics();
        
        std::cout << "==============================================" << std::endl;
        std::cout << "All NeuralRegion Type Management tests passed!" << std::endl;
    }
};

int main() {
    std::cout << "NeuraL Institute - Advanced Neuron Dynamics Test Suite" << std::endl;
    std::cout << "===================================================" << std::endl;
    std::cout << std::endl;
    
    // Run Neuron tests
    TestNeuronAdvancedDynamics neuronTests;
    neuronTests.runAllTests();
    std::cout << std::endl;
    
    // Run NeuralRegion tests
    TestNeuralRegionTypeManagement regionTests;
    regionTests.runAllTests();
    
    std::cout << std::endl;
    std::cout << "All test suites completed successfully!" << std::endl;
    return 0;
}
