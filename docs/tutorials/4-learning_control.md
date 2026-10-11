# Tutorial 4: Learning and Plasticity Control

## Overview

This tutorial provides a comprehensive guide to learning and plasticity systems in NLM. It covers learning rules, plasticity mechanisms, learning algorithms, and how to control and optimize the learning process in NLM brain networks.

## Learning in NLM

### What is Learning in NLM?

Learning in NLM is the process by which the brain network modifies its structure and function based on experience. This involves:

- **Synaptic Plasticity:** Changes in the strength of connections between neurons
- **Structural Plasticity:** Changes in the physical structure of neural networks
- **Learning Rules:** Algorithms that determine how connections should be modified
- **Memory Consolidation:** Stabilization of learned information over time

### Learning Architecture in NLM

```
Learning System
├── Learning Rules
│   ├── STDP (Spike-Timing-Dependent Plasticity)
│   ├── Hebbian Learning
│   ├── Anti-Hebbian Learning
│   ├── Homeostatic Plasticity
│   └── Neuromodulator-Gated Plasticity
├── Plasticity Controllers
│   ├── Synaptic Plasticity Controller
│   ├── Structural Plasticity Controller
│   ├── Neuronal Plasticity Controller
│   └── Meta-Learning Controller
├── Learning Scheduler
│   ├── Time-Based Scheduling
│   ├── Performance-Based Scheduling
│   ├── Event-Based Scheduling
│   └── Adaptive Scheduling
├── Reward Processing
│   ├── Prediction Error Calculation
│   ├── Reward Prediction
│   ├── Value Learning
│   └── Policy Optimization
└── Memory Integration
    ├── Working Memory Learning
    ├── Episodic Memory Consolidation
    ├── Semantic Memory Integration
    └── Forgetting Mechanisms
```

## Learning Rules

### 1. Spike-Timing-Dependent Plasticity (STDP)

STDP is one of the most well-studied learning rules in neuroscience. It strengthens connections when presynaptic spikes precede postsynaptic spikes, and weakens connections when the opposite occurs.

#### STDP Mathematical Model

```python
import numpy as np

class STDPPlasticity:
    def __init__(self, learning_rate=0.01, time_window=20, tau=20.0):
        """
        Initialize STDP plasticity.
        
        Parameters:
        -----------
        learning_rate : float
            Learning rate for STDP (0.0 to 1.0)
        time_window : int
            Time window for STDP (in milliseconds)
        tau : float
            Time constant for STDP
        """
        self.learning_rate = learning_rate
        self.time_window = time_window
        self.tau = tau
        
        # STDP parameters
        self.LTP_weight = 0.02      # Long-Term Potentiation weight
        self.LTD_weight = 0.015     # Long-Term Depression weight
        
        # Trace variables
        self.pre_trace = 0.0
        self.post_trace = 0.0
    
    def update(self, pre_spike, post_spike, current_time):
        """
        Update synaptic weights based on spike timing.
        
        Parameters:
        -----------
        pre_spike : bool
            Whether presynaptic neuron spiked
        post_spike : bool
            Whether postsynaptic neuron spiked
        current_time : float
            Current time in simulation
        
        Returns:
        --------
        float
            Change in synaptic weight
        """
        # Update traces
        self.pre_trace *= np.exp(-current_time / self.tau)
        self.post_trace *= np.exp(-current_time / self.tau)
        
        # Add spikes to traces
        if pre_spike:
            self.pre_trace += 1.0
        if post_spike:
            self.post_trace += 1.0
        
        # Calculate weight change
        if post_spike and pre_spike:
            # Post before pre (LTP)
            delta_weight = self.LTP_weight * self.post_trace * self.pre_trace
        elif pre_spike and post_spike:
            # Pre before post (LTD)
            delta_weight = -self.LTD_weight * self.post_trace * self.pre_trace
        else:
            delta_weight = 0.0
        
        # Apply weight change
        return delta_weight * self.learning_rate
    
    def reset(self):
        """Reset traces to zero."""
        self.pre_trace = 0.0
        self.post_trace = 0.0

# Example usage of STDP
if __name__ == "__main__":
    # Create STDP plasticity
    stdp = STDPPlasticity()
    
    # Simulate spike timing
    time = 0
    
    # Pre-synaptic spike
    stdp.update(pre_spike=True, post_spike=False, current_time=time)
    print(f"Time {time}: Pre spike, Post no spike")
    print(f"  Pre trace: {stdp.pre_trace:.3f}, Post trace: {stdp.post_trace:.3f}")
    
    # Post-synaptic spike (should strengthen connection)
    time += 5
    stdp.update(pre_spike=False, post_spike=True, current_time=time)
    print(f"Time {time}: Pre no spike, Post spike")
    print(f"  Pre trace: {stdp.pre_trace:.3f}, Post trace: {stdp.post_trace:.3f}")
    
    # Strong LTP: Post spike immediately after pre spike
    time += 5
    stdp.update(pre_spike=True, post_spike=False, current_time=time)
    time += 2
    stdp.update(pre_spike=False, post_spike=True, current_time=time)
    print(f"Time {time}: Strong LTP (Post spike after Pre spike)")
    print(f"  Pre trace: {stdp.pre_trace:.3f}, Post trace: {stdp.post_trace:.3f}")
    
    # Strong LTD: Pre spike immediately after post spike
    time += 5
    stdp.update(pre_spike=False, post_spike=True, current_time=time)
    time += 2
    stdp.update(pre_spike=True, post_spike=False, current_time=time)
    print(f"Time {time}: Strong LTD (Pre spike after Post spike)")
    print(f"  Pre trace: {stdp.pre_trace:.3f}, Post trace: {stdp.post_trace:.3f}")
```

### 2. Hebbian Learning

Hebbian learning is based on Donald Hebb's principle: "neurons that fire together, wire together."

#### Hebbian Learning Model

