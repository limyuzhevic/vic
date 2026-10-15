# NLM Neural Models

## Overview
NLM implements various neural models and dynamics for simulating brain-like computation. The system supports multiple neuron types and plasticity rules.

## Neural Model Types

### 1. Leaky Integrate-and-Fire (LIF) Neuron Model
The LIF model is the fundamental neuron model used in NLM. It implements the standard integrate-and-fire dynamics with adaptive firing thresholds.

#### Model Equations
```
dv/dt = (-v + R*I)/tau
if v >= v_threshold:
    spike = 1
    v = v_reset
    refractory_time = refractory_period
```

#### Model Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| membrane_tau | float | 0.01 | Membrane time constant |
| membrane_threshold | float | 0.5 | Firing threshold |
| membrane_reset | float | 0.0 | Reset potential |
| refractory_period | float | 0.005 | Refractory period |
| resting_potential | float | 0.0 | Resting potential |

#### Model Implementation
```cpp
class LIFNeuronModel : public NeuralDynamics {
public:
    float update(float v, float I, float dt) override {
        // Membrane potential update
        float dv = (-v + R * I) / membrane_tau;
        v += dv * dt;
        
        // Spike detection
        if (v >= membrane_threshold) {
            spike = true;
            v = membrane_reset;
            refractory_timer = refractory_period;
        }
        
        // Refractory period
        if (refractory_timer > 0) {
            refractory_timer -= dt;
            v = membrane_reset;
        }
        
        return v;
    }
    
    bool hasSpike() const override { return spike; }
    void resetSpike() override { spike = false; }
    
private:
    bool spike = false;
    float refractory_timer = 0.0;
    const float R = 1.0; // Resistance
};
```

### 2. Adaptive Exponential Integrate-and-Fire (AdEx) Neuron Model
The AdEx model provides a more biophysically realistic neuron model with adaptive firing rates and subthreshold oscillations.

#### Model Equations
```
dv/dt = (-v + R*I)/tau + delta_T * exp((v - v_threshold)/delta_v)
dw/dt = (a * (v - v_rest) - w)/tau_w
```

#### Model Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| v_threshold | float | -50.0 | Threshold potential |
| delta_T | float | 2.0 | Slope factor |
| delta_v | float | 1.0 | Scale factor |
| a | float | 4.0 | Subthreshold slope |
| b | float | 80.0 | Adaptation magnitude |
| tau_w | float | 100.0 | Adaptation time constant |

#### Model Implementation
```cpp
class AdExNeuronModel : public NeuralDynamics {
public:
    float update(float v, float I, float w, float dt) override {
        // Subthreshold dynamics
        float dv = (-v + R * I) / tau + delta_T * exp((v - v_threshold) / delta_v);
        float dw = (a * (v - v_rest) - w) / tau_w;
        
        v += dv * dt;
        w += dw * dt;
        
        // Spike detection
        if (v >= v_threshold) {
            spike = true;
            v = v_reset;
            w += b;
            refractory_timer = refractory_period;
        }
        
        return w;
    }
    
    bool hasSpike() const override { return spike; }
    void resetSpike() override { spike = false; }
    
private:
    bool spike = false;
    float refractory_timer = 0.0;
    const float R = 1.0; // Resistance
    const float v_reset = -70.0; // Reset potential
    const float v_rest = -65.0; // Resting potential
};
```

### 3. Generalized Linear Model (GLM) Neuron Model
The GLM neuron model provides a flexible framework for modeling neural responses to stimuli.

#### Model Equations
```
λ(t) = f(v(t))
dN/dt = -N(t)/τ + λ(t) * η(t)
```

where N(t) is the spike count and η(t) is the noise.

#### Model Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| tau | float | 0.01 | Time constant |
| f | function | exp(v/Δv) | Firing rate function |
| Δv | float | 2.0 | Scale factor |

#### Model Implementation
```cpp
class GLMNeuronModel : public NeuralDynamics {
public:
    float update(float v, float I, float dt) override {
        // Firing rate
        float lambda = firing_rate_function(v);
        
        // Spike generation
        float probability = 1.0 - exp(-lambda * dt);
        if (rng.random() < probability) {
            spike = true;
        }
        
        return 0.0; // No adaptation in this model
    }
    
    bool hasSpike() const override { return spike; }
    void resetSpike() override { spike = false; }
    
private:
    bool spike = false;
    
    float firing_rate_function(float v) const {
        return exp(v / delta_v);
    }
};
```

## Plasticity Rules

### 1. Spike-Timing-Dependent Plasticity (STDP)
STDP is a learning rule where the timing of pre- and post-synaptic spikes determines the change in synaptic strength.

#### STDP Equations
```
if t_post > t_pre:
    Δw = A_LTP * exp(-(t_post - t_pre)/τ_LTP)
else:
    Δw = -A_LTD * exp(-(t_pre - t_post)/τ_LTD)
```

