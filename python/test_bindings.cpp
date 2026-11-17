// Test module for enhanced NLM Python bindings
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <string>

namespace py = pybind11;

// Simple test class
class TestClass {
public:
    TestClass() {}
    
    std::string greet(const std::string& name) {
        return "Hello, " + name + "!";
    }
    
    int add(int a, int b) {
        return a + b;
    }
};

PYBIND11_MODULE(pynlm_test, m) {
    m.doc() = "Test module for enhanced NLM Python bindings";
    
    // Simple test class
    py::class_<TestClass>(m, "TestClass")
        .def(py::init<>())
        .def("greet", &TestClass::greet)
        .def("add", &TestClass::add);
    
    // Simple function
    m.def("multiply", [](int a, int b) { return a * b; }, "Multiply two numbers");
}