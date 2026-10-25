// Global instances for debugging and profiling
namespace nlm {
    // Global timeline profiler instance
    TimelineProfiler& getGlobalTimelineProfiler() {
        static TimelineProfiler globalTimelineProfiler;
        return globalTimelineProfiler;
    }
    
    // Global memory tracker instance  
    MemoryTracker& getGlobalMemoryTracker() {
        static MemoryTracker globalMemoryTracker;
        return globalMemoryTracker;
    }
    
    // Enhanced logging macros for debugging
    #define NLM_DEBUG_PERF(name) nlm::PerformanceProfiler profiler(name)
    #define NLM_MEASURE_TIME(name, func) nlm::PerformanceProfiler::profileSection(name, func)
    #define NLM_LOG_MEMORY_ALLOC(size) nlm::getGlobalMemoryTracker().trackAllocation(size, __FILE__, __LINE__, __func__)
    #define NLM_LOG_MEMORY_DEALLOC(size) nlm::getGlobalMemoryTracker().trackDeallocation(size, __FILE__, __LINE__, __func__)
    #define NLM_START_TIMELINE_EVENT(name, context) nlm::getGlobalTimelineProfiler().startEvent(name, context)
    #define NLM_END_TIMELINE_EVENT(name, context) nlm::getGlobalTimelineProfiler().endEvent(name, context)
    
    // Memory allocation tracking macros (for integration with new debug macros)
    void* operator new(size_t size) {
        void* ptr = std::malloc(size);
        NLM_LOG_MEMORY_ALLOC(size);
        return ptr;
    }
    
    void operator delete(void* ptr) noexcept {
        std::free(ptr);
        NLM_LOG_MEMORY_DEALLOC(0); // Size would need to be tracked differently
    }
    
    // Thread-local storage for per-thread profiling
    thread_local std::vector<std::string> threadLocalDebugContexts;
    
    // Utility functions for debugging
    void addDebugContext(const std::string& context) {
        threadLocalDebugContexts.push_back(context);
    }
    
    void clearDebugContext() {
        threadLocalDebugContexts.clear();
    }
    
    std::string getDebugContext() {
        std::stringstream ss;
        for (const auto& ctx : threadLocalDebugContexts) {
            ss << ctx << ">";
        }
        return ss.str();
    }
    
    // Enhanced logging with context
    inline void logWithContext(LogLevel level, const std::string& message, 
                               const std::string& file = "", int line = 0,
                               const std::string& function = "") {
        std::string context = getDebugContext();
        if (!context.empty()) {
            Logger::getGlobal().log(level, "[" + context + "] " + message,
                                   file, line, function);
        } else {
            Logger::getGlobal().log(level, message, file, line, function);
        }
    }

} // namespace nlm
