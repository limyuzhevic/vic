// Critical Implementation Plan for NLM Phase 6

// ============================================================================
// CURRENT STATE ANALYSIS
// ============================================================================

// The NLM project has a sophisticated architecture with CRITICAL inconsistencies:
// - Files exist but are not in build system
// - Documentation claims Phase 6 "COMPLETE" but reality is 35.8% completion
// - Config system has major bugs (JSON/YAML parser TODO, placeholder values)
// - Phase 6 systems are disconnected (have methods but aren't integrated)

// ============================================================================
// CRITICAL BUILD SYSTEM ISSUES (FILES EXIST BUT NOT BUILT)
// ============================================================================

// Phase 5: Lifetime Learning Experiment - FILES EXIST, BUILD SYSTEM MISSING
// - src/experiments/Phase5Experiment.hpp (COMPLETE - 362 lines)
// - src/experiments/Phase5Experiment.cpp (COMPLETE - 507 lines)
// - src/experiments/Phase5Demo.cpp (COMPLETE - 130 lines)

// Phase 6: Final Integration - FILES MOSTLY EXIST, BUILD SYSTEM MISSING
// - src/experiments/Phase6IntegratedExperiment.cpp (COMPLETE - 403 lines)
// - src/experiments/Phase6Demo.cpp (COMPLETE - 130 lines)
// - src/experiments/Phase6IntegratedExperiment.hpp (MISSING - NEEDS CREATION)

// ============================================================================
// CRITICAL CONFIG SYSTEM BUGS
// ============================================================================

// Config.cpp Issues:
// 1. JSON/YAML parser - TODO comment blocking implementation
// 2. saveToFile() - writes "PLACEHOLDER_VALUE" instead of actual values
// 3. configs/default.cfg - old key=value format, should be JSON

// ============================================================================
// DOCUMENTATION REALITY GAPS
// ============================================================================

// README.md Claims vs Reality:
// - Phase 6: "FINAL INTEGRATION" → 35.8% completion (43/120)
// - ./nlm_phase6_demo executable → NOT in build system
// - "All systems integrated" → Most systems disconnected
// - Phase 6 audit says "NOT STARTED" (Future Directions)

// ============================================================================
// IMPLEMENTATION PRIORITIES
// ============================================================================

// PRIORITY 1: CRITICAL (Week 1-2) - BUILD SYSTEM AND CONFIG FIXES
// 1. Fix CMakeLists.txt - Add Phase 5 and Phase 6 targets
// 2. Create Phase6IntegratedExperiment.hpp - Missing header
// 3. Fix Config.cpp - JSON/YAML parser and saveToFile() bug
// 4. Create configs/default.json - Replace old .cfg file

// PRIORITY 2: IMPORTANT (Week 3-4) - CONNECT DISCONNECTED SYSTEMS
// 5. Connect working memory to brain processing loop
// 6. Connect episodic memory to spike system
// 7. Complete neuromodulation integration (dopamine, curiosity, novelty)
// 8. Connect prediction system to brain processing
// 9. Connect cognitive systems (neural planner, concept formation, attention)
// 10. Complete checkpointing system

// ============================================================================
// SPECIFIC IMPLEMENTATION DETAILS
// ============================================================================

// ============================================================================
// 1. FIX CMakeLists.txt - Add Phase 5 and Phase 6 targets
// ============================================================================

// Current CMakeLists.txt only has Phase 3 and 4:
// - ✓ nlm_phase3, nlm_phase4 (Phase 3-4 libraries)
// - ✓ nlm_phase3_demo, nlm_phase4_demo (Phase 3-4 demos)
// - ✗ nlm_phase5, nlm_phase6 (Phase 5-6 libraries - MISSING)
// - ✗ nlm_phase5_demo, nlm_phase6_demo (Phase 5-6 demos - MISSING)

// Implementation:
// Add these sections to CMakeLists.txt:

// Add Phase 5 experiment library
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

// Add Phase 6 integration test library  
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
    src/experiments/Phase6IntegratedExperiment.hpp  // CREATE THIS
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

// Add Phase 5 demo executable
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

// Add Phase 6 demo executable (referenced in README)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

// Update install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

// ============================================================================
// 2. CREATE Phase6IntegratedExperiment.hpp
// ============================================================================

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
    // Implementation details...
};

} // namespace nlm

// ============================================================================
// 3. FIX Config.cpp - JSON/YAML Parser and saveToFile() Bug
// ============================================================================

// Problem 1: JSON/YAML parser - TODO comment
// Problem 2: saveToFile() writes "PLACEHOLDER_VALUE"

// Implementation:
// 1. Add JSON/YAML includes
// 2. Implement convertJSONToConfig() and convertYAMLToConfig()
// 3. Implement saveToFile() to save actual values
// 4. Create helper functions (getFileExtension, toConfigValueString)

// ============================================================================
// 4. CREATE configs/default.json
// ============================================================================

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

// ============================================================================
// 5. UPDATE DOCUMENTATION
// ============================================================================

// Update README.md to:
// - Remove Phase 6 demo executable claim if not available
// - Update Phase 6 audit results to show 35.8% completion
// - Reconcile claims with actual implementation

// ============================================================================
// VERIFICATION PROCEDURES
// ============================================================================

// Build verification:
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

// Check targets
make help | grep nlm_phase
// Should show: nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6

// Check demo executables
ls -la nlm_phase*_demo
// Should show all Phase 5 and 6 demos

// Run Phase 6 demo
./nlm_phase6_demo
// Should show integration results

// Test config system
./nlm --config="test_config.json" --save="test_save.json"
cat test_save.json
// Should show actual values, not "PLACEHOLDER_VALUE"

// ============================================================================
// SUMMARY OF CHANGES NEEDED
// ============================================================================

// Files to MODIFY:
// 1. CMakeLists.txt (add Phase 5 and 6 targets)
// 2. src/experiments/Phase6IntegratedExperiment.hpp (CREATE NEW)
// 3. src/core/Config/Config.cpp (implement JSON/YAML parser, fix saveToFile())
//

// Files to CREATE:
// 1. src/experiments/Phase6IntegratedExperiment.hpp
// 2. configs/default.json

// Files to UPDATE:
// 1. README.md (update documentation to match reality)
//

// ============================================================================
// CURRENT STATUS
// ============================================================================

// ✓ Phase 5 implementation COMPLETE (files exist but not built)
// ✓ Phase 6 implementation MOSTLY COMPLETE (files exist but not built, one header missing)
// ✗ Build system CRITICAL ISSUE (Phase 5/6 targets missing)
// ✗ Config system CRITICAL BUGS (JSON/YAML parser TODO, placeholder values)
// ✗ Documentation reality gaps (claims vs actual implementation)
//

// ============================================================================
// NEXT STEPS
// ============================================================================

// Immediate (Week 1-2):
// 1. Fix CMake build system (most critical)
// 2. Create missing Phase6IntegratedExperiment.hpp header
// 3. Fix Config.cpp JSON/YAML parser and saveToFile() bug
// 4. Create configs/default.json

// Short-term (Week 3-4):
// 5. Connect all disconnected Phase 6 systems
// 6. Complete checkpointing system
// 7. Update documentation

// These changes will transform NLM from a 35.8% complete basic neural simulator
// into a fully integrated artificial brain system with Phase 6 capabilities
// as documented in the README.

// The implementation guide is now complete and ready for execution.