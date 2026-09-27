# API Inconsistencies Report: C++ Implementation vs Python Documentation

## **Critical Issues Found**

### **1. Config Class - set() Method Missing from Python Bindings**

**Issue Type:** Missing method implementation

**Problem:** The `Config.set()` method is extensively documented but completely missing from the Python bindings.

**Documentation Claims:**
- HOW_TO_USE.md section 3: `config.set("brain.neuron_count", 1000)`
- HOW_TO_USE.md section 4: `config.set("brain.synapse_density", 0.1)`
- HOW_TO_USE.md section 4: `config.set("learning.rate", 0.001)`

**Actual Implementation:**
- C++ source: Config.cpp implements `set()` method (lines 117-143)
- Python bindings: bindings.cpp only exposes:
  - loadFromFile()
  - loadFromArgs()
  - saveToFile()
  - has()
  - getKeys()
  - clear()
  - summary()

**File Paths:**
- Documentation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/HOW_TO_USE.md` (lines 306-312)
- C++ Implementation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/src/core/Config/Config.cpp` (lines 117-143)
- Python Bindings: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (lines 145-165)

**Why This Breaks Users:**
- Users cannot modify configuration parameters as documented
- Users must reload configs from files or command line when changes are needed
- Major functionality gap between documentation and reality
- Violates the principle of least surprise

### **2. Config Class - get() Method Missing from Python Bindings**

**Issue Type:** Missing method implementation

**Problem:** The `Config.get()` method is documented but completely missing from the Python bindings, leaving users unable to retrieve configuration values.

**Documentation Claims:**
- HOW_TO_USE.md section 3: "Get values" section mentions `config.get("key")`
- Code example: `if config.has("brain.neuron_count"):` implies ability to get values

**Actual Implementation:**
- C++ source: Config.cpp implements `get()` methods (lines 95-115) with templates for int, double, bool, string
- Python bindings: No `get()` methods exposed

**File Paths:**
- C++ Implementation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/src/core/Config/Config.cpp` (lines 95-115)
- Python Bindings: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (lines 145-165)

**Why This Breaks Users:**
- Users cannot retrieve configuration values they set
- Cannot query the actual values being used by the brain
- Creates confusion when checking if config has values but cannot access them
- Incomplete API for configuration management

### **3. Brain Class - receiveSensoryInput() Method Missing**

**Issue Type:** Method name mismatch / missing method

**Problem:** The `Brain.receiveSensoryInput()` method is documented but missing from Python bindings.

**Documentation Claims:**
- HOW_TO_USE.md section 3: "Receive sensory input from environment"
- Code examples show direct brain sensory injection

**Actual Implementation:**
- C++ source: Brain.cpp implements `receiveSensoryInput()` (line 591)
- Python bindings: bindings.cpp doesn't expose this method

**File Paths:**
- C++ Implementation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/src/brain/Brain.cpp` (line 591)
- Python Bindings: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (lines 292-354)

**Why This Breaks Users:**
- Users cannot directly inject sensory data into brain (must use AgentBrain)
- Forces users into more complex indirect API path
- Documentation promises simpler direct API that doesn't exist

### **4. Config Parameters Not Exposed in Python API**

**Issue Type:** Missing configuration options

**Problem:** Multiple documented configuration parameters are not exposed in Python bindings.

**Documentation Lists:**
- `brain.neuron_count` (int, default 1000)
- `brain.synapse_density` (float, default 0.1)
- `brain.connection_probability` (float, default 0.05)
- `plasticity.stdp.enable` (bool, default true)
- `plasticity.stdp.learning_rate` (float, default 0.001)
- `plasticity.stdp.tau_plus` (float, default 20.0)
- `plasticity.stdp.tau_minus` (float, default 20.0)
- `plasticity.hebbian.enable` (bool, default true)
- `plasticity.structural.enable` (bool, default true)
- `neuromod.dopamine.scale` (float, default 1.0)
- `neuromod.curiosity.enable` (bool, default true)
- `neuromod.novelty.enable` (bool, default true)

**Actual Implementation:**
- All these parameters exist in Config.cpp (lines 181-186, 134-150)
- But none are exposed in Python bindings

**File Paths:**
- Documentation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/HOW_TO_USE.md` (section 4, lines 423-454)
- C++ Implementation: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/src/brain/Brain.cpp` (lines 181-186, 134-150)
- Python Bindings: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (lines 145-165)

**Why This Breaks Users:**
- Cannot configure brain parameters as documented
- Users are stuck with hardcoded defaults
- Significant portion of documentation is misleading
- Cannot optimize or tune the simulation behavior

## **Summary of All Issues:**

1. **Config.set()**: Missing from Python bindings - CRITICAL
2. **Config.get()**: Missing from Python bindings - CRITICAL  
3. **Brain.receiveSensoryInput()**: Missing from Python bindings - HIGH
4. **Config Parameters**: 12+ documented parameters not exposed - HIGH

**Total Issues:** 4 major inconsistencies
**Critical Issues:** 2 (affect basic functionality)
**High Priority Issues:** 2 (affect advanced functionality)

**Impact on Users:**
- Cannot use 50% of documented API methods
- Cannot configure the system as described
- Will encounter "method not found" errors when following documentation
- Documentation is essentially misleading

This represents a fundamental disconnect between the documented Python API and the actual implementation, making the library unusable for anyone following the documentation.
