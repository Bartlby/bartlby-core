AC_DEFUN(AC_CHECK_LIBMYSQLCLIENT, [
  AC_MSG_CHECKING(for MySQL Client support)

  AC_ARG_ENABLE(
    mysql,
    [  --disable-mysql         disable MySQL support],
    WITH_MYSQL="$enableval", WITH_MYSQL="yes")

  AC_ARG_WITH(
    mysql-inc,
    [  --with-mysql-inc=PATH   include path for MySQL headers],
    mysql_incdir="$withval", mysql_incdir="no")

  AC_ARG_WITH(
    mysql-lib,
    [  --with-mysql-lib=PATH   library path for MySQL libraries],
    mysql_libdir="$withval", mysql_libdir="no")

  if test "$WITH_MYSQL" = "no"; then
    AC_MSG_RESULT(no)
    dnl MySQL is required currently, enforce it.
    AC_MSG_ERROR([MySQL support is currently required. You tried to disable it.])
  else
    AC_MSG_RESULT(yes)

    if test -n "$ac_cv_header_mysqlclient_mysql_incdir"; then
      INC_MYSQL="$ac_cv_header_mysqlclient_mysql_incdir"
    fi

    if test -n "$ac_cv_lib_mysqlclient_mysql_libdir"; then
      LIB_MYSQL="$ac_cv_lib_mysqlclient_mysql_libdir"
    fi

    dnl MySQL Header Check
    temp="$CPPFLAGS"
    if test "$mysql_incdir" != "no"; then
      INC_MYSQL="-I$mysql_incdir"
      CPPFLAGS="$temp $INC_MYSQL"
      dnl Un-Set any cached results, since user is specifying a specific directory.
      unset ac_cv_header_mysql_mysql_h
    fi
    AC_CHECK_HEADER(mysql/mysql.h, WITH_MYSQL="yes", WITH_MYSQL="no")
    if test "$WITH_MYSQL" = "no"; then
      dnl If our normal search or user suggested attempt failed, then try the known default for MySQL.
      INC_MYSQL="-I/usr/local/include"
      CPPFLAGS="$temp $INC_MYSQL"
      dnl Un-Set any cached results, to force the second check to go.
      unset ac_cv_header_mysql_mysql_h
      AC_CHECK_HEADER(mysql/mysql.h, WITH_MYSQL="yes", WITH_MYSQL="no")
    fi
    CPPFLAGS="$temp"

    if test "$WITH_MYSQL" = "no"; then
      dnl The headers are required currently, enforce it.
      AC_MSG_ERROR([Unable to locate <mysql/mysql.h>. MySQL support is currently required.])
      dnl The above error will be removed eventually, and we need to clear this var if it fails.
      INC_MYSQL=""
    else
      dnl Cache trick for header.
      ac_cv_header_mysqlclient_mysql_incdir="$INC_MYSQL"
      dnl MySQL Lib Check
      temp="$LIBS"
      if test "$mysql_libdir" != "no"; then
        LIB_MYSQL="-L$mysql_libdir"
        LIBS="$temp $LIB_MYSQL"
        dnl Un-Set any cached results, since user is specifying a specific directory.
        unset ac_cv_lib_mysqlclient_mysql_real_connect
        unset ac_cv_lib_mysqlclient_mysql_real_query
      fi
      AC_CHECK_LIB(mysqlclient, mysql_real_connect, WITH_MYSQL="yes",
      [
        dnl If our normal search or user suggested attempt failed, then try the known default for MySQL.
        LIB_MYSQL="-L/usr/local/lib/mysql"
        LIBS="$temp $LIB_MYSQL"
        dnl Un-Set any cached results, to force the second check to go.
        unset ac_cv_lib_mysqlclient_mysql_real_connect
        unset ac_cv_lib_mysqlclient_mysql_real_query
        echo "Attempting to locate in $LIB_MYSQL..."
        AC_CHECK_LIB(mysqlclient, mysql_real_connect, WITH_MYSQL="yes",
        [
          dnl Try a common RPM location for the MySQL libraries.
          LIB_MYSQL="-L/usr/lib/mysql"
          LIBS="$temp $LIB_MYSQL"
          dnl Un-Set any cached results, to force the second check to go.
          unset ac_cv_lib_mysqlclient_mysql_real_connect
          unset ac_cv_lib_mysqlclient_mysql_real_query
          echo "Attempting to locate in $LIB_MYSQL..."
          AC_CHECK_LIB(mysqlclient, mysql_real_connect, WITH_MYSQL="yes", WITH_MYSQL="no")
        ])
      ])
      if test "$WITH_MYSQL" = "yes"; then
          AC_CHECK_LIB(mysqlclient, mysql_real_query, WITH_MYSQL="yes", WITH_MYSQL="no")
      fi
      LIBS="$temp"

      if test "$WITH_MYSQL" = "no"; then
        dnl The libs are required currently, enforce it.
        AC_MSG_ERROR([Unable to locate -lmysqlclient. MySQL support is currently required.])
        dnl The above error will be removed eventually, and we need to clear this var if it fails.
        LIB_MYSQL=""
      else
        AC_DEFINE(HAVE_MYSQL)
        dnl Cache trick for lib.
        ac_cv_lib_mysqlclient_mysql_libdir="$LIB_MYSQL"
        LIB_MYSQL="$LIB_MYSQL -lmysqlclient"
      fi
    fi

    dnl Subst must happen to remove variables.
    AC_SUBST(INC_MYSQL)
    AC_SUBST(LIB_MYSQL)
  fi
])


