#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeArray : public TreeNode {
    private:
        bool is_array_size_fixed;
        size_t repetitions;
        std::string repetition_reference;
        bool is_absolute_repetition_reference_path;
        std::vector<std::shared_ptr<TreeComponent>> items;

    private:
        void prepareItems(const nlohmann::json& outputJson) {
            items.clear();
            size_t current_repetitions = repetitions;
            if(!is_array_size_fixed){
                current_repetitions = 2;
                // find the value of the proposed element
                if(is_absolute_repetition_reference_path){
                    // ...
                } else {
                    // ...
                }
            }
            for(size_t i = 0; i < current_repetitions; i++){
                for (auto& child : this->getChildren()) {
                    items.push_back(child->clone());
                }
            }
        };

    public:
        NodeArray(std::string name, size_t repetitions) 
            : TreeNode(name), repetitions(repetitions), is_array_size_fixed(true),
              repetition_reference(""), is_absolute_repetition_reference_path(false) {}

        NodeArray(std::string name, std::string repetition_reference, bool is_absolute_repetition_reference_path = true) 
            : TreeNode(name), repetitions(0), is_array_size_fixed(false),
              repetition_reference(repetition_reference), is_absolute_repetition_reference_path(is_absolute_repetition_reference_path) {}

        NodeArray(const NodeArray& other) : TreeNode(other), 
                is_array_size_fixed(other.is_array_size_fixed),
                repetitions(other.repetitions),
                repetition_reference(other.repetition_reference),
                is_absolute_repetition_reference_path(other.is_absolute_repetition_reference_path) { 
            for (const auto& item : other.items) {
                if (item) {
                    items.push_back(item->clone()); 
                }
            }
        }

        NodeArray& operator=(const NodeArray& other) {
            if (this != &other) {
                TreeNode::operator=(other); // Chiama l'operatore di assegnazione della classe base
                this->is_array_size_fixed = other.is_array_size_fixed;
                this->repetitions = other.repetitions;
                this->repetition_reference = other.repetition_reference;
                this->is_absolute_repetition_reference_path = other.is_absolute_repetition_reference_path;
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
            return TreeNode::to_bitstream();
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::json& outputJson) override {
            prepareItems(outputJson);
            std::string array_key_basename = std::string(this->getFullName()) + std::string("/"); 
            int array_index = 0;
            for (auto& item : items) {
                // Evaluate the item
                item->bitstream_to_json(bitStream, outputJson);
                
                // Prepare the a local json with the correct key (/array/n)
                // and the value from the evaluated item
                nlohmann::json j;
                std::string array_key = array_key_basename + std::to_string(array_index);
                j[array_key] = outputJson[item->getFullName()];
                
                // Remove the current item from the actual evaluated json 
                outputJson.erase(item->getFullName());

                // Add the local json to the global json
                outputJson.update(j);
                array_index++;
            }
            return 0;
        };

        void from_json(const nlohmann::json json) override {};

        int getRepetitions() { return repetitions; }

    };
}

