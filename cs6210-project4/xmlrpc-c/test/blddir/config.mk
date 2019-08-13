# config.mk is generated by 'configure' using config.mk.in
# as a template and information that 'configure' gathers from the build
# system and from user options.

# config.mk should someday replace most of the other files that
# 'configure' generates, thus simplifying development and customization.
# config.mk is intended to contain information specific to the
# particular build environment or user build choices.

# Furthermore, most of the logic in 'configure', and thus 'configure.in',
# should go into the make files to simplify the build.  config.mk
# should just pass raw configure variables through to the make file.

# Tokens of the form @TOKEN@ in the template file get replaced by
# 'configure' with the values of variables of the same name within
# 'configure', because of a AC_SUBST(TOKEN) statement in the
# 'configure.in' from which 'configure' was built.

# Here are the options the user chose on 'configure':

ENABLE_ABYSS_SERVER    = yes
ENABLE_ABYSS_THREADS   = yes
ENABLE_CPLUSPLUS       = yes
ENABLE_CGI_SERVER      = yes
ENABLE_LIBXML2_BACKEND = no

MUST_BUILD_WININET_CLIENT = no
MUST_BUILD_CURL_CLIENT    = yes
MUST_BUILD_LIBWWW_CLIENT  = no
MUST_BUILD_ABYSS_OPENSSL  = yes
BUILD_TOOLS  = yes
BUILD_XMLRPC_PSTREAM  = no
WININET_LDADD = 
WININET_LIBDIR = 
CURL_CONFIG = /usr/local/bin/curl-config
CURL_LDADD = -L/usr/local/lib -lcurl
CURL_LIBDIR = /usr/local/lib
LIBWWW_LDADD = 
LIBWWW_LIBDIR = 
FEATURE_LIST = c++ abyss-server curl-client 
ABS_SRCDIR = /home/nihar/Documents/AOS/assignment4/xmlrpc-c
PREFIX = /usr/local


HAVE_WCHAR_H_DEFINE = 1

# Stuff 'configure' figured out about our build platform:

SHELL = /bin/bash
CC = gcc
CXX = g++
CCLD = $(CC)
CXXLD = $(CXX)
AR = ar
RANLIB = ranlib
LN_S = ln -s
INSTALL = $(SRCDIR)/install-sh

C_COMPILER_GNU = yes
CXX_COMPILER_GNU = yes

# Stuff 'configure' figured out via AC_CANONICAL_HOST macro in configure.in
# and config.guess program and 'configure' command options:

# HOST_OS names the operating system on which Xmlrpc-c is to run.
# E.g. "linux-gnu".
HOST_OS = linux-gnu

###############################################################################

MUST_BUILD_CLIENT = no
ifeq ($(MUST_BUILD_WININET_CLIENT),yes)
  MUST_BUILD_CLIENT = yes
endif
ifeq ($(MUST_BUILD_CURL_CLIENT),yes)
  MUST_BUILD_CLIENT = yes
endif
ifeq ($(MUST_BUILD_LIBWWW_CLIENT),yes)
  MUST_BUILD_CLIENT = yes
endif


##############################################################################
# SHARED LIBRARY STUFF
##############################################################################

# Shared libraries are very difficult, because how you build and use
# them varies greatly from one platform to the next.  

# First, we break down shared library schemes into a few major types,
# and indicate the type by SHARED_LIB_TYPE.

# We also have a bunch of other make variables that reflect the different
# ways we have to build on and for different platforms:

# CFLAGS_SHLIB is a set of flags needed to compile a module which will
# become part of a shared library.

# On older systems, you have to make shared libraries out of position
# independent code, so you need -fpic or -fPIC here.  (The rule is: if
# -fpic works, use it.  If it bombs, go to -fPIC).  On newer systems,
# it isn't necessary, but can save real memory at the expense of
# execution speed.  Without position independent code, the library
# loader may have to patch addresses into the executable text.  On an
# older system, this would cause a program crash because the loader
# would be writing into read-only shared memory.  But on newer
# systems, the system silently creates a private mapping of the page
# or segment being modified (the "copy on write" phenomenon).  So it
# needs its own private real page frame.

