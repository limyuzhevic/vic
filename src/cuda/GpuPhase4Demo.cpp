// GpuPhase4Demo.cpp: GPU-accelerated Phase 4 demo implementation
#include "CudaBrain.hpp"
#include "CudaConfig.hpp"
#include <iostream>

namespace nlm {
namespace cuda {

void runGpuPhase4Demo() {
    std::cout << "Running GPU-accelerated Phase 4 demo..." << std::endl;
    
    // Configure CUDA
    CudaConfig config;
    config.enableGpuAcceleration = true;
    config.deviceId = 0;
    config.cudaThreadsPerBlock = 256;
    
    // Initialize CUDA brain
    CudaBrain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize CUDA brain for Phase 4 demo" << std::endl;
        return;
    }
    
    // Configure for Phase 4 specific settings
    brain.setGpuConfig(config);
    
    std::cout << "Phase 4 demo with GPU acceleration completed" << std::endl;
}

} // namespace cuda
} // namespace nlm
