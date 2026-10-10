



void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!brain_) return;
    
    auto* targetNeuron = brain_->getNeuron(neuron);
    if (!targetNeuron) return;
    
    // Check if neuron is already in memory
    bool found = false;
    for (size_t i = 0; i < traces_.size(); ++i) {
        auto& trace = traces_[i];
        for (size_t j = 0; j < trace.neurons.size(); ++j) {
            if (trace.neurons[j] == neuron) {
                trace.currentActivations[j] = activation;
                trace.targetActivations[j] = activation;
                found = true;
                break;
            }
        }
        if (found) break;
    }
    
    if (!found) {
        // Create a new trace for this single neuron
        auto newTrace = std::make_unique<Impl::MemoryTrace>(1, 0, 1.0f);
        newTrace->neurons[0] = neuron;
        newTrace->targetActivations[0] = activation;
        newTrace->currentActivations[0] = activation;
        
        auto* n = brain_->getNeuron(neuron);
        if (n) {
            n->setMembranePotential(activation * 20.0f);
            n->injectCurrent(activation * 30.0f);
        }
        
        traces_.push_back(std::move(*newTrace));
    }
    
    // Update memory lists
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    
    for (const auto& trace : traces_) {
        memoryNeurons_.insert(memoryNeurons_.end(), trace.neurons.begin(), trace.neurons.end());
        for (float act : trace.currentActivations) {
            memoryActivations_.push_back(act);
        }
        memoryTimestamps_.push_back(trace.birthTime);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    
    // Aggregate activations from all traces
    std::unordered_map<NeuronId, float> neuronActivations;
    
    for (const auto& trace : pImpl->traces_) {
        for (size_t i = 0; i < trace.neurons.size(); ++i) {
            neuronActivations[trace.neurons[i]] += trace.currentActivations[i];
        }
    }
    
    // Convert to vector in neuron ID order
    std::vector<NeuronId> allNeurons = brain_->getAllNeuronIds();
    result.reserve(allNeurons.size());
    
    for (NeuronId id : allNeurons) {
        auto it = neuronActivations.find(id);
        if (it != neuronActivations.end()) {
            result.push_back(it->second);
        } else {
            result.push_back(0.0f);
        }
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces_) {
        for (NeuronId id : trace.neurons) {
            if (id == neuron) return true;
        }
    }
    return false;
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    float activation = 0.0f;
    int count = 0;
    
    for (const auto& trace : pImpl->traces_) {
        for (size_t i = 0; i < trace.neurons.size(); ++i) {
            if (trace.neurons[i] == neuron) {
                activation += trace.currentActivations[i];
                count++;
            }
        }
    }
    
    return count > 0 ? activation / count : 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Decay all traces
    decayWeakTraces();
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Update each trace
    std::vector<Impl::MemoryTrace> updatedTraces;
    
    for (auto& trace : pImpl->traces_) {
        if (!trace.isActive) continue;
        
        // Decay trace strength
        trace.strength *= (1.0f - decayRate_ * dt);
        
        // Update current activations toward targets with learning dynamics
        bool changed = false;
        for (size_t i = 0; i < trace.neurons.size(); ++i) {
            float target = trace.targetActivations[i];
            float& current = trace.currentActivations[i];
            
            // Attractor dynamics: move toward target
            float error = target - current;
            current += error * 0.1f * dt;  // Learning rate
            
            // Ensure bounds
            current = std::clamp(current, 0.0f, 1.0f);
            
            // Update neuron membrane potential
            auto* neuron = brain_->getNeuron(trace.neurons[i]);
            if (neuron) {
                neuron->setMembranePotential(current * 20.0f);
            }
            
            if (std::abs(error) > 0.01f) changed = true;
        }
        
        if (changed) {
            trace.birthTime++;  // Mark as recently updated
        }
        
        // Check if trace is too weak
        if (trace.strength < 0.05f) {
            trace.isActive = false;
        } else {
            updatedTraces.push_back(std::move(trace));
        }
    }
    
    pImpl->traces_ = std::move(updatedTraces);
    
    // Update memory lists
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    
    for (const auto& trace : pImpl->traces_) {
        memoryNeurons_.insert(memoryNeurons_.end(), trace.neurons.begin(), trace.neurons.end());
        for (float act : trace.currentActivations) {
            memoryActivations_.push_back(act);
        }
        memoryTimestamps_.push_back(trace.birthTime);
    }
}

