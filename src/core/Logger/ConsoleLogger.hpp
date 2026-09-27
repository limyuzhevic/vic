#pragma once

#include "Logger.hpp"

namespace nlm {

class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override = default;

    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();

    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
