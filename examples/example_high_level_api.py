#!/usr/bin/env python3
"""
Example usage of NLM high-level APIs.

This file demonstrates the usage of NLM's high-level APIs for neural networks,
experiment management, and machine learning-style workflows.

The high-level APIs provide:
- Scikit-learn style neural network API
- Experiment tracking and management
- Machine learning abstractions
- Performance monitoring
- Model persistence
"""

import pynlm
from examples.high_level_api import NeuralNetwork, NeuralNetworkConfig, benchmark_networks
import random

def simple_neural_network_example():
    """Example 1: Simple neural network creation and training."""
    print("=" * 60)
    print("Example 1: Simple Neural Network")
    print("=" * 60)
    
    # Create configuration
    config = NeuralNetworkConfig(
        name="MyFirstNN",
        neuron_count=500,
        region_count=2,
        connection_probability=0.15,
        learning_rate=0.05,
        epochs=20,
        target_performance=0.8
    )
    
    # Create and build neural network
    nn = NeuralNetwork(config).build()
    print(f"\n{nn.get_summary()}")
    
    # Generate synthetic training data
    print("\nGenerating synthetic training data...")
    X = []
    y = []
    for i in range(100):
        # Create random sensory input
        sample = {
            'vision': [random.random() for _ in range(10)],
            'internal': [random.random() for _ in range(5)],
            'timestamp': i
        }
        target = random.random()  # Random target value
        X.append(sample)
        y.append(target)
    
    # Train the network
    print("\nTraining neural network...")
    nn.fit(X, y, epochs=10)
    
    # Evaluate on training data
    print("\nEvaluating model...")
    metrics = nn.evaluate(X, y)
    print(f"  MSE: {metrics['mse']:.4f}")
    print(f"  MAE: {metrics['mae']:.4f}")
    print(f"  Accuracy: {metrics['accuracy']:.4f}")
    print(f"  R² Score: {metrics['r2_score']:.4f}")
    
    # Save the model
    print("\nSaving model...")
    nn.save("examples/saved_model.json")
    
    print("\nExample 1 completed successfully!\n")


def quick_start_example():
    """Example 2: Quick start with default configuration."""
    print("=" * 60)
    print("Example 2: Quick Start")
    print("=" * 60)
    
    # Quick start with minimal configuration
    nn = NeuralNetwork().build()
    print(f"\n{nn.get_summary()}")
    
    # Generate training data
    X = [{'value': random.random()} for _ in range(50)]
    y = [x['value'] * 0.8 + random.random() * 0.2 for x in X]  # Linear relationship with noise
    
    # Train
    nn.fit(X, y, epochs=15)
    
    # Make predictions
    test_samples = [{'value': 0.2}, {'value': 0.5}, {'value': 0.8}]
    predictions = nn.predict(test_samples)
    
    print("\nPredictions:")
    for sample, pred in zip(test_samples, predictions):
        print(f"  Input: {sample['value']:.1f} -> Predicted: {pred:.4f}")
    
    print("\nExample 2 completed successfully!\n")


def model_comparison_example():
    """Example 3: Compare different neural network configurations."""
    print("=" * 60)
    print("Example 3: Model Comparison")
    print("=" * 60)
    
    # Generate training data
    X = [{'features': [random.random() for _ in range(3)]} for _ in range(100)]
    y = [sum(x['features']) / 3 + random.random() * 0.1 for x in X]  # Average of features
    
    # Create different configurations
    configs = [
        NeuralNetworkConfig(name="SmallNN", neuron_count=200, region_count=1, learning_rate=0.01),
        NeuralNetworkConfig(name="MediumNN", neuron_count=500, region_count=2, learning_rate=0.05),
        NeuralNetworkConfig(name="LargeNN", neuron_count=1000, region_count=3, learning_rate=0.1)
    ]
    
    # Benchmark configurations
    print("Benchmarking different configurations...")
    results = benchmark_networks(configs, X, y, n_runs=2)
    
    # Display results
    print("\nBenchmark Results:")
    for result in results:
        config = result['config']
        print(f"\n{config.name}:")
        print(f"  Neurons: {config.neuron_count}, Regions: {config.region_count}")
        print(f"  Learning rate: {config.learning_rate}")
        print(f"  Average accuracy: {result['avg_accuracy']:.4f}")
        print(f"  Average loss: {result['avg_loss']:.4f}")
        print(f"  Average time: {result['avg_time']:.2f}s")
    
    print("\nExample 3 completed successfully!\n")


def advanced_usage_example():
    """Example 4: Advanced usage with callbacks and metrics."""
    print("=" * 60)
    print("Example 4: Advanced Usage")
    print("=" * 60)
    
    # Create neural network
    nn = NeuralNetwork()
    
    # Training callback to monitor progress
    def training_callback(metrics):
        print(f"  Callback: Epoch {metrics['epoch']}, "
              f"Accuracy: {metrics['accuracy']:.4f}, "
              f"Loss: {metrics['loss']:.4f}")
    
    # Generate training data
    X = [{'value': random.random()} for _ in range(200)]
    y = [x['value'] * 0.9 + random.random() * 0.1 for x in X]
    
    # Create validation data
    X_val = [{'value': random.random()} for _ in range(50)]
    y_val = [x['value'] * 0.9 + random.random() * 0.1 for x in X_val]
    
    print("Training with validation and callbacks...")
    
    # Train with validation
    nn.fit(X, y, validation_data=(X_val, y_val), epochs=25, callbacks=[training_callback])
    
    # Final evaluation
    print("\nFinal evaluation on test data...")
    train_metrics = nn.evaluate(X, y)
    val_metrics = nn.evaluate(X_val, y_val)
    
    print("\nTraining Metrics:")
    for key, value in train_metrics.items():
        print(f"  {key}: {value:.4f}")
        
    print("\nValidation Metrics:")
    for key, value in val_metrics.items():
        print(f"  {key}: {value:.4f}")
    
    # Save with metadata
    print("\nSaving with metadata...")
    nn.config.metadata = {
        'framework': 'pynlm',
        'version': '1.0',
        'architecture': 'FeedForward',
        'training_date': '2024-01-01',
        'author': 'Example User'
    }
    nn.save("examples/advanced_model.json")
    
    print("\nExample 4 completed successfully!\n")


def main():
    """Run all examples."""
    print("NLM High-Level API Examples")
    print("===========================")
    
    try:
        simple_neural_network_example()
        quick_start_example()
        model_comparison_example()
        advanced_usage_example()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        print("\nKey features demonstrated:")
        print("  • Neural network creation and training")
        print("  • Model persistence and loading")
        print("  • Training monitoring with callbacks")
        print("  • Performance benchmarking")
        print("  • Configuration management")
        print("  • Evaluation metrics")
        
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()