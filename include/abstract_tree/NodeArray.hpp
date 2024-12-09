#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeArray : public TreeNode {
    private:
        size_t repetitions;
        std::vector<std::shared_ptr<TreeComponent>> items;

    public:
        NodeArray(std::string name, size_t repetitions) : TreeNode(name), repetitions(repetitions) {}

        NodeArray(const NodeArray& other) : TreeNode(other) { 
            for (const auto& item : other.items) {
                if (item) {
                    items.push_back(item->clone()); 
                }
            }
        }

        NodeArray& operator=(const NodeArray& other) {
            if (this != &other) {
                this->setName(other.getName());
                items.clear();
                for (const auto& item : other.items) {
                    if (item) {
                        items.push_back(item->clone());
                    }
                }
            }
            return *this;
        }

        std::unique_ptr<TreeComponent> clone () const override{
            return std::make_unique<NodeArray>(*this);
        }

        BitStream to_bitstream() const override {
            std::cout << "NodeArray Override bitstream start" << std::endl;
            TreeNode::to_bitstream();
            std::cout << "NodeArray Override bitstream end" << std::endl;
        }

        nlohmann::json to_json() const override {
            nlohmann::json result;
            std::string base_flattened_key = std::string("/") + std::string(this->getName()) + std::string("/"); 
            int array_index = 0;
            for (auto& item : items) {
                std::string flattened_key = base_flattened_key + std::to_string(array_index);
                nlohmann::json j;
                j[flattened_key] = item->to_json()[flattened_key];
                result.update(j);
            }
            return result;
        }

        void from_bitstream(BitStream& bitstream) override {
            items.clear();
            for(int i = 0; i < repetitions; i++){
                for (auto& child : this->getChildren()) {
                    items.push_back(child->clone());
                }
            }
            for (auto& item : items) {
                item->from_bitstream(bitstream);
            }
        };
        void from_json(const nlohmann::json json) override {};

        int getRepetitions() { return repetitions; }

    };
}

