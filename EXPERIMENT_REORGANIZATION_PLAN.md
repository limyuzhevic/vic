# EXPERIMENT REORGANIZATION PLAN

## 1. Files to Move/Reorganize

### Current Phase-Specific Files (to be moved):

#### Phase 3 Files:
- `src/experiments/Phase3Demo.cpp` - Phase 3 world interaction demonstration
- `src/experiments/Phase3Experiment.cpp` - Phase 3 experiment implementation
- `src/experiments/Phase3Experiment.hpp` - Phase 3 experiment interface

#### Phase 4 Files:
- `src/experiments/Phase4Demo.cpp` - Phase 4 demonstration
- `src/experiments/Phase4Experiment.cpp` - Phase 4 experiment implementation
- `src/experiments/Phase4Experiment.hpp` - Phase 4 experiment interface

#### Phase 5 Files:
- `src/experiments/Phase5Experiment.cpp` - Phase 5 lifetime experiment implementation
- `src/experiments/Phase5Experiment.hpp` - Phase 5 experiment interface

#### Phase 6 Files:
- `src/experiments/Phase6Demo.cpp` - Phase 6 demonstration
- `src/experiments/Phase6IntegratedExperiment.cpp` - Phase 6 integration experiment implementation
- `src/experiments/Phase6IntegratedExperiment.hpp` - Phase 6 integration experiment interface

### Files to Keep in Experiments (tests/benchmarks only):

#### Core Experiment Infrastructure (keep in experiments/):
- `src/experiments/Experiment.cpp`
- `src/experiments/Experiment.hpp`
- `src/experiments/ExperimentRunner.cpp`
- `src/experiments/ExperimentRunner.hpp`
- `src/experiments/Metrics.cpp`
- `src/experiments/Metrics.hpp`
- `src/experiments/AblationSystem.hpp`
- `src/experiments/ScalingBenchmark.hpp`

#### New Test/Benchmark Files (to create):
- `src/experiments/tests/Phase4IntegrationTest.cpp` - Integration tests for Phase 4
- `src/experiments/tests/Phase5LifetimeLearningTest.cpp` - Lifetime learning tests
- `src/experiments/tests/Phase4PredictionTest.cpp` - Prediction system tests
- `src/experiments/tests/Phase4MemoryTest.cpp` - Memory system tests
- `src/experiments/tests/Phase4CognitiveTest.cpp` - Cognitive system tests
- `src/experiments/tests/Phase4PlanningTest.cpp` - Planning system tests
- `src/experiments/tests/Phase4SocialTest.cpp` - Social learning tests
- `src/experiments/tests/Phase4SelfModelTest.cpp` - Self-model tests
- `src/experiments/tests/Phase6IntegrationTest.cpp` - Phase 6 integration verification

## 2. Specific Renaming Strategy

### New Directory Structure:

#### src/core/experiment/
- `CoreExperimentSystem.hpp` - Base experiment framework
- `CoreExperimentSystem.cpp` - Core experiment logic

#### src/brain/experiment/
- `PredictionExperimentIntegration.hpp` - Prediction system experiments
- `PredictionExperimentIntegration.cpp` - Prediction experiments

#### src/neuromodulation/experiment/
- `NeuromodulationExperimentIntegration.hpp` - Neuromodulation experiments
- `NeuromodulationExperimentIntegration.cpp` - Neuromodulation experiments

#### src/memory/experiment/
- `MemoryExperimentIntegration.hpp` - Memory system experiments
- `MemoryExperimentIntegration.cpp` - Memory experiments

#### src/cognition/experiment/
- `CognitiveExperimentIntegration.hpp` - Cognitive system experiments
- `CognitiveExperimentIntegration.cpp` - Cognitive experiments

#### src/prediction/experiment/
- `PredictionIntegrationTest.hpp` - Prediction system tests
- `PredictionIntegrationTest.cpp` - Prediction system tests

#### src/performance/experiment/
- `PerformanceBenchmarks.hpp` - Performance benchmarks
- `PerformanceBenchmarks.cpp` - Performance benchmarks

#### src/experiments/tests/
- `Phase4IntegrationTest.cpp` - Phase 4 integration tests
- `Phase5LifetimeLearningTest.cpp` - Phase 5 lifetime learning tests
- `Phase6IntegrationTest.cpp` - Phase 6 integration tests
- `Phase4SystemBenchmark.cpp` - Phase 4 system benchmarks

### File Renaming Strategy:

#### For Phase 3 Logic:
- `Phase3Experiment` → `ExperienceBasedLearningSystem`
- `Phase3Demo` → `BasicWorldInteractionDemo`

#### For Phase 4 Logic:
- `Phase4Experiment` → `MultiSystemIntegrationTest`
- `Phase4Demo` → `AdvancedSystemIntegrationDemo`

#### For Phase 5 Logic:
- `Phase5Experiment` → `LifetimeLearningSystem`
- `Phase4Experiment` → `MultiSystemIntegration`

#### For Phase 6 Logic:
- `Phase6IntegratedExperiment` → `CompleteSystemIntegration`
- `Phase6Demo` → `FullSystemDemo`

## 3. Impact Assessment on Build System and Dependencies

### Build System Impacts:

#### CMakeLists.txt Changes:
- Remove references to old phase-specific files
- Add new experiment integration files
- Update include directories to reflect new structure

#### Dependencies:

##### Core Dependencies (unchanged):
- Logger system
- Configuration system
- Random number generation
- Simulation clock

##### Brain System Dependencies:
- New: `PredictionExperimentIntegration.cpp` depends on:
  - `NeuralPrediction.cpp`
  - `Brain.cpp`
  - `Config.cpp`
  
