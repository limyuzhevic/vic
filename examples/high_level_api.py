# High-level neural network builder for NLM
# Provides scikit-learn style APIs and machine learning abstractions

import pynlm
from typing import List, Dict, Optional, Callable, Union
from dataclasses import dataclass, field
import time
import json
from pathlib import Path

@dataclass
class NeuralNetworkConfig:
    """Configuration for a neural network model."""
    name: str = "NLM_NeuralNetwork"
    neuron_count: int = 1000
    region_count: int = 3
    connection_probability: float = 0.1
    learning_rate: float = 0.01
    epochs: int = 100
    batch_size: int = 32
    target_performance: float = 0.9
    random_seed: int = 42
    save_path: Optional[str] = None
    load_path: Optional[str] = None
    metadata: Dict[str, any] = field(default_factory=dict)

@dataclass
class TrainingMetrics:
    """Track training metrics over time."""
    epoch: int
    accuracy: float
    loss: float
    firing_rate: float
    spike_count: int
    time_taken: float
    performance_score: float

class NeuralNetwork:
    """
    High-level neural network API mimicking scikit-learn style.
    Provides ML-style training, validation, and persistence.
    """
    
    def __init__(self, config: Optional[NeuralNetworkConfig] = None):
        """
        Initialize neural network.
        
        Args:
            config: Neural network configuration
        """
        self.config = config or NeuralNetworkConfig()
        self.brain = None
        self.training_history: List[TrainingMetrics] = []
        self.validation_history: List[TrainingMetrics] = []
        self.is_fitted = False
        
    def build(self) -> 'NeuralNetwork':
        """Build the neural network architecture."""
        print(f"Building neural network: {self.config.name}")
        print(f"  - Neurons: {self.config.neuron_count}")
        print(f"  - Regions: {self.config.region_count}")
        print(f"  - Connection probability: {self.config.connection_probability}")
        
        # Create configuration
        config_dict = {
            "random_seed": self.config.random_seed,
            "neuron_count": self.config.neuron_count,
            "region_count": self.config.region_count,
            "connection_probability": self.config.connection_probability,
            "stdp_ltp_weight": self.config.learning_rate,
            "stdp_ltd_weight": self.config.learning_rate * 0.9,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "replay_interval": 100,
            "consolidation_interval": 1000
        }
        
        self.config_obj = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            self.config_obj.set(key, value)
            
        # Create brain
        self.brain = pynlm.createBrain(self.config_obj)
        self.brain.initialize()
        
        print(f"  - Network built successfully!")
        print(f"  - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {self.brain.getTotalSynapseCount()}")
        
        return self
    
    def fit(self, X: List[Dict], y: List[float], validation_data: Optional[tuple] = None,
            epochs: Optional[int] = None, callbacks: Optional[List[Callable]] = None) -> 'NeuralNetwork':
        """
        Train the neural network (supervised learning style).
        
        Args:
            X: List of input samples (sensory observations)
            y: List of target values
            validation_data: Optional validation data tuple (X_val, y_val)
            epochs: Number of training epochs
            callbacks: List of callback functions
            
        Returns:
            Self for method chaining
        """
        if self.brain is None:
            raise RuntimeError("Network not built. Call build() first.")
            
        epochs = epochs or self.config.epochs
        print(f"Training neural network for {epochs} epochs...")
        
        for epoch in range(epochs):
            epoch_start = time.time()
            epoch_loss = 0.0
            epoch_accuracy = 0.0
            
            print(f"  Epoch {epoch + 1}/{epochs}")
            
            for i, (sample, target) in enumerate(zip(X, y)):
                # Simulate learning from sample
                # In a real implementation, this would involve:
                # 1. Creating sensory input from sample
                # 2. Injecting into brain
                # 3. Running simulation steps
                # 4. Applying reward/dopamine based on error
                
                error = abs(target - 0.5)  # Simulated error
                epoch_loss += error
                epoch_accuracy += 1.0 - error  # Simulated accuracy
                
                # Inject current based on sample
                if isinstance(sample, dict) and 'vision' in sample:
                    vision_data = sample['vision']
                    if isinstance(vision_data, list):
                        # Create sensory input and inject
                        pass
                        
            # Calculate metrics
            avg_loss = epoch_loss / len(X) if X else 0
            avg_accuracy = epoch_accuracy / len(X) if X else 0
            
            # Get brain statistics
            spike_count = self.brain.getTotalSpikeCount()
            firing_rate = self.brain.getAverageFiringRate()
            performance_score = avg_accuracy * self.config.target_performance
            
            # Create metrics record
            metrics = TrainingMetrics(
                epoch=epoch + 1,
                accuracy=avg_accuracy,
                loss=avg_loss,
                firing_rate=firing_rate,
                spike_count=spike_count,
                time_taken=time.time() - epoch_start,
                performance_score=performance_score
            )
            
            self.training_history.append(metrics)
            
            print(f"    - Loss: {avg_loss:.4f}, Accuracy: {avg_accuracy:.4f}, "
                  f"Firing rate: {firing_rate:.2f}, Performance: {performance_score:.4f}")
            
            # Callbacks
            if callbacks:
                for callback in callbacks:
                    callback({
                        'epoch': epoch + 1,
                        'loss': avg_loss,
                        'accuracy': avg_accuracy,
                        'brain': self.brain
                    })
                    
        self.is_fitted = True
        print(f"Training completed! Final accuracy: {avg_accuracy:.4f}")
        return self
    
    def predict(self, X: List[Dict]) -> List[float]:
        """
        Make predictions on input data.
        
        Args:
            X: List of input samples
            
        Returns:
            List of predictions
        """
        if not self.is_fitted:
            raise RuntimeError("Model not trained. Call fit() first.")
            
        predictions = []
        for sample in X:
            # Simulate prediction
            # In a real implementation, this would involve:
            # 1. Creating sensory input
            # 2. Injecting into brain
            # 3. Running simulation
            # 4. Reading motor output/activity
            
            prediction = 0.5 + (hash(str(sample)) % 1000) / 2000.0  # Simulated prediction
            predictions.append(prediction)
            
        return predictions
    
    def evaluate(self, X: List[Dict], y: List[float]) -> Dict[str, float]:
        """
        Evaluate model performance on test data.
        
        Args:
            X: List of input samples
            y: List of true values
            
        Returns:
            Dictionary of evaluation metrics
        """
        predictions = self.predict(X)
        
        mse = sum((p - t) ** 2 for p, t in zip(predictions, y)) / len(predictions)
        mae = sum(abs(p - t) for p, t in zip(predictions, y)) / len(predictions)
        rmse = mse ** 0.5
        
        # Calculate performance score
        accuracy = sum(1.0 - abs(p - t) for p, t in zip(predictions, y)) / len(predictions)
        
        return {
            'mse': mse,
            'mae': mae,
            'rmse': rmse,
            'accuracy': accuracy,
            'loss': mse,
            'r2_score': 1.0 - mse / (sum((t - sum(y)/len(y)) ** 2 for t in y) if len(y) > 1 else 1.0)
        }
    
    def save(self, path: Optional[str] = None) -> None:
        """
        Save model to file.
        
        Args:
            path: File path (uses config.save_path if None)
        """
        save_path = path or self.config.save_path
        if not save_path:
            raise ValueError("No save path specified")
            
        # Save configuration
        config_dict = {
            'name': self.config.name,
            'neuron_count': self.config.neuron_count,
            'region_count': self.config.region_count,
            'connection_probability': self.config.connection_probability,
            'learning_rate': self.config.learning_rate,
            'epochs': self.config.epochs,
            'batch_size': self.config.batch_size,
            'target_performance': self.config.target_performance,
            'random_seed': self.config.random_seed,
            'metadata': self.config.metadata,
            'training_history': [
                {
                    'epoch': m.epoch,
                    'accuracy': m.accuracy,
                    'loss': m.loss,
                    'firing_rate': m.firing_rate,
                    'spike_count': m.spike_count,
                    'time_taken': m.time_taken,
                    'performance_score': m.performance_score
                }
                for m in self.training_history
            ],
            'validation_history': [
                {
                    'epoch': m.epoch,
                    'accuracy': m.accuracy,
                    'loss': m.loss,
                    'firing_rate': m.firing_rate,
                    'spike_count': m.spike_count,
                    'time_taken': m.time_taken,
                    'performance_score': m.performance_score
                }
                for m in self.validation_history
            ]
        }
        
        with open(save_path, 'w') as f:
            json.dump(config_dict, f, indent=2)
            
        print(f"Model saved to {save_path}")
    
    def load(self, path: Optional[str] = None) -> 'NeuralNetwork':
        """
        Load model from file.
        
        Args:
            path: File path (uses config.load_path if None)
            
        Returns:
            Self for method chaining
        """
        load_path = path or self.config.load_path
        if not load_path:
            raise ValueError("No load path specified")
            
        with open(load_path, 'r') as f:
            config_dict = json.load(f)
            
        # Update configuration
        self.config.name = config_dict.get('name', self.config.name)
        self.config.neuron_count = config_dict.get('neuron_count', self.config.neuron_count)
        self.config.region_count = config_dict.get('region_count', self.config.region_count)
        self.config.connection_probability = config_dict.get('connection_probability', self.config.connection_probability)
        self.config.learning_rate = config_dict.get('learning_rate', self.config.learning_rate)
        self.config.epochs = config_dict.get('epochs', self.config.epochs)
        self.config.random_seed = config_dict.get('random_seed', self.config.random_seed)
        self.config.metadata = config_dict.get('metadata', {})
        
        # Load training history
        self.training_history = []
        for history_dict in config_dict.get('training_history', []):
            metrics = TrainingMetrics(**history_dict)
            self.training_history.append(metrics)
            
        self.validation_history = []
        for history_dict in config_dict.get('validation_history', []):
            metrics = TrainingMetrics(**history_dict)
            self.validation_history.append(metrics)
            
        self.is_fitted = len(self.training_history) > 0
        
        print(f"Model loaded from {load_path}")
        return self
    
    def get_summary(self) -> str:
        """Get model summary."""
        if self.brain is None:
            return "Model not built yet."
            
        summary = f"Neural Network Summary:\n"
        summary += f"  Name: {self.config.name}\n"
        summary += f"  Neurons: {self.config.neuron_count}\n"
        summary += f"  Regions: {self.config.region_count}\n"
        summary += f"  Connection probability: {self.config.connection_probability}\n"
        summary += f"  Learning rate: {self.config.learning_rate}\n"
        summary += f"  Target performance: {self.config.target_performance}\n"
        
        if self.is_fitted:
            final_metrics = self.training_history[-1]
            summary += f"\n  Status: Fitted\n"
            summary += f"  Final accuracy: {final_metrics.accuracy:.4f}\n"
            summary += f"  Final loss: {final_metrics.loss:.4f}\n"
            summary += f"  Training epochs: {len(self.training_history)}\n"
        else:
            summary += f"\n  Status: Not fitted\n"
            
        return summary
    
    def __repr__(self) -> str:
        return f"NeuralNetwork(name='{self.config.name}', neurons={self.config.neuron_count})"


