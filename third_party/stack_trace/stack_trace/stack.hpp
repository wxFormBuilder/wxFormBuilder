// Copyright 2007 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <list>
#include <stdexcept>
#include <iosfwd>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace dbg {
class stack_error : public std::exception {
public:
	stack_error(const std::string& what);
	~stack_error() throw();

	const char* what() const throw();

private:
	std::string what_;
};

class stack_frame {
public:
	stack_frame(const void* instruction, const std::string& function);

	const void* instruction() const;
	const std::string& function() const;

private:
	const void* instruction_;
	std::string function_;
};

std::ostream& operator<<(std::ostream& out, const stack_frame& frame);

class stack {
public:
	typedef std::list<stack_frame>::size_type depth_type;
	typedef std::list<stack_frame>::const_iterator const_iterator;

	stack(depth_type limit = 0);

#if defined(_WIN32)
	stack(depth_type limit, CONTEXT* fromContext);
#endif

	const_iterator begin() const;
	const_iterator end() const;

	depth_type depth() const;

private:
	std::list<stack_frame> frames_;
};

} // namespace dbg