```python
import numpy as np

class HebbianPlasticity:
    def __init__(self, learning_rate=0.01, correlation_window=10):
        """
        Initialize Hebbian plasticity.
        
        Parameters:
        -----------
        learning_rate : float
            Learning rate for Hebbian learning (0.0 to 1.0)
        correlation_window : int
            Time window for correlation (in milliseconds)
        """
        self.learning_rate = learning_rate
        self.correlation_window = correlation_window
        
        # Correlation traces
        self.pre_correlation = 0.0
        self.post_correlation = 0.0
        
        # Historical correlations
        self.correlation_history = []
    
    def update(self, pre_spike, post_spike, current_time):
        """
        Update synaptic weights based on correlation.
        
        Parameters:
        -----------
        pre_spike : bool
            Whether presynaptic neuron spiked
        post_spike : bool
            Whether postsynaptic neuron spiked
        current_time : float
            Current time in simulation
        
        Returns:
        --------
        float
            Change in synaptic weight
        """
        # Update correlations
        self.pre_correlation *= np.exp(-current_time / self.correlation_window)
        self.post_correlation *= np.exp(-current_time / self.correlation_window)
        
        # Add spikes to correlations
        if pre_spike:
            self.pre_correlation += 1.0
        if post_spike:
            self.post_correlation += 1.0
        
        # Calculate Hebbian weight change
        if pre_spike and post_spike:
            # Coincident spikes strengthen connection
            delta_weight = self.learning_rate * self.pre_correlation * self.post_correlation
        else:
            delta_weight = 0.0
        
        # Store correlation history
        self.correlation_history.append({
            'time': current_time,
            'pre_correlation': self.pre_correlation,
            'post_correlation': self.post_correlation,
            'delta_weight': delta_weight
        })
        
        # Apply weight change
        return delta_weight
    
    def get_correlation_strength(self, time_window=1000):
        """
        Get correlation strength over a time window.
        
        Parameters:
        -----------
        time_window : int
            Time window for correlation (in milliseconds)
        
        Returns:
        --------
        float
            Average correlation strength
        """
        if not self.correlation_history:
            return 0.0
        
        # Filter recent correlations
        current_time = self.correlation_history[-1]['time']
        recent_correlations = [
            c for c in self.correlation_history
            if current_time - c['time'] <= time_window
        ]
        
        if not recent_correlations:
            return 0.0
        
        # Calculate average correlation
        avg_pre = np.mean([c['pre_correlation'] for c in recent_correlations])
        avg_post = np.mean([c['post_correlation'] for c in recent_correlations])
        
        return avg_pre * avg_post
    
    def reset(self):
        """Reset correlations to zero."""
        self.pre_correlation = 0.0
        self.post_correlation = 0.0
        self.correlation_history = []

# Example usage of Hebbian learning
if __name__ == "__main__":
    # Create Hebbian plasticity
    hebbian = HebbianPlasticity()
    
    # Simulate correlated spikes
    time = 0
    
    # Correlated spikes (strengthen connection)
    for i in range(5):
        hebbian.update(pre_spike=True, post_spike=True, current_time=time + i * 10)
        correlation = hebbian.get_correlation_strength(100)
        print(f"Time {time + i * 10}: Correlated spikes, Correlation: {correlation:.3f}")
    
    # Uncorrelated spikes (no change)
    time += 50
    hebbian.update(pre_spike=True, post_spike=False, current_time=time)
    hebbian.update(pre_spike=False, post_spike=True, current_time=time + 5)
    correlation = hebbian.get_correlation_strength(100)
    print(f"Time {time + 5}: Uncorrelated spikes, Correlation: {correlation:.3f}")
```

### 3. Homeostatic Plasticity

Homeostatic plasticity maintains stability in neural networks by regulating overall activity levels.

#### Homeostatic Plasticity Model

```python
import numpy as np

class HomeostaticPlasticity:
    def __init__(self, target_activity=0.8, learning_rate=0.001, timescale=1000):
        """
        Initialize homeostatic plasticity.
        
        Parameters:
        -----------n        target_activity : float
            Target firing rate (0.0 to 1.0)
        learning_rate : float
            Learning rate for homeostasis (0.0 to 1.0)
        timescale : float
            Timescale for homeostatic regulation (in milliseconds)
        """
        self.target_activity = target_activity
        self.learning_rate = learning_rate
        self.timescale = timescale
        
        # Homeostatic variables
        self.average_activity = 0.0
        self.activity_trace = 0.0
        
        # Synaptic weights
        self.synaptic_weights = {}
    
    def update_activity(self, current_activity, time_step):
        """
        Update average activity with exponential moving average.
        
        Parameters:
        -----------
        current_activity : float
            Current firing rate (0.0 to 1.0)
        time_step : float
            Time step for update
        """
        # Update average activity with exponential moving average
        self.activity_trace = self.activity_trace * np.exp(-time_step / self.timescale) + \
                             current_activity * (1 - np.exp(-time_step / self.timescale))
        self.average_activity = self.activity_trace
    
    def adjust_weights(self, neuron_id, current_weight, time_step):
        """
        Adjust synaptic weight to maintain target activity.
        
        Parameters:
        -----------
        neuron_id : str
            Identifier of the neuron
        current_weight : float
            Current synaptic weight
        time_step : float
            Time step for adjustment
        
        Returns:
        --------
        float
            Adjusted synaptic weight
        """
        # Calculate error from target activity
        activity_error = self.average_activity - self.target_activity
        
        # Apply homeostatic adjustment
        adjustment = self.learning_rate * activity_error * current_weight * time_step
        
        # New weight
        new_weight = current_weight + adjustment
        
        # Constrain weight to reasonable range
        new_weight = np.clip(new_weight, 0.0, 1.0)
        
        # Store weight
        self.synaptic_weights[neuron_id] = new_weight
        
        return new_weight
    
    def get_weight_adjustment(self, neuron_id, time_step):
        """
        Get weight adjustment for a neuron.
        
        Parameters:
        -----------
        neuron_id : str
            Identifier of the neuron
        time_step : float
            Time step for adjustment
        
        Returns:
        --------
        float
            Weight adjustment amount
        """
        current_weight = self.synaptic_weights.get(neuron_id, 0.5)
        new_weight = self.adjust_weights(neuron_id, current_weight, time_step)
        
        return new_weight - current_weight
    
    def reset(self):
        """Reset homeostatic variables."""
        self.average_activity = 0.0
        self.activity_trace = 0.0
        self.synaptic_weights.clear()

# Example usage of homeostatic plasticity
if __name__ == "__main__":
    # Create homeostatic plasticity
    homeostatic = HomeostaticPlasticity(target_activity=0.8, learning_rate=0.001)
    
    # Simulate activity dynamics
    time = 0
    for step in range(100):
        # Simulate varying activity
        if step < 20:
            current_activity = 0.2  # Low activity
        elif step < 60:
            current_activity = 0.8  # Target activity
        else:
            current_activity = 1.5  # High activity
        
        # Update activity
        homeostatic.update_activity(current_activity, 1.0)
        
        # Adjust weights for a neuron
        weight = homeostatic.synaptic_weights.get("neuron1", 0.5)
        adjustment = homeostatic.get_weight_adjustment("neuron1", 1.0)
        new_weight = weight + adjustment
        
        if step % 20 == 0:
            print(f"Step {step}: Activity={current_activity:.2f}, Average={homeostatic.average_activity:.3f}")
            print(f"  Weight adjustment: {adjustment:+.4f}, New weight: {new_weight:.3f}")
```

### 4. Neuromodulator-Gated Plasticity

Neuromodulators like dopamine and serotonin regulate plasticity based on behavioral context.

#### Neuromodulator-Gated Plasticity Model

