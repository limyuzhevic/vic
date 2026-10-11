# Tutorial 2: Setting Up Brain Networks

## Overview

This tutorial provides a comprehensive guide to setting up and configuring brain networks in NLM. It covers brain architecture, neuron types, region configuration, connectivity, and advanced brain setup techniques.

## Brain Network Fundamentals

### What is a Brain Network?

A brain network in NLM is a structured collection of neurons organized into functional regions. Each brain network represents a simplified model of a biological brain, with regions corresponding to different brain areas (cortex, hippocampus, motor areas, etc.) and neurons within regions performing specific computational functions.

### Key Components of a Brain Network:

1. **Regions:** Functional subdivisions of the brain
2. **Neurons:** Computational units that process and transmit information
3. **Synapses:** Connections between neurons that enable communication
4. **Plasticity Mechanisms:** Rules that modify synaptic strengths
5. **Simulation Engine:** Core system that manages neural dynamics

## Brain Network Architecture

### Hierarchical Structure

```
Brain Network
├── Regions
│   ├── Visual Cortex (sensory processing)
│   ├── Motor Cortex (movement planning)
│   ├── Hippocampus (memory formation)
│   ├── Prefrontal Cortex (decision making)
│   └── Other Regions
├── Neuron Population
│   ├── Excitatory Neurons (activate other neurons)
│   ├── Inhibitory Neurons (suppress firing)
│   ├── Intracortical Neurons (within-region connections)
│   └── Extracortical Neurons (between-region connections)
└── Synaptic Network
    ├── Local Synapses (within regions)
    ├── Feedforward Synapses (between regions)
    └── Feedback Synapses (recurrent connections)
```

## Creating Brain Networks

### 1. Basic Brain Network Setup

```python
import nlm
from nlm.brain import BrainNetwork

# Method 1: Using BrainNetwork class directly
def create_basic_brain(regions=3, neurons_per_region=100):
    """Create a basic brain network with specified regions."""
    
    # Create brain network
    brain = BrainNetwork()
    
    # Add regions
    for i in range(regions):
        region_name = f"Region_{i+1}"
        brain.add_region(region_name, neurons_per_region, "excitatory")
        print(f"Added {region_name} with {neurons_per_region} neurons")
    
    # Initialize brain
    brain.initialize()
    
    return brain

# Usage
basic_brain = create_basic_brain(3, 100)
print(f"Total neurons: {basic_brain.get_total_neuron_count()}")
print(f"Total regions: {basic_brain.get_region_count()}")
```

### 2. Advanced Brain Configuration

```python
import nlm
from nlm.brain import BrainNetwork
import numpy as np

class AdvancedBrainBuilder:
    def __init__(self):
        self.brain_config = {}
    
    def create_brain_from_config(self, config):
        """Create a brain network from detailed configuration."""
        
        self.brain_config = config
        brain = BrainNetwork(config)
        
        # Add regions based on configuration
        for region_name, region_config in config.get("regions", {}).items():
            neuron_count = region_config.get("neuron_count", 100)
            neuron_type = region_config.get("neuron_type", "excitatory")
            
            # Add region with custom properties
            self._add_region_with_properties(brain, region_name, neuron_count, neuron_type, region_config)
        
        # Initialize brain
        brain.initialize()
        
        # Add connections based on configuration
        self._setup_connections(brain, config)
        
        return brain
    
    def _add_region_with_properties(self, brain, name, neuron_count, neuron_type, config):
        """Add a brain region with custom properties."""
        
        # Create region configuration
        region_config = {
            "neuron_count": neuron_count,
            "neuron_type": neuron_type,
            "plasticity": config.get("plasticity", "stdp"),
            "special_properties": config.get("special_properties", {}),
            "input_sources": config.get("input_sources", []),
            "output_targets": config.get("output_targets", [])
        }
        
        # Add region to brain
        brain.add_region(name, region_config)
        
        # Set additional properties
        if "receptive_field" in config:
            brain.set_region_property(name, "receptive_field", config["receptive_field"])
        
        if "connection_density" in config:
            brain.set_region_property(name, "connection_density", config["connection_density"])
    
    def _setup_connections(self, brain, config):
        """Setup connections between brain regions."""
        
        connections_config = config.get("connections", {})
        
        for source_region, target_regions in connections_config.items():
            connection_strength = connections_config[source_region].get("strength", 0.1)
            connection_delay = connections_config[source_region].get("delay", 1.0)
            
            for target_region in target_regions:
                # Setup connection between regions
                brain.setup_region_connection(
                    source_region, target_region, 
                    connection_strength, connection_delay
                )
    
    def get_brain_statistics(self, brain):
        """Get comprehensive statistics about the brain network."""
        
        stats = {
            "total_regions": brain.get_region_count(),
            "total_neurons": brain.get_total_neuron_count(),
            "total_synapses": brain.get_total_synapse_count(),
            "connection_density": brain.get_connection_density(),
            "region_types": brain.get_region_types(),
            "neuron_types": brain.get_neuron_types(),
            "plasticity_types": brain.get_plasticity_types()
        }
        
        return stats

# Create advanced brain
advanced_config = {
    "regions": {
        "visual_cortex": {
            "neuron_count": 300,
            "neuron_type": "excitatory",
            "plasticity": "stdp",
            "receptive_field": 5,
            "input_sources": ["environment"],
            "output_targets": ["motor_cortex", "hippocampus"]
        },
        "motor_cortex": {
            "neuron_count": 200,
            "neuron_type": "motor",
            "plasticity": "hebbian",
            "special_properties": {"output_type": "action"},
            "input_sources": ["visual_cortex", "hippocampus"],
            "output_targets": ["environment"]
        },
        "hippocampus": {
            "neuron_count": 150,
            "neuron_type": "internal",
            "plasticity": "hebbian_ltp",
            "special_properties": {"memory_function": "episodic"},
            "input_sources": ["cortex"],
            "output_targets": ["cortex"]
        }
    },
    "connections": {
        "visual_cortex": {
            "targets": ["motor_cortex", "hippocampus"],
            "strength": 0.15,
            "delay": 2.0
        },
        "motor_cortex": {
            "targets": ["environment"],
            "strength": 0.2,
            "delay": 1.0
        },
        "hippocampus": {
            "targets": ["cortex"],
            "strength": 0.1,
            "delay": 5.0
        }
    }
}

brain_builder = AdvancedBrainBuilder()
advanced_brain = brain_builder.create_brain_from_config(advanced_config)
stats = brain_builder.get_brain_statistics(advanced_brain)

print("Advanced Brain Configuration:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### 3. Neuron Types and Properties

```python
import nlm
from nlm.brain import BrainNetwork

