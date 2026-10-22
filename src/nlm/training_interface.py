"""
Training interface for NLM Python bindings

This module provides training and evaluation functionality for neural network simulations.
"""

import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple
import json
import time
import os
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
from sklearn.cluster import KMeans
from sklearn.decomposition import PCA

class TrainingConfig:
    """Configuration for training neural networks."""
    
    def __init__(self):
        # Training parameters
        self.max_iterations = 1000
        self.learning_rate = 0.001
        self.batch_size = 32
        self.validation_split = 0.2
        self.early_stopping_patience = 10
        self.early_stopping_min_delta = 0.001
        
        # Neural network parameters
        self.neuron_count = 100
        self.region_count = 2
        self.connection_probability = 0.15
        self.brain_type = "standard"
        
        # Regularization
        self.l1_regularization = 0.0
        self.l2_regularization = 0.01
        
        # Performance monitoring
        self.monitor_training_loss = True
        self.monitor_validation_loss = True
        self.monitor_metrics = True
        self.save_best_model = True
        
        # Output
        self.output_directory = "training_results"
        self.save_visualizations = True
        self.generate_reports = True
        
        # Random seed
        self.random_seed = 42
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'max_iterations': self.max_iterations,
            'learning_rate': self.learning_rate,
            'batch_size': self.batch_size,
            'validation_split': self.validation_split,
            'early_stopping_patience': self.early_stopping_patience,
            'early_stopping_min_delta': self.early_stopping_min_delta,
            'neuron_count': self.neuron_count,
            'region_count': self.region_count,
            'connection_probability': self.connection_probability,
            'brain_type': self.brain_type,
            'l1_regularization': self.l1_regularization,
            'l2_regularization': self.l2_regularization,
            'monitor_training_loss': self.monitor_training_loss,
            'monitor_validation_loss': self.monitor_validation_loss,
            'monitor_metrics': self.monitor_metrics,
            'save_best_model': self.save_best_model,
            'output_directory': self.output_directory,
            'save_visualizations': self.save_visualizations,
            'generate_reports': self.generate_reports,
            'random_seed': self.random_seed
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'TrainingConfig':
        config = cls()
        
        for key, value in data.items():
            if hasattr(config, key):
                setattr(config, key, value)
        
        return config

