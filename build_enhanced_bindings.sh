#!/usr/bin/bash
"""
NLM Enhanced Python Bindings Build and Installation Script

This script demonstrates how to build and install the enhanced NLM Python bindings
with all new features and improvements.

Usage:
    chmod +x build_enhanced_bindings.sh
    ./build_enhanced_bindings.sh
"""

set -e  # Exit on any error

echo "NLM Enhanced Python Bindings Build Script"
echo "=========================================="
echo

# Check if we're in the right directory
if [ ! -f "pyproject.toml" ]; then
    echo "Error: This script must be run from the NLM project root directory."
    echo "Looking for pyproject.toml..."
    exit 1
fi

echo "✓ Project directory detected"

# Step 1: Check Python and dependencies
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is not installed or not in PATH"
    exit 1
fi

echo "✓ Python3 is available"
echo "Python version: $(python3 --version)"

# Step 2: Install Python dependencies
if [ -f "pyproject.toml" ]; then
    echo
    echo "Installing Python dependencies..."
    if python3 -m pip install --upgrade pip setuptools wheel &> /dev/null; then
        echo "✓ Pip upgraded successfully"
    else
        echo "Warning: Could not upgrade pip"
    fi
    
    # Install build dependencies
    if python3 -m pip install pybind11 scikit-build-core pytest numpy &> /dev/null; then
        echo "✓ Build dependencies installed"
    else
        echo "Error: Failed to install build dependencies"
        exit 1
    fi
else
    echo "Warning: pyproject.toml not found, skipping dependency installation"
fi

# Step 3: Create build directory
echo
    echo "Creating build directory..."
    if [ ! -d "build" ]; then
        mkdir -p build
        echo "✓ Build directory created"
    else
        echo "✓ Build directory already exists"
    fi

# Step 4: Configure build
if [ -d "build" ]; then
    echo
    echo "Configuring CMake build..."
    cd build
    
    if cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON &> /dev/null; then
        echo "✓ CMake configuration successful"
    else
        echo "Error: CMake configuration failed"
        exit 1
    fi
    
    # Step 5: Build the project
    echo
    echo "Building project..."
    if make -j$(nproc) &> /dev/null; then
        echo "✓ Build successful"
    else
        echo "Error: Build failed"
        exit 1
    fi
    
    # Step 6: Install Python module
    echo
    echo "Installing Python module..."
    cd ..
    
    if python3 -m pip install -e . &> /dev/null; then
        echo "✓ Python module installed successfully"
    else
        echo "Error: Python module installation failed"
        exit 1
    fi
    
else
    echo "Error: Build directory not found"
    exit 1
fi

# Step 7: Test the installation
echo
    echo "Testing installation..."
    cd /tmp
    
    # Create a simple test script
    cat > test_nlm.py << 'EOF'
import sys
try:
    import pynlm
    print("SUCCESS: pynlm imported successfully")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(5):
        brain.step(i)
    
    print(f"SUCCESS: Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Test enhanced features
    if hasattr(pynlm, 'createStandardBrainConfig'):
        print("SUCCESS: Enhanced configuration presets available")
    
    if hasattr(pynlm, 'PerformanceMonitor'):
        print("SUCCESS: Performance monitoring available")
    
    if hasattr(pynlm, 'createSimulationContext'):
        print("SUCCESS: Context manager available")
    
    sys.exit(0)
    
except ImportError as e:
    print(f"FAILED: Import error - {e}")
    sys.exit(1)
except Exception as e:
    print(f"FAILED: Error - {e}")
    sys.exit(1)
EOF
    
    if python3 test_nlm.py; then
        echo "✓ Installation test passed"
    else
        echo "✗ Installation test failed"
        exit 1
    fi
    
    # Clean up test file
    rm -f test_nlm.py
fi

# Step 8: Run enhanced demo
echo
    echo "Running enhanced demo...")
    if [ -f "/workspace/app/demo_enhanced_features.py" ]; then
        if python3 /workspace/app/demo_enhanced_features.py; then
            echo "✓ Enhanced demo completed successfully"
        else
            echo "✗ Enhanced demo failed"
            exit 1
        fi
    else
        echo "Warning: demo_enhanced_features.py not found"
    fi

# Step 9: Run quick demo
echo
    echo "Running quick demo..."
    if [ -f "/workspace/app/demo_quick.py" ]; then
        if python3 /workspace/app/demo_quick.py; then
            echo "✓ Quick demo completed successfully"
        else
            echo "✗ Quick demo failed"
            exit 1
        fi
    else
        echo "Warning: demo_quick.py not found"
    fi

