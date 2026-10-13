# Test CommandLineInterface

Test the CommandLineInterface with a simple demo program.

```cpp
#include <iostream>
#include "core/cli/CommandLineInterface.hpp"

int main() {
    std::cout << "NLM CommandLineInterface Test" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Create CommandLineInterface instance
    nlm::CommandLineInterface cli;
    
    // Initialize with minimal arguments
    int argc = 2;
    char* argv[] = {
        (char*)"nlm",
        (char*)"--help"
    };
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Failed to initialize CommandLineInterface" << std::endl;
        return 1;
    }
    
    std::cout << "CommandLineInterface initialized successfully" << std::endl;
    
    // Show help
    std::cout << "\nHelp:" << std::endl;
    std::cout << cli.getHelp() << std::endl;
    
    // Show status
    std::cout << "\nStatus:" << std::endl;
    std::cout << cli.getStatus() << std::endl;
    
    // Shutdown
    cli.shutdown();
    
    std::cout << "\nTest completed successfully" << std::endl;
    
    return 0;
}
```

Build and run:
```bash
mkdir build
cd build
cmake ..
cmake --build .
./demo_cli
```

Expected output:
```
NLM CommandLineInterface Test
============================
CommandLineInterface initialized successfully

Help:
NLM CommandLineInterface Help
============================

Available commands:
  help
  version
  config
  simulate
  run
  export
  import
  status
  benchmark
  performance
  logging
  visualization
  experiment
  batch
  compare
  pause
  resume
  step
  stop
  reset
  script
  load
  save
  history
  clear
  exit
  quit

Status:
NLM CommandLineInterface Status
================================
Interactive Mode: Disabled
Scripting Mode: Disabled
Simulation Running: No
Simulation Paused: No
Performance Monitoring: Disabled
Thread Safe: Yes
Uptime: 0.00 seconds
Commands Executed: 0
Errors Encountered: 0

Test completed successfully
```
