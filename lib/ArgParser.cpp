//
// Created by NeSai on 16/11/2024.
//

#include "ArgParser.h"
#include <regex>

std::regex sepFullRegex(R"(--([^=]+))");
std::regex sepShortRegex(R"(-([^=]))");

std::regex flagFullRegex(R"(--([^=]+))");
std::regex flagShortRegex(R"(-([^=]+))");

std::regex intFullRegex(R"(--([^=]+)=([0-9]+))");
std::regex intShortRegex(R"(-([^=])=([0-9]+))");

std::regex stringFullRegex(R"(--([^=]+)=([^=]+))");
std::regex stringShortRegex(R"(-([^=])=([^=]+))");

using namespace ArgumentParser;

bool ArgParser::Parse(std::vector<std::string> args) {

    for (int i = 1; i < args.size(); i++) {
        std::string &arg = args[i];

        if (arg == "--help")
            return true;

        // flags
        if (std::regex_match(arg, flagFullRegex) ||
            std::regex_match(arg, flagShortRegex)) {
            std::string param = arg.substr(1);

            if (param[0] == '-') {
                param.erase(0, 1);

                auto argumentIt = std::find_if(
                    _flagArguments.begin(),
                    _flagArguments.end(),
                    [&param](const Argument<bool> *ar) {
                        return (ar->name == param);
                    });

                if (argumentIt != _flagArguments.end())
                    (*argumentIt)->Update(!(*argumentIt)->value);
            }
            else {
                std::vector<Argument<bool> *> flags;
                for (auto f: param) {

                    auto flagIt = std::find_if(
                        _flagArguments.begin(),
                        _flagArguments.end(),
                        [&f](const Argument<bool> *ar) {
                            return ar->shortName == f;
                        });

                    if (flagIt != _flagArguments.end())
                        flags.push_back(*flagIt);
                }
                for (auto flag: flags)
                    flag->Update(!flag->value);
            }
        }

        // intArguments
        if (std::regex_match(arg, intFullRegex) ||
            std::regex_match(arg, intShortRegex)) {
            std::string param = arg.substr(1, arg.find('=') - 1);
            int value = std::stoi(arg.substr(arg.find('=') + 1));

            if (param[0] == '-') param.erase(0, 1);

            auto argumentIt = std::find_if(
                _intArguments.begin(),
                _intArguments.end(),
                [&param](const Argument<int> *ar) {
                    return (ar->name == param || ar->shortName == param[0]);
                });

            if (argumentIt != _intArguments.end())
                (*argumentIt)->Update(value);
        }

        // stringArguments
        else if (std::regex_match(arg, stringFullRegex) ||
                 std::regex_match(arg, stringShortRegex)) {
            std::string param = arg.substr(1, arg.find('=') - 1);
            std::string value = arg.substr(arg.find('=') + 1);

            if (param[0] == '-') param.erase(0, 1);

            auto argumentIt = std::find_if(
                _stringArguments.begin(),
                _stringArguments.end(),
                [&param](const Argument<std::string> *ar) {
                    return (ar->name == param || ar->shortName == param[0]);
                });

            if (argumentIt != _stringArguments.end())
                (*argumentIt)->Update(value);
        }

        // separated value
        else if (std::regex_match(arg, sepFullRegex) ||
                 std::regex_match(arg, sepShortRegex)) {
            std::string param = arg.substr(1, arg.find('=') - 1);
            if (param[0] == '-') param.erase(0, 1);

            auto intArgumentIt = std::find_if(
                _intArguments.begin(),
                _intArguments.end(),
                [&param](const Argument<int> *ar) {
                    return (ar->name == param || ar->shortName == param[0]);
                });

            auto stringArgumentIt = std::find_if(
                _stringArguments.begin(),
                _stringArguments.end(),
                [&param](const Argument<std::string> *ar) {
                    return (ar->name == param || ar->shortName == param[0]);
                });

            if (intArgumentIt != _intArguments.end()) {
                (*intArgumentIt)->Update(std::stoi(args[i + 1]));
                i++;
            }
            else if (stringArgumentIt != _stringArguments.end()) {
                (*stringArgumentIt)->Update(args[i + 1]);
                i++;
            }
        }

        // positional arguments
        else {
            auto intArgumentIt = std::find_if(
                _intArguments.begin(),
                _intArguments.end(),
                [](const Argument<int> *ar) { return ar->isPositional; });

            auto stringArgumentIt = std::find_if(
                _stringArguments.begin(),
                _stringArguments.end(),
                [](const Argument<std::string> *ar) { return ar->isPositional; });

            if (intArgumentIt != _intArguments.end())
                (*intArgumentIt)->Update(std::stoi(arg));
            else if (stringArgumentIt != _stringArguments.end())
                (*stringArgumentIt)->Update(arg);
        }
    }

    // verification
    for (auto argument: _intArguments) {
        if (!argument->hasDefault && !argument->wasUpdated)
            return false;
        if (argument->multiValue != -1 &&
            argument->multiValue > argument->values.size())
            return false;
    }

    for (auto argument: _stringArguments) {
        if (!argument->hasDefault && !argument->wasUpdated)
            return false;
        if (argument->multiValue != -1 &&
            argument->multiValue > argument->values.size())
            return false;
    }

    return true;
}

bool ArgParser::Help() {
    return true;
}

