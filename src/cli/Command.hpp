#pragma once

#include <string>
#include <memory>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace cli {

class Command {
public:
    Command(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain(brain), config(config) {}
    virtual ~Command() = default;
    
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    
protected:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
};

} // namespace cli
} // namespace nlm