class NeuronConfig:
    def __init__(self):
        self.configs = {}
    
    def create_neuron_type(self, name, properties):
        """Create a custom neuron type with specific properties."""
        self.configs[name] = properties
        return self.configs[name]
    
    def get_neuron_properties(self, neuron_type):
        """Get properties for a specific neuron type."""
        return self.configs.get(neuron_type, {})

# Define neuron types
neuron_config = NeuronConfig()

# LIF (Leaky Integrate-and-Fire) neurons
lif_neuron = neuron_config.create_neuron_type("lif", {
    "threshold": -55.0,              # Membrane potential threshold
    "resting_potential": -70.0,     # Resting membrane potential
    "refractory_period": 5,          # Refractory period in ms
    "membrane_time_constant": 20.0, # Membrane time constant
    "reset_potential": -65.0,       # Reset potential after spike
    "adaptation_strength": 0.01,     # Adaptation strength
    "noise_level": 0.1              # Noise level
})

# IF (Integrate-and-Fire) neurons
if_neuron = neuron_config.create_neuron_type("if", {
    "threshold": -50.0,
    "resting_potential": -65.0,
    "refractory_period": 2,
    "membrane_time_constant": 10.0,
    "reset_potential": -60.0
})

# HH (Hodgkin-Huxley) neurons (biophysically detailed)
hh_neuron = neuron_config.create_neuron_type("hh", {
    "threshold": -54.0,
    "resting_potential": -65.0,
    "refractory_period": 1,
    "membrane_time_constant": 5.0,
    "reset_potential": -60.0,
    "ion_channels": {
        "sodium": 120,
        "potassium": 36,
        "leakage": 0.3
    },
    "capacitance": 1.0,
    "conductance": 0.3
})

# Create brain with different neuron types
brain = BrainNetwork()
brain.initialize()

# Add regions with different neuron types
brain.add_region("visual_cortex", 200, "lif")
brain.add_region("motor_cortex", 150, "if")
brain.add_region("hippocampus", 100, "hh")

print("Neuron types available:")
for neuron_type in neuron_config.configs.keys():
    print(f"  {neuron_type}: {neuron_config.get_neuron_properties(neuron_type)}")
```

### 4. Synaptic Connections and Plasticity

```python
import nlm
import numpy as np
from nlm.brain import BrainNetwork