ArgParser::Argument<int> &ArgParser::AddIntArgument(std::string paramName) {
    auto *argument = new Argument<int>;
    argument->name = paramName;
    _intArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<int> &ArgParser::AddIntArgument(std::string paramName, std::string description) {
    auto *argument = new Argument<int>;
    argument->name = paramName;
    argument->description = description;
    _intArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<int> &ArgParser::AddIntArgument(char shortParamName, std::string paramName) {
    auto *argument = new Argument<int>;
    argument->name = paramName;
    argument->shortName = shortParamName;
    _intArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<int> &
ArgParser::AddIntArgument(char shortParamName, std::string paramName, std::string description) {
    auto *argument = new Argument<int>;
    argument->name = paramName;
    argument->shortName = shortParamName;
    argument->description = description;
    _intArguments.push_back(argument);
    return *argument;
}


ArgParser::Argument<std::string> &ArgParser::AddStringArgument(std::string paramName) {
    auto *argument = new Argument<std::string>;
    argument->name = paramName;
    _stringArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<std::string> &ArgParser::AddStringArgument(std::string paramName, std::string description) {
    auto *argument = new Argument<std::string>;
    argument->name = paramName;
    argument->description = description;
    _stringArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<std::string> &ArgParser::AddStringArgument(char shortParamName, std::string paramName) {
    auto *argument = new Argument<std::string>;
    argument->name = paramName;
    argument->shortName = shortParamName;
    _stringArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<std::string> &ArgParser::AddStringArgument(char shortParamName, std::string paramName,
                                                               std::string description) {
    auto *argument = new Argument<std::string>;
    argument->name = paramName;
    argument->shortName = shortParamName;
    argument->description = description;
    _stringArguments.push_back(argument);
    return *argument;
}

ArgParser::Argument<bool> &ArgParser::AddFlag(char flagShortName, std::string flagName) {
    auto *flag = new Argument<bool>;
    flag->name = flagName;
    flag->shortName = flagShortName;
    _flagArguments.push_back(flag);
    return *flag;
}

ArgParser::Argument<bool> &ArgParser::AddFlag(char flagShortName, std::string flagName, std::string description) {
    auto *flag = new Argument<bool>;
    flag->name = flagName;
    flag->shortName = flagShortName;
    flag->description = description;
    _flagArguments.push_back(flag);
    return *flag;
}

void ArgParser::AddHelp(char helpShortName, std::string helpName, std::string description) {
    help = new Argument<int>;
    help->name = helpName;
    help->shortName = helpShortName;
    help->description = description;
}

int ArgParser::GetIntValue(std::string key) {
    return (*std::find_if(
        _intArguments.begin(),
        _intArguments.end(),
        [key](Argument<int> *arg) { return arg->name == key; }))->value;
}

int ArgParser::GetIntValue(std::string key, int index) {
    return (*std::find_if(
        _intArguments.begin(),
        _intArguments.end(),
        [key](Argument<int> *arg) { return arg->name == key; }))->values.at(index);
}

std::string ArgParser::GetStringValue(std::string key) {
    return (*std::find_if(
        _stringArguments.begin(),
        _stringArguments.end(),
        [key](Argument<std::string> *arg) { return arg->name == key; }))->value;
}

std::string ArgParser::GetStringValue(std::string key, int index) {
    return (*std::find_if(
        _stringArguments.begin(),
        _stringArguments.end(),
        [key](Argument<std::string> *arg) { return arg->name == key; }))->values.at(index);
}

bool ArgParser::GetFlag(std::string key) {
    return (*std::find_if(
        _flagArguments.begin(),
        _flagArguments.end(),
        [key](Argument<bool> *flag) { return flag->name == key; }))->value;
}

std::string ArgParser::HelpDescription() {
    std::stringstream description;

    description << Name << '\n';
    description << help->description << "\n\n";

    for (auto arg : _stringArguments) {
        if (arg->shortName != '_')
            description << '-' << arg->shortName << ",";
        else
            description << "   ";
        description << "  --" + arg->name << "=<string>";
        description << ",  " + arg->description;
        if (arg->multiValue != -1)
            description << " [repeated, min args = " << arg->multiValue << "]";
        if (arg->hasDefault)
            description << " [default = " << (arg->hasDefault ? "true" : "false") << "]";
        description << '\n';
    }

    for (auto arg : _flagArguments) {
        if (arg->shortName != '_')
            description << '-' << arg->shortName << ",";
        else
            description << "   ";
        description << "  --" + arg->name;
        description << ",  " + arg->description;
        if (arg->hasDefault)
            description << " [default = " << (arg->hasDefault ? "true" : "false") << "]";
        description << '\n';
    }

    for (auto arg : _intArguments) {
        if (arg->shortName != '_')
            description << '-' << arg->shortName << ",";
        else
            description << "   ";
        description << "  --" + arg->name << "=<int>";
        description << ",  " + arg->description;
        if (arg->multiValue != -1)
            description << " [repeated, min args = " << arg->multiValue << "]";
        if (arg->hasDefault)
            description << " [default = " << (arg->hasDefault ? "true" : "false") << "]";
        description << '\n';
    }

    description << '\n';

    description << "-h, --help Display this help and exit\n";

    return description.str();
}

ArgParser::ArgParser(std::string name) {
    Name = name;
}
