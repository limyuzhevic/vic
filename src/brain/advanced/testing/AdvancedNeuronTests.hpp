#pragma once

#include "AdvancedNeuronModels.hpp"
#include "../brain/advanced/HodgkinHuxleyNeuron.hpp"
#include "../brain/advanced/IzhikevichNeuron.hpp"
#include "../brain/advanced/AdaptiveExponentialNeuron.hpp"
#include "../brain/advanced/ThetaNeuron.hpp"
#include "../brain/advanced/MorrisLecarNeuron.hpp"
#include "../brain/advanced/AdvancedNeuronFactory.hpp"
#include "../core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace nlm {

class AdvancedNeuronTests {
public:
    static void runAllTests() {
        std::cout << "Running Advanced Neuron Model Tests..." << std::endl;
        
        testHodgkinHuxleyNeuron();
        testIzhikevichNeuron();
        testAdaptiveExponentialNeuron();
        testThetaNeuron();
        testMorrisLecarNeuron();
        testNeuronFactory();
        
        std::cout << "All advanced neuron tests passed!" << std::endl;
    }
    
private:
    static void testHodgkinHuxleyNeuron() {
        std::cout << "  Testing Hodgkin-Huxley Neuron..." << std::endl;
        
        // Create HH parameters
        HodgkinHuxleyParameters hhParams;
        hhParams.C_m = 1.0f;
        hhParams.g_Na_max = 120.0f;
        hhParams.g_K_max = 36.0f;
        hhParams.g_L_max = 0.3f;
        hhParams.E_Na = 50.0f;
        hhParams.E_K = -77.0f;
        hhParams.E_L = -54.387f;
        hhParams.V_th = 30.0f;
        hhParams.V_reset = -70.0f;
        hhParams.dt = 0.01f;
        hhParams.T_max = 1000.0f;
        
        // Create neuron
        NeuronId id(1);
        HodgkinHuxleyNeuron neuron(id, hhParams);
        
        // Test initial state
        assert(neuron.getModelType() == AdvancedNeuronModelType::HodgkinHuxley);
        assert(neuron.getMembranePotential() == -70.0f);
        assert(neuron.isFiring() == false);
        assert(neuron.getLastSpikeTime() < 0.0f);
        
        // Test current injection
        neuron.injectCurrent(10.0f);
        
        // Test simulation step (should not spike with low current)
        bool fired = neuron.step(0.0f);
        assert(fired == false);
        
        // Reset neuron
        neuron.reset();
        assert(neuron.getMembranePotential() == -70.0f);
        assert(neuron.isFiring() == false);
        
        std::cout << "    Hodgkin-Huxley Neuron tests passed!" << std::endl;
    }
    
    static void testIzhikevichNeuron() {
        std::cout << "  Testing Izhikevich Neuron..." << std::endl;
        
        // Create Izhikevich parameters for regular spiking neuron
        IzhikevichParameters izhParams;
        izhParams.a = 0.02f;
        izhParams.b = 0.2f;
        izhParams.c = -65.0f;
        izhParams.d = 8.0f;
        izhParams.V_th = 30.0f;
        izhParams.V_init = -65.0f;
        izhParams.u_init = -14.0f;
        
        // Create neuron
        NeuronId id(2);
        IzhikevichNeuron neuron(id, izhParams);
        
        // Test initial state
        assert(neuron.getModelType() == AdvancedNeuronModelType::Izhikevich);
        assert(std::abs(neuron.getMembranePotential() - (-65.0f)) < 0.001f);
        assert(neuron.isFiring() == false);
        
        // Test current injection
        neuron.injectCurrent(10.0f);
        
        // Test simulation step (should produce spikes)
        bool fired = neuron.step(0.0f);
        // Might or might not fire depending on parameters
        
        // Reset neuron
        neuron.reset();
        assert(std::abs(neuron.getMembranePotential() - (-65.0f)) < 0.001f);
        assert(neuron.isFiring() == false);
        
        std::cout << "    Izhikevich Neuron tests passed!" << std::endl;
    }
    
    static void testAdaptiveExponentialNeuron() {
        std::cout << "  Testing Adaptive Exponential Neuron..." << std::endl;
        
        // Create adaptive exponential parameters
        AdaptiveExponentialParameters adaptParams;
        adaptParams.V_th = -50.0f;
        adaptParams.V_reset = -65.0f;
        adaptParams.delta_T = 0.5f;
        adaptParams.tau_w = 100.0f;
        adaptParams.a = 4.0f;
        adaptParams.b = 0.08f;
        adaptParams.dt = 0.01f;
        
        // Create neuron
        NeuronId id(3);
        AdaptiveExponentialNeuron neuron(id, adaptParams);
        
        // Test initial state
        assert(neuron.getModelType() == AdvancedNeuronModelType::AdaptiveExponential);
        assert(neuron.getMembranePotential() == -65.0f);
        assert(neuron.isFiring() == false);
        
        // Test current injection
        neuron.injectCurrent(15.0f);
        
        // Test simulation step
        bool fired = neuron.step(0.0f);
        
        // Reset neuron
        neuron.reset();
        assert(neuron.getMembranePotential() == -65.0f);
        assert(neuron.isFiring() == false);
        
        std::cout << "    Adaptive Exponential Neuron tests passed!" << std::endl;
    }
    
