#ifndef LOGGING_MANAGER_HPP
#define LOGGING_MANAGER_HPP

#include <memory>
#include "CLI.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class LoggingManager {
public:
    static std::shared_ptr<Logger> setupLogging(const CLI::Options& cliOptions);
};

} // namespace nlm

#endif // LOGGING_MANAGER_HPP
