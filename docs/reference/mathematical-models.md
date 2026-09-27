# Mathematical Models

## Overview

NLM implements several mathematical models inspired by biological neuroscience. This document provides detailed mathematical formulations, parameter ranges, and implementation details for all mathematical components in the system.

## 1. Leaky Integrate-and-Fire (LIF) Neuron Model

### Mathematical Formulation

The LIF neuron model is described by the differential equation:

```
dv/dt = (-v + R * I) / τ
```

Where:
- `v` = membrane potential (mV)
- `I` = total synaptic current (nA)
- `R` = membrane resistance (MΩ)
- `τ` = time constant (ms)

### Complete LIF Model with Refractory Period

```
dv/dt = (-v + R * I) / τ

if v ≥ V_threshold:
    spike = true
    v = V_reset
    t_refractory = τ_ref

if t_refractory > 0:
    dv/dt = 0
    t_refractory -= dt
```

### Parameters

| Parameter | Symbol | Description | Typical Value | Range | Units |
|-----------|--------|-------------|---------------|-------|-------|
| Membrane Capacitance | C | Membrane capacitance | 1.0 | [0.1, 10] | nF |
| Time Constant | τ | Integration time constant | 20.0 | [1, 100] | ms |
| Membrane Resistance | R | Membrane resistance | 10.0 | [1, 100] | MΩ |
| Threshold | V_th | Spike threshold | -55.0 | [-70, -40] | mV |
| Reset Potential | V_reset | Reset after spike | -70.0 | [-80, -60] | mV |
| Refractory Period | τ_ref | Absolute refractory period | 2.0 | [0.5, 10] | ms |

### Numerical Implementation

```cpp
class LIFNeuron {
public:
    struct Parameters {
        float C = 1.0f;      // Membrane capacitance (nF)
        float tau = 20.0f;   // Time constant (ms)
        float R = 10.0f;     // Membrane resistance (MΩ)
        float V_th = -55.0f; // Threshold (mV)
        float V_reset = -70.0f; // Reset potential (mV)
        float tau_ref = 2.0f;   // Refractory period (ms)
    };
    
    struct State {
        float v = 0.0f;           // Membrane potential (mV)
        float refractoryRemaining = 0.0f; // Remaining refractory time (ms)
        bool spiked = false;      // Spike flag
        Timestamp lastSpike = 0.0; // Last spike time (ms)
    };
    
    void update(float I_syn, float dt) {
        // Update membrane potential
        if (refractoryRemaining > 0) {
            // During refractory period: no integration
            refractoryRemaining -= dt;
        } else {
            // LIF dynamics
            float dv = (-state.v + R * I_syn) / tau;
            state.v += dv * dt;
            
            // Check for threshold crossing
            if (state.v >= V_th) {
                state.spiked = true;
                state.v = V_reset;
                state.refractoryRemaining = tau_ref;
                state.lastSpike = currentTime;
            }
        }
    }
};
```

## 2. Spike-Timing-Dependent Plasticity (STDP)

### Mathematical Formulation

STDP is based on the timing difference between pre-synaptic and post-synaptic spikes:

```
Δw = {
    +A_+ * exp(-(t_post - t_pre) / τ_+),  if t_post > t_pre
    -A_- * exp(-(t_pre - t_post) / τ_-), if t_pre > t_post
    0,                                     otherwise
}
```

Where:
- `t_pre` = pre-synaptic spike time
- `t_post` = post-synaptic spike time
- `Δt` = `t_post - t_pre`

### Implementation with Trace-based Approach

```cpp
dw/dt = -w/τ_w + ∑ δ(t - t_pre) - ∑ δ(t - t_post)
w(t) = w(0) + A_+ * trace_pre(t) - A_- * trace_post(t)
```

### Parameters

| Parameter | Symbol | Description | Typical Value | Range |
|-----------|--------|-------------|---------------|-------|
| LTP Magnitude | A_+ | Long-term potentiation magnitude | 0.01 | [0.001, 0.1] |
| LTD Magnitude | A_- | Long-term depression magnitude | 0.012 | [0.001, 0.1] |
| Time Constant | τ | STDP time window | 20.0 | [5, 50] |
| Trace Decay | τ_w | Trace decay time constant | 100.0 | [10, 1000] |
| Learning Rate | α | Overall learning rate | 0.001 | [0.0001, 0.01] |