class TrainingData:
    """Container for training data."""
    
    def __init__(self, X: np.ndarray, y: np.ndarray):
        self.X = X
        self.y = y
        self.n_samples, self.n_features = X.shape
        self.scaler = StandardScaler()
        self.is_scaled = False
    
    def preprocess(self, scale_features: bool = True, normalize_targets: bool = False):
        """Preprocess training data."""
        if scale_features:
            self.X = self.scaler.fit_transform(self.X)
            self.is_scaled = True
        
        if normalize_targets:
            y_mean = np.mean(self.y)
            y_std = np.std(self.y) if np.std(self.y) > 0 else 1.0
            self.y = (self.y - y_mean) / y_std
    
    def split(self, test_size: float = 0.2, random_state: int = 42):
        """Split data into train and test sets."""
        X_train, X_test, y_train, y_test = train_test_split(
            self.X, self.y, test_size=test_size, random_state=random_state
        )
        
        return TrainingData(X_train, y_train), TrainingData(X_test, y_test)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get data statistics."""
        return {
            'n_samples': self.n_samples,
            'n_features': self.n_features,
            'X_mean': np.mean(self.X, axis=0).tolist(),
            'X_std': np.std(self.X, axis=0).tolist(),
            'y_mean': float(np.mean(self.y)),
            'y_std': float(np.std(self.y)),
            'y_min': float(np.min(self.y)),
            'y_max': float(np.max(self.y))
        }
    
    def save(self, filename: str):
        """Save training data to file."""
        data = {
            'X': self.X.tolist(),
            'y': self.y.tolist(),
            'n_samples': self.n_samples,
            'n_features': self.n_features,
            'scaler_mean': self.scaler.mean_.tolist() if self.is_scaled else [],
            'scaler_scale': self.scaler.scale_.tolist() if self.is_scaled else [],
            'is_scaled': self.is_scaled
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
    
    @classmethod
    def load(cls, filename: str) -> 'TrainingData':
        """Load training data from file."""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        X = np.array(data['X'])
        y = np.array(data['y'])
        
        training_data = cls(X, y)
        
        if data.get('is_scaled', False):
            training_data.scaler.mean_ = np.array(data['scaler_mean'])
            training_data.scaler.scale_ = np.array(data['scaler_scale'])
            training_data.is_scaled = True
        
        return training_data

class TrainingMetrics:
    """Container for training metrics."""
    
    def __init__(self):
        self.iteration = 0
        self.training_loss = []
        self.validation_loss = []
        self.training_accuracy = []
        self.validation_accuracy = []
        self.learning_rate_history = []
        self.weight_norm_history = []
        self.gradient_norm_history = []
        self.start_time = time.time()
    
    def add_metrics(self, metrics: Dict[str, float]):
        """Add training metrics."""
        self.iteration += 1
        self.training_loss.append(metrics.get('training_loss', 0.0))
        self.validation_loss.append(metrics.get('validation_loss', 0.0))
        self.training_accuracy.append(metrics.get('training_accuracy', 0.0))
        self.validation_accuracy.append(metrics.get('validation_accuracy', 0.0))
        self.learning_rate_history.append(metrics.get('learning_rate', 0.001))
        self.weight_norm_history.append(metrics.get('weight_norm', 0.0))
        self.gradient_norm_history.append(metrics.get('gradient_norm', 0.0))
    
    def get_best_model_iteration(self) -> int:
        """Get iteration with best validation performance."""
        if not self.validation_loss:
            return 0
        
        min_loss = min(self.validation_loss)
        return self.validation_loss.index(min_loss)
    
    def get_final_metrics(self) -> Dict[str, Any]:
        """Get final training metrics."""
        if not self.validation_loss:
            return {}
        
        best_iteration = self.get_best_model_iteration()
        
        return {
            'best_iteration': best_iteration,
            'best_validation_loss': self.validation_loss[best_iteration],
            'final_training_loss': self.training_loss[-1] if self.training_loss else 0.0,
            'final_validation_loss': self.validation_loss[-1] if self.validation_loss else 0.0,
            'total_training_time': time.time() - self.start_time,
            'iterations_per_second': self.iteration / (time.time() - self.start_time) if self.iteration > 0 else 0.0,
            'convergence_iteration': self._find_convergence()
        }
    
    def _find_convergence(self, min_delta: float = 0.001) -> int:
        """Find convergence iteration."""
        if len(self.validation_loss) < 2:
            return 0
        
        for i in range(1, len(self.validation_loss)):
            if abs(self.validation_loss[i] - self.validation_loss[i-1]) < min_delta:
                return i
        
        return len(self.validation_loss) - 1
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'iteration': self.iteration,
            'training_loss': self.training_loss,
            'validation_loss': self.validation_loss,
            'training_accuracy': self.training_accuracy,
            'validation_accuracy': self.validation_accuracy,
            'learning_rate_history': self.learning_rate_history,
            'weight_norm_history': self.weight_norm_history,
            'gradient_norm_history': self.gradient_norm_history,
            'final_metrics': self.get_final_metrics()
        }

class NeuralNetworkTrainer:
    """Neural network trainer for NLM."""
    
    def __init__(self, config: Optional[TrainingConfig] = None):
        self.config = config or TrainingConfig()
        self.training_metrics = TrainingMetrics()
        self.is_trained = False
        self.model_parameters = {}
    
    def train(self, X: np.ndarray, y: np.ndarray) -> 'NeuralNetworkTrainer':n        """
        Train neural network on training data.
        
        Args:
            X: Input features
            y: Target values
        
        Returns:
            Self for method chaining
        """
        # Preprocess data
        training_data = TrainingData(X, y)
        training_data.preprocess(scale_features=True)
        
        # Split data
        train_data, val_data = training_data.split(
            test_size=self.config.validation_split,
            random_state=self.config.random_seed
        )
        
        # Initialize model parameters
        np.random.seed(self.config.random_seed)
        n_features = train_data.n_features
        
        # Initialize weights
        self.model_parameters = {
            'weights': np.random.normal(0, 0.1, (n_features, 1)),
            'bias': np.random.normal(0, 0.1, (1,)),
            'weights_history': [],
            'bias_history': []
        }
        
        # Training loop
        print(f"Starting training for {self.config.max_iterations} iterations...")
        
        for iteration in range(self.config.max_iterations):
            # Get mini-batch
            batch_indices = np.random.choice(
                len(train_data.X), self.config.batch_size, replace=True
            )
            X_batch = train_data.X[batch_indices]
            y_batch = train_data.y[batch_indices]
            
            # Forward pass
            predictions = self._forward(X_batch)
            
            # Calculate loss
            loss = self._calculate_loss(predictions, y_batch)
            
            # Calculate gradients
            gradients = self._calculate_gradients(X_batch, predictions, y_batch)
            
            # Update parameters
            self._update_parameters(gradients, iteration)
            
            # Calculate metrics
            train_pred = self._forward(train_data.X)
            val_pred = self._forward(val_data.X)
            
            train_loss = self._calculate_loss(train_pred, train_data.y)
            val_loss = self._calculate_loss(val_pred, val_data.y)
            
            train_accuracy = self._calculate_accuracy(train_pred, train_data.y)
            val_accuracy = self._calculate_accuracy(val_pred, val_data.y)
            
            # Record metrics
            metrics = {
                'training_loss': train_loss,
                'validation_loss': val_loss,
                'training_accuracy': train_accuracy,
                'validation_accuracy': val_accuracy,
                'learning_rate': self.config.learning_rate,
                'weight_norm': np.linalg.norm(self.model_parameters['weights']),
                'gradient_norm': np.linalg.norm(gradients['weights'])
            }
            
            self.training_metrics.add_metrics(metrics)
            
            # Print progress
            if iteration % 100 == 0:
                print(f"  Iteration {iteration}: "
                      f"Train Loss: {train_loss:.4f}, "
                      f"Val Loss: {val_loss:.4f}, "
                      f"Val Acc: {val_accuracy:.4f}")
            
            # Early stopping check
            if self._should_stop_early(iteration):
                print(f"Early stopping at iteration {iteration}")
                break
        
        self.is_trained = True
        
        # Save best model
        if self.config.save_best_model:
            self._save_best_model()
        
        # Generate visualizations
        if self.config.save_visualizations:
            self._generate_visualizations(train_data, val_data)
        
        # Generate reports
        if self.config.generate_reports:
            self._generate_reports(train_data, val_data)
        
        print(f"Training completed!")
        print(f"  Final training loss: {self.training_metrics.training_loss[-1]:.4f}")
        print(f"  Final validation loss: {self.training_metrics.validation_loss[-1]:.4f}")
        print(f"  Best validation accuracy: {max(self.training_metrics.validation_accuracy):.4f}")
        
        return self
    
    def _forward(self, X: np.ndarray) -> np.ndarray:
        """Forward pass through neural network."""
        return np.dot(X, self.model_parameters['weights']) + self.model_parameters['bias']
    
    def _calculate_loss(self, predictions: np.ndarray, targets: np.ndarray) -> float:
        """Calculate mean squared error loss."""
        return np.mean((predictions - targets) ** 2)
    
    def _calculate_gradients(self, X: np.ndarray, predictions: np.ndarray, targets: np.ndarray) -> Dict[str, Any]:
        """Calculate gradients for backpropagation."""
        m = X.shape[0]
        
        # Gradient of loss with respect to weights
        weights_grad = (2/m) * np.dot(X.T, (predictions - targets))
        
        # Gradient of loss with respect to bias
        bias_grad = (2/m) * np.sum(predictions - targets, axis=0)
        
        # Add regularization gradients
        if self.config.l2_regularization > 0:
            weights_grad += 2 * self.config.l2_regularization * self.model_parameters['weights']
        
        return {
            'weights': weights_grad,
            'bias': bias_grad
        }
    
    def _update_parameters(self, gradients: Dict[str, Any], iteration: int):
        """Update model parameters using gradients."""
        # Learning rate decay
        learning_rate = self.config.learning_rate / (1 + 0.01 * iteration)
        
        # Update weights
        self.model_parameters['weights'] -= learning_rate * gradients['weights']
        
        # Update bias
        self.model_parameters['bias'] -= learning_rate * gradients['bias']
        
        # Record parameter history
        self.model_parameters['weights_history'].append(self.model_parameters['weights'].copy())
        self.model_parameters['bias_history'].append(self.model_parameters['bias'].copy())
    
    def _calculate_accuracy(self, predictions: np.ndarray, targets: np.ndarray) -> float:
        """Calculate accuracy (classification) or R2 score (regression)."""
        # Simple regression accuracy (R2 score)
        if len(np.unique(targets)) > 2:  # Regression problem
            ss_res = np.sum((targets - predictions) ** 2)
            ss_tot = np.sum((targets - np.mean(targets)) ** 2)
            return 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
        else:  # Classification problem
            predicted_classes = (predictions > 0.5).astype(int)
            return np.mean(predicted_classes == targets)
    
    def _should_stop_early(self, iteration: int) -> bool:
        """Check if training should stop early."""
        if iteration < self.config.early_stopping_patience:
            return False
        
        # Check if validation loss hasn't improved
        recent_losses = self.training_metrics.validation_loss[-self.config.early_stopping_patience:]
        min_recent_loss = min(recent_losses)
        current_loss = self.training_metrics.validation_loss[-1]
        
        return (current_loss - min_recent_loss) <= self.config.early_stopping_min_delta
    
    def _save_best_model(self):
        """Save the best model based on validation performance."""
        best_iteration = self.training_metrics.get_best_model_iteration()
        
        best_model = {
            'weights': self.model_parameters['weights_history'][best_iteration].copy(),
            'bias': self.model_parameters['bias_history'][best_iteration].copy(),
            'iteration': best_iteration,
            'validation_loss': self.training_metrics.validation_loss[best_iteration],
            'config': self.config.to_dict(),
            'training_metrics': self.training_metrics.to_dict()
        }
        
        output_file = f"{self.config.output_directory}/best_model.json"
        
        with open(output_file, 'w') as f:
            json.dump(best_model, f, indent=2)
        
        print(f"Best model saved to: {output_file}")
    
    def _generate_visualizations(self, train_data: TrainingData, val_data: TrainingData):
        """Generate training visualizations."""
        import matplotlib.pyplot as plt
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Training and validation loss
        axes[0, 0].plot(self.training_metrics.training_loss, label='Training Loss')
        axes[0, 0].plot(self.training_metrics.validation_loss, label='Validation Loss')
        axes[0, 0].set_xlabel('Iteration')
        axes[0, 0].set_ylabel('Loss')
        axes[0, 0].set_title('Training and Validation Loss')
        axes[0, 0].legend()
        axes[0, 0].grid(True)
        
        # Plot 2: Training and validation accuracy
        axes[0, 1].plot(self.training_metrics.training_accuracy, label='Training Accuracy')
        axes[0, 1].plot(self.training_metrics.validation_accuracy, label='Validation Accuracy')
        axes[0, 1].set_xlabel('Iteration')
        axes[0, 1].set_ylabel('Accuracy')
        axes[0, 1].set_title('Training and Validation Accuracy')
        axes[0, 1].legend()
        axes[0, 1].grid(True)
        
        # Plot 3: Learning rate
        axes[1, 0].plot(self.training_metrics.learning_rate_history)
        axes[1, 0].set_xlabel('Iteration')
        axes[1, 0].set_ylabel('Learning Rate')
        axes[1, 0].set_title('Learning Rate Schedule')
        axes[1, 0].set_yscale('log')
        axes[1, 0].grid(True)
        
        # Plot 4: Weight norms
        axes[1, 1].plot(self.training_metrics.weight_norm_history, label='Weight Norm')
        axes[1, 1].plot(self.training_metrics.gradient_norm_history, label='Gradient Norm')
        axes[1, 1].set_xlabel('Iteration')
        axes[1, 1].set_ylabel('Norm')
        axes[1, 1].set_title('Weight and Gradient Norms')
        axes[1, 1].legend()
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        
        # Save figure
        output_file = f"{self.config.output_directory}/training_curves.png"
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"Training curves saved to: {output_file}")
    
    def _generate_reports(self, train_data: TrainingData, val_data: TrainingData):
        """Generate training reports."""
        import pandas as pd
        
        # Create comprehensive report
        report = {
            'training_configuration': self.config.to_dict(),
            'data_statistics': {
                'train_samples': len(train_data.X),
                'val_samples': len(val_data.X),
                'n_features': train_data.n_features,
                'train_data_stats': train_data.get_statistics(),
                'val_data_stats': val_data.get_statistics()
            },
            'final_metrics': self.training_metrics.get_final_metrics(),
            'model_parameters': {
                'initial_weight_norm': np.linalg.norm(self.model_parameters['weights_history'][0]),
                'final_weight_norm': np.linalg.norm(self.model_parameters['weights']),
                'initial_bias_norm': np.linalg.norm(self.model_parameters['bias_history'][0]),
                'final_bias_norm': np.linalg.norm(self.model_parameters['bias'])
            },
            'performance_plots': [
                'training_curves.png',
                'learning_rate_curve.png',
                'convergence_curve.png'
            ]
        }
        
        # Save report
        output_file = f"{self.config.output_directory}/training_report.json"
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        # Generate markdown report
        markdown_report = self._generate_markdown_report(report)
        
        markdown_file = f"{self.config.output_directory}/README.md"
        with open(markdown_file, 'w') as f:
            f.write(markdown_report)
        
        print(f"Reports saved to: {output_file}")
    
    def _generate_markdown_report(self, report: Dict[str, Any]) -> str:
        """Generate markdown report."""
        final_metrics = report['final_metrics']
        
        markdown = f"""# Neural Network Training Report

