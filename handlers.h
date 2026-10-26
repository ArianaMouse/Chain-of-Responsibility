#pragma once

#include "log_message.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

//Базовый класс обработчика.
class Handler {
public:
    virtual ~Handler() = default;

    //Устанавка следующего обработчика.
    void SetNext(std::unique_ptr<Handler> next) {
        next_ = std::move(next);
    }

    //Метод обработки.
    virtual void Handle(const LogMessage& msg) = 0;

protected:
    //Вспомогательный метод для передачи сообщения дальше.
    void Forward(const LogMessage& msg) {
        if (next_) {
            next_->Handle(msg);
        }
    }

    std::unique_ptr<Handler> next_;
};

//Обработчик фатальных ошибок.
class FatalErrorHandler : public Handler {
public:
    void Handle(const LogMessage& msg) override {
        if (msg.type() == Type::FatalError) {
            throw std::runtime_error("Fatal error: " + msg.message());
        }
        Forward(msg);
    }
};

//Обработчик обычных ошибок.
class ErrorHandler : public Handler {
public:
    explicit ErrorHandler(std::string file_path) : file_path_(std::move(file_path)) {}

    void Handle(const LogMessage& msg) override {
        if (msg.type() == Type::Error) {
            std::ofstream file(file_path_, std::ios::app);
            if (file.is_open()) {
                file << "[ERROR] " << msg.message() << std::endl;
            }
            return; //Обработали, дальше не передаем.
        }
        Forward(msg);
    }

private:
    std::string file_path_;
};

//Обработчик предупреждений.
class WarningHandler : public Handler {
public:
    void Handle(const LogMessage& msg) override {
        if (msg.type() == Type::Warning) {
            std::cout << "[WARNING] " << msg.message() << std::endl;
            return;
        }
        Forward(msg);
    }
};

//Обработчик неизвестных сообщений. Замыкает.
class UnknownHandler : public Handler {
public:
    void Handle(const LogMessage& msg) override {
        //Если сообщение не обработано ни одним предыдущим обработчиком.
        throw std::runtime_error("Unhandled message type: " + msg.message());
    }
};