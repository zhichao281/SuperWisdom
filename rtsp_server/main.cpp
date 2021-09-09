#include <stdio.h>
#include <iostream>
#include <memory>
#include <libgen.h>
#include <ctime>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "Config.h"
#include "dlog.h"
#include "Singleton.hpp"
#include "DDRTSPServer.h"
#include "utils.h"
#include "ReadH264File.h"
#include "RtspServerManager.h"

char volatile g_watch = 0;

static void sigHandle(int sig)
{
	LogInfo("Pid: %d: Aborted by signal %s (%d)...", getpid(), (char *)strsignal(sig), sig);
	switch(sig)
	{
		case SIGHUP:
		case SIGINT:
		case SIGQUIT:
		case SIGUSR1:
		case SIGUSR2:
		case SIGTERM:
			{
				stop = true;
				g_watch = 1;
			}
			break;
		case SIGCHLD:
			{
				while(0 < waitpid(-1, NULL, WNOHANG));
			}
			break;
		default:
			break;
	}
}
void ReadH264FileTest()
{
    std::chrono::milliseconds du(1000);
	std::this_thread::sleep_for(du);
    std::string live = "testh264";
    Singleton<RtspServerManager>::GetInstance()->CreateSession(live);
    
    FILE* fp = fopen("t.h264","rb");
    
    if(!fp){
        printf("open file t.h264 failed\n");
        return;
    }
    long file_size = filesize(fp);
    printf("file size=%d\n",file_size);
    unsigned char *buff=new unsigned char[file_size];
    while(true){
        size_t size = (size_t)getNextNalu(fp,buff);
        if(size == -1){
            fseek(fp,0,SEEK_SET);
            continue;
        }
        //printf("nalu size=%d\n",size);
        Singleton<RtspServerManager>::GetInstance()->PushPacket("testh264",(char*)buff,size);
        std::chrono::milliseconds du(30);
	    std::this_thread::sleep_for(du);
    }
    delete[] buff;
}
int main(int argc,char* argv[])
{
    char *currdir =dirname(argv[0]);
    char log_path[512];
    snprintf(log_path,512,"%s/rtsp_server.log",currdir);
    init_logger(log_path, S_DEBUG);

    char conf_file[512];
    snprintf(conf_file,512,"%s/conf.json",currdir);
    Singleton<Config>::Instance();
    try{
        if (!Singleton<Config>::GetInstance()->LoadConfig(conf_file))
        {
            return -1;
        }
    }catch(...){
        LogError("读取配置文件失败");
        return -1;
    }


    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
    portNumBits rtspServerPort = Singleton<Config>::GetInstance()->RtspPort;


    std::shared_ptr<DDRTSPServer> rtspServer = DDRTSPServer::getObject(*env,rtspServerPort);
    while(rtspServer == NULL) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        std::chrono::milliseconds du(2000);
        std::this_thread::sleep_for(du);
        rtspServer = DDRTSPServer::getObject(*env, rtspServerPort);

    }
    Singleton<RtspServerManager>::Instance(rtspServer);
    std::thread th(ReadH264FileTest);
    th.detach();
    LogInfo("Rtsp Server create success");

    // loop
	scheduler->doEventLoop(&g_watch);
}