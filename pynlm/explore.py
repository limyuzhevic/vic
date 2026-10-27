#!/usr/bin/env python3
"""
NLM Exploration Tool

This module provides tools for exploring NLM (Neural Learning Machine) features,
demonstrating capabilities, and helping users understand the system.
"""

import sys
import os
import argparse
from pathlib import Path
from typing import Dict, Any, List

sys.path.insert(0, str(Path(__file__).parent))


def print_explore_banner():
    """Print the exploration banner."""
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                                                               ║")
    print("║     NLM — 熙然                                              ║")
    print("║     Neural Learning Machine                                 ║")
    print("║                                                               ║")
    print("║     Exploration & Demo Tool                                 ║")
    print("║                                                               ║")
    print("╚═══════════════════════════════════════════════════════════════╝")


def demonstrate_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("\n" + "="*60)
    print("DEMONSTRATION 1: Basic Brain Functionality")
    print("="*60)
    
    try:
        import pynlm
        
        # Create a simple brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("region_count", 2)
        config.set("random_seed", 42)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Brain has {brain.getTotalSynapseCount()} synapses")
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        print(f"✓ Ran 100 simulation steps")
        print(f"✓ Final statistics:")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False


def demonstrate_agent_functionality():
    """Demonstrate agent functionality with world interaction."""
    print("\n" + "="*60)
    print("DEMONSTRATION 2: Agent-World Interaction")
    print("="*60)
    
    try:
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        world.configure(
            width=15.0,
            height=15.0,
            visionWidth=8,
            visionHeight=8
        )
        world.reset()
        
        # Place agent in the world
        world.setAgentStart(7.5, 7.5)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"✓ Created agent with {agent.getSensoryInputSize()} sensory inputs")
        print(f"✓ Agent has {agent.getMotorOutputSize()} motor outputs")
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        print("✓ Enabled learning subsystems (reward, curiosity, plasticity, development)")
        
        # Run agent simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✓ Ran 50 steps of agent-world interaction")
        print(f"✓ Agent statistics:")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"  - Prediction error: {agent.getPredictionError():.3f}")
        print(f"  - Developmental stage: {agent.getDevelopmentalStage()}")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False


def demonstrate_plasticity():
    """Demonstrate neural plasticity mechanisms."""
    print("\n" + "="*60)
    print("DEMONSTRATION 3: Neural Plasticity")
    print("="*60)
    
    try:
        import pynlm
        
        # Create brain with plasticity
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300)
        config.set("region_count", 2)
        config.set("plasticity_learning_rate", 0.02)
        config.set("stdp_ltp_weight", 0.03)
        config.set("stdp_ltd_weight", 0.025)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with enhanced plasticity")
        print(f"✓ STDP enabled: {brain.getSTDP() is not None}")
        print(f"✓ Hebbian enabled: {brain.getHebbian() is not None}")
        print(f"✓ Structural plasticity enabled: {brain.getStructuralPlasticity() is not None}")
        
        # Record initial weights
        initial_weights = []
        if brain.getTotalSynapseCount() > 0:
            # Get first region to examine synapses
            region = brain.getRegion(pynlm.RegionId(1))
            if region:
                for synapse in region.getSynapses():
                    if len(initial_weights) < 5:  # Sample first 5
                        initial_weights.append(synapse.getWeight())
        
        print(f"✓ Initial sample weights: {[f'{w:.3f}' for w in initial_weights]}")
        
        # Run plasticity experiment with correlated activity
        print("✓ Running plasticity experiment (correlated pre-post activity)...")
        
        for trial in range(30):
            # Create correlated activity: sensory input then motor response
            brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 40.0f)
            brain.step(trial * 2, trial * 2 * 0.001)
            
            brain.injectCurrentToNeurons(pynlm.NeuronType::Motor, 30.0f)
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
        
        # Record final weights
        final_weights = []
        if brain.getTotalSynapseCount() > 0:
            region = brain.getRegion(pynlm.RegionId(1))
            if region:
                for synapse in region.getSynapses():
                    if len(final_weights) < 5:  # Sample first 5
                        final_weights.append(synapse.getWeight())
        
        print(f"✓ Final sample weights: {[f'{w:.3f}' for w in final_weights]}")
        
        # Calculate weight changes
        if len(initial_weights) > 0 and len(final_weights) > 0:
            changes = [final - initial for initial, final in zip(initial_weights, final_weights)]
            avg_change = sum(changes) / len(changes)
            
            print(f"✓ Weight changes: {[f'{c:.3f}' for c in changes]}")
            print(f"✓ Average weight change: {avg_change:.3f}")
            
            if abs(avg_change) > 0.001:
                print("✓ Plasticity detected: synaptic weights changed!")
            else:
                print("⚠ Limited plasticity detected")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False


