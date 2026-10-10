#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>

namespace nlm {

class MemoryBufferImpl : public MemoryBuffer {
public:
    MemoryBufferImpl(size_t maxEntries = 10000) : maxEntries_(maxEntries) {}
    
    void addEntry(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.push_back(entry);
        if (entries_.size() > maxEntries_) {
            entries_.erase(entries_.begin());
        }
    }
    
    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
    }
    
    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.size();
    }
    
    std::vector<LogEntry> getEntries() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_;
    }
    
    void setMaxEntries(size_t max) override {
        std::lock_guard<std::mutex> lock(mutex_);
        maxEntries_ = max;
        if (entries_.size() > maxEntries_) {
            entries_.erase(entries_.begin(), entries_.end() - maxEntries_);
        }
    }
    
private:
    std::vector<LogEntry> entries_;
    mutable std::mutex mutex_;
    size_t maxEntries_;
};

} // namespace nlm