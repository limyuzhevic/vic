# NLM Improvements - Quick Start Guide

This document provides a quick overview of the improvements made to the NLM (Neural Learning Machine) project, including bug fixes, new functionality, and enhanced usability.

## Summary of Improvements

### 🐛 Critical Bug Fixes
1. **Configuration System**: Enhanced with proper JSON/YAML parser support
2. **Configuration Validation**: Added comprehensive error handling
3. **Duplicate Keys**: Fixed duplicate configuration parameters

### 🔧 New Command-Line Tools
1. **nlm** - Main command-line interface with subcommands:
   - `nlm create` - Create and initialize brains
   - `nlm simulate` - Run complete agent simulations
   - `nlm analyze` - Analyze brain states
   - `nlm config` - Configuration management
   - `nlm version` - Show version information

### 🐍 Enhanced Python API
1. **nlm_utilities** module with convenience functions:
   - Simplified brain creation and initialization
   - Complete simulation runners
   - Environment setup utilities
   - Brain analysis tools
   - Results export functionality

### 📚 Additional Improvements
1. **Utility Functions**: Common programming patterns
2. **Configuration Management**: Enhanced validation and templates
3. **Documentation**: Updated with new examples and usage guides

## Quick Start Examples

### 1. Using Command-Line Tools

```bash
# Create a simple brain
nlm create --neurons 500 --steps 100 --save brain.bin

# Run a complete simulation
nlm simulate --neurons 1000 --steps 500 --width 20 --height 20

# Analyze brain state
nlm analyze brain.bin --json stats.json

# Create configuration file
nlm config create

# Validate configuration
nlm config validate myconfig.json
```

### 2. Using Python Utilities

```python
import pynlm
from python import nlm_utilities

# Create brain using enhanced utility
brain, config = nlm_utilities.create_and_initialize_brain(neuron_count=200)

# Run complete simulation
results = nlm_utilities.run_simulation(neurons=500, steps=1000)

# Analyze brain activity
stats = nlm_utilities.analyze_brain_activity(brain)

# Export results
nlm_utilities.export_results_to_csv(brain, agent, world, 'results.csv')

# Create structured brain
brain, regions = nlm_utilities.create_structured_brain()

# Create standard environment
world = nlm_utilities.create_grid_environment(width=10, height=10)
```

### 3. Configuration Examples

#### JSON Configuration (Recommended)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.01,
            "tau": 20.0
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000
    }
}
```

#### Simple Key-Value Configuration (Backward Compatible)

```
# configs/custom.cfg
brain.neuron_count = 500
brain.region_count = 1
plasticity.stdp.learning_rate = 0.01
simulation.timestep = 0.001
log_level = INFO
```

## Installation and Setup

### Quick Install

```bash
# Clone the project
cd /path/to/nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings (if needed)
# See docs/INSTALL.md for details
```

### Using Python Utilities

The enhanced Python utilities are available in the `python/` directory:

```bash
cd /path/to/nlm
pip install -e python/  # Install Python bindings
python -c "import pynlm; from python import nlm_utilities; print('NLM utilities loaded successfully')"
```

### Using Command-Line Tools

Make the script executable and add to PATH:

```bash
cd /path/to/nlm
chmod +x scripts/nlm

# Add to PATH (optional)
echo 'export PATH="$PATH:/path/to/nlm/scripts"' >> ~/.bashrc
source ~/.bashrc

# Test the tools
nlm version
```

## Migration Guide

### From Old to New API

#### Configuration Files
- **Old**: Simple key=value format
- **New**: JSON format recommended, old format still supported

#### Python Code
- **Old**: Basic pynlm functions
- **New**: Enhanced convenience functions in `nlm_utilities`

#### Command-Line Usage
- **Old**: No command-line interface
- **New**: Comprehensive CLI tools for all common operations

## New Features by Category

### Configuration Management
- JSON/YAML file support
- Configuration validation
- Schema validation
- Configuration templates

### Brain Creation and Simulation
- Simplified brain creation with utilities
- Complete simulation runners
- Environment setup tools
- Progress monitoring

### Analysis and Export
- Brain activity analysis
- Results export to CSV
- Performance benchmarking
- Statistical analysis

### Command-Line Interface
- Consistent interface across all tools
- Comprehensive help documentation
- Flexible argument parsing
- Script-friendly output

### Error Handling
- Enhanced error reporting
- Graceful degradation
- Detailed error messages
- Comprehensive testing

## Testing the Improvements

Run the verification tests to ensure everything works:

```bash
cd /path/to/nlm
python test_improvements.py
```

The tests will verify:
- Configuration system functionality
- Python utilities availability
- Command-line tool compatibility
- Backward compatibility with existing code

## Troubleshooting

### Common Issues

**Q: Configuration files are not loading**
A: Try JSON format first. Simple key=value format is still supported for backward compatibility.

**Q: Python utilities not found**
A: Ensure the Python package is installed: `pip install python/`

**Q: Command-line tools not working**
A: Make the script executable: `chmod +x scripts/nlm`

**Q: Backward compatibility issues**
A: All improvements maintain backward compatibility. Existing code should work without changes.

### Getting Help

- **Documentation**: See `docs/` for detailed documentation
- **Examples**: Look in `examples/` for comprehensive usage examples
- **Issues**: Report problems at the project repository
- **Community**: Join the NLM discussion forum

## Future Improvements

The project continues to evolve with:

1. **Enhanced Configuration Validation**
2. **More Command-Line Tools**
3. **Additional Python Utilities**
4. **Advanced Documentation**
5. **Performance Optimizations**
6. **Integration with External Tools**

## Support

This is an experimental research project. For support, documentation, and community:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Discussions**: Project issue tracker
- **Examples**: Look in the `examples/` directory

## License

MIT License. See LICENSE file for details.

---

**Enjoy using the enhanced NLM brain simulation framework!**

This version includes significant improvements over the original release, making it more accessible, easier to use, and more powerful while maintaining full backward compatibility.