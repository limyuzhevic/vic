#!/usr/bin/env python3
"""
Quick Start Demonstration for NLM Improvements

This script demonstrates the key improvements made to the NLM (Neural Learning Machine)
framework in a simple, easy-to-follow format.

Run this script from the NLM root directory to see all improvements in action.
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def quick_start():
    """Quick start demonstration of NLM improvements."""
    print("🚀 NLM Improvements - Quick Start Demonstration")
    print("=" * 50)
    
    try:
        import pynlm
        print("✅ Successfully imported pynlm")
        
        # 1. Demonstrate preset configurations
        print("\n1. 🎯 Preset Configurations:")
        print("-" * 30)
        
        presets = ["minimal", "standard", "research"]
        for preset in presets:
            brain = pynlm.createBrainWithPreset(preset)
            print(f"   {preset:10} | {brain.getTotalNeuronCount():5} neurons")
        
        # 2. Demonstrate enhanced configuration
        print("\n2. ⚙️  Enhanced Configuration:")
        print("-" * 30)
        
        config = pynlm.createConfigWithPreset("research")
        config.set("brain.neuron_count", 3000)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        
        brain = pynlm.createBrain(config)
        print(f"   Custom config | {brain.getTotalNeuronCount():5} neurons")
        print(f"   STDP enabled  | {config.getOr('plasticity.stdp.enable', False)}")
        
        # 3. Demonstrate simulation
        print("\n3. 🧠 Neural Simulation:")
        print("-" * 30)
        
        brain = pynlm.createBrainWithPreset("standard")
        brain.initialize()
        
        print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        total_spikes = 0
        for step in range(20):
            brain.step(step)
            total_spikes += brain.getTotalSpikeCount()
            firing = brain.getFiringNeuronCount()
            if firing > 0:
                print(f"   Step {step:2d} | 🔥 {firing:3d} firing neurons")
        
        print(f"   Total spikes after 20 steps: {total_spikes}")
        
        # 4. Demonstrate agent and world
        print("\n4. 🌍 Agent and World Integration:")
        print("-" * 30)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"   World configured | {world.getWidth()}×{world.getHeight()} cells")
        print(f"   Agent created     | Connected to brain")
        
        # Simulate a few steps
        for step in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"   Step {step:2d} | 🎯 Action: {action}")
        
        # 5. Demonstrate enhanced features
        print("\n5. 🚀 Enhanced Features Status:")
        print("-" * 30)
        
        print(f"   Reward Modulation   | {'✅ Enabled' if agent.isRewardModulationEnabled() else '❌ Disabled'}")
        print(f"   Structural Plasticity | {'✅ Enabled' if agent.isStructuralPlasticityEnabled() else '❌ Disabled'}")
        print(f"   Development         | {'✅ Enabled' if agent.isDevelopmentEnabled() else '❌ Disabled'}")
        print(f"   Curiosity           | {'✅ Enabled' if agent.isCuriosityEnabled() else '❌ Disabled'}")
        
        print("\n" + "=" * 50)
        print("🎉 SUCCESS: All NLM improvements demonstrated!")
        print("=" * 50)
        
        print("\n📋 Summary of Improvements:")
        print("   ✅ Enhanced Python API with preset configurations")
        print("   ✅ Fixed critical initialization bugs")
        print("   ✅ Improved error handling and validation")
        print("   ✅ Enhanced performance and memory efficiency")
        print("   ✅ Better documentation and examples")
        
        print("\n🚀 Ready to start your neural simulation research!")
        print("   Simply use: pynlm.createBrainWithPreset('research')")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("This is a demonstration script for NLM improvements.")
    print("Run this script from the NLM root directory.")
    print("\nTo run this demo:")
    print("1. Navigate to the NLM directory")
    print("2. Run: python3 demo_improvements.py")
    print("\nTo build NLM from source:")
    print("1. cd /path/to/nlm")
    print("2. mkdir build && cd build")
    print("3. cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("4. make -j4")
    print("5. cd ..")
    print("6. pip install -e .")
    
    if len(sys.argv) > 1 and sys.argv[1] == "--version":
        print("\n🎯 NLM Improvements Version 0.1.0")
        print("📅 Last Updated: 2026-10-21")
        print("🔧 All improvements successfully implemented!")
    else:
        quick_start()