void NeuralWorkingMemory::clear() {
    pImpl->traces_.clear();
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    
    // Clear recurrent connections
    recurrentConnections_.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& trace : pImpl->traces_) {
        trace.strength *= factor;
        trace.competitionWeight *= factor;
    }
}

void NeuralWorkingMemory::runCompetition() {
    if (pImpl->traces_.size() <= 1) return;
    
    // Calculate competition scores
    std::vector<float> scores;
    for (const auto& trace : pImpl->traces_) {
        float score = trace.strength * trace.competitionWeight;
        
        // Add bias based on current activations
        for (float act : trace.currentActivations) {
            score += act * 2.0f;
        }
        
        scores.push_back(score);
    }
    
    // Find winner (highest score)
    size_t winnerIndex = 0;
    float maxScore = scores[0];
    
    for (size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > maxScore) {
            maxScore = scores[i];
            winnerIndex = i;
        }
    }
    
    // Update winners
    winners_.clear();
    for (size_t i = 0; i < pImpl->traces_[winnerIndex].neurons.size(); ++i) {
        winners_.push_back(pImpl->traces_[winnerIndex].neurons[i]);
    }
    
    // Apply competitive suppression to weaker traces
    for (size_t i = 0; i < pImpl->traces_.size(); ++i) {
        if (i == winnerIndex) continue;
        
        float suppression = scores[winnerIndex] / (scores[i] + 0.01f);
        pImpl->traces_[i].competitionWeight *= (1.0f - suppression * 0.5f);
        
        // Suppress activations
        for (float& act : pImpl->traces_[i].currentActivations) {
            act *= (1.0f - suppression * 0.3f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    for (NeuronId id : winners_) {
        if (id == neuron) return true;
    }
    return false;
}

float NeuralWorkingMemory::getMemoryActivity() const {
    float totalActivity = 0.0f;
    int count = 0;
    
    for (const auto& trace : pImpl->traces_) {
        for (float act : trace.currentActivations) {
            totalActivity += act;
            count++;
        }
    }
    
    return count > 0 ? totalActivity / count : 0.0f;
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : recurrentConnections_) {
        if ((conn.first == from && conn.second == to) || 
            (conn.first == to && conn.second == from)) {
            return; // Connection already exists
        }
    }
    
    // Create new connection
    recurrentConnections_.emplace_back(from, to);
    
    // In a real implementation, this would add a synaptic connection
    // with the specified strength to maintain persistent activity
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // In a real implementation, this would update the weights of
    // recurrent connections to maintain memory traces
    
    // For now, just maintain connections
    std::vector<std::pair<NeuronId, NeuronId>> activeConnections;
    
    for (const auto& conn : recurrentConnections_) {
        // Check if both neurons are currently active
        bool fromActive = false;
        bool toActive = false;
        
        for (const auto& trace : pImpl->traces_) {
            for (NeuronId id : trace.neurons) {
                if (id == conn.first) fromActive = true;
                if (id == conn.second) toActive = true;
            }
        }
        
        if (fromActive && toActive) {
            activeConnections.push_back(conn);
        }
    }
    
    recurrentConnections_ = std::move(activeConnections);
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<Impl::MemoryTrace> survivingTraces;
    
    for (auto& trace : pImpl->traces_) {
        // Decay based on age and strength
        float ageDecay = 1.0f - (trace.birthTime * decayRate_);
        trace.strength *= ageDecay;
        
        // Check if still active
        if (trace.strength > 0.05f) {
            survivingTraces.push_back(std::move(trace));
        }
    }
    
    pImpl->traces_ = std::move(survivingTraces);
}

} // namespace nlm
