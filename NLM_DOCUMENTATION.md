# NLM - Neural Learning Machine

**Comprehensive Documentation for Enhanced Neural Simulation Framework**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Table of Contents

1. [Enhanced Configuration System](python/nlm_advanced/config/CONFIG_DOCUMENTATION.md)
2. [Advanced Python Bindings](python/nlm_advanced/BINDINGS_DOCUMENTATION.md)
3. [Advanced Utilities and Tools](python/nlm_advanced/README.md)
4. [Visualization and Debugging Tools](python/nlm_advanced/debug/__init__.py)
5. [Examples and Tutorials](NLM_EXAMPLES.md)
6. [Best Practices](NLM_BEST_PRACTICES.md)
7. [Migration Guide](MIGRATION_GUIDE.md)
8. [API Reference](NLM_API_REFERENCE.md)
9. [Performance Optimization](NLM_TROUBLESHOOTING.md)
10. [Troubleshooting](NLM_TROUBLESHOOTING.md)

## Quick Start

### Installation

```bash
# Clone the NLM repository
https://github.com/nlm-project/nlm.git

# Build the enhanced bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python dependencies
pip install -r requirements.txt
pip install -e .
```

### Basic Usage

```python
#!/usr/bin/env python3
"""
Basic NLM Neural Simulation Example
"""

import sys
import os

# Add the build directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    from nlm_enhanced import *
    print("Successfully imported nlm_enhanced bindings")
except ImportError as e:
    print(f"Error importing nlm_enhanced: {e}")
    print("Make sure to build the enhanced bindings first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make -j4")
    sys.exit(1)

def demonstrate_basic_functionality():
    """Demonstrate basic functionality of enhanced bindings"""
    print("\n=== Basic Functionality Demo ===")
    
    # Create enhanced configuration
    print("\n1. Creating enhanced configuration...")
    config = createDefaultConfig()
    print(f"   Created configuration: {config.toString()}")
    
    # Set configuration values with automatic type conversion
    print("\n2. Setting configuration values...")
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    config.set("use_gpu", True)
    config.set("model_name", "neural_network")
    
    # Get values with automatic type conversion
    print("\n3. Getting configuration values...")
    print(f"   Learning rate: {config['learning_rate']} (type: {type(config['learning_rate']).__name__})")
    print(f"   Batch size: {config['batch_size']} (type: {type(config['batch_size']).__name__})")
    print(f"   Use GPU: {config['use_gpu']} (type: {type(config['use_gpu']).__name__})")
    print(f"   Model name: {config['model_name']} (type: {type(config['model_name']).__name__})")
    
    # Demonstrate iteration
    print("\n4. Iterating over configuration...")
    for key, value in config.items():
        print(f"   {key}: {value}")
    
    # Demonstrate validation
    print("\n5. Validating configuration...")
    if config.validate():
        print("   Configuration is valid!")
    else:
        print("   Configuration is invalid!")
    
    return config

if __name__ == "__main__":
    print("Enhanced NLM Python Bindings - Quick Start Demo")
    print("=" * 50)
    
    try:
        # Demonstrate basic functionality
        config = demonstrate_basic_functionality()
        
        print("\n" + "=" * 50)
        print("Basic functionality demo completed successfully!")
        print("\nNext Steps:")
        print("  1. Explore advanced features in the documentation")
        print("  2. Try the provided examples")
        print("  3. Configure your neural simulation parameters")
        print("  4. Start your neural simulation!")
        
    except Exception as e:
        print(f"\nError during demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
```

## Key Improvements

### Enhanced Configuration System

The enhanced configuration system provides:

1. **Type Safety**: Automatic type conversion and validation
2. **JSON/YAML Support**: Multiple configuration file formats
3. **Template System**: Reusable configuration templates
4. **Parameter Search**: Automated hyperparameter optimization
5. **Configuration Validation**: Comprehensive validation rules
6. **Inheritance Support**: Template inheritance for complex configurations

