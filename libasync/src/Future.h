/*
 * MIT License
 *
 * Copyright (c) 2018 jipeng wu
 * <recvfromsockaddr at gmail dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Common.h"
#include "Task.h"
#include <any>
#include <optional>

namespace wjp {

	class Future : public Task {
	public:
		template<typename T>
		std::optional<T> try_to_get_now() {
			if (value.has_value() && value.type() == typeid(T)) {
				return std::any_cast<T>(value);
			}else {
				return {};
			}
		}

		// Wait and return the result value
		template<typename T>
		std::optional<T> try_to_get() noexcept {
			try {
				this->wait();
			}catch (...) {
				return {};
			}
			return try_to_get_now<T>();
		}

		// Wait and return the result value, might throw interrupted/execution exceptions
		template<typename T>
		std::optional<T> get() {
			this->wait(); 
			return try_to_get_now<T>();
		}

		template<typename T>
		void set_value(T val) {
			value = val;
		}

		virtual ~Future() {}
	private:
		std::any value;
	};
}

