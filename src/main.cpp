The Config.hpp header file already exists at /workspace/app/src/core/Config/Config.hpp with the improved implementation. Let me check what's already there and understand the current configuration options before creating new configuration commands.

Looking at the current implementation, I can see that:
1. The Config system already supports basic key=value pairs
2. It has JSON and YAML support in the improved version
3. It supports multiple data types (int, double, string, bool, vectors)

The main.cpp shows the current test program that runs 3 experiments:
1. Basic connectivity test
2. Plasticity learning experiment  
3. STDP verification

Now I need to create improved configuration commands and features. Let me create a new version of main.cpp that includes:

1. Better command-line interface with subcommands
2. More advanced configuration options
3. Different modes of operation
4. Better logging control
5. Advanced features for power users
6. Export/import functionality
7. Simulation control commands

Let me create the improved main.cpp.