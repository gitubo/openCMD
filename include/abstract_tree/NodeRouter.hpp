#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeRouter : public TreeNode {
    private:
        size_t bitLength = 0;
        std::map<int32_t, std::string> routingTable;

    public:
        NodeRouter() : TreeNode() {}

        NodeRouter(std::string name) : TreeNode() {
            this->setName(name);
        }

        NodeRouter(const NodeRouter& other) : TreeNode(other), 
            bitLength(other.bitLength),
            routingTable(other.routingTable) {}

/*         NodeRouter& operator=(const NodeRouter& other) {
            if (this != &other) {
                TreeNode::operator=(other); 
                this->routingTable = other.routingTable;
            }
            return *this;
        } */

        virtual std::unique_ptr<TreeNode> clone() const override { 
            return std::make_unique<NodeRouter>(*this);
        }

        void addAttribute(const std::string& key, const TreeNodeAttribute& attribute) override {
            TreeNode::addAttribute(key,attribute);
            
            if(key=="bit_length"){
                if(!attribute.isInteger()){
                    Logger::getInstance().log("Attribute <bit_length> is not an integer", Logger::Level::ERROR);
                } else {
                    bitLength = attribute.getInteger().value();
                    if(bitLength<=0){
                        Logger::getInstance().log("Attribute <bit_length> is not valid (<=0)", Logger::Level::ERROR);
                        bitLength = 0;
                    }
                }
            } else if(key=="routing_table"){
                if(attribute.isArray()) {
                    std::cout << attribute.to_string() << std::endl;
                    for(auto item : attribute.getArray().value()){
                        std::cout << item.to_string() << std::endl;
                    }
                } else {
                    Logger::getInstance().log("Attribute <routing_table> is not an array", Logger::Level::ERROR);
                }
            }
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) override {
            return 0;
        };

        int json_to_bitstream(const nlohmann::json& inputJson, BitStream& bitStream) override {
            return 0;
        };


    };
}

