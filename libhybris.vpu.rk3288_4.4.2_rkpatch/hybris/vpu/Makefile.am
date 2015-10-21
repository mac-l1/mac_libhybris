lib_LTLIBRARIES = \
	libvpu.la

libvpu_la_SOURCES = vpu.c
libvpu_la_CFLAGS = -I$(top_srcdir)/include
if WANT_TRACE
libvpu_la_CFLAGS += -DDEBUG
endif
if WANT_DEBUG
libvpu_la_CFLAGS += -ggdb -O0
endif
libvpu_la_LDFLAGS = \
	$(top_builddir)/common/libhybris-common.la \
	-version-info "1":"0":"0"

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvpu.pc
