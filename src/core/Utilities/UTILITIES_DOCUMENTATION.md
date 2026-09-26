# ============================================================================
# UTILITIES DOCUMENTATION
# ============================================================================
# 
# This document provides comprehensive documentation for all utility functions
# implemented in the src/core/Utilities/ directory.
#

# ============================================================================
# INTRODUCTION
# ============================================================================
#
# The NLM utilities module provides a comprehensive set of reusable utility functions
# designed to eliminate code duplication and improve maintainability. These utilities
# were extracted based on common patterns found throughout the codebase, particularly
# in AgentBrain.cpp, bindings.cpp, MotorDecoder.cpp, and other core modules.
#
# ============================================================================
# UTILITY GROUPS
# ============================================================================
#

# -----------------------------------------------------------------------------
# GROUP 1: VALUE SCALING AND NORMALIZATION UTILITIES
# -----------------------------------------------------------------------------
#
# ## Overview
#
# This group provides functions for scaling and normalizing numerical values,
# encapsulating common patterns like `(value * 2.0f - 1.0f) * 5.0f`,
# `value * 5.0f`, and `value * 8.0f` found throughout the codebase.
#
# ## Functions
#
# ### scaleToRange()
#
# **Purpose**: Scale a value from source range to target range with comprehensive
# validation and error handling.
#
# **Signature**:
# ```cpp
template<typename T>
T scaleToRange(T value, T sourceMin, T sourceMax, T targetMin, T targetMax);
```
#
# **Parameters**:
# - `value`: Input value to scale
# - `sourceMin`: Minimum of source range
# - `sourceMax`: Maximum of source range
# - `targetMin`: Minimum of target range
# - `targetMax`: Maximum of target range
#
# **Returns**:
# Scaled value in target range
#
# **Examples**:
# ```cpp
# // Scale vision intensity from [0,1] to [-1,1]
# float normalized = scaleToRange(visionIntensity, 0.0f, 1.0f, -1.0f, 1.0f);
#
# // Scale motor command from [0,1] to [0,255]
# uint8_t pwm = static_cast<uint8_t>(scaleToRange(motorCmd, 0.0f, 1.0f, 0.0f, 255.0f));
# ```
#
# ### normalizeToSigned()
#
# **Purpose**: Convert normalized [0,1] values to signed [-1,1] range.
#
# **Signature**:
# ```cpp
template<typename T>
T normalizeToSigned(T value, T offset = 0.5);
```
#
# **Parameters**:
# - `value`: Input value in [0,1] range
# - `offset`: Optional offset to add before conversion (default: 0.5)
#
# **Returns**:
# Value in [-1,1] range
#
# **Notes**:
# This function encapsulates the common pattern `(value * 2.0f - 1.0f)`
#
# ### applyOffsetScale()
#
# **Purpose**: Apply offset-scale transformation commonly used in neural networks.
#
# **Signature**:
# ```cpp
template<typename T>
T applyOffsetScale(T value, T offset = -1.0, T scale = 5.0);
```
#
# **Parameters**:
# - `value`: Input value to transform
# - `offset`: Scaling offset (typically -1.0 or 1.0)
# - `scale`: Scaling factor (typically 5.0 or 8.0)
#
# **Returns**:
# Transformed value `(value * 2.0 - offset) * scale`
#
# **Notes**:
# This function encapsulates the common pattern `(value * 2.0f - 1.0f) * 5.0f`
#
# -----------------------------------------------------------------------------
# GROUP 2: NEURON GROUP DISTRIBUTION UTILITIES
# -----------------------------------------------------------------------------
#
# ## Overview
#
# This group provides functions for distributing neurons across groups,
# replacing complex modulo-based distribution logic found in AgentBrain initialization
# and other places.
#
# ## Functions
#
# ### distributeToGroups()
#
# **Purpose**: Distribute neurons across multiple groups using balanced modulo distribution.
#
# **Signature**:
# ```cpp
# std::vector<size_t> distributeToGroups(size_t neuronCount, size_t groupCount, size_t startIndex = 0);
# ```
#
# **Parameters**:
# - `neuronCount`: Total number of neurons to distribute
# - `groupCount`: Number of groups to distribute across
# - `startIndex`: Starting index for distribution (rotates distribution)
#
# **Returns**:
# Vector of group sizes representing balanced distribution
#
# **Examples**:
# ```cpp
# // Distribute 100 neurons across 6 motor groups
# auto distribution = distributeToGroups(100, 6, 0);
# // Result: [17, 16, 17, 16, 17, 17]
# ```
#
# ### distributeSensoryNeurons()
#
# **Purpose**: Specialized distribution for sensory neurons across modalities.
#
# **Signature**:
# ```cpp
# std::vector<size_t> distributeSensoryNeurons(size_t totalNeurons);
# ```
#
# **Parameters**:
# - `totalNeurons`: Total number of sensory neurons
#
# **Returns**:
# Vector of neuron counts per sensory modality
#
# ### distributeMotorNeurons()
#
# **Purpose**: Balanced distribution for motor neurons across action groups.
#
# **Signature**:
# ```cpp
# std::vector<size_t> distributeMotorNeurons(size_t totalNeurons);
# ```
#
# **Parameters**:
# - `totalNeurons`: Total number of motor neurons
#
# **Returns**:
# Vector of neuron counts per motor action group
#
# -----------------------------------------------------------------------------
# GROUP 3: ACTIVITY CALCULATION UTILITIES
# -----------------------------------------------------------------------------
#
# ## Overview
#
# This group provides functions for calculating neural activity, replacing repeated
# `calcActivity` lambda implementations found in MotorDecoder and other places.
#
# ## Functions
#
# ### calculateAverageActivity()
#
# **Purpose**: Calculate average neural activity from neuron states.
#
# **Signature**:
# ```cpp
# float calculateAverageActivity(const std::vector<Neuron*>& neurons);
# ```
#
# **Parameters**:
# - `neurons`: Vector of neuron pointers
#
# **Returns**:
# Average activity value across all neurons
#
# **Examples**:
# ```cpp
# // Calculate average activity across motor neuron population
# float avgActivity = calculateAverageActivity(motorNeurons);
# ```
#
# ### calculateMaxActivity()
#
# **Purpose**: Calculate maximum neural activity from neuron states.
#
# **Signature**:
# ```cpp
# float calculateMaxActivity(const std::vector<Neuron*>& neurons);
# ```
#
# **Parameters**:
# - `neurons`: Vector of neuron pointers
#
# **Returns**:
# Maximum activity value found in the population
#
# ### calculateWeightedActivity()
#
# **Purpose**: Calculate weighted activity sum with importance weights.
#
# **Signature**:
# ```cpp
# float calculateWeightedActivity(const std::vector<Neuron*>& neurons, const std::vector<float>& weights);
# ```
#
# **Parameters**:
# - `neurons`: Vector of neuron pointers
# - `weights`: Vector of weights for each neuron (must match neurons.size())
#
# **Returns**:
# Weighted sum of activities
#
# -----------------------------------------------------------------------------
# GROUP 4: NOVELTY AND SIMILARITY CALCULATION UTILITIES
# -----------------------------------------------------------------------------
#
# ## Overview
#
# This group provides functions for novelty and similarity calculations,
# replacing repeated difference calculation and normalization patterns found in
# AgentBrain.cpp for novelty detection.
#
# ## Functions
#
# ### calculateTemporalNovelty()
#
# **Purpose**: Calculate temporal novelty based on state difference over time with decay.
#
# **Signature**:
# ```cpp
# float calculateTemporalNovelty(const std::vector<float>& currentState, 
#                               const std::vector<float>& previousState,
#                               float decayRate = NOVELTY_DECAY_RATE);
# ```
#
# **Parameters**:
# - `currentState`: Current neural state vector
# - `previousState`: Previous neural state vector (same size as currentState)
# - `decayRate`: Decay factor for older differences (default: 0.99)
#
# **Returns**:
# Normalized novelty score
#
# ### calculateFeatureSimilarity()
#
# **Purpose**: Calculate similarity between feature vectors using different metrics.
#
# **Signature**:
# ```cpp
# float calculateFeatureSimilarity(const std::vector<float>& features1,
#                                 const std::vector<float>& features2,
#                                 int similarityMetric = 1);
# ```
#
# **Parameters**:
# - `features1`: First feature vector
# - `features2`: Second feature vector (same size as features1)
# - `similarityMetric`: Type of similarity metric (0=Euclidean, 1=Cosine)
#
# **Returns**:
# Similarity score (0.0 = dissimilar, 1.0 = identical)
#
# ### calculateWeightedDifference()
#
# **Purpose**: Calculate weighted absolute difference between two values.
#
# **Signature**:
# ```cpp
template<typename T>
T calculateWeightedDifference(T value1, T value2, T weight = 1.0);
```
#
# **Parameters**:
# - `value1`: First value
# - `value2`: Second value
# - `weight`: Weight for difference calculation
#
# **Returns**:
# Weighted absolute difference
#
# -----------------------------------------------------------------------------
# GROUP 5: CONFIGURATION AND CONSTANT ACCESS UTILITIES
# -----------------------------------------------------------------------------
#
# ## Overview
#
# This group provides functions for safe configuration and constant access,
# replacing direct use of magic numbers and hardcoded constants throughout the codebase.
#
# ## Functions
#
# ### getConfigValue()
#
# **Purpose**: Safely retrieve configuration value with type conversion.
#
# **Signature**:
# ```cpp
template<typename T>
T getConfigValue(const Config& config, const std::string& key, const T& defaultValue);
```
#
# **Parameters**:
# - `config`: Config object to retrieve value from
# - `key`: Configuration key
# - `defaultValue`: Default value if key not found
#
# **Returns**:
# Configuration value converted to requested type
#
# ### getConstantValue()
#
# **Purpose**: Access system constant with validation.
#
# **Signature**:
# ```cpp
template<typename T>
T getConstantValue(const std::string& constantName);
```
#
# **Parameters**:
# - `constantName`: Name of constant to retrieve
#
# **Returns**:
# Constant value
#
# ### validateParameter()
#
# **Purpose**: Validate parameter against allowed range with tolerance.
#
# **Signature**:
# ```cpp
template<typename T>
bool validateParameter(T value, T minValue, T maxValue, T tolerance = 0.001);
```
#
# **Parameters**:
# - `value`: Value to validate
# - `minValue`: Minimum allowed value
# - `maxValue`: Maximum allowed value
# - `tolerance`: Tolerance for floating point comparisons
#
# **Returns**:
# true if value is valid within tolerance, false otherwise
#
# -----------------------------------------------------------------------------
# USAGE EXAMPLES
# -----------------------------------------------------------------------------
#
# ## Example: Replacing Scaling Patterns
#
# **Before** (duplicated patterns):
# ```cpp
# // Pattern 1: (value * 2.0f - 1.0f) * 5.0f
# float scale1 = (vision[i] * 2.0f - 1.0f) * 5.0f;
#
# // Pattern 2: (value * 2.0f - 1.0f) * 3.0f
# float scale2 = (vision[i] * 2.0f - 1.0f) * 3.0f;
#
# // Pattern 3: value * 5.0f
# float scale3 = vision[i] * 5.0f;
#
# // Pattern 4: value * 8.0f
# float scale4 = vision[i] * 8.0f;
# ```
#
# **After** (using utilities):
# ```cpp
# // Pattern 1: Now uses utility function
# float scale1 = nlm::applyOffsetScale(vision[i], -1.0f, 5.0f);
#
# // Pattern 2: Now uses utility function
# float scale2 = nlm::applyOffsetScale(vision[i], -1.0f, 3.0f);
#
# // Pattern 3: Simple scaling (can still use directly)
# float scale3 = vision[i] * 5.0f;
#
# // Pattern 4: Simple scaling (can still use directly)
# float scale4 = vision[i] * 8.0f;
# ```
#
# ## Example: Replacing Neuron Distribution
#
# **Before** (complex modulo logic):
# ```cpp
# for (int i = 0; i < totalMotorNeurons; ++i) {
#     int group = i % MOTOR_ACTION_GROUPS;
#     motorGroups[group]++;
# }
# ```
#
# **After** (using utility):
# ```cpp
# auto distribution = nlm::distributeMotorNeurons(totalMotorNeurons);
# // Result: Balanced distribution across motor groups
# ```
#
# ## Example: Replacing Activity Calculation
#
# **Before** (repeated calcActivity lambda):
# ```cpp
# float calcActivity(const std::vector<Neuron*>& neurons) {
#     if (neurons.empty()) return 0.0f;
#     float sum = 0.0f;
#     for (Neuron* n : neurons) {
#         sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
#     }
#     return sum / neurons.size();
# }
# ```
#
# **After** (using utilities):
# ```cpp
# float avgActivity = nlm::calculateAverageActivity(neuronPopulation);
# float maxActivity = nlm::calculateMaxActivity(neuronPopulation);
# float weightedActivity = nlm::calculateWeightedActivity(neuronPopulation, weights);
# ```
#
# ## Example: Replacing Novelty Calculation
#
# **Before** (repeated difference calculation):
# ```cpp
# float novelty = 0.0f;
# for (int i = 0; i < stateSize; ++i) {
#     float diff = std::abs(currentState[i] - previousState[i]);
#     novelty += diff * std::pow(NOVELTY_DECAY_RATE, i);
# }
# novelty /= stateSize;
# ```
#
# **After** (using utility):
# ```cpp
# float novelty = nlm::calculateTemporalNovelty(currentState, previousState);
# ```
#
# ## Example: Replacing Magic Number Usage
#
# **Before** (direct magic numbers):
# ```cpp
# float scaleFactor = 5.0f;                    // Magic number!
# int visionWidth = 16;                        // Hardcoded!
# bool isValid = (value >= 0.0f && value <= 1.0f); // Manual validation!
# ```
#
# **After** (using utilities):
# ```cpp
# float scaleFactor = nlm::getConfigValue<float>(config, "vision.scale_factor", 5.0f);
# int visionWidth = nlm::getConstantValue<int>("VISION_WIDTH");
# bool isValid = nlm::validateParameter(value, 0.0f, 1.0f);
# ```
#
# ============================================================================
# PERFORMANCE CONSIDERATIONS
# ============================================================================
#
# All utility functions are optimized for performance:
#
# 1. **Const Correctness**: Functions use const references where appropriate
# 2. **Minimal Allocations**: Functions avoid unnecessary dynamic allocations
# 3. **Template Specialization**: Templates support both float and double precision
# 4. **Exception Safety**: Functions provide proper error handling and validation
# 5. **Cache Friendly**: Sequential memory access patterns for vector operations
#
# ============================================================================
# TESTING
# ============================================================================
#
# Comprehensive test coverage is provided through:
#
# 1. **Unit Tests**: Individual utility function testing
# 2. **Integration Tests**: Pattern replacement verification
# 3. **Performance Tests**: Benchmark comparisons with original patterns
# 4. **Edge Case Tests**: Boundary condition validation
#
# ============================================================================
# BACKWARD COMPATIBILITY
# ============================================================================
#
# This implementation maintains backward compatibility:
#
# 1. **No API Changes**: All existing public APIs remain unchanged
# 2. **Performance**: New utilities maintain or improve performance
# 3. **Behavior**: Functions provide same mathematical results as original patterns
# 4. **Safety**: Additional validation does not break existing valid usage
#
# ============================================================================
# MAINTENANCE AND EXTENSION
# ============================================================================
#
# ## Adding New Utilities
#
# To add a new utility function:
#
# 1. **Group Selection**: Choose an existing group or create a new one
# 2. **Documentation**: Add comprehensive Doxygen comments
# 3. **Implementation**: Provide template and non-template implementations
# 4. **Testing**: Add test cases and examples
# 5. **Review**: Ensure consistency with existing code style
#
# ## Updating Utilities
#
# When modifying existing utilities:
#
# 1. **Maintain Signature**: Don't change function signatures
# 2. **Update Documentation**: Refresh Doxygen comments
# 3. **Preserve Behavior**: Ensure mathematical equivalence
# 4. **Update Examples**: Refresh example code in documentation
#
# ## Migration Guide
#
# For existing code using the patterns being replaced:
#
# 1. **Identify Patterns**: Search for duplicated code patterns
# 2. **Replace Gradually**: Convert functions one at a time
# 3. **Test Thoroughly**: Verify mathematical equivalence
# 4. **Benchmark**: Compare performance if critical
#
# ============================================================================
# FUTURE ENHANCEMENTS
# ============================================================================
#
# Potential future enhancements include:
#
# 1. **SIMD Acceleration**: Vectorized implementations using SIMD intrinsics
# 2. **Parallel Processing**: Multi-threaded versions for large datasets
# 3. **GPU Support**: CUDA implementations for GPU acceleration
# 4. **Configuration System**: Enhanced config value type conversion
# 5. **Logging Integration**: Built-in performance and error logging
#
# ============================================================================
