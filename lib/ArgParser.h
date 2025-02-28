//
// Created by NeSai on 16/11/2024.
//

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <vector>
#include <string>

namespace ArgumentParser {

    class ArgParser {

        template<typename T>
        class Argument {
        public:
            std::string name;
            char shortName = '_';
            T value;
            std::vector<T> values;
            T* storedValue = nullptr;
            std::vector<T>* storedValues = nullptr;
            int multiValue = -1;
            std::string description;
            bool hasDefault = false;
            bool wasUpdated = false;
            bool isPositional = false;

            Argument<T>& Default(T defaultValue) {
                value = defaultValue;
                hasDefault = true;
                return *this;
            }

            Argument<T>& StoreValue(T& value) {
                storedValue = &value;
                this->value = value;
                return *this;
            }

            Argument<T>& MultiValue(size_t minArgsCount = 1) {
                multiValue = minArgsCount;
                return *this;
            }

            Argument<T>& StoreValues(std::vector<T>& values) {
                storedValues = &values;
                return *this;
            }

            Argument<T>& Positional() {
                isPositional = true;
                return *this;
            }

            void Update(T newValue) {
                if (this->multiValue != -1) {
                    this->values.push_back(newValue);
                    if (this->storedValues)
                        this->storedValues->push_back(this->values.back());
                }
                else {
                    this->value = newValue;
                    if (this->storedValue)
                        *this->storedValue = this->value;
                }

                this->wasUpdated = true;
            }
        };

        std::vector<Argument<int>*> _intArguments;
        std::vector<Argument<std::string>*> _stringArguments;
        std::vector<Argument<bool>*> _flagArguments;
        Argument<int>* help;

        void ParseInt(std::vector<std::string> args);
        void ParseString(std::vector<std::string> args);


    public:
        std::string Name;

        bool Parse(std::vector<std::string> args);

        bool Help();


        // Adders
        Argument<int>& AddIntArgument(std::string paramName);
        Argument<int>& AddIntArgument(std::string paramName, std::string description);
        Argument<int>& AddIntArgument(char shortParamName, std::string paramName);
        Argument<int>& AddIntArgument(char shortParamName, std::string paramName, std::string description);

        Argument<std::string>& AddStringArgument(std::string paramName);
        Argument<std::string>& AddStringArgument(std::string paramName, std::string description);
        Argument<std::string>& AddStringArgument(char shortParamName, std::string paramName);
        Argument<std::string>& AddStringArgument(char shortParamName, std::string paramName, std::string description);

        Argument<bool>& AddFlag(char flagShortName, std::string flagName);
        Argument<bool>& AddFlag(char flagShortName, std::string flagName, std::string description);

        void AddHelp(char helpShortName, std::string helpName, std::string description);


        // Getters
        int GetIntValue(std::string key);
        int GetIntValue(std::string key, int index);

        std::string GetStringValue(std::string key);
        std::string GetStringValue(std::string key, int index);

        bool GetFlag(std::string key);

        std::string HelpDescription();
        ArgParser(std::string name);
    };

}

#endif //ARGPARSER_H
