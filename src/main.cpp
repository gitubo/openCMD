#include <chrono>
#include "opencmd.hpp"


int main() {
    using namespace opencmd;
    Logger& logger = Logger::getInstance();
    logger.setSeverity(Logger::Level::DEBUG);

    const std::string fileName = "../catalog/can.json";
    nlohmann::json jsonData;
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        logger.log("Errore: Impossibile aprire il file " + fileName, Logger::Level::ERROR);
        return 1;
    }
    try {
        inputFile >> jsonData;
        inputFile.close();
        logger.log("File JSON caricato con successo", Logger::Level::INFO);
    } catch (const nlohmann::json::parse_error& e) {
        logger.log("Catalog file not correctly parsed: " + std::string(e.what()), Logger::Level::ERROR);
        return 2;
    }

    if(SchemaCatalog::getInstance().parseSchema("can", jsonData) < 0){
        logger.log("Error in parsing the schema", Logger::Level::ERROR);
    }

    logger.log("Implementing the tree", Logger::Level::INFO);

    auto rn = Engine::evaluateSchema(*SchemaCatalog::getInstance().getSchema("can"));
    if(!rn){
        Logger::getInstance().log("Error in evaluating schema", Logger::Level::ERROR);
        return 1;
    }


    // Root node
    auto rootNode = std::make_shared<NodeRoot>();

    auto can_start_of_frame = std::make_shared<NodeUnsignedInteger>("start_of_frame",1);
    auto can_identifier = std::make_shared<NodeUnsignedInteger>("identifier",11);
    auto can_rtr = std::make_shared<NodeUnsignedInteger>("RTR",1);
    auto can_ide = std::make_shared<NodeUnsignedInteger>("IDE",1);
    auto can_reserved = std::make_shared<NodeUnsignedInteger>("Reserved",1);
    auto can_dlc = std::make_shared<NodeUnsignedInteger>("DLC",4);
    auto can_data = std::make_shared<NodeArray>("data","/DLC");
    auto can_crc = std::make_shared<NodeUnsignedInteger>("CRC",15);
    auto can_crc_delimiter = std::make_shared<NodeUnsignedInteger>("CRC_delimiter",1);
    auto can_ack = std::make_shared<NodeUnsignedInteger>("ACK",1);
    auto can_ack_delimiter = std::make_shared<NodeUnsignedInteger>("ACK_delimiter",1);
    auto can_eof = std::make_shared<NodeUnsignedInteger>("EOF",7);

    can_data->addChild(std::make_shared<NodeUnsignedInteger>("data",8));

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

    nlohmann::json result;
    BitStream bs_b64 = BitStream(std::string("AUBAIGhgL/A="));
    logger.log("bitstream_to_json", Logger::Level::INFO);
    auto start = std::chrono::high_resolution_clock::now();
    int retVal = rootNode->bitstream_to_json(bs_b64, result);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    logger.log("bitstream_to_json " + std::to_string(retVal) + " evaluation completed in " + std::to_string(duration.count()) + " ns", Logger::Level::INFO);

    logger.log("JSON: " + result.unflatten().dump(), Logger::Level::INFO);

    return 0;
}
