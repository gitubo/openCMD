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
        Logger::getInstance().log("JSON file loaded", Logger::Level::INFO);
    } catch (const nlohmann::json::parse_error& e) {
        logger.log("Catalog file not correctly parsed: " + std::string(e.what()), Logger::Level::ERROR);
        return 2;
    }

    std::string schemaName = "can";
    if(SchemaCatalog::getInstance().parseSchema(schemaName, jsonData) < 0){
        Logger::getInstance().log("Error in parsing the schema", Logger::Level::ERROR);
    }

    logger.log("Implementing the tree", Logger::Level::INFO);

    auto schema = SchemaCatalog::getInstance().getSchema(schemaName);
    if(!schema){
        Logger::getInstance().log("Error in retrieving schema <" + schemaName + ">", Logger::Level::ERROR);
        return 1;
    }

//    auto rn = Engine::evaluateStructure(schema->getStructure(), "/");
    auto rn = SchemaCatalog::getInstance().getAbstractTree(schemaName);
    if(!rn){
        Logger::getInstance().log("Error in evaluating schema", Logger::Level::ERROR);
        return 1;
    }

    nlohmann::json result;
    BitStream bs_b64 = BitStream(std::string("AUBAIGhgL/A="));
    logger.log("bitstream_to_json", Logger::Level::INFO);
    auto start = std::chrono::high_resolution_clock::now();
    int retVal = rn->bitstream_to_json(bs_b64, result);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    logger.log("bitstream_to_json " + std::to_string(retVal) + " evaluation completed in " + std::to_string(duration.count()) + " ns", Logger::Level::INFO);

    logger.log("JSON: " + result.unflatten().dump(), Logger::Level::INFO);

    return 0;
}
