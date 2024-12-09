#include <chrono>
#include "opencmd.hpp"


int main() {
    using namespace opencmd;
    Logger::getInstance().setLevel(Logger::Level::DEBUG);

    Logger::getInstance().log("Implementing the tree", Logger::Level::INFO);

    // Root node
    auto rootNode = std::make_shared<NodeRoot>();

    auto can_start_of_frame = std::make_shared<ElementUnsignedInteger>("start_of_frame",1);
    auto can_identifier = std::make_shared<ElementUnsignedInteger>("identifier",11);
    auto can_rtr = std::make_shared<ElementUnsignedInteger>("RTR",1);
    auto can_ide = std::make_shared<ElementUnsignedInteger>("IDE",1);
    auto can_reserved = std::make_shared<ElementUnsignedInteger>("Reserved",1);
    auto can_dlc = std::make_shared<ElementUnsignedInteger>("DLC",4);
//    auto can_data = std::make_shared<NodeArray>("data",2);
    auto can_data = std::make_shared<NodeArray>("data","/DLC");
    auto can_crc = std::make_shared<ElementUnsignedInteger>("CRC",15);
    auto can_crc_delimiter = std::make_shared<ElementUnsignedInteger>("CRC_delimiter",1);
    auto can_ack = std::make_shared<ElementUnsignedInteger>("ACK",1);
    auto can_ack_delimiter = std::make_shared<ElementUnsignedInteger>("ACK_delimiter",1);
    auto can_eof = std::make_shared<ElementUnsignedInteger>("EOF",7);

    can_data->addChild(std::make_shared<ElementUnsignedInteger>("data",8));

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

/*     Logger::getInstance().log("from_bitstream", Logger::Level::INFO);
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
 */

    nlohmann::json result;
    BitStream bs_b64 = BitStream(std::string("AUBAIGhgL/A="));
    Logger::getInstance().log("bitstream_to_json", Logger::Level::INFO);
    auto start = std::chrono::high_resolution_clock::now();
    int retVal = rootNode->bitstream_to_json(bs_b64, result);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    Logger::getInstance().log("bitstream_to_json " + std::to_string(retVal) + " evaluation completed in " + std::to_string(duration.count()) + " ns", Logger::Level::INFO);

    Logger::getInstance().log("JSON: " + result.unflatten().dump(), Logger::Level::DEBUG);

    return 0;
}
