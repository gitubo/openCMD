#include "../../include/engine/Engine.hpp"

using namespace opencmd;

std::shared_ptr<TreeComponent> Engine::evaluateElement(const SchemaElement& element){
    Logger::getInstance().log("Calling evaluateElement with object: " + element.to_string(), Logger::Level::WARNING);
    std::shared_ptr<TreeComponent> node = std::make_shared<TreeComponent>();
    return node;
}

std::shared_ptr<NodeRoot> Engine::evaluateSchema(const Schema& schema){
    std::shared_ptr<NodeRoot> node_root = std::make_shared<NodeRoot>();
    for(auto element : schema.getStructure()){
        auto result = evaluateElement(element);
        node_root->addChild(result);
    }
    return node_root;
}