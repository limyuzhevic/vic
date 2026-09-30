#!/usr/bin/env python3
"""
NLM Advanced Features Demo

This script demonstrates NLM's advanced capabilities with a simple, working example.
It shows how to configure, run, and analyze demonstrations of the NLM brain.

The demo simulates a basic memory learning experiment using NLM's neural systems.
"""

import sys
import time
import json
import argparse
from typing import Dict, List, Optional

# Mock NLM classes for demonstration purposes
# In a real implementation, these would import from the actual NLM library

class MockNeuron:
    def __init__(self, neuron_id: int):
        self.id = neuron_id
        self.potential = -70.0
        self.threshold = -55.0
        self.firing_state = "resting"
        self.refractory_remaining = 0
        
    def inject_current(self, current: float):
        """Inject current into neuron (simulate synaptic input)"""
        self.potential += current * 0.1  # Simplified neuron dynamics
        
    def step(self):
        """Advance neuron state by one simulation step"""
        if self.refractory_remaining > 0:
            self.refractory_remaining -= 1
            if self.refractory_remaining == 0:
                self.firing_state = "resting"
                
        if self.potential >= self.threshold:
            self.firing_state = "firing"
            self.refractory_remaining = 5
            self.potential = -75.0  # Reset after firing
            
    def is_firing(self) -> bool:
        return self.firing_state == "firing"
        
    def get_potential(self) -> float:
        return self.potential

class MockSynapse:
    def __init__(self, source: int, target: int):
        self.source = source
        self.target = target
        self.weight = 0.1
        self.is_excitatory = True
        
    def get_weight(self) -> float:
        return self.weight
        
    def add_to_weight(self, delta: float):
        """Hebbian learning - strengthen synapse if both neurons fire"""
        self.weight += delta * 0.01  # Small learning rate

class MockMemorySystem:
    def __init__(self):
        self.episodes = []
        self.working_memory = {}
        
    def store_episode(self, neurons: List[int], reward: float):
        """Store an episodic memory"""
        episode = {
            'neurons': neurons,
            'reward': reward,
            'timestamp': time.time()
        }
        self.episodes.append(episode)
        
    def get_episode_count(self) -> int:
        return len(self.episodes)
        
    def get_working_memory_size(self) -> int:
        return len(self.working_memory)

