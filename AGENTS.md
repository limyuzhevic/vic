#!/usr/bin/env python3
"""
NLM Python Module - Comprehensive Documentation and Examples
===========================================================

This document provides comprehensive documentation and examples for the NLM Python module,
which offers advanced bindings and scripting capabilities for the NLM brain emulation project.

The module transforms NLM from a basic simulator into a sophisticated programmable brain
emulation platform suitable for research, education, and production applications.

Table of Contents
----------------
1. Overview
2. Installation and Setup
3. Python API Reference
4. Advanced Features
5. Examples and Demos
6. Performance Optimization
7. Troubleshooting
8. Extending NLM with Python

---

1. Overview
-----------

The NLM Python module provides comprehensive Python bindings and advanced scripting
capabilities for the NLM brain emulation project. It transforms the basic NLM
interface into a sophisticated platform with the following key features:

### Core Capabilities

1. **Advanced Python Bindings**
   - Comprehensive bindings for all NLM core components
   - Python-native interfaces for brain operations
   - Real-time brain control and monitoring from Python
   - Simulation control and experiment management
   - Advanced neural network analysis tools

2. **Advanced Scripting Features**
   - Dynamic neural network configuration and modification
   - Real-time simulation control and parameter tuning
   - Automated experiment generation and execution
   - Advanced data analysis and visualization from Python
   - Machine learning integration and model training
   - Custom learning algorithm implementation

3. **Integration with Advanced Systems**
   - Integration with command system and visualization
   - Real-time brain state monitoring and control
   - Advanced configuration management from Python
   - Performance profiling and optimization control
   - Error handling and debugging tools

4. **Production Features**
   - High-performance Python bindings with C++ extensions
   - Comprehensive documentation and examples
   - Error handling and debugging tools
   - Performance benchmarking and optimization
   - Multi-threading and parallel processing support
   - Cross-platform compatibility

### Module Structure

The NLM Python module consists of several submodules:

- **nlm_python_module**: Core simulation controller with advanced brain control
- **nlm_configuration_module**: Neural network configuration and parameter tuning
- **nlm_analysis_module**: Advanced data analysis and visualization tools
- **nlm_ml_module**: Machine learning integration and custom algorithms
- **nlm_debug_module**: Error handling, debugging tools, and performance benchmarking

### Key Classes and Interfaces

1. **NLPSimulation**: Main simulation controller with advanced scripting capabilities
2. **NetworkArchitecture**: Neural network architecture configuration
3. **PlatycityController**: Synaptic plasticity mechanism controller
4. **NeuromodulationTuner**: Neuromodulation parameter tuning
5. **ParameterOptimizer**: Advanced parameter optimization
6. **RealTimeParameterTuner**: Real-time parameter tuning during simulation
7. **NLMConfigurator**: Comprehensive configuration manager
8. **ConfigurationManager**: Configuration persistence and management
9. **NeuralActivityAnalyzer**: Advanced neural activity analysis
10. **NetworkTopologyAnalyzer**: Network topology analysis
11. **LearningProgressAnalyzer**: Learning progress and performance analysis
12. **VisualizationEngine**: Advanced visualization tools
13. **NLPMonitoring**: Real-time monitoring system
14. **NLPAnalysisSuite**: Comprehensive analysis suite
15. **NLPToolkit**: Complete debugging and performance toolkit

---

2. Installation and Setup
-------------------------

### Prerequisites

Before installing the NLM Python module, ensure you have:

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher
- pip (Python package installer)

### Installation from Source

1. **Clone the NLM repository**
```bash
https://github.com/your-repo/nlm.git
cd nlm
```

2. **Build the NLM C++ core**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Install Python dependencies**
```bash
pip install numpy pandas matplotlib seaborn scipy scikit-learn
```

4. **Install NLM Python module**
```bash
pip install -e .
```

### Quick Start

```python
#!/usr/bin/env python3

"""Quick start example for NLM Python module"""

import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_python_module import NLPSimulation, NLPAnalysis

def main():
    # Create simulation controller
    sim = NLPSimulation(headless=True)
    
    # Configure neural network
    sim.configure_neural_network(
        region_name="hippocampus",
        neuron_count=200,
        neuron_type="internal",
        connection_probability=0.05
    )
    
    # Configure plasticity
    sim.configure_plasticity(
        plasticity_types=["stdp", "hebbian"],
        learning_rate=0.01
    )
    
    # Configure neuromodulation
    sim.configure_neuromodulation(
        neuromodulators=["dopamine", "curiosity"]
    )
    
    # Start simulation
    sim.start_simulation(max_steps=1000, timestep=0.01)
    
    # Wait for completion
    while sim.get_simulation_status()['state'] in ['running', 'paused']:
        time.sleep(0.1)
    
    # Analyze results
    analysis = NLPAnalysis(sim)
    neural_activity = analysis.analyze_neural_activity()
    
    # Export results
    sim.export_results('simulation_results.json')
    
    print("Simulation completed successfully!")

if __name__ == "__main__":
    main()
```

### Alternative Installation (Conda)

```bash
conda create -n nlm python=3.8
conda activate nlm
conda install numpy pandas matplotlib seaborn scipy scikit-learn
pip install -e .
```

---

3. Python API Reference
------------------------

### NLPSimulation Class

```python
class NLPSimulation:
    def __init__(self, config_file: Optional[str] = None, headless: bool = False):
        """
        Initialize NLM simulation controller.
        
        Args:
            config_file: Path to configuration file (JSON/YAML)
            headless: Run without UI if available
        """
    
    def configure_neural_network(self, 
                                region_name: str = "default",
                                neuron_count: int = 100,
                                neuron_type: str = "internal",
                                connection_probability: float = 0.1,
                                connection_strength: float = 1.0,
                                connection_type: str = "excitatory"):
        """Dynamically configure neural network."""
    
    def configure_plasticity(self, 
                            plasticity_types: List[str],
                            learning_rate: float = 0.01,
                            stdp_tau_pre: float = 0.02,
                            stdp_tau_post: float = 0.02,
                            hebbian_strength: float = 1.0):
        """Configure synaptic plasticity mechanisms."""
    
    def configure_neuromodulation(self, 
                                 neuromodulators: List[str],
                                 baseline_levels: Dict[str, float] = None,
                                 gain_factors: Dict[str, float] = None):
        """Configure neuromodulation systems."""
    
    def start_simulation(self, 
                        max_steps: int = -1,
                        timestep: float = 0.01,
                        enable_monitoring: bool = True,
                        monitor_interval: float = 1.0):
        """Start NLM simulation with real-time monitoring."""
    
    def pause_simulation(self):
        """Pause the simulation."""
    
    def resume_simulation(self):
        """Resume the simulation."""
    
    def stop_simulation(self):
        """Stop the simulation."""
    
    def reset_simulation(self):
        """Reset the simulation to initial state."""
    
    def get_simulation_status(self) -> Dict[str, Any]:
        """Get current simulation status."""
    
    def analyze_data(self, 
                    metric: str = "firing_rate",
                    window: Optional[int] = None,
                    neurons: Optional[List[int]] = None,
                    start_step: Optional[int] = None,
                    end_step: Optional[int] = None) -> Dict[str, Any]:
        """Analyze simulation data."""
    
    def create_experiment(self, 
                         name: str,
                         description: str = "",
                         parameters: Optional[Dict[str, Any]] = None,
                         duration: int = 1000,
                         save_results: bool = True) -> Dict[str, Any]:
        """Create and run automated experiment."""
    
    def export_results(self, 
                      filename: str,
                      data_format: str = 'json',
                      include_step_history: bool = True,
                      include_performance_metrics: bool = True,
                      experiment_id: Optional[str] = None):
        """Export simulation results to file."""
```

### NetworkArchitecture Class

```python
class NetworkArchitecture:
    def __init__(self, 
                 name: str,
                 regions: Dict[str, Any] = None,
                 connections: Dict[str, Any] = None,
                 parameters: Dict[str, Any] = None):
        """Initialize network architecture."""
    
    def add_region(self, 
                   name: str,
                   neuron_count: int = 100,
                   neuron_type: str = "internal",
                   position: Tuple[float, float] = (0.0, 0.0),
                   parameters: Dict[str, Any] = None):
        """Add a new neural region."""
    
    def add_connection(self,
                      source_region: str,
                      target_region: str,
                      connection_type: str = "excitatory",
                      weight: float = 1.0,
                      delay: int = 1,
                      plasticity_enabled: bool = True):
        """Add connection between regions."""
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert architecture to dictionary."""
```

### NLMConfigurator Class

```python
class NLMConfigurator:
    def __init__(self, simulation: Any = None):
        """Initialize NLM configurator."""
    
    def create_architecture(self, 
                           name: str,
                           regions: List[Dict[str, Any]] = None,
                           connections: List[Dict[str, Any]] = None) -> NetworkArchitecture:
        """Create new neural network architecture."""
    
    def optimize_parameters(self,
                           objective_function: Callable,
                           initial_params: Dict[str, Any],
                           optimization_method: str = 'gradient_descent',
                           bounds: Dict[str, Tuple[float, float]] = None) -> Dict[str, Any]:
        """Optimize parameters using specified method."""
    
    def start_real_time_optimization(self):
        """Start real-time optimization."""
    
    def get_configuration_summary(self) -> Dict[str, Any]:
        """Get summary of current configuration."""
