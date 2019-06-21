// Matrix Construct
//
// Copyright (C) Matrix Construct Developers, Authors & Contributors
// Copyright (C) 2016-2018 Jason Volk <jason@zemos.net>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice is present in all copies. The
// full license for this software is available in the LICENSE file.

#ifndef HAVE_IRCD_IRCD_H
#define HAVE_IRCD_IRCD_H

// Generated by ./configure
#include "config.h"
#include "assert.h"
#include "stdinc.h"
#include "string_view.h"
#include "vector_view.h"
#include "byte_view.h"
#include "buffer/buffer.h"
#include "allocator.h"
#include "util/util.h"
#include "exception.h"
#include "run.h"
#include "demangle.h"
#include "localee.h"
#include "date.h"
#include "logger.h"
#include "info.h"
#include "nacl.h"
#include "rand.h"
#include "crh.h"
#include "ed25519.h"
#include "color.h"
#include "lex_cast.h"
#include "base.h"
#include "stringops.h"
#include "tokens.h"
#include "iov.h"
#include "grammar.h"
#include "parse.h"
#include "rfc1459.h"
#include "json/json.h"
#include "openssl.h"
#include "fmt.h"
#include "http.h"
#include "http2/http2.h"
#include "magics.h"
#include "conf.h"
#include "stats.h"
#include "prof.h"
#include "fs/fs.h"
#include "ios.h"
#include "ctx/ctx.h"
#include "db/db.h"
#include "js.h"
#include "mods/mods.h"
#include "rfc1035.h"
#include "rfc3986.h"
#include "net/net.h"
#include "server/server.h"
#include "m/m.h"
#include "resource/resource.h"
#include "client.h"
#include "magick.h"

/// \brief Internet Relay Chat daemon. This is the principal namespace for IRCd.
///
namespace ircd
{
	seconds uptime();

	void cont() noexcept;
	bool quit() noexcept;
	void init(boost::asio::io_context &ios, const string_view &origin, const string_view &hostname);

	extern conf::item<bool> restart;
	extern conf::item<bool> debugmode;
	extern conf::item<bool> read_only;
	extern conf::item<bool> write_avoid;
	extern conf::item<bool> soft_assert;
}

#endif // HAVE_IRCD_IRCD_H
