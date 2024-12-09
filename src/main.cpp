#include <chrono>
#include "opencmd.hpp"


int main() {
    using namespace opencmd;
    Logger::getInstance().setLevel(Logger::Level::DEBUG);

    Logger::getInstance().log("Implementing the tree", Logger::Level::INFO);

    // Root node
    auto rootNode = std::make_shared<NodeRoot>();

    // Integer
    auto can_start_of_frame = std::make_shared<ElementInteger>("start_of_frame",0,1);
    auto can_identifier = std::make_shared<ElementInteger>("identifier",0,11);
    auto can_rtr = std::make_shared<ElementInteger>("RTR",0,1);
    auto can_ide = std::make_shared<ElementInteger>("IDE",0,1);
    auto can_reserved = std::make_shared<ElementInteger>("Reserved",0,1);
    auto can_dlc = std::make_shared<ElementInteger>("DLC",0,4);
    auto can_crc = std::make_shared<ElementInteger>("CRC",0,15);
    auto can_crc_delimiter = std::make_shared<ElementInteger>("CRC_delimiter",0,1);
    auto can_ack = std::make_shared<ElementInteger>("ACK",0,1);
    auto can_ack_delimiter = std::make_shared<ElementInteger>("ACK_delimiter",0,1);
    auto can_eof = std::make_shared<ElementInteger>("EOF",0,7);

    auto can_data = std::make_shared<NodeArray>("data",2);
    can_data->addChild(std::make_shared<ElementInteger>("data",0,8));

    rootNode->addChild(can_start_of_frame);
    rootNode->addChild(can_identifier);
    rootNode->addChild(can_rtr);
    rootNode->addChild(can_ide);
    rootNode->addChild(can_reserved);
    rootNode->addChild(can_dlc);
    rootNode->addChild(can_data);
    rootNode->addChild(can_crc);
    rootNode->addChild(can_crc_delimiter);
    rootNode->addChild(can_ack);
    rootNode->addChild(can_ack_delimiter);
    rootNode->addChild(can_eof);

    Logger::getInstance().log("from_bitstream", Logger::Level::INFO);
    auto start = std::chrono::high_resolution_clock::now();
    BitStream bs_b64 = BitStream(std::string("AUBAIGhgL/A="));
    rootNode->from_bitstream(bs_b64);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    Logger::getInstance().log("from_bitstream evaluation completed in " + std::to_string(duration.count()) + " ns", Logger::Level::INFO);
                    
    Logger::getInstance().log("to_json", Logger::Level::INFO);
    start = std::chrono::high_resolution_clock::now();
    auto result = rootNode->to_json();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    Logger::getInstance().log("to_json evaluation completed in " + std::to_string(duration.count()) + " ns", Logger::Level::INFO);

    auto unflattened_json = result.unflatten();
    Logger::getInstance().log("JSON: " + unflattened_json.dump(), Logger::Level::DEBUG);

    return 0;
}
