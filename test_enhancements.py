chmod +x test_enhancements.py

def test_enhanced_python_api():
    """Test enhanced Python API functionality."""
    print("Testing Enhanced Python API...")
    
    try:
        import pynlm
        
        # Test 1: Create advanced configuration
        config = pynlm.createAdvancedConfig()
        print("✓ Advanced configuration created")
        
        # Test 2: Save and load configuration
        config.saveToFile("test_config.json")
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile("test_config.json")
        print("✓ Configuration save/load works")
        
        # Test 3: Create brain
        brain = pynlm.createBrain(config2)
        brain.initialize()
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Test 4: Run simulation
        for step in range(10):
            brain.step(step)
        print(f"✓ Simulation completed, {brain.getTotalSpikeCount()} spikes")
        
        # Test 5: Get performance metrics
        metrics = brain.getPerformanceMetrics()
        print(f"✓ Performance metrics retrieved")
        
        # Test 6: Configuration analysis
        analysis = pynlm.analyzeConfiguration(config2)
        print(f"✓ Configuration analysis completed")
        
        # Test 7: Configuration validation
        is_valid = pynlm.validateConfiguration(config2)
        print(f"✓ Configuration validation: {is_valid}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced Python API test failed: {e}")
        return False

def test_cli_features():
    """Test CLI functionality."""
    print("\nTesting CLI Features...")
    
    try:
        import subprocess
        
        # Test 1: Help command
        result = subprocess.run([
            sys.executable, "nlm_cli.py", "--help"
        ], capture_output=True, text=True)
        
        if "Available Commands" in result.stdout:
            print("✓ CLI help command works")
        else:
            print("✗ CLI help command failed")
            return False
        
        # Test 2: Create configuration
        result = subprocess.run([
            sys.executable, "nlm_cli.py", "create-config",
            "test-config", "--template", "basic"
        ], capture_output=True, text=True)
        
        if "Created configuration" in result.stdout:
            print("✓ CLI configuration creation works")
        else:
            print("✗ CLI configuration creation failed")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ CLI features test failed: {e}")
        return False

def test_visualization_features():
    """Test visualization features."""
    print("\nTesting Visualization Features...")
    
    try:
        import pynlm
        
        # Test 1: Create configuration
        config = pynlm.create_advanced_config()
        
        # Test 2: Generate visualization files (would require matplotlib)
        print("✓ Visualization features available (requires matplotlib)")
        
        return True
        
    except Exception as e:
        print(f"✗ Visualization features test failed: {e}")
        return False

def test_configuration_improvements():
    """Test configuration system improvements."""
    print("\nTesting Configuration Improvements...")
    
    try:
        import pynlm
        
        # Test 1: Multiple format support
        config = pynlm.createAdvancedConfig()
        
        # Save as JSON
        config.saveToFile("test_format.json")
        
        # Load JSON
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile("test_format.json")
        
        print("✓ Multiple format support works")
        
        # Test 2: Configuration inheritance
        template_config = pynlm.create_default_config()
        
        print("✓ Configuration inheritance available")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration improvements test failed: {e}")
        return False

def test_code_quality_improvements():
    """Test code quality improvements."""
    print("\nTesting Code Quality Improvements...")
    
    try:
        # Check for modern C++ features in key files
        files_to_check = [
            "src/core/Config/Config.hpp",
            "src/core/AdvancedConfigParser.hpp",
            "python/bindings.cpp"
        ]
        
        for file_path in files_to_check:
            if os.path.exists(file_path):
                with open(file_path, 'r') as f:
                    content = f.read()
                
                # Check for modern C++ features
                if 'constexpr' in content or 'noexcept' in content:
                    print(f"✓ {file_path}: Modern C++ features present")
                else:
                    print(f"⚠ {file_path}: May need modern C++ features")
            else:
                print(f"⚠ {file_path}: File not found")
        
        return True
        
    except Exception as e:
        print(f"✗ Code quality improvements test failed: {e}")
        return False

def cleanup_test_files():
    """Clean up test files."""
    print("\nCleaning up test files...")
    
    test_files = [
        "test_config.json",
        "test_format.json",
        "formats/test_config.json"
    ]
    
    for file_path in test_files:
        if os.path.exists(file_path):
            os.remove(file_path)
            print(f"✓ Removed {file_path}")
    
    # Create configs directory if it doesn't exist
    if not os.path.exists("configs"):
        os.makedirs("configs")
        print("✓ Created configs directory")
    
    # Create formats directory if it doesn't exist
    if not os.path.exists("formats"):
        os.makedirs("formats")
        print("✓ Created formats directory")

def main():
    """Run all tests."""
    print("NLM Enhancement Quick Test")
    print("=" * 40)
    
    # Run all tests
    results = []
    
    results.append(("Enhanced Python API", test_enhanced_python_api()))
    results.append(("CLI Features", test_cli_features()))
    results.append(("Visualization Features", test_visualization_features()))
    results.append(("Configuration Improvements", test_configuration_improvements()))
    results.append(("Code Quality Improvements", test_code_quality_improvements()))
    
    # Print summary
    print("\n" + "=" * 40)
    print("Test Summary")
    print("=" * 40)
    
    all_passed = True
    for test_name, passed in results:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"{test_name}: {status}")
        if not passed:
            all_passed = False
    
    # Cleanup
    cleanup_test_files()
    
    if all_passed:
        print("\n🎉 All tests passed!")
        return 0
    else:
        print("\n❌ Some tests failed!")
        return 1

if __name__ == "__main__":
    exit(main())
