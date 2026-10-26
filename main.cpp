#include "handlers.h"
#include "log_message.h"
#include <iostream>

int main() {
    //Собираем цепочку обязанностей.
    auto unknown = std::make_unique<UnknownHandler>();
    auto warning = std::make_unique<WarningHandler>();
    auto error = std::make_unique<ErrorHandler>("errors.log");
    auto fatal = std::make_unique<FatalErrorHandler>();

    //Выстраиваем связи.
    warning->SetNext(std::move(unknown));
    error->SetNext(std::move(warning));
    fatal->SetNext(std::move(error));

    //fatal главный обработчик.
    Handler* chain_head = fatal.get();

    std::cout << "--- Testing Warning ---\n";
    try {
        chain_head->Handle(LogMessage(Type::Warning, "Low memory"));
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    std::cout << "\n--- Testing Error ---\n";
    try {
        chain_head->Handle(LogMessage(Type::Error, "File not found"));
        std::cout << "(Check errors.log file)\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    std::cout << "\n--- Testing Fatal Error ---\n";
    try {
        chain_head->Handle(LogMessage(Type::FatalError, "Out of memory"));
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
    }

    std::cout << "\n--- Testing Unknown Type ---\n";
    try {
        chain_head->Handle(LogMessage(Type::Unknown, "Just a strange message"));
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
    }

    return 0;
}