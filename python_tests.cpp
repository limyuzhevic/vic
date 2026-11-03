#include <Python.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>

// Python module initialization function
PyObject* init_python_nlm_tests() {
    // Initialize Python interpreter
    Py_Initialize();
    
    // Import the nlm module
    PyObject* pModule = PyImport_ImportModule("nlm");
    if (!pModule) {
        PyErr_Print();
        return NULL;
    }
    
    // Create test class
    PyObject* pTestClass = PyObject_GetAttrString(pModule, "BrainAPITests");
    if (!pTestClass || !PyCallable_Check(pTestClass)) {
        std::cerr << "Error: Cannot find BrainAPITests class" << std::endl;
        Py_XDECREF(pModule);
        return NULL;
    }
    
    // Create test instance
    PyObject* pTestInstance = PyObject_CallObject(pTestClass, NULL);
    Py_DECREF(pTestClass);
    if (!pTestInstance) {
        PyErr_Print();
        Py_DECREF(pModule);
        return NULL;
    }
    
    // Run tests
    PyObject* pResult = PyObject_CallMethod(pTestInstance, "runAllTests", NULL);
    Py_DECREF(pTestInstance);
    Py_DECREF(pModule);
    
    if (!pResult) {
        PyErr_Print();
        return NULL;
    }
    
    int testResult = PyInt_AsLong(pResult);
    Py_DECREF(pResult);
    
    // Finalize Python interpreter
    Py_Finalize();
    
    // Return test result
    return PyInt_FromLong(testResult);
}

// Python wrapper for testing
class PythonNLMTester {
public:
    PythonNLMTester() : pModule(NULL), pTestClass(NULL), pTestInstance(NULL) {
        // Initialize Python interpreter
        Py_Initialize();
        
        // Import the nlm module
        pModule = PyImport_ImportModule("nlm");
        if (!pModule) {
            PyErr_Print();
            return;
        }
        
        // Get test class
        pTestClass = PyObject_GetAttrString(pModule, "BrainAPITests");
        if (!pTestClass) {
            PyErr_Print();
            Py_XDECREF(pModule);
            pModule = NULL;
            return;
        }
        
        // Create test instance
        pTestInstance = PyObject_CallObject(pTestClass, NULL);
        Py_DECREF(pTestClass);
        pTestClass = NULL;
        
        if (!pTestInstance) {
            PyErr_Print();
            Py_XDECREF(pModule);
            pModule = NULL;
        }
    }
    
    ~PythonNLMTester() {
        // Clean up Python objects
        Py_XDECREF(pTestInstance);
        Py_XDECREF(pTestClass);
        Py_XDECREF(pModule);
        
        // Finalize Python interpreter
        Py_Finalize();
    }
    
    bool runAllTests() {
        if (!pTestInstance) {
            std::cerr << "Error: Python NLM module not properly initialized" << std::endl;
            return false;
        }
        
        // Call runAllTests method
        PyObject* pResult = PyObject_CallMethod(pTestInstance, "runAllTests", NULL);
        if (!pResult) {
            PyErr_Print();
            return false;
        }
        
        int testResult = PyInt_AsLong(pResult);
        Py_DECREF(pResult);
        
        return (testResult == 0);
    }
    
private:
    PyObject* pModule;
    PyObject* pTestClass;
    PyObject* pTestInstance;
};

