#pragma once

#include <string>
#include <variant>
#include <optional>

namespace opencmd {

class TreeElementAttribute {

    public:
        using TreeElementAttributeVariant = std::variant<std::nullptr_t, bool, double, int64_t, std::string>;

    private:
        TreeElementAttributeVariant value;

    public:
        TreeElementAttribute() : value(nullptr) {}
        TreeElementAttribute(bool b) : value(b) {}
        TreeElementAttribute(double d) : value(d) {}
        TreeElementAttribute(int64_t i) : value(i) {}
        TreeElementAttribute(const std::string& s) : value(s) {}
        TreeElementAttribute(const TreeElementAttribute& other) { 
            value = other.value;
        }

        TreeElementAttribute& operator=(const TreeElementAttribute& other) {
            if (this != &other) {
                this->value = other.value;
            }
            return *this;
        }

        std::unique_ptr<TreeElementAttribute> clone() const { 
            return std::make_unique<TreeElementAttribute>(*this);
        }

        bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
        bool isBool() const { return std::holds_alternative<bool>(value); }
        bool isDecimal() const { return std::holds_alternative<double>(value); }
        bool isInteger() const { return std::holds_alternative<int64_t>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        TreeElementAttributeVariant get() const { return value; }

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
            } else {
                oss << "-attribute type not supported-";
            }
            return oss.str();
        }

    }; 

}