class MockBrain:
    def __init__(self, config: Dict):
        self.config = config
        self.neurons = []
        self.synapses = []
        self.memory = MockMemorySystem()
        self.step_count = 0
        self.total_spikes = 0
        
        # Initialize with configuration
        self.setup_neural_network()
        
    def setup_neural_network(self):
        """Create neural network based on configuration"""
        neuron_count = self.config.get('neuron_count', 100)
        region_count = self.config.get('region_count', 1)
        
        neurons_per_region = neuron_count // region_count
        
        for region in range(region_count):
            for i in range(neurons_per_region):
                neuron_id = region * 1000 + i
                neuron = MockNeuron(neuron_id)
                self.neurons.append(neuron)
                
        # Create some random synapses
        for i in range(min(50, len(self.neurons))):
            for j in range(min(5, len(self.neurons))):
                if i != j and j > i:  # Create directed connections
                    source = self.neurons[i].id
                    target = self.neurons[j].id
                    synapse = MockSynapse(source, target)
                    self.synapses.append(synapse)
                    
    def step(self):
        """Advance simulation by one step"""
        self.step_count += 1
        
        # Present sensory input (random pattern)
        self.present_sensory_input()
        
        # Update neural dynamics
        for neuron in self.neurons:
            neuron.step()
            
        # Record spikes and learning
        self.record_learning_events()
        
        # Store in episodic memory periodically
        if self.step_count % 10 == 0:
            self.update_episodic_memory()
            
    def present_sensory_input(self):
        """Present random sensory input to simulate environment interaction"""
        # Simulate random sensory input
        sensory_input = [0.5, 0.3, 0.7, 0.2, 0.9][:len(self.neurons) // 20]
        
        # Inject current into sensory neurons
        sensory_neuron_count = min(20, len(self.neurons))
        for i in range(sensory_neuron_count):
            if i < len(self.neurons):
                # Make some neurons respond to input
                if i % 3 == 0:  # Every 3rd neuron responds
                    self.neurons[i].inject_current(sensory_input[i % len(sensory_input)])
                    
    def record_learning_events(self):
        """Record spikes and synaptic changes for learning analysis"""
        # Count spikes
        spike_count = sum(1 for neuron in self.neurons if neuron.is_firing())
        self.total_spikes += spike_count
        
        # Hebbian learning - strengthen synapses between co-firing neurons
        for synapse in self.synapses:
            source_neuron = next((n for n in self.neurons if n.id == synapse.source), None)
            target_neuron = next((n for n in self.neurons if n.id == synapse.target), None)
            
            if source_neuron and target_neuron and source_neuron.is_firing() and target_neuron.is_firing():
                # Both neurons fired - strengthen synapse
                synapse.add_to_weight(1.0)
                
    def update_episodic_memory(self):
        """Update episodic memory with current brain state"""
        active_neurons = [neuron.id for neuron in self.neurons if neuron.is_firing()]
        reward = min(self.total_spikes / 10.0, 1.0)  # Simple reward signal
        
        self.memory.store_episode(active_neurons, reward)
        
    def get_status(self) -> Dict:
        """Get current brain status"""
        active_neurons = sum(1 for neuron in self.neurons if neuron.is_firing())
        avg_weight = sum(syn.get_weight() for syn in self.synapses) / max(len(self.synapses), 1)
        
        return {
            'step_count': self.step_count,
            'total_spikes': self.total_spikes,
            'active_neurons': active_neurons,
            'average_synaptic_weight': avg_weight,
            'memory_episodes': self.memory.get_episode_count(),
            'working_memory_size': self.memory.get_working_memory_size()
        }

class NLMAdvancedDemo:
    def __init__(self, config_file: Optional[str] = None):
        self.config = self.load_config(config_file)
        self.brain = MockBrain(self.config)
        self.results = []
        
    def load_config(self, config_file: Optional[str]) -> Dict:
        """Load configuration from file or use defaults"""
        if config_file:
            try:
                with open(config_file, 'r') as f:
                    return json.load(f)
            except FileNotFoundError:
                print(f"Config file {config_file} not found, using defaults")
                
        # Default configuration
        return {
            'neuron_count': 100,
            'region_count': 1,
            'simulation_steps': 100,
            'learning_rate': 0.01,
            'plasticity_enabled': True,
            'memory_capacity': 50,
            'visualization': False
        }
        
    def run_demo(self) -> Dict:
        """Run the demonstration"""
        print(f"Starting NLM Advanced Features Demo")
        print(f"Configuration: {self.config}")
        print(f"Neural network: {self.config['neuron_count']} neurons, {self.config['region_count']} regions")
        
        start_time = time.time()
        
        for step in range(self.config['simulation_steps']):
            self.brain.step()
            
            if step % 20 == 0:
                status = self.brain.get_status()
                self.results.append(status)
                print(f"Step {step}: Spikes={status['total_spikes']}, "
                      f"Active={status['active_neurons']}, "
                      f"Weight={status['average_synaptic_weight']:.3f}")
                      
        end_time = time.time()
        
        # Final status
        final_status = self.brain.get_status()
        final_status['duration_seconds'] = end_time - start_time
        final_status['steps_per_second'] = self.config['simulation_steps'] / (end_time - start_time)
        
        print(f"\nDemonstration Complete!")
        print(f"Total duration: {final_status['duration_seconds']:.2f} seconds")
        print(f"Performance: {final_status['steps_per_second']:.1f} steps/second")
        print(f"Total spikes: {final_status['total_spikes']}")
        print(f"Average synaptic weight: {final_status['average_synaptic_weight']:.3f}")
        print(f"Memory episodes stored: {final_status['memory_episodes']}")
        print(f"Working memory traces: {final_status['working_memory_size']}")
        
        return final_status
        
    def save_results(self, filename: str):
        """Save demonstration results to file"""
        with open(filename, 'w') as f:
            json.dump({
                'config': self.config,
                'results': self.results,
                'final_status': self.brain.get_status(),
                'timestamp': time.time()
            }, f, indent=2)
        print(f"Results saved to {filename}")
        
    def analyze_results(self) -> Dict:
        """Analyze demonstration results"""
        if not self.results:
            return {}
            
        final = self.results[-1]
        
        # Calculate trends
        initial_weight = self.results[0]['average_synaptic_weight']
        final_weight = final['average_synaptic_weight']
        weight_change = final_weight - initial_weight
        
        initial_spikes = self.results[0]['total_spikes']
        final_spikes = final['total_spikes']
        spike_change = final_spikes - initial_spikes
        
        # Memory analysis
        memory_growth = final['memory_episodes'] - (self.results[0]['memory_episodes'] if self.results else 0)
        
        return {
            'weight_change': weight_change,
            'spike_change': spike_change,
            'memory_growth': memory_growth,
            'final_performance': final,
            'demonstration_type': 'Advanced Features Demo',
            'success': weight_change > 0.01 or spike_change > 10  # Simple success criteria
        }

def main():
    parser = argparse.ArgumentParser(description='NLM Advanced Features Demonstration')
    parser.add_argument('--config', help='Configuration file (JSON format)')
    parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    parser.add_argument('--output', help='Output file for results')
    parser.add_argument('--analyze', action='store_true', help='Analyze and print detailed analysis')
    
    args = parser.parse_args()
    
    # Update config from command line
    config = {
        'neuron_count': args.neurons,
        'simulation_steps': args.steps,
        'learning_rate': 0.01,
        'plasticity_enabled': True,
        'memory_capacity': 50,
        'visualization': False
    }
    
    # Create and run demonstration
    demo = NLMAdvancedDemo(args.config)
    results = demo.run_demo()
    
    # Save results if requested
    if args.output:
        demo.save_results(args.output)
    
    # Analyze results if requested
    if args.analyze:
        analysis = demo.analyze_results()
        print(f"\n=== Analysis Results ===")
        print(f"Weight change: {analysis.get('weight_change', 0):.4f}")
        print(f"Spike change: {analysis.get('spike_change', 0)}")
        print(f"Memory growth: {analysis.get('memory_growth', 0)}")
        print(f"Demonstration success: {analysis.get('success', False)}")
    
    return 0

if __name__ == '__main__':
    sys.exit(main())