```python
import numpy as np

class NeuromodulatorGatedPlasticity:
    def __init__(self, neuromodulator_strength=1.0, baseline_plasticity=0.01):
        """
        Initialize neuromodulator-gated plasticity.
        
        Parameters:
        -----------
        neuromodulator_strength : float
            Strength of neuromodulator influence (0.0 to 1.0)
        baseline_plasticity : float
            Baseline plasticity rate (0.0 to 1.0)
        """
        self.neuromodulator_strength = neuromodulator_strength
        self.baseline_plasticity = baseline_plasticity
        
        # Neuromodulator levels
        self.dopamine_level = 0.0
        self.serotonin_level = 0.0
        self.noradrenaline_level = 0.0
        
        # Plasticity modifiers
        self.dopamine_modifier = 1.0
        self.serotonin_modifier = 1.0
        self.noradrenaline_modifier = 1.0
    
    def set_neuromodulator_levels(self, dopamine, serotonin, noradrenaline):
        """
        Set neuromodulator levels.
        
        Parameters:
        -----------
        dopamine : float
            Dopamine level (0.0 to 1.0)
        serotonin : float
            Serotonin level (0.0 to 1.0)
        noradrenaline : float
            Noradrenaline level (0.0 to 1.0)
        """
        self.dopamine_level = np.clip(dopamine, 0.0, 1.0)
        self.serotonin_level = np.clip(serotonin, 0.0, 1.0)
        self.noradrenaline_level = np.clip(noradrenaline, 0.0, 1.0)
        
        # Update plasticity modifiers
        self._update_plasticity_modifiers()
    
    def _update_plasticity_modifiers(self):
        """Update plasticity modifiers based on neuromodulator levels."""
        # Dopamine increases plasticity for reward prediction errors
        self.dopamine_modifier = 1.0 + self.dopamine_level * 2.0
        
        # Serotonin decreases plasticity for safety
        self.serotonin_modifier = max(0.1, 1.0 - self.serotonin_level * 0.5)
        
        # Noradrenaline increases plasticity for novelty detection
        self.noradrenaline_modifier = 1.0 + self.noradrenaline_level * 1.5
    
    def calculate_plasticity_modulator(self, spike_pair_type):
        """
        Calculate plasticity modifier based on spike pair type.
        
        Parameters:
        -----------
        spike_pair_type : str
            Type of spike pair ("LTP", "LTD", "coincident")
        
        Returns:
        --------
        float
            Plasticity modifier
        """
        if spike_pair_type == "LTP":
            # LTP enhanced by dopamine, decreased by serotonin
            modifier = self.dopamine_modifier * self.serotonin_modifier
        elif spike_pair_type == "LTD":
            # LTD increased by serotonin, decreased by dopamine
            modifier = (1.0 / self.dopamine_modifier) * self.serotonin_modifier
        elif spike_pair_type == "coincident":
            # Coincident spikes modulated by noradrenaline
            modifier = self.noradrenaline_modifier
        else:
            modifier = 1.0  # Baseline
        
        return modifier
    
    def calculate_weight_change(self, pre_spike, post_spike, base_learning_rate, spike_pair_type):
        """
        Calculate synaptic weight change with neuromodulator gating.
        
        Parameters:
        -----------
        pre_spike : bool
            Whether presynaptic neuron spiked
        post_spike : bool
            Whether postsynaptic neuron spiked
        base_learning_rate : float
            Base learning rate
        spike_pair_type : str
            Type of spike pair
        
        Returns:
        --------
        float
            Weight change
        """
        # Calculate plasticity modifier
        modifier = self.calculate_plasticity_modulator(spike_pair_type)
        
        # Calculate base weight change
        if spike_pair_type == "LTP":
            if pre_spike and post_spike:
                weight_change = base_learning_rate * modifier
            else:
                weight_change = 0.0
        elif spike_pair_type == "LTD":
            if post_spike and pre_spike:
                weight_change = -base_learning_rate * modifier
            else:
                weight_change = 0.0
        elif spike_pair_type == "coincident":
            if pre_spike and post_spike:
                weight_change = base_learning_rate * modifier
            else:
                weight_change = 0.0
        else:
            weight_change = 0.0
        
        return weight_change
    
    def get_plasticity_summary(self):
        """Get summary of plasticity modifiers."""
        return {
            "dopamine_modifier": self.dopamine_modifier,
            "serotonin_modifier": self.serotonin_modifier,
            "noradrenaline_modifier": self.noradrenaline_modifier,
            "overall_plasticity": (self.dopamine_modifier * self.serotonin_modifier * self.noradrenaline_modifier) ** (1/3)
        }

# Example usage of neuromodulator-gated plasticity
if __name__ == "__main__":
    # Create neuromodulator-gated plasticity
    neuromod = NeuromodulatorGatedPlasticity()
    
    # Simulate different neuromodulator states
    states = [
        {"dopamine": 0.0, "serotonin": 0.0, "noradrenaline": 0.0, "name": "baseline"},
        {"dopamine": 1.0, "serotonin": 0.0, "noradrenaline": 0.0, "name": "high reward"},
        {"dopamine": 0.0, "serotonin": 1.0, "noradrenaline": 0.0, "name": "high anxiety"},
        {"dopamine": 0.0, "serotonin": 0.0, "noradrenaline": 1.0, "name": "high novelty"},
        {"dopamine": 1.0, "serotonin": 1.0, "noradrenaline": 1.0, "name": "balanced"}
    ]
    
    base_learning_rate = 0.01
    spike_pair_type = "LTP"
    
    for state in states:
        neuromod.set_neuromodulator_levels(
            state["dopamine"], state["serotonin"], state["noradrenaline"]
        )
        
        modifier = neuromod.calculate_plasticity_modifier(spike_pair_type)
        weight_change = neuromod.calculate_weight_change(
            True, True, base_learning_rate, spike_pair_type
        )
        
        print(f"{state['name']:15} - Modifiers: Dopamine={neuromod.dopamine_modifier:.2f}, "
              f"Serotonin={neurom.serotonin_modifier:.2f}, "
              f"Noradrenaline={neurom.noradrenaline_modifier:.2f}")
        print(f"{'':15} - Plasticity modifier: {modifier:.3f}, Weight change: {weight_change:.4f}")
```

## Learning Algorithms

### 1. Backpropagation Learning

Backpropagation is a supervised learning algorithm for neural networks.

#### Backpropagation Model

