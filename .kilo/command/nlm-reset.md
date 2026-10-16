Reset NLM development environment to clean state

Reset the NLM development environment to a clean, reproducible state. This command removes build artifacts, temporary files, and cached data while preserving project configuration and source code. It's useful for:

1. **Development Cleanup**:
   - Remove compiled binaries and libraries
   - Clean build directories
   - Reset Python package cache
   - Clear temporary files

2. **Testing Preparation**:
   - Fresh environment for CI/CD
   - Clean state for reproducibility
   - Reset test caches and artifacts
   - Clear coverage reports

3. **Performance Optimization**:
   - Free disk space
   - Remove stale build outputs
   - Reset package managers
   - Clear logging buffers

Cleanup Operations:

1. **Build System Cleanup**:
   ```bash
   # Remove build artifacts
   rm -rf build/
   rm -rf *.o
   rm -rf *.a
   rm -rf *.so
   rm -rf *.dylib
   
   # Remove Python build artifacts
   rm -rf dist/
   rm -rf *.egg-info
   rm -rf __pycache__/
   rm -rf *.pyc
   ```

2. **Source Code Cleanup**:
   ```bash
   # Remove generated files
   rm -f docs/html/*
   rm -f coverage.xml
   rm -f reports/*
   
   # Clear caches
   rm -f .coverage
   rm -f .pytest_cache/
   rm -f .ruff_cache/
   ```

3. **Environment Reset**:
   ```bash
   # Reset Python virtual environment
   source venv/bin/deactivate 2>/dev/null || true
   
   # Clear pip cache
   pip cache purge
   
   # Remove temporary files
   rm -f /tmp/*.log
   rm -f /tmp/*.tmp
   ```

4. **Configuration Cleanup**:
   ```bash
   # Reset NLM configuration to defaults
   rm -f configs/*.user.cfg
   rm -f checkpoints/*.chk
   rm -f logs/*.log
   
   # Restore default configuration files
   git checkout configs/ 2>/dev/null || true
   ```

Cleanup Script Implementation:

The reset process includes:

1. **Build Directory Cleanup**:
   - Remove `build/` directory recursively
   - Clean CMake cache and generated files
   - Remove compiler output files
   - Clear linker artifacts

2. **Python Package Cleanup**:
   - Remove distribution builds
   - Clean Python module cache
   - Reset pip and setuptools caches
   - Clear virtual environment artifacts

3. **Documentation Cleanup**:
   - Remove generated HTML documentation
   - Clear API documentation builds
   - Remove LaTeX artifacts
   - Reset documentation caches

4. **Test and Benchmark Cleanup**:
   - Remove test reports
   - Clear coverage data
   - Reset benchmark results
   - Clean performance profiles

5. **Log and Temporary File Cleanup**:
   - Remove simulation logs
   - Clear temporary simulation data
   - Reset performance metrics
   - Clean up debug information

Verification Commands:

After cleanup, verify with:

```bash
# Check that source files remain
ls -la src/  # Should show only source files

# Check that build directory is empty or removed
ls -la build/ 2>/dev/null || echo "Build directory removed"

# Check that configuration files are preserved
ls -la configs/  # Should show config files

# Check that documentation structure remains
ls -la docs/  # Should show documentation source
```

Reset Modes:

Different reset levels available:

1. **Quick Reset** (`--level quick`):
   - Remove build artifacts
   - Clean Python cache
   - Reset temporary files

2. **Full Reset** (`--level full`):
   - Quick reset plus
   - Remove generated documentation
   - Clear test results
   - Reset environment variables

3. **Factory Reset** (`--level factory`):
   - Full reset plus
   - Restore configuration from git
   - Remove user-specific settings
   - Reset all customizations

Reset Examples:

```bash
# Quick cleanup for development
/nlm-reset --level quick

# Full cleanup for CI/CD
/nlm-reset --level full

# Factory reset for new development
/nlm-reset --level factory

# Reset with verification
/nlm-reset --level full --verify

# Reset with custom cleanup script
/nlm-reset --script custom_cleanup.sh
```

Reset Safety Features:

- **Backup Protection**: Configuration files are backed up before modification
- **Version Control Integration**: Respects git repository state
- **Selective Cleanup**: Can target specific file types or directories
- **Rollback Capability**: Can restore from backup if needed

Reset Statistics:

The command provides detailed statistics:
- Files removed and their sizes
- Space reclaimed
- Reset duration
- Verification results

This reset functionality ensures a clean development environment while preserving the essential project configuration and source code.