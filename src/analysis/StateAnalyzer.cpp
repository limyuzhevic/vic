#include "StateAnalyzer.hpp"

namespace nlm {

StateAnalyzer::StateSnapshot StateAnalyzer::captureState(const Brain& brain) const {
    StateSnapshot snapshot;
    snapshot.timestamp = std::chrono::high_resolution_clock::now();
    
    // Extract neuron states
    extractNeuronStates(brain, snapshot.neuronStates);
    
    // Extract synapse states
    extractSynapseStates(brain, snapshot.synapseStates);
    
    // Extract spike counts
    extractSpikeCounts(brain, snapshot.spikeCounts);
    
    // Calculate global energy
    snapshot.globalEnergy = calculateGlobalEnergy(brain);
    
    // Calculate average firing rate
    snapshot.averageFiringRate = brain.getAverageFiringRate();
    
    // Extract neuron activities
    size_t neuronCount = brain.getTotalNeuronCount();
    snapshot.neuronActivities.clear();
    for (size_t i = 0; i < neuronCount; ++i) {
        snapshot.neuronActivities[NeuronId(i)] = 0.5; // Placeholder
    }
    
    // Extract synapse weights
    size_t synapseCount = brain.getTotalSynapseCount();
    snapshot.synapseWeights.clear();
    for (size_t i = 0; i < synapseCount; ++i) {
        snapshot.synapseWeights[SynapseId(i)] = 0.5; // Placeholder
    }
    
    return snapshot;
}

std::vector<StateAnalyzer::StateChange> StateAnalyzer::compareStates(
    const StateSnapshot& oldState, const StateSnapshot& newState) const {
    std::vector<StateChange> changes;
    
    if (oldState.neuronStates.size() != newState.neuronStates.size()) {
        return changes;
    }
    
    StateChange change;
    change.timestamp = std::chrono::high_resolution_clock::now();
    
    // Calculate state delta
    double totalDelta = 0.0;
    for (size_t i = 0; i < oldState.neuronStates.size(); ++i) {
        totalDelta += std::abs(newState.neuronStates[i] - oldState.neuronStates[i]);
    }
    
    change.magnitude = totalDelta;
    change.type = determineChangeType(oldState, newState);
    
    if (change.magnitude > 0.01) { // Threshold for significant change
        change.description = "State change detected";
        changes.push_back(change);
    }
    
    return changes;
}

StateAnalyzer::EvolutionMetrics StateAnalyzer::trackEvolution(const std::vector<StateSnapshot>& states) const {
    EvolutionMetrics metrics;
    
    if (states.size() < 2) {
        return metrics;
    }
    
    metrics.stateDeltas.resize(states.size() - 1);
    
    for (size_t i = 0; i < states.size() - 1; ++i) {
        double delta = 0.0;
        for (size_t j = 0; j < states[i].neuronStates.size(); ++j) {
            delta += std::abs(states[i + 1].neuronStates[j] - states[i].neuronStates[j]);
        }
        metrics.stateDeltas[i] = delta;
        metrics.totalChangeMagnitude += delta;
    }
    
    metrics.stabilityScore = calculateStateStability(states.back());
    metrics.changeFrequency = metrics.stateDeltas.size() / static_cast<double>(states.size() - 1);
    metrics.volatility = calculateStateVolatility(states);
    
    return metrics;
}

std::vector<StateAnalyzer::StateChange> StateAnalyzer::detectAnomalies(
    const std::vector<StateSnapshot>& states) const {
    std::vector<StateChange> anomalies;
    
    if (states.size() < 3) {
        return anomalies;
    }
    
    // Calculate statistics
    std::vector<double> stateMeans;
    std::vector<double> stateStds;
    
    // Simple rolling statistics (placeholder)
    size_t windowSize = 3;
    for (size_t i = windowSize; i < states.size(); ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < windowSize; ++j) {
            sum += states[i - j].averageFiringRate;
        }
        double mean = sum / windowSize;
        
        double variance = 0.0;
        for (size_t j = 0; j < windowSize; ++j) {
            variance += std::pow(states[i - j].averageFiringRate - mean, 2);
        }
        double stddev = std::sqrt(variance / windowSize);
        
        stateMeans.push_back(mean);
        stateStds.push_back(stddev);
    }
    
