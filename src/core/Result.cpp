#include "core/Result.hpp"

namespace CO {

    Result::Result(ErrorCode code, std::string message)
        : code_(code), message_(std::move(message)) {}

    Result Result::ok() {
        return Result();
    }

    Result Result::fail(ErrorCode code, std::string message) {
        return Result(code, std::move(message));
    }

    bool Result::success() const {
        return code_ == ErrorCode::None;
    }

    bool Result::failed() const {
        return !success();
    }

    ErrorCode Result::code() const {
        return code_;
    }

    const std::string& Result::message() const {
        return message_;
    }

} // namespace editor2