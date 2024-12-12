#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeArray : public TreeNode {
    private:
        bool is_array_size_fixed;
        size_t repetitions;
        std::string repetition_reference;
        bool is_absolute_repetition_reference_path;
        std::vector<std::shared_ptr<TreeElement>> items;

    private:
        void prepareItems(const nlohmann::json& outputJson) {
            items.clear();
            size_t current_repetitions = repetitions;

            // If the size of the array depends on the value of another field
            // of the message, find it and align the repetitions attribute 
            if(!is_array_size_fixed){

                std::string repetition_reference_key = repetition_reference;
                
                // Evaluate the reference in case of not absolute value
                if(!is_absolute_repetition_reference_path){
                    repetition_reference_key = this->getFullName() + repetition_reference;
                }

                // Check if the reference is present
                if (!outputJson.contains(repetition_reference_key)) {
                    Logger::getInstance().log("Missing repetition reference " + repetition_reference_key + " in the evaluated json", Logger::Level::ERROR);
                    Logger::getInstance().log(outputJson.dump(), Logger::Level::ERROR);
                    return;
                } 

                // Getthe value of the reference
                auto value = outputJson[repetition_reference_key];
                Logger::getInstance().log("Repetition reference " + repetition_reference_key + " found with value " + value.dump(), Logger::Level::DEBUG);
                
                // Check the value is a valid integer
                if (!value.is_number_integer()) {
                    Logger::getInstance().log("Repetition reference value is not an integer", Logger::Level::ERROR);
                    return;
                }

                current_repetitions = value.get<int>();
            }

            // Create the array of elements to be used during the convertion
            for(size_t i = 0; i < current_repetitions; i++){
                for (auto& child : this->getChildren()) {
                    items.push_back(child->clone());
                }
            }
        };

    public:
        NodeArray() 
            : TreeNode(), repetitions(0), is_array_size_fixed(true),
              repetition_reference(""), is_absolute_repetition_reference_path(false) {}

        NodeArray(std::string name, size_t repetitions) 
            : TreeNode(), repetitions(repetitions), is_array_size_fixed(true),
              repetition_reference(""), is_absolute_repetition_reference_path(false) {
                this->setName(name);
              }

        NodeArray(std::string name, std::string repetition_reference, bool is_absolute_repetition_reference_path = true) 
            : TreeNode(), repetitions(0), is_array_size_fixed(false),
              repetition_reference(repetition_reference), is_absolute_repetition_reference_path(is_absolute_repetition_reference_path) {
                this->setName(name);
              }

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

        std::unique_ptr<TreeElement> clone () const override{
            return std::make_unique<NodeArray>(*this);
        }

        int getRepetitions() { return repetitions; }

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

        int json_to_bitstream(nlohmann::json&, BitStream&) override {
            return 0;
        };


    };
}