```

### ConfigurationManager Class

```python
class ConfigurationManager:
    def __init__(self, storage_file: str = "nlm_config.json"):
        """Initialize configuration manager."""
    
    def load_configurations(self):
        """Load configurations from storage file."""
    
    def save_configurations(self):
        """Save configurations to storage file."""
    
    def export_configuration(self,
                            filename: str,
                            format: str = 'json',
                            include_architectures: bool = True,
                            include_connections: bool = True,
                            include_plasticity: bool = True):
        """Export configuration to file."""
    
    def import_configuration(self,
                            filename: str,
                            format: str = 'json',
                            merge: bool = True):
        """Import configuration from file."""
```

### NLPAnalysisSuite Class

```python
class NLPAnalysisSuite:
    def __init__(self, simulation: Any = None):
        """Initialize analysis suite."""
    
    def run_comprehensive_analysis(self,
                                  analysis_types: List[str] = None,
                                  visualization_types: List[str] = None,
                                  time_window: Dict[str, Any] = None) -> Dict[str, Any]:
        """Run comprehensive analysis suite."""
    
    def analyze_neural_activity(self,
                               neurons: Optional[List[int]] = None,
                               time_window: Optional[Dict[str, Any]] = None,
                               metrics: List[str] = None) -> Dict[str, Any]:
        """Analyze neural activity patterns."""
    
    def analyze_network_topology(self,
                                metrics: List[str] = None) -> Dict[str, Any]:
        """Analyze network topology and connectivity."""
    
    def analyze_learning_progress(self,
                                 experiment_ids: Optional[List[str]] = None) -> Dict[str, Any]:
        """Analyze learning progress across experiments."""