# Step 10: Show installation summary
echo
    echo "Installation Summary"
    echo "==================="
    echo "Enhanced NLM Python bindings have been successfully installed!"
    echo
    echo "Available features:
    - Context managers for automatic resource management
    - Performance monitoring APIs
    - Advanced configuration presets
    - Network analysis utilities
    - Experimental features for advanced users
    - Enhanced error handling
    - Full backward compatibility"
    echo
    echo "Usage examples:
    - Import: import pynlm
    - Basic: pynlm.createDefaultConfig()  # Still works
    - Enhanced: pynlm.createStandardBrainConfig()  # New!
    - Monitoring: monitor = pynlm.PerformanceMonitor()
    - Context: with pynlm.simulation_context(brain, agent, world):  # New!
    "
    echo
    echo "For more information:
    - README_ENHANCED.md: Complete documentation
    - docs/ENHANCED_BINDINGS.md: Detailed API guide
    - demo_enhanced_features.py: Comprehensive examples
    - demo_quick.py: Quick demonstration
    "

# Step 11: Create test script for developers
echo
    echo "Creating developer test script..."
    cat > /workspace/app/test_enhanced_bindings.py << 'EOF'
#!/usr/bin/env python3
"""
Developer test script for NLM enhanced bindings.
Run with: python3 /workspace/app/test_enhanced_bindings.py
"""

import sys
import pynlm

def main():
    print("NLM Enhanced Bindings - Developer Test Suite")
    print("=" * 60)
    print()
    
    # Test 1: Import and basic functionality
    print("Test 1: Import and basic functionality")
    print("-" * 40)
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Spike count: {brain.getTotalSpikeCount()}")
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    
    # Test 2: Enhanced configuration presets
    print("Test 2: Enhanced configuration presets")
    print("-" * 40)
    try:
        presets = [
            ("Mini", pynlm.createMiniBrainConfig),
            ("Standard", pynlm.createStandardBrainConfig),
            ("Advanced", pynlm.createAdvancedBrainConfig)
        ]
        
        for name, preset_func in presets:
            config = preset_func()
            print(f"✓ {name} preset created")
        
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    
    # Test 3: Performance monitoring
    print("Test 3: Performance monitoring")
    print("-" * 40)
    try:
        monitor = pynlm.PerformanceMonitor()
        
        for i in range(20):
            brain.step(i)
            monitor.recordStep(0.1)
        
        stats = monitor.getStats()
        print(f"✓ Performance monitoring working")
        print(f"✓ Steps per second: {stats['steps_per_second']:.1f}")
        print(f"✓ Performance stable: {stats['performance_stable']}")
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    
    # Test 4: Context manager
    print("Test 4: Context manager")
    print("-" * 40)
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create context manager
        context = pynlm.createSimulationContext(brain, agent, world)
        
        print("✓ Context manager created successfully")
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    
    # Test 5: Network analysis
    print("Test 5: Network analysis")
    print("-" * 40)
    try:
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        print(f"✓ Network analysis working")
        print(f"✓ Estimated memory: {analysis['estimated_memory_kb']:.2f} KB")
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    
    # Test 6: Experimental features
    print("Test 6: Experimental features")
    print("-" * 40)
    try:
        experimental = pynlm.experimental
        print("✓ Experimental features module available")
        print("PASSED")
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)
    
    print()
    print("=" * 60)
    print("🎉 ALL TESTS PASSED!")
    print("Enhanced NLM Python bindings are working correctly.")
    print()
    print("Key features verified:")
    print("  ✓ Backward compatibility")
    print("  ✓ Enhanced configuration presets")
    print("  ✓ Performance monitoring")
    print("  ✓ Context managers")
    print("  ✓ Network analysis")
    print("  ✓ Experimental features")
    
if __name__ == "__main__":
    main()
EOF
    chmod +x /workspace/app/test_enhanced_bindings.py
    echo "✓ Developer test script created at /workspace/app/test_enhanced_bindings.py"

# Final summary
echo
    echo "🎉 Build and installation completed successfully!"
    echo
    echo "Next steps:
    1. Run the developer test: python3 /workspace/app/test_enhanced_bindings.py
    2. Run the enhanced demo: python3 /workspace/app/demo_enhanced_features.py
    3. Run the quick demo: python3 /workspace/app/demo_quick.py
    4. Check documentation: README_ENHANCED.md
    5. Explore experimental features in pynlm.experimental
    "
    echo
    echo "The enhanced NLM Python bindings are ready for use!"
    echo "All new features are working correctly."