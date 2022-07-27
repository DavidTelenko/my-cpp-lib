#pragma once

#include <my/format/dbg.hpp>
#include <my/format/repr.hpp>
#include <my/util/cli/status.hpp>
#include <my/util/str_utils.hpp>
//
#include <any>
#include <cassert>
#include <functional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace my::cli {

template <class... Types>
constexpr const std::string&& str(const std::variant<Types...>&& v) {
    return std::get<std::string>(std::move(v));
}

template <class... Types>
constexpr const std::string& str(const std::variant<Types...>& v) {
    return std::get<std::string>(v);
}

template <class... Types>
constexpr const int32_t&& i64(const std::variant<Types...>&& v) {
    return std::get<int32_t>(std::move(v));
}

template <class... Types>
constexpr const int32_t& i64(const std::variant<Types...>& v) {
    return std::get<int32_t>(v);
}

template <class... Types>
constexpr const uint64_t&& u64(const std::variant<Types...>&& v) {
    return std::get<uint64_t>(std::move(v));
}

template <class... Types>
constexpr const uint64_t& u64(const std::variant<Types...>& v) {
    return std::get<uint64_t>(v);
}

template <class... Types>
constexpr const float&& f32(const std::variant<Types...>&& v) {
    return std::get<float>(std::move(v));
}

template <class... Types>
constexpr const float& f32(const std::variant<Types...>& v) {
    return std::get<float>(v);
}

class Application {
    using InitList = std::initializer_list<std::string>;

   public:
    using ArgHolder = std::variant<int64_t, uint64_t, float, std::string>;
    using ArgList = std::vector<std::string>;
    using ArgHolderList = std::vector<ArgHolder>;
    using Behaviour = std::function<Status(const ArgHolderList&)>;

    /**
     * @brief Library of default restrictions
     *
     */
    struct Constraints {
        static auto makeArgsAmountConstraint(size_t min, size_t max) {
            assert(max > min);

            return [min, max](const auto& args) -> Status {
                const auto emptyStatus = _checkNotEmptyArgs(args);
                if (not emptyStatus) return emptyStatus;

                if (args.size() < min) {
                    return Status(
                        Status::Code::ArgumentsAmountMismatch,
                        "Not enough arguments for \"{}\" command",
                        str(args[0]));
                }
                if (args.size() > max) {
                    return Status(
                        Status::Code::ArgumentsAmountMismatch,
                        "Too many arguments for \"{}\" command",
                        str(args[0]));
                }
                return {};
            };
        }

        static auto makeArgsAmountConstraint(size_t exactly) {
            return [exactly](const auto& args) -> Status {
                const auto emptyStatus = _checkNotEmptyArgs(args);
                if (not emptyStatus) return emptyStatus;

                if (args.size() != exactly) {
                    return Status(
                        Status::Code::ArgumentsAmountMismatch,
                        "\"{}\" command accepts exactly {} argument{}",
                        str(args[0]),
                        exactly - 1, (exactly > 2 ? "s" : ""));
                }
                return {};
            };
        }

        static auto makeArgStringLengthConstraint(size_t index, size_t max) {
            return [index, max](const auto& args) -> Status {
                if (str(args[index]).size() > max) {
                    return Status(
                        Status::Code::ArgumentValueMismatch,
                        "\"{}\" command accepts argument o with length no more than {} chars",
                        str(args[0]), max);
                }
                return {};
            };
        }

       private:
        static Status _checkNotEmptyArgs(const auto& args) {
            if (args.empty()) {
                return Status(
                    Status::Code::CallbackError,
                    "Empty arguments, (note: first argument must be the name of the command)");
            }
            return {};
        }
    };

    /**
     * @brief Library of default input output functions required for application
     *
     */
    struct DefaultIo {
        static auto getInput() {
            std::string input;
            std::getline(std::cin, input);
            return input;
        }

        static auto printHelper() {
            std::cout << "> ";
        }

        static auto onError(const Status& status) {
            my::printf(std::cerr, "Error: {}\n", *status);
        }

