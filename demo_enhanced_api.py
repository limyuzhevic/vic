#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python API Improvements

This script demonstrates the new research-oriented features and convenience methods
added to the NLM Python bindings, making it easier for both beginners and advanced
researchers to work with the neural simulation framework.
"""

import pynlm
import json
import time

def main():
    print("=== Enhanced NLM Python API Demonstration ===\n")
    
    # 1. Quick Start with Convenience Methods
    print("1. Quick Start - Creating a research brain with sensible defaults")
    print("=" * 60)
    
    # Use the new convenience function for research
    brain = pynlm.createResearchBrain()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # 2. Research Environment Setup
    print("2. Setting up a standard research environment")
    print("=" * 60)
    
    world = pynlm.createStandardEnvironment()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    print(f"Agent initialized with {agent.getSensoryInputSize()} sensory input dimensions")
    print()
    
    # 3. Running a Complete Research Experiment
    print("3. Running a complete research experiment")
    print("=" * 60)
    
    # Enable learning and curiosity for more interesting behavior
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    start_time = time.time()
    
    # Use the new research experiment runner
    results = pynlm.runResearchExperiment(
        brain=brain,
        world=world,
        agent=agent,
        numSteps=500,
        enableLearning=True,
        enableCuriosity=True
    )
    
    end_time = time.time()
    
    print(f"Experiment completed in {end_time - start_time:.2f} seconds")
    print("Results:")
    for key, value in results.items():
        print(f"  {key}: {value}")
    print()
    
    # 4. Exploring Research Presets
    print("4. Exploring research configuration presets")
    print("=" * 60)
    
    presets = pynlm.getResearchPresets()
    for name, preset in presets.items():
        print(f"\n{name.upper()} preset:")
        for key, value in preset.items():
            if key != "name":
                print(f"  {key}: {value}")
    print()
    
    # 5. Basic Data Analysis
    print("5. Basic data analysis and export")
    print("=" * 60)
    
    # Create a simple analysis dictionary
    analysis = {
        "experiment_type": "neural_dynamics",
        "brain_size": brain.getTotalNeuronCount(),
        "total_spikes": results.get("total_spikes", 0),
        "learning_enabled": results.get("learning_enabled", False),
        "curiosity_enabled": results.get("curiosity_enabled", False),
        "final_development_stage": str(results.get("final_development_stage", "Unknown")),
        "analysis_timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    }
    
    print("Analysis Summary:")
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    # Export results using the convenience function
    filename = "nlm_experiment_results.csv"
    try:
        success = pynlm.exportExperimentResults(results, filename)
        if success:
            print(f"\n✅ Results successfully exported to {filename}")
            
            # Show a preview of the CSV file
            print(f"\nCSV file preview ({filename}):")
            with open(filename, 'r') as f:
                lines = f.readlines()
                for i, line in enumerate(lines[:min(6, len(lines))]):
                    print(f"  {i+1}: {line.strip()}")
                    if i >= 4:
                        break
                if len(lines) > 6:
                    print(f"  ... ({len(lines)-6} more lines)")
        else:
            print(f"\n❌ Failed to export results to {filename}")
    except Exception as e:
        print(f"\n❌ Error exporting results: {e}")
    
    print("\n" + "=" * 60)
    print("Enhanced NLM API demonstration completed successfully!")
    print("\nKey improvements demonstrated:")
    print("• Convenience functions for easy research setup")
    print("• Automated research experiment runner")
    print("• Standardized research environments")
    print("• Result export for analysis")
    print("• Research configuration presets")
    print("• Better error handling and documentation")
    print("=" * 60)

if __name__ == "__main__":
    main()
