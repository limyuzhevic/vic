template<typename T>
class MemoryBuffer {
public:
    void add(const T& item) {
        buffer_.push_back(item);
        if (buffer_.size() > maxSize_) {
            buffer_.erase(buffer_.begin());
        }
    }
    
    std::vector<T> getRecent(size_t count) const {
        if (count > buffer_.size()) count = buffer_.size();
        return std::vector<T>(buffer_.end() - count, buffer_.end());
    }
    
    const T& getLatest() const { return buffer_.back(); }
    bool empty() const { return buffer_.empty(); }
    
private:
    static const size_t maxSize_ = 10;
    std::vector<T> buffer_;
};

class MemoryContext {
public:
    struct Trace {
        std::vector<float> pattern;
        float relevance;
        SimulationStep timestamp;
        
        Trace(const std::vector<float>& p, float r, SimulationStep t)
            : pattern(p), relevance(r), timestamp(t) {}
    };
    
    struct Episode {
        EpisodicMemoryItem item;
        float relevance;
        
        Episode(const EpisodicMemoryItem& it, float r) : item(it), relevance(r) {}
    };
    
    void addWorkingMemoryTrace(const std::vector<float>& pattern, float relevance, SimulationStep timestamp) {
        workingMemoryBuffer_.add(Trace(pattern, relevance, timestamp));
    }
    
    std::vector<Trace> getWorkingMemoryTraces(size_t maxCount) const {
        return workingMemoryBuffer_.getRecent(maxCount);
    }
    
    void addEpisodicEpisode(const EpisodicMemoryItem& episode) {
        float relevance = computeEpisodeRelevance(episode);
        episodicBuffer_.add(Episode(episode, relevance));
    }
    
    std::vector<Episode> getRecentEpisodes(size_t maxCount) const {
        return episodicBuffer_.getRecent(maxCount);
    }
    
    std::vector<Episode> retrieveRelevantEpisodes(const std::vector<float>& queryPattern, size_t maxResults) const {
        std::vector<Episode> relevant;
        auto episodes = episodicBuffer_.getRecent(episodicBuffer_.size());
        
        for (const auto& episode : episodes) {
            float sim = computePatternSimilarity(queryPattern, episode.item.sensoryState);
            if (sim > 0.7f) {
                relevant.push_back(episode);
            }
        }
        
        // Sort by relevance (similarity * episode relevance)
        std::sort(relevant.begin(), relevant.end(), 
            [](const Episode& a, const Episode& b) {
                float simA = computePatternSimilarity(queryPattern, a.item.sensoryState);
                float simB = computePatternSimilarity(queryPattern, b.item.sensoryState);
                return simA * a.relevance > simB * b.relevance;
            });
        
        if (relevant.size() > maxResults) {
            relevant.resize(maxResults);
        }
        
        return relevant;
    }
    
    std::vector<Trace> retrieveRelevantWorkingMemory(const std::vector<float>& queryPattern, size_t maxResults) const {
        std::vector<Trace> relevant;
        auto traces = workingMemoryBuffer_.getRecent(workingMemoryBuffer_.size());
        
        for (const auto& trace : traces) {
            float sim = computePatternSimilarity(queryPattern, trace.pattern);
            if (sim > 0.6f) {
                relevant.push_back(trace);
            }
        }
        
        // Sort by relevance and recency
        std::sort(relevant.begin(), relevant.end(),
            [](const Trace& a, const Trace& b) {
                float simA = computePatternSimilarity(queryPattern, a.pattern);
                float simB = computePatternSimilarity(queryPattern, b.pattern);
                float scoreA = simA * a.relevance * (1000.0f / (1000.0f + (1000.0f - a.timestamp)));
                float scoreB = simB * b.relevance * (1000.0f / (1000.0f + (1000.0f - b.timestamp)));
                return scoreA > scoreB;
            });
        
        if (relevant.size() > maxResults) {
            relevant.resize(maxResults);
        }
        
        return relevant;
    }
    
    float computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    float computeEpisodeRelevance(const EpisodicMemoryItem& episode) const {
        float relevance = 0.0f;
        
        // Higher reward = higher relevance
        if (episode.reward > 0) {
            relevance += episode.reward * 0.5f;
        }
        
        // Recent episodes are more relevant
        relevance += 0.2f * (1.0f - static_cast<float>(episode.age) / 1000.0f);
        
        // Novelty increases relevance
        relevance += episode.novelty * 0.3f;
        
        return std::min(1.0f, relevance);
    }
    
    void clear() {
        workingMemoryBuffer_.clear();
        episodicBuffer_.clear();
    }
    
private:
    MemoryBuffer<Trace> workingMemoryBuffer_;
    MemoryBuffer<Episode> episodicBuffer_;
};