```python
import numpy as np

class BackpropagationLearning:
    def __init__(self, learning_rate=0.01, momentum=0.9, weight_decay=0.001):
        """
        Initialize backpropagation learning.
        
        Parameters:
        -----------
        learning_rate : float
            Learning rate for backpropagation (0.0 to 1.0)
        momentum : float
            Momentum factor (0.0 to 1.0)
        weight_decay : float
            Weight decay factor (0.0 to 1.0)
        """
        self.learning_rate = learning_rate
        self.momentum = momentum
        self.weight_decay = weight_decay
        
        # Network parameters
        self.weights = {}
        self.biases = {}
        self.weight_updates = {}
        self.bias_updates = {}
        
        # Learning statistics
        self.loss_history = []
        self.accuracy_history = []
    
    def initialize_network(self, layer_sizes):
        """
        Initialize neural network with given layer sizes.
        
        Parameters:
        -----------
        layer_sizes : list
            List of layer sizes (input, hidden1, hidden2, ..., output)
        """
        # Initialize weights and biases
        for i in range(len(layer_sizes) - 1):
            input_size = layer_sizes[i]
            output_size = layer_sizes[i + 1]
            
            # Xavier initialization
            scale = np.sqrt(2.0 / (input_size + output_size))
            
            # Create weights
            self.weights[f"layer_{i}_to_{i+1}"] = np.random.randn(input_size, output_size) * scale
            
            # Create biases
            self.biases[f"layer_{i+1}"] = np.zeros(output_size)
            
            # Initialize updates
            self.weight_updates[f"layer_{i}_to_{i+1}"] = np.zeros((input_size, output_size))
            self.bias_updates[f"layer_{i+1}"] = np.zeros(output_size)
    
    def sigmoid(self, x):
        """Sigmoid activation function."""
        return 1.0 / (1.0 + np.exp(-np.clip(x, -500, 500)))
    
    def sigmoid_derivative(self, x):
        """Derivative of sigmoid function."""
        return x * (1.0 - x)
    
    def forward_pass(self, X):
        """
        Perform forward pass through the network.
        
        Parameters:
        -----------
        X : numpy.ndarray
            Input data
        
        Returns:
        --------
        dict
            Dictionary containing activations and pre-activations
        """
        activations = {"input": X}
        pre_activations = {}
        
        # Forward pass
        for i in range(1, len(self.weights) + 1):
            layer_name = f"layer_{i-1}_to_{i}"
            
            # Calculate pre-activation
            pre_act = np.dot(activations[f"layer_{i-1}"], self.weights[layer_name]) + self.biases[f"layer_{i}"]
            pre_activations[layer_name] = pre_act
            
            # Apply activation function
            activations[f"layer_{i}"] = self.sigmoid(pre_act)
        
        return activations, pre_activations
    
    def compute_loss(self, activations, y_true):
        """
        Compute mean squared error loss.
        
        Parameters:
        -----------
        activations : dict
            Dictionary of layer activations
        y_true : numpy.ndarray
            True labels
        
        Returns:
        --------
        float
            Mean squared error loss
        """
        # Get output layer
        output = activations[f"layer_{len(self.weights)}"]
        
        # Compute mean squared error
        mse = np.mean((output - y_true) ** 2)
        
        return mse
    
    def backpropagation(self, X, y_true):
        """
        Perform backpropagation.
        
        Parameters:
        -----------
        X : numpy.ndarray
            Input data
        y_true : numpy.ndarray
            True labels
        
        Returns:
        --------
        dict
            Dictionary containing gradients
        """
        # Forward pass
        activations, pre_activations = self.forward_pass(X)
        
        # Compute loss
        loss = self.compute_loss(activations, y_true)
        self.loss_history.append(loss)
        
        # Initialize gradients
        grad_weights = {}
        grad_biases = {}
        
        # Output layer gradients
        output_layer = f"layer_{len(self.weights)}"
        output_activation = activations[output_layer]
        output_error = (output_activation - y_true) * self.sigmoid_derivative(output_activation)
        
        # Backpropagate through layers
        for i in range(len(self.weights), 0, -1):
            layer_name = f"layer_{i-1}_to_{i}"
            
            # Compute gradients
            prev_layer = f"layer_{i-1}" if i > 1 else "input"
            grad_weights[layer_name] = np.outer(activations[prev_layer], output_error)
            grad_biases[f"layer_{i}"] = np.sum(output_error, axis=0)
            
            # Compute error for previous layer
            if i > 1:
                output_error = np.dot(output_error, self.weights[layer_name].T) * \
                              self.sigmoid_derivative(activations[prev_layer])
        
        return grad_weights, grad_biases, loss
    
    def update_weights(self, grad_weights, grad_biases):
        """
        Update network weights using gradients.
        
        Parameters:
        -----------
        grad_weights : dict
            Gradients for weights
        grad_biases : dict
            Gradients for biases
        """
        # Update weights with momentum
        for layer_name in self.weights:
            # Add weight decay
            self.weights[layer_name] *= (1.0 - self.weight_decay)
            
            # Update with momentum
            self.weight_updates[layer_name] = \
                self.momentum * self.weight_updates[layer_name] + \
                self.learning_rate * grad_weights[layer_name]
            
            self.weights[layer_name] += self.weight_updates[layer_name]
        
        # Update biases with momentum
        for layer_name in self.biases:
            self.bias_updates[layer_name] = \
                self.momentum * self.bias_updates[layer_name] + \
                self.learning_rate * grad_biases[layer_name]
            
            self.biases[layer_name] += self.bias_updates[layer_name]
    
    def train(self, X, y, epochs=100, batch_size=None, verbose=True):
        """
        Train the network using backpropagation.
        
        Parameters:
        -----------
        X : numpy.ndarray
            Training data
        y : numpy.ndarray
            Training labels
        epochs : int
            Number of training epochs
        batch_size : int
            Batch size (None for online learning)
        verbose : bool
            Whether to print training progress
        """
        if batch_size is None:
            # Online learning
            for epoch in range(epochs):
                # Shuffle data
                indices = np.random.permutation(len(X))
                X_shuffled = X[indices]
                y_shuffled = y[indices]
                
                # Train on each sample
                for i in range(len(X_shuffled)):
                    sample_X = X_shuffled[i:i+1]
                    sample_y = y_shuffled[i:i+1]
                    
                    # Backpropagate and update
                    grad_weights, grad_biases, loss = self.backpropagation(sample_X, sample_y)
                    self.update_weights(grad_weights, grad_biases)
                
                # Print progress
                if verbose and epoch % 10 == 0:
                    avg_loss = np.mean(self.loss_history[-10:])
                    print(f"Epoch {epoch}: Loss = {avg_loss:.4f}")
        else:
            # Mini-batch learning
            num_batches = len(X) // batch_size
            
            for epoch in range(epochs):
                # Shuffle data
                indices = np.random.permutation(len(X))
                X_shuffled = X[indices]
                y_shuffled = y[indices]
                
                # Process batches
                for batch in range(num_batches):
                    start_idx = batch * batch_size
                    end_idx = start_idx + batch_size
                    
                    batch_X = X_shuffled[start_idx:end_idx]
                    batch_y = y_shuffled[start_idx:end_idx]
                    
                    # Backpropagate and update
                    grad_weights, grad_biases, loss = self.backpropagation(batch_X, batch_y)
                    self.update_weights(grad_weights, grad_biases)
                
                # Print progress
                if verbose and epoch % 10 == 0:
                    avg_loss = np.mean(self.loss_history[-10:])
                    print(f"Epoch {epoch}: Loss = {avg_loss:.4f}")
    
    def predict(self, X):
        """
        Make predictions using the trained network.
        
        Parameters:
        -----------
        X : numpy.ndarray
            Input data
        
        Returns:
        --------
        numpy.ndarray
            Predicted outputs
        """
        activations, _ = self.forward_pass(X)
        output_layer = f"layer_{len(self.weights)}"
        return activations[output_layer]

# Example usage of backpropagation
if __name__ == "__main__":
    # Create simple dataset
    np.random.seed(42)
    X = np.random.randn(100, 2)  # 100 samples, 2 features
    y = np.array([1 if x[0] > x[1] else 0 for x in X])  # Simple classification rule
    
    # Create network
    layer_sizes = [2, 10, 1]  # Input, hidden, output
    bp = BackpropagationLearning(learning_rate=0.1, momentum=0.9)
    bp.initialize_network(layer_sizes)
    
    # Train network
    print("Training network with backpropagation...")
    bp.train(X, y, epochs=100, batch_size=10, verbose=True)
    
    # Make predictions
    predictions = bp.predict(X)
    predicted_classes = (predictions > 0.5).astype(int)
    
    # Calculate accuracy
    accuracy = np.mean(predicted_classes == y)
    print(f"\nTraining completed. Accuracy: {accuracy:.2f}")
```

### 2. Reinforcement Learning

