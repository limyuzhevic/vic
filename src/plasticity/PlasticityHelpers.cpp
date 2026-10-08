#include "Brain.hpp"
#include "../core/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Function to apply Hebbian learning with proper error handling
void applyHebbianLearning(Synapse* synapse, 
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse) {
        throw NeuralComputationException("Hebbian", "applyHebbianLearning",
                                        "Null synapse pointer provided", ErrorSeverity::Error);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        throw NeuralComputationException("Hebbian", "applyHebbianLearning",
                                        "Empty spike history provided", ErrorSeverity::Warning);
    }
    
    // Calculate activity statistics
    float preActivity = calculateActivity(preSpikes);
    float postActivity = calculateActivity(postSpikes);
    float covariance = calculateCovariance(preSpikes, postSpikes);
    
    // Update running statistics
    updateRunningStatistics(preActivity, postActivity, covariance, dt);
    
    // Calculate weight change
    float delta = calculateWeightChange(covariance, preActivity, postActivity);
    
    // Apply weight change with bounds checking
    if (std::abs(delta) > 1e-6f) {
        applyWeightChangeWithBounds(synapse, delta);
    }
}

// Function to apply STDP with error handling
void applySTDP(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt) {
    if (!synapse) {
        throw NeuralComputationException("STDP", "applySTDP",
                                        "Null synapse pointer provided", ErrorSeverity::Error);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return; // STDP can handle empty spikes (no change)
    }
    
    float weightChange = calculateSTDPWeightChange(synapse, preSpikes, postSpikes, dt);
    
    applyWeightChangeWithBounds(synapse, weightChange);
}

// Function to apply optimization rules with error handling
void applyOptimizedPlasticity(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse) {
        throw NeuralComputationException("OptimizedPlasticity", "applyOptimizedPlasticity",
                                        "Null synapse pointer provided", ErrorSeverity::Error);
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return; // Can handle empty spikes gracefully
    }
    
    // Use optimized computation with spatial indexing if available
    if (hasSpatialIndexing()) {
        float weightChange = calculateOptimizedWeightChange(synapse, preSpikes, postSpikes, dt);
        applyWeightChangeWithBounds(synapse, weightChange);
    } else {
        // Fall back to standard computation
        applySTDP(synapse, preSpikes, postSpikes, dt);
        applyHebbianLearning(synapse, preSpikes, postSpikes, dt);
    }
}

// Function to initialize plasticity systems with error handling
void initializePlasticitySystems(Brain* brain) {
    if (!brain) {
        throw NeuralComputationException("Plasticity", "initializePlasticitySystems",
                                        "Null brain pointer provided", ErrorSeverity::Fatal);
    }
    
    try {
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        if (!stdp || !hebbian || !structural) {
            throw NeuralComputationException("Plasticity", "initializePlasticitySystems",
                                            "Null plasticity system pointer", ErrorSeverity::Error);
        }
        
        // Configure from brain configuration if available
        if (brain->getConfig()) {
            auto config = brain->getConfig();
            
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            
            stdp->configure(ltpWeight, ltdWeight, tau);
            
            // Configure Hebbian parameters
            hebbian->setLearningRate(config->getOr<float>("hebbian_learning_rate", 0.01f));
            hebbian->setMaxWeight(config->getOr<float>("hebbian_max_weight", 1.0f));
            
            // Configure structural plasticity
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            
            structural->setSynaptogenesisRate(synaptogenesisRate);
            structural->setPruningRate(pruningRate);
        }
        
    } catch (const NLMException& e) {
        // Re-throw NLM exceptions
        throw;
    } catch (const std::exception& e) {
        // Convert other exceptions to NLMException
        throw NeuralComputationException("Plasticity", "initializePlasticitySystems",
                                        std::string("Unexpected exception: ") + e.what(),
                                        ErrorSeverity::Error);
    }
}

// Function to handle simulation step with comprehensive error handling
void executeSimulationStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) {
        throw NeuralComputationException("Simulation", "executeSimulationStep",
                                        "Null brain pointer provided", ErrorSeverity::Fatal);
    }
    
    try {
        // Check if brain is in valid state
        if (!brain->initialize()) {
            throw NeuralComputationException("Simulation", "executeSimulationStep",
                                            "Brain initialization failed", ErrorSeverity::Critical);
        }
        
        // Execute the main simulation step
        brain->step(currentStep, currentTime);
        
        // Collect and log statistics
        logSimulationStatistics(brain);
        
    } catch (const NLMException& e) {
        // Handle NLM exceptions
        if (e.getSeverity() >= ErrorSeverity::Critical) {
            // For critical errors, we might need to terminate
            throw;
        }
        // For recoverable errors, continue with degraded functionality
        logDegradedOperation(e);
    } catch (const std::exception& e) {
        // Convert other exceptions to NLMException
        throw NeuralComputationException("Simulation", "executeSimulationStep",
                                        std::string("Unexpected exception: ") + e.what(),
                                        ErrorSeverity::Error);
    }
}

