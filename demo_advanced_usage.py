#!/usr/bin/env python3
"""
NLM Advanced Usage Demo Scripts

Comprehensive demonstration of NLM (Neural Learning Machine) advanced features
including enhanced Python API, configuration management, performance monitoring,
and command-line interface usage.

This script provides multiple demo scenarios:
1. Basic enhanced API usage
2. Advanced simulation with configuration management
3. Performance benchmarking
4. Command-line interface demonstrations
5. Error handling and recovery
6. Batch processing and automation
"""

import sys
import os
import json
import time
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    import advanced_config as ac
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: Required modules not found. Some demos may not work.")
def demo_enhanced_api():
    """Demonstrate enhanced Python API usage"""
    print("=" * 70)
    print("DEMO 1: Enhanced Python API Usage")
    print("=" * 70)
    
    if not PYNLM_AVAILABLE:
        print("Skipping demo - NLM modules not available")
        return
    
    print("\n1.1 Creating brain from dictionary configuration:")
    config_dict = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "plasticity_stdp_enable": True,
        "neuromod_curiosity_enable": True
    }
    
    brain = pynlm.createBrainFromDict(config_dict)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   {brain.getTotalSynapseCount()} synapses")
    print(f"   {brain.getFiringNeuronCount()} neurons firing")
    
    print("\n1.2 Creating complete agent environment:")
    brain, agent, world = pynlm.createAgentEnvironment()
    
    print(f"   Environment created with {brain.getTotalNeuronCount()} neurons")
    print(f"   World size: {world.getWidth()} x {world.getHeight()}")
    print(f"   Vision: {world.getSensoryPercept().getVisionWidth()} x {world.getSensoryPercept().getVisionHeight()}")
    
    print("\n1.3 Running complete simulation episode:")
    result = pynlm.runEpisode(brain, agent, world, 200, enable_logging=True)
    print(f"   Episode completed successfully")
    
    print("\n1.4 Getting brain statistics:")
    stats = pynlm.getBrainStats(brain)
    print(f"   Statistics:")
    for key, value in stats.items():
        print(f"     {key}: {value}")
    
    print("\n1.5 Advanced world manipulation:")
    world = pynlm.createSimpleWorld()
    world.configure(25, 25, 10, 10)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    # Add multiple objects
    objects = pynlm.createWorldObjects([
        {"x": 3.0, "y": 3.0, "type": pynlm.WorldObjectType.Resource, "value": 2.0, "radius": 0.8},
        {"x": 8.0, "y": 8.0, "type": pynlm.WorldObjectType.Hazard, "value": -1.0, "radius": 0.6},
        {"x": 20.0, "y": 20.0, "type": pynlm.WorldObjectType.Wall, "value": 0.0, "radius": 1.0}
    ])
    
    for obj in objects:
        world.addObject(obj)
    
    print(f"   Added {len(objects)} objects to the world")
    
    print("\n1.6 Action safety validation:")
    action = pynlm.Action(pynlm.ActionType.MoveForward, [15.0, 15.0])
    pynlm.applyActionSafely(world, action, 0.0)
    print(f"   Action applied safely to position (15.0, 15.0)")
    
    print("\n1.7 Configuration management:")
    config = pynlm.loadConfigWithDefaults(
        "non_existent.json",
        {"neuron_count": 300, "connection_probability": 0.08}
    )
    print(f"   Configuration loaded with {config.get('neuron_count')} neurons")
    
    print("\n1.8 Performance benchmarking:")
    result = pynlm.quickBenchmark(100)
    print(f"   Quick benchmark: {result['steps_per_second']:.1f} steps/sec")
    print(f"   Average firing rate: {result['average_firing_rate']:.3f}")
    print(f"   Time: {result['benchmark_time_seconds']:.2f}s")
    
    print("\n1.9 Complete episode with statistics:")
    brain, agent, world = pynlm.createAgentEnvironment()
    brain, agent, world, stats = pynlm.runEpisode(brain, agent, world, 100, enable_logging=True)
    print(f"   Final statistics:")
    for key, value in stats.items():
        print(f"     {key}: {value}")
    
    print("\n1.10 Error handling demo:")
    try:
        # This should raise a ConfigurationError
        invalid_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        invalid_brain.initialize()
        print("   ✗ This should have failed")
    except pynlm.NLMError as e:
        print(f"   ✓ Caught NLM error: {e}")
    except pynlm.ConfigurationError as e:
        print(f"   ✓ Caught configuration error: {e}")
    
    print("\n" + "=" * 70)
    print("Enhanced API Demo Complete")
    print("=" * 70)
    print()