Reinforcement learning is a type of machine learning where an agent learns to make decisions by interacting with an environment.

#### Q-Learning Model

```python
import numpy as np

class QLearning:
    def __init__(self, state_size, action_size, learning_rate=0.1, discount_factor=0.99, exploration_rate=0.1):
        """
        Initialize Q-learning agent.
        
        Parameters:
        -----------
        state_size : int
            Number of possible states
        action_size : int
            Number of possible actions
        learning_rate : float
            Learning rate (0.0 to 1.0)
        discount_factor : float
            Discount factor for future rewards (0.0 to 1.0)
        exploration_rate : float
            Exploration rate (0.0 to 1.0)
        """
        self.state_size = state_size
        self.action_size = action_size
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.exploration_rate = exploration_rate
        
        # Q-table
        self.q_table = np.zeros((state_size, action_size))
        
        # Tracking
        self.episode_count = 0
        self.total_reward = 0.0
        
        # Exploration strategy
        self.exploration_strategy = "epsilon_greedy"
    
    def choose_action(self, state, epsilon=None):
        """
        Choose an action using the current policy.
        
        Parameters:
        -----------
        state : int
            Current state
        epsilon : float
            Exploration rate (if None, use instance value)
        
        Returns:
        --------
        int
            Selected action
        """
        if epsilon is None:
            epsilon = self.exploration_rate
        
        # Epsilon-greedy exploration
        if np.random.random() < epsilon:
            return np.random.randint(self.action_size)
        else:
            return np.argmax(self.q_table[state])
    
    def learn(self, state, action, reward, next_state, done):
        """
        Update Q-values using the Q-learning update rule.
        
        Parameters:
        -----------n        state : int
            Current state
        action : int
            Action taken
        reward : float
            Reward received
        next_state : int
            Next state
        done : bool
            Whether the episode is done
        """
        # Q-learning update rule
        if done:
            target = reward
        else:
            target = reward + self.discount_factor * np.max(self.q_table[next_state])
        
        # Update Q-value
        self.q_table[state, action] += self.learning_rate * (target - self.q_table[state, action])
        
        # Update statistics
        self.episode_count += 1
        self.total_reward += reward
    
    def train(self, env_func, num_episodes=1000, max_steps=100, verbose=True):
        """
        Train the agent using Q-learning.
        
        Parameters:
        -----------
        env_func : function
            Environment function that returns (next_state, reward, done)
        num_episodes : int
            Number of training episodes
        max_steps : int
            Maximum steps per episode
        verbose : bool
            Whether to print training progress
        """
        for episode in range(num_episodes):
            # Reset environment
            state = self.reset_state(env_func)
            
            # Run episode
            episode_reward = 0
            for step in range(max_steps):
                # Choose action
                action = self.choose_action(state)
                
                # Take action
                next_state, reward, done = env_func(state, action)
                
                # Learn from experience
                self.learn(state, action, reward, next_state, done)
                
                # Update state
                state = next_state
                episode_reward += reward
                
                if done:
                    break
            
            # Print progress
            if verbose and episode % 100 == 0:
                avg_reward = self.total_reward / self.episode_count if self.episode_count > 0 else 0
                print(f"Episode {episode}: Average Reward = {avg_reward:.2f}")
            
            # Reset statistics
            if episode % 100 == 0:
                self.total_reward = 0.0
                self.episode_count = 0
    
    def reset_state(self, env_func):
        """
        Reset to initial state.
        
        Parameters:
        -----------
        env_func : function
            Environment function
        
        Returns:
        --------
        int
            Initial state
        """
        # For simplicity, return a random initial state
        return np.random.randint(0, self.state_size)
    
    def evaluate(self, env_func, num_episodes=10, max_steps=100):
        """
        Evaluate the trained agent.
        
        Parameters:
        -----------
        env_func : function
            Environment function
        num_episodes : int
            Number of evaluation episodes
        max_steps : int
            Maximum steps per episode
        
        Returns:
        --------
        float
            Average reward per episode
        """
        total_reward = 0.0
        
        for episode in range(num_episodes):
            state = self.reset_state(env_func)
            episode_reward = 0
            
            for step in range(max_steps):
                action = self.choose_action(state, epsilon=0.0)  # No exploration
                next_state, reward, done = env_func(state, action)
                
                episode_reward += reward
                state = next_state
                
                if done:
                    break
            
            total_reward += episode_reward
        
        return total_reward / num_episodes

# Example usage of Q-learning
if __name__ == "__main__":
    # Create simple environment
    def create_environment():
        """Create a simple environment for Q-learning."""
        
        def env_step(state, action):
            """Environment step function."""
            # Simple deterministic environment
            next_state = (state + action) % 10
            
            # Reward based on state and action
            if state == 5 and action == 5:
                reward = 100  # Terminal state
                done = True
            elif next_state == 0:
                reward = -10  # Penalty for returning to start
                done = False
            else:
                reward = -1  # Small penalty for each step
                done = False
            
            return next_state, reward, done
        
        return env_step
    
    # Create Q-learning agent
    env = create_environment()
    agent = QLearning(state_size=10, action_size=10, learning_rate=0.1, exploration_rate=0.1)
    
    # Train agent
    print("Training Q-learning agent...")
    agent.train(env, num_episodes=1000, max_steps=50, verbose=True)
    
    # Evaluate agent
    print("\nEvaluating agent...")
    avg_reward = agent.evaluate(env, num_episodes=10, max_steps=50)
    print(f"Average reward: {avg_reward:.2f}")
    
    # Print learned Q-table (simplified)
    print("\nLearned Q-table (first 5 states, first 5 actions):")
    print(agent.q_table[:5, :5])
```

## Plasticity Control

### 1. Adaptive Learning Rate

Adaptive learning rate adjusts the learning rate based on the training progress.

#### Adaptive Learning Rate Model