```

### NLPToolkit Class

```python
class NLPToolkit:
    def __init__(self, simulation: Any = None):
        """Initialize debugging and performance toolkit."""
    
    def handle_exception(self, 
                        exception: Exception,
                        context: Dict[str, Any] = None,
                        callback: Callable = None):
        """Handle exception with full context."""
    
    def profile_function(self, func: Callable, *args, **kwargs) -> Any:
        """Profile function execution."""
    
    def benchmark(self, 
                  func: Callable,
                  name: str = None,
                  iterations: int = 10,
                  warmup_iterations: int = 3,
                  *args,
                  **kwargs) -> Dict[str, Any]:
        """Benchmark function performance."""
    
    def export_debug_data(self, filepath: str):
        """Export debug data to file."""
    
    def cleanup(self):
        """Cleanup toolkit resources."""
```

---

4. Advanced Features
-------------------

### 4.1 Dynamic Neural Network Configuration

The NLM Python module provides advanced tools for dynamic neural network configuration:

```python
# Create network architecture
configurator = NLMConfigurator()
arch = configurator.create_architecture(
    name="complex_brain",
    regions=[
        {'name': 'hippocampus', 'neuron_count': 200, 'neuron_type': 'internal'},
        {'name': 'cortex', 'neuron_count': 500, 'neuron_type': 'excitatory'}
    ],
    connections=[
        {'source_region': 'hippocampus', 'target_region': 'cortex', 'weight': 0.8}
    ]
)

# Modify architecture dynamically
arch.add_region('amygdala', neuron_count=100, neuron_type='interneuron')
arch.add_connection('cortex', 'amygdala', weight=0.5, connection_type='inhibitory')
```

### 4.2 Real-time Parameter Tuning

```python
# Create real-time parameter tuner
from nlm_configuration_module import RealTimeParameterTuner

def get_current_performance():
    # Get current simulation performance
    return {'accuracy': 0.85, 'loss': 0.15}

tuner = RealTimeParameterTuner(get_current_performance)

# Add tuning rules
tuner.add_tuning_rule(
    name='learning_rate',
    metric='accuracy',
    target=0.9,
    sensitivity=0.1
)

# Start real-time tuning
tuner.start_real_time_tuning(monitor_interval=5.0)
```

### 4.3 Advanced Data Analysis

```python
# Create analysis suite
from nlm_analysis_module import NLPAnalysisSuite

analysis = NLPAnalysisSuite(simulation)

# Run comprehensive analysis
results = analysis.run_comprehensive_analysis(
    analysis_types=['neural_activity', 'network_topology', 'learning_progress'],
    visualization_types=['heatmap', 'time_series', 'network_graph']
)

# Analyze neural activity
neural_results = analysis.neural_analyzer.analyze_spike_patterns(
    neurons=[1, 2, 3],
    metrics=['firing_rate', 'coherence', 'synchrony']
)

# Analyze network topology
topology_results = analysis.topology_analyzer.analyze_connectivity_patterns(
    neurons=[1, 2, 3, 4, 5],
    min_connection_strength=0.1
)
```

### 4.4 Machine Learning Integration

```python
# Create deep learning framework
from nlm_ml_module import DeepLearningFramework

framework = DeepLearningFramework()

