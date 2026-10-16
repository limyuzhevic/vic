// GpuMain.cpp: GPU-accelerated main implementation
#include "CudaBrain.hpp"
#include "CudaConfig.hpp"
#include <iostream>

namespace nlm {
namespace cuda {

// Global CUDA brain instance
global CudaBrain* gCudaBrain = nullptr;

// Initialize GPU acceleration
bool initializeGpuAcceleration(const CudaConfig& config) {
    std::cout << "Initializing GPU acceleration..." << std::endl;
    
    // Create CUDA brain
    gCudaBrain = new CudaBrain(config);
    
    // Initialize
    if (!gCudaBrain->initialize()) {
        std::cerr << "Failed to initialize CUDA brain" << std::endl;
        delete gCudaBrain;
        gCudaBrain = nullptr;
        return false;
    }
    
    std::cout << "GPU acceleration initialized successfully" << std::endl;
    return true;
}

// Shutdown GPU acceleration
void shutdownGpuAcceleration() {
    if (gCudaBrain) {
        gCudaBrain->logStatus();
        delete gCudaBrain;
        gCudaBrain = nullptr;
    }
    std::cout << "GPU acceleration shutdown" << std::endl;
}

// Get current CUDA brain instance
CudaBrain* getCudaBrain() {
    return gCudaBrain;
}

// Check if GPU acceleration is available
bool isGpuAccelerationAvailable() {
    return (gCudaBrain != nullptr) && gCudaBrain->isGpuAccelerated();
}

// Print GPU acceleration status
void printGpuStatus() {
    std::cout << "=== GPU Acceleration Status ===" << std::endl;
    if (gCudaBrain) {
        gCudaBrain->logStatus();
    } else {
        std::cout << "GPU acceleration not initialized" << std::endl;
    }
    std::cout << "================================" << std::endl;
}

} // namespace cuda
} // namespace nlm
