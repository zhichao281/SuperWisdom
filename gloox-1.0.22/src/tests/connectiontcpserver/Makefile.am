##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual 

noinst_PROGRAMS = connectiontcpserver_test

connectiontcpserver_test_SOURCES = connectiontcpserver_test.cpp
connectiontcpserver_test_LDADD = ../../connectiontcpserver.o ../../gloox.o ../../util.o ../../logsink.o \
                                 ../../connectiontcpbase.o ../../mutex.o ../../dns.o ../../prep.o ../../connectiontcpclient.o
connectiontcpserver_test_CFLAGS = $(CPPFLAGS)

