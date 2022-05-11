#pragma once

enum class ValueType {
    Integer,
    Float,
    String,
};

struct TypeField {
    std::string name;
    ValueType type;
    size_t position;
};

struct Type {
    std::string name;
    std::map<std::string, TypeField> fields;
    std::vector<std::string> field_names;
    size_t size;
};

class ComplexType {
public:
    void None() {
        is_none = true;
    }

    void Integer() {
        is_standard = true;
        standard = ValueType::Integer;
    }

    void Float() {
        is_standard = true;
        standard = ValueType::Float;
    }

    void String() {
        is_standard = true;
        standard = ValueType::String;
    }

    void Struct(Type *type) {
        is_type = true;
        this->type = type;
    }

    bool IsNone() {
        return is_none;
    }

    bool IsInteger() {
        return is_standard && standard == ValueType::Integer;
    }

    bool IsFloat() {
        return is_standard && standard == ValueType::Float;
    }

    bool IsString() {
        return is_standard && standard == ValueType::String;
    }

    bool IsType() {
        return is_type && type != nullptr;
    }

    Type *GetType() { return type; }

private:
    bool is_standard = false;
    bool is_none = false;
    bool is_type = false;
    ValueType standard;
    struct Type *type;
};

ComplexType TypeNone();
ComplexType TypeInteger();
ComplexType TypeFloat();
ComplexType TypeString();
ComplexType TypeStruct(Type *type);