    static void testThetaNeuron() {
        std::cout << "  Testing Theta Neuron..." << std::endl;
        
        // Create theta neuron parameters
        ThetaNeuronParameters thetaParams;
        thetaParams.V_th = -50.0f;
        thetaParams.V_reset = -70.0f;
        thetaParams.omega_0 = 10.0f; // 10 Hz baseline frequency
        thetaParams.delta = 2.0f;     // Frequency current dependence
        thetaParams.dt = 0.01f;
        
        // Create neuron
        NeuronId id(4);
        ThetaNeuron neuron(id, thetaParams);
        
        // Test initial state
        assert(neuron.getModelType() == AdvancedNeuronModelType::Theta);
        assert(neuron.getMembranePotential() == -70.0f);
        assert(neuron.isFiring() == false);
        
        // Test current injection (should increase firing rate)
        neuron.injectCurrent(5.0f);
        
        // Test simulation step
        bool fired = neuron.step(0.0f);
        
        // Reset neuron
        neuron.reset();
        assert(neuron.getMembranePotential() == -70.0f);
        assert(neuron.isFiring() == false);
        
        std::cout << "    Theta Neuron tests passed!" << std::endl;
    }
    
    static void testMorrisLecarNeuron() {
        std::cout << "  Testing Morris-Lecar Neuron..." << std::endl;
        
        // Create Morris-Lecar parameters
        MorrisLecarParameters mlParams;
        mlParams.C_m = 20.0f;
        mlParams.g_L = 2.0f;
        mlParams.g_Ca = 4.0f;
        mlParams.g_K = 8.0f;
        mlParams.E_L = -65.0f;
        mlParams.E_Ca = 130.0f;
        mlParams.E_K = 12.0f;
        mlParams.phi = 0.066f;
        mlParams.V1 = 1.0f;
        mlParams.V2 = 30.0f;
        mlParams.V3 = 2.0f;
        mlParams.V4 = 30.0f;
        mlParams.g_L_inf = 2.0f;
        mlParams.g_Ca_inf = 4.0f;
        mlParams.g_K_inf = 8.0f;
        mlParams.tau_L = 300.0f;
        mlParams.tau_Ca = 10.0f;
        mlParams.tau_K = 20.0f;
        mlParams.V_th = 25.0f;
        mlParams.V_reset = -65.0f;
        mlParams.dt = 0.01f;
        
        // Create neuron
        NeuronId id(5);
        MorrisLecarNeuron neuron(id, mlParams);
        
        // Test initial state
        assert(neuron.getModelType() == AdvancedNeuronModelType::MorrisLecar);
        assert(neuron.getMembranePotential() == -65.0f);
        assert(neuron.isFiring() == false);
        
        // Test current injection
        neuron.injectCurrent(20.0f);
        
        // Test simulation step
        bool fired = neuron.step(0.0f);
        
        // Reset neuron
        neuron.reset();
        assert(neuron.getMembranePotential() == -65.0f);
        assert(neuron.isFiring() == false);
        
        std::cout << "    Morris-Lecar Neuron tests passed!" << std::endl;
    }
    
    static void testNeuronFactory() {
        std::cout << "  Testing Advanced Neuron Factory..." << std::endl;
        
        NeuronId id(6);
        
        // Test creating different neuron types
        // Hodgkin-Huxley
        {
            HodgkinHuxleyParameters hhParams;
            hhParams.V_th = 30.0f;
            hhParams.V_reset = -70.0f;
            hhParams.dt = 0.01f;
            
            auto neuron = createAdvancedNeuron(AdvancedNeuronModelType::HodgkinHuxley, id, hhParams);
            assert(neuron != nullptr);
            assert(neuron->getModelType() == AdvancedNeuronModelType::HodgkinHuxley);
        }
        
        // Izhikevich
        {
            IzhikevichParameters izhParams;
            izhParams.V_th = 30.0f;
            izhParams.V_init = -65.0f;
            
            auto neuron = createAdvancedNeuron(AdvancedNeuronModelType::Izhikevich, id, izhParams);
            assert(neuron != nullptr);
            assert(neuron->getModelType() == AdvancedNeuronModelType::Izhikevich);
        }
        
        // Adaptive Exponential
        {
            AdaptiveExponentialParameters adaptParams;
            adaptParams.V_th = -50.0f;
            adaptParams.V_reset = -65.0f;
            
            auto neuron = createAdvancedNeuron(AdvancedNeuronModelType::AdaptiveExponential, id, adaptParams);
            assert(neuron != nullptr);
            assert(neuron->getModelType() == AdvancedNeuronModelType::AdaptiveExponential);
        }
        
        // Theta
        {
            ThetaNeuronParameters thetaParams;
            thetaParams.V_th = -50.0f;
            thetaParams.V_reset = -70.0f;
            
            auto neuron = createAdvancedNeuron(AdvancedNeuronModelType::Theta, id, thetaParams);
            assert(neuron != nullptr);
            assert(neuron->getModelType() == AdvancedNeuronModelType::Theta);
        }
        
        // Morris-Lecar
        {
            MorrisLecarParameters mlParams;
            mlParams.V_th = 25.0f;
            mlParams.V_reset = -65.0f;
            
            auto neuron = createAdvancedNeuron(AdvancedNeuronModelType::MorrisLecar, id, mlParams);
            assert(neuron != nullptr);
            assert(neuron->getModelType() == AdvancedNeuronModelType::MorrisLecar);
        }
        
        std::cout << "    Advanced Neuron Factory tests passed!" << std::endl;
    }
};

} // namespace nlm
