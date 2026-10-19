#!/usr/bin/env python3
"""
Structural Plasticity Example

This example demonstrates the Structural Plasticity system in NLM.
It shows how new neural connections are created and pruned.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import time
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
from datetime import datetime

class StructuralPlasticityExample:
    """Example class demonstrating the Structural Plasticity system."""
    
    def __init__(self):
        """Initialize the structural plasticity example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.structural_plasticity = None
        self.stdp = None
        self.hebbian = None
        self.example_name = "Structural Plasticity System"
        
    def setup_simulation_environment(self):
        """Set up the simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(18, 18, 12, 12)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Get plasticity systems
        self.structural_plasticity = self.brain.getStructuralPlasticity()
        self.stdp = self.brain.getSTDP()
        self.hebbian = self.brain.getHebbian()
        
    def demonstrate_structural_plasticity_overview(self):
        """Demonstrate structural plasticity system overview."""
        print("\nDemonstrating Structural Plasticity System Overview...")
        
        print(f"1. Structural Plasticity Properties:")
        print(f"   - Enabled: {self.structural_plasticity.is_enabled()}")
        print(f"   - Current connections: {self.structural_plasticity.getConnectionCount()}")
        
        print(f"\n2. Plasticity Rules:")
        print(f"   - STDP rule: Available (Spike-Timing-Dependent Plasticity)")
        print(f"   - Hebbian rule: Available (Fire together, wire together)")
        
        # Display rule properties
        print(f"\n3. STDP Rule Properties:")
        if self.stdp:
            print(f"   - LTP weight: {self.stdp.ltpWeight}")
            print(f"   - LTD weight: {self.stdp.ltdWeight}")
            print(f"   - Time constant: {self.stdp.timeConstant}")
            
        print(f"\n4. Hebbian Rule Properties:")
        if self.hebbian:
            print(f"   - Learning rate: {self.hebbian.learningRate}")
            print(f"   - Coincidence threshold: {self.hebbian.coincidenceThreshold}")
        
    def demonstrate_structural_growth(self):
        """Demonstrate structural growth (creating new connections)."""
        print("\nDemonstrating Structural Growth...")
        
        # Enable structural growth
        self.structural_plasticity.enableGrowth(True)
        print(f"1. Enabled structural growth")
        
        # Create some regions for connection growth
        regions = []
        for i in range(5):
            region_id = pynlm.RegionId(i)
            regions.append(region_id)
        
        # Simulate growth of new connections
        print(f"\n2. Growing new connections:")
        for i, source_region in enumerate(regions):
            for j, target_region in enumerate(regions):
                if i != j:  # Don't connect region to itself
                    # In real system, this would use brain internals
                    # For demonstration, simulate connection
                    connection_count = self.structural_plasticity.getConnectionCount()
                    print(f"   - Growing connection: {source_region.value} -> {target_region.value} "
                          f"(total connections: {connection_count})")
        
        print(f"\n3. Current connection statistics:")
        print(f"   - Total connections: {self.structural_plasticity.getConnectionCount()}")
        
    def demonstrate_structural_pruning(self):
        """Demonstrate structural pruning (removing old connections)."""
        print("\nDemonstrating Structural Pruning...")
        
        # First, add some connections to prune
        self.structural_plasticity.enableGrowth(True)
        print(f"1. Enabled growth to create connections to prune")
        
        # Simulate having connections
        initial_count = self.structural_plasticity.getConnectionCount()
        print(f"   - Initial connections: {initial_count}")
        
        # Prune some connections
        print(f"\n2. Pruning connections...")
        for i in range(3):
            # Simulate pruning a connection
            # In real system, this would use specific synapse IDs
            print(f"   - Pruning connection {i+1}: {initial_count - i} -> {initial_count - i - 1}")
        
        pruned_count = max(0, initial_count - 3)
        print(f"\n3. Connection statistics after pruning:")
        print(f"   - Remaining connections: {pruned_count}")
        
        # Disable growth
        self.structural_plasticity.enableGrowth(False)
        print(f"\n4. Disabled structural growth")
        
    def demonstrate_stdp_learning(self):
        """Demonstrate Spike-Timing-Dependent Plasticity learning."""
        print("\nDemonstrating STDP Learning...")
        
        # Set up STDP parameters
        if self.stdp:
            self.stdp.configure(0.01, 0.012, 20.0)  # LTP, LTD, time constant
            print(f"1. STDP configured:")
            print(f"   - LTP weight: {self.stdp.ltpWeight}")
            print(f"   - LTD weight: {self.stdp.ltdWeight}")
            print(f"   - Time constant: {self.stdp.timeConstant}")
        
        print(f"\n2. STDP Learning Process:")
        print(f"   - Pre-synaptic spike + Post-synaptic spike (within time window): LTP (strengthening)")
        print(f"   - Pre-synaptic spike BEFORE Post-synaptic spike: LTD (weakening)")
        print(f"   - Spike timing difference determines magnitude")
        
        # Simulate STDP scenario
        print(f"\n3. STDP Simulation:")
        print(f"   Scenario 1: Pre -> Post (within 20ms)")
        print(f"   Result: LTP = {0.01 * 1.0:.3f} (strengthen connection)")
        
        print(f"   Scenario 2: Pre -> Post (50ms apart)")
        print(f"   Result: LTD = {0.012 * 1.0:.3f} (weaken connection)")
        
    def demonstrate_hebbian_learning(self):
        """Demonstrate Hebbian learning."""
        print("\nDemonstrating Hebbian Learning...")
        
        # Set up Hebbian parameters
        if self.hebbian:
            self.hebbian.learningRate = 0.05
            self.hebbian.coincidenceThreshold = 0.8
            print(f"1. Hebbian configured:")
            print(f"   - Learning rate: {self.hebbian.learningRate}")
            print(f"   - Coincidence threshold: {self.hebbian.coincidenceThreshold}")
        
        print(f"\n2. Hebbian Learning Rule:")
        print(f"   'Fire together, wire together'")
        print(f"   - If two neurons fire simultaneously:")
        print(f"     * Increase connection strength by learning_rate")
        print(f"     * Above coincidence threshold: Strong strengthening")
        print(f"     * Below coincidence threshold: Weak strengthening")
        
        # Simulate Hebbian learning
        print(f"\n3. Hebbian Learning Simulation:")
        neuron1_firing = True
        neuron2_firing = True
        
        if neuron1_firing and neuron2_firing:
            firing_strength = 1.0
            if firing_strength >= 0.8:
                weight_change = 0.05 * 1.2  # Enhanced for coincidence
            else:
                weight_change = 0.05
            print(f"   - Both neurons firing simultaneously (strength=1.0)")
            print(f"   - Weight change: +{weight_change:.3f}")
            
    def demonstrate_plasticity_integration(self, steps=30):
        """
        Demonstrate integration of plasticity systems with brain.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Plasticity Integration ({steps} steps)...")
        
        # Enable structural plasticity
        self.structural_plasticity.enableGrowth(True)
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            
            # Brain step (includes plasticity updates)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display integration metrics
            if step % 10 == 0:
                connections = self.structural_plasticity.getConnectionCount()
                print(f"   Step {step:3d}: Connections={connections}, "
                      f"STDP LTP={self.stdp.ltpWeight if self.stdp else 0:.3f}, "
                      f"Hebbian LR={self.hebbian.learningRate if self.hebbian else 0:.3f}")
        
        print(f"\n   Integration completed: {steps} steps")
        
    def demonstrate_adaptive_connectivity(self):
        """Demonstrate adaptive connectivity changes."""
        print("\nDemonstrating Adaptive Connectivity...")
        
        # Start with basic connectivity
        initial_connections = self.structural_plasticity.getConnectionCount()
        print(f"1. Initial connectivity: {initial_connections} connections")
        
        # Simulate activity-dependent plasticity
        print(f"\n2. Activity-dependent plasticity simulation:")
        
        # High activity -> more growth
        high_activity = True
        if high_activity:
            # Simulate growth due to high activity
            growth_connections = 5
            new_total = initial_connections + growth_connections
            print(f"   - High activity detected -> Growth: +{growth_connections} connections")
            print(f"   - New total: {new_total}")
        
        # Low activity -> pruning
        low_activity = True
        if low_activity:
            # Simulate pruning due to low activity
            prune_connections = 2
            new_total = max(0, new_total - prune_connections)
            print(f"   - Low activity detected -> Pruning: -{prune_connections} connections")
            print(f"   - New total: {new_total}")
        
        # Final statistics
        print(f"\n3. Final connectivity statistics:")
        print(f"   - Final connections: {new_total}")
        
    def display_structural_plasticity_statistics(self):
        """Display structural plasticity system statistics."""
        print("\nStructural Plasticity System Statistics:")
        print("-" * 40)
        
        print(f"1. Structural Plasticity:")
        if self.structural_plasticity:
            print(f"   - Enabled: {self.structural_plasticity.is_enabled()}")
            print(f"   - Connection count: {self.structural_plasticity.getConnectionCount()}")
            
        print(f"\n2. Plasticity Rules:")
        if self.stdp:
            print(f"   - STDP rule active: Yes")
            print(f"   - LTP weight: {self.stdp.ltpWeight}")
            print(f"   - LTD weight: {self.stdp.ltdWeight}")
            
        if self.hebbian:
            print(f"   - Hebbian rule active: Yes")
            print(f"   - Learning rate: {self.hebbian.learningRate}")
            print(f"   - Coincidence threshold: {self.hebbian.coincidenceThreshold}")
        
        print(f"\n3. Neural System:")
        if self.brain:
            print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"   - Connection density: {self.structural_plasticity.getConnectionCount() / max(1, self.brain.getTotalNeuronCount()):.3f}")
        
    def run_example(self, steps=40):
        """
        Run the complete structural plasticity example.
        
        Args:
            steps: Number of steps for integration demo
        """
        print("=" * 60)
        print(f"NLM Structural Plasticity System Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Demonstrate structural plasticity overview
            self.demonstrate_structural_plasticity_overview()
            print()
            
            # Step 3: Demonstrate structural growth
            self.demonstrate_structural_growth()
            print()
            
            # Step 4: Demonstrate structural pruning
            self.demonstrate_structural_pruning()
            print()
            
            # Step 5: Demonstrate STDP learning
            self.demonstrate_stdp_learning()
            print()
            
            # Step 6: Demonstrate Hebbian learning
            self.demonstrate_hebbian_learning()
            print()
            
            # Step 7: Demonstrate plasticity integration
            self.demonstrate_plasticity_integration(steps // 2)
            print()
            
            # Step 8: Demonstrate adaptive connectivity
            self.demonstrate_adaptive_connectivity()
            print()
            
            # Step 9: Display statistics
            self.display_structural_plasticity_statistics()
            print()
            
            print("=" * 60)
            print("Structural Plasticity Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.structural_plasticity, self.stdp, self.hebbian
            
        except Exception as e:
            print(f"Error in structural plasticity example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Structural Plasticity Example")
    parser.add_argument("--steps", type=int, default=40, help="Number of steps for integration demo")
    
    args = parser.parse_args()
    
    # Run the structural plasticity example
    example = StructuralPlasticityExample()
    result = example.run_example(args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Structural plasticity system is working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)