#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(bartlby, 1.1.4, helmut@januschka.com)
AC_PREFIX_DEFAULT(/usr/local/bartlby)

dnl check for mysql user

AC_ARG_WITH(mysql-user, AC_HELP_STRING([--with-mysql-user], [Mysql User]), MYSQL_USER="${withval}", MYSQL_USER="root")
AC_SUBST(MYSQL_USER)


dnl check for mysql password 

AC_ARG_WITH(mysql-password,AC_HELP_STRING([--with-mysql-password], [Mysql password]), MYSQL_PASS="$withval", MYSQL_PASS="xxx")
AC_SUBST(MYSQL_PASS)

dnl check for mysql host
AC_ARG_WITH(mysql-host, AC_HELP_STRING([--with-mysql-host], [Mysql host]), MYSQL_HOST="${withval}",MYSQL_HOST="localhost")
AC_SUBST(MYSQL_HOST)

dnl check for mysql db

AC_ARG_WITH(mysql-db,AC_HELP_STRING([--with-mysql-db], [Mysql db]), MYSQL_DB="$withval", MYSQL_DB="bartlby")
AC_SUBST(MYSQL_DB)

dnl check for plugin dir

AC_ARG_WITH(plugin-dir,AC_HELP_STRING([--with-plugin-dir], [Where are the plugins located]), PLUGIN_DIR="${withval}", PLUGIN_DIR="${prefix}/bartlby-plugins/")
AC_SUBST(PLUGIN_DIR)

AC_CHECK_LIB(socket, socket)

AC_CHECK_LIBMYSQLCLIENT


AC_MSG_CHECKING(for nrep enabled)
AC_ARG_ENABLE(
    nrpe,
    [  --enable-nrpe	enable NRPE support],
    WITH_NRPE="$enableval", WITH_NRPE="no")

if test "$WITH_NRPE" = "yes"; then
	NRPE_ADDON="-DWITH_NRPE"
	AC_SUBST(NRPE_ADDON)
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)	
fi;

AC_MSG_CHECKING(for libsnmp support)

AC_ARG_ENABLE(
    snmp,
    [  --enable-snmp	enable SNMP support],
    WITH_SNMP="$enableval", WITH_SNMP="no")

