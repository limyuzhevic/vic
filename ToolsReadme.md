# NLM Advanced Command-Line Tools

This document provides an overview of the advanced command-line tools available for the NLM (Neural Learning Machine) project, designed to support Phase 6 integration and provide enhanced functionality for advanced users.

## Overview

The NLM advanced tools provide comprehensive support for:

- **Configuration Management**: Configuration validation, generation, comparison, and parameter sensitivity analysis
- **Network Analysis**: Neuron connectivity analysis, clustering, network topology visualization
- **Performance Profiling**: Performance profiling, memory usage analysis, benchmarking
- **Integration Validation**: Phase 6 integration verification, testing automation, and reporting
- **Data Export**: Brain state export, configuration export, simulation results export

## Available Tools

### 1. NLMConfig - Configuration Management Tool

**Description**: Advanced configuration management with validation, generation, comparison, and parameter sensitivity analysis.

**Features**:
- Configuration file validation (JSON/YAML/CFG)
- Configuration generation with templates
- Configuration comparison (diff, similarity)
- Parameter sensitivity analysis
- Integration with NLM configuration system

**Usage Examples**:
```bash
# Validate a configuration file
nlmconfig --validate configs/default.cfg

# Generate configuration from template
nlmconfig --generate my_config.cfg

# Compare two configurations
nlmconfig --compare configs/default.cfg my_config.cfg

# Analyze parameter sensitivity
nlmconfig --analyze configs/default.cfg

# Export as JSON
nlmconfig --export-json config.json configs/default.cfg
```

**Supported Configurations**:
- Simple .cfg files (key=value format)
- JSON files (.json)
- YAML files (.yaml, .yml)

---

### 2. NLMAnalyze - Network Analysis Tool

**Description**: Comprehensive network analysis including connectivity, clustering, topology, and performance metrics.

**Features**:
- Network connectivity analysis (neurons, synapses, regions)
- Clustering analysis (community detection, modularity)
- Network topology visualization (graph metrics, degree distribution)
- Performance metrics calculation (firing rates, connectivity density)
- Integration with NLM brain analysis capabilities

**Usage Examples**:
```bash
# Analyze network connectivity
nlmanalyze --connectivity configs/default.cfg

# Analyze clustering
nlmanalyze --clustering configs/default.cfg

# Analyze topology
nlmanalyze --topology configs/default.cfg

# Analyze performance
nlmanalyze --performance configs/default.cfg

# Run all analyses
nlmanalyze --all configs/default.cfg

# Save analysis report
nlmanalyze --connectivity --output analysis.txt configs/default.cfg
```

**Analysis Categories**:
- Connectivity analysis
- Clustering analysis
- Topology analysis
- Performance analysis

---

### 3. NLMProfile - Performance Profiling Tool

**Description**: Advanced performance profiling and benchmarking for NLM systems.

**Features**:
- Performance profiling (CPU, memory, I/O)
- Memory usage analysis
- Memory leak detection
- Performance bottleneck detection
- Benchmarking for different configurations
- Performance optimization recommendations

**Usage Examples**:
```bash
# Profile performance for 60 seconds
nlmprofile --performance 60 configs/default.cfg

# Profile memory usage
nlmprofile --memory configs/default.cfg

# Profile for memory leaks
nlmprofile --memory-leak 30 configs/default.cfg

# Detect performance bottlenecks
nlmprofile --bottlenecks configs/default.cfg

# Benchmark configuration
nlmprofile --benchmark "neuron_count=500,region_count=1" configs/default.cfg

# Run all profiling types
nlmprofile --all configs/default.cfg

# Save profiling report
nlmprofile --performance 30 --output profile.txt configs/default.cfg
```

**Profiling Categories**:
- Performance profiling
- Memory profiling
- Memory leak detection
- Bottleneck detection
- Configuration benchmarking

---

### 4. NLMIntegrate - Integration Validation Tool

**Description**: Comprehensive integration validation and testing for Phase 6.