        static auto parse(const std::string& input) {
            ArgList tokens;

            bool quote = false;
            size_t next = 0;

            for (; next < input.size(); ++next) {
                if (!std::isspace(input[next])) break;
            }

            size_t last = next;

            for (; next < input.size(); ++next) {
                const char el = input[next];
                if (el == '"') quote = !quote;

                if (!quote and el == ' ') {
                    if (last < next) {
                        tokens.push_back(input.substr(last, next - last));
                    }
                    last = next + 1;
                }
            }
            if (last < input.size()) {
                tokens.push_back(input.substr(last));
            }

            my::pretty.printf("tokens: {}\n", tokens);

            return tokens;
        }
    };

    class Command {
       public:
        struct Argument {
            enum class Type {
                i64,
                u64,
                f32,
                string
            };

            Type type;

            std::string name{};
            std::string description{};

            // int32_t position = -1;  // if named than this does not matter

            // bool named = false;
            // bool optional = false;
        };

        Command() {
            // first is always the string typed, command name
            _args.push_back({
                .type = Argument::Type::string,
                .name = "@name",
                .description = "name of the function",
            });
        };

        Command& describe(const std::string& description) {
            _description = description;
            return *this;
        }

        const auto& description() const {
            return _description;
        }

        Command& callback(Behaviour behaviour) {
            _callback = behaviour;
            return *this;
        }

        Command& constrain(Behaviour behaviour) {
            static size_t id = 0;
            _constraints[id++] = behaviour;
            return *this;
        }

        Command& args(std::initializer_list<Argument> args) {
            _args.reserve(args.size());
            std::ranges::copy(args, std::back_inserter(_args));
            return *this;
        }

        const std::vector<Argument>& args() const { return _args; }

        Status invoke(const ArgList& args) {
            static auto makeArgTypeMismatch = [](const auto& type,
                                                 const auto& arg) {
                return Status(
                    Status::Code::ArgumentsTypeMismatch,
                    "Argument of type {} is not assignable to value of: {}",
                    type, arg);
            };

            // elaborate case, happens if outer parser fails
            if (args.empty()) {
                return Status(
                    Status::Code::CallbackError,
                    "Empty arguments, (note: first argument must be the name of the command)");
            }

            std::vector<ArgHolder> typedArgs;
            typedArgs.reserve(args.size());

            // first is always the string typed, command name
            typedArgs.push_back(args[0]);

            size_t i = 1;
            // all provided arguments will be parsed to its variant
            for (; i < std::min(args.size(), _args.size()); ++i) {
                const auto& arg = args[i];
                switch (_args[i].type) {
                    case Argument::Type::i64: {
                        auto parsed = my::parse<int64_t>(arg);
                        if (!parsed) return makeArgTypeMismatch("i64", arg);
                        typedArgs.push_back(*parsed);
                        break;
                    }
                    case Argument::Type::u64: {
                        auto parsed = my::parse<uint64_t>(arg);
                        if (!parsed) return makeArgTypeMismatch("u64", arg);
                        typedArgs.push_back(*parsed);
                        break;
                    }
                    case Argument::Type::f32: {
                        auto parsed = my::parse<float>(arg);
                        if (!parsed) return makeArgTypeMismatch("f32", arg);
                        typedArgs.push_back(*parsed);
                        break;
                    }
                    default: {
                        // string assumed by default
                        typedArgs.push_back(arg);
                        break;
                    }
                }
            }
            // if there were more provided arguments then command has the
            // following args will be implicitly read as string type
            for (; i < args.size(); ++i) typedArgs.push_back(args[i]);

            // then call all constraints with parsed args and stop if
            // any of constraint failed
            for (auto&& constraint : _constraints) {
                auto status = constraint.second(typedArgs);
                if (not status) return status;
            }

            // then call callback with parsed args
            return _callback(typedArgs);
        }

       private:
        Behaviour _callback{};                                 // one callback
        std::unordered_map<size_t, Behaviour> _constraints{};  // multiple constraints
        std::string _description = "no description";
        std::vector<Argument> _args{};
        std::string _name = "";
    };

