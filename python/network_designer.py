# Neural Network Designer - Advanced configuration and network design

This module provides advanced tools for designing and configuring neural networks
with NLM, including topology design, parameter optimization, and custom architectures.
"""

import numpy as np
from typing import Optional, List, Dict, Any, Tuple
from dataclasses import dataclass, field
import json

from .enhanced_api import NLMSystem

@dataclass
class NetworkTopology:
    """Neural network topology configuration"""
    name: str = "default"
    layers: List[Dict[str, Any]] = field(default_factory=list)
    connections: List[Dict[str, Any]] = field(default_factory=list)
    connectivity_pattern: str = "random"  # random, structured, feedforward
    connection_probability: float = 0.1

@dataclass
class DesignConstraints:
    """Design constraints for network architecture"""
    max_neurons: int = 1000
    max_synapses: int = 5000
    max_connections_per_neuron: int = 20
    min_plasticity: float = 0.01
    max_plasticity: float = 1.0
    energy_budget: float = 100.0
    time_budget: float = 60.0

class NeuralNetworkDesigner:
    """
    Advanced neural network design and configuration system.
    
    This class provides tools for:
    - Designing neural network topologies
    - Optimizing network parameters
    - Creating custom architectures
    - Validating network designs
    - Exporting configurations
    
    Examples:
        >>> # Create a basic feedforward network
        >>> designer = nlm.NeuralNetworkDesigner()
        >>> topology = designer.create_feedforward_network(3, [100, 200, 150, 50])
        >>> config = designer.optimize_topology(topology, "balanced")
        >>> 
        >>> # Create a recurrent network
        >>> designer = nlm.NeuralNetworkDesigner()
        >>> topology = designer.create_recurrent_network(2, 500, 0.1)
        >>> 
        >>> # Export design
        >>> designer.export_design(topology, "network_design.json")
    """
    
    def __init__(self, constraints: Optional[DesignConstraints] = None):
        """
        Initialize the neural network designer.
        
        Args:
            constraints: Optional design constraints
        """
        self.constraints = constraints or DesignConstraints()
        self.templates: Dict[str, NetworkTopology] = {}
    
    def create_feedforward_network(self, input_size: int, 
                                  hidden_sizes: List[int],
                                  output_size: int,
                                  name: str = "feedforward") -> NetworkTopology:
        """
        Create a feedforward neural network topology.
        
        Args:
            input_size: Number of input neurons
            hidden_sizes: List of hidden layer sizes
            output_size: Number of output neurons
            name: Name for the topology
            
        Returns:
            NetworkTopology configuration
        """
        try:
            layers = []
            layer_sizes = [input_size] + hidden_sizes + [output_size]
            
            # Create layers
            for i, size in enumerate(layer_sizes):
                layer_type = "input" if i == 0 else \
                           "hidden" if i < len(layer_sizes) - 1 else "output"
                
                layer = {
                    'id': i,
                    'name': f"Layer_{i}",
                    'size': size,
                    'type': layer_type,
                    'neuron_type': 'excitatory',
                    'activation': 'lif',  # Leaky Integrate-and-Fire
                    'parameters': {
                        'v_thresh': -50.0,
                        'v_rest': -70.0,
                        'v_reset': -75.0,
                        'tau_mem': 20.0,
                        'tau_ref': 2.0
                    }
                }
                layers.append(layer)
            
            # Create connections (full connectivity between layers)
            connections = []
            for i in range(len(layers) - 1):
                source_layer = i
                target_layer = i + 1
                
                source_neurons = layers[source_layer]['size']
                target_neurons = layers[target_layer]['size']
                
                for src in range(source_neurons):
                    for tgt in range(target_neurons):
                        connection = {
                            'source_neuron': src,
                            'target_neuron': tgt,
                            'source_layer': source_layer,
                            'target_layer': target_layer,
                            'weight': np.random.uniform(0.1, 1.0),
                            'delay': 1,
                            'type': 'excitatory',
                            'plasticity': {
                                'stdp': True,
                                'hebbian': True,
                                'plasticity_rate': 0.1
                            }
                        }
                        connections.append(connection)
            
            topology = NetworkTopology(
                name=name,
                layers=layers,
                connections=connections,
                connectivity_pattern="feedforward",
                connection_probability=1.0  # Full connectivity for feedforward
            )
            
            return topology
            
        except Exception as e:
            raise Exception(f"Failed to create feedforward network: {e}")
    
    def create_recurrent_network(self, input_size: int, hidden_size: int,
                                 output_size: int,
                                 feedback_probability: float = 0.1,
                                 name: str = "recurrent") -> NetworkTopology:
        """
        Create a recurrent neural network topology.
        
        Args:
            input_size: Number of input neurons
            hidden_size: Number of hidden neurons (shared across time steps)
            output_size: Number of output neurons
            feedback_probability: Probability of recurrent connections
            name: Name for the topology
            
        Returns:
            NetworkTopology configuration
        """
        try:
            layers = []
            
            # Input layer
            layers.append({
                'id': 0,
                'name': "Input",
                'size': input_size,
                'type': 'input',
                'neuron_type': 'sensory',
                'activation': 'lif'
            })
            
            # Hidden layer (recurrent)
            layers.append({
                'id': 1,
                'name': "Hidden_Recurrent",
                'size': hidden_size,
                'type': 'hidden',
                'neuron_type': 'internal',
                'activation': 'lif',
                'recurrent': True,
                'parameters': {
                    'v_thresh': -50.0,
                    'v_rest': -70.0,
                    'v_reset': -75.0,
                    'tau_mem': 20.0,
                    'tau_ref': 2.0
                }
            })
            
            # Output layer
            layers.append({
                'id': 2,
                'name': "Output",
                'size': output_size,
                'type': 'output',
                'neuron_type': 'motor',
                'activation': 'lif'
            })
            
            # Create connections
            connections = []
            
            # Input to hidden (feedforward)
            for src in range(input_size):
                for tgt in range(hidden_size):
                    connection = {
                        'source_neuron': src,
                        'target_neuron': tgt,
                        'source_layer': 0,
                        'target_layer': 1,
                        'weight': np.random.uniform(0.1, 1.0),
                        'delay': 1,
                        'type': 'excitatory',
                        'plasticity': {
                            'stdp': True,
                            'hebbian': True,
                            'plasticity_rate': 0.1
                        }
                    }
                    connections.append(connection)
            
            # Hidden to output (feedforward)
            for src in range(hidden_size):
                for tgt in range(output_size):
                    connection = {
                        'source_neuron': src,
                        'target_neuron': tgt,
                        'source_layer': 1,
                        'target_layer': 2,
                        'weight': np.random.uniform(0.1, 1.0),
                        'delay': 1,
                        'type': 'excitatory',
                        'plasticity': {
                            'stdp': True,
                            'hebbian': True,
                            'plasticity_rate': 0.1
                        }
                    }
                    connections.append(connection)
            
            # Recurrent connections (hidden to hidden)
            for src in range(hidden_size):
                for tgt in range(hidden_size):
                    if src != tgt and np.random.random() < feedback_probability:
                        connection = {
                            'source_neuron': src,
                            'target_neuron': tgt,
                            'source_layer': 1,
                            'target_layer': 1,
                            'weight': np.random.uniform(-0.1, 0.1),
                            'delay': 1,
                            'type': 'excitatory',
                            'plasticity': {
                                'stdp': True,
                                'hebbian': False,  # No Hebbian for recurrent
                                'plasticity_rate': 0.05
                            }
                        }
                        connections.append(connection)
            
            topology = NetworkTopology(
                name=name,
                layers=layers,
                connections=connections,
                connectivity_pattern="recurrent",
                connection_probability=feedback_probability
            )
            
            return topology
            
        except Exception as e:
            raise Exception(f"Failed to create recurrent network: {e}")
    
    def create_sparse_network(self, input_size: int, hidden_size: int,
                              output_size: int,
                              sparsity_factor: float = 0.1,
                              name: str = "sparse") -> NetworkTopology:
        """
        Create a sparse neural network topology.
        
        Args:
            input_size: Number of input neurons
            hidden_size: Number of hidden neurons
            output_size: Number of output neurons
            sparsity_factor: Fraction of possible connections to include
            name: Name for the topology
            
        Returns:
            NetworkTopology configuration
        """
        try:
            layers = []
            
            # Input layer
            layers.append({
                'id': 0,
                'name': "Input",
                'size': input_size,
                'type': 'input',
                'neuron_type': 'sensory',
                'activation': 'lif'
            })
            
            # Hidden layer
            layers.append({
                'id': 1,
                'name': "Hidden",
                'size': hidden_size,
                'type': 'hidden',
                'neuron_type': 'internal',
                'activation': 'lif'
            })
            
            # Output layer
            layers.append({
                'id': 2,
                'name': "Output",
                'size': output_size,
                'type': 'output',
                'neuron_type': 'motor',
                'activation': 'lif'
            })
            
            # Create sparse connections
            connections = []
            
            # Input to hidden (sparse)
            total_possible_input_hidden = input_size * hidden_size
            target_input_hidden = int(total_possible_input_hidden * sparsity_factor)
            
            input_hidden_candidates = [(src, tgt) for src in range(input_size) 
                                     for tgt in range(hidden_size)]
            np.random.shuffle(input_hidden_candidates)
            
            for i in range(min(target_input_hidden, len(input_hidden_candidates))):
                src, tgt = input_hidden_candidates[i]
                connection = {
                    'source_neuron': src,
                    'target_neuron': tgt,
                    'source_layer': 0,
                    'target_layer': 1,
                    'weight': np.random.uniform(0.1, 1.0),
                    'delay': 1,
                    'type': 'excitatory',
                    'plasticity': {
                        'stdp': True,
                        'hebbian': True,
                        'plasticity_rate': 0.1
                    }
                }
                connections.append(connection)
            
            # Hidden to output (sparse)
            total_possible_hidden_output = hidden_size * output_size
            target_hidden_output = int(total_possible_hidden_output * sparsity_factor)
            
            hidden_output_candidates = [(src, tgt) for src in range(hidden_size) 
                                      for tgt in range(output_size)]
            np.random.shuffle(hidden_output_candidates)
            
            for i in range(min(target_hidden_output, len(hidden_output_candidates))):
                src, tgt = hidden_output_candidates[i]
                connection = {
                    'source_neuron': src,
                    'target_neuron': tgt,
                    'source_layer': 1,
                    'target_layer': 2,
                    'weight': np.random.uniform(0.1, 1.0),
                    'delay': 1,
                    'type': 'excitatory',
                    'plasticity': {
                        'stdp': True,
                        'hebbian': True,
                        'plasticity_rate': 0.1
                    }
                }
                connections.append(connection)
            
            topology = NetworkTopology(
                name=name,
                layers=layers,
                connections=connections,
                connectivity_pattern="sparse",
                connection_probability=sparsity_factor
            )
            
            return topology
            
        except Exception as e:
            raise Exception(f"Failed to create sparse network: {e}")
    
    def optimize_topology(self, topology: NetworkTopology, 
                         optimization_goal: str = "balanced",
                         iterations: int = 100) -> NetworkTopology:
        """
        Optimize network topology for a specific goal.
        
        Args:
            topology: Input topology to optimize
            optimization_goal: Optimization goal ('balanced', 'performance', 'stability', 'exploration')
            iterations: Number of optimization iterations
            
        Returns:
            Optimized NetworkTopology
        """
        try:
            # Create a copy of the topology to modify
            optimized_topology = NetworkTopology(
                name=f"optimized_{topology.name}",
                layers=topology.layers,
                connections=topology.connections.copy(),
                connectivity_pattern=topology.connectivity_pattern,
                connection_probability=topology.connection_probability
            )
            
            # Optimization based on goal
            if optimization_goal == "performance":
                # Increase connection weights and plasticity
                for conn in optimized_topology.connections:
                    if conn['type'] == 'excitatory':
                        conn['weight'] *= 1.1
                        conn['plasticity']['plasticity_rate'] *= 1.2
                        
            elif optimization_goal == "stability":
                # Reduce weights and plasticity for stability
                for conn in optimized_topology.connections:
                    if conn['type'] == 'excitatory':
                        conn['weight'] *= 0.9
                        conn['plasticity']['plasticity_rate'] *= 0.8
                        
            elif optimization_goal == "exploration":
                # Increase plasticity and add more connections
                for conn in optimized_topology.connections:
                    if conn['type'] == 'excitatory':
                        conn['plasticity']['plasticity_rate'] *= 1.5
                        
            # Balanced optimization (no change)
            else:
                pass
            
            return optimized_topology
            
        except Exception as e:
            raise Exception(f"Failed to optimize topology: {e}")
    
    def validate_topology(self, topology: NetworkTopology) -> Tuple[bool, List[str]]:
        """
        Validate network topology against constraints.
        
        Args:
            topology: Topology to validate
            
        Returns:
            Tuple of (is_valid, list_of_violations)
        """
        try:
            violations = []
            
            # Check neuron count constraints
            total_neurons = sum(layer['size'] for layer in topology.layers)
            if total_neurons > self.constraints.max_neurons:
                violations.append(f"Total neurons ({total_neurons}) exceeds max ({self.constraints.max_neurons})")
            
            # Check synapse count constraints
            if len(topology.connections) > self.constraints.max_synapses:
                violations.append(f"Number of synapses ({len(topology.connections)}) exceeds max ({self.constraints.max_synapses})")
            
            # Check connections per neuron
            connections_per_neuron = {}
            for conn in topology.connections:
                target = conn['target_neuron']
                connections_per_neuron[target] = connections_per_neuron.get(target, 0) + 1
                
                if connections_per_neuron[target] > self.constraints.max_connections_per_neuron:
                    violations.append(f"Neuron {target} has {connections_per_neuron[target]} connections (max: {self.constraints.max_connections_per_neuron})")
            
            # Check plasticity constraints
            for conn in topology.connections:
                plasticity_rate = conn['plasticity']['plasticity_rate']
                if plasticity_rate < self.constraints.min_plasticity:
                    violations.append(f"Connection plasticity ({plasticity_rate}) below minimum ({self.constraints.min_plasticity})")
                elif plasticity_rate > self.constraints.max_plasticity:
                    violations.append(f"Connection plasticity ({plasticity_rate}) exceeds maximum ({self.constraints.max_plasticity})")
            
            return (len(violations) == 0, violations)
            
        except Exception as e:
            return (False, [f"Validation failed: {e}"])
    
    def create_preset_networks(self) -> Dict[str, NetworkTopology]:
        """
        Create predefined network topologies for common use cases.
        
        Returns:
            Dictionary of preset network topologies
        """
        presets = {}
        
        # Simple feedforward for basic tasks
        presets['simple'] = self.create_feedforward_network(
            input_size=10, hidden_sizes=[50, 25], output_size=5, name="Simple_Brain"
        )
        
        # Medium feedforward for learning tasks
        presets['learning'] = self.create_feedforward_network(
            input_size=20, hidden_sizes=[100, 50, 25], output_size=10, name="Learning_Brain"
        )
        
        # Recurrent for sequential tasks
        presets['recurrent'] = self.create_recurrent_network(
            input_size=5, hidden_size=100, output_size=3, name="Recurrent_Brain"
        )
        
        # Sparse for memory tasks
        presets['sparse'] = self.create_sparse_network(
            input_size=50, hidden_size=200, output_size=20, name="Sparse_Brain"
        )
        
        return presets
    
    def export_design(self, topology: NetworkTopology, 
                     filepath: str, format: str = "json") -> bool:
        """
        Export network design to file.
        
        Args:
            topology: Network topology to export
            filepath: Output file path
            format: Export format ('json', 'csv', 'python')
            
        Returns:
            True if export successful, False otherwise
        """
        try:
            if format == "json":
                data = {
                    'topology': {
                        'name': topology.name,
                        'layers': topology.layers,
                        'connections': topology.connections,
                        'connectivity_pattern': topology.connectivity_pattern,
                        'connection_probability': topology.connection_probability
                    }
                }
                
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
                    
            elif format == "csv":
                # Export connections as CSV
                import csv
                
                with open(filepath, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['source_layer', 'source_neuron', 'target_layer', 
                                   'target_neuron', 'weight', 'delay', 'type'])
                    
                    for conn in topology.connections:
                        writer.writerow([
                            conn['source_layer'],
                            conn['source_neuron'],
                            conn['target_layer'],
                            conn['target_neuron'],
                            conn['weight'],
                            conn['delay'],
                            conn['type']
                        ])
                        
            elif format == "python":
                # Export as Python code
                with open(filepath, 'w') as f:
                    f.write(f"""