class SynapticConnection:
    def __init__(self):
        self.connections = {}
    
    def create_connection(self, source_neuron, target_neuron, properties):
        """Create a synaptic connection between two neurons."""
        connection_id = f"{source_neuron}_{target_neuron}"
        
        self.connections[connection_id] = {
            "source_neuron": source_neuron,
            "target_neuron": target_neuron,
            "properties": properties,
            "weight": properties.get("initial_weight", 0.1),
            "delay": properties.get("delay", 1.0),
            "type": properties.get("type", "excitatory"),
            "plasticity": properties.get("plasticity", "static")
        }
        
        return connection_id
    
    def get_connection_strength(self, connection_id):
        """Get the strength of a synaptic connection."""
        return self.connections[connection_id]["weight"]
    
    def update_connection_strength(self, connection_id, new_strength):
        """Update the strength of a synaptic connection."""
        if connection_id in self.connections:
            self.connections[connection_id]["weight"] = new_strength
            return True
        return False

# Create brain and setup connections
brain = BrainNetwork()
brain.initialize()

# Create synaptic connection manager
synapse_manager = SynapticConnection()

# Create connections between regions
def setup_region_connections(brain, source_region, target_region, num_connections=50):
    """Setup connections between two regions."""
    
    # Get neurons from each region
    source_neurons = brain.get_region_neurons(source_region)
    target_neurons = brain.get_region_neurons(target_region)
    
    if len(source_neurons) == 0 or len(target_neurons) == 0:
        print(f"Warning: Cannot setup connections. No neurons in {source_region} or {target_region}")
        return
    
    # Create random connections
    for i in range(min(num_connections, len(source_neurons) * len(target_neurons))):
        source_neuron = np.random.choice(source_neurons)
        target_neuron = np.random.choice(target_neurons)
        
        connection_properties = {
            "initial_weight": np.random.uniform(0.01, 0.5),
            "delay": np.random.uniform(0.5, 5.0),
            "type": "excitatory" if np.random.random() > 0.8 else "inhibitory",
            "plasticity": "stdp" if np.random.random() > 0.5 else "hebbian"
        }
        
        connection_id = synapse_manager.create_connection(source_neuron, target_neuron, connection_properties)
        brain.add_synapse(connection_id)
        
        print(f"Created connection {connection_id}: {source_neuron} -> {target_neuron}")

# Setup connections between regions
setup_region_connections(brain, "Region_1", "Region_2", 100)
setup_region_connections(brain, "Region_2", "Region_3", 80)
setup_region_connections(brain, "Region_1", "Region_3", 60)

print(f"Total synapses in brain: {brain.get_total_synapse_count()}")
```

### 5. Brain Network Simulation

```python
import nlm
from nlm.brain import BrainNetwork
import time
import numpy as np

class BrainSimulator:
    def __init__(self, brain):
        self.brain = brain
        self.simulation_step = 0
        self.history = []
    
    def run_simulation(self, steps, dt=0.1):
        """Run a simulation for specified number of steps."""
        
        print(f"Starting simulation for {steps} steps (dt={dt}ms)")
        
        # Clear history
        self.history = []
        
        # Run simulation
        for step in range(steps):
            self.simulation_step = step
            
            # Get brain statistics
            stats = self.brain.get_statistics()
            stats["step"] = step
            stats["time"] = step * dt
            
            # Store statistics
            self.history.append(stats)
            
            # Perform simulation step
            self._simulation_step(dt)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}/{steps}: {stats['firing_rate']:.2f} Hz")
        
        print("Simulation completed")
        return self.history
    
    def _simulation_step(self, dt):
        """Perform a single simulation step."""
        
        # Update neuron states
        self.brain.update_neurons(dt)
        
        # Process synaptic connections
        self.brain.process_synapses(dt)
        
        # Apply plasticity rules
        self.brain.apply_plasticity(dt)
        
        # Check for termination conditions
        if self._should_terminate():
            self._terminate_simulation()
    
    def get_activity_summary(self):
        """Get summary of neural activity during simulation."""
        
        if not self.history:
            return None
        
        # Extract activity data
        activities = [entry["activity"] for entry in self.history]
        spike_counts = [entry["spike_count"] for entry in self.history]
        
        # Calculate statistics
        summary = {
            "total_steps": len(self.history),
            "total_spikes": sum(spike_counts),
            "average_activity": np.mean(activities),
            "peak_activity": np.max(activities),
            "activity_variance": np.var(activities),
            "firing_rate_range": (np.min(spike_counts), np.max(spike_counts)),
            "time_range": (0, (len(self.history) - 1) * 0.1)  # Assuming dt=0.1
        }
        
        return summary
    
    def _should_terminate(self):
        """Check if simulation should terminate."""
        # Add termination conditions here
        # For example: stability, convergence, or external trigger
        return False
    
    def _terminate_simulation(self):
        """Terminate the simulation."""
        print("Simulation terminated early")