```python
import numpy as np

class AdaptiveLearningRate:
    def __init__(self, initial_rate=0.01, target_loss=0.1, min_rate=0.0001, max_rate=0.1):
        """
        Initialize adaptive learning rate.
        
        Parameters:
        -----------
        initial_rate : float
            Initial learning rate
        target_loss : float
            Target loss value
        min_rate : float
            Minimum learning rate
        max_rate : float
            Maximum learning rate
        """
        self.initial_rate = initial_rate
        self.target_loss = target_loss
        self.min_rate = min_rate
        self.max_rate = max_rate
        
        # Current learning rate
        self.current_rate = initial_rate
        
        # Loss history
        self.loss_history = []
        self.learning_rate_history = []
    
    def update(self, current_loss, improvement_factor=0.1):
        """
        Update learning rate based on current loss.
        
        Parameters:
        -----------
        current_loss : float
            Current loss value
        improvement_factor : float
            Factor for calculating improvement
        
        Returns:
        --------
        float
            Updated learning rate
        """
        # Calculate improvement
        if len(self.loss_history) > 0:
            previous_loss = self.loss_history[-1]
            improvement = previous_loss - current_loss
        else:
            improvement = 0.0
        
        # Calculate new learning rate
        if improvement > improvement_factor:
            # Good improvement: increase learning rate
            self.current_rate = min(self.current_rate * 1.1, self.max_rate)
        elif improvement < -improvement_factor:
            # Poor improvement: decrease learning rate
            self.current_rate = max(self.current_rate * 0.9, self.min_rate)
        else:
            # Similar improvement: keep learning rate
            pass
        
        # Store history
        self.loss_history.append(current_loss)
        self.learning_rate_history.append(self.current_rate)
        
        return self.current_rate
    
    def reset(self):
        """Reset learning rate to initial value."""
        self.current_rate = self.initial_rate
        self.loss_history.clear()
        self.learning_rate_history.clear()
    
    def get_history(self):
        """Get learning rate history."""
        return {
            "loss": self.loss_history,
            "rate": self.learning_rate_history
        }

# Example usage of adaptive learning rate
if __name__ == "__main__":
    # Create adaptive learning rate
    adaptive_lr = AdaptiveLearningRate(initial_rate=0.01, target_loss=0.1)
    
    # Simulate training with varying losses
    losses = [0.8, 0.6, 0.4, 0.3, 0.25, 0.22, 0.21, 0.205, 0.201, 0.2]
    
    print("Training with adaptive learning rate:")
    for epoch, loss in enumerate(losses):
        lr = adaptive_lr.update(loss, improvement_factor=0.05)
        print(f"  Epoch {epoch}: Loss = {loss:.3f}, Learning Rate = {lr:.4f}")
    
    # Plot learning rate history (if matplotlib available)
    try:
        import matplotlib.pyplot as plt
        
        history = adaptive_lr.get_history()
        
        plt.figure(figsize=(10, 6))
        plt.plot(history["loss"], label="Loss")
        plt.plot(history["rate"], label="Learning Rate")
        plt.xlabel("Epoch")
        plt.ylabel("Value")
        plt.title("Adaptive Learning Rate During Training")
        plt.legend()
        plt.grid(True)
        plt.savefig("adaptive_learning_rate.png", dpi=150, bbox_inches='tight')
        print("\nLearning rate plot saved to 'adaptive_learning_rate.png'")
        
    except ImportError:
        print("\nMatplotlib not available. Skipping plot generation.")
```

### 2. Curriculum Learning

Curriculum learning exposes the agent to simpler tasks first, gradually increasing complexity.

#### Curriculum Learning Model

```python
import numpy as np

class CurriculumLearning:
    def __init__(self, tasks, initial_task_level=0, difficulty_increment=1):
        """
        Initialize curriculum learning.
        
        Parameters:
        -----------
        tasks : list
            List of tasks with difficulty levels and requirements
        initial_task_level : int
            Initial task level (0 for easiest)
        difficulty_increment : int
            Amount to increase task level each time
        """
        self.tasks = tasks
        self.current_task_level = initial_task_level
        self.difficulty_increment = difficulty_increment
        
        # Task statistics
        self.task_performance = {}
        self.task_progress = {}
        
        # Curriculum state
        self.is_complete = False
    
    def set_task(self, task_level):
        """
        Set the current task level.
        
        Parameters:
        -----------
        task_level : int
            Task level to set
        """
        if 0 <= task_level < len(self.tasks):
            self.current_task_level = task_level
            self.task_progress[task_level] = 0
            print(f"Set task level to {task_level}: {self.tasks[task_level]['name']}")
        else:
            print(f"Invalid task level: {task_level}. Must be between 0 and {len(self.tasks)-1}.")
    
    def complete_task(self, task_level, performance):
        """
        Mark a task as completed and update statistics.
        
        Parameters:
        -----------
        task_level : int
            Task level that was completed
        performance : float
            Performance on the task (0.0 to 1.0)
        """
        if task_level in self.task_progress:
            self.task_performance[task_level] = performance
            self.task_progress[task_level] = 1
            print(f"Task {task_level} completed with performance: {performance:.2f}")
            
            # Check if we can move to next task level
            self._check_task_progression()
    
    def _check_task_progression(self):
        """Check if we should progress to the next task level."""
        # Check if current task is completed and performance is good
        if self.current_task_level in self.task_progress and \
           self.task_progress[self.current_task_level] == 1 and \
           self.task_performance[self.current_task_level] > 0.7:
            
            # Check if there is a next task level
            if self.current_task_level + self.difficulty_increment < len(self.tasks):
                next_level = self.current_task_level + self.difficulty_increment
                self.set_task(next_level)
            else:
                self.is_complete = True
                print("All tasks completed! Curriculum learning finished.")
    
    def get_current_task(self):
        """
        Get the current task.
        
        Returns:
        --------
        dict
            Current task information
        """
        if self.current_task_level < len(self.tasks):
            return self.tasks[self.current_task_level]
        else:
            return None
    
    def get_progress(self):
        """
        Get curriculum progress.
        
        Returns:
        --------
        dict
            Curriculum progress information
        """
        completed_tasks = [i for i, progress in self.task_progress.items() if progress == 1]
        
        return {
            "current_level": self.current_task_level,
            "current_task": self.tasks[self.current_task_level]["name"] if self.current_task_level < len(self.tasks) else None,
            "completed_tasks": completed_tasks,
            "total_tasks": len(self.tasks),
            "completion_percentage": len(completed_tasks) / len(self.tasks),
            "is_complete": self.is_complete
        }
    
    def reset(self):
        """Reset curriculum learning."""
        self.current_task_level = 0
        self.task_performance.clear()
        self.task_progress.clear()
        self.is_complete = False
        print("Curriculum learning reset.")

# Example usage of curriculum learning
if __name__ == "__main__":
    # Define curriculum tasks
    curriculum_tasks = [
        {
            "name": "Simple linear classification",
            "difficulty": 0,
            "requirements": {"simple": True, "features": 2},
            "description": "Classify points based on linear relationship"
        },
        {
            "name": "Binary classification with noise",
            "difficulty": 1,
            "requirements": {"noisy_data": True, "features": 2},
            "description": "Classify noisy points with some error tolerance"
        },
        {
            "name": "Multi-class classification",
            "difficulty": 2,
            "requirements": {"multi_class": True, "features": 3},
            "description": "Classify points into three or more classes"
        },
        {
            "name": "Non-linear classification",
            "difficulty": 3,
            "requirements": {"nonlinear": True, "features": 2},
            "description": "Classify points with non-linear decision boundary"
        },
        {
            "name": "High-dimensional classification",
            "difficulty": 4,
            "requirements": {"high_dim": True, "features": 10},
            "description": "Classify points in high-dimensional space"
        }
    ]
    
    # Create curriculum learning
    curriculum = CurriculumLearning(curriculum_tasks, initial_task_level=0, difficulty_increment=1)
    
    # Simulate learning through curriculum
    print("=== Curriculum Learning Simulation ===\n")
    
    # Start with first task
    current_task = curriculum.get_current_task()
    print(f"Current task: {current_task['name']} (Difficulty: {current_task['difficulty']})")
    print(f"Description: {current_task['description']}")
    print()
    
    # Simulate performance on each task
    for episode in range(50):
        # Simulate learning progress
        progress = min(1.0, episode / 10.0)  # Linear progress
        
        # Complete current task at certain milestones
        if episode % 10 == 9:
            curriculum.complete_task(curriculum.current_task_level, progress)
        
        # Print progress every 10 episodes
        if episode % 10 == 9:
            progress_info = curriculum.get_progress()
            print(f"Episode {episode:2d}: Progress = {progress_info['completion_percentage']:.1%}")
            if progress_info['current_task']:
                print(f"  Current task: {progress_info['current_task']}")
    
    # Final progress
    final_progress = curriculum.get_progress()
    print("\n=== Final Progress ===")
    print(f"Completion: {final_progress['completion_percentage']:.1%}")
    print(f"Completed tasks: {final_progress['completed_tasks']}")
    print(f"Current task: {final_progress['current_task']}")
    print(f"Is complete: {final_progress['is_complete']}")
```

