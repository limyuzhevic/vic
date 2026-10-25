#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Command Line Interface

A comprehensive command-line interface for NLM simulation and analysis.
"""

import sys
import os
import argparse
import json
from typing import Optional, List, Dict, Any
import pynlm

def create_brain(args):
    """Create and configure a brain"""
    config = pynlm.createDefaultConfig()
    
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    if args.regions:
        config.set("brain.region_count", args.regions)
    if args.sigma:
        config.set("brain.synapse_density", args.sigma)
    if args.stdp_ltp:
        config.set("stdp_ltp_weight", args.stdp_ltp)
    if args.stdp_ltd:
        config.set("stdp_ltd_weight", args.stdp_ltd)
    if args.synaptogenesis:
        config.set("synaptogenesis_rate", args.synaptogenesis)
    if args.pruning:
        config.set("pruning_rate", args.pruning)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    if args.save:
        brain.save(args.save)
        print(f"Brain saved to {args.save}")
    
    return brain

def simulate_brain(args):
    """Run simulation with a brain"""
    brain = create_brain(args)
    
    print(f"Running simulation for {args.steps} steps...")
    
    for step in range(args.steps):
        brain.step(step)
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getAverageFiringRate():.2f} avg firing rate")
    
    print(f"\nSimulation complete:")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")
    
    if args.save:
        brain.save(args.save)
        print(f"Final brain state saved to {args.save}")
    
    if args.stats:
        stats = brain.getBrainStats()
        print(f"\nBrain statistics: {json.dumps(stats, indent=2)}")
    
    if args.export:
        connectivity = brain.exportNetwork()
        with open(args.export, 'w') as f:
            json.dump(connectivity, f, indent=2)
        print(f"Network connectivity exported to {args.export}")

def create_agent_setup(args):
    """Create complete agent setup"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    world.configure(20, 20, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Created agent setup:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  Sensory input size: {agent.getSensoryInputSize()}")
    print(f"  Motor output size: {agent.getMotorOutputSize()}")
    
    return brain, agent, world

