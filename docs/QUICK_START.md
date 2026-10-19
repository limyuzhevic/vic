# NLM Phase 6 Quick Start Guide
# Neural Learning Machine - Final Integration

This guide provides the fastest way to get started with NLM Phase 6 and run your first simulation.

## Quick Start (Under 5 Minutes)

### 1. Install Dependencies

```bash
# Check for required tools
gcc --version || clang --version || echo "No C++ compiler found"
cmake --version || echo "CMake not found"
make --version || echo "Make not found"
```

### 2. Build NLM

**Option A: Use the setup script (recommended)**
```bash
chmod +x setup.sh
./setup.sh quick
```

**Option B: Manual build**
```bash
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
```

### 3. Run Your First Simulation

```bash
# Integration demo (shows all subsystems working)
./nlm --experiment demo

# Quick plasticity experiment
./nlm --experiment plasticity --steps 1000

# Performance benchmark
./nlm --benchmark
```

## What You'll See

### Sample Output

```
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 6: FINAL INTEGRATION                               ║
║                                                               ║
║     Complete artificial brain with all subsystems integrated.   ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

=============================================================
NLM Phase 6 System Information
=============================================================

Core Components:
• Real LIF neuron dynamics with event-driven spike propagation
• Working memory with persistent activity and competition
• Episodic memory with experience encoding and replay
• Associative memory with Hebbian pattern associations
• Neuromodulation (dopamine, curiosity, novelty, prediction error)

Cognitive Systems:
• Neural planner with action sequence evaluation
• Concept formation from experience patterns
• Attention with competitive selection dynamics

Integration Features:
• Prediction system integrated into brain loop
• Development stages affecting plasticity rates
• Checkpoint save/load implemented
• Replay and consolidation during rest/sleep

=============================================================
Running NLM Phase 6 Integration Demo...
This tests the complete integrated brain system.

=== Test 1: Basic Neural Connectivity ===
  ✓ Basic connectivity working

=== Test 2: Memory System Integration ===
  ✓ Memory systems integrated and operational

=== Test 3: Neuromodulation Integration ===
  ✓ Neuromodulation integrated

=== Test 4: Cognitive Systems Integration ===
  ✓ Cognitive systems integrated

=== Test 5: Prediction System Integration ===
  ✓ Prediction system integrated

=============================================================
=== Final Brain Status ===
Total neurons: 500
Total synapses: 12345
Sensory neurons: 125
Motor neurons: 100
Working memory capacity: 50/50
Episodic memory: 0 episodes
Associative memory: 0 patterns

=============================================================
=== Phase 6 Complete ===

Phase 6 integration demo completed successfully!

The NLM brain is now a fully functioning artificial neural system
with all subsystems integrated and operational.

=============================================================
```

## Command Line Options

### Essential Commands

| Command | Purpose |
|---------|---------|
| `./nlm --help` | Show help and usage information |
| `./nlm --version` | Show version information |
| `./nlm --experiment demo` | Run complete integration demo |
| `./nlm --benchmark` | Run performance benchmark |

### Configuration Examples

```bash
# Run with custom configuration
./nlm --config configs/basic.cfg --verbose

# Run for specific number of steps
./nlm --experiment demo --steps 5000

# Save brain state
./nlm --experiment demo --save-brain brain_state.bin

# Load and continue
./nlm --experiment demo --load-brain brain_state.bin
```

## Configuration Files

### Pre-configured Files

1. **`configs/default.cfg`** - Complete Phase 6 configuration
2. **`configs/basic.cfg`** - Simplified configuration for quick testing

### Create Custom Configuration

```bash
# Copy template to create custom config
cp configs/custom_template.cfg my_config.cfg

# Edit my_config.cfg with your settings
# (You can use any text editor)

# Run with custom config
./nlm --config my_config.cfg --verbose
```

## Troubleshooting Common Issues

### Problem: "No such file or directory" for `./nlm`

**Solution:** Build first:
```bash
./setup.sh quick
# or manually:
mkdir build && cd build && cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
cd ..
```

### Problem: Slow simulation performance

**Solutions:**
```bash
# Use faster configuration
./nlm --config configs/basic.cfg --steps 1000

# Disable optimizations for faster development
./nlm --benchmark --verbose --use-simd=false
```

### Problem: Configuration errors

**Check your config file:**
```bash
# View default config
cat configs/default.cfg | head -20

# Fix syntax errors (must be "key = value" format)
```

