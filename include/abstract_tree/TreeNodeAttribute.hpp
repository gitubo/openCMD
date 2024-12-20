#pragma once

#include <string>
#include <variant>
#include <optional>

namespace opencmd {

class TreeNodeAttribute {

    public:
        using TreeNodeAttributeObject = std::unordered_map<std::string, TreeNodeAttribute>;
        using TreeNodeAttributeArray = std::vector<TreeNodeAttribute>;
        using TreeNodeAttributeVariant = std::variant<
            std::nullptr_t, 
            bool, 
            double, 
            int64_t, 
            std::string, 
            TreeNodeAttributeObject, 
            TreeNodeAttributeArray
        >;

    private:
        TreeNodeAttributeVariant value;

    public:
        TreeNodeAttribute() : value(nullptr) {}
        TreeNodeAttribute(bool b) : value(b) {}
        TreeNodeAttribute(double d) : value(d) {}
        TreeNodeAttribute(int64_t i) : value(i) {}
        TreeNodeAttribute(const std::string& s) : value(s) {}
        TreeNodeAttribute(const TreeNodeAttributeObject& o) : value(o) {}
        TreeNodeAttribute(const TreeNodeAttributeArray& a) : value(a) {}
        TreeNodeAttribute(const TreeNodeAttribute& other) { value = other.value; }

        TreeNodeAttribute& operator=(const TreeNodeAttribute& other) {
            if (this != &other) {
                this->value = other.value;
            }
            return *this;
        }

        std::unique_ptr<TreeNodeAttribute> clone() const { 
            return std::make_unique<TreeNodeAttribute>(*this);
        }

        bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
        bool isBool() const { return std::holds_alternative<bool>(value); }
        bool isDecimal() const { return std::holds_alternative<double>(value); }
        bool isInteger() const { return std::holds_alternative<int64_t>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        bool isObject() const { return std::holds_alternative<TreeNodeAttributeObject>(value); }
        bool isArray() const { return std::holds_alternative<TreeNodeAttributeArray>(value); }

        TreeNodeAttributeVariant get() const { return value; }

        std::optional<std::nullptr_t> getNull() const {
            if (std::holds_alternative<std::nullptr_t>(value)) {
                return std::get<std::nullptr_t>(value);
            }
            return std::nullopt;
        }
        std::optional<bool> getBool() const {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
            return std::nullopt;
        }
        std::optional<double> getDecimal() const {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
            return std::nullopt;
        }
        std::optional<int64_t> getInteger() const {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value);
            }
            return std::nullopt;
        }
        std::optional<std::string> getString() const {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
            return std::nullopt;
        }
        std::optional<TreeNodeAttributeObject> getObject() const {
            if (isObject()) return std::get<TreeNodeAttributeObject>(value);
            return std::nullopt;
        }
        std::optional<TreeNodeAttributeArray> getArray() const {
            if (isArray()) return std::get<TreeNodeAttributeArray>(value);
            return std::nullopt;
        }

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
            } else if (isObject()) {
                oss << "{\n";
                for (const auto& [key, val] : std::get<TreeNodeAttributeObject>(value)) {
                    oss << indentStr << "  \"" << key << "\": " << val.to_string(indent + 2) << ",\n";
                }
                oss << indentStr << "}";
            } else if (isArray()) {
                oss << "[\n";
                for (const auto& item : std::get<TreeNodeAttributeArray>(value)) {
                    oss << indentStr << "  " << item.to_string(indent + 2) << ",\n";
                }
                oss << indentStr << "]";
            } else {
                oss << "-attribute type not supported-";
            }
            return oss.str();
        }

    }; 

}