# Create neural network model
model = framework.model_factory.create_feed_forward_model(
    input_shape=(100,),
    output_shape=(10,),
    hidden_layers=[128, 64, 32],
    activation='relu',
    output_activation='softmax'
)

# Train model
X_train = np.random.randn(1000, 100)
y_train = np.random.randint(0, 10, 1000)

results = framework.training_engine.train_model(
    model, X_train, y_train,
    epochs=50,
    batch_size=32,
    validation_split=0.2
)

# Evaluate model
X_test = np.random.randn(100, 100)
y_test = np.random.randint(0, 10, 100)

evaluation = framework.evaluate_model(model, X_test, y_test)
print(f"Accuracy: {evaluation['accuracy']:.4f}")
```

### 4.5 Performance Monitoring and Debugging

```python
# Create toolkit for debugging and monitoring
from nlm_debug_module import NLPToolkit

toolkit = NLPToolkit(simulation)

# Set up breakpoints and watch variables
toolkit.set_breakpoint("test_breakpoint", condition=lambda msg, data, comp: "error" in msg.lower())
toolkit.watch_variable("performance_metric", 0.0)

# Profile function execution
@toolkit.profile_function
def complex_function(x):
    time.sleep(0.01)
    return x * 2

# Benchmark performance
bench_result = toolkit.benchmark(complex_function, 5, name="complex_function", iterations=10)

# Handle exceptions with context
try:
    # Code that might raise exceptions
    result = complex_function(10)
except Exception as e:
    toolkit.handle_exception(e, {"context": "benchmark_test"})
```

---

5. Examples and Demos
--------------------

### 5.1 Basic Simulation Example

```python
"""Basic simulation example"""

import time
import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_python_module import NLPSimulation, NLPAnalysis

def basic_simulation_example():
    """Example of basic simulation"""
    print("=== Basic Simulation Example ===")
    
    # Create simulation
    sim = NLPSimulation(headless=True)
    
    # Configure neural network
    sim.configure_neural_network(
        region_name="default",
        neuron_count=100,
        neuron_type="internal"
    )
    
    # Configure plasticity
    sim.configure_plasticity(
        plasticity_types=["stdp"],
        learning_rate=0.01
    )
    
    # Configure neuromodulation
    sim.configure_neuromodulation(
        neuromodulators=["dopamine"]
    )
    
    # Run simulation
    sim.start_simulation(max_steps=100, timestep=0.01)
    
    # Wait for completion
    while sim.get_simulation_status()['state'] in ['running', 'paused']:
        time.sleep(0.1)
    
    # Analyze results
    analysis = NLPAnalysis(sim)
    status = sim.get_simulation_status()
    
    print(f"Simulation completed with {status['total_steps']} steps")
    print(f"Final accuracy: {status.get('brain_statistics', {}).get('average_firing_rate', 0):.4f}")
    
    # Export results
    sim.export_results("basic_simulation_results.json")
    
    return sim, analysis

if __name__ == "__main__":
    basic_simulation_example()
```

### 5.2 Advanced Network Configuration Example

```python
"""Advanced network configuration example"""

import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_configuration_module import NLMConfigurator, NetworkArchitecture

def advanced_configuration_example():
    """Example of advanced network configuration"""
    print("=== Advanced Network Configuration Example ===")
    
    # Create configurator
    configurator = NLMConfigurator()
    
    # Create complex architecture
    architecture = configurator.create_architecture(
        name="brain_v2.0",
        regions=[
            {
                'name': 'hippocampus',
                'neuron_count': 200,
                'neuron_type': 'internal',
                'position': (10, 10),
                'parameters': {
                    'capacity': 500,
                    'decay_rate': 0.01
                }
            },
            {
                'name': 'cortex',
                'neuron_count': 500,
                'neuron_type': 'excitatory',
                'position': (20, 20),
                'parameters': {
                    'inhibition_strength': 0.5,
                    'excitation_strength': 1.0
                }
            },
            {
                'name': 'amygdala',
                'neuron_count': 100,
                'neuron_type': 'interneuron',
                'position': (15, 5),
                'parameters': {
                    'modulation_strength': 0.8
                }
            }
        ],
        connections=[
            {
                'source_region': 'hippocampus',
                'target_region': 'cortex',
                'connection_type': 'excitatory',
                'weight': 0.8,
                'delay': 2,
                'plasticity_enabled': True
            },
            {
                'source_region': 'cortex',
                'target_region': 'amygdala',
                'connection_type': 'inhibitory',
                'weight': -0.5,
                'delay': 1,
                'plasticity_enabled': True
            },
            {
                'source_region': 'amygdala',
                'target_region': 'hippocampus',
                'connection_type': 'modulatory',
                'weight': 0.3,
                'delay': 3,
                'plasticity_enabled': True
            }
        ]
    )
    
    # Configure plasticity
    configurator.plasticity_controller.configure_stdp(
        learning_rate=0.01,
        tau_pre=0.02,
        tau_post=0.02
    )
    
    configurator.plasticity_controller.configure_hebbian(
        learning_rate=0.005,
        coherence_threshold=0.5
    )
    
    # Configure neuromodulation
    configurator.neuromodulation_tuner.neuromodulators['dopamine']['baseline'] = 0.05
    configurator.neuromodulation_tuner.neuromodulators['dopamine']['phasic_gain'] = 1.0
    
    # Optimize parameters
    def objective_function(params):
        # Simple objective: balance between learning rate and stability
        learning_rate = params.get('learning_rate', 0.01)
        stdp_stability = params.get('stdp_stability', 1.0)
        return -abs(learning_rate * stdp_stability - 0.8)  # Maximize toward 0.8
    
    initial_params = {'learning_rate': 0.01, 'stdp_stability': 1.0}
    bounds = {'learning_rate': (0.001, 0.1), 'stdp_stability': (0.5, 2.0)}
    
    optimization_result = configurator.optimize_parameters(
        objective_function=objective_function,
        initial_params=initial_params,
        optimization_method='random_search',
        bounds=bounds
    )
    
    print(f"Architecture created with {len(architecture.regions)} regions and {len(architecture.connections)} connections")
    print(f"Optimized parameters: {optimization_result['optimized_parameters']}")
    print(f"Final score: {optimization_result['final_score']:.6f}")
    
    # Save configuration
    configurator.save_architecture(architecture)
    configurator.save_configurations()
    
    return configurator, architecture

