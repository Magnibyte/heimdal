/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xauth.h>

#include <krb.h>

#include <roken.h>

extern char *prog;

int copy_encrypted (int fd1, int fd2, des_cblock *iv,
		    des_key_schedule schedule);

extern char x_socket[];
extern u_int32_t display_num;
extern char xauthfile[];
extern int xauthfile_size;
extern u_char cookie[];
extern size_t cookie_len;

int get_xsockets (int *unix_socket, int *tcp_socket);
int connect_local_xsocket (unsigned dnr);

#define KX_PORT 2111

#define COOKIE_TYPE "MIT-MAGIC-COOKIE-1"
