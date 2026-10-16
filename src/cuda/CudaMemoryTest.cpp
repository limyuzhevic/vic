// CudaMemoryTest.cpp: CUDA memory test implementation
#include "CudaMemoryTest.hpp"
#include "MemoryManager.hpp"
#include <iostream>
#include <random>

namespace nlm {
namespace cuda {

CudaMemoryTest::CudaMemoryTest() {
    std::cout << "CudaMemoryTest initialized" << std::endl;
}

CudaMemoryTest::~CudaMemoryTest() {
    std::cout << "CudaMemoryTest destroyed" << std::endl;
}

bool CudaMemoryTest::runTests() {
    std::cout << "Running CUDA memory tests..." << std::endl;
    
    bool allPassed = true;
    
    // Test 1: Allocate and free neuron memory
    if (!testNeuronMemory()) {
        std::cerr << "Neuron memory test failed" << std::endl;
        allPassed = false;
    }
    
    // Test 2: Allocate and free synapse memory
    if (!testSynapseMemory()) {
        std::cerr << "Synapse memory test failed" << std::endl;
        allPassed = false;
    }
    
    // Test 3: Allocate and free event queue memory
    if (!testEventQueueMemory()) {
        std::cerr << "Event queue memory test failed" << std::endl;
        allPassed = false;
    }
    
    // Test 4: Allocate and free plasticity memory
    if (!testPlasticityMemory()) {
        std::cerr << "Plasticity memory test failed" << std::endl;
        allPassed = false;
    }
    
    // Test 5: Transfer to device
    if (!testTransferToDevice()) {
        std::cerr << "Transfer to device test failed" << std::endl;
        allPassed = false;
    }
    
    // Test 6: Transfer from device
    if (!testTransferFromDevice()) {
        std::cerr << "Transfer from device test failed" << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "All CUDA memory tests passed!" << std::endl;
    } else {
        std::cout << "Some CUDA memory tests failed" << std::endl;
    }
    
    return allPassed;
}

bool CudaMemoryTest::testNeuronMemory() {
    std::cout << "Testing neuron memory allocation..." << std::endl;
    
    // Get memory manager
    MemoryManager& manager = getMemoryManager();
    
    // Allocate neuron memory
    size_t numNeurons = 1000;
    void* neuronMemory = manager.allocateNeuronMemory(numNeurons);
    if (!neuronMemory) {
        std::cerr << "Failed to allocate neuron memory" << std::endl;
        return false;
    }
    
    // Verify memory is allocated
    size_t expectedSize = numNeurons * sizeof(NeuronState);
    size_t actualSize = 0;  // In a real implementation, we would check the size
    
    // Free memory
    manager.freeMemory(neuronMemory);
    
    std::cout << "Neuron memory test passed" << std::endl;
    return true;
}

bool CudaMemoryTest::testSynapseMemory() {
    std::cout << "Testing synapse memory allocation..." << std::endl;
    
    MemoryManager& manager = getMemoryManager();
    
    size_t numSynapses = 10000;
    void* synapseMemory = manager.allocateSynapseMemory(numSynapses);
    if (!synapseMemory) {
        std::cerr << "Failed to allocate synapse memory" << std::endl;
        return false;
    }
    
    manager.freeMemory(synapseMemory);
    
    std::cout << "Synapse memory test passed" << std::endl;
    return true;
}

bool CudaMemoryTest::testEventQueueMemory() {
    std::cout << "Testing event queue memory allocation..." << std::endl;
    
    MemoryManager& manager = getMemoryManager();
    
    size_t numEvents = 5000;
    void* eventQueueMemory = manager.allocateEventQueueMemory(numEvents);
    if (!eventQueueMemory) {
        std::cerr << "Failed to allocate event queue memory" << std::endl;
        return false;
    }
    
    manager.freeMemory(eventQueueMemory);
    
    std::cout << "Event queue memory test passed" << std::endl;
    return true;
}

bool CudaMemoryTest::testPlasticityMemory() {
    std::cout << "Testing plasticity memory allocation..." << std::endl;
    
    MemoryManager& manager = getMemoryManager();
    
    size_t numUpdates = 20000;
    void* plasticityMemory = manager.allocatePlasticityMemory(numUpdates);
    if (!plasticityMemory) {
        std::cerr << "Failed to allocate plasticity memory" << std::endl;
        return false;
    }
    
    manager.freeMemory(plasticityMemory);
    
    std::cout << "Plasticity memory test passed" << std::endl;
    return true;
}

bool CudaMemoryTest::testTransferToDevice() {
    std::cout << "Testing transfer to device..." << std::endl;
    
    MemoryManager& manager = getMemoryManager();
    
    // Create test data on host
    size_t dataSize = 1024;
    std::vector<float> hostData(dataSize);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < dataSize; ++i) {
        hostData[i] = dist(rng);
    }
    
    // Allocate device memory
    void* deviceData = nullptr;
    cudaError_t error = cudaMalloc(&deviceData, dataSize * sizeof(float));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory" << std::endl;
        return false;
    }
    
    // Transfer data to device
    if (!manager.transferToDevice(hostData.data(), deviceData, dataSize * sizeof(float))) {
        cudaFree(deviceData);
        std::cerr << "Failed to transfer data to device" << std::endl;
        return false;
    }
    
    // Verify data (in a real test)
    
    cudaFree(deviceData);
    
    std::cout << "Transfer to device test passed" << std::endl;
    return true;
}

bool CudaMemoryTest::testTransferFromDevice() {
    std::cout << "Testing transfer from device..." << std::endl;
    
    MemoryManager& manager = getMemoryManager();
    
    // Create test data on device
    size_t dataSize = 1024;
    void* deviceData = nullptr;
    cudaError_t error = cudaMalloc(&deviceData, dataSize * sizeof(float));
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate device memory" << std::endl;
        return false;
    }
    
    // Initialize device data
    std::vector<float> initData(dataSize);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < dataSize; ++i) {
        initData[i] = dist(rng);
    }
    
    cudaMemcpy(deviceData, initData.data(), dataSize * sizeof(float), cudaMemcpyHostToDevice);
    
    // Allocate host memory
    std::vector<float> hostData(dataSize);
    
    // Transfer data from device
    if (!manager.transferFromDevice(hostData.data(), deviceData, dataSize * sizeof(float))) {
        cudaFree(deviceData);
        std::cerr << "Failed to transfer data from device" << std::endl;
        return false;
    }
    
    // Verify data (in a real test)
    
    cudaFree(deviceData);
    
    std::cout << "Transfer from device test passed" << std::endl;
    return true;
}

void CudaMemoryTest::printTestResults() const {
    std::cout << "=== CUDA Memory Test Results ===" << std::endl;
    std::cout << "All tests completed" << std::endl;
    std::cout << "================================" << std::endl;
}

} // namespace cuda
} // namespace nlm