if __name__ == "__main__":
    advanced_configuration_example()
```

### 5.3 Real-time Tuning Example

```python
"""Real-time tuning example"""

import time
import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_configuration_module import RealTimeParameterTuner

def real_time_tuning_example():
    """Example of real-time parameter tuning"""
    print("=== Real-time Tuning Example ===")
    
    # Create performance callback
    def get_current_performance():
        # Simulate getting current performance metrics
        return {
            'accuracy': 0.75 + time.time() * 0.001 % 0.2,
            'loss': 0.25 - time.time() * 0.001 % 0.1,
            'learning_rate': 0.01
        }
    
    # Create real-time tuner
    tuner = RealTimeParameterTuner(get_current_performance)
    
    # Add tuning rules
    tuner.add_tuning_rule(
        name='accuracy_target',
        metric='accuracy',
        target=0.9,
        sensitivity=0.5
    )
    
    tuner.add_tuning_rule(
        name='loss_target',
        metric='loss',
        target=0.1,
        sensitivity=0.3
    )
    
    # Add learning rate adjustment rule
    def learning_rate_adjustment(performance, rule_config):
        current_accuracy = performance.get('accuracy', 0.0)
        target_accuracy = rule_config['target']
        sensitivity = rule_config['sensitivity']
        
        # Adjust learning rate based on performance
        adjustment = (target_accuracy - current_accuracy) * sensitivity
        new_learning_rate = max(0.001, min(0.1, performance['learning_rate'] + adjustment))
        
        return {'learning_rate': new_learning_rate}
    
    # Create custom tuning rule
    custom_rule = {
        'name': 'learning_rate_adjustment',
        'condition': lambda perf: perf.get('accuracy', 0.0) < 0.8,
        'strategy': learning_rate_adjustment,
        'metric': 'accuracy',
        'target': 0.8,
        'sensitivity': 0.2
    }
    
    # Add to tuner
    tuner.add_tuning_rule(
        name=custom_rule['name'],
        condition=custom_rule['condition'],
        metric=custom_rule['metric'],
        target=custom_rule['target'],
        sensitivity=custom_rule['sensitivity']
    )
    
    # Start real-time tuning
    print("Starting real-time tuning...")
    tuner.start_real_time_tuning(monitor_interval=2.0)
    
    # Simulate running simulation
    for i in range(10):
        time.sleep(2)
        print(f"Tuning cycle {i + 1} completed")
    
    # Get tuning history
    history = tuner.get_tuning_history(limit=5)
    print(f"\nTuning history (last 5 cycles):")
    for i, entry in enumerate(history):
        print(f"  Cycle {i + 1}: {entry['performance']}")
    
    # Stop tuning
    tuner.stop_tuning()
    print("Real-time tuning stopped")
    
    return tuner

if __name__ == "__main__":
    real_time_tuning_example()
```

### 5.4 Machine Learning Integration Example

```python
"""Machine learning integration example"""

import numpy as np
import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_ml_module import DeepLearningFramework, NLPAutoML

