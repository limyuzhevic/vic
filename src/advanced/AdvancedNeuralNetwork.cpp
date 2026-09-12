#include "AdvancedNeuralNetwork.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

struct AdvancedNeuralNetwork::Impl {
    // Network layers
    std::vector<Layer> layers;
    
    // Neurons organized by layer
    std::vector<std::vector<Neuron*>> neuronsByLayer;
    
    // Synapses organized by source layer
    std::vector<std::vector<std::vector<Synapse*>>> synapsesByLayer;
    
    // External connections
    std::unordered_map<NeuronId, std::vector<float>> externalInputs;
    std::unordered_map<NeuronId, std::function<void(std::vector<float>)>> externalOutputs;
    
    // Neuron/synapse factories
    std::unordered_map<std::string, std::unique_ptr<NeuronFactory>> neuronFactories;
    std::unordered_map<std::string, std::unique_ptr<SynapseFactory>> synapseFactories;
    
    // Network state
    std::vector<std::vector<float>> layerOutputs;
    std::vector<float> networkOutput;
    
    // Connectivity patterns
    std::unordered_map<std::string, std::function<bool(size_t, size_t)>> connectivityRules;
    
    Impl() {
        // Initialize connectivity rules
        connectivityRules["random"] = [this](size_t sourceIdx, size_t destIdx) {
            return rng.bernoulli(0.1f);  // Default 10% connection probability
        };
        
        connectivityRules["grid"] = [this](size_t sourceIdx, size_t destIdx) {
            // Simple grid pattern - connect nearby neurons
            size_t sourceLayerSize = layers[sourceIdx].neuronCount;
            size_t destLayerSize = layers[destIdx].neuronCount;
            
            // Map neuron indices to 2D positions
            float sourceX = static_cast<float>(sourceIdx) / layers.size();
            float sourceY = static_cast<float>(sourceIdx % sourceLayerSize) / sourceLayerSize;
            float destX = static_cast<float>(destIdx) / layers.size();
            float destY = static_cast<float>(destIdx % destLayerSize) / destLayerSize;
            
            // Connect if within reasonable distance
            float dist = std::sqrt((sourceX - destX) * (sourceX - destX) +
                                 (sourceY - destY) * (sourceY - destY));
            return dist < 0.5f;
        };
        
        connectivityRules["small_world"] = [this](size_t sourceIdx, size_t destIdx) {
            // Watts-Strogatz small-world model
            // For now, use random connections with small-world properties
            return rng.bernoulli(0.3f);  // Higher connectivity
        };
        
        connectivityRules["scale_free"] = [this](size_t sourceIdx, size_t destIdx) {
            // Barabási-Albert scale-free model
            // For now, use preferential attachment heuristic
            float sourceDegree = getNodeDegree(sourceIdx);
            float destDegree = getNodeDegree(destIdx);
            
            // Higher degree nodes more likely to be connected
            float probability = (sourceDegree + 1.0f) / (sourceDegree + destDegree + layers.size());
            return rng.bernoulli(probability);
        };
    }
    
    std::mt19937 rng{std::random_device{}()};
    
    float getNodeDegree(size_t nodeIdx) const {
        float degree = 0.0f;
        for (size_t i = 0; i < synapsesByLayer.size(); ++i) {
            for (const auto& synList : synapsesByLayer[i]) {
                for (Synapse* syn : synList) {
                    if (syn->getSourceNeuron().index() == nodeIdx ||
                        syn->getDestinationNeuron().index() == nodeIdx) {
                        ++degree;
                    }
                }
            }
        }
        return degree;
    }
};

AdvancedNeuralNetwork::AdvancedNeuralNetwork() : pImpl(std::make_unique<Impl>()) {}
AdvancedNeuralNetwork::~AdvancedNeuralNetwork() = default;

void AdvancedNeuralNetwork::addLayer(const Layer& layer) {
    pImpl->layers.push_back(layer);
    pImpl->neuronsByLayer.push_back(std::vector<Neuron*>());
    pImpl->synapsesByLayer.push_back(std::vector<std::vector<Synapse*>>());
    pImpl->layerOutputs.push_back(std::vector<float>());
    
    NLM_LOG_INFO("Added layer: " + layer.name + " with " + 
                 std::to_string(layer.neuronCount) + " neurons, type: " + 
                 std::to_string(static_cast<int>(layer.type)));
}