def run_episode(args):
    """Run an agent episode"""
    brain, agent, world = create_agent_setup(args)
    
    print(f"Running episode for {args.steps} steps...")
    
    total_reward = 0.0
    for step in range(args.steps):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        total_reward += reward
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: Action={action}, Reward={reward:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    print(f"\nEpisode complete:")
    print(f"Total reward: {total_reward}")
    print(f"Average reward: {total_reward / args.steps:.4f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
    print(f"Development stage: {agent.getDevelopmentalStage()}")
    
    if args.stats:
        stats = agent.getEpisodeStats()
        print(f"\nEpisode statistics: {json.dumps(stats, indent=2)}")
    
    return brain, agent, world, total_reward

def benchmark(args):
    """Benchmark brain performance"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Benchmarking brain with {brain.getTotalNeuronCount()} neurons for {args.steps} steps...")
    
    import time
    start_time = time.time()
    
    for step in range(args.steps):
        brain.step(step)
    
    end_time = time.time()
    duration_ms = (end_time - start_time) * 1000
    
    print(f"\nBenchmark results:")
    print(f"  Steps completed: {args.steps}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Spikes per second: {brain.getTotalSpikeCount() / (duration_ms / 1000):.0f}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Duration: {duration_ms:.2f} ms")
    print(f"  Steps per second: {args.steps / (duration_ms / 1000):.0f}")
    
    if args.plots:
        # Generate simple performance plot data
        plot_data = {
            "steps": list(range(0, min(args.steps, 100), max(1, args.steps // 100))),
            "spike_counts": [brain.getTotalSpikeCount() * i // args.steps for i in range(0, min(args.steps, 100), max(1, args.steps // 100))]
        }
        with open(args.plots, 'w') as f:
            json.dump(plot_data, f, indent=2)
        print(f"  Performance data saved to {args.plots}")

def export_state(args):
    """Export brain state to file"""
    if not args.input:
        print("Error: --input file required for export")
        return
    
    brain = pynlm.importSimulationState(args.input)
    if brain:
        if args.output:
            brain.save(args.output)
            print(f"Brain state exported from {args.input} to {args.output}")
        else:
            print(f"Brain state loaded successfully from {args.input}")
    else:
        print(f"Error: Failed to load brain state from {args.input}")

def import_state(args):
    """Import brain state from file"""
    if not args.input:
        print("Error: --input file required for import")
        return
    
    brain = pynlm.importSimulationState(args.input)
    if brain:
        print(f"Brain state imported successfully from {args.input}")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
    else:
        print(f"Error: Failed to import brain state from {args.input}")

def analyze_connectivity(args):
    """Analyze neural connectivity"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    connectivity = brain.analyzeConnectivity()
    
    if args.json:
        with open(args.json, 'w') as f:
            json.dump(connectivity, f, indent=2)
        print(f"Connectivity analysis saved to {args.json}")
    else:
        print("Connectivity analysis:")
        print(f"  Total connections: {sum(len(conns) for conns in connectivity.values())}")
        print(f"  Source neurons: {len(connectivity)}")
        print(f"  Average connections per neuron: {sum(len(conns) for conns in connectivity.values()) / len(connectivity) if connectivity else 0:.1f}")
        
        if args.verbose:
            print(f"\nDetailed connectivity:")
            for source, targets in connectivity.items():
                if len(targets) > 0:
                    print(f"  Neuron {source} -> {[t for t in targets[:5]]}{'...' if len(targets) > 5 else ''}")

def plasticity_control(args):
    """Control plasticity system"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get current plasticity settings
    plasticity = brain.getPlasticityInfo()
    print("Current plasticity settings:")
    for key, value in plasticity.items():
        print(f"  {key}: {value}")
    
    if args.stdp_ltp:
        brain.updatePlasticityConfig({"stdp_ltp": args.stdp_ltp})
        print(f"Updated STDP LTP weight to {args.stdp_ltp}")
    
    if args.stdp_ltd:
        brain.updatePlasticityConfig({"stdp_ltd": args.stdp_ltd})
        print(f"Updated STDP LTD weight to {args.stdp_ltd}")
    
    if args.hebbian:
        brain.updatePlasticityConfig({"hebbian_rate": args.hebbian})
        print(f"Updated Hebbian learning rate to {args.hebbian}")
    
    if args.synaptogenesis:
        brain.updatePlasticityConfig({"synaptogenesis_rate": args.synaptogenesis})
        print(f"Updated synaptogenesis rate to {args.synaptogenesis}")
    
    if args.pruning:
        brain.updatePlasticityConfig({"pruning_rate": args.pruning})
        print(f"Updated pruning rate to {args.pruning}")

def train_agent(args):
    """Train an agent"""
    brain, agent, world = create_agent_setup(args)
    
    print(f"Training agent for {args.episodes} episodes...")
    
    total_reward = 0.0
    for episode in range(args.episodes):
        agent.reset()
        agent.initialize(world)
        
        for step in range(args.steps):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        total_reward += agent.getNeuromodulationLevel()
        
        if args.verbose and (episode + 1) % args.verbose == 0:
            print(f"Episode {episode + 1}: Reward={total_reward / (episode + 1):.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"\nTraining complete:")
    print(f"Average reward: {total_reward / args.episodes:.4f}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")

def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli brain --neurons 1000 --steps 1000
  nlm-cli episode --steps 500
  nlm-cli benchmark --neurons 500 --steps 5000
  nlm-cli train --episodes 100
  nlm-cli analyze-connectivity --neurons 1000
  nlm-cli plasticity --stdp_ltp 0.02
  nlm-cli export --input mybrain.bin --output newbrain.bin
        """)
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Brain simulation command
    brain_parser = subparsers.add_parser('brain', help='Simulate a brain')
    brain_parser.add_argument('--neurons', type=int, help='Number of neurons')
    brain_parser.add_argument('--regions', type=int, help='Number of regions')
    brain_parser.add_argument('--sigma', type=float, help='Synapse density')
    brain_parser.add_argument('--stdp_ltp', type=float, help='STDP LTP weight')
    brain_parser.add_argument('--stdp_ltd', type=float, help='STDP LTD weight')
    brain_parser.add_argument('--synaptogenesis', type=float, help='Synaptogenesis rate')
    brain_parser.add_argument('--pruning', type=float, help='Pruning rate')
    brain_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    brain_parser.add_argument('--save', help='Save brain state to file')
    brain_parser.add_argument('--stats', action='store_true', help='Print brain statistics')
    brain_parser.add_argument('--export', help='Export network connectivity to JSON file')
    brain_parser.add_argument('--verbose', type=int, help='Print verbose output every N steps')
    
    # Episode command
    episode_parser = subparsers.add_parser('episode', help='Run an agent episode')
    episode_parser.add_argument('--steps', type=int, default=100, help='Number of steps')
    episode_parser.add_argument('--stats', action='store_true', help='Print episode statistics')
    episode_parser.add_argument('--verbose', type=int, help='Print verbose output every N steps')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark brain performance')
    benchmark_parser.add_argument('--neurons', type=int, help='Number of neurons')
    benchmark_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    benchmark_parser.add_argument('--plots', help='Save performance data to JSON file')
    
    # Export state command
    export_parser = subparsers.add_parser('export', help='Export brain state to file')
    export_parser.add_argument('--input', help='Input brain state file')
    export_parser.add_argument('--output', help='Output brain state file')
    
    # Import state command
    import_parser = subparsers.add_parser('import', help='Import brain state from file')
    import_parser.add_argument('--input', help='Input brain state file')
    
    # Analyze connectivity command
    analyze_parser = subparsers.add_parser('analyze-connectivity', help='Analyze neural connectivity')
    analyze_parser.add_argument('--neurons', type=int, help='Number of neurons')
    analyze_parser.add_argument('--json', help='Save analysis to JSON file')
    analyze_parser.add_argument('--verbose', action='store_true', help='Print detailed connectivity')
    
    # Plasticity control command
    plasticity_parser = subparsers.add_parser('plasticity', help='Control plasticity system')
    plasticity_parser.add_argument('--stdp_ltp', type=float, help='Set STDP LTP weight')
    plasticity_parser.add_argument('--stdp_ltd', type=float, help='Set STDP LTD weight')
    plasticity_parser.add_argument('--hebbian', type=float, help='Set Hebbian learning rate')
    plasticity_parser.add_argument('--synaptogenesis', type=float, help='Set synaptogenesis rate')
    plasticity_parser.add_argument('--pruning', type=float, help='Set pruning rate')
    
    # Train agent command
    train_parser = subparsers.add_parser('train', help='Train an agent')
    train_parser.add_argument('--episodes', type=int, default=10, help='Number of training episodes')
    train_parser.add_argument('--steps', type=int, default=100, help='Number of steps per episode')
    train_parser.add_argument('--verbose', type=int, help='Print verbose output every N episodes')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    try:
        if args.command == 'brain':
            simulate_brain(args)
        elif args.command == 'episode':
            run_episode(args)
        elif args.command == 'benchmark':
            benchmark(args)
        elif args.command == 'export':
            export_state(args)
        elif args.command == 'import':
            import_state(args)
        elif args.command == 'analyze-connectivity':
            analyze_connectivity(args)
        elif args.command == 'plasticity':
            plasticity_control(args)
        elif args.command == 'train':
            train_agent(args)
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