def ml_integration_example():
    """Example of machine learning integration"""
    print("=== Machine Learning Integration Example ===")
    
    # Initialize deep learning framework
    framework = DeepLearningFramework()
    
    # Create synthetic data for classification
    np.random.seed(42)
    n_samples = 1000
    n_features = 20
    n_classes = 3
    
    X = np.random.randn(n_samples, n_features)
    y = np.random.randint(0, n_classes, n_samples)
    
    # Add some patterns to make data learnable
    X[y == 0, 0:5] += 2.0
    X[y == 1, 5:10] += 2.0
    X[y == 2, 10:15] += 2.0
    
    # Split data
    split_idx = int(0.8 * n_samples)
    X_train, X_test = X[:split_idx], X[split_idx:]
    y_train, y_test = y[:split_idx], y[split_idx:]
    
    print(f"Dataset created: {n_samples} samples, {n_features} features, {n_classes} classes")
    
    # Initialize AutoML
    automl = NLPAutoML()
    
    # Run AutoML optimization
    print("\nRunning AutoML optimization...")
    automl_results = automl.auto_train(
        X_train, y_train,
        problem_type='classification',
        n_trials=50
    )
    
    print(f"AutoML completed:")
    print(f"  - Best model: {automl_results['best_model_config']['name']}")
    print(f"  - Best score: {automl_results['best_score']:.4f}")
    
    # Create deep learning models
    print("\nCreating deep learning models...")
    
    # Feed-forward model for classification
    ff_model = framework.model_factory.create_feed_forward_model(
        input_shape=(n_features,),
        output_shape=(n_classes,),
        hidden_layers=[128, 64, 32],
        activation='relu',
        output_activation='softmax',
        learning_rate=0.001
    )
    
    # Train feed-forward model
    print("Training feed-forward neural network...")
    ff_results = framework.training_engine.train_model(
        ff_model, X_train, y_train,
        epochs=50,
        batch_size=32,
        validation_split=0.2,
        strategy='mini_batch_training'
    )
    
    print(f"FFNN training completed: {ff_results['epochs']} epochs, final loss: {ff_results['final_loss']:.4f}")
    
    # Evaluate feed-forward model
    ff_evaluation = framework.evaluate_model(ff_model, X_test, y_test)
    print(f"FFNN evaluation: accuracy = {ff_evaluation['accuracy']:.4f}")
    
    # Create recurrent model for sequence data
    print("\nCreating recurrent neural network...")
    
    # For sequence data, we need different input shape
    X_train_seq = np.random.randn(1000, 10, n_features)  # (samples, timesteps, features)
    y_train_seq = np.random.randint(0, n_classes, 1000)
    
    rnn_model = framework.model_factory.create_recurrent_model(
        input_shape=(10, n_features),  # (timesteps, features)
        output_shape=(n_classes,),
        hidden_units=64,
        lstm_layers=2,
        learning_rate=0.001
    )
    
    print("RNN model created (not trained in this example)")
    
    # Create transformer model
    print("\nCreating transformer model...")
    
    transformer_model = framework.model_factory.create_transformer_model(
        input_shape=(50, n_features),  # (sequence_length, features)
        output_shape=(n_classes,),
        n_heads=8,
        d_model=256,
        num_encoder_layers=4,
        learning_rate=0.001
    )
    
    print("Transformer model created (not trained in this example)")
    
    return framework, automl, ff_model

if __name__ == "__main__":
    ml_integration_example()
```

### 5.5 Debugging and Performance Analysis Example

```python
"""Debugging and performance analysis example"""

import time
import sys
sys.path.insert(0, '/path/to/nlm/python')

from nlm_debug_module import NLPToolkit

