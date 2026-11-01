#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings
Shows all the new features and improvements added to pynlm.
"""

import pynlm
import time

def print_header(title):
    """Print formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def demo_basic_usage():
    """Demonstrate basic usage of NLM with enhanced bindings."""
    print_header("1. BASIC USAGE - Enhanced NLM")
    
    # Create default configuration
    print("Creating default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Set some configuration parameters
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    print("✓ Configuration set with enhanced parameters")
    
    # Create brain
    print("Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized")
    
    # Run simulation
    print("Running simulation...")
    for step in range(100):
        brain.step(step)
    print("✓ Simulation completed")
    
    # Get statistics
    print("\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

def demo_advanced_configuration():
    """Demonstrate advanced configuration features."""
    print_header("2. ADVANCED CONFIGURATION - New Python API")
    
    # Example 1: Configuration from dictionary
    print("Example 1: Configuration from dictionary")
    advanced_params = {
        "brain.neuron_count": 3000,
        "brain.region_count": 4,
        "brain.population_distribution": "balanced",
        "plasticity.stdp.enable": True,
        "plasticity.stdp.ltp_weight": 0.025,
        "plasticity.stdp.ltd_weight": 0.02,
        "plasticity.stdp.tau": 25.0,
        "plasticity.hebbian.enable": True,
        "plasticity.hebbian.learning_rate": 0.002,
        "neuromod.dopamine.enable": True,
        "neuromod.dopamine.baseline": 0.15,
        "neuromod.dopamine.scale": 1.2,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.threshold": 0.6,
        "memory.working.enable": True,
        "memory.working.capacity": 500,
        "memory.episodic.enable": True,
        "memory.episodic.capacity": 200,
        "performance.simd.enable": True,
        "performance.parallel.enable": True,
        "debug.log_level": "INFO",
        "debug.log_to_file": True,
        "debug.log_filename": "nlm_advanced.log"
    }
    
    # Create configuration from dictionary
    config = pynlm.createConfigFromDict(advanced_params)
    print("✓ Advanced configuration created from dictionary")
    
    # Validate configuration
    validation = pynlm.validateBrainConfig(config)
    print(f"✓ Configuration validated: {validation[:100]}...")
    
    # Configure brain from dictionary
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    pynlm.configureBrainFromDict(brain.getConfig(), advanced_params)
    print("✓ Brain configured with advanced parameters")

def demo_configuration_templates():
    """Demonstrate configuration templates."""
    print_header("3. CONFIGURATION TEMPLATES - Ready-to-use configurations")
    
    # Template 1: Fast Training Configuration
    print("Template 1: Fast Training Configuration")
    fast_training_config = {
        "brain.neuron_count": 1000,
        "plasticity.stdp.ltp_weight": 0.01,
        "plasticity.stdp.ltd_weight": 0.005,
        "performance.simd.enable": True,
        "performance.parallel.enable": True,
        "debug.log_level": "WARN"
    }
    
    fast_config = pynlm.createConfigFromDict(fast_training_config)
    print(f"✓ Fast training config: {fast_config.getOr('brain.neuron_count', 0)} neurons")
    
    # Template 2: Precision Configuration
    print("\nTemplate 2: Precision Configuration")
    precision_config = {
        "brain.neuron_count": 5000,
        "brain.initial_weight_std": 0.05,
        "plasticity.stdp.ltp_weight": 0.015,
        "plasticity.stdp.ltd_weight": 0.01,
        "plasticity.structural.enable": True,
        "memory.working.capacity": 500,
        "memory.episodic.capacity": 200,
        "debug.log_level": "DEBUG"
    }
    
    precision_config = pynlm.createConfigFromDict(precision_config)
    print(f"✓ Precision config: {precision_config.getOr('brain.neuron_count', 0)} neurons")
    
    # Template 3: Curiosity-Driven Exploration
    print("\nTemplate 3: Curiosity-Driven Exploration")
    exploration_config = {
        "brain.neuron_count": 2000,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.threshold": 0.7,
        "neuromod.novelty.enable": True,
        "neuromod.novelty.sensitivity": 0.2,
        "development.enable": True,
        "development.initial_stage": "CriticalPeriod"
    }
    
    exploration_config = pynlm.createConfigFromDict(exploration_config)
    print(f"✓ Exploration config: curiosity enabled = {exploration_config.getOr('neuromod.curiosity.enable', False)}")
    
    # Template 4: Memory-Intensive Configuration
    print("\nTemplate 4: Memory-Intensive Configuration")
    memory_config = {
        "brain.neuron_count": 3000,
        "memory.working.enable": True,
        "memory.working.capacity": 1000,
        "memory.episodic.enable": True,
        "memory.episodic.capacity": 500,
        "memory.associative.enable": True,
        "performance.checkpointing.enable": True,
        "performance.checkpoint_interval": 500
    }
    
    memory_config = pynlm.createConfigFromDict(memory_config)
    print(f"✓ Memory config: working capacity = {memory_config.getOr('memory.working.capacity', 0)}")

def demo_batch_operations():
    """Demonstrate batch operations for better performance."""
    print_header("4. BATCH OPERATIONS - Performance enhancements")
    
    # Create brain for batch operations demo
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Batch inject current (more efficient than individual calls)
    print("Batch injecting current to excitatory neurons...")
    pynlm.batchInjectCurrent(brain, pynlm.NeuronType.Excitatory, 50.0, 10)
    print("✓ Batch inject completed")
    
    # Batch get neuron statistics
    print("Getting batch statistics for excitatory neurons...")
    stats = pynlm.batchGetNeuronStats(brain, pynlm.NeuronType.Excitatory)
    if stats:
        print(f"✓ Batch stats collected:")
        print(f"  - Count: {stats[0]}")
        print(f"  - Avg potential: {stats[1]:.3f}")
        print(f"  - Avg firing rate: {stats[2]:.2f} Hz")
        print(f"  - Avg threshold: {stats[3]:.3f}")

def demo_brain_inspection():
    """Demonstrate enhanced brain inspection capabilities."""
    print_header("5. BRAIN INSPECTION - Detailed analysis tools")
    
    # Create and run a simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Basic inspection
    print("Basic brain inspection:")
    inspection = pynlm.inspectBrain(brain, False)
    print("✓ Basic inspection completed")
    
    # Advanced inspection (verbose)
    print("\nAdvanced (verbose) brain inspection:")
    advanced_inspection = pynlm.inspectBrain(brain, True)
    print("✓ Advanced inspection completed")
    
    # Configuration analysis
    print(f"\nConfiguration analysis:")
    config = brain.getConfig()
    print(f"  Total parameters: {len(config.getKeys())}")
    
    categories = {}
    for key in config.getKeys():
        category = key.split('.')[0]
        categories[category] = categories.get(category, 0) + 1
    
    print(f"  Parameter categories: {list(categories.keys())}")
    print(f"  Performance optimization hints:")
    optimization_hints = pynlm.optimizeBrainConfig(config)
    for hint in optimization_hints.split('\n'):
        if hint.strip():
            print(f"    - {hint.strip()}")

def demo_learning_rate_control():
    """Demonstrate enhanced learning rate control."""
    print_header("6. LEARNING RATE CONTROL - Fine-tune plasticity")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Show initial learning rates
    print("Initial learning rates:")
    initial_stdp_ltp = config.getOr("plasticity.stdp.ltp_weight", 0.02)
    initial_stdp_ltd = config.getOr("plasticity.stdp.ltd_weight", 0.015)
    initial_hebbian = config.getOr("plasticity.hebbian.learning_rate", 0.001)
    
    print(f"  STDP LTP weight: {initial_stdp_ltp}")
    print(f"  STDP LTD weight: {initial_stdp_ltd}")
    print(f"  Hebbian learning rate: {initial_hebbian}")
    
    # Adjust learning rates
    print("\nAdjusting learning rates...")
    result = pynlm.adjustLearningRates(brain, 1.5, 0.9)
    print(f"✓ Learning rates adjusted: {result}")
    
    # Show adjusted learning rates
    adjusted_config = brain.getConfig()
    adjusted_stdp_ltp = adjusted_config.getOr("plasticity.stdp.ltp_weight", initial_stdp_ltp)
    adjusted_stdp_ltd = adjusted_config.getOr("plasticity.stdp.ltd_weight", initial_stdp_ltd)
    adjusted_hebbian = adjusted_config.getOr("plasticity.hebbian.learning_rate", initial_hebbian)
    
    print("\nAdjusted learning rates:")
    print(f"  STDP LTP weight: {adjusted_stdp_ltp} (factor: {adjusted_stdp_ltp/initial_stdp_ltp:.2f})")
    print(f"  STDP LTD weight: {adjusted_stdp_ltd} (factor: {adjusted_stdp_ltd/initial_stdp_ltd:.2f})")
    print(f"  Hebbian learning rate: {adjusted_hebbian} (factor: {adjusted_hebbian/initial_hebbian:.2f})")

def demo_performance_profiling():
    """Demonstrate performance profiling capabilities."""
    print_header("7. PERFORMANCE PROFILING - Monitor brain performance")
    
    # Create larger brain for profiling
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Profiling brain performance over 100 steps...")
    start_time = time.time()
    
    profile_result = pynlm.profileBrain(brain, 100)
    
    end_time = time.time()
    print(f"✓ Profiling completed in {end_time - start_time:.2f} seconds")
    
    print(f"\nProfile results:")
    # Extract key metrics from profile
    if "Firing rate change:" in profile_result:
        import re
        rate_match = re.search(r'Firing rate change: ([\d.-]+)%', profile_result)
        if rate_match:
            print(f"  - Firing rate change: {rate_match.group(1)}%")
    
    if "Average firing rate:" in profile_result:
        rate_match = re.search(r'Average firing rate: ([\d.-]+)', profile_result)
        if rate_match:
            print(f"  - Average firing rate: {rate_match.group(1)} Hz")

def demo_error_handling():
    """Demonstrate improved error handling."""
    print_header("8. ERROR HANDLING - Robust configuration validation")
    
    # Test 1: Invalid configuration
    print("Test 1: Invalid configuration detection")
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("brain.neuron_count", -10)  # Invalid: too low
    
    validation = pynlm.validateBrainConfig(invalid_config)
    if "must be between" in validation:
        print("✓ Correctly detected invalid neuron count")
    
    # Test 2: Missing configuration
    print("\nTest 2: Missing configuration detection")
    simple_config = pynlm.createDefaultConfig()
    if not simple_config.has("nonexistent.parameter"):
        print("✓ Correctly detected missing parameter")
    
    # Test 3: Valid configuration
    print("\nTest 3: Valid configuration validation")
    valid_config = pynlm.createDefaultConfig()
    valid_config.set("brain.neuron_count", 500)
    valid_config.set("plasticity.stdp.ltp_weight", 0.025)
    
    validation = pynlm.validateBrainConfig(valid_config)
    if "valid" in validation.lower():
        print("✓ Valid configuration accepted")
    
    # Test 4: Configuration optimization hints
    print("\nTest 4: Configuration optimization hints")
    large_config = pynlm.createDefaultConfig()
    large_config.set("brain.neuron_count", 10000)  # Large config
    
    optimization_hints = pynlm.optimizeBrainConfig(large_config)
    print("✓ Optimization hints generated")
    for hint in optimization_hints.split('\n'):
        if hint.strip() and len(hint.strip()) > 20:
            print(f"  - {hint.strip()}")

def demo_simple_simulation():
    """Demonstrate simple simulation creation."""
    print_header("9. SIMPLE SIMULATION - Quick setup tools")
    
    print("Creating simple simulation...")
    simple_brain = pynlm.createSimpleSimulation(100, 20)
    print("✓ Simple simulation created")
    
    print(f"Simulation stats:")
    print(f"  - Neurons: {simple_brain.getTotalNeuronCount()}")
    print(f"  - Steps completed: 20")
    
    # Reset and reinitialize
    print("\nResetting brain simulation...")
    result = pynlm.resetBrainSimulation(simple_brain)
    print(f"✓ Brain reset: {result}")

def demo_configuration_manager():
    """Demonstrate configuration management features."""
    print_header("10. CONFIGURATION MANAGEMENT - Advanced tools")
    
    print("Configuration management features:")
    print("  ✓ Configuration from dictionary")
    print("  ✓ Configuration validation")
    print("  ✓ Configuration optimization")
    print("  ✓ Configuration analysis")
    print("  ✓ Configuration templates")
    print("  ✓ Batch operations")
    print("  ✓ Learning rate control")
    print("  ✓ Performance profiling")
    print("  ✓ Error handling")
    print("  ✓ Simple simulation tools")
    
    print("\nConfiguration management benefits:")
    print("  ✓ Faster development with ready-to-use configurations")
    print("  ✓ Better performance with batch operations")
    print("  ✓ Enhanced debugging with inspection tools")
    print("  ✓ Robust error handling with validation")
    print("  ✓ Optimized performance with configuration hints")
    print("  ✓ Simplified setup with simple simulation tools")

def main():
    """Main demonstration function."""
    print("🚀 ENHANCED NLM PYTHON BINDINGS DEMONSTRATION")
    print("=" * 60)
    print("This demonstration showcases all the new features and improvements")
    print("added to the NLM Python bindings for better usability and performance.")
    print("=" * 60)
    
    # Run all demonstrations
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Advanced Configuration", demo_advanced_configuration),
        ("Configuration Templates", demo_configuration_templates),
        ("Batch Operations", demo_batch_operations),
        ("Brain Inspection", demo_brain_inspection),
        ("Learning Rate Control", demo_learning_rate_control),
        ("Performance Profiling", demo_performance_profiling),
        ("Error Handling", demo_error_handling),
        ("Simple Simulation", demo_simple_simulation),
        ("Configuration Management", demo_configuration_manager),
    ]
    
    for demo_name, demo_func in demos:
        try:
            demo_func()
        except Exception as e:
            print(f"❌ Error in {demo_name}: {e}")
            import traceback
            traceback.print_exc()
    
    print_header("DEMONSTRATION COMPLETE")
    print("\nThe enhanced NLM Python bindings now provide:")
    print("  ✓ Advanced configuration options with dictionary-based setup")
    print("  ✓ Powerful batch operations for better performance")
    print("  ✓ Comprehensive brain inspection and debugging tools")
    print("  ✓ Flexible learning rate control for plasticity tuning")
    print("  ✓ Performance profiling for optimization")
    print("  ✓ Robust error handling and validation")
    print("  ✓ Ready-to-use configuration templates")
    print("  ✓ Simple simulation setup tools")
    print("  ✓ Configuration management and analysis")
    print("\nThese improvements make NLM more accessible to users while")
    print("providing advanced capabilities for experienced users.")

if __name__ == "__main__":
    main()
