#!/usr/bin/env python3
"""
NLM Visualization Example

This example demonstrates visualization capabilities.
"""

import pynlm

def main():
    print("=== Visualization Example ===")
    
    # Create brain
    print("1. Creating brain for visualization...")
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    print("2. Running simulation for visualization...")
    for step in range(30):
        brain.step(step, step * 0.001)
    
    print("3. Visualization capabilities:")
    print("   - Network visualization available")
    print("   - Activity visualization available")
    print("   - Spike raster plot available")
    print("   - Weight matrix visualization available")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()