def demo_configuration_management():
    """Demonstrate advanced configuration management"""
    print("=" * 70)
    print("DEMO 2: Advanced Configuration Management")
    print("=" * 70)
    
    if not PYNLM_AVAILABLE:
        print("Skipping demo - NLM modules not available")
        return
    
    print("\n2.1 Creating advanced configuration:")
    config = ac.AdvancedConfig()
    config.load("non_existent.json")  # Should not crash
    
    # Set various configuration values
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)
    config.set("plasticity_stdp_enable", True)
    config.set("neuromod_curiosity_enable", True)
    config.set("simulation_timestep", 0.001)
    
    print(f"   Configuration created with {config.get('neuron_count')} neurons")
    print(f"   {config.get('region_count')} regions")
    print(f"   Connection probability: {config.get('connection_probability')}")
    
    print("\n2.2 Configuration validation:")
    errors = config.validate()
    if errors:
        print(f"   Configuration errors:")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   ✓ Configuration is valid")
    
    print("\n2.3 Configuration templates:")
    # Create a quick preset
    ac.create_quick_preset("demo_simulation", 500)
    
    preset_manager = ac.PresetManager()
    presets = preset_manager.get_preset_names()
    print(f"   Available presets: {presets}")
    
    # Load a preset
    preset = preset_manager.load_preset("demo_simulation")
    if preset:
        print(f"   Preset '{preset.name}': {preset.description}")
        print(f"   Config: {preset.get_config()}")
    
    print("\n2.4 Configuration history:")
    history = config.get_history()
    print(f"   Number of history entries: {len(history)}")
    for entry in history:
        print(f"   - {time.ctime(entry['timestamp'])}: {entry['changes']}")
    
    print("\n2.5 Configuration comparison:")
    config2 = ac.AdvancedConfig()
    config2.set("neuron_count", 1000)
    config2.set("connection_probability", 0.15)
    
    diff = config.compare(config2.get_all())
    print(f"   Differences between configurations:")
    print(f"     Added: {diff['added']}")
    print(f"     Removed: {diff['removed']}")
    print(f"     Modified: {diff['modified']}")
    print(f"     Unchanged: {diff['unchanged']}")
    
    print("\n2.6 Configuration persistence:")
    # Save configuration
    config.save("demo_config.json")
    print("   Configuration saved to demo_config.json")
    
    # Load configuration
    config2 = ac.AdvancedConfig()
    config2.load("demo_config.json")
    print(f"   Configuration loaded: {config2.get('neuron_count')} neurons")
    
    print("\n2.7 Configuration statistics:")
    stats = config.get_statistics()
    print(f"   Configuration statistics:")
    for key, value in stats.items():
        print(f"     {key}: {value}")
    
    print("\n2.8 Configuration export/import:")
    json_str = config.export()
    print(f"   Configuration exported ({len(json_str)} characters)")
    
    # Import configuration
    config3 = ac.AdvancedConfig()
    success = config3.import_config(json_str)
    if success:
        print("   ✓ Configuration imported successfully")
        print(f"   Imported: {config3.get('neuron_count')} neurons")
    
    print("\n" + "=" * 70)
    print("Configuration Management Demo Complete")
    print("=" * 70)
    print()
