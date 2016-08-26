/*
 * m_privs.c: Shows effective operator privileges
 *
 * Copyright (C) 2008 Jilles Tjoelker
 * Copyright (C) 2008 charybdis development team
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 2.Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3.The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
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

using namespace ircd;

static const char privs_desc[] = "Provides the PRIVS command to inspect an operator's privileges";

static void m_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[]);
static void me_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[]);
static void mo_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[]);

struct Message privs_msgtab = {
	"PRIVS", 0, 0, 0, 0,
	{mg_unreg, {m_privs, 0}, mg_ignore, mg_ignore, {me_privs, 0}, {mo_privs, 0}}
};

mapi_clist_av1 privs_clist[] = {
	&privs_msgtab,
	NULL
};

/* XXX this is a copy, not so nice
 *
 * Sort of... it's int in newconf.c since oper confs don't need 64-bit wide flags.
 * --Elizafox
 */
struct mode_table_
{
	const char *name;
	uint64_t mode;
};

/* there is no such table like this anywhere else */
static struct mode_table_ auth_client_table[] = {
	{"resv_exempt",		client::flags::EXEMPTRESV	},
	{"kline_exempt",	client::flags::EXEMPTKLINE	},
	{"flood_exempt",	client::flags::EXEMPTFLOOD	},
	{"spambot_exempt",	client::flags::EXEMPTSPAMBOT	},
	{"shide_exempt",	client::flags::EXEMPTSHIDE	},
	{"jupe_exempt",		client::flags::EXEMPTJUPE	},
	{"extend_chans",	client::flags::EXTENDCHANS	},
	{NULL, 0}
};

DECLARE_MODULE_AV2(privs, NULL, NULL, privs_clist, NULL, NULL, NULL, NULL, privs_desc);

static void show_privs(client::client &source, client::client *target_p)
{
	char buf[512];
	struct mode_table_ *p;

	buf[0] = '\0';
	if (target_p->localClient->privset)
		rb_strlcat(buf, target_p->localClient->privset->privs, sizeof buf);
	if (is(*target_p, umode::OPER))
	{
		if (buf[0] != '\0')
			rb_strlcat(buf, " ", sizeof buf);
		rb_strlcat(buf, "operator:", sizeof buf);
		rb_strlcat(buf, target_p->localClient->opername, sizeof buf);

		if (target_p->localClient->privset)
		{
			if (buf[0] != '\0')
				rb_strlcat(buf, " ", sizeof buf);
			rb_strlcat(buf, "privset:", sizeof buf);
			rb_strlcat(buf, target_p->localClient->privset->name, sizeof buf);
		}
	}
	p = &auth_client_table[0];
	while (p->name != NULL)
	{
		if (target_p->flags & p->mode)
		{
			if (buf[0] != '\0')
				rb_strlcat(buf, " ", sizeof buf);
			rb_strlcat(buf, p->name, sizeof buf);
		}
		p++;
	}
	sendto_one_numeric(&source, RPL_PRIVS, form_str(RPL_PRIVS),
			target_p->name, buf);
}

static void
me_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[])
{
	client::client *target_p;

	if (!is(source, umode::OPER) || parc < 2 || EmptyString(parv[1]))
		return;

	/* we cannot show privs for remote clients */
	if((target_p = client::find_person(parv[1])) && my(*target_p))
		show_privs(source, target_p);
}

static void
mo_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[])
{
	client::client *target_p;

	if (parc < 2 || EmptyString(parv[1]))
		target_p = &source;
	else
	{
		target_p = client::find_named_person(parv[1]);
		if (target_p == NULL)
		{
			sendto_one_numeric(&source, ERR_NOSUCHNICK,
					   form_str(ERR_NOSUCHNICK), parv[1]);
			return;
		}
	}

	if (my(*target_p))
		show_privs(source, target_p);
	else
		sendto_one(target_p, ":%s ENCAP %s PRIVS %s",
				get_id(&source, target_p),
				target_p->servptr->name,
				use_id(target_p));
}

static void
m_privs(struct MsgBuf *msgbuf_p, client::client &client, client::client &source, int parc, const char *parv[])
{
	if (parc >= 2 && !EmptyString(parv[1]) &&
			irccmp(parv[1], source.name)) {
		sendto_one_numeric(&source, ERR_NOPRIVILEGES,
				   form_str(ERR_NOPRIVILEGES));
		return;
	}

	show_privs(source, &source);
}
