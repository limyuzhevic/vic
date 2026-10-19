#!/usr/bin/env python3
"""
Basic Brain Setup and Initialization Example

This example demonstrates the fundamental setup and initialization of an NLM brain.
It covers creating a brain with default configuration, initializing it, and performing basic operations.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
import numpy as np
from datetime import datetime

class BasicBrainExample:
    """Example class demonstrating basic brain setup and initialization."""
    
    def __init__(self):
        """Initialize the basic brain example."""
        self.brain = None
        self.config = None
        self.example_name = "Basic Brain Setup"
        
    def create_brain_with_default_config(self):
        """
        Create a brain using the default configuration.
        
        Returns:
            pynlm.Brain: Initialized brain object
        """
        print("Step 1: Creating brain with default configuration...")
        
        # Create default configuration
        self.config = pynlm.createDefaultConfig()
        print(f"  - Configuration created with {self.config.get('neuron_count', 'default')} neurons")
        
        # Create brain instance
        self.brain = pynlm.createBrain(self.config)
        print("  - Brain object created")
        
        return self.brain
        
    def initialize_brain(self, brain):
        """
        Initialize the brain for simulation.
        
        Args:
            brain: Brain object to initialize
            
        Returns:
            pynlm.Brain: Initialized brain object
        """
        print("Step 2: Initializing brain...")
        
        # Initialize the brain
        brain.initialize()
        print("  - Brain initialization completed")
        
        # Display some basic brain statistics
        neuron_count = brain.getTotalNeuronCount()
        region_count = brain.getRegionCount()
        
        print(f"  - Neuron count: {neuron_count}")
        print(f"  - Region count: {region_count}")
        
        return brain
        
    def demonstrate_basic_operations(self, brain):
        """
        Demonstrate basic brain operations and properties.
        
        Args:
            brain: Initialized brain object
        """
        print("Step 3: Demonstrating basic brain operations...")
        
        # Get brain configuration
        config = brain.getConfig()
        print(f"  - Brain configuration loaded")
        
        # Display brain state information
        print("  - Brain state information:")
        print(f"    * Total neurons: {brain.getTotalNeuronCount()}")
        print(f"    * Total synapses: {brain.getTotalSynapseCount()}")
        print(f"    * Active neurons: {brain.getActiveNeuronCount()}")
        print(f"    * Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    * Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"    * Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"    * Total spike count: {brain.getTotalSpikeCount()}")
        
        # Display developmental stage
        dev_stage = brain.getDevelopmentalStage()
        print(f"    * Developmental stage: {dev_stage}")
        
    def save_and_load_brain(self, brain, filepath="basic_brain.nld"):
        """
        Demonstrate brain save/load functionality.
        
        Args:
            brain: Brain object to save
            filepath: File path to save brain state
        """
        print("Step 4: Demonstrating brain persistence...")
        
        # Save brain state
        brain.save(filepath)
        print(f"  - Brain state saved to: {filepath}")
        
        # Verify file exists
        if os.path.exists(filepath):
            file_size = os.path.getsize(filepath) / 1024  # KB
            print(f"  - Saved file size: {file_size:.2f} KB")
            
        # Demonstrate loading (create new brain and load)
        print("  - Demonstrating brain load functionality...")
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        new_brain.load(filepath)
        print(f"  - Brain state successfully loaded from: {filepath}")
        
    def run_example(self):
        """Run the complete basic brain example."""
        print("=" * 60)
        print(f"NLM Basic Brain Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Create brain with default configuration
            self.create_brain_with_default_config()
            print()
            
            # Step 2: Initialize brain
            self.initialize_brain(self.brain)
            print()
            
            # Step 3: Demonstrate basic operations
            self.demonstrate_basic_operations(self.brain)
            print()
            
            # Step 4: Save and load brain
            self.save_and_load_brain(self.brain)
            print()
            
            print("=" * 60)
            print("Basic Brain Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain
            
        except Exception as e:
            print(f"Error in basic brain example: {e}")
            import traceback
            traceback.print_exc()
            return None

if __name__ == "__main__":
    # Run the basic brain example
    example = BasicBrainExample()
    result = example.run_example()
    
    if result is not None:
        print("\nExample completed successfully. The brain is ready for simulation.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)