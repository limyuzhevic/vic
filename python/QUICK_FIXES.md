# QUICK REFERENCE: BINDINGS FIXES

## 1. URGENT: Memory Safety Fix

**FILE**: `/workspace/app/python/bindings.cpp` (lines 150-158)

**PROBLEM**: Unsafe const_cast in loadFromArgs binding

**FIX**:

```cpp
// REPLACE THIS:
.def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
    return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
}, py::arg("args"),
   "Load configuration from command line arguments")

// WITH THIS:
.def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
    // Validate input
    if (args.empty()) {
        throw std::invalid_argument("Empty arguments list");
    }
    
    // Create safe copy of strings
    std::vector<std::string> args_copy = args;
    std::vector<char*> argv;
    argv.reserve(args_copy.size());
    for (const auto& a : args_copy) {
        // Allocate temporary copy - C++ code may modify this
        char* temp_str = new char[a.size() + 1];
        std::strcpy(temp_str, a.c_str());
        argv.push_back(temp_str);
    }
    
    try {
        bool result = self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        // Cleanup allocated strings
        for (char* str : argv) {
            delete[] str;
        }
        return result;
    } catch (...) {
        // Cleanup on exception
        for (char* str : argv) {
            delete[] str;
        }
        throw;
    }
}, py::arg("args"),
   "Load configuration from command line arguments (safely copies input)")
```

## 2. URGENT: Add Missing Exception Types

**FILE**: `/workspace/app/python/bindings.cpp` (line 30)

**PROBLEM**: Only std::runtime_error registered, but C++ throws many exceptions

**FIX**:

```cpp
// ADD THESE LINES AFTER EXISTING REGISTRATION:
py::register_exception<std::invalid_argument>(m, "ValueError");
py::register_exception<std::out_of_range>(m, "IndexError");
py::register_exception<std::length_error>(m, "ValueError");
py::register_exception<std::domain_error>(m, "ValueError");
py::register_exception<std::overflow_error>(m, "ValueError");
py::register_exception<std::underflow_error>(m, "ValueError");
```

## 3. HIGH: Add Neuron Class Binding

**FILE**: `/workspace/app/python/bindings.cpp` (add after NeuronId binding)

**PROBLEM**: No Python access to individual neurons

**FIX**:

```cpp
// ADD AFTER NEURONID BINDING (around line 42):
py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron", R"pbdoc(Spiking neuron)pbdoc")
    .def(py::init<NeuronId>(), py::arg("id"))
    .def("getId", &Neuron::getId)
    .def("getType", &Neuron::getType)
    .def("setType", &Neuron::setType, py::arg("type"))
    .def("getMembranePotential", &Neuron::getMembranePotential)
    .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
    .def("getThreshold", &Neuron::getThreshold)
    .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
    .def("isFiring", &Neuron::isFiring)
    .def("isRefractory", &Neuron::isRefractory)
    .def("getFiringRate", &Neuron::getFiringRate)
    .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
    .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("timestep"));
```

## 4. HIGH: Add RandomGenerator Binding

**FILE**: `/workspace/app/python/bindings.cpp` (add after Audio binding)

**PROBLEM**: No random number generation in Python

**FIX**:

```cpp
// ADD AFTER AUDIO BINDING (around line 201):
py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator)pbdoc")
    .def(py::init<>())
    .def("uniformInt", [](RandomGenerator& self, int min, int max) {
        return self.uniformInt(min, max);
    }, py::arg("min"), py::arg("max"), "Generate random integer")
    .def("uniformReal", [](RandomGenerator& self, double min, double max) {
        return self.uniformReal(min, max);
    }, py::arg("min"), py::arg("max"), "Generate random float")
    .def("normal", [](RandomGenerator& self, double mean, double stddev) {
        return self.normal(mean, stddev);
    }, py::arg("mean"), py::arg("stddev"), "Generate normal random value")
    .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p"), "Generate Bernoulli random value")
    .def("randomBool", &RandomGenerator::randomBool, "Generate random boolean");
```

## 5. MEDIUM: Improve Config Error Handling

**FILE**: `/workspace/app/python/bindings.cpp` (lines 147-148)

**PROBLEM**: loadFromFile returns bool but doesn't throw Python exceptions

**FIX**:

```cpp
// REPLACE THIS:
.def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
     "Load configuration from a JSON file")

// WITH THIS:
.def("loadFromFile", [](Config& self, const std::string& filepath) -> bool {
    try {
        return self.loadFromFile(filepath);
    } catch (const std::exception& e) {
        // In production, you might want to throw a Python exception here
        return false;
    }
}, py::arg("filepath"),
   "Load configuration from a JSON file. Returns True on success, False on error.")
```

## 6. MEDIUM: Null Pointer Safety for getRegion

**FILE**: `/workspace/app/python/bindings.cpp` (lines 328-331)

**PROBLEM**: getRegion returns nullptr if not found

**FIX**:

```cpp
// REPLACE THIS:
.def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
     py::arg("id"),
     py::return_value_policy::reference_internal,
     "Get a region by ID")

// WITH THIS:
.def("getRegion", [](Brain& self, RegionId id) -> py::object {
    NeuralRegion* region = self.getRegion(id);
    if (!region) {
        return py::none(); // Return None/None in Python
    }
    return py::cast(region);
}, py::arg("id"),
   "Get a region by ID. Returns None if region not found.")
```

## PRIORITY ORDER FOR IMPLEMENTATION

1. **Days 1-2**: Memory safety fix + exception mapping (URGENT)
2. **Days 3-7**: Add Neuron and RandomGenerator bindings (HIGH)
3. **Days 8-14**: Add remaining core classes (STDP, WorkingMemory, etc.)
4. **Days 15-21**: Improve error handling and documentation

## TESTING RECOMMENDATIONS

```bash
# Test memory safety
python -c "from pynlm import Config; c = Config(); c.loadFromArgs(['--test=value'])"

# Test exception handling  
python -c "from pynlm import RandomGenerator; rg = RandomGenerator(); rg.uniformInt(10, 1)"

# Test new API
python -c "from pynlm import Neuron, NeuronId; n = Neuron(NeuronId(1)); print(n.getId())"
```

## TIME ESTIMATE

- **Critical fixes (1-2 days)**: Memory safety + exceptions
- **Core API (1 week)**: Neuron, RandomGenerator, Synapse
- **Documentation (2-3 days)**: Add parameter descriptions
- **Testing (1 day)**: Verify all fixes work

**Total**: **~2 weeks** for production-ready bindings

These fixes address the most critical issues first and provide a foundation for a complete, production-ready Python API.