## Learning and Plasticity Optimization

### 1. Gradient Clipping

Gradient clipping prevents exploding gradients by clipping gradients to a maximum norm.

#### Gradient Clipping Model

```python
import numpy as np

class GradientClipping:
    def __init__(self, clip_norm=1.0, norm_type='euclidean'):
        """
        Initialize gradient clipping.
        
        Parameters:
        -----------
        clip_norm : float
            Maximum norm for gradients (0.0 to infinity)
        norm_type : str
            Type of norm ('euclidean', 'max', 'sum')
        """
        self.clip_norm = clip_norm
        self.norm_type = norm_type
    
    def clip_gradients(self, gradients):
        """
        Clip gradients to prevent exploding gradients.
        
        Parameters:
        -----------
        gradients : dict
            Dictionary of gradients
        
        Returns:
        --------
        dict
            Clipped gradients
        """
        clipped_gradients = {}
        
        for layer_name, grad in gradients.items():
            if grad is None:
                clipped_gradients[layer_name] = grad
                continue
            
            # Calculate gradient norm
            if self.norm_type == 'euclidean':
                norm = np.linalg.norm(grad)
            elif self.norm_type == 'max':
                norm = np.max(np.abs(grad))
            elif self.norm_type == 'sum':
                norm = np.sum(np.abs(grad))
            else:
                raise ValueError(f"Unknown norm type: {self.norm_type}")
            
            # Clip gradients
            if norm > self.clip_norm:
                clipped_grad = grad * (self.clip_norm / norm)
                print(f"Layer {layer_name}: Clipped gradient from {norm:.4f} to {self.clip_norm:.4f}")
            else:
                clipped_grad = grad
            
            clipped_gradients[layer_name] = clipped_grad
        
        return clipped_gradients
    
    def get_clipping_info(self, gradients):
        """
        Get information about gradient clipping.
        
        Parameters:
        -----------
        gradients : dict
            Dictionary of gradients
        
        Returns:
        --------
        dict
            Information about gradient clipping
        """
        clipping_info = {
            'original_norms': {},
            'clipped_norms': {},
            'clipping_applied': 0
        }
        
        for layer_name, grad in gradients.items():
            if grad is None:
                clipping_info['original_norms'][layer_name] = 0.0
                clipping_info['clipped_norms'][layer_name] = 0.0
                continue
            
            # Calculate original norm
            if self.norm_type == 'euclidean':
                original_norm = np.linalg.norm(grad)
            elif self.norm_type == 'max':
                original_norm = np.max(np.abs(grad))
            elif self.norm_type == 'sum':
                original_norm = np.sum(np.abs(grad))
            
            # Calculate clipped norm
            if original_norm > self.clip_norm:
                clipped_norm = self.clip_norm
                clipping_info['clipping_applied'] += 1
            else:
                clipped_norm = original_norm
            
            clipping_info['original_norms'][layer_name] = original_norm
            clipping_info['clipped_norms'][layer_name] = clipped_norm
        
        return clipping_info

# Example usage of gradient clipping
if __name__ == "__main__":
    # Create gradient clipping
    clipper = GradientClipping(clip_norm=1.0, norm_type='euclidean')
    
    # Simulate gradients for a neural network
    gradients = {
        'layer_1': np.random.randn(10, 5) * 10,  # Large gradients
        'layer_2': np.random.randn(5, 1) * 2,   # Moderate gradients
        'layer_3': np.random.randn(1, 1) * 0.5  # Small gradients
    }
    
    print("=== Gradient Clipping Example ===")
    print(f"Clip norm: {clipper.clip_norm}")
    print(f"Norm type: {clipper.norm_type}")
    print()
    
    # Clip gradients
    clipped_gradients = clipper.clip_gradients(gradients)
    
    # Get clipping information
    clipping_info = clipper.get_clipping_info(gradients)
    
    print("Gradient clipping information:")
    for layer_name, orig_norm in clipping_info['original_norms'].items():
        clipped_norm = clipping_info['clipped_norms'][layer_name]
        clipped = clipped_norm < orig_norm
        print(f"  {layer_name}: Original={orig_norm:.4f}, Clipped={clipped_norm:.4f}, Applied={clipped}")
    
    print(f"\nLayers clipped: {clipping_info['clipping_applied']}/{len(clipping_info['original_norms'])}")
```

### 2. Batch Normalization

Batch normalization normalizes the inputs to each layer to have zero mean and unit variance.

#### Batch Normalization Model

