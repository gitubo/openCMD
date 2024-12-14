#pragma once

#include <iostream>

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
            creators[className] = []() -> std::unique_ptr<TreeElement> {
                return std::make_unique<T>();
            };
        }

        std::unique_ptr<TreeElement> create(const std::string& className) {
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

