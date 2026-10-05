#!/usr/bin/env python3
"""
Advanced Configuration Demo
Example: Demonstrating advanced configuration management features

This example shows the new configuration capabilities including:
- Configuration templates and presets
- Configuration validation
- Configuration comparison and optimization
- Configuration export/import
- Configuration analysis and insights
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
import json
import time
from nlm_utils import ConfigBuilder, ConfigValidator, AdvancedConfigManager

def main():
    print("=== Advanced Configuration Demo ===")
    
    # Initialize advanced configuration manager
    print("\n1. Initializing Advanced Configuration Manager...")
    config = AdvancedConfigManager()
    print("   ✓ AdvancedConfigManager created")
    
    # Create default configuration
    print("\n2. Creating default configuration...")
    config = ConfigBuilder.create_default_config()
    print(f"   ✓ Created default config with {config.getEntryCount()} entries")
    
    # Display configuration summary
    print("\n3. Configuration Summary:")
    print(f"   Total entries: {config.getEntryCount()}")
    print(f"   Valid entries: {config.getValidEntryCount()}")
    print(f"   Invalid entries: {config.getInvalidEntryCount()}")
    
    # Add schemas and validate
    print("\n4. Adding schemas and validation...")
    schema = ConfigValidator.create_schema(
        key="test_param",
        type="int",
        description="Test parameter",
        min_value=0,
        max_value=100,
        required=False
    )
    config.add_schema(schema)
    print("   ✓ Schema added")
    
    config.set("test_param", 50)
    validation_errors = config.validate()
    print(f"   ✓ Validation completed: {len(validation_errors)} errors")
    
    # Test template system
    print("\n5. Testing template system...")
    config.load_template("advanced")
    print("   ✓ Advanced template loaded")
    
    # Apply presets
    print("\n6. Testing preset system...")
    training_preset = config.apply_preset("training")
    print(f"   ✓ Applied training preset: {training_preset.name}")
    print(f"   Preset performance score: {training_preset.performanceScore}")
    
    simulation_preset = config.apply_preset("simulation")
    print(f"   ✓ Applied simulation preset: {simulation_preset.name}")
    
    # Test configuration optimization
    print("\n7. Testing configuration optimization...")
    optimized_config = config.optimize("general")
    print(f"   ✓ Optimized configuration created")
    
    suggestions = config.suggest_improvements("performance")
    print(f"   ✓ Generated {len(suggestions)} optimization suggestions")
    if suggestions:
        print(f"   Top suggestion: {suggestions[0].key} -> {suggestions[0].suggestedValue} ({suggestions[0].reason})")
    
    # Test configuration export/import
    print("\n8. Testing export/import...")
    json_file = "advanced_config_demo.json"
    config.export_to_json(json_file, include_defaults=True, pretty_print=True)
    print(f"   ✓ Exported to {json_file}")
    
    # Import in a new config
    new_config = ConfigBuilder.create_default_config()
    new_config.import_from_json(json_file)
    print(f"   ✓ Imported configuration: {new_config.getEntryCount()} entries")
    
    # Test configuration comparison
    print("\n9. Testing configuration comparison...")
    similarity = config.calculate_similarity(new_config)
    print(f"   ✓ Similarity between configs: {similarity:.2%}")
    
    changes = config.get_changes()
    print(f"   ✓ Found {len(changes)} changes")
    
    # Test configuration analysis
    print("\n10. Testing configuration analysis...")
    patterns = config.detect_patterns()
    print(f"   ✓ Detected patterns: {len(patterns)}")
    for key, pattern in patterns.items():
        print(f"     - {key}: {pattern}")
    
    performance = config.estimate_performance("training")
    print(f"   ✓ Estimated performance for 'training': {performance:.2%}")
    
    # Create brain with advanced configuration
    print("\n11. Creating brain with advanced configuration...")
    brain = nlm_py.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Brain created successfully")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    print("\n12. Running simulation...")
    start_time = time.time()
    
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            print(f"   Step {step}: Firing rate: {firing_rate:.2f} Hz, Active neurons: {active_neurons}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\n   ✓ Simulation completed!")
    print(f"   Total time: {total_time:.2f} seconds")
    print(f"   Average FPS: {50/total_time:.2f}")
    
    # Configuration comparison with optimized version
    print("\n13. Configuration comparison with optimized version...")
    optimized_config = ConfigBuilder.create_research_config()
    
    # Merge with current config
    config.merge(optimized_config)
    print("   ✓ Merged with optimized research configuration")
    
    # Create documentation
    print("\n14. Generating configuration documentation...")
    documentation = config.generate_documentation()
    print(f"   ✓ Generated documentation ({len(documentation)} characters)")
    
    # Create backup
    print("\n15. Creating configuration backup...")
    backup_path = "config_backups"
    backup_success = config.backup(backup_path)
    print(f"   ✓ Backup created: {backup_success}")
    
    # Clean up test files
    print("\n16. Cleaning up test files...")
    if os.path.exists(json_file):
        os.remove(json_file)
        print(f"   ✓ Removed {json_file}")
    
    # Summary
    print("\n=== Advanced Configuration Demo Completed Successfully ===")
    print("\nKey features demonstrated:")
    print("✓ Advanced configuration management with schemas and validation")
    print("✓ Template-based configuration presets")
    print("✓ Configuration optimization and improvement suggestions")
    print("✓ Configuration export/import with multiple formats")
    print("✓ Configuration comparison and similarity analysis")
    print("✓ Configuration patterns and performance estimation")
    print("✓ Configuration documentation generation")
    print("✓ Configuration backup and restore")
    
    print("\nThe enhanced configuration system now provides:")
    print("• Comprehensive configuration templates for common use cases")
    print("• Robust validation rules and error reporting")
    print("• Advanced comparison and merging utilities")
    print("• Automatic configuration optimization tools")
    print("• Flexible export/import capabilities")
    print("• Configuration analysis and insights")

if __name__ == "__main__":
    main()
