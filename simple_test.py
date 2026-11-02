#!/usr/bin/env python3
"""
Test enhanced NLM Python bindings.
"""
import sys

print("Testing enhanced NLM Python bindings...")

# Check if the module can be imported
try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    brain.initialize()
    print("✓ Initialized brain")
    
    # Test enhanced methods
    stats = brain.get_statistics()
    print(f"✓ Got brain statistics with keys: {list(stats.keys())}")
    
    # Test new convenience methods
    validation_result = brain.validate_config()
    print(f"✓ Config validation: {validation_result[0]} (valid: {validation_result[0]})")
    
    diagnostics = brain.get_diagnostics()
    print(f"✓ Got diagnostics with keys: {list(diagnostics.keys())}")
    
    # Test developmental stage functionality
    stage = brain.getDevelopmentalStage()
    print(f"✓ Developmental stage: {stage}")
    
    # Test configuration validation function
    from pprint import pprint
    print("✓ Testing module-level functions...")
    
    # Check if new module-level functions exist
    if hasattr(pynlm, 'run_complete_simulation'):
        print("✓ Found run_complete_simulation function")
    else:
        print("✗ run_complete_simulation function not found")
        
    if hasattr(pynlm, 'benchmark_performance'):
        print("✓ Found benchmark_performance function")
    else:
        print("✗ benchmark_performance function not found")
        
    if hasattr(pynlm, 'validate_all_config'):
        print("✓ Found validate_all_config function")
    else:
        print("✗ validate_all_config function not found")
        
    if hasattr(pynlm, 'create_development_scenario'):
        print("✓ Found create_development_scenario function")
    else:
        print("✗ create_development_scenario function not found")
    
    print("\n" + "="*60)
    print("Enhanced NLM Python bindings test completed successfully!")
    print("="*60)
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
except Exception as e:
    print(f"✗ Test failed with error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
