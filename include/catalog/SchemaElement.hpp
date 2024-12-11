#pragma once

#include <variant>
#include <vector>
#include <map>
#include <string>


namespace opencmd {
    class SchemaElement {

    public:
        using SchemaElementObject = std::map<std::string, SchemaElement>;
        using SchemaElementArray = std::vector<SchemaElement>;
        using SchemaElementVariant = std::variant<std::nullptr_t, bool, double, int64_t, std::string, SchemaElementArray, SchemaElementObject>;

    private:
        SchemaElementVariant value;

    public:
        SchemaElement() : value(nullptr) {}
        SchemaElement(bool b) : value(b) {}
        SchemaElement(double d) : value(d) {}
        SchemaElement(int64_t i) : value(i) {}
        SchemaElement(const std::string& s) : value(s) {}
        SchemaElement(const SchemaElementArray& a) : value(a) {}
        SchemaElement(const SchemaElementObject& o) : value(o) {}

        bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
        bool isBool() const { return std::holds_alternative<bool>(value); }
        bool isDecimal() const { return std::holds_alternative<double>(value); }
        bool isInteger() const { return std::holds_alternative<int64_t>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        bool isArray() const { return std::holds_alternative<SchemaElementArray>(value); }
        bool isObject() const { return std::holds_alternative<SchemaElementObject>(value); }

        SchemaElementVariant get() const { return value; }

        std::string to_string(size_t indent = 0) const {
            std::ostringstream oss;
            std::string indentStr(indent, ' '); 

            if (isNull()) {
                oss << "null";
            } else if (isBool()) {
                oss << (std::get<bool>(value) ? "true" : "false");
            } else if (isDecimal()) {
                oss << std::get<double>(value);
            } else if (isInteger()) {
                oss << std::get<int64_t>(value);
            } else if (isString()) {
                oss << "\"" << std::get<std::string>(value) << "\"";
            } else if (isArray()) {
                oss << "[\n";
                const auto& array = std::get<SchemaElementArray>(value);
                for (auto it = array.begin(); it != array.end(); ++it) {
                    oss << indentStr << "  " << it->to_string(indent + 2);
                    if (std::next(it) != array.end()) { 
                        oss << ",\n";
                    } else {
                        oss << "\n";
                    }
                }
                oss << indentStr << "]";
            } else if (isObject()) {
                oss << "{\n";
                const auto& object = std::get<SchemaElementObject>(value);

                for (auto it = object.begin(); it != object.end(); ++it) {
                    auto key = it->first;
                    auto elem = it->second;
                    oss << indentStr << "  \"" << key << "\": " << elem.to_string(indent + 2);
                    if (std::next(it) != object.end()) { 
                        oss << ",\n";
                    } else {
                        oss << "\n";
                    }
                }
                oss << indentStr << "}";
            } else {
                oss << "unknown";
            }
            return oss.str();
        }

    };

}