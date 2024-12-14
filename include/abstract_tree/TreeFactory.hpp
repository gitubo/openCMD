#pragma once

#include <iostream>

#include "TreeNode.hpp"

namespace opencmd {

    class TreeFactory{

    public:

        static TreeFactory& getInstance() {
            static TreeFactory instance;
            return instance;
        }


        template <typename T>
        void registerClass(const std::string& className) {
            creators[className] = []() -> std::unique_ptr<TreeNode> {
                return std::make_unique<T>();
            };
        }

        std::unique_ptr<TreeNode> create(const std::string& className) {
            if (creators.find(className) != creators.end()) {
                return creators[className]();
            }
            Logger::getInstance().log("Failed to create class for classname <" + className + ">", Logger::Level::ERROR);
            return nullptr;
            
        }

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<TreeNode>()>> creators;

    private:
        TreeFactory() = default;
        TreeFactory& operator=(const TreeFactory&) = delete;

    };
}