### Implementation Details

```cpp
class STDPPlasticity {
public:
    struct Parameters {
        float A_plus = 0.01f;    // LTP magnitude
        float A_minus = 0.012f;  // LTD magnitude
        float tau_plus = 20.0f;  // Positive time constant (ms)
        float tau_minus = 20.0f; // Negative time constant (ms)
        float tau_trace = 100.0f; // Trace decay (ms)
    };
    
    void configure(const Parameters& params) {
        params_ = params;
    }
    
    float calculateWeightChange(float preTime, float postTime, float currentTime) {
        if (std::abs(preTime - postTime) < 1e-6) return 0.0f;
        
        float delta_t = postTime - preTime;
        
        if (delta_t > 0) {
            // Post after pre: potentiation
            return A_plus * std::exp(-delta_t / tau_plus);
        } else {
            // Pre after post: depression
            return -A_minus * std::exp(delta_t / tau_minus);
        }
    }
    
    // Trace-based implementation for continuous learning
    class TraceBasedSTDP {
    public:
        void addPreTrace(float currentTime) {
            preTrace = 1.0f;
            preTraceTime = currentTime;
        }
        
        void addPostTrace(float currentTime) {
            postTrace = 1.0f;
            postTraceTime = currentTime;
        }
        
        void updateTraces(float currentTime) {
            // Decay traces
            preTrace *= std::exp(-(currentTime - preTraceTime) / tau_trace);
            postTrace *= std::exp(-(currentTime - postTraceTime) / tau_trace);
        }
        
        float getWeightChange() const {
            return A_plus * postTrace - A_minus * preTrace;
        }
        
    private:
        float preTrace = 0.0f;
        float postTrace = 0.0f;
        float preTraceTime = 0.0f;
        float postTraceTime = 0.0f;
    };
};
```

## 3. Hebbian Learning

### Mathematical Formulation

Hebbian learning follows the principle: "neurons that fire together, wire together":

```
Δw_ij = η * x_i * x_j
```

Where:
- `η` = learning rate
- `x_i` = pre-synaptic activity
- `x_j` = post-synaptic activity

### Implementation

```cpp
class HebbianPlasticity {
public:
    struct Parameters {
        float learningRate = 0.001f;    // Learning rate
        float correlationThreshold = 0.1f; // Activity correlation threshold
        float minWeight = 0.0f;        // Minimum weight
        float maxWeight = 1.0f;        // Maximum weight
    };
    
    float calculateWeightChange(float preActivity, float postActivity,
                                float currentWeight) {
        float delta = learningRate * preActivity * postActivity;
        
        // Constrain weight changes
        float newWeight = currentWeight + delta;
        newWeight = std::max(minWeight, std::min(maxWeight, newWeight));
        
        return newWeight - currentWeight;
    }
};
```

## 4. Neuromodulation Models

### 4.1 Dopamine Modulation

**Reward Prediction Error (RPE)**

```
dδ/dt = -δ/τ_δ + (r - V(s)) * γ
```

Where:
- `δ` = prediction error
- `r` = received reward
- `V(s)` = predicted value of state `s`
- `γ` = learning rate

**Dopamine Signal**

```
DA(t) = α_DA * δ(t) * exp(-t / τ_DA)
```

### 4.2 Curiosity Modulation

**Novelty Detection**

```
n(t) = ||x(t) - μ(t)||² / σ²(t)
```

Where:
- `x(t)` = current sensory input
- `μ(t)` = predicted sensory input
- `σ²(t)` = prediction uncertainty

**Curiosity Signal**

```
C(t) = β_curiosity * (n(t) + λ * |δ(t)|)
```

### 4.3 Prediction Error

**Temporal Difference Learning**

```
dδ/dt = -δ/τ_δ + (r + γ * V(s') - V(s))
```

Where:
- `r` = immediate reward
- `γ` = discount factor
- `V(s)` = value of current state
- `V(s')` = value of next state

## 5. Memory Models

### 5.1 Working Memory (Persistent Activity)

**Attractor Network Dynamics**

```
dv_i/dt = (-v_i + Σ_j w_ij * σ(v_j)) / τ
```

