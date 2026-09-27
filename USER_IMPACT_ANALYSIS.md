# Why API Inconsistencies Break User Expectations

## **User Expectation Violation Analysis**

### **1. Config.set() Missing - Breaks Basic Configuration Workflow**

**User Expectation:**
Based on documentation, users expect to be able to configure the brain behavior directly:

```python
# What users read in documentation:
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # ← Users expect this to work
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)

brain = pynlm.createBrain(config)
brain.initialize()
```

**Reality:**
```python
import pynlm

config = pynlm.createDefaultConfig()
# What actually works:
config.loadFromFile("config.txt")  # Limited to file loading
config.has("key")                  # Can check but not set
config.getKeys()                   # Can list but not modify
# Config.set() is completely missing!

brain = pynlm.createBrain(config)
brain.initialize()
```

**Why This Breaks Users:**

1. **Broken Workflow:** Users following the documentation cannot configure their brain as shown in examples
2. **Confusing API:** Users see `config.has("key")` but cannot actually use that key
3. **Lost Functionality:** Cannot dynamically adjust simulation parameters during runtime
4. **Documentation Lies:** Users get "method not found" errors when following examples

**Specific User Scenarios:**

- **Scenario 1:** User wants to create a smaller brain for testing (but cannot set neuron_count)
- **Scenario 2:** User wants to tune learning rates (but cannot set plasticity.stdp.learning_rate)
- **Scenario 3:** User wants to enable/disable neuromodulation (but cannot set neuromod.dopamine.scale)
- **Scenario 4:** User wants to experiment with different configurations (but cannot set any parameters)

**Expected vs Actual Behavior:**
```python
# Expected (from documentation):
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # User wants 500 neurons
# Result: Brain has 1000 neurons (hardcoded default)

# Actually:
config = pynlm.createDefaultConfig()
# User tries: config.set("brain.neuron_count", 500)
# Error: AttributeError: 'Config' object has no attribute 'set'
```

### **2. Config.get() Missing - Breaks Configuration Querying**

**User Expectation:**
Based on documentation, users expect to read configuration values back:

```python
# What users read in documentation:
config = pynlm.createDefaultConfig()

if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")  # ← Users expect this to work
    print(f"Brain has {neuron_count} neurons")

keys = config.getKeys()  # Should list all configured keys
for key in keys:
    print(f"{key}: {config.get(key)}")  # ← Users expect this to work
```

**Reality:**
```python
# What actually works:
config = pynlm.createDefaultConfig()

if config.has("brain.neuron_count"):
    # User tries to get the value but cannot!
    # No way to retrieve config values
    
keys = config.getKeys()  # Returns list of keys
# But user cannot access the actual values!
```

**Why This Breaks Users:**

1. **Incomplete API:** Users can list keys but cannot access their values
2. **Debugging Nightmare:** Cannot verify which configuration is actually being used
3. **Feature Gap:** Users cannot implement config validation or logging
4. **Confusing State:** Config object exists but is essentially read-only for querying

**Specific User Problems:**

- **Problem 1:** User wants to validate their config before running simulation
- **Problem 2:** User wants to log which configuration is being used for reproducibility
- **Problem 3:** User wants to save/load different configurations
- **Problem 4:** User wants to debug why simulation behaves unexpectedly

**Example of User Frustration:**
```python
# User's debugging code that fails:
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # User cannot do this!

# User tries to verify:
print(config.getKeys())  # Shows ['brain.neuron_count']
# User tries to get actual value:
print(config.get("brain.neuron_count"))  # AttributeError: 'Config' object has no attribute 'get'
# User is stuck!
```

### **3. Config.getOr() Missing - Breaks Default Value Handling**

**User Expectation:**
Based on documentation, users expect fallback behavior for missing config values:

```python
# What users read in documentation:
config = pynlm.createDefaultConfig()

# User might want to:
neuron_count = config.getOr("brain.neuron_count", 1000)  # Get value or use default
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
# Both should work but config.getOr() is missing!
```

**Reality:**
```python
# What actually works:
config = pynlm.createDefaultConfig()

# User tries:
neuron_count = config.getOr("brain.neuron_count", 1000)  # AttributeError!
# No fallback mechanism available
```

**Why This Breaks Users:**

1. **Missing Robustness:** Users cannot implement safe config access patterns
2. **No Default Values:** Cannot provide sensible defaults for missing parameters
3. **Error-Prone Code:** Users must implement manual try/catch or has()/get() patterns
4. **Incomplete Tooling:** Missing essential utility method for configuration management

**User Impact Scenarios:**

- **Scenario 1:** User wants optional configuration parameters with defaults
- **Scenario 2:** User wants to implement configuration validation with fallbacks
- **Scenario 3:** User wants to create flexible config loading that doesn't crash on missing values
- **Scenario 4:** User wants to implement progressive configuration discovery

**Best Practice Pattern Users Cannot Implement:**
```python
# Pattern that users want to use (but cannot):
def createBrainWithConfig(custom_config=None):
    config = pynlm.createDefaultConfig()
    
    if custom_config:
        for key, value in custom_config.items():
            config.set(key, value)  # Missing method!
    
    # Safe access with defaults (missing method!)
    neuron_count = config.getOr("brain.neuron_count", 1000)
    learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
    
    return pynlm.createBrain(config)
```

## **Overall User Experience Impact**

### **Trust Issues:**
- Users cannot trust the documentation
- Users fear the library is unfinished or broken
- Users waste time debugging "missing method" errors

### **Productivity Issues:**
- Users cannot follow documentation examples
- Users cannot configure the system as advertised
- Users must implement workarounds for basic functionality

### **Professional Impact:**
- Library appears unreliable for production use
- Users cannot build production applications
- Users must modify the source code to use basic features

### **Adoption Barriers:**
- New users will be frustrated immediately
- Users will abandon the library for alternatives
- The library appears to be "broken" despite having working C++ code

## **The Root Problem:**

**Incomplete Bindings:** The Python bindings (bindings.cpp) are missing essential methods that exist in the C++ implementation. This creates a situation where:

1. **C++ Code:** Has all the functionality
2. **Python Bindings:** Missing ~50% of the API
3. **Documentation:** Describes the complete API
4. **User Reality:** Gets a partially working API

**Solution:** The bindings.cpp file needs to be updated to include:
- `Config.set()` method (5 overloads)
- `Config.get()` method (template)
- `Config.getOr()` method

This is a **technical debt** issue that can be fixed by updating the pybind11 bindings.