# Create brain and setup simulator
brain = BrainNetwork({
    "regions": 2,
    "neurons_per_region": 100,
    "plasticity": "stdp"
})
brain.initialize()

# Setup connections
for i in range(brain.get_region_count() - 1):
    source = f"Region_{i+1}"
    target = f"Region_{i+2}"
    setup_region_connections(brain, source, target, 50)

# Create simulator and run simulation
simulator = BrainSimulator(brain)
history = simulator.run_simulation(steps=500, dt=0.1)

# Get activity summary
activity_summary = simulator.get_activity_summary()
if activity_summary:
    print("Simulation Summary:")
    for key, value in activity_summary.items():
        print(f"  {key}: {value}")
```

### 6. Advanced Brain Configuration Techniques

```python
import nlm
from nlm.brain import BrainNetwork

class AdvancedBrainConfiguration:
    def __init__(self):
        pass
    
    def create_topology(self, topology_type="random", config=None):
        """Create brain with specific connectivity topology."""
        
        if topology_type == "random":
            return self._create_random_topology(config)
        elif topology_type == "small_world":
            return self._create_small_world_topology(config)
        elif topology_type == "scale_free":
            return self._create_scale_free_topology(config)
        elif topology_type == "modular":
            return self._create_modular_topology(config)
        else:
            raise ValueError(f"Unknown topology type: {topology_type}")
    
    def _create_random_topology(self, config):
        """Create brain with random connectivity."""
        
        regions = config.get("regions", 3)
        neurons_per_region = config.get("neurons_per_region", 100)
        connection_probability = config.get("connection_probability", 0.1)
        
        brain = BrainNetwork()
        brain.initialize()
        
        # Add regions
        for i in range(regions):
            brain.add_region(f"Region_{i+1}", neurons_per_region, "excitatory")
        
        # Create random connections
        for i in range(regions):
            for j in range(i + 1, regions):
                source = f"Region_{i+1}"
                target = f"Region_{j+1}"
                
                for source_neuron in brain.get_region_neurons(source):
                    for target_neuron in brain.get_region_neurons(target):
                        if np.random.random() < connection_probability:
                            # Create synapse
                            synapse_id = f"{source_neuron}_{target_neuron}"
                            brain.add_synapse(synapse_id)
        
        return brain
    
    def _create_small_world_topology(self, config):
        """Create brain with small-world connectivity."""
        
        regions = config.get("regions", 3)
        neurons_per_region = config.get("neurons_per_region", 100)
        clustering_coefficient = config.get("clustering_coefficient", 0.3)
        path_length = config.get("path_length", 2.0)
        
        brain = BrainNetwork()
        brain.initialize()
        
        # Add regions
        for i in range(regions):
            brain.add_region(f"Region_{i+1}", neurons_per_region, "excitatory")
        
        # Create small-world connections
        # This is a simplified implementation
        for i in range(regions):
            source = f"Region_{i+1}"
            target = f"Region_{(i + 1) % regions}"  # Wrap around
            
            # Create high clustering within regions
            for neuron in brain.get_region_neurons(source):
                neighbors = np.random.choice(brain.get_region_neurons(source), 
                                           size=int(clustering_coefficient * neurons_per_region),
                                           replace=False)
                for neighbor in neighbors:
                    synapse_id = f"{neuron}_{neighbor}"
                    brain.add_synapse(synapse_id)
            
            # Create small-world connections between regions
            for neuron in brain.get_region_neurons(source):
                for target_neuron in brain.get_region_neurons(target):
                    if np.random.random() < path_length / regions:
                        synapse_id = f"{neuron}_{target_neuron}"
                        brain.add_synapse(synapse_id)
        
        return brain
    
    def create_brain_with_visualization(self, brain, output_file="brain_topology.png"):
        """Create visualization of brain topology."""
        
        try:
            import matplotlib.pyplot as plt
            import networkx as nx
            
            # Create network graph
            G = nx.Graph()
            
            # Add nodes (neurons)
            for region_name in brain.get_region_names():
                neurons = brain.get_region_neurons(region_name)
                for neuron in neurons:
                    G.add_node(neuron, region=region_name)
            
            # Add edges (synapses)
            for synapse_id in brain.get_all_synapses():
                source, target = synapse_id.split("_")
                G.add_edge(int(source), int(target))
            
            # Draw network
            plt.figure(figsize=(12, 8))
            
            # Positional layout
            pos = nx.spring_layout(G, k=1, iterations=50)
            
            # Draw nodes by region
            region_colors = {
                "Region_1": "#FF6B6B",
                "Region_2": "#4ECDC4",
                "Region_3": "#45B7D1"
            }
            
            for region, color in region_colors.items():
                nodes_in_region = [node for node in G.nodes() if G.nodes[node].get("region") == region]
                nx.draw_networkx_nodes(G, pos, nodelist=nodes_in_region, 
                                     node_color=color, node_size=10, alpha=0.8)
            
            # Draw edges
            nx.draw_networkx_edges(G, pos, alpha=0.3, width=0.5)
            
            # Draw labels for regions
            for region, color in region_colors.items():
                center = self._get_region_center(brain, region)
                nx.draw_networkx_labels(G, pos, {center: region}, 
                                       font_size=12, font_weight="bold")
            
            plt.title("Brain Network Topology")
            plt.axis("off")
            plt.tight_layout()
            
            # Save figure
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Brain topology saved to {output_file}")
            
            plt.close()
            
        except ImportError:
            print("NetworkX or matplotlib not available. Skipping visualization.")
        except Exception as e:
            print(f"Error creating visualization: {e}")
    
    def _get_region_center(self, brain, region_name):
        """Get center position for a region in visualization."""
        # This is a simple implementation
        # In a more sophisticated implementation, you might use actual brain coordinates
        region_positions = {
            "Region_1": (0.3, 0.7),
            "Region_2": (0.5, 0.5),
            "Region_3": (0.7, 0.3)
        }
        return region_positions.get(region_name, (0.5, 0.5))