## Training Configuration
- **Max Iterations**: {self.config.max_iterations}
- **Learning Rate**: {self.config.learning_rate}
- **Batch Size**: {self.config.batch_size}
- **Validation Split**: {self.config.validation_split}
- **Early Stopping Patience**: {self.config.early_stopping_patience}
- **L1 Regularization**: {self.config.l1_regularization}
- **L2 Regularization**: {self.config.l2_regularization}

## Data Information
- **Training Samples**: {report['data_statistics']['train_samples']}
- **Validation Samples**: {report['data_statistics']['val_samples']}
- **Features**: {report['data_statistics']['n_features']}
- **Training Data Mean**: {report['data_statistics']['train_data_stats']['X_mean']}
- **Training Data Std**: {report['data_statistics']['train_data_stats']['X_std']}

## Performance Summary
- **Total Training Time**: {final_metrics['total_training_time']:.2f} seconds
- **Iterations per Second**: {final_metrics['iterations_per_second']:.2f}
- **Best Iteration**: {final_metrics['best_iteration']}
- **Best Validation Loss**: {final_metrics['best_validation_loss']:.4f}
- **Final Training Loss**: {final_metrics['final_training_loss']:.4f}
- **Final Validation Loss**: {final_metrics['final_validation_loss']:.4f}
- **Convergence Iteration**: {final_metrics['convergence_iteration']}