### Enhanced Python Bindings

The enhanced Python bindings provide:

1. **Type Safety**: NLMType system with automatic type conversion
2. **Error Handling**: Custom exception types with detailed error information
3. **Pythonic Interfaces**: Context managers and intuitive APIs
4. **Performance**: Optimized for large-scale neural simulations
5. **Backward Compatibility**: Seamless integration with existing pynlm

### Advanced Utilities

The advanced utilities package includes:

1. **Simulation Control**: Custom time-stepping, pause/resume, multi-agent coordination
2. **Performance Monitoring**: Resource usage tracking, simulation progress tracking
3. **Data Management**: Batch processing, checkpoint management, result aggregation
4. **Agent Capabilities**: Custom architectures, social learning, hierarchical control
5. **Robustness**: Auto-checkpointing, error recovery, state inspection

### Visualization and Debugging Tools

The visualization and debugging toolkit provides:

1. **Real-time Visualization**: Neural activity monitoring and plotting
2. **Connectivity Analysis**: Neural connectivity graph exploration
3. **Activity Profiling**: Statistical analysis and heat map generation
4. **Step Debugging**: Breakpoint-based debugging with state inspection
5. **Performance Profiling**: Comprehensive performance monitoring

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/                    # Source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/         # Neural dynamics
│   ├── plasticity/       # Plasticity rules
│   ├── development/      # Developmental system
│   ├── neuromodulation/   # Neuromodulators (integrated)
│   ├── memory/           # Memory systems (integrated)
│   ├── prediction/       # Prediction systems (integrated)
│   ├── cognition/        # Cognitive mechanisms (integrated)
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework (Phase 6)
│   └── visualization/    # Visualization
├── python/                # Python bindings
│   ├── nlm_advanced/     # Enhanced NLM utilities
│   │   ├── __init__.py
│   ├── config/           # Configuration tools
│   ├── debug/            # Debugging and visualization tools
│   ├── sim/              # Simulation control
│   ├── performance/      # Performance monitoring
│   ├── agent/            # Agent capabilities
│   ├── data/             # Data management
│   ├── batch/            # Batch processing
│   ├── recovery/         # Recovery and robustness
├── tests/                 # Tests
└── configs/              # Configuration files
```

## Getting Help

### Documentation

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues

### Community

- **Discord**: Join our community for discussions
- **Forum**: Ask questions and share experiences
- **Slack**: Real-time support

### Support

For support with the enhanced NLM Python bindings:

1. **Check the Documentation**: Start with the Quick Start guide
2. **Review Examples**: Look at the provided examples
3. **Search the Issue Tracker**: Check if your issue has been reported
4. **Open a New Issue**: Provide detailed information about your problem
5. **Contact Support**: Reach out to the development team

## Contributing

### Code Contributions

1. **Fork the Repository**: Create your own fork of the NLM repository
2. **Create a Branch**: Work on a specific feature branch
3. **Write Tests**: Ensure your changes are well-tested
4. **Follow Code Style**: Adhere to the project's coding standards
5. **Submit a Pull Request**: Submit your changes for review

### Documentation Contributions

1. **Improve Existing Documentation**: Fix errors and add examples
2. **Add New Documentation**: Create documentation for new features
3. **Update Tutorials**: Improve existing tutorials
4. **Write Best Practices**: Document recommended practices

### Bug Reports

1. **Reproduce the Bug**: Provide clear steps to reproduce the issue
2. **Provide Error Messages**: Include relevant error messages and stack traces
3. **Test Environment**: Describe your test environment
4. **Expected Behavior**: Explain what should happen
5. **Actual Behavior**: Explain what actually happens

## License

MIT

## Authors

Research project — See docs for scientific background.

---

*Last updated: $(date '+%Y-%m-%d %H:%M:%S UTC')*

*This documentation is part of the NLM Phase 6 Final Integration project.*

For more information about the NLM project, visit our [website](https://nlm.org) or [documentation](https://nlm.readthedocs.io).