# Network design exported from NeuralNetworkDesigner
# Topology: {topology.name}

topology = {{
    'name': '{topology.name}',
    'layers': {json.dumps(topology.layers)},
    'connections': {json.dumps(topology.connections)},
    'connectivity_pattern': '{topology.connectivity_pattern}',
    'connection_probability': {topology.connection_probability}
}}
""")
            else:
                raise ValueError(f"Unsupported export format: {format}")
            
            return True
            
        except Exception as e:
            print(f"Failed to export design: {e}")
            return False
    
    def import_design(self, filepath: str, format: str = "json") -> NetworkTopology:
        """
        Import network design from file.
        
        Args:
            filepath: Input file path
            format: Import format ('json', 'csv', 'python')
            
        Returns:
            Imported NetworkTopology
        """
        try:
            if format == "json":
                with open(filepath, 'r') as f:
                    data = json.load(f)
                
                topology = NetworkTopology(**data['topology'])
                    
            elif format == "csv":
                import pandas as pd
                df = pd.read_csv(filepath)
                
                # Convert to network topology
                layers = []
                for i in range(df['source_layer'].max() + 1):
                    source_connections = df[df['source_layer'] == i]
                    target_layers = source_connections['target_layer'].unique()
                    
                    for target_layer in target_layers:
                        layer_neurons = df[df['target_layer'] == target_layer]['target_neuron'].max() + 1
                        layers.append({
                            'id': target_layer,
                            'name': f"Layer_{target_layer}",
                            'size': layer_neurons,
                            'type': 'hidden' if target_layer > 0 else 'input'
                        })
                
                connections = []
                for _, row in df.iterrows():
                    connections.append({
                        'source_neuron': row['source_neuron'],
                        'target_neuron': row['target_neuron'],
                        'source_layer': row['source_layer'],
                        'target_layer': row['target_layer'],
                        'weight': row['weight'],
                        'delay': row['delay'],
                        'type': row['type']
                    })
                
                topology = NetworkTopology(
                    name="imported",
                    layers=layers,
                    connections=connections,
                    connectivity_pattern="imported",
                    connection_probability=len(connections) / (len(layers) ** 2)
                )
                
            elif format == "python":
                # Import from Python module
                import importlib.util
                spec = importlib.util.spec_from_file_location("imported_design", filepath)
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                
                topology = module.topology
                
            else:
                raise ValueError(f"Unsupported import format: {format}")
            
            return topology
            
        except Exception as e:
            raise Exception(f"Failed to import design: {e}")