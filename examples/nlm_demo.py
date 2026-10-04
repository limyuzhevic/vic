#!/usr/bin/env python3
"""
NLM Demo Script - Interactive Neural Network Simulation

This script provides an interactive interface to demonstrate the NLM (Neural Learning Machine)
Phase 6 integrated artificial brain system. It showcases various capabilities including:

1. Basic neural simulation
2. Memory system integration
3. Neuromodulation
4. Development and learning
5. Performance benchmarking

Usage:
    python3 nlm_demo.py [options]

Options:
    --neurons N        Number of neurons (default: 1000)
    --steps S          Number of simulation steps (default: 1000)
    --mode MODE        Demo mode: basic, memory, neuromod, development, benchmark
    --help             Show this help message
"""

import pynlm
import time
import sys

def print_welcome():
    print("=== NLM Neural Learning Machine Demo ===")
    print("Phase 6: Integrated Artificial Developmental Brain")
    print()
    print("This demo showcases the NLM brain's capabilities including:")
    print("• Real spiking neural computation")
    print("• Working memory and episodic memory systems")
    print("• Neuromodulation (dopamine, curiosity, novelty)")
    print("• Developmental processes")
    print("• Plasticity mechanisms (STDP, Hebbian)")
    print()

def print_help():
    print("=== NLM Demo Help ===")
    print()
    print("Available Demo Modes:")
    print("  basic      - Basic neural simulation with sensory input")
    print("  memory     - Memory system demonstration")
    print("  neuromod    - Neuromodulation and learning")
    print("  development - Developmental processes")
    print("  benchmark  - Performance benchmarking")
    print("  all        - Run all demos in sequence")
    print()
    print("Common Commands (interactive demo mode):")
    print("  help       - Show this help")
    print("  status     - Show current brain state")
    print("  simulate   - Run simulation")
    print("  analyze    - Analyze brain statistics")
    print("  plasticity - Test plasticity mechanisms")
    print("  memory     - Test memory systems")
    print("  neuromod    - Test neuromodulation")
    print("  development - Test development")
    print("  quit/q     - Exit demo")
    print()

