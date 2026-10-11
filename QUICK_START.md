# NLM Advanced Tools - Quick Start Guide

## Overview

This guide provides quick access to the five NLM advanced command-line tools for configuration management, network analysis, performance profiling, integration validation, and data export.

## Available Tools

### 1. NLMConfig - Configuration Management

**Quick Start:**
```bash
# Validate configuration
nlmconfig --validate my_config.cfg

# Generate configuration
nlmconfig --generate my_config.cfg

# Compare configurations
nlmconfig --compare config1.cfg config2.cfg

# Analyze parameters
nlmconfig --analyze config.cfg

# Export as JSON
nlmconfig --export-json config.json config.cfg
```

**Key Features:**
- Configuration validation (JSON/YAML/CFG)
- Configuration generation from templates
- Configuration comparison and diff
- Parameter sensitivity analysis
- Integration with NLM configuration system

---

### 2. NLMAnalyze - Network Analysis

**Quick Start:**
```bash
# Analyze network connectivity
nlmanalyze --connectivity my_config.cfg

# Analyze clustering
nlmanalyze --clustering my_config.cfg

# Analyze topology
nlmanalyze --topology my_config.cfg

# Analyze performance
nlmanalyze --performance my_config.cfg

# Run all analyses
nlmanalyze --all my_config.cfg

# Save analysis report
nlmanalyze --connectivity --output analysis.txt my_config.cfg
```

**Key Features:**
- Network connectivity analysis
- Community detection and clustering
- Network topology and graph metrics
- Performance metrics calculation
- Integration with NLM brain analysis

---

### 3. NLMProfile - Performance Profiling

**Quick Start:**
```bash
# Profile performance
nlmprofile --performance 60 my_config.cfg

# Profile memory usage
nlmprofile --memory my_config.cfg

# Detect memory leaks
nlmprofile --memory-leak 30 my_config.cfg

# Detect bottlenecks
nlmprofile --bottlenecks my_config.cfg

# Benchmark configuration
nlmprofile --benchmark "neuron_count=500,region_count=1" my_config.cfg

# Run all profiling
nlmprofile --all my_config.cfg

# Save profiling report
nlmprofile --performance 30 --output profile.txt my_config.cfg
```

**Key Features:**
- Performance profiling (CPU, memory, I/O)
- Memory usage analysis and leak detection
- Performance bottleneck detection
- Configuration benchmarking
- Performance optimization recommendations

---

### 4. NLMIntegrate - Integration Validation

**Quick Start:**
```bash
# Verify Phase 6 integration
nlmintegrate --verify my_config.cfg

# Run quick integration test
nlmintegrate --quick-test my_config.cfg

# Run comprehensive integration test
nlmintegrate --comprehensive-test my_config.cfg

# Run all integration tests
nlmintegrate --all-tests my_config.cfg

# Perform health check
nlmintegrate --health-check my_config.cfg

# Generate integration report
nlmintegrate --generate-report integration_report.txt my_config.cfg
```

**Key Features:**
- Phase 6 integration verification
- Integration testing automation
- Integration validation and reporting
- Integration health monitoring
- Integration with Phase 6 experiment framework

---

### 5. NLMExport - Data Export

**Quick Start:**
```bash
# Export configuration
nllexport --export-config config.json json my_config.cfg

# Export brain data
nllexport --export-brain-data brain_data.json json my_config.cfg

# Export simulation results
nllexport --export-simulation-results results.json my_config.cfg

# Export analysis data
nllexport --export-analysis-data analysis.json my_config.cfg

# Export all data to directory
nllexport --export-all output_data my_config.cfg
```

**Key Features:**
- Brain state export (JSON, binary, etc.)
- Configuration export
- Simulation results export
- Analysis data export
- Integration with NLM data management

---

## Installation and Setup

### Prerequisites
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

## Usage Examples

### Example 1: Complete Analysis Workflow

```bash
# 1. Validate and enhance configuration
nlmconfig --validate my_config.cfg
nlmconfig --generate enhanced_config.cfg

# 2. Analyze network connectivity
nlmanalyze --connectivity enhanced_config.cfg
nlmanalyze --clustering enhanced_config.cfg
nlmanalyze --topology enhanced_config.cfg
nlmanalyze --performance enhanced_config.cfg

# 3. Profile performance
nlmprofile --performance 60 enhanced_config.cfg
nlmprofile --memory enhanced_config.cfg
nlmprofile --bottlenecks enhanced_config.cfg

# 4. Validate integration
nlmintegrate --verify enhanced_config.cfg
nlmintegrate --comprehensive-test enhanced_config.cfg
nlmintegrate --health-check enhanced_config.cfg

# 5. Export data
nllexport --export-config config.json json enhanced_config.cfg
nllexport --export-brain-data brain_data.json json enhanced_config.cfg
nllexport --export-all exported_data enhanced_config.cfg

# 6. Generate reports
nlmanalyze --connectivity --output analysis.txt enhanced_config.cfg
nlmprofile --performance 60 --output profile.txt enhanced_config.cfg
nlmintegrate --generate-report integration_report.txt enhanced_config.cfg
```

### Example 2: Python API Usage

```python
import nlm.tools

# Create tool instances
config_tool = nlm.tools.NLMConfig()
analyze_tool = nlm.tools.NLMAnalyze()
profile_tool = nlm.tools.NLMProfile()
integrate_tool = nlm.tools.NLMIntegrate()
export_tool = nlm.tools.NLMExport()

# Load configuration
config_tool.load_config("my_config.cfg")

# Run analysis
results = analyze_tool.analyze_connectivity()

# Profile performance
metrics = profile_tool.profile_performance(60)

# Validate integration
verification = integrate_tool.verify_integration()

# Export data
export_tool.export_brain_data("brain_data.json", brain_data)
```

