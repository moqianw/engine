#pragma once

#include <optional>
#include <string>
#include <utility>

namespace CO {

	enum class ErrorCode {
		None = 0,
		InvalidArgument,
		NotFound,
		AlreadyExists,
		TypeMismatch,
		InvalidState,
		Unsupported,
		IoError,
		SerializationError,
		EvaluationError,
		EngineSyncError,
		Canceled,
		Stale,
		InternalError
	};

	class Result {
	public:
		Result() = default;
		Result(ErrorCode code, std::string message);

		static Result ok();
		static Result fail(ErrorCode code, std::string message);

		bool success() const;
		bool failed() const;

		ErrorCode code() const;
		const std::string& message() const;

	private:
		ErrorCode code_ = ErrorCode::None;
		std::string message_;
	};

	template <typename T>
	class ResultT {
	public:
		ResultT(const T& value)
			: result_(Result::ok()), value_(value) {}

		ResultT(T&& value)
			: result_(Result::ok()), value_(std::move(value)) {}

		ResultT(ErrorCode code, std::string message)
			: result_(Result::fail(code, std::move(message))) {}

		ResultT(Result result)
			: result_(std::move(result)) {}

		bool success() const {
			return result_.success();
		}

		bool failed() const {
			return result_.failed();
		}

		const Result& result() const {
			return result_;
		}

		const T& value() const {
			return *value_;
		}

		T& value() {
			return *value_;
		}

		const T& operator*() const {
			return *value_;
		}

		T& operator*() {
			return *value_;
		}

		const T* operator->() const {
			return &(*value_);
		}

		T* operator->() {
			return &(*value_);
		}

	private:
		Result result_;
		std::optional<T> value_;
	};

} // namespace editor2