void AdvancedNeuralNetwork::connectLayers() {
    for (size_t sourceIdx = 0; sourceIdx < pImpl->layers.size(); ++sourceIdx) {
        for (size_t destIdx = 0; destIdx < pImpl->layers.size(); ++destIdx) {
            if (sourceIdx == destIdx) continue;  // Skip self-connections for now
            
            const Layer& sourceLayer = pImpl->layers[sourceIdx];
            const Layer& destLayer = pImpl->layers[destIdx];
            
            // Apply connectivity pattern
            auto& rule = pImpl->connectivityRules[sourceLayer.connectivityPattern];
            if (rule(sourceIdx, destIdx)) {
                // Create neurons if needed
                while (pImpl->neuronsByLayer[sourceIdx].size() <= sourceIdx) {
                    NeuronId id(static_cast<uint64_t>(pImpl->neuronsByLayer[sourceIdx].size()) + 1);
                    pImpl->neuronsByLayer[sourceIdx].push_back(new Neuron(id));
                }
                
                while (pImpl->neuronsByLayer[destIdx].size() <= destIdx) {
                    NeuronId id(static_cast<uint64_t>(pImpl->neuronsByLayer[destIdx].size()) + 1);
                    pImpl->neuronsByLayer[destIdx].push_back(new Neuron(id));
                }
                
                // Create synapse
                SynapseId synId(static_cast<uint64_t>(pImpl->synapsesByLayer[sourceIdx].size()) + 1);
                Synapse* synapse = new Synapse(synId, 
                    pImpl->neuronsByLayer[sourceIdx][sourceIdx]->getId(),
                    pImpl->neuronsByLayer[destIdx][destIdx]->getId());
                
                // Set synapse type based on neuron types
                NeuronType sourceType = pImpl->neuronsByLayer[sourceIdx][sourceIdx]->getType();
                if (sourceType == NeuronType::Excitatory) {
                    synapse->setType(SynapseType::Excitatory);
                    synapse->setWeight(0.5f);
                } else if (sourceType == NeuronType::Inhibitory) {
                    synapse->setType(SynapseType::Inhibitory);
                    synapse->setWeight(-0.5f);
                }
                
                pImpl->synapsesByLayer[sourceIdx].push_back(std::vector<Synapse*>());
                pImpl->synapsesByLayer[sourceIdx].back().push_back(synapse);
            }
        }
    }
}

void AdvancedNeuralNetwork::configureConnectivity(const std::string& pattern, float probability) {
    pImpl->connectivityRules[pattern] = [pattern, probability, this](size_t sourceIdx, size_t destIdx) {
        if (pImpl->connectivityRules.find(pattern) != pImpl->connectivityRules.end()) {
            auto& rule = pImpl->connectivityRules[pattern];
            return rule(sourceIdx, destIdx);
        }
        return probability > pImpl->rng.uniformReal(0.0f, 1.0f);
    };
    
    NLM_LOG_INFO("Configured connectivity pattern: " + pattern + " with probability: " + 
                 std::to_string(probability));
}

void AdvancedNeuralNetwork::registerNeuronType(const std::string& typeName, 
                                             std::unique_ptr<NeuronFactory> factory) {
    pImpl->neuronFactories[typeName] = std::move(factory);
    NLM_LOG_INFO("Registered neuron type: " + typeName);
}

void AdvancedNeuralNetwork::registerSynapseType(const std::string& typeName,
                                               std::unique_ptr<SynapseFactory> factory) {
    pImpl->synapseFactories[typeName] = std::move(factory);
    NLM_LOG_INFO("Registered synapse type: " + typeName);
}

void AdvancedNeuralNetwork::addExternalInput(NeuronId neuronId, const std::vector<float>& inputData) {
    pImpl->externalInputs[neuronId] = inputData;
    NLM_LOG_INFO("Added external input to neuron " + std::to_string(neuronId.index()));
}

void AdvancedNeuralNetwork::addExternalOutput(NeuronId neuronId, 
                                            std::function<void(std::vector<float>)> outputCallback) {
    pImpl->externalOutputs[neuronId] = outputCallback;
    NLM_LOG_INFO("Added external output callback for neuron " + std::to_string(neuronId.index()));
}

