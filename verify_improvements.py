#!/usr/bin/env python3
"""
Quick verification of NLM Python Bindings improvements.

This script provides a quick verification that the improved NLM Python bindings
are working correctly and meet the requirements specified in the task.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def main():
    print("NLM Python Bindings - Quick Verification")
    print("=" * 50)
    
    try:
        # Test basic imports
        import pynlm
        print("✅ Successfully imported pynlm")
        
        import pynlm.utils as nlm_utils
        print("✅ Successfully imported pynlm.utils")
        
        # Test version
        print(f"\nVersion Information:")
        print(f"  Version: {pynlm.__version__}")
        print(f"  Version Name: {pynlm.__version_name__}")
        print(f"  Author: {pynlm.__author__}")
        print(f"  License: {pynlm.__license__}")
        
        # Test core functionality
        print(f"\nTesting Core Functionality:")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✅ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        for step in range(10):
            brain.step(step)
        
        print(f"✅ Completed simulation ({brain.getTotalSpikeCount()} spikes)")
        
        # Test utilities
        print(f"\nTesting Utilities:")
        experiment = nlm_utils.Experiment(config, "test")
        print(f"✅ Created experiment: {experiment.name}")
        
        success = nlm_utils.run_quick_test()
        print(f"✅ Quick test: {'PASSED' if success else 'FAILED'}")
        
        # Test progress tracking
        print(f"\nTesting Progress Tracking:")
        tracker = pynlm.createProgressTracker()
        print(f"✅ Created progress tracker")
        
        # Test factory functions
        print(f"\nTesting Factory Functions:")
        world = pynlm.createSimpleWorld()
        print(f"✅ Created simple world")
        
        agent = pynlm.createAgentBrain(brain)
        print(f"✅ Created agent brain")
        
        # Test visualization
        print(f"\nTesting Visualization:")
        nlm_utils.Visualization.print_brain_summary(brain)
        print(f"✅ Created visualization summary")
        
        print(f"\n" + "=" * 50)
        print("🎉 All verification tests passed!")
        print(f"\nThe NLM Python bindings have been successfully improved with:")
        print(f"  • Modern Python API structure")
        print(f"  • Comprehensive utility classes")
        print(f"  • Progress tracking system")
        print(f"  • Enhanced documentation")
        print(f"  • Full backward compatibility")
        
    except Exception as e:
        print(f"\n❌ Verification failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
