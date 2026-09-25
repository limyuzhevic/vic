#pragma once

namespace nlm {
namespace cli {
namespace options {

class InteractiveOptions {
public:
    InteractiveOptions();
    ~InteractiveOptions() = default;
    
    // Parse options from argument list
    void parse(const std::vector<std::string>& args);
    
    // Get parsed values
    bool getInteractive() const { return interactive; }
    bool getWatch() const { return watch; }
    bool getMonitor() const { return monitor; }
    
    // Set values
    void setInteractive(bool interactive) { this->interactive = interactive; }
    void setWatch(bool watch) { this->watch = watch; }
    void setMonitor(bool monitor) { this->monitor = monitor; }
    
private:
    bool interactive;
    bool watch;
    bool monitor;
};

} // namespace options
} // namespace cli
} // namespace nlm