# We have seen -fPIC required on IA64 and AMD64 machines (GNU
# compiler/linker).  Build-time linking fails without it.  I don't
# know why -- history seems to be repeating itself.  2005.02.23.

# SHLIB_CLIB is the link option to include the C library in a shared library,
# normally "-lc".  On typical systems, this serves no purpose.  On some,
# though, it causes information about which C library to use to be recorded
# in the shared library and thus choose the correct library among several or
# avoid using an incompatible one.  But on some systems, the link fails.
# On 2002.09.30, "John H. DuBois III" <spcecdt@armory.com> reports that on 
# SCO OpenServer, he gets the following error message with -lc:
#
#  -lc; relocations referenced  ;  from file(s) /usr/ccs/lib/libc.so(random.o);
#   fatal error: relocations remain against allocatable but non-writable 
#   section: ; .text
#
# On Bryan's system, with gcc 2.95.3 and glibc 2.2.2, -lc causes
# throws (from anywhere in a program that links the shared library)
# not to work.  I have no idea how.

# LDFLAGS_SHLIB is the linker (Ld) flags needed to generate a shared
# library from object files.  It may use $(SONAME) as the soname for
# the shared library being created (assuming sonames exist).
#
# This make file defines these functions that the including make file
# can use:
#
#   $(call shlibfn, LIBNAMELIST): file names of shared libraries
#     whose base names are LIBNAMELIST.  E.g. if LIBNAMELIST is
#     "libfoo libbar", function returns "libfoo.so.3.1 libbar.so.3.1"
#
#   $(call shliblefn, LIBNAMELIST): same as shlibfn, but for the file you
#     use at link-edit time.  E.g. libfoo.so .

# NEED_RPATH says on this platform, when you link-edit an executable you
# need to have -R linker options to tell where to look, at run time,
# for the shared libraries that the program uses.  The linker puts that
# information into the executable.

# NEED_WL_RPATH is like NEED_RPATH, but it's a compiler option for when
# you have the compiler call the linker.  So E.g. "-Wl,-rpath,/my/runtime",
# which tells the compiler to pass the option "-rpath /my/runtime" to
# the linker.

# Defaults:
NEED_WL_RPATH=no
NEED_RPATH=no

# We build shared libraries only for platforms for which we've figured
# out how.  For the rest, we have this default:
SHARED_LIB_TYPE = NONE
MUST_BUILD_SHLIB = N
MUST_BUILD_SHLIBLE = N
shlibfn = $(1:%=%.shlibdummy)
shliblefn = $(1:%=%.shlibledummy)

# HOST_OS is usually has a version number suffix, e.g. "aix5.3.0.0", so
# we compare based on prefix.

ifeq ($(patsubst linux-%,linux-,$(HOST_OS)),linux-)
  # Examples we've seen that work here are linux-gnuXXX and linux-uclibcXXX
  # Assume linker is GNU Compiler (gcc)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
#  SHLIB_CLIB = -lc
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
  CFLAGS_SHLIB=-fPIC
endif

ifeq ($(patsubst solaris%,solaris,$(HOST_OS)),solaris)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  # We assume Sun compiler and linker here.  It isn't clear what to do
  # about a user who uses GNU compiler and Ld instead.  For that, the
  # options should be the same as "linux-gnu" platform, above, except
  # with NEED_WL_RPATH.  If the user uses the GNU compiler but the Sun
  # linker, it's even more complicated: we need an rpath option of the
  # form -Wl,-R .

  # Solaris compiler (Sun C 5.5) can't take multiple ld options as
  # -Wl,-a,-b .  Ld sees -a,-b in that case.
  LDFLAGS_SHLIB = -Wl,-Bdynamic -Wl,-G -Wl,-h -Wl,$(SONAME)
  CFLAGS_SHLIB = -Kpic
  NEED_RPATH=yes
endif