def debugging_example():
    """Example of debugging and performance analysis"""
    print("=== Debugging and Performance Analysis Example ===")
    
    # Create toolkit
    toolkit = NLPToolkit()
    
    # Start monitoring
    print("\nStarting performance monitoring...")
    toolkit.start_monitoring()
    
    # Set up debugging
    print("\nSetting up debugging...")
    toolkit.set_breakpoint("performance_breakpoint", condition=lambda msg, data, comp: "performance" in msg.lower())
    toolkit.watch_variable("current_accuracy", 0.0)
    toolkit.watch_variable("current_loss", 0.0)
    
    # Simulate some work with profiling
    print("\nProfiling function execution...")
    
    @toolkit.profile_function
    def complex_computation(x, iterations=1000):
        """Complex computation for benchmarking"""
        result = 0
        for i in range(iterations):
            result += x * (i % 100) * 0.01
        return result
    
    # Benchmark the function
    print("Benchmarking complex computation...")
    bench_result = toolkit.benchmark(complex_computation, 5, name="complex_computation", iterations=10)
    
    print(f"Benchmark results:")
    print(f"  - Mean time: {bench_result['mean_time']:.6f}s")
    print(f"  - Throughput: {bench_result['throughput']:.2f} calls/s")
    print(f"  - Min time: {bench_result['min_time']:.6f}s")
    print(f"  - Max time: {bench_result['max_time']:.6f}s")
    
    # Simulate error handling
    print("\nSimulating error handling...")
    
    def problematic_function():
        """Function that simulates errors"""
        time.sleep(0.1)
        
        # Simulate different types of errors
        if time.time() % 3 < 1:
            raise ValueError("Invalid input parameter")
        elif time.time() % 3 < 2:
            raise RuntimeError("Resource allocation failed")
        else:
            raise MemoryError("Insufficient memory")
    
    # Run problematic function and handle errors
    for i in range(5):
        print(f"\nAttempt {i + 1}:")
        try:
            result = problematic_function()
            print(f"  Success: {result}")
        except Exception as e:
            # Handle with toolkit
            result = toolkit.handle_exception(e, {"attempt": i + 1, "component": "problematic_function"})
            print(f"  Error handled: {result.get('handled', False)}")
            print(f"  Error type: {result.get('error_type', 'unknown')}")
    
    # Get performance summary
    print("\nGetting performance summary...")
    perf_summary = toolkit.get_performance_summary(time_window=60)
    print(f"Performance summary:")
    print(f"  - Total samples: {perf_summary.get('samples_count', 0)}")
    print(f"  - Average CPU: {perf_summary.get('cpu', {}).get('mean', 0):.2f}%")
    print(f"  - Peak CPU: {perf_summary.get('cpu', {}).get('max', 0):.2f}%")
    print(f"  - Average memory: {perf_summary.get('memory', {}).get('mean', 0):.2f}%")
    
    # Get debug log
    print("\nGetting debug log...")
    debug_log = toolkit.get_debug_log(limit=10, level='info')
    print(f"Debug log entries: {len(debug_log)}")
    
    # Export debug data
    print("\nExporting debug data...")
    toolkit.export_debug_data("debug_output.json")
    print("Debug data exported successfully")
    
    # Stop monitoring
    print("\nStopping monitoring...")
    toolkit.stop_monitoring()
    
    return toolkit

if __name__ == "__main__":
    debugging_example()
```

---

6. Performance Optimization
-------------------------

### 6.1 Performance Optimization Strategies

1. **Model Optimization**
   - Use appropriate model architectures for the task
   - Implement batch normalization and dropout
   - Use learning rate scheduling
   - Apply gradient clipping

2. **Data Optimization**
   - Use efficient data loaders
   - Implement data augmentation
   - Use mixed precision training
   - Optimize memory usage

3. **Hardware Optimization**
   - Use GPU acceleration
   - Optimize thread pool size
   - Configure memory allocation
   - Use efficient data structures

### 6.2 Benchmarking

```python
# Benchmark different operations
from nlm_debug_module import PerformanceMonitor

monitor = PerformanceMonitor()

# Benchmark function execution
result = monitor.benchmark_function(
    some_function, 
    arg1, arg2, 
    name="some_function",
    iterations=100,
    warmup_iterations=10
)

# Profile code execution
profile_result = monitor.profile_code("""
import time
def compute():
    time.sleep(0.1)
    return 42
compute()
""")

# Get performance summary
summary = monitor.get_performance_summary(time_window=3600)  # Last hour
```

### 6.3 Memory Optimization

```python
# Monitor memory usage
def get_memory_usage():
    import psutil
    return psutil.virtual_memory().percent

# Use memory optimizer
from nlm_debug_module import MemoryOptimizer

optimizer = MemoryOptimizer()
optimizer.optimize_memory()