def run_basic_demo(neuron_count=1000, steps=1000):
    print("=== Basic Neural Simulation Demo ===")
    print(f"Running simulation with {neuron_count} neurons for {steps} steps")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for basic simulation
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Created {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    print("Starting simulation...")
    start_time = time.time()
    
    for step in range(steps):
        # Inject sensory input every 100 steps
        if step % 100 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0f)
        
        brain.step(step, step * 0.001)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            firing_rate = brain.getFiringNeuronCount() / 100.0  # Rough estimate
            print(f"  Step {step:4d}: {brain.getFiringNeuronCount():3d} firing neurons, "
                  f"{firing_rate:5.1f} Hz average")
    
    elapsed = time.time() - start_time
    print(f"✓ Simulation completed in {elapsed:.2f} seconds")
    print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def run_memory_demo(neuron_count=500, steps=2000):
    print("=== Memory System Demo ===")
    print(f"Demonstrating working and episodic memory systems")
    print(f"Using {neuron_count} neurons for {steps} steps")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("✓ Memory systems created:")
    print(f"  - Working memory capacity: {brain.getWorkingMemory().getCapacity() if brain.getWorkingMemory() else 'N/A'} traces")
    print(f"  - Episodic memory max episodes: {brain.getEpisodicMemory().getMaxEpisodes() if brain.getEpisodicMemory() else 'N/A'}")
    
    # Run simulation with memory formation
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        # Store activity in working memory
        if step % 50 == 0:
            # Inject some activity
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
    
    # Show memory statistics
    if brain.getWorkingMemory():
        print(f"✓ Working memory active traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"✓ Episodic memory episodes stored: {brain.getEpisodicMemory().getEpisodeCount()}")
        
        # Get sample episodes
        episodes = brain.getEpisodicMemory().getEpisodesForReplay(3)
        print(f"✓ Sample episodes for replay: {len(episodes)}")
    
    print()

def run_neuromodulation_demo(neuron_count=300, steps=1500):
    print("=== Neuromodulation Demo ===")
    print(f"Demonstrating dopamine, curiosity, and novelty systems")
    print(f"Using {neuron_count} neurons for {steps} steps")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("random_seed", 42)
    config.set("neuromod.dopamine.scale", 1.0f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent to control neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Enable all neuromodulation subsystems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Neuromodulation systems enabled:")
    print(f"  - Reward modulation: {'ON' if agent.isRewardModulationEnabled() else 'OFF'}")
    print(f"  - Development: {'ON' if agent.isDevelopmentEnabled() else 'OFF'}")
    print(f"  - Curiosity: {'ON' if agent.isCuriosityEnabled() else 'OFF'}")
    
    # Run simulation
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        # Apply reward modulation based on activity
        reward = brain.getAverageFiringRate() / 10.0  # Normalize reward
        agent.applyRewardModulation(reward, 0.0f)
        
        # Update development
        if step % 10 == 0:
            agent.updateDevelopment(0.001)
    
    # Show neuromodulation levels
    print(f"✓ Current neuromodulation levels:")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent.getPredictionError():.3f}")
    print()

def run_development_demo(neuron_count=400, steps=2000):
    print("=== Development Demo ===")
    print(f"Demonstrating developmental processes and plasticity")
    print(f"Using {neuron_count} neurons for {steps} steps")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("random_seed", 42)
    config.set("development.enable_critical_period", True)
    config.set("development.enable_synaptogenesis", True)
    config.set("development.enable_pruning", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Developmental systems initialized:")
    print(f"  - Developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"  - Initial plasticity: HIGH (Critical period)")
    
    # Run development simulation
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        # Development updates occur every 100 steps
        if step % 100 == 0:
            # Simulate age progression
            age = step / 100.0
            if age < 5:
                brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
            elif age < 10:
                brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
            elif age < 20:
                brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            else:
                brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Aging)
            
            print(f"  Age {age:.1f}: {brain.getDevelopmentalStage().name}")
    
    print(f"✓ Development simulation completed")
    print(f"✓ Final developmental stage: {brain.getDevelopmentalStage().name}")
    print()

def run_benchmark_demo():
    print("=== Performance Benchmark Demo ===")
    print("Measuring performance of NLM Phase 6 implementation")
    print()
    
    test_configs = [
        (100, 500, "Small (100 neurons)"),
        (500, 1000, "Medium (500 neurons)"),
        (1000, 500, "Large (1000 neurons)")
    ]
    
    for neurons, steps, description in test_configs:
        print(f"Testing {description}...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        config.set("random_seed", 42)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        for step in range(steps):
            brain.step(step, step * 0.001)
        
        elapsed = time.time() - start_time
        spikes = brain.getTotalSpikeCount()
        efficiency = spikes / (neurons * steps * 0.001)  # spikes per neuron per ms
        
        print(f"  ✓ Time: {elapsed:.2f}s, Efficiency: {efficiency:.2f} spikes/neuron/ms")
    
    print()

def run_interactive_demo(neuron_count=200):
    print("=== Interactive NLM Demo ===")
    print("Use the commands below to explore the brain system")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created interactive brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Available commands:")
    print("  status  - Show brain statistics")
    print("  simulate N - Run N simulation steps")
    print("  analyze - Analyze brain state")
    print("  quit    - Exit demo")
    print()
    
    while True:
        print(f">>> ", end="")
        try:
            command = input().strip().lower()
            
            if command == "help":
                print_help()
            
            elif command == "status":
                print(f"Brain Status:")
                print(f"  Neurons: {brain.getTotalNeuronCount()}")
                print(f"  Synapses: {brain.getTotalSynapseCount()}")
                print(f"  Active: {brain.getActiveNeuronCount()}")
                print(f"  Firing: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            
            elif command.startswith("simulate "):
                try:
                    steps = int(command.split()[1])
                    print(f"Running {steps} simulation steps...")
                    for step in range(steps):
                        brain.step(step, step * 0.001)
                        if step % 100 == 0:
                            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                    print(f"✓ Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
                except (ValueError, IndexError):
                    print("Invalid command. Usage: simulate <steps>")
            
            elif command == "analyze":
                print("Brain Analysis:")
                # E/I ratio
                ei_ratio = brain.getExcitationInhibitionRatio()
                print(f"  Excitation/Inhibition ratio: {ei_ratio:.3f}")
                if ei_ratio > 1000:
                    print("  ⚠️  Very high E/I ratio - possible imbalance")
                elif ei_ratio < 0.1:
                    print("  ⚠️  Very low E/I ratio - possible imbalance")
                
                # Firing rate
                avg_firing = brain.getAverageFiringRate()
                print(f"  Average firing rate: {avg_firing:.2f} Hz")
                
                # Memory status
                if brain.getWorkingMemory():
                    print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
                
                if brain.getEpisodicMemory():
                    print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
            
            elif command == "quit" or command == "q":
                print("Exiting interactive demo. Goodbye!")
                return
            
            else:
                print(f"Unknown command: {command}")
                print("Type 'help' for available commands")
                
        except KeyboardInterrupt:
            print("\nInterrupted. Exiting demo.")
            return
        except Exception as e:
            print(f"Error: {e}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Demo Script")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons (default: 1000)")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps (default: 1000)")
    parser.add_argument("--mode", choices=["basic", "memory", "neuromod", "development", "benchmark", "interactive", "all"], 
                       default="interactive", help="Demo mode (default: interactive)")
    
    args = parser.parse_args()
    
    print_welcome()
    
    if args.mode == "basic":
        run_basic_demo(args.neurons, args.steps)
    
    elif args.mode == "memory":
        run_memory_demo(args.neurons, args.steps)
    
    elif args.mode == "neuromod":
        run_neuromodulation_demo(args.neurons, args.steps)
    
    elif args.mode == "development":
        run_development_demo(args.neurons, args.steps)
    
    elif args.mode == "benchmark":
        run_benchmark_demo()
    
    elif args.mode == "interactive":
        run_interactive_demo(args.neurons)
    
    elif args.mode == "all":
        print("Running all demo modes...")
        run_basic_demo(100, 200)
        run_memory_demo(200, 400)
        run_neuromodulation_demo(150, 300)
        run_development_demo(250, 500)
        run_benchmark_demo()
    
    print("=== Demo Complete ===")

if __name__ == "__main__":
    main()
