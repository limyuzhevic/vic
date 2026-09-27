# Detailed Analysis of API Inconsistencies

## **1. Config.set() Method Missing**

**Specific Problem:** Config.set() method is documented but not implemented in Python bindings.

**Documentation Evidence:**
- HOW_TO_USE.md lines 306-307: `config.set("brain.neuron_count", 1000)`
- HOW_TO_USE.md lines 307-308: `config.set("learning.rate", 0.001)`
- HOW_TO_USE.md section 4: Complete table of configuration options

**Implementation Gap:**
- C++ Header: Config.hpp shows set() method declarations (lines 75-79)
- C++ Implementation: Config.cpp has complete set() method implementations (lines 117-143)
- Python Bindings: No set() method in bindings.cpp

**Specific Line Numbers:**
- Missing from: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (should be added after line 165)

**Code Example vs Reality:**
```python
# What documentation shows:
config.set("brain.neuron_count", 1000)

# What actually exists:
config.has("brain.neuron_count")  # Can check existence
config.getKeys()                  # Can list keys
# But cannot set values!
```

## **2. Config.get() Method Missing**

**Specific Problem:** Config.get() method is documented but not implemented in Python bindings.

**Documentation Evidence:**
- HOW_TO_USE.md section 3: "Query" section mentions getting values
- HOW_TO_USE.md lines 302-303: Code example showing querying config

**Implementation Gap:**
- C++ Header: Config.hpp shows get() template method (lines 68-72)
- C++ Implementation: Config.cpp has get() and getOr() implementations (lines 95-115)
- Python Bindings: No get() or getOr() methods exposed

**Specific Line Numbers:**
- Missing from: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (should be added after line 165)

**Code Example vs Reality:**
```python
# What documentation suggests:
if config.has("brain.neuron_count"):
    # Access the value somehow
    value = config.get("brain.neuron_count")

# What actually exists:
if config.has("brain.neuron_count"):
    keys = config.getKeys()  # Can list keys but not retrieve specific values
```

## **3. Brain.receiveSensoryInput() Method Missing**

**Specific Problem:** Brain.receiveSensoryInput() method is documented but not exposed in Python bindings.

**Documentation Evidence:**
- HOW_TO_USE.md section 3: "Receive sensory input from environment"
- Code examples in easy_usage.md show direct brain sensory injection

**Implementation Gap:**
- C++ Header: Brain.hpp shows receiveSensoryInput() declaration (line 75)
- C++ Implementation: Brain.cpp has full implementation (line 591)
- Python Bindings: Only hasBrain.initialize(), brain.step(), etc. but no receiveSensoryInput()

**Specific Line Numbers:**
- Missing from: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (should be added after line 354)

**Code Example vs Reality:**
```python
# What documentation shows:
brain.receiveSensoryInput(sensory_input)

# What actually exists:
agent = pynlm.createAgentBrain(brain)
agent.processSensoryInput(percept)  # More complex indirect approach
```

## **4. Configuration Parameters Not Exposed**

**Specific Problem:** All documented configuration parameters are missing from Python API.

**Documentation Evidence:**
- HOW_TO_USE.md section 4: Complete configuration parameter table
- Parameters include: brain.neuron_count, plasticity.stdp.learning_rate, neuromod.dopamine.scale, etc.

**Implementation Gap:**
- C++ Implementation: Brain.cpp uses these config parameters (lines 181-186, 134-150)
- Python Bindings: Config object cannot be configured

**Specific Line Numbers:**
- Missing configuration access in: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_50218b7d-8436-4e98-84d4-76e500697800/python/bindings.cpp` (lines 145-165)

**Impact:** Users cannot configure the brain behavior as documented:
```python
# What documentation shows:
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)

# What actually works:
config = pynlm.createDefaultConfig()
# No way to modify config values!
```

## **Critical Missing Method Signatures**

### **Config.set() Signature (C++):**
```cpp
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
```

### **Config.get() Signature (C++):**
```cpp
template<typename T>
std::optional<T> get(const std::string& key) const;

template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;
```

### **Brain.receiveSensoryInput() Signature (C++):**
```cpp
void receiveSensoryInput(const class SensoryInput& input);
```

## **Python Binding Gaps Summary:**

**Missing Methods in bindings.cpp:**
1. `Config.set()` - 5 overloads missing
2. `Config.get()` - Template method missing  
3. `Config.getOr()` - Default value method missing
4. `Brain.receiveSensoryInput()` - Direct sensory injection missing

**Missing Configuration Parameter Access:**
- 12+ documented parameters cannot be set via Python API
- All plasticity, neuromodulation, and brain parameters hardcoded

**Methods Present But Incomplete:**
- `Config.has()` - Works
- `Config.getKeys()` - Works  
- `Config.summary()` - Works
- But lack essential get/set capabilities

## **Root Cause Analysis:**

1. **Incomplete Python Bindings:** The bindings.cpp file was not updated to match the C++ API documentation
2. **Documentation Drift:** Documentation was written but bindings were never implemented
3. **Feature Gap:** Core configuration and sensory injection capabilities are missing from Python API
4. **User Experience Impact:** Users following documentation will encounter "method not found" errors

This represents a fundamental disconnect between the promised API and the actual implementation.
