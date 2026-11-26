/* The IPv6 types FileZilla 2.2.32 names, for the NT 4.0 headers, which have none or
   an early draft of them. Included by AsyncSocketEx.h after ws2tcpip.h. FileZilla
   looks getaddrinfo up at run time; NT 4.0's ws2_32.dll doesn't have it, so these are
   never filled in there and FileZilla stays on IPv4. Written for this port. */
#ifndef FZ_IPV6_H
#define FZ_IPV6_H
#ifndef AF_INET6
#define AF_INET6 23
#endif
struct fz_in6_addr { union { u_char Byte[16]; u_short Word[8]; } u; };
struct fz_sockaddr_in6 {
	short sin6_family;
	u_short sin6_port;
	u_long sin6_flowinfo;
	struct fz_in6_addr sin6_addr;
	u_long sin6_scope_id;
};
struct fz_addrinfo {
	int ai_flags, ai_family, ai_socktype, ai_protocol;
	size_t ai_addrlen;
	char *ai_canonname;
	struct sockaddr *ai_addr;
	struct fz_addrinfo *ai_next;
};
#define in6_addr fz_in6_addr
#define s6_bytes u.Byte
#define sockaddr_in6 fz_sockaddr_in6
#define SOCKADDR_IN6 struct fz_sockaddr_in6
#define addrinfo fz_addrinfo
static const struct fz_in6_addr in6addr_any = { { { 0 } } };
#endif