#### STDP Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| A_LTP | float | 0.01 | LTP magnitude |
| A_LTD | float | 0.012 | LTD magnitude |
| τ_LTP | float | 20.0 | LTP time constant |
| τ_LTD | float | 20.0 | LTD time constant |

#### STDP Implementation
```cpp
class STDPPlasticityRule : public PlasticityRule {
public:
    float computeWeightChange(float preSpikeTime, float postSpikeTime, 
                              float currentTime, float currentWeight) override {
        if (postSpikeTime > preSpikeTime) {
            // LTP
            return A_LTP * exp(-(postSpikeTime - preSpikeTime) / τ_LTP);
        } else {
            // LTD
            return -A_LTD * exp(-(preSpikeTime - postSpikeTime) / τ_LTD);
        }
    }
    
    std::string getName() const override { return "STDP"; }
    
private:
    const float A_LTP = 0.01;
    const float A_LTD = 0.012;
    const float τ_LTP = 20.0;
    const float τ_LTD = 20.0;
};
```

### 2. Hebbian Plasticity
Hebbian plasticity strengthens synapses that are active together.

#### Hebbian Equations
```
Δw = η * pre_spike * post_spike
```

#### Hebbian Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| η | float | 0.01 | Learning rate |

#### Hebbian Implementation
```cpp
class HebbianPlasticityRule : public PlasticityRule {
public:
    float computeWeightChange(float preSpikeTime, float postSpikeTime,
                              float currentTime, float currentWeight) override {
        // Simple Hebbian rule
        bool pre_spike = (currentTime - preSpikeTime) < 1.0;
        bool post_spike = (currentTime - postSpikeTime) < 1.0;
        
        return learning_rate * (pre_spike ? 1.0 : 0.0) * 
                          (post_spike ? 1.0 : 0.0);
    }
    
    std::string getName() const override { return "Hebbian"; }
    
private:
    const float learning_rate = 0.01;
};
```

### 3. Reward-Modulated STDP
Reward-modulated STDP combines STDP with reward signals to modulate learning.

#### Reward-Modulated STDP Equations
```
Δw = η * post_spike * pre_spike * (1 + r * λ)
```

#### Reward-Modulated STDP Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| η | float | 0.01 | Base learning rate |
| r | float | 0.1 | Reward modulation factor |
| λ | float | 0.0 | Reward signal |

#### Reward-Modulated STDP Implementation
```cpp
class RewardModulatedSTDP : public PlasticityRule {
public:
    float computeWeightChange(float preSpikeTime, float postSpikeTime,
                              float currentTime, float currentWeight,
                              float reward) override {
        // Reward-modulated STDP
        bool pre_spike = (currentTime - preSpikeTime) < 1.0;
        bool post_spike = (currentTime - postSpikeTime) < 1.0;
        
        return learning_rate * (post_spike ? 1.0 : 0.0) * 
                          (pre_spike ? 1.0 : 0.0) * (1.0 + reward_modulation * reward);
    }
    
    std::string getName() const override { return "RewardModulatedSTDP"; }
    
private:
    const float learning_rate = 0.01;
    const float reward_modulation = 0.1;
};
```

## Structural Plasticity

### 1. Synaptogenesis
Synaptogenesis is the formation of new synapses.

#### Synaptogenesis Equations
```
probability_of_synaptogenesis = rate * (1 - current_connections)
```

#### Synaptogenesis Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| synaptogenesis_rate | float | 0.001 | Synaptogenesis rate |

#### Synaptogenesis Implementation
```cpp
class SynaptogenesisPlasticity : public StructuralPlasticity {
public:
    bool shouldCreateSynapse(float preNeuron, float postNeuron,
                             const Brain& brain) override {
        // Check connection probability
        if (rng.random() < connection_probability) {
            // Check current connections
            if (brain.getConnectionCount(preNeuron, postNeuron) == 0) {
                // Create new synapse
                return true;
            }
        }
        return false;
    }
    
    std::string getName() const override { return "Synaptogenesis"; }
    
private:
    const float connection_probability = 0.15;
};
```

### 2. Pruning
Pruning is the elimination of weak synapses.

#### Pruning Equations
```
probability_of_pruning = rate * weight_magnitude
```

#### Pruning Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| pruning_rate | float | 0.0001 | Pruning rate |

#### Pruning Implementation
```cpp
class PruningPlasticity : public StructuralPlasticity {
public:
    bool shouldRemoveSynapse(float preNeuron, float postNeuron,
                            float currentWeight, const Brain& brain) override {
        // Check weight threshold
        if (currentWeight < weight_threshold) {
            // Check pruning probability
            if (rng.random() < pruning_probability * currentWeight) {
                // Remove synapse
                return true;
            }
        }
        return false;
    }
    
    std::string getName() const override { return "Pruning"; }
    
private:
    const float weight_threshold = 0.1;
    const float pruning_rate = 0.0001;
    float pruning_probability = pruning_rate;
};
```

## Neuromodulation Models

### 1. Dopamine Neuromodulator
Dopamine modulates synaptic plasticity and learning.

