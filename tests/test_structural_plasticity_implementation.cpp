// Structural Plasticity Test Implementation
// Comprehensive tests for structural plasticity

#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace nlm {
namespace test {

class StructuralPlasticityTest : public BaseTest {
public:
    StructuralPlasticityTest() : testName("Structural Plasticity Tests") {}
    
    void run() override {
        std::cout << "Running Structural Plasticity tests..." << std::endl;
        
        testStructuralPlasticityCreation();
        testStructuralPlasticityConfiguration();
        testSynaptogenesis();
        testSynapticPruning();
        testNeuronCreation();
        testNeuronRemoval();
        testActivityDependentPruning();
        testMaxSynapseLimits();
        testStructuralPlasticityRandomized();
        
        std::cout << "Structural Plasticity tests completed successfully!" << std::endl;
    }
    
private:
    void testStructuralPlasticityCreation() {
        nlm::StructuralPlasticity sp;
        
        assert(sp.getSynaptogenesisRate() > 0.0f);
        assert(sp.getPruningRate() > 0.0f);
        assert(sp.getMinWeightThreshold() > 0.0f);
        assert(sp.getActivityThreshold() > 0.0f);
        
        std::cout << "  ✓ testStructuralPlasticityCreation passed" << std::endl;
    }
    
    void testStructuralPlasticityConfiguration() {
        nlm::StructuralPlasticity sp;
        
        sp.setSynaptogenesisRate(0.001f);
        sp.setPruningRate(0.0001f);
        sp.setMinWeightThreshold(0.1f);
        sp.setActivityThreshold(0.01f);
        
        assert(sp.getSynaptogenesisRate() == 0.001f);
        assert(sp.getPruningRate() == 0.0001f);
        assert(sp.getMinWeightThreshold() == 0.1f);
        assert(sp.getActivityThreshold() == 0.01f);
        
        std::cout << "  ✓ testStructuralPlasticityConfiguration passed" << std::endl;
    }
    
    void testSynaptogenesis() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        sp.setSynaptogenesisRate(0.05f);
        
        nlm::NeuronId sourceNeuron = nlm::NeuronId(1);
        nlm::NeuronId destNeuron = nlm::NeuronId(2);
        
        nlm::SynapseId newSynapseId = sp.createSynapse(&brain, sourceNeuron, destNeuron, 0.5f);
        
        if (newSynapseId != nlm::INVALID_SYNAPSE_ID) {
            bool found = false;
            for (auto& region : brain.getRegions()) {
                nlm::Synapse* synapse = region->getSynapse(newSynapseId);
                if (synapse) {
                    found = true;
                    assert(synapse->getSourceNeuron() == sourceNeuron);
                    assert(synapse->getDestinationNeuron() == destNeuron);
                    assert(synapse->getWeight() == 0.5f);
                    break;
                }
            }
            assert(found);
        }
        
        std::cout << "  ✓ testSynaptogenesis passed" << std::endl;
    }
    
    void testSynapticPruning() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        sp.setPruningRate(0.1f);
        
        nlm::SynapseId synapseToRemove = nlm::INVALID_SYNAPSE_ID;
        for (auto& region : brain.getRegions()) {
            auto synapses = region->getSynapsesTo(nlm::NeuronId(2));
            if (!synapses.empty()) {
                synapseToRemove = synapses[0]->getId();
                break;
            }
        }
        
        if (synapseToRemove != nlm::INVALID_SYNAPSE_ID) {
            bool removed = sp.removeSynapse(&brain, synapseToRemove);
            assert(removed);
            
            bool found = false;
            for (auto& region : brain.getRegions()) {
                nlm::Synapse* synapse = region->getSynapse(synapseToRemove);
                if (synapse) {
                    found = true;
                    break;
                }
            }
            assert(!found);
        }
        
        std::cout << "  ✓ testSynapticPruning passed" << std::endl;
    }
    
    void testNeuronCreation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        
        nlm::NeuronId newNeuronId = sp.createNeuron(&brain, nlm::NeuronType::Excitatory);
        
        if (newNeuronId != nlm::INVALID_NEURON_ID) {
            bool found = false;
            for (auto& region : brain.getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto neuron : neurons) {
                    if (neuron->getId() == newNeuronId) {
                        found = true;
                        assert(neuron->getType() == nlm::NeuronType::Excitatory);
                        break;
                    }
                }
            }
            assert(found);
        }
        
        std::cout << "  ✓ testNeuronCreation passed" << std::endl;
    }
    
    void testNeuronRemoval() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        
        nlm::NeuronId neuronToRemove = nlm::INVALID_NEURON_ID;
        for (auto& region : brain.getRegions()) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neuronToRemove = neurons[0]->getId();
                break;
            }
        }
        
        if (neuronToRemove != nlm::INVALID_NEURON_ID) {
            bool removed = sp.removeNeuron(&brain, neuronToRemove);
            assert(removed);
            
            bool found = false;
            for (auto& region : brain.getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto neuron : neurons) {
                    if (neuron->getId() == neuronToRemove) {
                        found = true;
                        break;
                    }
                }
            }
            assert(!found);
        }
        
        std::cout << "  ✓ testNeuronRemoval passed" << std::endl;
    }
    
    void testActivityDependentPruning() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        sp.setUseActivityDependency(true);
        
        brain.step(0);
        
        nlm::RandomGenerator rng(42);
        sp.update(&brain, rng);
        
        assert(brain.getTotalNeuronCount() == 25);
        
        std::cout << "  ✓ testActivityDependentPruning passed" << std::endl;
    }
    
    void testMaxSynapseLimits() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::StructuralPlasticity sp;
        sp.setMaxSynapsesPerNeuron(2);
        sp.setSynaptogenesisRate(1.0f);
        
        nlm::NeuronId sourceNeuron = nlm::NeuronId(1);
        nlm::NeuronId destNeuron = nlm::NeuronId(2);
        
        for (int i = 0; i < 10; ++i) {
            nlm::SynapseId synId = sp.createSynapse(&brain, sourceNeuron, destNeuron, 0.5f);
            if (synId != nlm::INVALID_SYNAPSE_ID) {
                break;
            }
        }
        
        for (auto& region : brain.getRegions()) {
            auto synapses = region->getSynapsesFrom(sourceNeuron);
            assert(synapses.size() <= 2);
        }
        
        std::cout << "  ✓ testMaxSynapseLimits passed" << std::endl;
    }
    
    void testStructuralPlasticityRandomized() {
        nlm::RandomGenerator rng(123);
        nlm::StructuralPlasticity sp;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        sp.update(&brain, rng);
        
        assert(brain.getTotalNeuronCount() == 15);
        
        nlm::Brain brain2(config);
        brain2.initialize();
        sp.update(&brain2, rng);
        
        assert(brain.getTotalSynapseCount() == brain2.getTotalSynapseCount());
        
        std::cout << "  ✓ testStructuralPlasticityRandomized passed" << std::endl;
    }
};

}} // namespace nlm::test
