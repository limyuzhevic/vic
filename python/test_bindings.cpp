#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Simple test to verify the bindings compile
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Test module for pynlm bindings";
    
    // Simple class for testing
    py::class_<TestClass>(m, "TestClass")
        .def(py::init<>())
        .def("getValue", &TestClass::getValue)
        .def("setValue", &TestClass::setValue);
    
    // Test function
    m.def("testFunction", []() {
        return "Hello from Python bindings!";
    });
}

// Test class implementation
class TestClass {
public:
    TestClass() : value_(0) {}
    int getValue() const { return value_; }
    void setValue(int val) { value_ = val; }
private:
    int value_;
};