**Features**:
- Phase 6 integration verification
- Integration testing automation
- Integration validation and validation reports
- Integration health monitoring
- Integration with Phase 6 experiment framework

**Usage Examples**:
```bash
# Verify Phase 6 integration
nlmintegrate --verify configs/default.cfg

# Run quick integration test
nlmintegrate --quick-test configs/default.cfg

# Run comprehensive integration test
nlmintegrate --comprehensive-test configs/default.cfg

# Run all integration tests
nlmintegrate --all-tests configs/default.cfg

# Perform integration health check
nlmintegrate --health-check configs/default.cfg

# Generate integration report
nlmintegrate --generate-report report.txt configs/default.cfg
```

**Integration Tests**:
- Integration verification
- Quick integration test
- Comprehensive integration test
- Health monitoring
- Report generation

---

### 5. NLMExport - Data Export Tool

**Description**: Comprehensive data export capabilities for NLM systems.

**Features**:
- Brain state export (JSON, binary, etc.)
- Configuration export
- Simulation results export
- Analysis data export
- Integration with NLM data management

**Usage Examples**:
```bash
# Export configuration
nllexport --export-config config.json json configs/default.cfg

# Export brain data
nllexport --export-brain-data brain_data.json json configs/default.cfg

# Export simulation results
nllexport --export-simulation-results results.json configs/default.cfg

# Export analysis data
nllexport --export-analysis-data analysis.json configs/default.cfg

# Export all data to directory
nllexport --export-all /path/to/output/configs/default.cfg
```

**Export Categories**:
- Configuration export
- Brain data export
- Simulation results export
- Analysis data export

---

## Tool Features

### Language Support
- **C++ Interfaces**: All tools provide C++ interfaces for performance-critical applications
- **Python Interfaces**: All tools provide Python interfaces for scripting and automation
- **Python bindings**: Full Python bindings available via `pynlm` module

### Command-Line Interface
- **Consistent syntax**: All tools follow a consistent command-line interface pattern
- **Help system**: All tools provide comprehensive help documentation
- **Error handling**: Robust error handling with informative messages
- **Validation**: Input validation and parameter checking

### Configuration Support
- **Multiple formats**: Support for JSON, YAML, and simple key=value configuration files
- **Integration with NLM config system**: Seamless integration with the existing NLM configuration system
- **Configuration validation**: Built-in configuration validation

### Output Formats
- **Text reports**: Human-readable text reports for interactive use
- **JSON output**: Structured JSON output for machine processing
- **Binary formats**: Binary formats for efficiency
- **File output**: Support for saving outputs to files

### Testing Support
- **Unit tests**: Each tool includes comprehensive unit tests
- **Integration tests**: Integration testing for tool interactions
- **Performance testing**: Performance benchmarking capabilities
- **Configuration testing**: Configuration validation and testing

### Documentation
- **Help system**: Built-in help system with usage examples
- **Documentation**: Comprehensive documentation for all tools
- **Examples**: Usage examples for common tasks
- **API references**: Detailed API documentation

---

## Installation and Usage

### Prerequisites
- NLM project installed and built
- C++ compiler with C++11 support
- Python 3.6+ (for Python tools)
- Required dependencies (psutil, numpy for Python tools)

### Building

#### C++ Tools
```bash
cd /path/to/nlm/source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Python Tools
```bash
# Install Python dependencies
pip install psutil numpy

# Install NLM Python module
pip install -e .
```

### Running Tools

#### From Build Directory
```bash
# Run C++ tools
./nlmconfig [options] [config_file]
./nlmanalyze [options] [config_file]
./nlmprofile [options] [config_file]
./nlmintegrate [options] [config_file]
./nllexport [options] [config_file]
```

#### Python Tools
```bash
# Run Python tools from command line
cd /path/to/nlm/source
python tools/NLMConfig/main.py [options] [config_file]
python tools/NLMAnalyze/main.py [options] [config_file]
python tools/NLMProfile/main.py [options] [config_file]
python tools/NLMIntegrate/main.py [options] [config_file]
python tools/NLMExport/main.py [options] [config_file]
```

### Python API

For Python users, all tools are available through the `nlm.tools` module:

```python
import nlm.tools

