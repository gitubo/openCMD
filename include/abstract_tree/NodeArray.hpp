#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeArray : public TreeNode {
    private:
        bool is_array_size_fixed = true;
        size_t repetitions = 0;
        std::string repetition_reference = " = ";
        bool is_absolute_reference = true;
        std::vector<std::shared_ptr<TreeNode>> items;

    private:
        void prepareItems(const nlohmann::json& outputJson) {
            items.clear();
            size_t current_repetitions = repetitions;

            // If the size of the array depends on the value of another field
            // of the message, find it and align the repetitions attribute 
            if(!is_array_size_fixed){

                std::string repetition_reference_key = repetition_reference;
                
                // Evaluate the reference in case of not absolute value
                if(!is_absolute_reference){
                    repetition_reference_key = this->getFullName() + repetition_reference;
                }

                // Check if the reference is present
                if (!outputJson.contains(repetition_reference_key)) {
                    Logger::getInstance().log("Missing repetition reference " + repetition_reference_key + " in the evaluated json", Logger::Level::ERROR);
                    Logger::getInstance().log(outputJson.dump(), Logger::Level::ERROR);
                    return;
                } 

                // Get the value of the reference
                auto value = outputJson[repetition_reference_key];
                
                // Check the value is a valid integer
                if (!value.is_number_integer()) {
                    Logger::getInstance().log("Repetition reference value is not an integer", Logger::Level::ERROR);
                    return;
                }
                current_repetitions = value.get<int>();
            }
            items.reserve(current_repetitions*this->getChildren().size());
            // Create the array of elements to be used during the convertion
            for(size_t i = 0; i < current_repetitions; i++){
                for (auto& child : this->getChildren()) {
                    items.emplace_back(child->clone());
                }
            }
        };

    public:
        NodeArray() : TreeNode() {}

        NodeArray(std::string name) : TreeNode() {
            this->setName(name);
        }

        NodeArray(const NodeArray& other) : TreeNode(other), 
                is_array_size_fixed(other.is_array_size_fixed),
                repetitions(other.repetitions),
                repetition_reference(other.repetition_reference),
                is_absolute_reference(other.is_absolute_reference) { 
            items.reserve(other.items.size());
            for (const auto& item : other.items) {
                if (item) {
                    items.emplace_back(item->clone());
                }
            }
        }

/*         NodeArray& operator=(const NodeArray& other) {
            if (this != &other) {
                TreeNode::operator=(other); 
                this->is_array_size_fixed = other.is_array_size_fixed;
                this->repetitions = other.repetitions;
                this->repetition_reference = other.repetition_reference;
                this->is_absolute_reference = other.is_absolute_reference;
                items.clear();
                items.reserve(other.items.size());
                for (const auto& item : other.items) {
                    if (item) {
                        items.emplace_back(item->clone());
                    }
                }
            }
            return *this;
        } */

        virtual std::unique_ptr<TreeNode> clone() const override { 
            return std::make_unique<NodeArray>(*this);
        }

        void addAttribute(const std::string& key, const TreeNodeAttribute& attribute) override {
            TreeNode::addAttribute(key,attribute);
            
            if(key=="repetitions"){
                if(attribute.isInteger()) {
                    repetitions = attribute.getInteger().value();
                    is_array_size_fixed = true;
                    repetition_reference = " = ";
                    is_absolute_reference = false;
                } else if(attribute.isString()){
                    repetitions = 0;
                    is_array_size_fixed = false;
                    repetition_reference = attribute.getString().value();
                } else {
                    Logger::getInstance().log("Attribute <repetitions> is not a string or an integer", Logger::Level::ERROR);
                }
            } else if(key=="is_absolute_reference"){
                if(!attribute.isBool()){
                    Logger::getInstance().log("Attribute <is_absolute_path> is not a boolean", Logger::Level::ERROR);
                } else {
                    is_absolute_reference = attribute.getBool().value();
                }
            }
        }

        int getRepetitions() { return repetitions; }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) override {
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

        int json_to_bitstream(const nlohmann::json& inputJson, BitStream& bitStream) override {
            
            if(!inputJson.contains(this->getFullName()+"/0")){
                Logger::getInstance().error("Key <"+this->getFullName()+"/0"+"> not found in the provided json object or the related value is not an array");
                return 100;      
            }
            items.clear();
            for (auto& [key, val] : inputJson.items()){
                if(key.rfind(this->getFullName(), 0) == 0){
                    for (auto& child : this->getChildren()) {
                        items.emplace_back(child->clone());
                    }
                }
            }
            int index = 0;
            for (auto& item : items) {
                item->setName(std::to_string(index));
                item->setParentName(this->getFullName());
                item->json_to_bitstream(inputJson, bitStream);
                index++;
            }
            
            return 0;
        };


    };
}

