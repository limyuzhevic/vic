"""
Configuration Presets Example
=============================

This example demonstrates how to use NLM's configuration system with built-in
presets and custom configurations. It shows how to create reproducible setups
and optimize configurations for different use cases.

## What This Example Shows

1. Accessing built-in configuration presets
2. Creating configurations with presets
3. Creating custom configurations with overrides
4. Validating configurations
5. Best practices for configuration management

## When to Use This Example

- You need reproducible experimental setups
- You want to optimize configurations for different use cases
- You're running parameter sweeps or hyperparameter optimization
- You need to share configuration with other researchers
- You're setting up multiple experiments with similar but different configurations
"""

import pynlm
from advanced_api import NLMConfigurator

def main():
    """Run the configuration presets example"""
    print("=== Configuration Presets Example ===")
    print()
    
    # 1. Explore available presets
    print("Step 1: Exploring available configuration presets...")
    
    presets = NLMConfigurator.get_presets()
    print(f"Available presets: {list(presets.keys())}")
    print()
    
    for preset_name, preset_config in presets.items():
        print(f"Preset: {preset_name}")
        print(f"  Neurons: {preset_config.get('brain.neuron_count', 'default')}")
        print(f"  STDP enabled: {preset_config.get('plasticity.stdp.enable', 'default')}")
        print(f"  Curiosity enabled: {preset_config.get('neuromod.curiosity.enable', 'default')}")
        print(f"  Hebbian enabled: {preset_config.get('plasticity.hebbian.enable', 'default')}")
        print()
    
    # 2. Create brains with different presets
    print("Step 2: Creating brains with different presets...")
    
    # Minimal configuration
    print("Creating minimal configuration...")
    minimal_config = NLMConfigurator.create_config('minimal')
    minimal_brain = pynlm.createBrain(minimal_config)
    minimal_brain.initialize()
    
    print(f"  Neurons: {minimal_brain.getTotalNeuronCount()}")
    print(f"  Development stage: {minimal_brain.getDevelopmentalStage()}")
    
    # Research configuration
    print("Creating research configuration...")
    research_config = NLMConfigurator.create_config('research')
    research_brain = pynlm.createBrain(research_config)
    research_brain.initialize()
    
    print(f"  Neurons: {research_brain.getTotalNeuronCount()}")
    
    # Production configuration
    print("Creating production configuration...")
    production_config = NLMConfigurator.create_config('production')
    production_brain = pynlm.createBrain(production_config)
    production_brain.initialize()
    
    print(f"  Neurons: {production_brain.getTotalNeuronCount()}")
    
    # 3. Create custom configuration
    print("Step 3: Creating custom configuration with overrides...")
    
    custom_config = NLMConfigurator.create_config(
        brain.neuron_count=750,
        plasticity.stdp.enable=True,
        plasticity.stdp.learning_rate=0.02,
        neuromod.curiosity.enable=True,
        neuromod.novelty.enable=False,
        brain.synapse_density=0.08
    )
    
    print("Custom configuration settings:")
    for key, value in custom_config.getKeys():
        print(f"  {key}: {value}")
    
    custom_brain = pynlm.createBrain(custom_config)
    custom_brain.initialize()
    print(f"  Custom brain neurons: {custom_brain.getTotalNeuronCount()}")
    
    # 4. Configuration validation
    print("Step 4: Configuration validation...")
    
    from advanced_api import validate_brain_state
    
    validation_issues = []
    
    for brain_name, brain in [("minimal", minimal_brain), 
                             ("research", research_brain),
                             ("production", production_brain),
                             ("custom", custom_brain)]:
        issues = validate_brain_state(brain)
        if issues:
            print(f"  {brain_name} brain validation issues:")
            for issue in issues:
                print(f"    - {issue}")
        else:
            print(f"  {brain_name} brain validation: PASSED")
    
    # 5. Configuration comparison
    print("Step 5: Configuration comparison...")
    
    print("Configuration comparison (key metrics):")
    print(f"{'Config':<10} {'Neurons':<8} {'Firing Rate':<12} {'Development':<12}")
    print(f"{'-------':<10} {'-------':<8} {'-----------':<12} {'------------':<12}")
    print(f"{'minimal':<10} {minimal_brain.getTotalNeuronCount():<8} "
          f"{minimal_brain.getAverageFiringRate():<12.4f} "
          f"{minimal_brain.getDevelopmentalStage():<12}")
    print(f"{'research':<10} {research_brain.getTotalNeuronCount():<8} "
          f"{research_brain.getAverageFiringRate():<12.4f} "
          f"{research_brain.getDevelopmentalStage():<12}")
    print(f"{'production':<10} {production_brain.getTotalNeuronCount():<8} "
          f"{production_brain.getAverageFiringRate():<12.4f} "
          f"{production_brain.getDevelopmentalStage():<12}")
    print(f"{'custom':<10} {custom_brain.getTotalNeuronCount():<8} "
          f"{custom_brain.getAverageFiringRate():<12.4f} "
          f"{custom_brain.getDevelopmentalStage():<12}")
    
    # 6. Configuration best practices
    print("Step 6: Configuration best practices...")
    
    print("Best practices for NLM configuration:")
    print("  1. Use presets for standard configurations")
    print("  2. Customize presets with specific overrides")
    print("  3. Validate configurations before use")
    print("  4. Document configuration settings")
    print("  5. Use consistent naming for parameters")
    print("  6. Test configurations in isolation first")
    print()
    print("Recommended preset usage:")
    print("  - 'minimal': Quick testing and prototyping")
    print("  - 'research': Scientific experiments with learning")
    print("  - 'production': Stable, reliable simulations")
    
    print()
    print("=== Configuration Presets Example Complete ===")

if __name__ == "__main__":
    main()
