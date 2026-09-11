#include "ParallelProcessing.hpp"

namespace nlm {

struct ParallelProcessing::Impl {
    size_t threadCount;
    bool enabled;
    std::vector<std::thread> threads;
};

ParallelProcessing::ParallelProcessing() : pImpl(std::make_unique<Impl>()) {
    pImpl->threadCount = std::thread::hardware_concurrency();
    pImpl->enabled = true;
}

ParallelProcessing::~ParallelProcessing() {
    enable(false);
}

void ParallelProcessing::setThreadCount(size_t count) {
    pImpl->threadCount = count;
}

size_t ParallelProcessing::getThreadCount() const {
    return pImpl->threadCount;
}

template<typename Func>
void ParallelProcessing::parallelFor(size_t begin, size_t end, Func func) {
    if (!pImpl->enabled || pImpl->threadCount <= 1) {
        for (size_t i = begin; i < end; ++i) {
            func(i);
        }
        return;
    }
    
    size_t chunkSize = (end - begin) / pImpl->threadCount;
    pImpl->threads.clear();
    
    for (size_t t = 0; t < pImpl->threadCount; ++t) {
        size_t start = begin + t * chunkSize;
        size_t stop = (t == pImpl->threadCount - 1) ? end : start + chunkSize;
        
        pImpl->threads.emplace_back([start, stop, func]() {
            for (size_t i = start; i < stop; ++i) {
                func(i);
            }
        });
    }
    
    for (auto& thread : pImpl->threads) {
        thread.join();
    }
}

template<typename Func>
void ParallelProcessing::parallelMap(const std::vector<float>& input, std::vector<float>& output, Func func) {
    if (!pImpl->enabled || pImpl->threadCount <= 1) {
        for (size_t i = 0; i < input.size(); ++i) {
            output[i] = func(input[i]);
        }
        return;
    }
    
    size_t chunkSize = input.size() / pImpl->threadCount;
    pImpl->threads.clear();
    
    for (size_t t = 0; t < pImpl->threadCount; ++t) {
        size_t start = t * chunkSize;
        size_t stop = (t == pImpl->threadCount - 1) ? input.size() : start + chunkSize;
        
        pImpl->threads.emplace_back([&, start, stop]() {
            for (size_t i = start; i < stop; ++i) {
                output[i] = func(input[i]);
            }
        });
    }
    
    for (auto& thread : pImpl->threads) {
        thread.join();
    }
}

bool ParallelProcessing::isEnabled() const {
    return pImpl->enabled;
}

void ParallelProcessing::enable(bool enable) {
    if (!enable) {
        for (auto& thread : pImpl->threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        pImpl->threads.clear();
    }
    pImpl->enabled = enable;
}

// Instantiate templates
namespace {
    template void ParallelProcessing::parallelFor<size_t(*)(size_t)>(size_t, size_t, size_t(*)(size_t));
    template void ParallelProcessing::parallelMap<size_t(*)(float)>(const std::vector<float>&, std::vector<float>&, size_t(*)(float));
}

} // namespace nlm
