# NLM Advanced Tools

This directory contains the five advanced NLM command-line tools designed to support Phase 6 integration and provide enhanced functionality for advanced users.

## Available Tools

1. **NLMConfig** - Configuration Management Tool
2. **NLMAnalyze** - Network Analysis Tool  
3. **NLMProfile** - Performance Profiling Tool
4. **NLMIntegrate** - Integration Validation Tool
5. **NLMExport** - Data Export Tool

## Quick Start

### Installation

```bash
# Install Python dependencies
pip install psutil numpy pyyaml

# Install NLM Python module
pip install -e .
```

### Building C++ Tools

```bash
cd /path/to/nlm/source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Tools

#### Python Tools
```bash
# Run from command line
cd /path/to/nlm/source
python3 tools/NLMConfig/main.py --help
python3 tools/NLMAnalyze/main.py --help
python3 tools/NLMProfile/main.py --help
python3 tools/NLMIntegrate/main.py --help
python3 tools/NLMExport/main.py --help
```

#### C++ Tools
```bash
# Run from build directory
cd /path/to/nlm/source/build
./nlmconfig --help
./nlmanalyze --help
./nlmprofile --help
./nlmintegrate --help
./nllexport --help
```

## Usage Examples

### Configuration Management
```bash
# Validate configuration
nlmconfig --validate configs/default.cfg

# Generate configuration
nlmconfig --generate my_config.cfg

# Compare configurations
nlmconfig --compare config1.cfg config2.cfg
```

### Network Analysis
```bash
# Analyze network connectivity
nlmanalyze --connectivity my_config.cfg

# Analyze clustering
nlmanalyze --clustering my_config.cfg

# Analyze topology
nlmanalyze --topology my_config.cfg

# Analyze performance
nlmanalyze --performance my_config.cfg
```

### Performance Profiling
```bash
# Profile performance
nlmprofile --performance 60 my_config.cfg

# Profile memory usage
nlmprofile --memory my_config.cfg

# Detect bottlenecks
nlmprofile --bottlenecks my_config.cfg

# Benchmark configuration
nlmprofile --benchmark "neuron_count=500" my_config.cfg
```

### Integration Validation
```bash
# Verify Phase 6 integration
nlmintegrate --verify my_config.cfg

# Run comprehensive integration test
nlmintegrate --comprehensive-test my_config.cfg

# Perform health check
nlmintegrate --health-check my_config.cfg
```

### Data Export
```bash
# Export configuration
nllexport --export-config config.json json my_config.cfg

# Export brain data
nllexport --export-brain-data brain_data.json json my_config.cfg

# Export all data to directory
nllexport --export-all output_data my_config.cfg
```

## Testing

### Tool Tests
```bash
cd /path/to/nlm/source
python3 -m pytest tests/test_nlmconfig.py
python3 -m pytest tests/test_nlmanalyze.py
python3 -m pytest tests/test_nlmprofile.py
python3 -m pytest tests/test_nlmintegrate.py
python3 -m pytest tests/test_nlmexport.py
```

### Integration Tests
```bash
cd /path/to/nlm/source
./scripts/test_all_tools.sh
./scripts/integration_workflow.sh
```

## Documentation

### Tool Documentation
- `docs/TOOLS.md` - Comprehensive tool documentation
- `docs/QUICK_START.md` - Quick start guide

### Implementation Summary
- `IMPLEMENTATION_SUMMARY.md` - Detailed implementation overview
- `TOOLS.md` - Complete tool reference

### Scripts
- `scripts/` - Testing and workflow scripts
- `tests/` - Unit test files

## Configuration Files

### Sample Configuration
```bash
# View sample configuration
cat configs/default.cfg
```

### Configuration Formats
- **Simple (.cfg)**: Key=value format
- **JSON (.json)**: Structured JSON
- **YAML (.yaml)**: YAML configuration

## Key Features

### Language Support
- **C++ Interfaces**: High performance for critical applications
- **Python Interfaces**: Easy scripting and automation
- **Python bindings**: Full Python integration via `pynlm`

### Configuration Support
- Multiple configuration file formats
- Integration with NLM configuration system
- Configuration validation and generation

### Analysis Capabilities
- Network connectivity analysis
- Clustering and community detection
- Topology and graph analysis
- Performance metrics calculation

### Profiling Capabilities
- Performance profiling (CPU, memory, I/O)
- Memory usage analysis and leak detection
- Bottleneck detection and optimization recommendations
- Configuration benchmarking

### Integration Testing
- Phase 6 integration verification
- Comprehensive integration testing
- Health monitoring and status reporting

### Data Export
- Brain state export (JSON, binary)
- Configuration export
- Simulation results export
- Analysis data export

## Phase 6 Integration Support

All tools are specifically designed to support Phase 6 integration:

1. **Configuration Management**: Support for Phase 6 configuration parameters
2. **Network Analysis**: Analysis of integrated brain networks
3. **Performance Profiling**: Profiling of integrated system performance
4. **Integration Testing**: Comprehensive integration testing for Phase 6
5. **Data Export**: Export of Phase 6 system data

## Getting Help

### Tool Help
```bash
nlmconfig --help
nlmanalyze --help
nlmprofile --help
nlmintegrate --help
nllexport --help
```

### Python Help
```python
import nlm.tools
help(nlm.tools.NLMConfig)
help(nlm.tools.NLMAnalyze)
help(nlm.tools.NLMProfile)
help(nlm.tools.NLMIntegrate)
help(nlm.tools.NLMExport)
```

### Documentation
- See `docs/tools/` for detailed documentation
- Check `README.md` in the project root
- Review `TOOLS.md` in the tools directory

## Installation Issues?

If you encounter installation issues, visit the NLM project repository for:

- Installation guides
- Troubleshooting documentation
- Community support
- Issue reporting

## License

These tools are licensed under the MIT License. See `LICENSE` for details.

---

**Version**: 1.0
**Author**: NLM Project Team
**Date**: 2026-10-11
**Phase**: 6 - Final Integration

The NLM advanced tools provide comprehensive functionality for configuration management, network analysis, performance profiling, integration validation, and data export. All tools are production-ready and designed to work seamlessly with the existing NLM framework.