# Create tool instances
config_tool = nlm.tools.NLMConfig()
analyze_tool = nlm.tools.NLMAnalyze()
profile_tool = nlm.tools.NLMProfile()
integrate_tool = nlm.tools.NLMIntegrate()
export_tool = nlm.tools.NLMExport()

# Load configuration
config_tool.load_config("configs/default.cfg")

# Run analysis
results = analyze_tool.analyze_connectivity()
```

### Tool Integration

The tools are designed to work together seamlessly:

1. **Configuration Tool**: Use `nlmconfig` to manage and validate configurations
2. **Analysis Tool**: Use `nlmanalyze` to analyze brain networks and performance
3. **Profiling Tool**: Use `nlmprofile` to profile system performance
4. **Integration Tool**: Use `nlmintegrate` to verify and test integration
5. **Export Tool**: Use `nllexport` to export data for further analysis

### Phase 6 Integration

All tools are specifically designed to support Phase 6 integration:

- **Integration Verification**: `nlmintegrate --verify` validates Phase 6 integration
- **Integration Testing**: `nlmintegrate --comprehensive-test` runs complete integration tests
- **Performance Analysis**: `nlmanalyze` and `nlmprofile` help optimize integrated systems
- **Configuration Management**: `nlmconfig` manages Phase 6 configuration
- **Data Export**: `nllexport` exports integrated system data

### Tool Scripts

Shell scripts are provided for common workflows:

- `scripts/nlm_analysis.sh`: Run complete network analysis
- `scripts/nlm_profile.sh`: Run performance profiling
- `scripts/nlm_integration.sh`: Run integration testing
- `scripts/nlm_export.sh`: Export all data

### Testing

Run the tool tests:

```bash
cd /path/to/nlm/source
./test/TestNLMConfig
./test/TestNLMAnalyze
./test/TestNLMProfile
./test/TestNLMIntegrate
./test/TestNLMExport
```

### Troubleshooting

#### Common Issues

1. **Tool not found**: Ensure tools are built and in PATH
2. **Python dependencies missing**: Install required Python packages
3. **Configuration file not found**: Check file path and permissions
4. **Permission denied**: Check file and directory permissions
5. **Memory usage high**: Close other applications or increase system resources

#### Getting Help

- **Tool help**: `tool_name --help`
- **Python help**: `help(nlm.tools.ToolName)`
- **Documentation**: See `docs/tools/` for detailed documentation
- **Issues**: Report issues to the NLM project repository

### Acknowledgments

These tools are built upon the existing NLM framework and leverage:

- **NLM Configuration System**: Advanced configuration management
- **NLM Error Handling System**: Robust error handling and validation
- **NLM Input Validation System**: Comprehensive input validation
- **Phase 6 Integration Framework**: Seamless integration with Phase 6
- **Python Bindings**: Full Python integration via `pynlm`

### License

These tools are licensed under the MIT License. See `LICENSE` for details.

---

## Tool Summary

| Tool | Description | Languages | Key Features |
|------|-------------|-----------|--------------|
| **NLMConfig** | Configuration Management | C++, Python | Validation, Generation, Comparison, Analysis |
| **NLMAnalyze** | Network Analysis | C++, Python | Connectivity, Clustering, Topology, Performance |
| **NLMProfile** | Performance Profiling | C++, Python | Profiling, Memory Analysis, Benchmarking |
| **NLMIntegrate** | Integration Validation | C++, Python | Verification, Testing, Health Monitoring |
| **NLMExport** | Data Export | C++, Python | Export, Reporting, Data Management |

All tools are designed to be easy to use, powerful, extensible, and follow NLM standards while providing comprehensive support for Phase 6 integration and advanced analysis capabilities.