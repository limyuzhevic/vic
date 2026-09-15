#ifndef NLM_LOG_STREAM_HPP
#define NLM_LOG_STREAM_HPP

#include "../core/Logger/Logger.hpp"

namespace nlm {

// Enhanced LogStream for better chaining and debugging
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Get the logged message when stream is destroyed
    std::string getMessage() const { return oss_.str(); }
    
    // Chain multiple insertions
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // End of stream for formatted output
    class EndStream {
    public:
        EndStream(LogStream& stream) : stream_(stream) {}
        ~EndStream() {
            // Log the stream when it goes out of scope
            if (!stream_.oss_.str().empty()) {
                std::string msg = stream_.oss_.str();
                stream_.logCurrent(msg);
            }
        }
    private:
        LogStream& stream_;
        void logCurrent(const std::string& msg) {
            // Use the LogStream's level and location info
            Logger::getGlobal().log(stream_.level_, msg, 
                                  stream_.file_, stream_.line_, 
                                  stream_.function_);
        }
    };
    
    // Create end stream marker
    EndStream end() { return EndStream(*this); }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

} // namespace nlm

#endif // NLM_LOG_STREAM_HPP