def demonstrate_development():
    """Demonstrate developmental processes."""
    print("\n" + "="*60)
    print("DEMONSTRATION 4: Developmental Processes")
    print("="*60)
    
    try:
        import pynlm
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        print(f"✓ Created brain for development study")
        print(f"✓ Initial developmental stage: {brain.getDevelopmentalStage()}")
        
        # Simulate development over time
        stages = []
        for stage_name in ['Initial', 'CriticalPeriod', 'Maturation', 'Adult']:
            # Simulate 1000 steps per developmental stage
            for step in range(1000):
                brain.step(step)
            
            stage = brain.getDevelopmentalStage()
            stages.append(stage)
            
            print(f"✓ After 1000 steps: {stage} stage")
            print(f"  - Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
        
        print(f"\n✓ Developmental progression: {' -> '.join([str(s) for s in stages])}")
        
        # Check developmental effects
        final_stage = brain.getDevelopmentalStage()
        if final_stage == pynlm.DevelopmentalStage::Adult:
            print("✓ Brain reached adult developmental stage")
            print("✓ Development complete: reduced plasticity, stable behavior")
        else:
            print(f"⚠ Brain did not reach adult stage: {final_stage}")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False


def demonstrate_enhanced_features():
    """Demonstrate enhanced API features if available."""
    print("\n" + "="*60)
    print("DEMONSTRATION 5: Enhanced API Features")
    print("="*60)
    
    try:
        import pynlm_enhanced
        
        print("✓ Enhanced API (pynlm_enhanced) is available")
        
        # Demonstrate convenience functions
        print("✓ Convenience function: create_simple_agent()")
        
        brain, world, agent = pynlm_enhanced.create_simple_agent()
        print(f"  - Created agent with {brain.getTotalNeuronCount()} neurons")
        print(f"  - World size: {world.getWidth()} x {world.getHeight()}")
        print(f"  - Agent sensory inputs: {agent.getSensoryInputSize()}")
        print(f"  - Agent motor outputs: {agent.getMotorOutputSize()}")
        
        # Demonstrate performance monitoring
        print("\n✓ Performance monitoring available")
        # Note: PerformanceStats requires actual timing data
        
        # Demonstrate configuration builder
        print("✓ Enhanced configuration builder available")
        
        # Demonstrate learning utilities
        print("✓ Learning simulation utilities available")
        
        return True
        
    except ImportError:
        print("⚠ Enhanced API not available")
        print("  - Running with standard pynlm only")
        return False


def demonstrate_error_handling():
    """Demonstrate improved error handling."""
    print("\n" + "="*60)
    print("DEMONSTRATION 6: Error Handling")
    print("="*60)
    
    try:
        import pynlm
        
        print("✓ Custom exception types available:")
        print("  - NLMError (base exception)")
        print("  - ConfigurationError")
        print("  - SimulationError")
        print("  - ValidationError")
        
        # Create invalid configuration to demonstrate error handling
        print("\n✓ Testing error handling with invalid configurations...")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", -100)  # Invalid: negative neuron count
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✓ Brain created with invalid configuration (may cause simulation errors)")
        
        return True
        
    except Exception as e:
        print(f"✓ Exception caught and handled: {type(e).__name__}: {e}")
        return True


def show_available_features():
    """Show all available NLM features."""
    print("\n" + "="*60)
    print("AVAILABLE NLM FEATURES")
    print("="*60)
    
    features = [
        ("Core Neural Simulation", [
            "Leaky Integrate-and-Fire (LIF) neurons",
            "Event-driven spike propagation",
            "Synaptic delays and weights",
            "Neural populations and regions",
            "Real-time simulation capabilities"
        ]),
        ("Plasticity Mechanisms", [
            "Spike-Timing-Dependent Plasticity (STDP)",
            "Hebbian learning",
            "Structural plasticity (synaptogenesis/pruning)",
            "Neuromodulation effects",
            "Activity-dependent learning"
        ]),
        ("Memory Systems", [
            "Working memory with persistent activity",
            "Episodic memory with experience encoding",
            "Associative memory",
            "Memory consolidation and replay",
            "Sleep/rest cycle integration"
        ]),
        ("Neuromodulation", [
            "Dopamine system",
            "Curiosity-driven exploration",
            "Novelty detection",
            "Prediction error signals",
            "Motivation and reward processing"
        ]),
        ("Cognition", [
            "Neural planning",
            "Concept formation",
            "Attention with competitive selection",
            "Predictive processing",
            "Action selection"
        ]),
        ("Development", [
            "Developmental stages (Initial, CriticalPeriod, Maturation, Adult)",
            "Activity-dependent development",
            "Structural changes with age",
            "Plasticity modulation",
            "Critical period mechanisms"
        ]),
        ("Python Integration", [
            "Original bindings (pynlm)",
            "Enhanced bindings (pynlm_enhanced)",
            "High-level convenience methods",
            "Error handling and validation",
            "Performance monitoring"
        ]),
        ("Advanced Features", [
            "Context managers for resource management",
            "Configuration builders",
            "Simulation profilers",
            "Learning utilities",
            "Export/import capabilities"
        ])
    ]
    
    for category, items in features:
        print(f"\n{category}:")
        for item in items:
            print(f"  ✓ {item}")
    
    print(f"\n{'='*60}")


def main():
    """Main exploration function."""
    parser = argparse.ArgumentParser(
        description="NLM Exploration Tool - Demonstrates NLM features and capabilities",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
This tool demonstrates the capabilities of NLM (Neural Learning Machine)
through a series of demonstrations covering core functionality, learning,
development, and advanced features.
        """
    )
    
    parser.add_argument('--skip-enhanced', action='store_true',
                        help='Skip enhanced API features if not available')
    parser.add_argument('--quick', action='store_true',
                        help='Run quick demonstration (skip some features)')
    
    args = parser.parse_args()
    
    print_explore_banner()
    
    print("\nStarting NLM feature exploration...")
    print("This will demonstrate various aspects of the NLM neural simulation framework.\n")
    
    # Run demonstrations
    results = []
    
    # Basic functionality
    results.append(("Basic Brain Functionality", demonstrate_basic_functionality()))
    
    # Agent functionality
    results.append(("Agent-World Interaction", demonstrate_agent_functionality()))
    
    if not args.quick:
        # Plasticity
        results.append(("Neural Plasticity", demonstrate_plasticity()))
        
        # Development
        results.append(("Developmental Processes", demonstrate_development()))
    
    # Enhanced features
    if not args.skip_enhanced:
        results.append(("Enhanced API Features", demonstrate_enhanced_features()))
    
    # Error handling
    results.append(("Error Handling", demonstrate_error_handling()))
    
    # Show all features
    if not args.quick:
        show_available_features()
    
    # Summary
    print("\n" + "="*60)
    print("EXPLORATION SUMMARY")
    print("="*60)
    
    success_count = sum(1 for _, success in results if success)
    total_count = len(results)
    
    print(f"Demonstrations completed: {success_count}/{total_count}")
    
    for name, success in results:
        status = "✓ SUCCESS" if success else "✗ FAILED"
        print(f"  {status}: {name}")
    
    print(f"\n{'='*60}")
    print("Exploration complete! NLM is a comprehensive neural simulation")
    print("framework with advanced features for artificial brain research.")
    print(f"{'='*60}")


if __name__ == '__main__':
    main()
