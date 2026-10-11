#include "AdvancedNeuronModels.hpp"
#include "HodgkinHuxleyNeuron.hpp"
#include "IzhikevichNeuron.hpp"
#include "AdaptiveExponentialNeuron.hpp"
#include "ThetaNeuron.hpp"
#include "MorrisLecarNeuron.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <any>

namespace nlm {

std::unique_ptr<AdvancedNeuron> createAdvancedNeuron(
    AdvancedNeuronModelType modelType,
    NeuronId id,
    const std::any& parameters,
    const NeuronState& initialState) {
    
    switch (modelType) {
        case AdvancedNeuronModelType::LIF:
            // LIF is the base neuron - could create using existing Neuron class
            // For now, we'll return nullptr as LIF is handled by the base Neuron class
            return nullptr;
            
        case AdvancedNeuronModelType::HodgkinHuxley:
            try {
                const HodgkinHuxleyParameters& hhParams = std::any_cast< HodgkinHuxleyParameters>(parameters);
                auto neuron = std::make_unique<HodgkinHuxleyNeuron>(id, hhParams);
                
                // Copy initial state if available
                if (initialState.membranePotential != -70.0f) {
                    // Note: HH neurons don't use the same state structure directly
                    // but we could set equivalent values if needed
                }
                
                return neuron;
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
            
        case AdvancedNeuronModelType::Izhikevich:
            try {
                const IzhikevichParameters& izhParams = std::any_cast<IzhikevichParameters>(parameters);
                auto neuron = std::make_unique<IzhikevichNeuron>(id, izhParams);
                
                // Copy initial state
                if (initialState.membranePotential != -70.0f) {
                    // Note: Izhikevich neurons have their own state representation
                }
                
                return neuron;
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
            
        case AdvancedNeuronModelType::AdaptiveExponential:
            try {
                const AdaptiveExponentialParameters& adaptParams = std::any_cast<AdaptiveExponentialParameters>(parameters);
                auto neuron = std::make_unique<AdaptiveExponentialNeuron>(id, adaptParams);
                return neuron;
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
            
        case AdvancedNeuronModelType::Theta:
            try {
                const ThetaNeuronParameters& thetaParams = std::any_cast<ThetaNeuronParameters>(parameters);
                auto neuron = std::make_unique<ThetaNeuron>(id, thetaParams);
                return neuron;
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
            
        case AdvancedNeuronModelType::MorrisLecar:
            try {
                const MorrisLecarParameters& mlParams = std::any_cast<MorrisLecarParameters>(parameters);
                auto neuron = std::make_unique<MorrisLecarNeuron>(id, mlParams);
                return neuron;
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
            
        case AdvancedNeuronModelType::MultipleUnit:
            // MultipleUnit is a special case for coupled neurons
            // Could create a population of neurons
            return nullptr;
            
        default:
            return nullptr;
    }
}

} // namespace nlm
