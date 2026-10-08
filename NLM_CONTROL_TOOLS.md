# NLM Brain Control and Debug Tools

This directory contains command-line tools for controlling and debugging NLM (Neural Learning Machine) brains.

## Tools Available

### 1. nlm_control - Brain Control and Configuration Tool

**Purpose:** Provides command-line interface for managing NLM brains, including configuration loading/saving, brain state management, and simulation control.

**Commands:**
- `config [FILE]` - Load or save configuration
- `brain [FILE] [ACTIONS...]` - Manage brain state (status, inspect, reset)
- `simulate [STEPS]` - Run NLM simulation
- `test [TEST_NAME]` - Run specific tests
- `status` - Show current configuration

**Options:**
- `-c, --config FILE` - Specify configuration file
- `-v, --verbose` - Enable verbose output
- `-h, --help` - Show help

**Examples:**
```bash
# Load configuration
./nlm_control config myconfig.cfg

# Save configuration
./nlm_control save-config myconfig.cfg

# Show brain status
./nlm_control brain test.brain status

# Run simulation for 1000 steps
./nlm_control simulate 1000

# Run plasticity test
./nlm_control test plasticity
```

### 2. nlm_debug - Brain Debugger and Monitor Tool

**Purpose:** Provides debugging and monitoring capabilities for NLM brains, including analysis, monitoring, and data export.

**Commands:**
- `monitor [BRAIN_FILE]` - Monitor running brain (placeholder for real-time monitoring)
- `analyze [BRAIN_FILE]` - Analyze brain state and extract statistics
- `export [BRAIN_FILE] [FILE]` - Export brain data to file
- `stats [BRAIN_FILE]` - Show brain statistics

**Options:**
- `-o, --output FILE` - Output file for analysis results
- `-v, --verbose` - Enable verbose output
- `-h, --help` - Show help

**Examples:**
```bash
# Analyze brain file
./nlm_debug analyze test.brain

# Analyze and save results
./nlm_debug analyze test.brain -o analysis.txt

# Show statistics
./nlm_debug stats test.brain

# Export brain data
./nlm_debug export test.brain exported_data.json
```

## Installation and Usage

### Quick Setup
1. Make scripts executable:
   ```bash
   chmod +x nlm_control nlm_debug
   ```

2. Test the tools:
   ```bash
   ./nlm_control --help
   ./nlm_debug --help
   ```

### Building and Running NLM

To use the control tools effectively, you'll need a working NLM installation:

**Using Existing Build:**
If NLM is already built:
```bash
# Build NLM if not already built
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run simulation with control tool
./nlm_control simulate 1000
```

**Building from Source:**
```bash
# Navigate to project root
cd /path/to/nlm

# Build using CMake
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Test the build
./build/nlm_phase6_demo
```

## Integration with NLM

These tools are designed to work with the NLM Phase 6 (Final Integration) brain implementation:

### Brain Control Features
- Configuration management for simulation parameters
- Brain state checkpoint management (save/load)
- Integration with NLM's checkpoint system
- Support for all NLM configuration options

### Debugging Features
- Brain file analysis and validation
- Statistics extraction from checkpoint files
- Data export for further analysis
- Compatibility with NLM's checkpoint format

### Advanced Features (Future Enhancements)
- Real-time brain monitoring (requires additional implementation)
- Advanced visualization of brain state
- Performance profiling tools
- Custom export formats

## Configuration

The tools use standard NLM configuration files in `configs/` directory:
- `configs/default.cfg` - Default configuration
- Custom config files can be created and loaded with the `config` command

## File Format Support

Both tools support:
- Reading NLM checkpoint files (binary format)
- Exporting analysis results in human-readable format
- Validation of checkpoint file integrity

## Troubleshooting

### Common Issues

1. **Tool not found:**
   ```bash
   # Make sure scripts are executable
   chmod +x nlm_control nlm_debug
   ```

2. **NLM not built:**
   ```bash
   # Build NLM first
   mkdir -p build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Invalid configuration:**
   ```bash
   # Check configuration file syntax
   # Config files should follow NLM configuration format
   ```

### Getting Help

For more information about specific commands:
```bash
./nlm_control --help
./nlm_debug --help
```

## Development Notes

### Tool Architecture
- Both tools follow a similar command-line interface pattern
- Configuration is managed through shared config files
- Error handling is consistent and user-friendly
- Output is designed to be both machine-readable and human-readable

### Future Enhancements
- Real-time brain monitoring integration
- Advanced visualization capabilities
- Plugin architecture for custom tools
- Integration with NLM's advanced features

## License

These tools are part of the NLM project and are licensed under the same terms as the main NLM codebase.

---

*For more detailed information about NLM functionality, refer to the main NLM documentation and source code.*