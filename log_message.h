#pragma once

#include <string>

//Типы сообщений.
enum class Type {
    Warning,
    Error,
    FatalError,
    Unknown
};

class LogMessage {
public:
    LogMessage(Type type, std::string message)
        : type_(type), message_(std::move(message)) {
    }

    Type type() const {
        return type_;
    }

    const std::string& message() const {
        return message_;
    }

private:
    Type type_;
    std::string message_;
};