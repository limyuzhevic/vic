// Profiler.cpp
// Implementation of code profiling for NLM

#include "Profiler.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

ProfilerSession::ProfilerSession(const std::string& name)
    : name_(name), durationMs_(0.0), callCount_(0), minTimeMs_(std::numeric_limits<double>::max()),
      maxTimeMs_(0.0) {}

ProfilerSession::~ProfilerSession() {}

void ProfilerSession::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
}

void ProfilerSession::end() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    double timeMs = static_cast<double>(duration.count()) / 1000.0;
    
    durationMs_ += timeMs;
    callCount_++;
    
    if (timeMs < minTimeMs_) {
        minTimeMs_ = timeMs;
    }
    if (timeMs > maxTimeMs_) {
        maxTimeMs_ = timeMs;
    }
}

Profiler::Profiler() : enabled_(true), totalTime_(0.0) {}

Profiler::~Profiler() {
    for (auto* session : sessions_) {
        delete session;
    }
    sessions_.clear();
}

ProfilerSession* Profiler::startSession(const std::string& name) {
    if (!enabled_) {
        return nullptr;
    }
    
    ProfilerSession* session = new ProfilerSession(name);
    session->start();
    sessions_.push_back(session);
    return session;
}

void Profiler::endSession(ProfilerSession* session) {
    if (!session || !enabled_) {
        return;
    }
    
    session->end();
    totalTime_ += session->getDuration();
}

void Profiler::generateReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "=== NLM Profiler Report ===\n";
    file << "Generated on: " << getCurrentTimestamp() << "\n\n";
    
    file << std::left << std::setw(40) << "Session Name"
         << std::setw(20) << "Duration (ms)"
         << std::setw(15) << "Calls"
         << std::setw(15) << "Average (ms)"
         << std::setw(15) << "Min (ms)"
         << std::setw(15) << "Max (ms)" << "\n";
    
    file << std::string(120, '-') << "\n";
    
    for (const auto* session : sessions_) {
        double avgTime = session->getCallCount() > 0 ? 
            session->getDuration() / session->getCallCount() : 0.0;
        
        file << std::left << std::setw(40) << session->getName()
             << std::setw(20) << std::fixed << std::setprecision(6) << session->getDuration()
             << std::setw(15) << session->getCallCount()
             << std::setw(15) << std::fixed << std::setprecision(6) << avgTime
             << std::setw(15) << std::fixed << std::setprecision(6) << session->getMinTime()
             << std::setw(15) << std::fixed << std::setprecision(6) << session->getMaxTime() << "\n";
    }
    
    file << "\nTotal profiled time: " << std::fixed << std::setprecision(6) << totalTime_ << " ms\n";
    file.close();
}

std::string Profiler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

}} // namespace nlm