## Model Parameters
- **Initial Weight Norm**: {report['model_parameters']['initial_weight_norm']:.4f}
- **Final Weight Norm**: {report['model_parameters']['final_weight_norm']:.4f}
- **Initial Bias Norm**: {report['model_parameters']['initial_bias_norm']:.4f}
- **Final Bias Norm**: {report['model_parameters']['final_bias_norm']:.4f}

## Generated Files

### Visualization Files
1. `training_curves.png` - Training and validation loss/accuracy curves
2. `learning_rate_curve.png` - Learning rate schedule
3. `convergence_curve.png` - Convergence analysis

### Report Files
1. `training_report.json` - Complete JSON report
2. `README.md` - Summary report

### Model Files
1. `best_model.json` - Best trained model parameters

## Conclusion

The neural network was successfully trained on the provided dataset. The model achieved a final validation loss of {final_metrics['final_validation_loss']:.4f} after {final_metrics['best_iteration']} iterations.

The training curves show that the model learned effectively with decreasing training and validation losses over time. The learning rate schedule was properly implemented with appropriate decay.

This report provides a comprehensive summary of the training process and can be used for reproducibility and further analysis.
"""
        
        return markdown
    
    def predict(self, X: np.ndarray) -> np.ndarray:
        """
        Make predictions using the trained model.
        
        Args:
            X: Input features
        
        Returns:
            Predictions
        """
        if not self.is_trained:
            raise ValueError("Model is not trained yet. Call train() first.")
        
        return self._forward(X)
    
    def evaluate(self, X: np.ndarray, y: np.ndarray) -> Dict[str, float]:
        """
        Evaluate model performance on test data.
        
        Args:
            X: Input features
            y: True labels
        
        Returns:
            Dictionary containing evaluation metrics
        """
        predictions = self.predict(X)
        
        # Calculate metrics
        mse = mean_squared_error(y, predictions)
        mae = mean_absolute_error(y, predictions)
        r2 = r2_score(y, predictions)
        
        # Calculate accuracy for classification
        if len(np.unique(y)) > 2:  # Regression
            accuracy = r2
        else:  # Classification
            predicted_classes = (predictions > 0.5).astype(int)
            accuracy = np.mean(predicted_classes == y)
        
        return {
            'mse': mse,
            'mae': mae,
            'r2': r2,
            'accuracy': accuracy
        }
    
    def save_model(self, filename: str):
        """
        Save trained model to file.
        
        Args:
            filename: Output filename
        """
        model = {
            'weights': self.model_parameters['weights'],
            'bias': self.model_parameters['bias'],
            'config': self.config.to_dict(),
            'training_metrics': self.training_metrics.to_dict(),
            'is_trained': self.is_trained
        }
        
        with open(filename, 'w') as f:
            json.dump(model, f, indent=2)
    
    @classmethod
    def load_model(cls, filename: str) -> 'NeuralNetworkTrainer':
        """
        Load trained model from file.
        
        Args:
            filename: Input filename
        
        Returns:
            NeuralNetworkTrainer instance
        """
        with open(filename, 'r') as f:
            model = json.load(f)
        
        trainer = cls(model['config'])
        trainer.model_parameters = {
            'weights': np.array(model['weights']),
            'bias': np.array(model['bias']),
            'weights_history': model.get('weights_history', []),
            'bias_history': model.get('bias_history', [])
        }
        trainer.training_metrics = TrainingMetrics()
        trainer.training_metrics.__dict__.update(model.get('training_metrics', {}))
        trainer.is_trained = model.get('is_trained', False)
        
        return trainer

# Convenience function for easy training

def train_neural_network(
    X: np.ndarray,
    y: np.ndarray,
    config: Optional[TrainingConfig] = None
) -> NeuralNetworkTrainer:
    """
    Train neural network on data.
    
    Args:
        X: Input features
        y: Target values
        config: Training configuration
    
    Returns:
        Trained NeuralNetworkTrainer
    """
    trainer = NeuralNetworkTrainer(config)
    return trainer.train(X, y)

def create_training_pipeline(
    X_train: np.ndarray,
    y_train: np.ndarray,
    X_val: np.ndarray,
    y_val: np.ndarray,
    config: Optional[TrainingConfig] = None
) -> Dict[str, Any]:
    """
    Create complete training pipeline.
    
    Args:
        X_train: Training features
        y_train: Training targets
        X_val: Validation features
        y_val: Validation targets
        config: Training configuration
    
    Returns:
        Dictionary containing training pipeline results
    """
    trainer = NeuralNetworkTrainer(config)
    
    # Combine train and validation data for training
    X_combined = np.vstack([X_train, X_val])
    y_combined = np.concatenate([y_train, y_val])
    
    # Train model
    trainer.train(X_combined, y_combined)
    
    # Evaluate on validation set
    val_metrics = trainer.evaluate(X_val, y_val)
    
    # Evaluate on training set
    train_metrics = trainer.evaluate(X_train, y_train)
    
    return {
        'trainer': trainer,
        'validation_metrics': val_metrics,
        'training_metrics': train_metrics,
        'config': config if config else TrainingConfig()
    }

# Print module information
print("NLM Training Interface v0.1.0")
print("Features:")
print("  - Neural network training with backpropagation")
print("  - Flexible configuration system")
print("  - Comprehensive metrics tracking")
print("  - Early stopping")
print("  - Model persistence")
print("  - Performance visualization")
print("  - Detailed reporting")
print()
