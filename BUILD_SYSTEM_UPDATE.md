# NLM Build System Update

This file documents the build system changes made to support the new features in the NLM project.

## Summary of Build Changes

The following files have been modified to support the new features:

### 1. CMakeLists.txt (Root)
- Added `src/core/HelpSystem.cpp` to nlm_core library
- Added `src/core/Profiler.cpp` to nlm_core library
- Added `src/core/ConfigGenerator.cpp` to nlm_core library
- Added `src/core/PowerUserTools.cpp` to nlm_core library
- Added CURL library dependency for remote execution
- Added `nlm_phase6_demo` to install targets

### 2. tests/CMakeLists.txt (Unchanged)
- Existing tests continue to work unchanged
- No modifications needed for new features

## Build Instructions

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with Debug
```bash
mkdir build
debug
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Install (Development)
```bash
cd build
make install
```

## New Executables

### Phase 6 Demo
```bash
./build/nlm_phase6_demo [options]
```

### Help Usage
```bash
./build/nlm_phase6_demo --help
./build/nlm --help
./build/nlm_phase3_demo --help
./build/nlm_phase4_demo --help
```

## Key New Features

1. **Help System**: Comprehensive help for all commands
2. **Profiler**: Performance profiling and debugging tools
3. **ConfigGenerator**: Configuration file management
4. **PowerUserTools**: Advanced features for power users
5. **Enhanced Phase6 Demo**: Full integration test with advanced options

## Dependencies

The project now requires:
- CMake 3.16 or higher
- C++20 compiler
- CURL library (for remote execution)
- (Optional) JSON library (simplified implementation included)

## Build Log

For detailed build logs and troubleshooting, refer to:
- `build/CMakeFiles/*.dir/build.make`
- `build/CMakeFiles/*.dir/build.log`

## Testing

Run the test suite:
```bash
cd build
ctest
```

## Notes

1. The build system automatically detects and includes all new features
2. Features can be enabled/disabled via CMake options (if needed)
3. All new features are included in the core library to ensure proper linking
4. The build system maintains backward compatibility with existing code

## Troubleshooting

### CURL Library Not Found
If CURL is not found during configuration, install it:

- Ubuntu/Debian: `sudo apt-get install libcurl4-openssl-dev`
- macOS: `brew install curl`
- CentOS/RHEL: `sudo yum install libcurl-devel`

### JSON Library Issues
The project includes a simplified JSON implementation. For better JSON support, consider installing:
- Ubuntu/Debian: `sudo apt-get install libjsoncpp-dev`
- macOS: `brew install jsoncpp`

### Build Fails
Check the build logs for detailed error messages. Common issues include:
1. Missing dependencies
2. Compiler version issues
3. CMake version requirements

## Future Build Updates

This build system is designed to be easily extensible. New features can be added by:

1. Adding new source files to the nlm_core library
2. Adding new executables to the CMakeLists.txt
3. Updating the build type (Debug/Release) options
4. Adding new dependencies as needed

The build system is now ready for production use and includes all the new features described in the improvements summary.