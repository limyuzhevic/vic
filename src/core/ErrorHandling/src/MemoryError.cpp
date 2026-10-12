// MemoryError implementation
namespace nlm {

MemoryError::MemoryError(const std::string& message,
                        MemoryErrorType type,
                        size_t address,
                        const std::string& suggestion)
    : NLMError("Memory Error: " + message, ErrorSeverity::Critical, ErrorCategory::Memory),
      memoryErrorType_(type),
      address_(address),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string MemoryError::getRecoveryHint() const {
    switch (memoryErrorType_) {
        case MemoryErrorType::AllocationFailed:
            return "Check memory allocation. Increase memory pool or reduce memory usage.";
        case MemoryErrorType::DeallocationFailed:
            return "Check memory deallocation logic. Verify memory management.";
        case MemoryErrorType::BufferOverflow:
            return "Check buffer bounds. Fix buffer overflow issue or increase buffer size.";
        case MemoryErrorType::MemoryLeakDetected:
            return "Check for memory leaks. Fix memory leak issue or enable memory leak detection.";
        case MemoryErrorType::OutOfMemory:
            return "Release unused memory. Increase system memory or reduce memory usage.";
        case MemoryErrorType::AccessViolation:
            return "Check memory access. Fix memory access violation or verify memory protection.";
        case MemoryErrorType::DoubleFree:
            return "Check memory deallocation. Fix double free issue or verify memory management.";
        case MemoryErrorType::UseAfterFree:
            return "Check memory access after deallocation. Fix use-after-free issue or verify memory lifecycle.";
        case MemoryErrorType::MemoryCorruption:
            return "Check memory corruption. Fix memory corruption issue or use memory protection.";
        case MemoryErrorType::PoolExhaustion:
            return "Increase memory pool size or release memory. Check memory pool configuration.";
        case MemoryErrorType::MemoryFragmentation:
            return "Compact memory or reduce fragmentation. Use memory optimization techniques.";
        default:
            return "Check memory usage. Fix memory issue or use memory optimization techniques.";
    }
}

} // namespace nlm