### Problem: No output or crashes

**Solutions:**
```bash
# Add verbose flag
./nlm --experiment demo --verbose

# Check logs
./nlm --experiment demo --log-level DEBUG

# Use minimal configuration
./nlm --config configs/basic.cfg
```

## Getting Help

### Online Resources

- **Documentation:** `docs/NLM_Phase6_User_Guide.md`
- **GitHub:** https://github.com/nlm-project/nlm
- **Issues:** Report bugs at GitHub issues
- **Discussions:** Ask questions at GitHub discussions

### Local Resources

- **Help:** `./nlm --help`
- **Version:** `./nlm --version`
- **Examples:** Look in `examples/` directory
- **Tests:** Run unit tests with `./nlm --test`

## Advanced Usage

### Configuration Override

Command line arguments override configuration file values:

```bash
# This overrides neuron_count from configs/default.cfg
./nlm --config configs/default.cfg --steps 10000 --neuron_count 1000
```

### Performance Monitoring

```bash
# Run with detailed logging
./nlm --benchmark --verbose --log-level DEBUG

# Monitor system resources during simulation
# Use: top, htop, or your system's task manager
```

### Development Mode

```bash
# Build with debug symbols
./setup.sh full

# Run with debugger
./nlm --experiment demo --verbose

# Use GDB for debugging
make run  # if using Makefile
```

## Examples Directory

### Example 1: Basic Usage

```bash
# Create a simple simulation script
scat > run_example.sh << 'EOF'
#!/bin/bash

echo "Running NLM Phase 6 example..."

# Build if not already built
if [ ! -f "build/nlm" ]; then
    echo "Building NLM..."
    ./setup.sh quick
fi

# Run example simulation
echo "Running integration demo..."
./nlm --experiment demo --steps 1000

echo "Example completed successfully!"
EOF

chmod +x run_example.sh
./run_example.sh
```

### Example 2: Automated Testing

```bash
# Create a test script
cat > test_nlm.sh << 'EOF'
#!/bin/bash

echo "Running NLM Phase 6 tests..."

# Test 1: Basic functionality
./nlm --experiment demo --steps 100 --quiet

# Test 2: Plasticity
./nlm --experiment plasticity --steps 1000

# Test 3: STDP
./nlm --experiment stdp

# Test 4: Benchmark
./nlm --benchmark --quiet

echo "All tests passed!"
EOF

chmod +x test_nlm.sh
./test_nlm.sh
```

## Next Steps

### After Running Examples

1. **Explore the code:** Look at `src/` directory for implementation details
2. **Modify configurations:** Create your own `configs/` files
3. **Extend functionality:** Add new experiments or subsystems
4. **Contribute:** Share your improvements with the community

### For Researchers

1. **Study the architecture:** Read `docs/NLM_Phase6_User_Guide.md`
2. **Experiment with configurations:** Modify `configs/default.cfg`
3. **Run benchmarks:** Use `./nlm --benchmark` for performance data
4. **Document your findings:** Add to the documentation

### For Developers

1. **Join the project:** Visit https://github.com/nlm-project/nlm
2. **Submit pull requests:** Add new features or fixes
3. **Improve documentation:** Update guides and examples
4. **Add tests:** Increase test coverage and integration tests

## System Requirements

### Minimum Requirements

- **CPU:** 2 cores (4 recommended)
- **RAM:** 2 GB (4 GB recommended)
- **Storage:** 1 GB free space
- **Compiler:** C++17 compatible
- **Build Tools:** CMake 3.16+, Make

### Recommended for Production

- **CPU:** 8+ cores
- **RAM:** 8+ GB
- **Storage:** SSD (faster I/O)
- **Compiler:** GCC 9+ or Clang 10+
- **Build Tools:** CMake latest stable

### Performance Tips

1. **Use SSD:** Faster disk I/O improves performance
2. **Multiple threads:** Enable parallel processing (`-DUSE_PARALLEL=ON`)
3. **Memory optimization:** Use memory pools (`-DUSE_MEMORY_POOLS=ON`)
4. **SIMD instructions:** Enable vectorization (`-DUSE_SIMD=ON`)
5. **Configuration tuning:** Adjust parameters for your use case

---

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

---

## Copyright

© 2024 NLM Project. All rights reserved.

---

*This guide is continuously updated. For the latest version, visit the project documentation.*