# Get memory statistics
stats = optimo.get_stats()
```

---

7. Troubleshooting
------------------

### 7.1 Common Issues and Solutions

1. **Module Import Errors**
   ```
   # Error: cannot import name 'SomeClass'
   
   Solution: Check if the class exists in the module and update imports.
   ```

2. **Performance Issues**
   ```
   # Error: Simulation too slow
   
   Solution: 
   - Reduce model size
   - Increase batch size
   - Use GPU acceleration
   - Optimize hyperparameters
   ```

3. **Memory Issues**
   ```
   # Error: Memory limit exceeded
   
   Solution:
   - Clear unused variables
   - Use garbage collection
   - Reduce batch size
   - Use memory-efficient data structures
   ```

4. **Configuration Errors**
   ```
   # Error: Invalid configuration
   
   Solution:
   - Check configuration file format
   - Validate required parameters
   - Use default configuration
   ```

### 7.2 Debugging Techniques

1. **Use Debugging Tools**
   ```python
   toolkit = NLPToolkit()
   toolkit.set_breakpoint("my_breakpoint", condition=lambda msg, data, comp: "error" in msg)
   toolkit.watch_variable("my_var", 0)
   ```

2. **Profile Performance**
   ```python
   result = toolkit.benchmark(my_function, name="my_function", iterations=10)
   ```

3. **Handle Exceptions**
   ```python
   try:
       my_function()
   except Exception as e:
       toolkit.handle_exception(e, {"context": "my_context"})
   ```

4. **Monitor System Resources**
   ```python
   status = toolkit.get_system_status()
   print(f"CPU usage: {status['system']['cpu']}%})
   print(f"Memory usage: {status['system']['memory']}%})
   ```

---

8. Extending NLM with Python
--------------------------

### 8.1 Custom Neural Network Architectures

```python
from nlm_ml_module import CustomNeuralNetwork, ModelArchitecture

class CustomNetwork(CustomNeuralNetwork):
    def __init__(self, input_dim, output_dim, **kwargs):
        super().__init__(input_dim, output_dim, **kwargs)
    
    def _build_model(self):
        # Build custom architecture
        pass
    
    def train(self, X_train, y_train, **kwargs):
        # Custom training logic
        pass
    
    def predict(self, X):
        # Custom prediction logic
        pass
```

### 8.2 Custom Learning Algorithms

```python
class CustomLearningAlgorithm:
    def __init__(self, **kwargs):
        self.params = kwargs
    
    def fit(self, X, y):
        # Custom learning algorithm
        pass
    
    def predict(self, X):
        # Custom prediction
        pass
```

### 8.3 Plugin Architecture

```python
# Create custom plugins
def my_plugin(simulation, data):
    # Plugin implementation
    return processed_data

# Register plugin
toolkit.register_plugin("my_plugin", my_plugin)
```

---

9. Installation and Setup
-------------------------

### 9.1 System Requirements

- **Operating System**: Linux, macOS, Windows
- **Python**: 3.8 or higher
- **Memory**: 8GB RAM (minimum), 16GB RAM (recommended)
- **Storage**: 1GB free space
- **Compiler**: C++ compiler with C++20 support

### 9.2 Quick Installation

```bash
# Clone repository
git clone https://github.com/your-repo/nlm.git
cd nlm

# Build and install
pip install -e .

# Install additional dependencies
pip install numpy pandas matplotlib seaborn scipy scikit-learn tensorflow
```

### 9.3 Development Installation

```bash
# For development, install in editable mode with all dependencies
pip install -e .[dev]
```

### 9.4 Testing

```bash
# Run tests
pytest tests/

# Run specific test modules
pytest tests/test_nlm_python_module.py
pytest tests/test_nlm_configuration_module.py
pytest tests/test_nlm_analysis_module.py
```

---

10. API Reference Summary
-------------------------

### Core Classes

#### NLPSimulation
- Main simulation controller
- Neural network configuration
- Real-time simulation control
- Experiment management

#### NetworkArchitecture
- Neural network architecture definition
- Region and connection management
- Dynamic configuration

#### NLMConfigurator
- Comprehensive configuration management
- Parameter optimization
- Real-time tuning
- Configuration persistence

#### NLPAnalysisSuite
- Neural activity analysis
- Network topology analysis
- Learning progress analysis
- Visualization generation

#### NLPToolkit
- Error handling and recovery
- Performance monitoring and benchmarking
- Debugging and profiling
- System-wide monitoring

### Configuration and Optimization

#### PlasticityController
- STDP plasticity configuration
- Hebbian plasticity configuration
- Structural plasticity configuration
- Reward-modulated plasticity configuration

#### NeuromodulationTuner
- Dopamine neuromodulation tuning
- Curiosity neuromodulation tuning
- Novelty detection tuning
- Reward prediction error tuning

#### ParameterOptimizer
- Gradient descent optimization
- Random search optimization
- Bayesian optimization
- Cross-validation

### Machine Learning

#### DeepLearningFramework
- Feed-forward neural networks
- Recurrent neural networks
- Transformer models
- Model training and evaluation

#### NLPAutoML
- Automated model selection
- Hyperparameter optimization
- Feature engineering
- Model validation

### Debugging and Performance

#### ErrorHandler
- Exception handling and recovery
- Error categorization and severity
- Retry mechanisms
- Error history management

#### Debugger
- Breakpoint management
- Watch variable tracking
- Line profiling
- Memory profiling

#### PerformanceMonitor
- Function benchmarking
- Code profiling
- Performance metrics
- System monitoring

---

Conclusion
----------

The NLM Python module provides a comprehensive set of tools for advanced brain emulation,
neural network configuration, simulation control, and data analysis. It transforms
the NLM project from a basic simulator into a sophisticated programmable platform
suitable for research, education, and production applications.

Key features include:

- Comprehensive Python bindings for all NLM core components
- Advanced scripting capabilities for neural network configuration
- Real-time simulation control and parameter tuning
- Machine learning integration and custom algorithm support
- Error handling, debugging tools, and performance benchmarking
- Comprehensive documentation and examples
- Production-ready performance optimization

The module is designed to be extensible, allowing users to add custom neural network
architectures, learning algorithms, and plugins to extend functionality.

---

For more information, please visit the NLM documentation website or contact the NLM team.