```python
import numpy as np

class BatchNormalization:
    def __init__(self, epsilon=1e-5, momentum=0.9):
        """
        Initialize batch normalization.
        
        Parameters:
        -----------
        epsilon : float
            Small constant for numerical stability
        momentum : float
            Momentum for updating running statistics
        """
        self.epsilon = epsilon
        self.momentum = momentum
        
        # Batch normalization statistics
        self.running_mean = 0.0
        self.running_variance = 1.0
        
        # Mini-batch statistics
        self.batch_mean = 0.0
        self.batch_variance = 1.0
        
        # Scale and shift parameters
        self.gamma = 1.0
        self.beta = 0.0
        
        # Gradients
        self.gamma_gradient = 0.0
        self.beta_gradient = 0.0
        
        # State
        self.training = True
    
    def forward(self, x):
        """
        Forward pass through batch normalization.
        
        Parameters:
        -----------
        x : numpy.ndarray
            Input data
        
        Returns:
        --------
        numpy.ndarray
            Normalized data
        """
        if self.training:
            # Calculate batch statistics
            self.batch_mean = np.mean(x, axis=0)
            self.batch_variance = np.var(x, axis=0)
            
            # Normalize
            x_normalized = (x - self.batch_mean) / np.sqrt(self.batch_variance + self.epsilon)
            
            # Scale and shift
            output = self.gamma * x_normalized + self.beta
        else:
            # Use running statistics
            x_normalized = (x - self.running_mean) / np.sqrt(self.running_variance + self.epsilon)
            output = self.gamma * x_normalized + self.beta
        
        return output
    
    def backward(self, x, output_gradient):
        """
        Backward pass through batch normalization.
        
        Parameters:
        -----------
        x : numpy.ndarray
            Input data
        output_gradient : numpy.ndarray
            Gradient of loss with respect to output
        
        Returns:
        --------
        tuple
            (gradient_wrt_input, gamma_gradient, beta_gradient)
        """
        if not self.training:
            return output_gradient
        
        # Calculate gradients
        m = x.shape[0]  # Mini-batch size
        
        # Gradient with respect to input
        x_normalized = (x - self.batch_mean) / np.sqrt(self.batch_variance + self.epsilon)
        
        dx = output_gradient / np.sqrt(self.batch_variance + self.epsilon)
        
        # Gradient for batch mean
        dmean = -np.sum(dx, axis=0) / (self.batch_variance + self.epsilon) ** 0.5
        dmean /= m
        
        # Gradient for batch variance
        dvariance = -0.5 * np.sum((x - self.batch_mean) * dx, axis=0) / \
                   ((self.batch_variance + self.epsilon) ** 1.5)
        dvariance /= m
        
        # Update input gradient
        dx = dx + dmean + dvariance * (x - self.batch_mean)
        
        # Gradient for gamma
        dgamma = np.sum(output_gradient * x_normalized, axis=0)
        
        # Gradient for beta
        dbeta = np.sum(output_gradient, axis=0)
        
        self.gamma_gradient = dgamma
        self.beta_gradient = dbeta
        
        return dx
    
    def update_running_statistics(self):
        """Update running statistics with mini-batch statistics."""
        self.running_mean = (
            self.momentum * self.running_mean +
            (1 - self.momentum) * self.batch_mean
        )
        
        self.running_variance = (
            self.momentum * self.running_variance +
            (1 - self.momentum) * self.batch_variance
        )
    
    def set_training_mode(self, training):
        """Set training mode."""
        self.training = training
    
    def get_statistics(self):
        """Get batch normalization statistics."""
        return {
            "running_mean": self.running_mean,
            "running_variance": self.running_variance,
            "batch_mean": self.batch_mean,
            "batch_variance": self.batch_variance,
            "gamma": self.gamma,
            "beta": self.beta,
            "training": self.training
        }

# Example usage of batch normalization
if __name__ == "__main__":
    # Create batch normalization layer
    bn = BatchNormalization(epsilon=1e-5, momentum=0.9)
    
    # Generate sample data
    np.random.seed(42)
    x = np.random.randn(32, 10)  # 32 samples, 10 features
    
    print("=== Batch Normalization Example ===")
    print(f"Input shape: {x.shape}")
    print(f"Input mean: {np.mean(x):.4f}, std: {np.std(x):.4f}")
    
    # Training mode
    bn.set_training_mode(True)
    
    # Forward pass
    output = bn.forward(x)
    print(f"Output mean: {np.mean(output):.4f}, std: {np.std(output):.4f}")
    
    # Backward pass
    output_gradient = np.random.randn(32, 10)
    input_gradient = bn.backward(x, output_gradient)
    
    print(f"Input gradient norm: {np.linalg.norm(input_gradient):.4f}")
    print(f"Gamma gradient: {bn.gamma_gradient}")
    print(f"Beta gradient: {bn.beta_gradient}")
    
    # Update running statistics
    bn.update_running_statistics()
    print(f"Running mean: {bn.running_mean}")
    print(f"Running variance: {bn.running_variance}")
    
    # Test inference mode
    print("\n--- Inference Mode ---")
    bn.set_training_mode(False)
    test_x = np.random.randn(5, 10)
    test_output = bn.forward(test_x)
    print(f"Test output mean: {np.mean(test_output):.4f}, std: {np.std(test_output):.4f}")
```

## Key Takeaways

### 1. Learning Rules

- **STDP:** Spike-timing-dependent plasticity strengthens connections based on spike timing
- **Hebbian Learning:** "Neurons that fire together, wire together"
- **Homeostatic Plasticity:** Maintains stability by regulating overall activity
- **Neuromodulator-Gated Plasticity:** Modulators like dopamine regulate plasticity based on behavioral context

### 2. Learning Algorithms

- **Backpropagation:** Supervised learning algorithm using gradient descent
- **Q-Learning:** Model-free reinforcement learning algorithm
- **Adaptive Learning Rate:** Adjusts learning rate based on training progress
- **Curriculum Learning:** Gradual progression from simple to complex tasks

### 3. Optimization Techniques

- **Gradient Clipping:** Prevents exploding gradients by clipping gradient norms
- **Batch Normalization:** Normalizes inputs to reduce internal covariate shift
- **Curricular Learning:** Structures learning to maximize retention and generalization

### 4. Plasticity Control

- **Learning Rate Adaptation:** Adjusts learning rate based on training progress
- **Curriculum Learning:** Progresses through increasingly complex tasks
- **Gradient Clipping:** Prevents gradient explosion during training
- **Batch Normalization:** Stabilizes training by normalizing layer inputs

### 5. Biological Plausibility

- **Activity-Dependent Plasticity:** Plasticity changes based on neural activity
- **Neuromodulator Influence:** Modulators like dopamine regulate learning
- **Homeostatic Mechanisms:** Maintain stability in neural networks
- **Experience-Dependent Learning:** Learning adapts based on experience

## Further Learning

### Practice Exercises:

1. **Implement STDP:** Create a neural network with STDP learning
2. **Study Hebbian Learning:** Explore the effects of Hebbian learning on network dynamics
3. **Design Homeostatic Plasticity:** Implement homeostatic mechanisms for stability
4. **Apply Neuromodulator-Gated Plasticity:** Model the effects of neuromodulators on learning
5. **Implement Learning Algorithms:** Use backpropagation and Q-learning for different tasks

### Advanced Topics:

1. **Advanced Learning Theory:** Study theoretical foundations of learning algorithms
2. **Neural Network Optimization:** Explore optimization techniques for deep learning
3. **Biological Learning Systems:** Study learning in biological systems
4. **Machine Learning Applications:** Apply learning algorithms to real-world problems
5. **Extensions:** Create custom learning rules and algorithms

### Research Applications:

1. **Cognitive Modeling:** Model human learning and memory
2. **Neural Computation:** Study neural mechanisms of learning
3. **Artificial Intelligence:** Develop intelligent learning systems
4. **Neuroscience:** Investigate neural plasticity in vivo
5. **Robotics:** Implement learning in robotic systems

## Conclusion

This tutorial has provided a comprehensive guide to learning and plasticity control in NLM. You have learned how to:

1. **Implement Learning Rules:** Create STDP, Hebbian, and homeostatic plasticity mechanisms
2. **Use Learning Algorithms:** Apply backpropagation and Q-learning for different tasks
3. **Control Plasticity:** Use adaptive learning rates and curriculum learning
4. **Optimize Learning:** Implement gradient clipping and batch normalization
5. **Model Neuromodulation:** Simulate the effects of neuromodulators on learning

Learning and plasticity are fundamental to intelligent behavior. In NLM, these mechanisms enable the brain network to:

- **Learn from experience:** Modify connections based on neural activity
- **Adapt to environment:** Adjust behavior based on feedback
- **Generalize knowledge:** Apply learned patterns to new situations
- **Maintain stability:** Prevent runaway excitation or inactivity
- **Optimize performance:** Maximize reward while minimizing cost

By mastering these learning and plasticity mechanisms, you can create NLM agents that:

- Learn efficiently from their environment
- Adapt to changing conditions
- Make optimal decisions based on experience
- Generalize knowledge to novel situations
- Maintain stable and robust behavior

The learning and plasticity systems you've learned here form the core of intelligent behavior in NLM. As you continue to explore and develop your NLM applications, you'll find that effective learning is key to creating truly intelligent and adaptive systems.

Happy modeling!