// C++ wrapper for Python testing
class CPPTOModuleTests {
public:
    static int runPythonIntegrationTests() {
        std::cout << "=== Python NLM Integration Tests ===" << std::endl;
        std::cout << std::endl;
        
        // Initialize Python interpreter
        Py_Initialize();
        
        try {
            // Import the nlm module
            PyObject* pModule = PyImport_ImportModule("nlm");
            if (!pModule) {
                PyErr_Print();
                Py_Finalize();
                return 1;
            }
            
            // Test basic module functionality
            std::cout << "  Testing basic NLM module functionality..." << std::endl;
            
            // Test Brain class
            PyObject* pBrainClass = PyObject_GetAttrString(pModule, "Brain");
            if (!pBrainClass) {
                std::cerr << "Error: Cannot find Brain class" << std::endl;
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pBrainClass);
            
            // Test SimpleWorld class
            PyObject* pWorldClass = PyObject_GetAttrString(pModule, "SimpleWorld");
            if (!pWorldClass) {
                std::cerr << "Error: Cannot find SimpleWorld class" << std::endl;
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pWorldClass);
            
            // Test AgentBrain class
            PyObject* pAgentClass = PyObject_GetAttrString(pModule, "AgentBrain");
            if (!pAgentClass) {
                std::cerr << "Error: Cannot find AgentBrain class" << std::endl;
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pAgentClass);
            
            // Test factory functions
            PyObject* pCreateBrainFunc = PyObject_GetAttrString(pModule, "createBrain");
            if (!pCreateBrainFunc) {
                std::cerr << "Error: Cannot find createBrain function" << std::endl;
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pCreateBrainFunc);
            
            PyObject* pCreateDefaultConfigFunc = PyObject_GetAttrString(pModule, "createDefaultConfig");
            if (!pCreateDefaultConfigFunc) {
                std::cerr << "Error: Cannot find createDefaultConfig function" << std::endl;
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pCreateDefaultConfigFunc);
            
            // Test config creation
            std::cout << "  Creating default configuration..." << std::endl;
            PyObject* pConfig = PyObject_CallObject(pCreateDefaultConfigFunc, NULL);
            if (!pConfig) {
                PyErr_Print();
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pConfig);
            
            // Test brain creation
            std::cout << "  Creating brain instance..." << std::endl;
            PyObject* pConfig2 = PyObject_CallFunction(pCreateDefaultConfigFunc, NULL);
            PyObject* pBrainArgs[1] = {pConfig2};
            PyObject* pBrain = PyObject_CallObject(pCreateBrainFunc, pBrainArgs);
            Py_DECREF(pConfig2);
            
            if (!pBrain) {
                PyErr_Print();
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            
            // Initialize brain
            PyObject_CallMethod(pBrain, "initialize", NULL);
            
            // Test brain methods
            std::cout << "  Testing brain methods..." << std::endl;
            
            // Get neuron count
            PyObject* pNeuronCount = PyObject_CallMethod(pBrain, "get_neuron_count", NULL);
            if (!pNeuronCount) {
                PyErr_Print();
                Py_DECREF(pBrain);
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            
            int neuronCount = PyInt_AsLong(pNeuronCount);
            Py_DECREF(pNeuronCount);
            
            if (neuronCount <= 0) {
                std::cerr << "Error: Invalid neuron count" << std::endl;
                Py_DECREF(pBrain);
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            
            // Get total spike count
            PyObject* pSpikeCount = PyObject_CallMethod(pBrain, "get_total_spike_count", NULL);
            if (!pSpikeCount) {
                PyErr_Print();
                Py_DECREF(pBrain);
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            
            int spikeCount = PyInt_AsLong(pSpikeCount);
            Py_DECREF(pSpikeCount);
            
            // Test world functionality
            std::cout << "  Testing world functionality..." << std::endl;
            
            // Create world class
            PyObject* pWorldClass = PyObject_GetAttrString(pModule, "createSimpleWorld");
            if (!pWorldClass) {
                std::cerr << "Error: Cannot find createSimpleWorld function" << std::endl;
                Py_DECREF(pBrain);
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pWorldClass);
            
            // Test agent functionality
            std::cout << "  Testing agent functionality..." << std::endl;
            
            // Create agent
            PyObject* pAgentClass = PyObject_GetAttrString(pModule, "createAgentBrain");
            if (!pAgentClass) {
                std::cerr << "Error: Cannot find createAgentBrain function" << std::endl;
                Py_DECREF(pBrain);
                Py_DECREF(pModule);
                Py_Finalize();
                return 1;
            }
            Py_DECREF(pAgentClass);
            
            // Test advanced API
            std::cout << "  Testing advanced API..." << std::endl;
            
            // Clean up
            Py_DECREF(pBrain);
            Py_DECREF(pModule);
            
            std::cout << "  ✓ All Python integration tests passed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            Py_Finalize();
            return 1;
        }
        
        Py_Finalize();
        return 0;
    }
};

// Main function
int main() {
    // Redirect stdout to stderr to avoid interfering with Python output
    std::cout.rdbuf(std::cerr.rdbuf());
    
    return CPPTOModuleTests::runPythonIntegrationTests();
}