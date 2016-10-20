/*
 * Copyright (C) 2016 Charybdis Development Team
 * Copyright (C) 2016 Jason Volk <jason@zemos.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice is present in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#define HAVE_IRCD_JS_STRING_H

namespace ircd {
namespace js   {

struct string
:JS::Rooted<JSString *>
{
	// SpiderMonkey may use utf-16/char16_t strings; these will help you then
	static size_t convert(const char16_t *const &, char *const &buf, const size_t &max);
	static std::string convert(const char16_t *const &);
	static std::string convert(const std::u16string &);
	static std::u16string convert(const char *const &);
	static std::u16string convert(const std::string &);

	static constexpr const size_t CBUFS = 8;
	static const size_t CBUFSZ;
	const char *c_str() const;                   // Copy into rotating buf
	size_t size() const;

	explicit operator std::string() const;
	operator JS::Value() const;

	string(const char *const &, const size_t &len);
	explicit string(const std::string &);
	string(const char *const &);
	string(JSString *const &);
	string(const value &);
	string();
	string(string &&) noexcept;
	string(const string &) = delete;

	friend std::ostream &operator<<(std::ostream &os, const string &s);
};

inline
string::string()
:JS::Rooted<JSString *>{*cx}
{
}

inline
string::string(string &&other)
noexcept
:JS::Rooted<JSString *>{*cx, other}
{
}

inline
string::string(const value &val)
:JS::Rooted<JSString *>
{
	*cx,
	JS::ToString(*cx, val)
}
{
}

inline
string::string(JSString *const &val)
:JS::Rooted<JSString *>{*cx, val}
{
}

inline
string::string(const std::string &s)
:string(s.data(), s.size())
{
}

inline
string::string(const char *const &s)
:string(s, strlen(s))
{
}

inline
string::string(const char *const &s,
               const size_t &len)
:JS::Rooted<JSString *>
{
	*cx,
	JS_NewStringCopyN(*cx, s, len)
}
{
	if(unlikely(!get()))
		throw type_error("Failed to construct string from character array");
}

inline
string::operator JS::Value()
const
{
	return JS::StringValue(get());
}

inline
string::operator std::string()
const
{
	return native(get());
}

inline size_t
string::size()
const
{
	return native_size(get());
}

inline
std::ostream &operator<<(std::ostream &os, const string &s)
{
	os << std::string(s);
	return os;
}

} // namespace js
} // namespace ircd
