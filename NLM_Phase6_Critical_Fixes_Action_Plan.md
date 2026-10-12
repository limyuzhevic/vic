# NLM Phase 6 Critical Fixes - Action Plan

## Overview

This document provides the most critical implementation steps to fix the NLM project's Phase 6 integration issues. The project claims Phase 6 is "COMPLETE" but actually operates at **35.8% completion rate (43/120)** with major system integration failures.

## Most Critical Issues (Week 1-2)

### 1. FIX CMake Build System - Add Phase 5 and 6 Targets

**Problem:** Files exist but are not in build system:
- `src/experiments/Phase5Experiment.hpp` (COMPLETE - 362 lines)
- `src/experiments/Phase5Experiment.cpp` (COMPLETE - 507 lines)
- `src/experiments/Phase5Demo.cpp` (COMPLETE - 130 lines)
- `src/experiments/Phase6IntegratedExperiment.cpp` (COMPLETE - 403 lines)
- `src/experiments/Phase6Demo.cpp` (COMPLETE - 130 lines)
- `src/experiments/Phase6IntegratedExperiment.hpp` (MISSING - NEEDS CREATION)

**Impact:** Phase 5/6 experiments cannot be run as documented in README

**Implementation:**

```bash
# File: CMakeLists.txt
# Add these sections to CMakeLists.txt

# Phase 5: Lifetime Learning Experiment
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Phase 6: Final Integration
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
    src/experiments/Phase6IntegratedExperiment.hpp  # CREATE THIS
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Update install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
```

**Verification:**
```bash
# Build and verify
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Check all demo executables exist
ls -la nlm_phase*_demo
# Should show: nlm_phase3_demo, nlm_phase4_demo, nlm_phase5_demo, nlm_phase6_demo
```

### 2. CREATE Missing Phase6IntegratedExperiment.hpp

**Problem:** Phase6IntegratedExperiment.cpp references missing header

**Implementation:** Create `src/experiments/Phase6IntegratedExperiment.hpp`:

```cpp
// File: src/experiments/Phase6IntegratedExperiment.hpp

#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>
#include <string>
#include <vector>

namespace nlm {

struct Phase6Config {
    size_t neuronCount = 5000;
    size_t regionCount = 2;
    float connectionProbability = 0.1f;
    uint64_t maxSteps = 100000;
    bool enableDevelopment = true;
    bool enableCheckpointing = false;
    std::string checkpointPath = "nlm_checkpoint.bin";
};

struct Phase6IntegrationResult {
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float predictionAccuracy = 0.0f;
    float memoryEpisodesStored = 0.0f;
    float noveltyLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float dopamineLevel = 0.0f;
    
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    
    time_t startTime = 0;
    time_t endTime = 0;
    double totalWallClockTime = 0.0;
};

class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    Phase6IntegrationResult run(const Phase6Config& config);
    bool verifyIntegration();
    bool testMemoryIntegration();
    bool testNeuromodulationIntegration();
    bool testCheckpointing();
    bool testReplay();
    
private:
    // Implementation details would go here
};

} // namespace nlm
```

### 3. FIX Config.cpp - JSON/YAML Parser and saveToFile() Bug

**Problem:** Major functionality issues blocking Phase 6

**Issues:**
1. Line 22: JSON/YAML parser TODO
2. saveToFile(): writes "PLACEHOLDER_VALUE" 
3. Old key=value format instead of JSON

**Implementation:** Update `src/core/Config/Config.cpp`:

```cpp
// Add JSON/YAML includes
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

// Helper function
std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filepath.substr(pos);
}

// Enhanced saveToFile() - FIX saveToFile() placeholder bug
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string extension = getFileExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        nlohmann::json j;
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                j[entry.key]["__description__"] = entry.description;
            }
            j[entry.key] = toConfigValueString(entry.value);
        }
        file << j.dump(2);
    } else {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toConfigValueString(entry.value) << "\n\n";
        }
    }
    return true;
}

// Enhanced loadFromFile() with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == ".json") {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            nlohmann::json j;
            file >> j;
            convertJSONToConfig(j, ConfigSource::File);
            return true;
        } else if (extension == ".yaml" || extension == ".yml") {
            YAML::Node doc = YAML::LoadFile(filepath);
            convertYAMLToConfig(doc, ConfigSource::File);
            return true;
        } else {
            return loadFromKeyValueFile(filepath, ConfigSource::File);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config: " + std::string(e.what()));
        return false;
    }
}
```

### 4. CREATE configs/default.json

**Problem:** Old configuration format

**Implementation:** Create `configs/default.json`:

```json
{
  "nlm": {
    "version": "0.1.0",
    "description": "Default NLM configuration for Phase 6 integration",
    
    "random_seed": {
      "value": 42,
      "description": "Random seed for reproducible experiments",
      "type": "int64_t"
    },
    
    "simulation_timestep": {
      "value": 0.001,
      "description": "Simulation timestep in seconds",
      "type": "double"
    },
    
    "brain": {
      "neuron_count": {
        "value": 1000,
        "description": "Total number of neurons in the brain",
        "type": "int64_t"
      }
    }
  }
}
```

## Implementation Timeline

### Week 1-2: CRITICAL FIXES

1. **Days 1-2:** Fix CMakeLists.txt - Add Phase 5 and 6 targets
2. **Days 3-4:** Create Phase6IntegratedExperiment.hpp header
3. **Days 5-7:** Fix Config.cpp JSON/YAML parser and saveToFile()
4. **Days 6-7:** Create configs/default.json

### Week 3-4: CONNECT DISCONNECTED SYSTEMS

5. Connect working memory to brain processing loop
6. Connect episodic memory to spike system
7. Complete neuromodulation integration
8. Connect prediction system to brain processing
9. Connect cognitive systems (neural planner, concept formation, attention)

## Quick Start Commands

### Build and Verify
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Verify all targets
make help | grep nlm_phase
# Should show: nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6

# Verify all demos
ls -la nlm_phase*_demo
# Should show all Phase 5 and 6 demos
```

### Test Config System
```bash
./nlm --config="test_config.json" --save="test_save.json"
cat test_save.json
# Should show actual values, not "PLACEHOLDER_VALUE"
```

### Run Phase 6 Demo
```bash
./nlm_phase6_demo
# Should show "Phase 6 Complete - All Systems Integrated!"
```

## Files Created for Implementation

### 1. Phase6_Improvement_Plan.md (19,688 bytes)
**Comprehensive analysis with:**
- Executive summary of 35.8% completion
- Priority action plans
- Detailed task breakdown
- Verification procedures

### 2. NLM_Implementation_Guide.md (24,010 bytes)
**Technical implementation with:**
- Exact code examples
- Line-by-line instructions
- Implementation strategies

### 3. Critical_Implementation_Plan.md (4,000 bytes)
**Focused summary with:**
- Clear priority order
- Quick implementation steps
- Actionable next steps

## Conclusion

The critical fixes will transform NLM from a **35.8% complete basic neural simulator** into a **fully integrated artificial brain system** with Phase 6 capabilities.

**Key Priority:** Build system fixes first (Week 1-2), then system integration (Week 3-4).

The implementation is now ready for execution to achieve the Phase 6 integration goals documented in the README.