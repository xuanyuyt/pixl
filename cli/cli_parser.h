//
// Copyright (c) 2017. See AUTHORS file.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef PIXL_CLI_PARSER_H
#define PIXL_CLI_PARSER_H

#include <vector>
#include "log.h"

// A CliArg has the form -x [optional value]
struct CliArg {
    CliArg(std::string name, std::string description, bool hasParam, bool required = false)
        : name(name), description(description), hasParam(hasParam), required(required) {}

    std::string name;
    std::string description;
    std::string param;
    bool hasParam;
    bool required;
};


// A CliSubcommand must follow the application name.
//
// A exmaple would be git, where you have different subcommands for different kind
// of functionality. E.g. git log, git commit, git diff, etc.
class CliSubcommand {
    friend class CliParser;

public:
    // The name of the command. Eg. git log, where 'log' is the name
    CliSubcommand(std::string name) : name(name) {}
    ~CliSubcommand() {}

    // Adds a new argument to this subcommand.
    void addArg(CliArg* arg) { this->args.push_back(arg); }

    // The name of the subcommand.
    std::string name;

private:
    // List of arguements of this subcommand.
    std::vector<CliArg*> args;
};


// The result returned by the parser
struct CliParserResult {
    CliParserResult() {}
    ~CliParserResult() {}

    // Checks if there is an argument in the specs
    CliArg* getArgument(std::string name) {
        for (auto a : this->args) {
            if (a->name == name)
                return a;
        }
        return nullptr;
    }

    std::string errorMessage;
    std::vector<CliArg*> args;
    CliSubcommand* subcommand = nullptr;
};


// A command line parser.
class CliParser {
public:
    CliParser() {}
    ~CliParser() {}

    // Adds a subcommand
    void addSubcommand(CliSubcommand* sub) { this->subcommands.push_back(sub); }

    // Adds an argument
    void addArg(CliArg* arg) { this->args.push_back(arg); }

    // Prints the generated help text
    std::string help() { return "help"; }

    // Parses the arguments provided by the main function.
    // Returns true if parsing suceeded.
    // args is the output paramter.
    bool parse(int argc, char** argv, CliParserResult& result) {
        if (argc < 2) {
            result.errorMessage = "No args";
            return false;
        }

        // check wether it's a subcommand-style command or not
        CliSubcommand* subcommand = getSubcommand(argv[1]);
        if (subcommand != nullptr) {
            result.subcommand = subcommand;
            return processSubcommand(argc - 2, argv + 2, result);
        }

        // ... otherwise parse it as normal argument style command
        return processArguments(argc - 1, argv + 1, result);
    }

private:
    std::vector<CliSubcommand*> subcommands;
    std::vector<CliArg*> args;

    // Checks if there is a subcommand in the specs
    CliSubcommand* getSubcommand(std::string name) {
        for (auto c : this->subcommands) {
            if (c->name == name)
                return c;
        }
        return nullptr;
    }

    // Checks if there is an argument in the specs
    CliArg* getArgument(std::string name) {
        for (auto a : this->args) {
            if (a->name == name)
                return a;
        }
        return nullptr;
    }

    // Parses subcommand style commands
    bool processSubcommand(int argc, char** argv, CliParserResult& result) {
        auto cmd = result.subcommand;
        LOG_DEBUG("Processing subcommand " << cmd->name);

        // add subcommand arg specs to parser arg specs list
        // so this actually says args.addAll(cmd->args)
        args.clear();
        args.insert(args.end(), cmd->args.begin(), cmd->args.end());

        return processArguments(argc, argv, result);
    }

    // Parses arguments both with and without values (like this: -l, -l value)
    // Returns false if something went wrong.
    bool processArguments(int argc, char** argv, CliParserResult& result) {
        // Parse arguments
        int i = 0;
        while (i < argc) { // flag with value
            if (i + 2 <= argc && argv[i][0] == '-' && argv[i + 1][0] != '-') {
                auto name = argv[i] + 1;
                auto value = argv[i + 1];
                LOG_DEBUG("flag with arg: " << name << ": " << value);
                if (!processArgument(name, value, result)) {
                    return false;
                }

                i += 2;
            } else if (argv[i][0] == '-') { // simple flag, no value
                auto name = argv[i] + 1;
                LOG_DEBUG("simple flag: " << argv[i]);
                if (!processFlag(name, result)) {
                    return false;
                }

                i += 1;
            } else { // some weird input
                result.errorMessage = "Weird input";
                return false;
            }
        }

        // Check if required parameters are present
        for (auto arg : this->args) {
            if (!arg->required)
                continue;

            if (result.getArgument(arg->name) == nullptr) {
                result.errorMessage = "Missing required argument: " + arg->name;
                return false;
            }
        }

        return true;
    }

    // Processes an argument without value, like this: -f
    // Returns false if prcocessing failed.
    bool processFlag(char* name, CliParserResult& result) {
        auto arg = getArgument(name);
        if (arg != nullptr) {
            if (arg->hasParam) {
                result.errorMessage = "Argument " + std::string(name) + " must have a parameter";
                return false;
            }
            result.args.push_back(arg);
        } else {
            result.errorMessage = "Unkown argument " + std::string(name);
            return false;
        }

        return true;
    }

    // Processes an argument with a parameter value, like this: -l value
    // Returns false if prcocessing failed.
    bool processArgument(char* name, char* value, CliParserResult& result) {
        auto arg = getArgument(name);
        if (arg != nullptr) {
            if (!arg->hasParam) {
                result.errorMessage = "Argument " + std::string(name) + " can not have a parameter";
                return false;
            }
            arg->param = value;
            result.args.push_back(arg);
        } else {
            result.errorMessage = "Unkown argument " + std::string(name);
            return false;
        }

        return true;
    }
};

#endif