# NLM Installation and Development Guides

## Overview

The NLM installation and development guides provide **comprehensive setup instructions** for different environments and user needs. These guides cover installation on various platforms, development setup, and advanced development workflows.

> **Key Feature:** Installation guides provide **step-by-step instructions** for different platforms and environments, from basic setup to advanced development workflows.

## Table of Contents

- [Quick Start Installation](#quick-start-installation)
- [Installation on Linux/Unix Systems](#installation-on-linuxunix-systems)
- [Installation on macOS](#installation-on-macos)
- [Installation on Windows](#installation-on-windows)
- [Installation in Docker Containers](#installation-in-docker-containers)
- [Installation in Cloud Environments](#installation-in-cloud-environments)
- [Development Setup](#development-setup)
- [Building from Source](#building-from-source)
- [Installing Python Bindings](#installing-python-bindings)
- [Development Workflow](#development-workflow)
- [Project Structure](#project-structure)
- [Development Best Practices](#development-best-practices)
- [Troubleshooting Installation Issues](#troubleshooting-installation-issues)
- [Installation Checklist](#installation-checklist)

---

## Quick Start Installation

### What You'll Learn

This guide provides a **quick overview** of NLM installation for different platforms. It covers:

- Basic installation requirements
- Quick setup for common platforms
- Testing installation
- Basic troubleshooting

> **Target Audience:** Users who want to install NLM quickly and efficiently
> **Time Required:** 10-30 minutes

### Quick Installation Steps

#### Step 1: Check System Requirements

```bash
# Check system requirements
python3 --version
# Should output: Python 3.8.0 or higher

cmake --version
# Should output: cmake version 3.16.0 or higher

g++ --version
# Should output: g++ (GCC) version 11.0 or higher
```

#### Step 2: Install Python Dependencies

```bash
# Install Python dependencies
pip3 install --upgrade pip
pip3 install numpy matplotlib scikit-learn pandas
```

#### Step 3: Clone and Build NLM

```bash
# Clone NLM repository
mkdir ~/projects
cd ~/projects
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir -p build
cd build

# Configure with Phase 6 integration
cmake .. -DENABLE_PHASE6_INTEGRATION=ON -DBUILD_PYTHON_BINDINGS=ON

# Build with optimization
make -j$(nproc)
```

#### Step 4: Install Python Bindings

```bash
# Install Python bindings
cd ~/projects/nlm/python
pip3 install -e .[dev,perf,cog]

# Verify installation
python3 -c "
import pynlm
print('✓ NLM Python bindings installed successfully')
print(f'  Version: {pynlm.__version__}')
print(f'  Phase 6 features: {hasattr(pynlm, \"createPhase6Config\")}')
"
```

#### Step 5: Test Installation

```bash
# Test NLM installation
cd ~/projects/nlm/build

# Test basic NLM executable
./nlm --help

# Test Phase 6 demo
./nlm_phase6_demo --help

# Test Python bindings
python3 -c "
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f'✓ Brain created: {brain.getNeuronCount()} neurons')
brain.shutdown()
"
```

### Quick Installation Summary

| Platform | Command | Notes |
|----------|---------|-------|
| **Linux/Unix** | `bash install.sh` | Recommended for development |
| **macOS** | `bash install.sh` | Native support |
| **Windows** | `install.bat` | PowerShell or Command Prompt |
| **Docker** | `docker build -t nlm .` | Containerized deployment |
| **Cloud** | `gcloud build` | Google Cloud Platform |

### Quick Troubleshooting

**Common Issues and Solutions:**

#### Issue 1: CMake Version Too Old
```bash
# Install latest CMake
sudo apt-get update
sudo apt-get install -y cmake
# Or install from source
# Download from https://cmake.org/download/
```

#### Issue 2: Python Dependencies Missing
```bash
# Install missing dependencies
pip3 install --missing-deps
pip3 install pybind11 scikit-build-core pytest
```

#### Issue 3: Build Errors
```bash
# Try clean build
rm -rf build
mkdir build
cd build
cmake .. -DENABLE_PHASE6_INTEGRATION=ON
make -j$(nproc) VERBOSE=1
```

### Quick Installation Verification

```bash
# Verify NLM installation
cat > ~/nlm_installation_test.py << 'EOF'
#!/usr/bin/env python3
"""
Quick NLM installation test
"""
import pynlm
import sys

def test_installation():
    print("=== NLM Installation Test ===\n")
    
    # Test 1: Python bindings
    print("Test 1: Python Bindings")
    print("-" * 30)
    
    try:
        import pynlm
        print("✓ Python module imported successfully")
        print(f"  Version: {pynlm.__version__}")
        print(f"  Phase 6 support: {hasattr(pynlm, 'createPhase6Config')}")
        
    except ImportError as e:
        print(f"✗ Python import failed: {e}")
        return False
    
    # Test 2: Configuration
    print("\nTest 2: Configuration")
    print("-" * 30)
    
    try:
        default_config = pynlm.createDefaultConfig()
        print("✓ Default configuration created")
        print(f"  Neuron count: {default_config.get('brain.neuron_count')}")
        
        phase6_config = pynlm.createPhase6Config()
        print("✓ Phase 6 configuration created")
        print(f"  Memory systems: {phase6_config.get('memory.working.enabled')}")
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        return False
    
    # Test 3: Brain creation
    print("\nTest 3: Brain Creation")
    print("-" * 30)
    
    try:
        brain = pynlm.createBrain(default_config)
        brain.initialize()
        
        print("✓ Brain created and initialized")
        print(f"  Neurons: {brain.getNeuronCount()}")
        print(f"  Spike count: {brain.getTotalSpikeCount()}")
        
        brain.shutdown()
        
    except Exception as e:
        print(f"✗ Brain creation failed: {e}")
        return False
    
    # Test 4: Memory systems
    print("\nTest 4: Memory Systems")
    print("-" * 30)
    
    try:
        brain = pynlm.createBrain(phase6_config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        assert working_memory is not None
        assert episodic_memory is not None
        assert associative_memory is not None
        
        print("✓ All memory systems available")
        
        # Test working memory operations
        working_memory.store("test", "value")
        retrieved = working_memory.retrieve("test", threshold=0.7)
        
        assert retrieved is not None
        print("✓ Working memory operations working")
        
        brain.shutdown()
        
    except Exception as e:
        print(f"✗ Memory system test failed: {e}")
        return False
    
    print("\n=== All Installation Tests Passed ===")
    return True

if __name__ == "__main__":
    success = test_installation()
    sys.exit(0 if success else 1)
EOF

python3 ~/nlm_installation_test.py
```

### Installation Success Criteria

✅ **Python Dependencies:** All required packages installed
✅ **C++ Build:** NLM compiled successfully
✅ **Python Bindings:** Phase 6 features available
✅ **Basic Functionality:** Brain creation and simulation works
✅ **Memory Systems:** All memory systems operational
✅ **Performance:** Reasonable execution speed
✅ **Error Handling:** Graceful error handling

### Installation Support

**Online Resources:**
- **Documentation**: https://nlm-project.org/docs
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Support**: support@nlm-project.org

**Community Support:**
- **Slack/Discord**: Real-time community support
- **Forums**: Technical discussions and Q&A
- **Email**: Direct technical support

### Installation FAQs

**Q: What Python packages are required?**
A: `numpy`, `matplotlib`, `scikit-learn`, `pandas`, `pybind11`, `scikit-build-core`, `pytest`

**Q: What CMake options are available?**
A: `-DENABLE_PHASE6_INTEGRATION=ON`, `-DENABLE_ADVANCED_COGNITION=ON`, `-DENABLE_PERFORMANCE_OPTIMIZATIONS=ON`, `-DENABLE_CUDA=ON`, `-DENABLE_MPI=ON`

**Q: How do I test the installation?**
A: Run the provided test scripts or use the `python3 ~/nlm_installation_test.py` command.

**Q: What if the build fails?**
A: Check system requirements, update packages, and try a clean build. See troubleshooting section for details.

**Q: Can I install without Python bindings?**
A: Yes, you can install the C++ version only by setting `-DBUILD_PYTHON_BINDINGS=OFF`.

### Installation Conclusion

The quick start installation provides a **fast and efficient way** to set up NLM for most users. For more advanced needs or specific requirements, refer to the detailed installation guides below.

> **Quick Installation Complete:** You now have NLM v1.0 with Phase 6 integration installed and ready to use!

**Next Steps:**
1. **Explore Tutorial 1:** Basic Neural Computation
2. **Learn Phase 6 Features:** Advanced cognitive systems
3. **Set up Research:** Create experimental configurations
4. **Optimize Performance:** Use performance guides
5. **Contribute:** Share your experience with the community

**Happy installing with NLM! 🚀**