ifeq ($(patsubst aix%,aix,$(HOST_OS)),aix)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
# AIX can use a classic .a archive file as a shared library, and that is
# how e.g. libc works.  But as of late, it also can use an XCOFF file with
# the shared flag set, with the conventional suffix .so.  We build our library
# that way to avoid confusion with .a static link libraries.
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  # -brtl says "rtl option".  rtl option says to allow runtime linking, which
  # means that the link editor can use a share object (.so) file instead of an
  # archive (.a) file and build the library to link to that shared object at
  # runtime.  Without -brtl, the link editor ignores .so files.  One thing
  # that is typically a .so file is the Curl library.
  LDFLAGS_SHLIB = -qmkshrobj -brtl
  ifeq ($(C_COMPILER_GNU), no)
    C_COMPILER_IBM = yes
    CXX_COMPILER_IBM = yes
  endif
endif

ifeq ($(patsubst hpux%,hpux,$(HOST_OS)),hpux)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = sl
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB: -shared -fPIC
endif

ifeq ($(patsubst osf%,osf,$(HOST_OS)),osf)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB = -shared -expect_unresolved
endif

ifeq ($(patsubst netbsd%,netbsd,$(HOST_OS)),netbsd)
  SHARED_LIB_TYPE = unix
  SHLIB_SUFFIX = so
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  CFLAGS_SHLIB = -fpic
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
  NEED_WL_RPATH=yes
endif

ifeq ($(patsubst freebsd%,freebsd,$(HOST_OS)),freebsd)
  SHARED_LIB_TYPE = unix
  SHLIB_SUFFIX = so
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  CFLAGS_SHLIB = -fpic
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
  NEED_WL_RPATH=yes
endif

ifeq ($(findstring interix,$(HOST_OS)),interix)
  SHARED_LIB_TYPE = unix
  SHLIB_SUFFIX = so
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  CFLAGS_SHLIB =
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
  NEED_WL_RPATH=yes
endif

ifeq ($(patsubst dragonfly%,dragonfly,$(HOST_OS)),dragonfly)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  CFLAGS_SHLIB = -fpic
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
endif

ifeq ($(patsubst beos%,beos,$(HOST_OS)),beos)
  SHARED_LIB_TYPE = unix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ).$(MIN))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB = -nostart
endif

ifeq ($(patsubst darwin%,darwin,$(HOST_OS)),darwin)
  SHARED_LIB_TYPE = dylib
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = dylib
  shlibfn = $(1:%=%.$(MAJ).$(MIN).$(SHLIB_SUFFIX))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB = -dynamiclib -undefined suppress -single_module \
	-flat_namespace -install_name $(LIBINST_DIR)/$(SONAME) $(SHLIB_CLIB)
endif

ifeq ($(patsubst irix%,irix,$(HOST_OS)),irix)
  SHARED_LIB_TYPE = irix
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = Y
  SHLIB_SUFFIX = so
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX).$(MAJ))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))

  VERSIONPERLPROG = \
    print "sgi$(MAJ)." . join(":sgi$(MAJ) . ", (0..$(MIN))) . "\n"
  LDFLAGS_SHLIB = -shared -n32 -soname $(SONAME) \
    -set_version $(shell perl -e '$(VERSIONPERLPROG)') -lc
endif

ifeq ($(patsubst cygwin%,cygwin,$(HOST_OS)),cygwin)
  SHARED_LIB_TYPE = dll
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = N
  SHLIB_SUFFIX = dll
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
endif

ifeq ($(patsubst mingw32%,mingw32,$(HOST_OS)),mingw32)
  SHARED_LIB_TYPE = dll
  MUST_BUILD_SHLIB = Y
  MUST_BUILD_SHLIBLE = N
  SHLIB_SUFFIX = dll
  shlibfn = $(1:%=%.$(SHLIB_SUFFIX))
  shliblefn = $(1:%=%.$(SHLIB_SUFFIX))
  LDFLAGS_SHLIB = -shared -Wl,-soname,$(SONAME) $(SHLIB_CLIB)
  MSVCRT = yes
endif

ifeq ($(patsubst mingw32%,mingw32,$(HOST_OS)),mingw32)
  SOCKETLIBOPT = -lws2_32 -lwsock32
else
  SOCKETLIBOPT = 