# Additional utility functions
def create_from_config_file(filepath: str, **kwargs) -> NeuralNetwork:
    """Create neural network from configuration file."""
    import json
    
    with open(filepath, 'r') as f:
        config_dict = json.load(f)
        
    config = NeuralNetworkConfig(**config_dict)
    
    # Override with kwargs
    for key, value in kwargs.items():
        if hasattr(config, key):
            setattr(config, key, value)
            
    return NeuralNetwork(config).build()


def benchmark_networks(network_configs: List[NeuralNetworkConfig], X: List[Dict], y: List[float],
                       n_runs: int = 3) -> List[Dict]:
    """
    Benchmark multiple neural network configurations.
    
    Args:
        network_configs: List of network configurations to test
        X: Training data
        y: Training labels
        n_runs: Number of runs for each configuration
        
    Returns:
        List of benchmark results
    """
    results = []
    
    for config in network_configs:
        print(f"Benchmarking {config.name}...")
        
        config_times = []
        config_accuracies = []
        config_losses = []
        
        for run in range(n_runs):
            start_time = time.time()
            
            network = NeuralNetwork(config).build()
            network.fit(X, y, epochs=10)  # Use fewer epochs for benchmarking
            metrics = network.evaluate(X, y)
            
            end_time = time.time()
            
            config_times.append(end_time - start_time)
            config_accuracies.append(metrics['accuracy'])
            config_losses.append(metrics['loss'])
            
        avg_time = sum(config_times) / len(config_times)
        avg_accuracy = sum(config_accuracies) / len(config_accuracies)
        avg_loss = sum(config_losses) / len(config_losses)
        
        result = {
            'config': config,
            'avg_time': avg_time,
            'avg_accuracy': avg_accuracy,
            'avg_loss': avg_loss,
            'times': config_times,
            'accuracies': config_accuracies,
            'losses': config_losses
        }
        
        results.append(result)
        
        print(f"  Average time: {avg_time:.2f}s")
        print(f"  Average accuracy: {avg_accuracy:.4f}")
        print(f"  Average loss: {avg_loss:.4f}")
        
    return results