# Create advanced brain configuration
advanced_config = {
    "regions": 3,
    "neurons_per_region": 50,
    "connection_probability": 0.05,
    "clustering_coefficient": 0.2,
    "path_length": 1.5
}

# Create brain with specific topology
brain_config = AdvancedBrainConfiguration()
brain = brain_config.create_topology("small_world", advanced_config)

# Visualize brain topology
brain_config.create_brain_with_visualization(brain, "small_world_brain.png")

print(f"Created brain with {brain.get_region_count()} regions and {brain.get_total_neuron_count()} neurons")
print(f"Total synapses: {brain.get_total_synapse_count()}")
```

## Key Takeaways

### 1. Brain Network Structure

- Brain networks consist of regions, neurons, and synaptic connections
- Different neuron types have different properties and behaviors
- Synaptic connections enable communication between neurons
- Plasticity mechanisms modify connection strengths over time

### 2. Configuration Techniques

- Use configuration files or Python API to create brain networks
- Different brain topologies affect network dynamics
- Modular design allows for flexible experimentation
- Visualization helps understand network structure

### 3. Simulation Parameters

- Simulation step size affects accuracy and performance
- Brain initialization is critical for proper function
- Connection setup determines network connectivity
- Activity monitoring provides insights into network behavior

### 4. Advanced Configuration

- Random connectivity creates diverse network behaviors
- Small-world topology balances local and global connectivity
- Modular design supports complex functional organization
- Visualization aids in understanding and debugging

## Further Learning

### Practice Exercises:

1. **Create a simple brain network:** Use the basic configuration to create a 2-region brain
2. **Explore connectivity:** Experiment with different connection probabilities
3. **Study neuron types:** Compare behavior of different neuron types
4. **Visualize networks:** Create visualizations of different brain topologies
5. **Analyze activity:** Study the statistical properties of neural activity

### Advanced Topics:

1. **Network dynamics:** Study emergent behaviors in neural networks
2. **Learning algorithms:** Explore different plasticity rules
3. **Memory systems:** Investigate how NLM handles memory
4. **Neuromodulation:** Study the role of neuromodulators in learning
5. **Extensions:** Create custom brain network types

### Research Applications:

1. **Cognitive modeling:** Model human cognitive processes
2. **Neuroscience research:** Investigate neural mechanisms
3. **Artificial intelligence:** Develop intelligent systems
4. **Clinical applications:** Study neurological disorders

## Conclusion

This tutorial has provided a comprehensive guide to setting up and configuring brain networks in NLM. You have learned how to:

1. **Create brain networks:** Build basic and advanced brain structures
2. **Configure neurons:** Work with different neuron types and properties
3. **Setup connections:** Create synaptic connections between neurons
4. **Run simulations:** Execute neural simulations and analyze results
5. **Visualize networks:** Understand and debug network topology

By mastering these skills, you can create sophisticated brain models for various research and application purposes. The flexibility of NLM allows you to experiment with different configurations, study network dynamics, and investigate the principles underlying neural computation.

The foundation you've built here will serve you well as you advance to more complex topics and applications in NLM. Continue exploring, experimenting, and learning to fully leverage the power of NLM for your research and development needs.

Happy modeling!