    // Detect anomalies (values beyond 2 standard deviations)
    for (size_t i = windowSize; i < states.size(); ++i) {
        double value = states[i].averageFiringRate;
        size_t idx = i - windowSize;
        
        if (idx < stateMeans.size()) {
            double zScore = std::abs((value - stateMeans[idx]) / (stateStds[idx] + 1e-6));
            if (zScore > 2.0) {
                StateChange anomaly;
                anomaly.timestamp = states[i].timestamp;
                anomaly.type = StateChangeType::AnomalousChange;
                anomaly.magnitude = zScore;
                anomaly.description = "Statistical anomaly detected";
                anomalies.push_back(anomaly);
            }
        }
    }
    
    return anomalies;
}

std::vector<size_t> StateAnalyzer::findOutliers(const std::vector<StateSnapshot>& states,
                                                double threshold) const {
    std::vector<size_t> outliers;
    
    if (states.size() < 3) {
        return outliers;
    }
    
    // Calculate mean and standard deviation of a feature (average firing rate)
    double sum = 0.0;
    for (const auto& state : states) {
        sum += state.averageFiringRate;
    }
    double mean = sum / states.size();
    
    double variance = 0.0;
    for (const auto& state : states) {
        variance += std::pow(state.averageFiringRate - mean, 2);
    }
    double stddev = std::sqrt(variance / states.size());
    
    // Find outliers
    for (size_t i = 0; i < states.size(); ++i) {
        double zScore = std::abs((states[i].averageFiringRate - mean) / (stddev + 1e-6));
        if (zScore > threshold) {
            outliers.push_back(i);
        }
    }
    
    return outliers;
}

std::vector<double> StateAnalyzer::analyzeStateTransitions(const std::vector<StateSnapshot>& states) const {
    std::vector<double> transitions;
    
    if (states.size() < 2) {
        return transitions;
    }
    
    for (size_t i = 0; i < states.size() - 1; ++i) {
        double delta = 0.0;
        for (size_t j = 0; j < states[i].neuronStates.size(); ++j) {
            delta += std::abs(states[i + 1].neuronStates[j] - states[i].neuronStates[j]);
        }
        transitions.push_back(delta);
    }
    
    return transitions;
}