Where:
- `σ(v) = 1 / (1 + exp(-β(v - θ)))` is the activation function
- `β` controls gain
- `θ` controls threshold

**Stability Analysis**

For stability, the largest eigenvalue of the weight matrix must satisfy:

```
ρ(W * J) < 1
```

Where:
- `ρ` = spectral radius
- `J` = Jacobian of activation function

### 5.2 Episodic Memory (Neural Pattern Storage)

**Memory Encoding**

```
E = H(x) = Σ_i ||x - c_i||²
```

Where:
- `H` = encoding function
- `c_i` = prototype vectors
- `x` = input pattern

**Pattern Completion**

```
R(x) = arg min_{i} ||x - c_i||²
```

**Replay Dynamics**

```
dp/dt = -(p - p₀)/τ_p + η * δ * replay(t)
```

### 5.3 Associative Memory

**Hopfield Network Model**

```
Energy function: E = -½ Σ_{i,j} w_ij * x_i * x_j + Σ_i θ_i * x_i
```

**Update Rule**

```
x_i(t+1) = σ(Σ_j w_ij * x_j(t) - θ_i)
```

## 6. Development Models

### 6.1 Developmental Plasticity

**Plasticity Schedule**

```
p(age) = p_adult + (p_initial - p_adult) * exp(-age / τ_age)
```

Where:
- `p(adult)` = adult plasticity level
- `p(initial)` = initial plasticity level
- `τ_age` = developmental time constant

### 6.2 Critical Period

**Critical Period Dynamics**

```
p_c(age) = {
    0,                           age < age_start
    exp(-(age - age_start) / τ_c), age_start ≤ age < age_end
    0,                           age ≥ age_end
}
```

### 6.3 Synaptogenesis

**Probabilistic Synapse Formation**

```
w_{ij}(t) = {
    random.uniform(0, 1), if learning and connectivity_possible
    0,                     otherwise
}
```

### 6.4 Pruning

**Activity-Dependent Pruning**

```
prune_{ij} = {
    1, if ||activity_i|| < θ_prune and weight_{ij} < θ_weak
    0, otherwise
}
```

## 7. Prediction and Cognitive Models

### 7.1 Prediction System

**Temporal Sequence Learning**

```
p(y|x) = Σ_{t} α_t * similarity(x_t, y_t)
```

**Multi-step Prediction**

```
p(y|x, T) = Σ_{k=1}^{T} β_k * p(y_t+k|x, t)
```

### 7.2 Attention Mechanism

**Competitive Selection**

```
alpha_i = exp(β_i) / Σ_j exp(β_j)
β_i = w_i^T * x + b_i
```

**Lateral Inhibition**

```
beta_i = x_i - λ * Σ_j w_ij * x_j
```

### 7.3 Planning

**Value Function Approximation**

```
V(s) = w^T * φ(s)
```

**Q-learning**

```
Q(s,a) ← Q(s,a) + α * (r + γ * max_a' Q(s',a') - Q(s,a))
```

## 8. Performance Models

### 8.1 Computational Complexity

**Time Complexity**

| Operation | Time Complexity | Description |
|-----------|-----------------|-------------|
| Neuron Update | O(1) | Constant per neuron |
| Synapse Processing | O(1) | Constant per synapse |
| Spike Event Handling | O(log N) | Priority queue operations |
| Memory Storage | O(log N) | Tree-based indexing |
| Pattern Matching | O(log N) | KD-tree search |

**Space Complexity**

```
Space = N_neuron * S_neuron + N_synapse * S_synapse + 
        N_memory * S_memory + N_patterns * S_pattern
```

### 8.2 Performance Metrics

**Throughput**

```
throughput = spikes_per_second = N_spikes / T_total
```

**Latency**

```
latency = t_response - t_stimulus
```

**Memory Efficiency**

```
efficiency = useful_data / total_memory = (N_patterns * S_pattern) / memory_usage
```

**Scalability**

```
scalability = (performance_N / performance_1) / (N / 1)
```

## 9. Parameter Sensitivity Analysis

### 9.1 LIF Parameters

**Sensitivity to Time Constant (τ)**

```
∂firing_rate/∂τ ≈ -firing_rate / τ
```