#### Dopamine Equations
```
dopamine_level = baseline + reward_signal * modulation_strength
```

#### Dopamine Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| baseline | float | 0.0 | Baseline dopamine level |
| modulation_strength | float | 0.1 | Modulation strength |

#### Dopamine Implementation
```cpp
class DopamineNeuromodulator : public Neuromodulator {
public:
    float computeModulation(float reward, float prediction_error) override {
        // Compute dopamine level
        float dopamine = baseline + reward * modulation_strength;
        
        // Apply prediction error
        dopamine *= (1.0 + prediction_error * prediction_error_modulation);
        
        return dopamine;
    }
    
    std::string getName() const override { return "Dopamine"; }
    
private:
    const float baseline = 0.0;
    const float modulation_strength = 0.1;
    const float prediction_error_modulation = 0.05;
};
```

### 2. Curiosity Neuromodulator
Curiosity drives exploration and novelty detection.

#### Curiosity Equations
```
curiosity_level = novelty_detection * exploration_strength
```

#### Curiosity Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| novelty_detection | float | 0.0 | Novelty detection signal |
| exploration_strength | float | 0.5 | Exploration strength |

#### Curiosity Implementation
```cpp
class CuriosityNeuromodulator : public Neuromodulator {
public:
    float computeModulation(float novelty, float uncertainty) override {
        // Compute curiosity level
        float curiosity = novelty * exploration_strength;
        
        // Apply uncertainty
        curiosity *= (1.0 + uncertainty * uncertainty_modulation);
        
        return curiosity;
    }
    
    std::string getName() const override { return "Curiosity"; }
    
private:
    const float exploration_strength = 0.5;
    const float uncertainty_modulation = 0.1;
};
```

## Memory Models

### 1. Working Memory
Working memory stores temporary information for immediate processing.

#### Working Memory Equations
```
activation_level = input * recurrent_connections
memory_capacity = number_of_stores
```

#### Working Memory Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| capacity | int | 100 | Working memory capacity |
| decay_rate | float | 0.01 | Memory decay rate |

#### Working Memory Implementation
```cpp
class NeuralWorkingMemory : public MemorySystem {
public:
    void store(float neural_activity, float time) override {
        // Check capacity
        if (active_traces.size() >= capacity) {
            // Remove oldest trace
            active_traces.pop_front();
        }
        
        // Store new trace
        active_traces.push_back({neural_activity, time});
    }
    
    float retrieve(float time_window) override {
        // Calculate average activation
        float total_activation = 0.0;
        int count = 0;
        
        for (const auto& trace : active_traces) {
            if (time - trace.time <= time_window) {
                total_activation += trace.activation;
                count++;
            }
        }
        
        return count > 0 ? total_activation / count : 0.0;
    }
    
    std::string getName() const override { return "NeuralWorkingMemory"; }
    
private:
    const int capacity = 100;
    const float decay_rate = 0.01;
    
    struct MemoryTrace {
        float activation;
        float time;
    };
    
    std::deque<MemoryTrace> active_traces;
};
```

### 2. Episodic Memory
Episodic memory stores experiences with temporal and spatial context.

#### Episodic Memory Equations
```
episode_id = unique_identifier
temporal_context = time_of_occurrence
spatial_context = location_at_time
```

#### Episodic Memory Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| max_episodes | int | 1000 | Maximum number of episodes |
| retrieval_window | float | 10.0 | Episode retrieval window |

#### Episodic Memory Implementation
```cpp
class NeuralEpisodicMemory : public MemorySystem {
public:
    void store(float neural_pattern, float time, float spatial_context) override {
        // Create new episode
        Episode episode;
        episode.id = next_episode_id++;
        episode.pattern = neural_pattern;
        episode.temporal_context = time;
        episode.spatial_context = spatial_context;
        
        // Store episode
        if (episodes.size() >= max_episodes) {
            // Remove oldest episode
            episodes.erase(episodes.begin());
        }
        episodes.push_back(episode);
    }
    
    float retrieve(float time_window, float spatial_context) override {
        // Retrieve similar episodes
        float total_similarity = 0.0;
        float total_weight = 0.0;
        
        for (const auto& episode : episodes) {
            if (std::abs(episode.temporal_context - (current_time - time_window)) <= time_window &&
                episode.spatial_context == spatial_context) {
                
                float similarity = calculateSimilarity(episode.pattern, current_pattern);
                total_similarity += similarity * episode.weight;
                total_weight += episode.weight;
            }
        }
        
        return total_weight > 0 ? total_similarity / total_weight : 0.0;
    }
    
    std::string getName() const override { return "NeuralEpisodicMemory"; }
    
private:
    const int max_episodes = 1000;
    const float retrieval_window = 10.0;
    
    struct Episode {
        int id;
        float pattern;
        float temporal_context;
        float spatial_context;
        float weight;
    };
    
    std::vector<Episode> episodes;
    int next_episode_id = 0;
    float current_time = 0.0;
    float current_pattern = 0.0;
};