## Configuration File Support

All tools support multiple configuration file formats:

### Simple Format (.cfg)
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
true_value = true
false_value = false
```

### JSON Format (.json)
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "log_level": "INFO",
  "true_value": true,
  "false_value": false
}
```

### YAML Format (.yaml)
```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.1
log_level: INFO
true_value: true
false_value: false
```

## Output Formats

### Text Reports
Human-readable text reports for interactive use.

### JSON Output
Structured JSON output for machine processing and integration.

### Binary Formats
Efficient binary formats for performance-critical applications.

### File Output
Support for saving outputs to files with customizable names and directories.

## Testing

### Tool Tests
```bash
# Run individual tool tests
./test/TestNLMConfig
./test/TestNLMAnalyze
./test/TestNLMProfile
./test/TestNLMIntegrate
./test/TestNLMExport
```

### Python Tests
```bash
# Run Python tool tests
python3 -m pytest tests/test_nlmconfig.py
python3 -m pytest tests/test_nlmanalyze.py
python3 -m pytest tests/test_nlmprofile.py
python3 -m pytest tests/test_nlmintegrate.py
python3 -m pytest tests/test_nlmexport.py
```

### Integration Tests
```bash
# Run complete integration test
./scripts/test_all_tools.sh

# Run integration workflow
./scripts/integration_workflow.sh
```

## Scripts and Automation

### Available Scripts
- `scripts/test_nlmconfig.sh` - Test NLMConfig tool
- `scripts/test_nlmanalyze.sh` - Test NLMAnalyze tool
- `scripts/test_nlmprofile.sh` - Test NLMProfile tool
- `scripts/test_nlmintegrate.sh` - Test NLMIntegrate tool
- `scripts/test_nlmexport.sh` - Test NLMExport tool
- `scripts/test_all_tools.sh` - Test all tools
- `scripts/integration_workflow.sh` - Complete integration workflow

### Workflow Examples
- `scripts/integration_workflow.sh` - Complete workflow example
- `scripts/test_all_tools.sh` - Quick test suite
- `scripts/test_nlmconfig.sh` - NLMConfig testing

## Phase 6 Integration Support

All tools are specifically designed to support Phase 6 integration:

### Integration Verification
- `nlmintegrate --verify` validates Phase 6 integration
- `nlmintegrate --comprehensive-test` runs complete integration tests

### Configuration Management
- `nlmconfig` manages Phase 6 configuration
- Supports Phase 6 specific parameters
- Integration with NLM configuration system

### Performance Analysis
- `nlmanalyze` analyzes integrated system performance
- `nlmprofile` profiles Phase 6 system performance
- Optimization recommendations for integrated systems

### Data Export
- `nllexport` exports Phase 6 system data
- Supports various export formats
- Integration with NLM data management

## Tool Summary

| Tool | Primary Purpose | Languages | Key Features |
|------|-----------------|-----------|--------------|
| **NLMConfig** | Configuration Management | C++, Python | Validation, Generation, Comparison, Analysis |
| **NLMAnalyze** | Network Analysis | C++, Python | Connectivity, Clustering, Topology, Performance |
| **NLMProfile** | Performance Profiling | C++, Python | Profiling, Memory Analysis, Benchmarking |
| **NLMIntegrate** | Integration Validation | C++, Python | Verification, Testing, Health Monitoring |
| **NLMExport** | Data Export | C++, Python | Export, Reporting, Data Management |

## Getting Help

### Tool Help
```bash
# Get help for any tool
nlmconfig --help
nlmanalyze --help
nlmprofile --help
nlmintegrate --help
nllexport --help
```

### Python Help
```python
import nlm.tools

# Get help for any tool
help(nlm.tools.NLMConfig)
help(nlm.tools.NLMAnalyze)
help(nlm.tools.NLMProfile)
help(nlm.tools.NLMIntegrate)
help(nlm.tools.NLMExport)
```

### Documentation
- See `docs/tools/` for detailed documentation
- Check `README.md` in each tool directory for specific information
- Review `ToolsReadme.md` for comprehensive guide

## Troubleshooting

### Common Issues

1. **Tool not found**
   - Ensure tools are built and in PATH
   - Check installation and build processes

2. **Python dependencies missing**
   - Install required packages: `pip install psutil numpy pyyaml`
   - Update Python environment if needed

3. **Configuration file not found**
   - Check file path and permissions
   - Verify file format and structure

4. **Permission denied**
   - Check file and directory permissions
   - Run with appropriate privileges

5. **Memory usage high**
   - Close other applications
   - Increase system resources if needed

### Getting Support

- **Documentation**: Check `docs/tools/` directory
- **Issues**: Report issues to the NLM project repository
- **Community**: Join NLM community forums and discussions
- **Examples**: Use provided script examples for guidance

## Acknowledgments

These tools are built upon the existing NLM framework and leverage:

- **NLM Configuration System**: Advanced configuration management
- **NLM Error Handling System**: Robust error handling and validation
- **NLM Input Validation System**: Comprehensive input validation
- **Phase 6 Integration Framework**: Seamless integration with Phase 6
- **Python Bindings**: Full Python integration via `pynlm`

## License

These tools are licensed under the MIT License. See `LICENSE` for details.

---

**Version:** 1.0
**Author:** NLM Project Team
**Date:** 2026-10-11
**Phase:** 6 - Final Integration

All tools are production-ready and designed to work seamlessly with the existing NLM framework while providing advanced functionality for configuration management, network analysis, performance profiling, integration validation, and data export.