if test "$WITH_SNMP" = "yes"; then
	AC_MSG_RESULT(yes)
	AC_ARG_WITH(snmp-include-dir,
		AC_HELP_STRING(
			[--with-snmp-include-dir],
			[SNMP include base directory [[/usr/(local/)include]]]),
		[snmp_include_dir=$withval])
	AC_ARG_WITH(snmp-lib-dir,
		AC_HELP_STRING(
			[--with-snmp-lib-dir],
			[SNMP library directory [[/usr/(local/)lib]]]),
		[snmp_lib_dir=$withval])
		
		
	# Determine UCD or Net-SNMP include path
	if test "x$snmp_include_dir" != "x"; then
	  for i in / /ucd-snmp /include/ucd-snmp; do
	    test -f $snmp_include_dir/$i/snmp.h && SNMP_INCDIR=$snmp_include_dir/$i
	  done
	  for i in / /net-snmp /include/net-snmp; do
	    test -f $snmp_include_dir/$i/net-snmp-config.h && SNMP_INCDIR=$snmp_include_dir/$i
	  done
	else
	  for i in /usr/local/include /usr/include; do
	    test -f $i/snmp.h && SNMP_INCDIR=$i
	    test -f $i/ucd-snmp/snmp.h && SNMP_INCDIR=$i/ucd-snmp
	    test -f $i/net-snmp/net-snmp-config.h && SNMP_INCDIR=$i/net-snmp
	    test -f $i/snmp/snmp.h && SNMP_INCDIR=$i/snmp
	    test -f $i/snmp/include/ucd-snmp/snmp.h && SNMP_INCDIR=$i/snmp/include/ucd-snmp
	    test -f $i/snmp/include/net-snmp/net-snmp-config.h && SNMP_INCDIR=$i/snmp/include/net-snmp
	  done
	fi
	
	# Determine UCD or Net-SNMP library path
	if test "x$snmp_lib_dir" != "x"; then
	  test -f $snmp_lib_dir/libsnmp.a -o -f $snmp_lib_dir/libsnmp.so && SNMP_LIBDIR=$snmp_lib_dir
	  test -f $snmp_lib_dir/libnetsnmp.a -o -f $snmp_lib_dir/libnetsnmp.so && SNMP_LIBDIR=$snmp_lib_dir
	else
	  for i in /usr/local/lib /usr/lib /usr/sfw/lib; do
	    test -f $i/libsnmp.a -o -f $i/libsnmp.so && SNMP_LIBDIR=$i
	    test -f $i/libnetsnmp.a -o -f $i/libnetsnmp.so && SNMP_LIBDIR=$i
	  done
	fi
	
	if test -z "$SNMP_INCDIR"; then
	  if test "x$snmp_include_dir" != "x";then
	    AC_MSG_ERROR(Cannot find SNMP headers under $snmp_include_dir)
	  else
	    AC_MSG_ERROR(Cannot find SNMP headers.  Use --with-snmp-include-dir= to specify non-default path.)
	  fi
	fi
	
	if test -z "$SNMP_LIBDIR"; then
	  if test "x$snmp_lib_dir" != "x";then
	    AC_MSG_ERROR(Cannot find SNMP libraries under $snmp_lib_dir)
	  else
	    AC_MSG_ERROR(Cannot find SNMP libraries.  Use --with-snmp-lib-dir= to specify non-default path.)
	  fi
	fi
	LDFLAGS="$LDFLAGS -L$SNMP_LIBDIR"
	CPPFLAGS="$CPPFLAGS -I$SNMP_INCDIR"
	
	AC_CHECK_LIB(snmp, snmp_timeout,,
		AC_MSG_ERROR(Cannot find UCD-SNMP libraries))
	
	SNMP_ADDON="-DSNMP_ADDON"
	SNMP_FLAG="-lsnmp"
	SNMP_LIBDIR="-L$SNMP_LIBDIR"
	SNMP_INCDIR="-I$SNMP_INCDIR"
	AC_SUBST(SNMP_LIBDIR)
	AC_SUBST(SNMP_INCDIR)
	AC_SUBST(SNMP_ADDON)
	AC_SUBST(SNMP_FLAG)
	
else
	AC_MSG_RESULT(no)
fi;	

dnl Check for ssl library.
  enable_ssl=yes
  
  AC_ARG_ENABLE(ssl,
  [  --enable-ssl=PREFIX       Enable SSL in varios parts of CORE)],
  [
  	case "$withval" in
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
		openssl dhparam -C 512 | awk '/^-----/ {exit} {print}' > include/bartlby_v2_dh.h
	fi;
  		
  else
  	AC_MSG_ERROR([Unable to locate -lssl -lcrypto. OpenSSL support is currently required.])
  	
  fi



AC_CHECK_LIB([dl], [dlopen],[LD_DL="-ldl"],[AC_MSG_ERROR([libdl needed to be found])])
AC_SUBST(LD_DL)


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
AC_PATH_PROG(OPEN_SSL_PATH, openssl)
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


AC_CONFIG_FILES([Makefile.conf bartlby.cfg bartlby.startup preinstall-pak postinstall-pak])
AC_OUTPUT
echo "######################################"
echo "#          bartlby Core              #"
echo "######################################"
echo "Base path: $prefix";
echo "Plugin dir: $PLUGIN_DIR";
echo "MySQL settings"
echo "Include: $INC_MYSQL";
echo "lib: $LIB_MYSQL";
echo "User: $MYSQL_USER";
echo "Password: $MYSQL_PASS";
echo "Host: $MYSQL_HOST";
echo "database: $MYSQL_DB";
echo "install: $INSTALL_PATH a $install_path";
