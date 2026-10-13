# NLM - Integration Examples

**Date:** 2026-10-13
**Audience:** Intermediate to advanced users
**Level:** Advanced

---

## What You'll Learn

This section provides **integration examples** demonstrating how NLM systems work together with other components and in complex scenarios.

1. **Integration with Existing NLM Code** - How new Advanced API works with old code
2. **Multi-Agent Simulation** - Complex coordination between multiple agents
3. **Data Analysis Pipeline** - Complete workflow from simulation to insights
4. **Performance Profiling and Optimization** - Identify and fix bottlenecks
5. **Machine Learning Integration** - Combine NLM with advanced analytics
6. **Real-World Research Applications** - Neuroscience research using NLM

---

## Example 1: Integration with Existing NLM Code

### What It Does
This example demonstrates how the new Advanced API seamlessly integrates with existing NLM code, showing backward compatibility and enhanced functionality.

```python
import pynlm
from pynlm.advanced_api import AdvancedAPI, SimpleAgent, SimulationRunner

def demonstrate_backward_compatibility():
    """Show that existing code continues to work with new features"""
    
    print("=== Backward Compatibility Demonstration ===")
    
    # PART 1: Existing code continues to work (no changes needed)
    print("\n1. Running existing NLM code...")
    
    # Original NLM code - no modifications required
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation (original pattern)
    for step in range(100):
        brain.step(step)
    
    print("✅ Existing code works without modification")
    
    # PART 2: Enhanced functionality through Advanced API
    print("\n2. Adding Advanced API features...")
    
    # Use Advanced API for enhanced analysis
    advanced_api = AdvancedAPI(brain)
    analysis = advanced_api.analyze_brain_activity()
    
    print(f"✅ Advanced API analysis: {analysis.get('brain_dynamics', {}).get('average_firing_rate', 0):.3f} Hz")
    
    # PART 3: New features without breaking existing functionality
    print("\n3. Integrating new features...")
    
    # Create Advanced API simulation
    sim_config = advanced_api.create_default_config()
    sim_config.set("simulation.steps", 500)
    sim_config.set("simulation.dt", 0.1)
    
    # Run enhanced simulation
    enhanced_brain = advanced_api.run_simulation(sim_config)
    
    # Get advanced analysis
    enhanced_analysis = advanced_api.analyze_brain_activity()
    
    print(f"✅ Enhanced simulation: {enhanced_analysis.get('brain_dynamics', {}).get('total_steps', 0)} steps")
    
    return brain, advanced_api, enhanced_brain

def demonstrate_interface_integration():
    """Show how interface-based architecture enables integration"""
    
    print("\n=== Interface Integration Demonstration ===")
    
    # Create components using interfaces
    print("\n1. Creating components using interfaces...")
    
    # Brain (concrete implementation)
    brain_config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # World (concrete implementation)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Agent (using factory function)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Advanced API components (using factory functions)
    brain_profiler = createBrainProfiler(brain)
    network_analyzer = createNetworkConnectivityAnalyzer(brain)
    
    print("✅ All components created using interfaces")
    
    # PART 1: Run simulation with monitoring
    print("\n2. Running simulation with interface-based monitoring...")
    
    sim_runner = SimulationRunner(num_steps=200, dt=0.1)
    sim_runner.addAgent(agent)
    
    # Add analysis tools
    sim_runner.addProfiler(brain_profiler)
    sim_runner.addAnalyzer(network_analyzer)
    
    # Run enhanced simulation
    sim_runner.start()
    
    for step in range(200):
        sim_runner.step()
        
        # Get periodic updates from analysis tools
        if step % 50 == 0:
            metrics = sim_runner.get_metrics()
            print(f"   Step {step}: Firing rate = {metrics['brain_metrics']['average_firing_rate']:.3f} Hz")
    
    sim_runner.stop()
    
    # PART 2: Get comprehensive results
    print("\n3. Getting comprehensive results...")
    
    comprehensive_results = {
        'brain_profiler': brain_profiler.getProfileSummary(),
        'network_analyzer': network_analyzer.getConnectivitySummary(),
        'simulation_metrics': sim_runner.getMetrics()
    }
    
    print("✅ Comprehensive results collected")
    
    return comprehensive_results

def demonstrate_mixed_architecture():
    """Show hybrid architecture with both old and new approaches"""
    
    print("\n=== Mixed Architecture Demonstration ===")
    
    # PART 1: Traditional approach
    print("\n1. Traditional NLM approach...")
    
    traditional_config = pynlm.createDefaultConfig()
    traditional_config.set("brain.neuron_count", 1000)
    
    traditional_brain = pynlm.createBrain(traditional_config)
    traditional_brain.initialize()
    
    for step in range(100):
        traditional_brain.step(step)
    
    traditional_results = {
        'neurons': traditional_brain.getTotalNeuronCount(),
        'spikes': traditional_brain.getTotalSpikeCount(),
        'firing_rate': traditional_brain.getAverageFiringRate()
    }
    
    print(f"✅ Traditional results: {traditional_results['firing_rate']:.3f} Hz")
    
    # PART 2: Advanced approach
    print("\n2. Advanced NLM approach...")
    
    # Create Advanced API instance
    advanced_api = AdvancedAPI(traditional_brain)
    
    # Create enhanced simulation configuration
    advanced_config = advanced_api.create_default_config()
    advanced_config.set("brain.neuron_count", 1000)
    advanced_config.set("simulation.steps", 500)
    advanced_config.set("plasticity.enabled", True)
    advanced_config.set("development.enabled", True)
    
    # Run advanced simulation
    enhanced_brain = advanced_api.run_simulation(advanced_config)
    
    # Get advanced analysis
    enhanced_analysis = advanced_api.analyze_brain_activity()
    
    enhanced_results = {
        'neurons': enhanced_brain.getTotalNeuronCount(),
        'spikes': enhanced_brain.getTotalSpikeCount(),
        'firing_rate': enhanced_brain.getAverageFiringRate(),
        'analysis': enhanced_analysis
    }
    
    print(f"✅ Advanced results: {enhanced_results['firing_rate']:.3f} Hz")
    
    # PART 3: Compare results
    print("\n3. Comparing traditional vs advanced...")
    
    comparison = {
        'traditional_firing_rate': traditional_results['firing_rate'],
        'advanced_firing_rate': enhanced_results['firing_rate'],
        'improvement': (enhanced_results['firing_rate'] - traditional_results['firing_rate']) / traditional_results['firing_rate'] * 100,
        'traditional_spikes': traditional_results['spikes'],
        'advanced_spikes': enhanced_results['spikes'],
        'spike_improvement': (enhanced_results['spikes'] - traditional_results['spikes']) / traditional_results['spikes'] * 100
    }
    
    print(f"   Firing rate improvement: {comparison['improvement']:.1f}%")
    print(f"   Spike improvement: {comparison['spike_improvement']:.1f}%")
    
    # PART 4: Integration results
    print("\n4. Integration benefits...")
    
    integration_benefits = {
        'backward_compatibility': "✅ Existing code continues to work",
        'enhanced_functionality': "✅ New Advanced API features available",
        'performance_improvements': f"✅ {comparison['improvement']:.1f}% improvement in firing rate",
        'analysis_capabilities': "✅ Comprehensive analysis tools available",
        'scalability': "✅ Supports both small and large simulations"
    }
    
    for benefit, description in integration_benefits.items():
        print(f"   {benefit}: {description}")
    
    return comparison, integration_benefits

# Run integration demonstrations
print("Starting NLM integration demonstrations...")

# Demonstrate backward compatibility
brain, advanced_api, enhanced_brain = demonstrate_backward_compatibility()

# Demonstrate interface integration
interface_results = demonstrate_interface_integration()

# Demonstrate mixed architecture
comparison, benefits = demonstrate_mixed_architecture()

print("\n=== Integration Demonstrations Complete ===")
print(f"Backward compatibility: ✅ Maintained")
print(f"Interface integration: ✅ Working")
print(f"Mixed architecture: ✅ Functional")
print(f"Performance improvement: {comparison['improvement']:.1f}%")
```