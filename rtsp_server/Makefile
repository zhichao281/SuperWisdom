TARGET := rtsp_server.bin

CC := gcc
CPP := g++

#### ------------------ cc ----------------
CFLAGS := -std=c++17 -Wall -O0 -Wfatal-errors
# 去掉warning: XXX is deprecated [-Wdeprecated-declarations]警告
CFLAGS += -Wno-error=deprecated-declarations -Wno-deprecated-declarations
CFLAGS += -Wl,--as-needed
CFLAGS += -DNO_OPENSSL
#CFLAGS += '-Wl,-rpath=$$ORIGIN/live555/lib'

INCLUDE = 
INCLUDE += -I./jsonla
INCLUDE += -I./live555
INCLUDE += -I./live555/include/BasicUsageEnvironment
INCLUDE += -I./live555/include/groupsock
INCLUDE += -I./live555/include/liveMedia
INCLUDE += -I./live555/include/UsageEnvironment


#LDFLAGS =  -L./lib
LDFLAGS = -L./live555/lib

LIBS = -lpthread
LIBS += -lliveMedia
LIBS += -lBasicUsageEnvironment
LIBS += -lgroupsock
LIBS += -lUsageEnvironment


SRCS =
SRCS += dlog.cpp
SRCS += Config.cpp
SRCS += jsonla/jsonla.cpp
SRCS += live555/DDRTSPServer.cpp
SRCS += live555/H264CamSMSubs.cpp
SRCS += live555/CamStreamSource.cpp
SRCS += RtspServerManager.cpp
SRCS += main.cpp


OBJS := $(patsubst %.cpp, %.o, $(SRCS))
OBJS := $(patsubst %.c, %.o, $(OBJS))

#### ----------------------------------------

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<  $(LDFLAGS) $(LIBS)

%.o:%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -o $@ -c $<  $(LDFLAGS) $(LIBS)

all: $(OBJS)
	$(CPP) $(CFLAGS)  -o $(TARGET) $(OBJS) $(INCLUDE) ${ORIGIN} $(LDFLAGS) $(LIBS) 
clean:
	@rm -rf $(TARGET) $(CLIENT) $(OBJS) *.o
 
