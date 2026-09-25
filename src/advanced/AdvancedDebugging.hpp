#ifndef NLM_ADVANCED_DEBUGGING_HPP
#define NLM_ADVANCED_DEBUGGING_HPP

#include <string>
#include <memory>
#include <vector>
#include <chrono>

namespace nlm {

class AdvancedDebugger {
public:
    static void analyzeMemoryLeaks(const std::shared_ptr<Brain>& brain);
    static void profilePerformance(const std::shared_ptr<Brain>& brain, double duration);
    static void detectPerformanceBottlenecks(const std::shared_ptr<Brain>& brain);
    static void analyzeConnectivity(const std::shared_ptr<Brain>& brain);
    static void generateDebugReport(const std::shared_ptr<Brain>& brain);
};

} // namespace nlm

#endif // NLM_ADVANCED_DEBUGGING_HPP