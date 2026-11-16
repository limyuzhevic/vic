# NLM CLI Quick Start Guide

## Getting Started

Welcome to the NLM (Neural Learning Machine) unified command-line interface! This guide will help you get up and running with the new CLI in just a few minutes.

## 1. Installation

### Prerequisites

- C++20 compiler (g++ 9+, clang 10+)
- Python 3.8+ (optional, for enhanced bindings)
- CMake 3.16+

### Build Instructions

```bash
# Clone the repository
cd /path/to/your/project
git clone <repository-url>
cd nlm

# Configure with CLI support
cmake -B build -D NLM_BUILD_PYTHON_BINDINGS=ON

# Build
cmake --build build
```

## 2. Basic Usage

### Quick Test

Run a simple neural simulation:

```bash
./build/nlm simulate --episodes 1 --steps 50
```

This will run a basic Phase 2 simulation with 1 episode and 50 steps.

### Phase Commands

Run specific phase demonstrations:

```bash
# Phase 2: Basic neural computation
./build/nlm phase2

# Phase 3: World interaction (5 episodes, 200 steps each)
./build/nlm demo-phase3 --episodes 5 --steps 200

# Phase 4: Cognitive capabilities (50 trials)
./build/nlm phase4 --trials 50

# Phase 5: Performance optimizations
./build/nlm demo-phase5

# Phase 6: Integration tests
./build/nlm demo-phase6
```

### Configuration

Load your own configuration:

```bash
# Load JSON configuration
./build/nlm simulate --config configs/myconfig.json

# Load YAML configuration
./build/nlm simulate --config configs/myconfig.yaml

# Load legacy configuration
./build/nlm simulate --config configs/legacy.cfg
```

## 3. Help and Documentation

Get help for any command:

```bash
# General help
./build/nlm help

# Help for specific command
./build/nlm phase3 --help
./build/nlm benchmark --help
./build/nlm config --help
```

View complete documentation:

```bash
# View online documentation (if available)
browse https://nlm.readthedocs.io

# View local documentation
cat docs/CLI.md
```

## 4. Examples

### Basic Simulation

```bash
# Simple test with 1 episode, 100 steps
./build/nlm simulate --episodes 1 --steps 100

# Longer simulation with custom config
./build/nlm simulate --config configs/production.json --episodes 10
```

### Benchmarking

```bash
# Quick benchmark (default)
./build/nlm benchmark

# Comprehensive benchmark
./build/nlm benchmark --type comprehensive --iterations 3
```

### Configuration Management

```bash
# Save current configuration
./build/nlm config save --file configs/myconfig.json

# Validate configuration
./build/nlm config validate --file configs/myconfig.json

# Show current configuration
./build/nlm config show
```

## 5. Configuration Files

The NLM CLI supports three configuration file formats:

### JSON (configs/default.json)
```json
{
  "neuron_count": 500,
  "region_count": 2,
  "connection_probability": 0.1,
  "random_seed": 42
}
```

### YAML (configs/default.yaml)
```yaml
neuron_count: 500
region_count: 2
connection_probability: 0.1
random_seed: 42
```

### Legacy (configs/legacy.cfg)
```
neuron_count = 500
region_count = 2
connection_probability = 0.1
random_seed = 42
```

## 6. Common Commands

| Command | Description | Example |
|---------|-------------|---------|
| `./build/nlm help` | Show general help | `./build/nlm help` |
| `./build/nlm version` | Show version info | `./build/nlm version` |
| `./build/nlm status` | Show system status | `./build/nlm status` |
| `./build/nlm simulate` | Run neural simulation | `./build/nlm simulate` |
| `./build/nlm benchmark` | Run performance tests | `./build/nlm benchmark` |
| `./build/nlm config` | Manage configuration | `./build/nlm config load` |

## 7. Tips and Tricks

### For Beginners

1. **Start Simple**: Begin with `./build/nlm simulate --episodes 1 --steps 50`
2. **Use Examples**: Check the `configs/` directory for example configurations
3. **Check Help**: Use `--help` for detailed command information
4. **Verbose Mode**: Add `--verbose` to see detailed output

### For Advanced Users

1. **Custom Configurations**: Create your own JSON/YAML configuration files
2. **Benchmarking**: Run `./build/nlm benchmark` to test performance
3. **Scripting**: Automate tasks with shell scripts
4. **Enhanced Bindings**: Enable with `export NLM_ENHANCED=1`

## 8. Troubleshooting

### Common Issues

**Configuration File Not Found**
```bash
# Solution: Use absolute path or check file exists
./build/nlm simulate --config /path/to/configs/myconfig.json
```

**Command Not Recognized**
```bash
# Solution: Check command spelling or use help
./build/nlm unknown-command --help
```

**Memory Issues**
```bash
# Solution: Reduce simulation size
./build/nlm simulate --episodes 1 --steps 50
```

### Getting Help

1. **Online Documentation**: https://nlm.readthedocs.io
2. **GitHub Issues**: https://github.com/yourorg/nlm/issues
3. **Community Discord**: Join our Discord server for help

## 9. Next Steps

1. **Learn More**: Read the [CLI Documentation](src/python/CLI.md)
2. **Experiment**: Try different configuration options
3. **Benchmark**: Run `./build/nlm benchmark` to see performance
4. **Create Scripts**: Automate your workflows with shell scripts
5. **Contribute**: Help improve the CLI by reporting issues or adding features

## 10. Python Integration (Optional)

If you want to use NLM programmatically with Python:

```bash
# Enable enhanced Python bindings
export NLM_ENHANCED=1

# Run CLI with enhanced features
./build/nlm simulate --enhanced
```

The enhanced Python bindings provide:
- Context managers for better resource management
- Better error handling and debugging
- Advanced simulation controls
- Programmatic access to all NLM features

## Quick Reference

### Essential Commands

```bash
# Test installation
./build/nlm version

# Quick simulation
./build/nlm simulate --episodes 1 --steps 50

# Performance benchmark
./build/nlm benchmark

# Configuration management
./build/nlm config save --file myconfig.json
./build/nlm config load --file myconfig.json

# Get help
./build/nlm help
./build/nlm <command> --help
```

### Key Options

- `--help, -h`: Show help
- `--version, -v`: Show version
- `--config FILE, -c FILE`: Load configuration
- `--episodes N, -e N`: Number of episodes
- `--steps N, -s N`: Steps per episode
- `--verbose, -vv`: Verbose output
- `--enhanced, -e`: Use enhanced bindings

## Conclusion

The NLM unified CLI is now ready for use! You can:

1. **Run neural simulations** with various configurations
2. **Benchmark performance** to optimize your setup
3. **Manage configurations** with flexible file format support
4. **Experiment with different phases** to explore NLM capabilities
5. **Integrate with Python** for advanced programmatic control

The CLI is designed to be both powerful and easy to use, with comprehensive help and extensive configuration options.

Happy experimenting with the Neural Learning Machine!

---

*Version: 0.1.0*
*Last Updated: November 16, 2026*
*For more information: https://nlm.readthedocs.io*