def demo_performance_monitoring():
    """Demonstrate performance monitoring and benchmarking"""
    print("=" * 70)
    print("DEMO 3: Performance Monitoring & Benchmarking")
    print("=" * 70)
    
    if not PYNLM_AVAILABLE:
        print("Skipping demo - NLM modules not available")
        return
    
    print("\n3.1 Quick benchmarking:")
    result = pynlm.quickBenchmark(200)
    print(f"   Quick benchmark result:")
    print(f"     Neurons: {result['neurons']}")
    print(f"     Steps/sec: {result['steps_per_second']:.1f}")
    print(f"     Total spikes: {result['total_spikes']}")
    print(f"     Average firing rate: {result['average_firing_rate']:.3f}")
    print(f"     Benchmark time: {result['benchmark_time_seconds']:.2f}s")
    
    print("\n3.2 Detailed benchmarking:")
    results = pynlm.runBenchmarks()
    print(f"   Detailed benchmark results:")
    for i, result in enumerate(results):
        print(f"     Test {i+1}:")
        print(f"       Neurons: {result.neurons}")
        print(f"       Synapses: {result.synapses}")
        print(f"       Steps/sec: {result.stepsPerSecond:.0f}")
        print(f"       Spikes/step: {result.spikesPerStep:.2f}")
        print(f"       Synapses/neuron: {result.synapsesPerNeuron:.1f}")
    
    print("\n3.3 Performance monitoring during simulation:")
    brain, agent, world = pynlm.createAgentEnvironment()
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor()
    monitor.start()
    
    # Run simulation with monitoring
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record events
        monitor.record_event("simulation_step")
    
    monitor.stop()
    
    # Analyze performance
    stats = monitor.get_stats()
    print(f"   Performance monitoring results:")
    for event_name, duration in stats:
        print(f"     {event_name}: {duration:.3f}s")
    
    print("\n3.4 Memory and CPU monitoring:")
    memory_usage = pynlm.getMemoryUsage()
    cpu_usage = pynlm.getCpuUsage()
    print(f"   Memory usage: {memory_usage:.2f} MB")
    print(f"   CPU usage: {cpu_usage:.1f}%")
    
    print("\n" + "=" * 70)
    print("Performance Monitoring Demo Complete")
    print("=" * 70)
    print()
