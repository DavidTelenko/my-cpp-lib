#pragma once

#include <my/format/format.hpp>
#include <string>

namespace my::cli {

class Status {
   public:
    enum class Code {
        Ok,
        ArgumentsAmountMismatch,
        ArgumentsTypeMismatch,
        ArgumentValueMismatch,
        NoSuchCommand,
        ParsingError,
        HandlingError,
        CallbackError,
        UnknownError,
    };

    using enum Code;

    // No error constructor
    Status() = default;

    // unknown error with message and code
    template <class... Args>
    explicit Status(Code code, std::string_view format, Args&&... args)
        : _code(code),
          _message(my::format(format.data(),
                              std::forward<Args>(args)...)) {}

    // unknown error with message
    template <class... Args>
    explicit Status(std::string_view format, Args&&... args)
        : Status(Code::UnknownError,
                 std::move(format),
                 std::forward<Args>(args)...) {}

    operator std::string() const { return _message; }
    const std::string& operator*() const { return _message; }
    const std::string& message() const { return _message; }

    template <class... Args>
    void setMessage(std::string_view format, Args&&... args) {
        _message = my::format(format.data(), std::forward<Args>(args)...);
    }

    operator bool() const { return _code == Code::Ok; }
    const bool ok() const { return _code == Code::Ok; }

    Code& code() { return _code; }
    const Code& code() const { return _code; }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const Status& status) {
        os << status.message();
        return os;
    }

   private:
    Code _code = Code::Ok;
    std::string _message{};
};

};  // namespace my::cli