#pragma once

#include <iostream>

#include "../logger/Logger.hpp"
#include "TreeElement.hpp"

namespace opencmd {

    class TreeFactory{

    public:

        static TreeFactory& getInstance() {
            static TreeFactory instance;
            return instance;
        }


        template <typename T>
        void registerClass(const std::string& className) {
            creators[className] = [&className]() -> std::unique_ptr<TreeElement> {
                Logger::getInstance().log("Registering class with classname <" + className + ">", Logger::Level::DEBUG);
                return std::make_unique<T>();
            };
        }

        std::unique_ptr<TreeElement> create(const std::string& className) {
            Logger::getInstance().log("Invoking create for class name <" + className + ">", Logger::Level::DEBUG);
            if (creators.find(className) != creators.end()) {
                Logger::getInstance().log("Class found", Logger::Level::DEBUG);
            } else {
                Logger::getInstance().log("Class NOT found", Logger::Level::DEBUG);
            }


            if (creators.find(className) != creators.end()) {
                return creators[className]();
            }
            Logger::getInstance().log("Failed to create class for classname <" + className + ">", Logger::Level::ERROR);
            return nullptr;
            
        }

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<TreeElement>()>> creators;

    private:
        TreeFactory() = default;
        TreeFactory& operator=(const TreeFactory&) = delete;

    };
}