// Helper functions
float calculateActivity(const std::vector<Timestamp>& spikes) {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average firing rate
    float windowDuration = 100.0f; // 100ms window
    size_t spikeCount = spikes.size();
    
    return (static_cast<float>(spikeCount) / windowDuration) * 1000.0f; // Convert to Hz
}

float calculateCovariance(const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simplified covariance calculation
    float preActivity = calculateActivity(preSpikes);
    float postActivity = calculateActivity(postSpikes);
    
    // Calculate overlapping spikes
    size_t overlappingSpikes = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            if (std::abs(static_cast<float>(postTime) - static_cast<float>(preTime)) < 20.0f) {
                overlappingSpikes++;
                break;
            }
        }
    }
    
    float normalizedOverlap = (overlappingSpikes / static_cast<float>(preSpikes.size() * postSpikes.size()));
    return normalizedOverlap * preActivity * postActivity;
}

void updateRunningStatistics(float preActivity, float postActivity, float covariance, TimestepDuration dt) {
    // Simple exponential moving average
    static float decayFactor = 0.95f;
    
    static float runningPreActivity = 0.0f;
    static float runningPostActivity = 0.0f;
    static float runningCovariance = 0.0f;
    
    runningPreActivity = runningPreActivity * decayFactor + preActivity * (1.0f - decayFactor);
    runningPostActivity = runningPostActivity * decayFactor + postActivity * (1.0f - decayFactor);
    runningCovariance = runningCovariance * decayFactor + covariance * (1.0f - decayFactor);
}

float calculateWeightChange(float covariance, float preActivity, float postActivity) {
    // Base weight change from covariance
    float delta = 0.01f * covariance;
    
    // Subtract baseline to prevent runaway potentiation
    float baseline = 0.01f * preActivity * postActivity;
    delta -= baseline;
    
    // Apply normalization by activity levels
    float activityNormalization = std::sqrt(preActivity * postActivity);
    if (activityNormalization > 0.0f) {
        delta /= activityNormalization;
    }
    
    return delta;
}

float calculateSTDPWeightChange(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = 20.0f; // Default STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtVal = static_cast<float>(postTime - preTime);
            
            if (dtVal > 0) {
                // Pre before post: POTENTIATION
                float delta = 0.01f * std::exp(-dtVal / tau);
                totalDelta += delta;
            } else if (dtVal < 0) {
                // Post before pre: DEPRESSION
                float delta = -0.012f * std::exp(dtVal / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

void applyWeightChangeWithBounds(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f); // Default weight bounds
    synapse->setWeight(newWeight);
}

void logSimulationStatistics(Brain* brain) {
    if (!brain) return;
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float averageFiringRate = brain->getAverageFiringRate();
    
    // Log statistics using the brain's logger
    // In production, this would use the brain's logging system
    std::cout << "Simulation Statistics:" << std::endl;
    std::cout << "  Total neurons: " << totalNeurons << std::endl;
    std::cout << "  Total synapses: " << totalSynapses << std::endl;
    std::cout << "  Active neurons: " << activeNeurons << std::endl;
    std::cout << "  Firing neurons: " << firingNeurons << std::endl;
    std::cout << "  Average firing rate: " << averageFiringRate << " Hz" << std::endl;
}

void logDegradedOperation(const NLMException& exception) {
    // Log degraded operation for monitoring and debugging
    std::cerr << "[WARNING] Degraded operation due to: " << exception.toString() << std::endl;
}

bool hasSpatialIndexing() {
    // In production, this would check if spatial indexing is enabled
    return true; // For demonstration, assume it's enabled
}

float calculateOptimizedWeightChange(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) {
    // Simplified optimized calculation using traces
    float totalDelta = 0.0f;
    float tau = 20.0f;
    
    // Use pre/post spike traces for faster computation
    float preTrace = calculateTraceActivity(preSpikes);
    float postTrace = calculateTraceActivity(postSpikes);
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtVal = static_cast<float>(postTime - preTime);
            
            if (dtVal > 0) {
                // Pre before post with trace enhancement
                float delta = 0.01f * std::exp(-dtVal / tau) * (1.0f + postTrace);
                totalDelta += delta;
            } else if (dtVal < 0) {
                // Post before pre with trace enhancement
                float delta = -0.012f * std::exp(dtVal / tau) * (1.0f + preTrace);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float calculateTraceActivity(const std::vector<Timestamp>& spikes) {
    // Calculate trace activity based on recent spike history
    if (spikes.empty()) return 0.0f;
    
    // Simple exponential trace decay
    float trace = 0.0f;
    float decayFactor = 0.9f;
    
    for (Timestamp spikeTime : spikes) {
        trace = trace * decayFactor + 1.0f; // Each spike adds to trace
    }
    
    return trace;
}

} // namespace nlm