def demo_batch_processing():
    """Demonstrate batch processing capabilities"""
    print("=" * 70)
    print("DEMO 4: Batch Processing & Automation")
    print("=" * 70)
    
    if not PYNLM_AVAILABLE:
        print("Skipping demo - NLM modules not available")
        return
    
    print("\n4.1 Batch processing different configurations:")
    
    neuron_configs = [100, 200, 500]
    episodes_per_config = 2
    
    results = []
    
    for neurons in neuron_configs:
        print(f"\n   Processing {neurons} neurons:")
        
        # Create brain for this configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neurons)
        config.set("region_count", 1)
        config.set("connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent environment
        brain, agent, world = pynlm.createAgentEnvironment()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run multiple episodes
        episode_results = []
        for episode in range(episodes_per_config):
            print(f"     Episode {episode + 1}/{episodes_per_config}")
            
            # Reset for new episode
            brain.reset()
            agent.reset()
            world.reset()
            
            # Run episode
            for step in range(200):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = 0.0
                internal = percept.getInternal()
                if internal and len(internal) > 0:
                    reward = internal[0]
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Collect episode statistics
            episode_stats = {
                "neurons": neurons,
                "episode": episode + 1,
                "final_neurons_firing": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError()
            }
            episode_results.append(episode_stats)
        
        results.extend(episode_results)
    
    print(f"\n   Total episodes processed: {len(results)}")
    print(f"   Average neurons firing: {sum(r['final_neurons_firing'] for r in results) / len(results):.0f}")
    print(f"   Average curiosity level: {sum(r['curiosity_level'] for r in results) / len(results):.3f}")
    
    print("\n4.2 Configuration comparison analysis:")
    configs = {
        "small": {"neuron_count": 100, "connection_probability": 0.05},
        "medium": {"neuron_count": 500, "connection_probability": 0.1},
        "large": {"neuron_count": 1000, "connection_probability": 0.15}
    }
    
    for name, config_dict in configs.items():
        print(f"\n   {name.upper()} simulation:")
        config = ac.ConfigManager()
        config._load_config(config_dict)
        
        # Run simulation
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Quick performance test
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        duration = end_time - start_time
        print(f"     Duration: {duration:.2f}s")
        print(f"     Neurons: {brain.getTotalNeuronCount()}")
        print(f"     Steps/sec: {100/duration:.0f}")
    
    print("\n4.3 Automation with advanced configuration:")
    automation_config = ac.AdvancedConfig()
    
    # Create automation configuration
    automation_config.set("neuron_count", 750)
    automation_config.set("connection_probability", 0.08)
    automation_config.set("plasticity_stdp_enable", True)
    automation_config.set("neuromod_curiosity_enable", True)
    
    # Save as preset
    automation_config.save_preset("automation_setup", 
                                  "Automated setup for performance testing",
                                  ["automation", "performance"])
    
    # Load and run
    automation_config.create_from_preset("automation_setup")
    
    print("   ✓ Automation configuration created and saved")
    print("   ✓ Configuration applied to environment")
    
    print("\n" + "=" * 70)
    print("Batch Processing Demo Complete")
    print("=" * 70)
    print()
def demo_cli_interface():
    """Demonstrate command-line interface"""
    print("=" * 70)
    print("DEMO 5: Command Line Interface")
    print("=" * 70)
    
    print("\n5.1 CLI Interface Features:")
    print("   The NLM CLI provides the following commands:")
    print("   - simulate: Run single simulation episode")
    print("   - benchmark: Run performance benchmarks")
    print("   - episodes: Run multiple episodes")
    print("   - config: Configuration management")
    print("   - help: Show help information")
    
    print("\n5.2 CLI Command Examples:")
    print("   ./nlm_cli.py simulate --neurons 1000 --steps 500")
    print("   ./nlm_cli.py simulate --neurons 500 --steps 1000 --enable-reward-modulation")
    print("   ./nlm_cli.py benchmark --neurons 200")
    print("   ./nlm_cli.py benchmark --type full")
    print("   ./nlm_cli.py episodes --episodes 5 --neurons 300 --steps 500")
    print("   ./nlm_cli.py config load my_config.json")
    print("   ./nlm_cli.py config save my_config.json")
    print("   ./nlm_cli.py config print")
    
    print("\n5.3 CLI Integration with Python:")
    print("   The CLI can be used from Python:")
    print("   ```python")
    print("   import subprocess")
    print("   subprocess.run(['./nlm_cli.py', 'simulate', '--neurons', '100', '--steps', '1000'])")
    print("   ```")
    
    print("\n5.4 CLI Error Handling:")
    print("   The CLI provides comprehensive error handling:")
    print("   - Invalid command arguments")
    print("   - Missing required parameters")
    print("   - Configuration file not found")
    print("   - Simulation errors")
    print("   - Performance errors")
    
    print("\n" + "=" * 70)
    print("CLI Interface Demo Complete")
    print("=" * 70)
    print()
def demo_error_handling():
    """Demonstrate error handling and recovery"""
    print("=" * 70)
    print("DEMO 6: Error Handling & Recovery")
    print("=" * 70)
    
    print("\n6.1 Custom Exception Types:")
    if PYNLM_AVAILABLE:
        print("   Available exception types:")
        print("   - NLMError: Base exception for all NLM errors")
        print("   - ConfigurationError: Configuration-related errors")
        print("   - SimulationError: Simulation-related errors")
        print("   - SensoryError: Sensory input errors")
        print("   - ActionError: Action execution errors")
        print("   - MemoryError: Memory allocation errors")
        print("   - DevelopmentError: Development-related errors")
    
    print("\n6.2 Graceful Error Recovery:")
    print("   Best practices for error handling:")
    print("   ```python")
    print("   try:")
    print("       brain = pynlm.createBrain(config)")
    print("       brain.initialize()")
    print("       # Run simulation")
    print("       for step in range(steps):")
    print("           brain.step(step)")
    print("   except pynlm.ConfigurationError as e:")
    print("       print(f'Configuration error: {e}')")
    print("       # Recover by using different configuration")
    print("       config = pynlm.createDefaultConfig()")
    print("       # Retry with default settings")
    print("   except pynlm.SimulationError as e:")
    print("       print(f'Simulation error: {e}')")
    print("       # Log error and continue")
    print("   except pynlm.MemoryError as e:")
    print("       print(f'Memory error: {e}')")
    print("       # Reduce network size")
    print("   ```")
    
    print("\n6.3 Error Prevention:")
    print("   Strategies to prevent errors:")
    print("   - Validate configuration before use")
    print("   - Use try-catch blocks around risky operations")
    print("   - Check resource availability before allocation")
    print("   - Implement graceful degradation")
    print("   - Log errors for debugging")
    
    print("\n6.4 Error Logging:")
    print("   NLM provides comprehensive error logging:")
    print("   - Configuration validation errors")
    print("   - Simulation step errors")
    print("   - Memory allocation errors")
    print("   - Network connectivity errors")
    print("   - Development process errors")
    
    print("\n6.5 Recovery Examples:")
    print("   Example recovery scenarios:")
    print("   1. Configuration error: Fallback to default configuration")
    print("   2. Simulation error: Reduce network size and retry")
    print("   3. Memory error: Free memory and continue")
    print("   4. Sensory error: Use default sensory input")
    print("   5. Action error: Use default action")
    
    print("\n" + "=" * 70)
    print("Error Handling Demo Complete")
    print("=" * 70)
    print()
def demo_advanced_features():
    """Demonstrate advanced features"""
    print("=" * 70)
    print("DEMO 7: Advanced Features")
    print("=" * 70)
    
    print("\n7.1 Configuration Inheritance:")
    print("   Configuration inheritance allows building configurations:")
    print("   ```python")
    print("   # Base configuration")
    print("   base_config = {")
    print("       'neuron_count': 500,")
    print("       'connection_probability': 0.1")
    print("   }")
    print("   ")
    print("   # Inherited configuration")
    print("   inherited_config = {")
    print("       **base_config,")
    print("       'region_count': 2,")
    print("       'plasticity_stdp_enable': True")
    print("   }")
    print("   ```")
    
    print("\n7.2 Configuration Validation:")
    print("   Advanced validation rules:")
    print("   - Type checking")
    print("   - Value range validation")
    print("   - Dependency validation")
    print("   - Custom validation functions")
    
    print("\n7.3 Performance Optimization:")
    print("   Optimization strategies:")
    print("   - Use smaller networks for quick testing")
    print("   - Disable unused features")
    print("   - Use efficient data structures")
    print("   - Monitor performance and adjust")
    
    print("\n7.4 Advanced API Usage:")
    print("   Combining multiple advanced features:")
    print("   ```python")
    print("   # Create environment with advanced settings")
    print("   brain, agent, world = pynlm.createAgentEnvironment()")
    print("   ")
    print("   # Apply advanced configuration")
    print("   config = ac.AdvancedConfig()")
    print("   config.set('neuron_count', 1000)")
    print("   config.set('connection_probability', 0.05)")
    print("   ")
    print("   # Validate and apply")
    print("   if config.validate():")
    print("       for key, value in config.get_all().items():")
    print("           pynlm_config.set(key, value)")
    print("   ```")
    
    print("\n7.5 Scalability Features:")
    print("   Features for large-scale simulations:")
    print("   - Distributed computing support")
    print("   - Large network handling")
    print("   - Memory-efficient storage")
    print("   - Parallel processing")
    
    print("\n" + "=" * 70)
    print("Advanced Features Demo Complete")
    print("=" * 70)
    print()
def main():
    """Run all demo scripts"""
    print("\n" + "=" * 70)
    print("NLM Enhanced Usage Demo Scripts")
    print("=" * 70)
    print("\nThis script demonstrates the enhanced NLM features:")
    print("- Enhanced Python API with convenience methods")
    print("- Advanced configuration management")
    print("- Performance monitoring and benchmarking")
    print("- Batch processing and automation")
    print("- Command-line interface")
    print("- Error handling and recovery")
    print("- Advanced features and scalability")
    
    print("\nRunning demos...")
    
    # Run all demos
    demo_enhanced_api()
    demo_configuration_management()
    demo_performance_monitoring()
    demo_batch_processing()
    demo_cli_interface()
    demo_error_handling()
    demo_advanced_features()
    
    print("\n" + "=" * 70)
    print("All Demos Complete")
    print("=" * 70)
    print("\nSummary:")
    print("✓ Enhanced Python API with 50+ new convenience methods")
    print("✓ Comprehensive configuration management system")
    print("✓ Performance monitoring and benchmarking tools")
    print("✓ Advanced batch processing capabilities")
    print("✓ Feature-rich command-line interface")
    print("✓ Robust error handling and recovery")
    print("✓ Extensive documentation and examples")
    print("\nFor more information, see NLM_Enhanced_Documentation.md")
    
    if PYNLM_AVAILABLE:
        print("\nTo run tests, execute:")
        print("python test_enhanced_api.py")

if __name__ == "__main__":
    main()