    struct Commands {
        struct DefaultCommandPrinter {
            void operator()(const std::string& name,
                            const Command& command) const {
                my::printf("    {} - {}\n",
                           name, command.description());

                if (not command.args().empty()) {
                    my::printf("      call model:\n");

                    for (auto&& arg : command.args()) {
                        my::printf("        {} - {}\n",
                                   arg.name, arg.description);
                    }
                }

                my::printf("\n");
            }
        };

        template <class CommandPrinter = DefaultCommandPrinter>
        static Command helpCommand(Application& app,
                                   CommandPrinter print = {}) {
            Command help;

            help.describe("prints help message")
                .args({
                    {
                        .type = Command::Argument::Type::string,
                        .name = "target",
                        .description = "optional parameter, get the information about particular command",
                    },
                })
                .constrain(Constraints::makeArgsAmountConstraint(1, 2))
                .callback([&app, print](const auto& args) -> Status {
                    const auto& commands = app.commands();

                    if (args.size() == 1) {
                        for (auto&& pair : commands) {
                            print(pair.first, pair.second);
                        }
                        return {};
                    }

                    const auto command =
                        commands.find(str(args[1]));

                    if (command == commands.end()) {
                        return Status(
                            Status::Code::ArgumentValueMismatch,
                            "Cannot provide help for: \"{}\" no such command",
                            str(args[1]));
                    }

                    print(command->first, command->second);

                    return {};
                });

            return help;
        }

        static Command exitCommand(Application& app) {
            Command exit;
            exit.describe("exits from application")
                .constrain(Constraints::makeArgsAmountConstraint(1))
                .callback([&app](const auto& args) -> Status {
                    app.halt();
                    return {};
                });
            return exit;
        }
    };

    Command& record(const std::string& name) {
        _alias[name] = name;
        return _commands[name];
    }

    Command& record(std::initializer_list<std::string> names) {
        assert(names.size());

        const auto primary = *names.begin();

        auto commandIterator = _commands.find(primary);

        if (commandIterator == _commands.end()) {
            auto insertionResult =
                _commands.insert_or_assign(primary, Command{});

            assert(insertionResult.second);
            commandIterator = insertionResult.first;
        }

        for (auto&& name : names) {
            _alias[name] = primary;
        }

        return commandIterator->second;
    }

    auto& constrainAll(std::initializer_list<std::string> names,
                       Behaviour behaviour) {
        for (auto&& name : names) {
            auto aliasIterator = _alias.find(name);
            assert(aliasIterator != _alias.end());

            auto commandIterator = _commands.find(aliasIterator->second);
            assert(commandIterator != _commands.end());

            commandIterator->second.constrain(behaviour);
        }
        return *this;
    }

    const auto& commands() const { return _commands; }

    auto remove(const std::string& name) {
        return _commands.erase(name);
    }

    Status invoke(const ArgList& args) {
        const auto& name = args.front();

        auto aliasIterator = _alias.find(name);
        if (aliasIterator == _alias.end()) {
            return Status(Status::Code::NoSuchCommand,
                          "No such command: \"{}\"", name);
        }

        auto commandIterator = _commands.find(aliasIterator->second);
        assert(commandIterator != _commands.end());  // unexpected

        return commandIterator->second.invoke(args);
    }

    template <class Parser, class InputHandle,
              class OutputInputHelper, class ErrorHandle>
    void launch(Parser parse, InputHandle input,
                OutputInputHelper helper, ErrorHandle onError) {
        _running = true;

        while (running()) {
            helper();

            auto command = input();
            if (command.empty()) continue;

            auto status = invoke(parse(command));
            if (not status) onError(status);
        }
    }

    void launch() {
        launch(&DefaultIo::parse, &DefaultIo::getInput,
               &DefaultIo::printHelper, &DefaultIo::onError);
    }

    void halt() { _running = false; }
    bool running() const { return _running; }

   private:
    using CommandsMap = std::unordered_map<std::string, Command>;
    using AliasMap = std::unordered_map<std::string, std::string>;

    CommandsMap _commands{};
    AliasMap _alias{};

    bool _running = false;
};

using Command = Application::Command;
using Commands = Application::Commands;
using Constraints = Application::Constraints;
using Argument = Application::Command::Argument;

}  // namespace my::cli
