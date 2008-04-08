#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(bartlby-agent, 1.1.4, helmut@januschka.com)
AC_PREFIX_DEFAULT(/usr/local/bartlby-agent)


dnl check for plugin dir
AC_ARG_WITH(plugin-dir,AC_HELP_STRING([--with-plugin-dir], [Where are the plugins located]), PLUGIN_DIR="${withval}", PLUGIN_DIR="${prefix}/plugins/")
AC_SUBST(PLUGIN_DIR)

dnl check for allowed IPS
AC_ARG_WITH(allowed-ips,AC_HELP_STRING([--with-allowed-ips], [from wich ip addresses connections are allowed? seperated with commas]), ALLOWEDIPS="${withval}", ALLOWEDIPS="127.0.0.1")
AC_SUBST(ALLOWEDIPS)


dnl check for allowed IPS
AC_ARG_WITH(load-limit,AC_HELP_STRING([--with-load-limit], [load limit to skip checks]), LOADLIMIT="${withval}", LOADLIMIT="50")
AC_SUBST(LOADLIMIT)

dnl check for allowed IPS
AC_ARG_WITH(sync-url,AC_HELP_STRING([--with-sync-url], [Url to the bartlby-ui installation]), BARTLBY_SYNC="${withval}", BARTLBY_SYNC="http://localhost/bartlby-ui/")
AC_SUBST(BARTLBY_SYNC)

dnl check for allowed IPS
AC_ARG_WITH(sync-user,AC_HELP_STRING([--with-sync-user], [username for the sync]), BARTLBY_SYNC_USER="${withval}", BARTLBY_SYNC_USER="sync")
AC_SUBST(BARTLBY_SYNC_USER)

dnl check for allowed IPS
AC_ARG_WITH(sync-pw,AC_HELP_STRING([--with-sync-password], [password for the sync]), BARTLBY_SYNC_PW="${withval}", BARTLBY_SYNC_PW="sync")
AC_SUBST(BARTLBY_SYNC_PW)

AC_ARG_WITH(sync-clientname,AC_HELP_STRING([--with-sync-clientname], [clientname for the sync]), BARTLBY_SYNC_CLIENTNAME="${withval}", BARTLBY_SYNC_CLIENTNAME="bartlbydemo")
AC_SUBST(BARTLBY_SYNC_CLIENTNAME)

AC_ARG_WITH(sync-arch,AC_HELP_STRING([--with-sync-arch], [arch for the sync]), BARTLBY_SYNC_ARCH="${withval}", BARTLBY_SYNC_ARCH="i386")
AC_SUBST(BARTLBY_SYNC_ARCH)

AC_CHECK_LIB(socket, socket)

dnl Check for ssl library.
 
  
  AC_ARG_ENABLE(ssl,
  [  --enable-ssl=PREFIX       Enable SSL in varios parts of CORE)],
  [
  	
  	case "$enable_ssl" in
  		n|no)
  			;;
  		y|ye|yes)
  			enable_ssl=yes
  			;;
  		*)
  			SSL_DIR="$withval"
  			LIBS="$LIBS -L$SSL_DIR/lib"
  			enable_ssl=yes
  			;;
  	esac
  ])
  AC_MSG_CHECKING(for OpenSSL support)
  
  if test "$enable_ssl" == "yes" ; then
  	AC_MSG_RESULT(yes)
  	AC_CHECK_LIB(ssl, SSL_library_init,ssl_lib_found=yes,ssl_lib_found=no)
  	AC_CHECK_HEADER(openssl/ssl.h,ssl_lib_found=yes,ssl_lib_found=no)
  	
  	if test "$ssl_lib_found" == "yes"; then
  	
  		OPENSSL_ADDON="-DHAVE_SSL"
		OPENSSL_FLAG="-lssl -lcrypto"
		OPENSSL_LIBDIR="-L$SSL_DIR/include"
		OPENSSL_INCDIR=""
		AC_SUBST(OPENSSL_LIBDIR)
		AC_SUBST(OPENSSL_INCDIR)
		AC_SUBST(OPENSSL_ADDON)
		AC_SUBST(OPENSSL_FLAG)
		openssl dhparam -C 512 | awk '/^-----/ {exit} {print}' > bartlby_v2_dh.h
	fi;
  		
  else
  	#AC_MSG_ERROR([Unable to locate -lssl -lcrypto. OpenSSL support is currently required.])
  	AC_MSG_RESULT(no)
  	
  fi



dnl User stuff

AC_ARG_WITH(user, AC_HELP_STRING([--with-user], [Bartlby User]), BARTLBY_USER="${withval}",BARTLBY_USER="bartlby")
AC_SUBST(BARTLBY_USER)
INSTALL_OPTIONS="-o $BARTLBY_USER";
AC_SUBST(BARTLBY_USER)

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET

AC_PATH_PROG(INSTALL_PATH, install)
if test "x${INSTALL_PATH}" = "x" ; then
 AC_MSG_ERROR(["install" is not found please fix it])
fi

AC_SUBST(INSTALL_PATH)
AC_SUBST(INSTALL_OPTIONS)


# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h malloc.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm gethostbyname gettimeofday inet_ntoa memmove memset setenv socket strdup strstr])


AC_CONFIG_FILES([Makefile.conf bartlby.cfg postinstall-pak preinstall-pak agent_sync.cfg])
AC_OUTPUT
echo "######################################"
echo "#          bartlby Agent              #"
echo "######################################"
echo "Base path: $prefix";
echo "Plugin dir: $PLUGIN_DIR";
echo "Sync Host: $BARTLBY_SYNC";
echo "install: $INSTALL_PATH a $install_path";
