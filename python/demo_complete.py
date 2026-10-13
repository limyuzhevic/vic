#!/usr/bin/env python3
"""
Complete NLM Enhanced Python Bindings Demo

This comprehensive demo showcases all the enhanced features of the NLM Python bindings
for Phase 6: Final Integration of Artificial Brain Systems.

This demo requires:
1. Enhanced Python bindings built and installed
2. Knowledge of basic NLM concepts

Run:
python demo_complete.py
"""

import pynlm
import time

def demo_header(title, description=""):
    """Print formatted demo header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    if description:
        print(f"{description}")
    print(f"{'='*70}")

def intro_demo():
    """Introduction to enhanced bindings"""
    demo_header("Welcome to Enhanced NLM Python Bindings Demo",
                "This demo showcases Phase 6: Final Integration of Artificial Brain Systems")
    
    print("\nKey Features Demonstrated:")
    print("1. Configuration presets and templates")
    print("2. Advanced brain introspection and analysis")
    print("3. Enhanced neuromodulation control")
    print("4. Development cycle simulation")
    print("5. Performance benchmarking")
    print("6. Batch operations for efficiency")
    print("7. Complete episode simulation")
    
    print(f"\nNLM Version: {pynlm.version()}")
    print(f"Available presets: {list(pynlm.PRESETS.keys())}")

def configuration_demo():
    """Demonstrate configuration presets and management"""
    demo_header("Configuration Presets and Management",
                "Show how to use different configuration presets for various scenarios")
    
    print("\n1. Available Configuration Presets:")
    for preset_name, preset_config in pynlm.PRESETS.items():
        print(f"\n   Preset: {preset_name.upper()}")
        print(f"   Config: {preset_config}")
    
    print("\n2. Creating and using configurations:")
    
    # Minimal configuration
    print("\n   a) Minimal Configuration (for testing):")
    minimal = pynlm.getMinimalConfig()
    print(f"      Neurons: {minimal.get('brain.neuron_count')}")
    print(f"      Regions: {minimal.get('brain.region_count')}")
    print(f"      STDP: {minimal.get('plasticity.stdp.enable')}")
    
    # Standard configuration
    print("\n   b) Standard Configuration (for general use):")
    standard = pynlm.getStandardConfig()
    print(f"      Neurons: {standard.get('brain.neuron_count')}")
    print(f"      Regions: {standard.get('brain.region_count')}")
    print(f"      Development: {standard.get('development.enable')}")
    
    # Research configuration
    print("\n   c) Research Configuration (for complex scenarios):")
    research = pynlm.getResearchConfig()
    print(f"      Neurons: {research.get('brain.neuron_count')}")
    print(f"      Regions: {research.get('brain.region_count')}")
    print(f"      Structural Plasticity: {research.get('plasticity.structural.enable')}")
    
    print("\n3. Advanced Configuration Management:")
    print("   Creating custom configuration:")
    
    custom = pynlm.createDefaultConfig()
    custom.set("brain.neuron_count", 3000)
    custom.set("brain.region_count", 4)
    custom.set("brain.connection_probability", 0.12)
    custom.set("plasticity.stdp.enable", True)
    custom.set("plasticity.hebbian.enable", True)
    custom.set("plasticity.structural.enable", True)
    custom.set("neuromod.dopamine.scale", 1.3)
    custom.set("neuromod.curiosity.enable", True)
    custom.set("neuromod.novelty.enable", True)
    custom.set("neuromod.prediction_error.enable", True)
    custom.set("development.enable", True)
    
    print(f"      Custom neurons: {custom.get('brain.neuron_count')}")
    print(f"      Custom regions: {custom.get('brain.region_count')}")
    print(f"      Development enabled: {custom.get('development.enable')}")
    
    print("\nConfiguration demo completed successfully!")

def brain_analysis_demo():
    """Demonstrate advanced brain analysis and introspection"""
    demo_header("Brain Analysis and Introspection",
                "Comprehensive analysis of brain structure, function, and state")
    
    print("\n1. Creating research brain:")
    config = pynlm.getResearchConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   {brain.getTotalSynapseCount()} synapses")
    print(f"   {brain.getRegionCount()} regions")
    
    print("\n2. Comprehensive Brain Statistics:")
    stats = brain.getNeuronStats()
    print(f"   Neuron Statistics:")
    print(f"     Total: {stats['total']}")
    print(f"     Active: {stats['active']} ({stats['active']/stats['total']*100:.1f}%)")
    print(f"     Firing (current): {stats['firing']}")
    print(f"     Average firing rate: {stats['avg_firing_rate']:.3f}")
    print(f"     Total spikes: {stats['spike_count']}")
    print(f"     E/I ratio: {stats['ei_ratio']:.3f}")
    print(f"     Pending spike events: {stats['pending_spikes']}")
    
    print("\n3. Regional Brain Analysis:")
    regions = brain.getRegionStats()
    print(f"   Regions analyzed:")
    for region in regions:
        print(f"     Region {region['id']} ({region['name']}):")
        print(f"       Neurons: {region['neuron_count']}")
        print(f"       Synapses: {region['synapse_count']}")
        print(f"       Average synaptic weight: {region['avg_weight']:.4f}")
        activity = "Active" if region['neuron_count'] > 0 else "Inactive"
        print(f"       Activity level: {activity}")
    
    print("\n4. Neuromodulation Analysis:")
    neuromod = brain.getNeuromodulationStats()
    print(f"   Neuromodulator levels:")
    for key, value in neuromod.items():
        print(f"     {key}: {value:.3f}")
    
    print("\n5. Memory System Analysis:")
    memory = brain.getMemoryStats()
    print(f"   Memory system status:")
    for key, value in memory.items():
        print(f"     {key}: {value}")
    
    print("\n6. Development Analysis:")
    stage = brain.getDevelopmentalStage()
    print(f"   Current developmental stage: {stage}")
    print(f"   Plasticity factor: {stage.plasticity_factor():.3f}")
    print(f"   Plasticity status: {'Plastic' if stage.is_plastic() else 'Mature'}")
    
    print("\nBrain analysis demo completed successfully!")

def enhanced_agent_demo():
    """Demonstrate enhanced agent with full neuromodulation"""
    demo_header("Enhanced Agent with Neuromodulation",
                "Complete agent simulation with advanced neuromodulation control")
    
    print("\n1. Setting up enhanced agent environment:")
    
    config = pynlm.getResearchConfig()
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Agent environment configured:")
    print(f"     World size: {world.getWidth()} x {world.getHeight()}")
    print(f"     Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"     Agent motor output size: {agent.getMotorOutputSize()}")
    print(f"     Neuromodulation enabled: Reward={agent.isRewardModulationEnabled()}, "
          f"Dev={agent.isDevelopmentEnabled()}, Curiosity={agent.isCuriosityEnabled()}")
    
    print("\n2. Running enhanced agent simulation:")
    print(f"   Running 50 steps of enhanced agent simulation...")
    
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"\n   Step {step}:")
            print(f"     Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"     Energy: {world.getAgentBody().energy:.1f}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Development stage: {brain.getDevelopmentalStage()}")
    
    print("\n3. Final Agent Status:")
    print(f"   Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Total neurons processed: {brain.getTotalNeuronCount() * 50}")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    
    print("\nEnhanced agent demo completed successfully!")

def performance_demo():
    """Demonstrate performance features and benchmarking"""
    demo_header("Performance Features and Benchmarking",
                "Demonstrate batch operations and performance monitoring")
    
    print("\n1. Benchmarking different configurations:")
    
    configs = [
        ("Minimal", pynlm.getMinimalConfig()),
        ("Standard", pynlm.getStandardConfig()),
        ("Research", pynlm.getResearchConfig())
    ]
    
    for name, config in configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark single steps
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        duration = end_time - start_time
        print(f"\n   {name} Configuration:")
        print(f"     Single step average: {duration/100*1000:.2f} ms")
        print(f"     Steps per second: {100/duration:.0f}")
        print(f"     Neurons processed: {brain.getTotalNeuronCount() * 100}")
    
    print("\n2. Testing batch operations:")
    config = pynlm.getStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    brain.stepMultiple(0, 1000, 0.001)
    end_time = time.time()
    
    duration = end_time - start_time
    print(f"\n   Batch operation (1000 steps):")
    print(f"     Total time: {duration:.3f} seconds")
    print(f"     Average per step: {duration/1000*1000:.2f} ms")
    print(f"     Throughput: {1000/duration:.0f} steps/second")
    
    print("\nPerformance demo completed successfully!")

def development_demo():
    """Demonstrate development cycle and maturation"""
    demo_header("Development Cycle and Maturation",
                "Simulate brain development through different stages")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\n1. Starting development simulation:")
    print(f"   Initial developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Initial plasticity factor: {brain.getDevelopmentalStage().plasticity_factor():.3f}")
    
    print(f"\n2. Simulating development over time:")
    print(f"   Running development simulation...")
    
    for stage in range(0, 600, 100):
        # Run development steps
        for step in range(10):
            brain.step(stage + step)
        
        current_stage = brain.getDevelopmentalStage()
        print(f"\n   After {stage + 10} steps:")
        print(f"     Developmental stage: {current_stage}")
        print(f"     Plasticity factor: {current_stage.plasticity_factor():.3f}")
        print(f"     Activity: {brain.getFiringNeuronCount()} firing neurons")
        print(f"     Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        if current_stage.is_mature():
            print(f"     Status: Mature")
        elif current_stage.is_plastic():
            print(f"     Status: Plastic (High learning capacity)")
    
    print(f"\n3. Final Development Status:")
    final_stage = brain.getDevelopmentalStage()
    print(f"   Final stage: {final_stage}")
    print(f"   Final plasticity: {final_stage.plasticity_factor():.3f}")
    print(f"   Learning capacity: {'High' if final_stage.is_plastic() else 'Low'}")
    
    print("\nDevelopment demo completed successfully!")

def complete_episode_demo():
    """Demonstrate complete episode with convenience methods"""
    demo_header("Complete Episode Simulation",
                "Run complete agent episodes with convenience methods")
    
    config = pynlm.getResearchConfig()
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    world.setAgentStart(7.5, 7.5)
    
    print(f"\n1. Starting complete episode simulation:")
    print(f"   Episode length: 100 steps")
    print(f"   World size: {world.getWidth()} x {world.getHeight()}")
    print(f"   Initial position: (7.5, 7.5)")
    
    print(f"\n2. Running complete episode:")
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"\n   Step {step}:")
            print(f"     Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"     Orientation: {world.getAgentBody().orientation:.1f}")
            print(f"     Energy: {world.getAgentBody().energy:.1f}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"\n3. Episode Results:")
    print(f"   Final development stage: {brain.getDevelopmentalStage()}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Total neurons processed: {brain.getTotalNeuronCount() * 100}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\nComplete episode demo completed successfully!")

def main():
    """Run complete demo"""
    intro_demo()
    
    configuration_demo()
    brain_analysis_demo()
    enhanced_agent_demo()
    performance_demo()
    development_demo()
    complete_episode_demo()
    
    demo_header("Demo Complete!", 
                "All enhanced NLM Python bindings features have been demonstrated successfully.")
    
    print("\nSummary of Enhanced Features:")
    print("✓ Configuration presets and templates")
    print("✓ Advanced brain introspection and analysis")
    print("✓ Enhanced neuromodulation control")
    print("✓ Development cycle simulation")
    print("✓ Performance benchmarking")
    print("✓ Batch operations for efficiency")
    print("✓ Complete episode simulation")
    
    print(f"\nNext Steps:")
    print("1. Build enhanced bindings:")
    print("   cd /workspace/app")
    print("   mkdir build && cd build")
    print("   cmake .. && make -j4")
    print("   cd python && pip install -e .")
    print("\n2. Run additional examples:")
    print("   python python/examples/basic_usage.py")
    print("   python python/examples/advanced_features.py")
    print("\n3. Explore documentation in docs/ directory")
    print("\n4. Modify configurations for your specific use case")
    
    print("\nEnhanced NLM Python Bindings Demo Complete!")
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
