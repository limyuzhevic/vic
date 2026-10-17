# CLI Framework Quick Start

## 1-Second Demo

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run help
./nlm --help

# Run demo
./nlm

# Run Phase 3 demo  
./nlm demo --demo-type phase3

# Run benchmark
./nlm benchmark
```

## Key Features

✅ **Modern CLI**: Clean argument parsing with --help, --version
✅ **Multiple Commands**: demo, benchmark, script, test, validate  
✅ **Config Support**: JSON, YAML, key=value files
✅ **Auto-format Detection**: Knows file type from extension/content
✅ **Backward Compatible**: Old --config flag still works
✅ **Comprehensive Help**: Detailed usage and examples
✅ **Logging Options**: --log-level, --log-file, --verbose/--quiet

## Example Commands

### Basic Usage
```bash
# Phase 6 integration test (default)
./nlm

# Phase 3 world interaction demo  
./nlm demo --demo-type phase3

# Phase 4 emerging cognition demo
./nlm demo --demo-type phase4

# Phase 6 integration demo
./nlm demo --demo-type phase6

# Performance benchmarks
./nlm benchmark

# Configuration validation
./nlm --config myconfig.cfg validate
```

### Advanced Usage
```bash
# Run with custom settings
./nlm demo --demo-type phase3 --episodes 10 --steps 500

# Run benchmark with specific config
./nlm benchmark --benchmark-config 500,1000

# Save benchmark results
./nlm benchmark --benchmark-config 1000,500 --benchmark-output results.txt

# Debug logging
./nlm --log-level debug demo

# Quiet mode (no output)
./nlm --quiet benchmark
```

### Configuration Files

**Simple key-value (`.cfg`):**
```
# config.cfg
random_seed = 42
neuron_count = 1000
log_level = INFO
```

**JSON (`.json`):**
```json
{
  "random_seed": 42,
  "neuron_count": 1000,
  "log_level": "INFO"
}
```

**YAML (`.yaml`):**
```yaml
# config.yaml
random_seed: 42
neuron_count: 1000
log_level: INFO
```

### Using Configs
```bash
# Load configuration and run
./nlm --config config.cfg demo

# Validate configuration
./nlm --config config.json validate
```

## Getting Help

```bash
# General help
./nlm --help

# Command help
./nlm demo --help
./nlm benchmark --help
./nlm validate --help

# Version info
./nlm --version
```

## Migration Guide

**Old way:**
```bash
# Phase 3 demo
./nlm_phase3_demo 10 200

# With config
./nlm_phase3_demo --config configs/default.cfg
```

**New way:**
```bash
# Phase 3 demo
./nlm demo --demo-type phase3 --episodes 10 --steps 200

# With config
./nlm --config configs/default.cfg demo
```

## What This Framework Provides

1. **Unified CLI**: All NLM executables use the same interface
2. **Modern Features**: Short/long options, value parsing, subcommands
3. **Rich Documentation**: Built-in help with examples
4. **Flexible Config**: Multiple file formats with auto-detection
5. **Backward Compatible**: Old scripts still work
6. **Extensible**: Easy to add new commands and options
7. **Production Ready**: Robust error handling and validation

## Next Steps

1. **Build the project** (see above)
2. **Run `./nlm --help`** to explore all options
3. **Try `./nlm demo --demo-type phase3`** for a quick demo
4. **Explore `./nlm benchmark`** for performance testing
5. **Test `./nlm --config myconfig.cfg validate`** for config validation

The CLI framework makes it easy to run any NLM demo, benchmark, or validation with a clean, intuitive interface!