endif

##############################################################################
#                     MISCELLANEOUS
##############################################################################

# CC_FOR_BUILD is the compiler to use to generate build tools, which we
# will then run to build the product.  The typical reason this would be
# different from CC is that you're cross-compiling: the product will run
# in Environment A, but you're building in Environment B, so you must
# build the build tools for Environment B.

# The build system uses CC_FOR_BUILD for linking as well.

# The build system use CFLAGS_FOR_BUILD and LDFLAGS_FOR_BUILD when compiling
# and linking, respectively, build tools.

# The cross compiling user can update config.mk or override
# CC_FOR_BUILD on a make command.

# LDFLAGS_FOR_BUILD is similar

# These variable names are conventional.

CC_FOR_BUILD = $(CC)
CFLAGS_FOR_BUILD = $(CFLAGS)
LDFLAGS_FOR_BUILD = $(LDFLAGS)


ifeq ($(C_COMPILER_GNU),yes)
  CFLAGS_NO_INLINE_WARNING = -Wno-inline
else
  CFLAGS_NO_INLINE_WARNING =
endif

# Today, we use make files only on systems that have pthreads (libpthread),
# so the following is hardcoded.  But it is conceivable that we could later
# use make files on something that doesn't, like some Windows environment.
HAVE_PTHREAD = yes
# Much of our code uses pthreads and some code that doesn't use them directly
# needs them anyway because it uses libraries that are meant to work with othe
# code that does.  So we build everything with pthread capability.
#
# On some systems, that just means we have to link a pthread library.  But
# on other systems, it is more involved and the compiler and linker have a
# -pthread option to take care of everything, including linking whatever
# libraries are required.
ifeq ($(C_COMPILER_GNU),yes)
  # We assume the linker is GCC as well.
  CFLAGS_PTHREAD = -pthread
  LDFLAGS_PTHREAD = -pthread
  THREAD_LIBS =
else
  CFLAGS_PTHREAD =
  LDFLAGS_PTHREAD =
  THREAD_LIBS = -lpthread
endif

# Here are the commands 'make install' uses to install various kinds of files:

INSTALL_PROGRAM ?= $(INSTALL) -c -m 755
INSTALL_SHLIB   ?= $(INSTALL) -c -m 755
INSTALL_DATA    ?= $(INSTALL) -c -m 644
INSTALL_SCRIPT  ?= $(INSTALL) -c -m 755

# Here are the locations at which 'make install' puts files:

# PREFIX is designed to be overridden at make time if the user decides
# he doesn't like the default specified at 'configure' time.

prefix = $(PREFIX)
datarootdir = $(DATAROOT_DIR)

#datarootdir is the new Autoconf(2.60) name for datadir, which is still
#accepted, but a warning is issued if datarootdir is not also used.

exec_prefix = ${prefix}
DATAROOT_DIR = ${prefix}/share
DATAINST_DIR = ${datarootdir}
LIBINST_DIR = ${exec_prefix}/lib
HEADERINST_DIR = ${prefix}/include
PROGRAMINST_DIR = ${exec_prefix}/bin
MANINST_DIR = ${datarootdir}/man/man1
PKGCONFIGINST_DIR = ${exec_prefix}/lib/pkgconfig

# DESTDIR is designed to be overridden at make time in order to relocate
# the entire install into a subdirectory.
DESTDIR =

# VPATH probably doesn't belong in this file, but it's a convenient
# place to set it once.  VPATH is a special Make variable that tells
# Make where to look for dependencies.  E.g. if a make file says bar.c
# is a dependency of bar.o and VPATH is ".:/usr/src/mypkg", Make will
# look for bar.c first in the current directory (.) (as it would with
# no VPATH), then in /usr/src/mypkg.  The purpose of this is to allow
# you to build in a fresh build directory, while your source stays in
# the read-only directory /usr/src/mypkg .

VPATH := .:$(SRCDIR)/$(SUBDIR)

HAVE_OPENSSL = @HAVE_OPENSSL@
OPENSSL_LDADD = -lssl -lcrypto
OPENSSL_CFLAGS = 