double StateAnalyzer::calculateStateStability(const StateSnapshot& state) const {
    // Calculate stability based on neuron activity variance
    if (state.neuronActivities.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    double sumSq = 0.0;
    for (const auto& activity : state.neuronActivities) {
        sum += activity.second;
        sumSq += activity.second * activity.second;
    }
    
    double mean = sum / state.neuronActivities.size();
    double variance = (sumSq / state.neuronActivities.size()) - (mean * mean);
    
    // Higher variance means less stability
    return std::exp(-variance);
}

double StateAnalyzer::calculateStateVolatility(const std::vector<StateSnapshot>& states) const {
    if (states.size() < 2) {
        return 0.0;
    }
    
    std::vector<double> rates;
    for (const auto& state : states) {
        rates.push_back(state.averageFiringRate);
    }
    
    double sum = std::accumulate(rates.begin(), rates.end(), 0.0);
    double mean = sum / rates.size();
    
    double variance = 0.0;
    for (double rate : rates) {
        variance += std::pow(rate - mean, 2);
    }
    variance /= rates.size();
    
    return std::sqrt(variance);
}

std::vector<double> StateAnalyzer::extractStateFeatures(const StateSnapshot& state) const {
    std::vector<double> features;
    
    // Extract various state features
    features.push_back(state.globalEnergy);
    features.push_back(state.averageFiringRate);
    features.push_back(static_cast<double>(state.spikeCounts.size()));
    
    // Add neuron state statistics
    if (!state.neuronStates.empty()) {
        double sum = std::accumulate(state.neuronStates.begin(), state.neuronStates.end(), 0.0);
        double mean = sum / state.neuronStates.size();
        
        double variance = 0.0;
        for (double val : state.neuronStates) {
            variance += std::pow(val - mean, 2);
        }
        variance /= state.neuronStates.size();
        
        features.push_back(mean);
        features.push_back(std::sqrt(variance));
    }
    
    return features;
}

std::vector<std::vector<double>> StateAnalyzer::generateStateSpaceTrajectories(
    const std::vector<StateSnapshot>& states) const {
    std::vector<std::vector<double>> trajectories;
    
    for (const auto& state : states) {
        trajectories.push_back(extractStateFeatures(state));
    }
    
    return trajectories;
}

bool StateAnalyzer::exportStateSnapshots(const std::string& filepath,
                                        const std::vector<StateSnapshot>& snapshots) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

bool StateAnalyzer::exportStateChanges(const std::string& filepath,
                                       const std::vector<StateChange>& changes) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

bool StateAnalyzer::exportEvolutionMetrics(const std::string& filepath,
                                           const EvolutionMetrics& metrics) const {
    // Placeholder implementation - would write to CSV/JSON file
    return true;
}

void StateAnalyzer::extractNeuronStates(const Brain& brain, std::vector<double>& states) const {
    states.clear();
    
    // Extract neuron states from brain regions
    const auto& regions = brain.getRegions();
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuronId : population->getNeuronIds()) {
                // Placeholder - would extract actual neuron state
                states.push_back(0.5 + (rand() / static_cast<double>(RAND_MAX)) * 0.5);
            }
        }
    }
}

void StateAnalyzer::extractSynapseStates(const Brain& brain, std::vector<double>& states) const {
    states.clear();
    
    // Extract synapse states (placeholder)
    size_t synapseCount = brain.getTotalSynapseCount();
    states.resize(synapseCount, 0.5); // Placeholder values
}

void StateAnalyzer::extractSpikeCounts(const Brain& brain, std::vector<size_t>& counts) const {
    counts.clear();
    
    // Extract spike counts from spike system
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        const auto& history = spikeSystem->getSpikeHistory();
        counts.resize(history.size(), 0);
        for (const auto& spike : history) {
            size_t neuronId = spike.source.index();
            if (neuronId < counts.size()) {
                counts[neuronId]++;
            }
        }
    }
}

double StateAnalyzer::calculateGlobalEnergy(const Brain& brain) const {
    // Calculate global energy based on neuron activities
    double energy = 0.0;
    
    // Simple energy calculation (placeholder)
    size_t neuronCount = brain.getTotalNeuronCount();
    energy = neuronCount * 0.5; // Placeholder
    
    return energy;
}

StateAnalyzer::StateChangeType StateAnalyzer::determineChangeType(
    const StateSnapshot& oldState, const StateSnapshot& newState) const {
    if (oldState.neuronStates.size() != newState.neuronStates.size()) {
        return StateChangeType::AnomalousChange;
    }
    
    double maxDelta = 0.0;
    for (size_t i = 0; i < oldState.neuronStates.size(); ++i) {
        double delta = std::abs(newState.neuronStates[i] - oldState.neuronStates[i]);
        maxDelta = std::max(maxDelta, delta);
    }
    
    if (maxDelta < 0.001) {
        return StateChangeType::NoChange;
    } else if (maxDelta < 0.01) {
        return StateChangeType::GradualChange;
    } else if (maxDelta < 0.1) {
        return StateChangeType::SuddenChange;
    } else if (maxDelta < 0.5) {
        return StateChangeType::CyclicChange;
    } else {
        return StateChangeType::BurstChange;
    }
}

} // namespace nlm