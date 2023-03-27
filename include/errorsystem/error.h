/**
 * Copyright (c) 2014-present, The osquery authors
 *
 * This source code is licensed as defined by the LICENSE file found in the
 * root directory of this source tree.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR GPL-2.0-only)
 */

#pragma once

#include "abi.h"
#include "errorsystem/attribute.h"

#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <typeinfo>

#include <type_traits>

namespace errorsystem {

namespace {
/**
 * Conversion either scoped or unscoped enum value to std::string
 * of human readable representation.
 *
 * enum class En {
 *  First = 1,
 * };
 * to<std::string>(En::First) -> "En::First[1]"
 */
template <typename ToType, typename FromType>
inline typename std::enable_if<std::is_enum<FromType>::value &&
                                   std::is_same<ToType, std::string>::value,
                               ToType>::type
to(FromType from) noexcept {
    auto str = ToType{boost::core::demangle(typeid(from).name())};
    str.append("[");
    str.append(std::to_string(
        static_cast<typename std::underlying_type<FromType>::type>(from)));
    str.append("]");
    return str;
}
} // namespace

class ErrorBase {
public:
    virtual std::string getNonRecursiveMessage() const = 0;
    virtual std::string getMessage() const = 0;
    virtual ~ErrorBase() = default;
    ErrorBase() = default;
    ErrorBase(const ErrorBase& other) = default;
};

template <typename ErrorCodeEnumType>
class Error final : public ErrorBase {
public:
    using SelfType = Error<ErrorCodeEnumType>;

    explicit Error(ErrorCodeEnumType error_code,
                   std::string message,
                   std::unique_ptr<ErrorBase> underlying_error = nullptr)
        : errorCode_(error_code),
          message_(std::move(message)),
          underlyingError_(std::move(underlying_error)) {}

    explicit Error(ErrorCodeEnumType error_code,
                   std::unique_ptr<ErrorBase> underlying_error = nullptr)
        : errorCode_(error_code),
          underlyingError_(std::move(underlying_error)) {}

    virtual ~Error() = default;

    Error(Error&& other) = default;
    Error(const Error& other) = delete;

    Error& operator=(Error&& other) = default;
    Error& operator=(const Error& other) = delete;

    ErrorCodeEnumType getErrorCode() const {
        return errorCode_;
    }

    bool hasUnderlyingError() const {
        return underlyingError_ != nullptr;
    }

    const ErrorBase& getUnderlyingError() const {
        return *underlyingError_;
    }

    std::unique_ptr<ErrorBase> takeUnderlyingError() {
        return std::move(underlyingError_);
    }

    std::string getNonRecursiveMessage() const override {
        std::string full_message = to<std::string>(errorCode_);
        if (message_.size() > 0) {
            full_message += " (" + message_ + ")";
        }
        return full_message;
    }

    std::string getMessage() const override {
        std::string full_message = getNonRecursiveMessage();
        if (underlyingError_) {
            full_message += " <- " + underlyingError_->getMessage();
        }
        return full_message;
    }

    void appendToMessage(const std::string& text) {
        message_.append(text);
    }

private:
    ErrorCodeEnumType errorCode_;
    std::string message_;
    std::unique_ptr<ErrorBase> underlyingError_;
};

template <class T>
inline bool operator==(const Error<T>& lhs, const Error<T>& rhs) {
    return lhs.getErrorCode() == rhs.getErrorCode();
}

template <class T>
inline bool operator==(const Error<T>* lhs, const T rhs) {
    return lhs->getErrorCode() == rhs;
}

template <class T>
inline bool operator==(const Error<T>& lhs, const T rhs) {
    return lhs.getErrorCode() == rhs;
}

template <class T>
inline bool operator==(const ErrorBase& lhs, const T rhs) {
    try {
        const Error<T>& casted_lhs = dynamic_cast<const Error<T>&>(lhs);
        return casted_lhs == rhs;
    } catch (std::bad_cast _) {
        return false;
    }
}

inline std::ostream& operator<<(std::ostream& out, const ErrorBase& error) {
    out << error.getMessage();
    return out;
}

template <typename ErrorCodeEnumType, typename OtherErrorCodeEnumType>
ERRORSYSTEM_NODISCARD Error<ErrorCodeEnumType> createError(
    ErrorCodeEnumType error_code,
    Error<OtherErrorCodeEnumType> underlying_error) {
    return Error<ErrorCodeEnumType>(
        error_code,
        std::make_unique<Error<OtherErrorCodeEnumType>>(
            std::move(underlying_error)));
}

template <typename ErrorCodeEnumType>
ERRORSYSTEM_NODISCARD Error<ErrorCodeEnumType> createError(
    ErrorCodeEnumType error_code) {
    return Error<ErrorCodeEnumType>(error_code);
}

template <typename ErrorType,
          typename ValueType,
          typename = typename std::enable_if<
              std::is_base_of<ErrorBase, ErrorType>::value>::type>
inline ErrorType operator<<(ErrorType&& error, const ValueType& value) {
    std::ostringstream ostr{};
    ostr << value;
    error.appendToMessage(ostr.str());
    return std::forward<ErrorType>(error);
}

} // namespace errorsystem