**Sensitivity to Threshold (V_th)**

```
∂firing_rate/∂V_th ≈ -β * exp(-β * (V_th - V_rest))
```

### 9.2 STDP Parameters

**Sensitivity to LTP Magnitude (A_+)**

```
∂weight_change/∂A_+ ≈ exp(-(t_post - t_pre) / τ_+)
```

**Sensitivity to Learning Rate (α)**

```
∂weight_change/∂α ≈ t_pre * t_post * exp(-|t_post - t_pre| / τ)
```

### 9.3 Memory Parameters

**Sensitivity to Working Memory Capacity**

```
∂retrieval_success/∂capacity ≈ 1 / capacity^2
```

**Sensitivity to Forgetting Rate**

```
∂memory_retention/∂forgetting_rate ≈ -t * exp(-forgetting_rate * t)
```

## 10. Stability Analysis

### 10.1 Network Stability

**Neural Network Stability**

For stability of the LIF network, the spectral radius of the effective connectivity matrix must satisfy:

```
ρ(W_eff) < 1
```

Where:
- `W_eff = J * W` is the effective connectivity matrix
- `J` is the Jacobian of the activation function
- `W` is the synaptic weight matrix

### 10.2 Plasticity Stability

**Plasticity Convergence**

For stable plasticity, the learning rate must satisfy:

```
α < 2 / (λ_max(W^T W))
```

Where:
- `λ_max` is the maximum eigenvalue

### 10.3 Memory Stability

**Episodic Memory Stability**

For stable episodic memory, the replay rate must satisfy:

```
replay_rate < 2 / (τ_consolidation + τ_decay)
```

## 11. Numerical Methods

### 11.1 Integration Methods

**Euler Method**

```
x_{n+1} = x_n + f(x_n, t_n) * Δt
y_{n+1} = y_n + g(x_n, t_n) * Δt
```

**Runge-Kutta 4th Order**

```
k1 = f(x_n, t_n)
k2 = f(x_n + 0.5*k1*Δt, t_n + 0.5*Δt)
k3 = f(x_n + 0.5*k2*Δt, t_n + 0.5*Δt)
k4 = f(x_n + k3*Δt, t_n + Δt)

x_{n+1} = x_n + (k1 + 2*k2 + 2*k3 + k4) * Δt / 6
```

### 11.2 Sparse Matrix Methods

**Compressed Sparse Row (CSR)**

```cpp
struct CSRMatrix {
    std::vector<float> values;
    std::vector<int> columnIndices;
    std::vector<int> rowPointers;
    
    void multiply(const std::vector<float>& x, std::vector<float>& y) {
        for (int i = 0; i < numRows; ++i) {
            float sum = 0.0f;
            for (int j = rowPointers[i]; j < rowPointers[i+1]; ++j) {
                sum += values[j] * x[columnIndices[j]];
            }
            y[i] = sum;
        }
    }
};
```

### 11.3 Parallel Processing

**Domain Decomposition**

```cpp
void parallelNeuronUpdate(const std::vector<Neuron>& neurons, 
                          std::vector<Neuron>& results,
                          int numThreads) {
    std::vector<std::thread> threads;
    int neuronsPerThread = neurons.size() / numThreads;
    
    for (int i = 0; i < numThreads; ++i) {
        int start = i * neuronsPerThread;
        int end = (i == numThreads - 1) ? neurons.size() : start + neuronsPerThread;
        
        threads.emplace_back([&, start, end]() {
            for (int j = start; j < end; ++j) {
                results[j] = neurons[j].update();
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}
```

## References

1. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neuronal Dynamics: From Single Neurons to Neural Networks. Cambridge University Press.
3. Morimoto, J., & Omura, Y. (2002). Neural Networks for Control and Robot Applications. Springer.
4. Song, S., Miller, K. D., & Abbott, L. F. (2000). Computational principles of network plasticity: empirical and theoretical results. Neural Networks, 13(4-5), 459-474.
5. Seung, H. S., & Miller, K. D. (2000). Equilibrium effects of synaptic coding in recurrent networks. Neural Computation, 12(7), 1393-1410.

## License

Mathematical models and formulas are licensed under the MIT License.

## Copyright

© 2026 NLM Development Team. All rights reserved.