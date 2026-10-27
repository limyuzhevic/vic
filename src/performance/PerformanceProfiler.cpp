#include "PerformanceProfiler.hpp"
namespace nlm {

PerformanceProfiler::Scope::Scope(PerformanceProfiler& profiler, const std::string& name)
    : profiler_(profiler)
    , name_(name)
{
    profiler_.startTiming(name_);
}

PerformanceProfiler::Scope::~Scope() {
    profiler_.stopTiming(name_);
}

} // namespace nlm