void AdvancedNeuralNetwork::forwardPropagate(const std::vector<float>& input) {
    // Clear previous outputs
    pImpl->layerOutputs.clear();
    
    // Process input layer
    if (!pImpl->layers.empty() && pImpl->layers[0].type == LayerType::Input) {
        std::vector<float> inputLayerOutput;
        for (size_t i = 0; i < pImpl->layers[0].neuronCount; ++i) {
            inputLayerOutput.push_back(input.size() > i ? input[i] : 0.0f);
        }
        pImpl->layerOutputs.push_back(inputLayerOutput);
        
        // Apply external inputs
        for (const auto& extInput : pImpl->externalInputs) {
            for (size_t layerIdx = 0; layerIdx < pImpl->layers.size(); ++layerIdx) {
                for (Neuron* neuron : pImpl->neuronsByLayer[layerIdx]) {
                    if (neuron->getId() == extInput.first) {
                        // Add external input to neuron
                        neuron->injectCurrent(extInput.second[0]);  // Simplified
                    }
                }
            }
        }
    }
    
    // Process hidden and output layers
    for (size_t layerIdx = 1; layerIdx < pImpl->layers.size(); ++layerIdx) {
        const Layer& layer = pImpl->layers[layerIdx];
        std::vector<float> layerOutput;
        
        // For now, simple activation function (sigmoid)
        for (size_t neuronIdx = 0; neuronIdx < layer.neuronCount; ++neuronIdx) {
            float sum = 0.0f;
            
            // Collect inputs from previous layer
            if (layerIdx > 0 && neuronIdx < pImpl->neuronsByLayer[layerIdx-1].size()) {
                Neuron* neuron = pImpl->neuronsByLayer[layerIdx-1][neuronIdx];
                // Get membrane potential as activation
                sum = neuron->getMembranePotential();
            }
            
            // Apply activation function
            float activation = 1.0f / (1.0f + std::exp(-sum));
            layerOutput.push_back(activation);
        }
        
        pImpl->layerOutputs.push_back(layerOutput);
    }
    
    // Apply external outputs
    for (const auto& extOutput : pImpl->externalOutputs) {
        if (extOutput.first.index() < pImpl->layerOutputs.size()) {
            extOutput.second(pImpl->layerOutputs[extOutput.first.index()]);
        }
    }
}

std::vector<float> AdvancedNeuralNetwork::getLayerOutput(size_t layerIndex) const {
    if (layerIndex < pImpl->layerOutputs.size()) {
        return pImpl->layerOutputs[layerIndex];
    }
    return std::vector<float>();
}

std::vector<float> AdvancedNeuralNetwork::getNetworkOutput() const {
    if (!pImpl->layers.empty()) {
        size_t lastLayerIdx = pImpl->layers.size() - 1;
        if (lastLayerIdx < pImpl->layerOutputs.size()) {
            return pImpl->layerOutputs[lastLayerIdx];
        }
    }
    return std::vector<float>();
}

std::vector<Neuron*> AdvancedNeuralNetwork::getNeuronsByType(NeuronType type) const {
    std::vector<Neuron*> result;
    for (const auto& layerNeurons : pImpl->neuronsByLayer) {
        for (Neuron* neuron : layerNeurons) {
            if (neuron->getType() == type) {
                result.push_back(neuron);
            }
        }
    }
    return result;
}

std::vector<Synapse*> AdvancedNeuralNetwork::getSynapsesByType(SynapseType type) const {
    std::vector<Synapse*> result;
    for (const auto& layerSynapses : pImpl->synapsesByLayer) {
        for (const auto& synList : layerSynapses) {
            for (Synapse* synapse : synList) {
                if (synapse->getType() == type) {
                    result.push_back(synapse);
                }
            }
        }
    }
    return result;
}

size_t AdvancedNeuralNetwork::getLayerCount() const {
    return pImpl->layers.size();
}

size_t AdvancedNeuralNetwork::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& layerNeurons : pImpl->neuronsByLayer) {
        total += layerNeurons.size();
    }
    return total;
}

float AdvancedNeuralNetwork::getNetworkActivity() const {
    float totalActivity = 0.0f;
    float neuronCount = 0.0f;
    
    for (const auto& layerNeurons : pImpl->neuronsByLayer) {
        for (const Neuron* neuron : layerNeurons) {
            totalActivity += neuron->isFiring() ? 1.0f : 0.0f;
            ++neuronCount;
        }
    }
    
    return neuronCount > 0.0f ? totalActivity / neuronCount : 0.0f;
}

} // namespace nlm