- New: `MemoryExperimentIntegration.cpp` depends on:
  - `NeuralWorkingMemory.cpp`
  - `NeuralEpisodicMemory.cpp`
  - `Memory.cpp`

##### Neuromodulation Dependencies:
- New: `NeuromodulationExperimentIntegration.cpp` depends on:
  - `Reward.cpp`
  - `Curiosity.cpp`
  - `PredictionError.cpp`
  - `Novelty.cpp`

##### Cognition Dependencies:
- New: `CognitiveExperimentIntegration.cpp` depends on:
  - `NeuralPlanner.cpp`
  - `ConceptFormation.cpp`
  - `Attention.cpp`

### Build Time Impact:
- **Positive**: Reduced compilation due to fewer phase-specific files
- **Neutral**: New integration files add some build time
- **Net**: Minimal change to overall build time

### Maintenance Impact:
- **Positive**: Better separation of concerns
- **Positive**: Easier to modify individual systems without affecting other phases
- **Challenge**: Need to update multiple integration files when systems change

## 4. Step-by-Step Reorganization Plan

### Phase 1: Preparation (Week 1)

#### Tasks:
1. **Create new directory structure**
   ```bash
   mkdir -p src/core/experiment
   mkdir -p src/brain/experiment
   mkdir -p src/neuromodulation/experiment
   mkdir -p src/memory/experiment
   mkdir -p src/cognition/experiment
   mkdir -p src/prediction/experiment
   mkdir -p src/performance/experiment
   mkdir -p src/experiments/tests
   ```

2. **Create header files for new integration systems**
   - Create `src/core/experiment/CoreExperimentSystem.hpp`
   - Create `src/brain/experiment/PredictionExperimentIntegration.hpp`
   - etc.

3. **Analyze and extract phase logic**
   - Review each phase file to identify system-specific logic
   - Extract brain system logic from Phase 4
   - Extract memory system logic from Phase 4
   - Extract cognition system logic from Phase 4
   - Extract neuromodulation system logic from Phase 4

### Phase 2: Core Integration (Week 2)

#### Tasks:
1. **Move Phase 3 logic to appropriate systems**
   - Experience learning → `src/core/experiment/CoreExperimentSystem.cpp`
   - World interaction → `src/brain/experiment/PredictionExperimentIntegration.cpp`

2. **Move Phase 4 logic to integration tests**
   - Create integration test framework
   - Extract system-specific logic to integration files
   - Create test harnesses for each system

3. **Move Phase 5 logic to lifetime learning system**
   - Create `src/core/experiment/LifetimeLearningSystem.cpp`
   - Extract developmental phases to system-specific experiments

4. **Move Phase 6 logic to complete integration system**
   - Create `src/core/experiment/CompleteSystemIntegration.cpp`
   - Extract all Phase 6 systems to integration files

### Phase 3: Tests and Benchmarks (Week 3)

#### Tasks:
1. **Create new test infrastructure**
   - `src/experiments/tests/Phase4IntegrationTest.cpp`
   - `src/experiments/tests/Phase5LifetimeLearningTest.cpp`
   - `src/experiments/tests/Phase6IntegrationTest.cpp`

2. **Convert old phase files to tests**
   - `Phase3Demo.cpp` → `src/experiments/tests/BasicWorldInteractionDemo.cpp`
   - `Phase4Demo.cpp` → `src/experiments/tests/AdvancedSystemIntegrationDemo.cpp`
   - `Phase6Demo.cpp` → `src/experiments/tests/FullSystemDemo.cpp`

3. **Create system benchmarks**
   - `src/performance/experiment/PerformanceBenchmarks.cpp`
   - Add benchmark test suite to experiments/tests/

### Phase 4: Update Dependencies (Week 4)

#### Tasks:
1. **Update CMakeLists.txt**
   - Add new source files
   - Update include paths
   - Remove old phase-specific files from build

2. **Update header includes**
   - Replace `#include "Phase3Experiment.hpp"` with `#include "src/core/experiment/CoreExperimentSystem.hpp"`
   - Update all file dependencies

3. **Update main.cpp**
   - Update demo main function to use new structure
   - Ensure backward compatibility for any external users

### Phase 5: Testing and Verification (Week 5)

#### Tasks:
1. **Build and test new structure**
   - Compile entire project
   - Run existing tests
   - Verify integration tests work

2. **Performance testing**
   - Compare build times before/after
   - Run system benchmarks
   - Verify no regression in functionality

3. **Documentation updates**
   - Update README with new structure
   - Document integration interfaces
   - Add usage examples for new test framework

### Phase 6: Cleanup (Week 6)

#### Tasks:
1. **Remove obsolete files**
   - Delete original phase experiment files
   - Remove temporary files
   - Clean up backup files

2. **Final verification**
   - Run full test suite
   - Ensure all systems work correctly
   - Document any remaining issues

## Key Benefits of This Reorganization:

1. **Separation of Concerns**: Each system has its own experiment integration
2. **Maintainability**: Easier to modify individual systems
3. **Testability**: Better isolation for testing individual systems
4. **Extensibility**: Easy to add new systems or experiment types
5. **Performance**: Reduced build times due to fewer phase-specific files
6. **Documentation**: Clear structure makes system organization visible

## Risk Mitigation:

1. **Build Failures**: Regular build testing throughout the process
2. **Functionality Loss**: Comprehensive test suite for each system
3. **Documentation Gaps**: Maintain inline documentation for all new interfaces
4. **Dependency Issues**: Careful tracking of all file dependencies

This reorganization plan addresses all requirements while maintaining system functionality and improving code organization.