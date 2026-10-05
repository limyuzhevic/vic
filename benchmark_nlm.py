#!/usr/bin/env python3
# NLM Performance Benchmark Script
# Comprehensive performance benchmarking for NLM neural simulation framework

import sys
import os
import time
import json
import argparse
from datetime import datetime
from pathlib import Path

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("ERROR: NLM Python bindings not found!")
    print("Please build and install NLM Python bindings first.")
    sys.exit(1)

class NLMBenchmark:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.agent = None
        self.results = {}
        self.start_time = None
        self.end_time = None
        
    def run_benchmark(self, benchmark_type, **kwargs):
        """Run a specific benchmark"""
        self.start_time = time.time()
        
        try:
            if benchmark_type == "simple":
                self._benchmark_simple(**kwargs)
            elif benchmark_type == "plasticity":
                self._benchmark_plasticity(**kwargs)
            elif benchmark_type == "integration":
                self._benchmark_integration(**kwargs)
            elif benchmark_type == "memory":
                self._benchmark_memory(**kwargs)
            elif benchmark_type == "complete":
                self._benchmark_complete(**kwargs)
            else:
                raise ValueError(f"Unknown benchmark type: {benchmark_type}")
                
            self.end_time = time.time()
            
        except Exception as e:
            print(f"ERROR in benchmark {benchmark_type}: {e}")
            raise
            
    def _benchmark_simple(self, steps=1000, neuron_count=1000):
        """Benchmark simple brain simulation"""
        print(f"Running simple benchmark: {steps} steps, {neuron_count} neurons")
        
        # Update config
        self.config.set("brain.neuron_count", neuron_count)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Run simulation
        for i in range(steps):
            self.brain.step(i)
            
        # Collect results
        self.results = {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "e_i_ratio": self.brain.getExcitationInhibitionRatio(),
            "total_neurons": self.brain.getTotalNeuronCount(),
            "total_synapses": self.brain.getTotalSynapseCount(),
            "time_per_step_ms": (self.end_time - self.start_time) / steps * 1000
        }
        
        print(f"  Total spikes: {self.results['total_spikes']}")
        print(f"  Average firing rate: {self.results['avg_firing_rate']:.2f} Hz")
        
    def _benchmark_plasticity(self, steps=5000, enable_structures=True):
        """Benchmark plasticity learning"""
        print(f"Running plasticity benchmark: {steps} steps")
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(world)
        
        # Enable plasticity features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(enable_structures)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Run simulation with plasticity
        for step in range(steps):
            world.update(0.1)
            
            # Generate random sensory input
            percept = world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = 0.1 if step % 100 == 0 else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
        # Collect plasticity results
        self.results = {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "curiosity_level": self.agent.getCuriosityLevel(),
            "novelty_level": self.agent.getNoveltyLevel(),
            "prediction_error": self.agent.getPredictionError(),
            "neuromodulation_level": self.agent.getNeuromodulationLevel(),
            "development_stage": self.brain.getDevelopmentalStage(),
            "synaptic_changes": "plasticity enabled",
            "time_per_step_ms": (self.end_time - self.start_time) / steps * 1000
        }
        
        print(f"  Final curiosity: {self.results['curiosity_level']:.3f}")
        print(f"  Final novelty: {self.results['novelty_level']:.3f}")
        print(f"  Development stage: {self.results['development_stage']}")
        
    def _benchmark_integration(self, steps=2000):
        """Benchmark complete brain integration"""
        print(f"Running integration benchmark: {steps} steps")
        
        # Create complete agent system
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(world)
        
        # Enable all brain systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Run complete integration
        for step in range(steps):
            world.update(0.1)
            
            # Process complete sensory-motor loop
            percept = world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
        # Collect integration results
        self.results = {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "prediction_error": self.agent.getPredictionError(),
            "development_stage": self.brain.getDevelopmentalStage(),
            "working_memory_traces": "active" if self.brain.getWorkingMemory() else "inactive",
            "time_per_step_ms": (self.end_time - self.start_time) / steps * 1000,
            "simulation_fps": steps / (self.end_time - self.start_time)
        }
        
        print(f"  Simulation FPS: {self.results['simulation_fps']:.1f}")
        print(f"  Final development stage: {self.results['development_stage']}")
        
    def _benchmark_memory(self, episodes=100):
        """Benchmark memory systems"""
        print(f"Running memory benchmark: {episodes} episodes")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(world)
        
        # Enable memory systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Run episodes to test memory
        for episode in range(episodes):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(episode)
            
            action = self.agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Memory trigger
            if episode % 10 == 0:
                reward = 1.0
                self.agent.applyRewardModulation(reward, 0.0)
            
        # Collect memory results
        self.results = {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "prediction_error": self.agent.getPredictionError(),
            "development_stage": self.brain.getDevelopmentalStage(),
            "episodes_completed": episodes,
            "time_per_step_ms": (self.end_time - self.start_time) / (episodes * 10) * 1000
        }
        
        print(f"  Final curiosity: {self.results['curiosity']:.3f}")
        print(f"  Final novelty: {self.results['novelty']:.3f}")
        
    def _benchmark_complete(self, **kwargs):
        """Run complete benchmark suite"""
        print("Running complete benchmark suite...")
        
        # Run individual benchmarks
        self.run_benchmark("simple", **kwargs)
        simple_results = self.results.copy()
        
        self.run_benchmark("plasticity", **kwargs)
        plasticity_results = self.results.copy()
        
        self.run_benchmark("integration", **kwargs)
        integration_results = self.results.copy()
        
        # Create comprehensive report
        self.results = {
            "benchmarks": {
                "simple": simple_results,
                "plasticity": plasticity_results,
                "integration": integration_results
            },
            "summary": {
                "total_time_seconds": self.end_time - self.start_time,
                "total_spikes": simple_results.get("total_spikes", 0),
                "avg_firing_rate": simple_results.get("avg_firing_rate", 0),
                "final_development_stage": integration_results.get("development_stage"),
                "memory_systems": "active"
            }
        }
        
    def save_results(self, filepath=None):
        """Save benchmark results to JSON file"""
        if filepath is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filepath = f"nlm_benchmark_{timestamp}.json"
        
        # Convert results to JSON-serializable format
        results = {
            "timestamp": datetime.now().isoformat(),
            "benchmark_data": self.results
        }
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to: {filepath}")
        return filepath

def main():
    parser = argparse.ArgumentParser(description="NLM Performance Benchmark Suite")
    parser.add_argument("--type", choices=["simple", "plasticity", "integration", "memory", "complete"], 
                       default="simple", help="Benchmark type to run")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--output", type=str, help="Output file for benchmark results")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    # Create benchmark instance
    benchmark = NLMBenchmark()
    
    # Run benchmark
    print(f"Starting NLM benchmark: {args.type}")
    print(f"Parameters: steps={args.steps}, neurons={args.neurons}")
    
    try:
        benchmark.run_benchmark(args.type, steps=args.steps, neuron_count=args.neurons)
        
        # Print summary
        print("\n" + "="*50)
        print("BENCHMARK SUMMARY")
        print("="*50)
        
        if args.type in ["simple", "plasticity", "integration", "memory"]:
            for key, value in benchmark.results.items():
                print(f"{key}: {value}")
        
        # Save results
        output_file = args.output or benchmark.save_results()
        
        print("\nBenchmark completed successfully!")
        
    except Exception as e:
        print(f"\nBenchmark failed with error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()