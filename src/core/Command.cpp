#include "Command.hpp"
#include <chrono>

namespace nlm {

CommandResult ConditionalExecution::executeIf(Command& cmd, CommandContext& context) const {
    if (negate) {
        if (!condition || !condition->evaluate(context)) {
            return cmd.execute();
        }
    } else {
        if (condition && condition->evaluate(context)) {
            return cmd.execute();
        }
    }
    return CommandResult(true, "Command skipped (condition not met)");
}

CommandResult ConditionalExecution::executeWhile(Command& cmd, CommandContext& context) const {
    std::vector<CommandResult> results;
    bool conditionMet = true;
    while (conditionMet) {
        CommandResult result;
        if (negate) {
            if (!condition || !condition->evaluate(context)) {
                result = cmd.execute();
            } else {
                conditionMet = false;
            }
        } else {
            if (condition && condition->evaluate(context)) {
                result = cmd.execute();
            } else {
                conditionMet = false;
            }
        }
        if (result.success) {
            results.push_back(result);
        } else {
            break;
        }
    }
    
    // Combine results
    std::string output;
    for (const auto& res : results) {
        if (!output.empty()) output += "\n";
        output += res.output;
    }
    return CommandResult(true, output);
}

CommandResult ConditionalExecution::executeUntil(Command& cmd, CommandContext& context) const {
    std::vector<CommandResult> results;
    bool conditionNotMet = true;
    while (conditionNotMet) {
        CommandResult result = cmd.execute();
        if (!result.success) {
            break;
        }
        
        if (negate) {
            if (condition && !condition->evaluate(context)) {
                conditionNotMet = false;
            }
        } else {
            if (condition && condition->evaluate(context)) {
                conditionNotMet = false;
            }
        }
        
        results.push_back(result);
    }
    
    // Combine results
    std::string output;
    for (const auto& res : results) {
        if (!output.empty()) output += "\n";
        output += res.output;
    }
    return CommandResult(true, output);
}

} // namespace nlm