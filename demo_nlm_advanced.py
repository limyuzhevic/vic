#!/usr/bin/env python3
"""
NLM Project Demonstration Script
Shows how to use the new Advanced Features of NLM

This script demonstrates the enhanced features of NLM, including:
1. Advanced Configuration Management
2. Working Memory and Learning
3. Performance Profiling
4. Advanced Command-Line Interface
"""

import sys
import os
import tempfile

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def demo_basic_usage():
    """Demonstrate basic NLM usage"""
    print("=== Basic NLM Usage Demo ===")
    
    try:
        import pynlm
        
        # Create a simple brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few simulation steps
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Basic simulation completed")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
    except Exception as e:
        print(f"✗ Basic usage demo failed: {e}")
        return False
    
    return True

def demo_advanced_configuration():
    """Demonstrate advanced configuration management"""
    print("\n=== Advanced Configuration Demo ===")
    
    try:
        import pynlm
        
        # Create a brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Show configuration
        print(f"✓ Custom configuration applied")
        print(f"  Neuron count: {brain.getConfig().getOr('brain.neuron_count', 0)}")
        print(f"  Synapse density: {brain.getConfig().getOr('brain.synapse_density', 0.0)}")
        print(f"  STDP enabled: {brain.getConfig().getOr('plasticity.stdp.enable', false)}")
        print(f"  Hebbian enabled: {brain.getConfig().getOr('plasticity.hebbian.enable', false)}")
        print(f"  Curiosity enabled: {brain.getConfig().getOr('neuromod.curiosity.enable', false)}")
        
    except Exception as e:
        print(f"✗ Advanced configuration demo failed: {e}")
        return False
    
    return True

def demo_working_memory():
    """Demonstrate working memory and learning"""
    print("\n=== Working Memory Demo ===")
    
    try:
        import pynlm
        
        # Create a brain with memory features
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent with memory systems
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run a learning episode
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 5 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        print(f"✓ Working memory demo completed")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
        
    except Exception as e:
        print(f"✗ Working memory demo failed: {e}")
        return False
    
    return True

def demo_performance_profiling():
    """Demonstrate performance profiling"""
    print("\n=== Performance Profiling Demo ===")
    
    try:
        import pynlm
        from profiling import PerformanceProfiler
        
        # Initialize performance profiler
        PerformanceProfiler::getInstance().clearMetrics()
        PerformanceProfiler::getInstance().setEnabled(true)
        
        # Create and run a simulation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with profiling
        start_time = __import__('time').time()
        
        for step in range(50):
            brain.step(step)
        
        end_time = __import__('time').time()
        
        # Stop profiling
        PerformanceProfiler::getInstance().setEnabled(false)
        
        print(f"✓ Performance profiling completed")
        print(f"  Simulation time: {end_time - start_time:.3f} seconds")
        print(f"  Average step time: {(end_time - start_time) / 50:.3f} seconds")
        print(f"  Total neurons processed: {brain.getTotalNeuronCount()}")
        print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
        
        # Print performance report
        PerformanceProfiler::getInstance().printReport()
        
    except Exception as e:
        print(f"✗ Performance profiling demo failed: {e}")
        return False
    
    return True

def demo_command_line_interface():
    """Demonstrate advanced command-line interface"""
    print("\n=== Command-Line Interface Demo ===")
    
    try:
        from cli import AdvancedCLI
        
        # Create and parse command line arguments
        cli = AdvancedCLI()
        
        # Simulate command line arguments
        test_args = [
            "--help",  # Show help
            "--config", "test_config.cfg",  # Configuration file
            "--output", "json",  # Output format
            "--verbose",  # Verbose mode
            "run",  # Command
        ]
        
        # Parse arguments (simplified for demo)
        print(f"✓ CLI demonstration setup")
        print(f"  Help command: {cli.help_}")
        print(f"  Version command: {cli.version_}")
        print(f"  Quiet mode: {cli.quiet_}")
        print(f"  Verbose mode: {cli.verbose_}")
        print(f"  Debug mode: {cli.debug_}")
        print(f"  Output format: {cli.outputFormat_}")
        print(f"  Log level: {cli.logLevel_}")
        print(f"  Config file: {cli.configFile_}")
        print(f"  Workspace dir: {cli.workspaceDir_}")
        print(f"  Command: {cli.command_}")
        
    except Exception as e:
        print(f"✗ Command-line interface demo failed: {e}")
        return False
    
    return True

def demo_configuration_management():
    """Demonstrate configuration management"""
    print("\n=== Configuration Management Demo ===")
    
    try:
        import pynlm
        import tempfile
        import os
        
        # Create a complex configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("world.width", 10)
        config.set("world.height", 10)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Show configuration
        print(f"✓ Configuration management demonstrated")
        print(f"  Configuration keys: {len(brain.getConfig().getKeys())}")
        
        # Export configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_filename = f.name
        
        brain.getConfig().saveToFile(temp_filename)
        
        print(f"  Configuration exported to: {temp_filename}")
        
        # Clean up
        os.unlink(temp_filename)
        
    except Exception as e:
        print(f"✗ Configuration management demo failed: {e}")
        return False
    
    return True

def main():
    """Run all demonstrations"""
    print("NLM Advanced Features Demonstration")
    print("==================================")
    
    # Run all demonstrations
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Advanced Configuration", demo_advanced_configuration),
        ("Working Memory", demo_working_memory),
        ("Performance Profiling", demo_performance_profiling),
        ("Command-Line Interface", demo_command_line_interface),
        ("Configuration Management", demo_configuration_management),
    ]
    
    results = []
    
    for demo_name, demo_func in demos:
        print(f"\n{'='*60}")
        result = demo_func()
        results.append((demo_name, result))
    
    # Summary
    print(f"\n{'='*60}")
    print("DEMONSTRATION SUMMARY")
    print(f"{'='*60}")
    
    passed = 0
    failed = 0
    
    for demo_name, result in results:
        status = "✓ PASSED" if result else "✗ FAILED"
        print(f"{demo_name:30} {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print(f"\n{'='*60}")
    print(f"Total: {passed + failed} demonstrations")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed > 0:
        print(f"\n{failed} demonstrations failed. Please check the errors above.")
        return 1
    else:
        print(f"\nAll demonstrations completed successfully!")
        return 0

if __name__ == "__main__":
    sys.exit(main())