Removed duplicate/unused Observation files:

## Files Deleted
- src/environment/Observation.hpp
- src/environment/Observation.cpp

## Reason
These files were not being compiled in the main CMakeLists.txt:
- src/world/Observation.hpp/cpp ARE included in nlm_core library
- src/environment/Observation.hpp/cpp are NOT included anywhere

## Verification
- Reward.cpp and Novelty.cpp use Observation class forward-declared in their headers
- Reward.hpp and Novelty.hpp use "class Observation" forward-declaration
- Both expect the src/world/Observation interface (getData/getDimensions methods)
- The src/environment/Observation class has a completely different interface (SensoryInput* methods)
- The src/environment/Observation files contained placeholder code with TODO PHASE 1 comments

## Impact
- No build or compilation impact
- Removed dead code that was never used
- Eliminated class name ambiguity
- Maintained the intended src/world/Observation interface