#pragma once
#include <stdint.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include "GuiManager.h"
#include "RTSPClient.h"

class XiheClient
{
public:
    enum TaskType
    {
        TASK_NONE = 0,
        TASK_PLAY,
        TASK_STOP_PLAY,
        TASK_EXIT
    };
    typedef struct PlayParam
    {
        std::string m_strRemoteIp = "";
        uint16_t m_nRemotePort = 0;
        std::string m_strDevideName = "";
        PlayParam()
        {
            m_strRemoteIp = "";
            m_nRemotePort = 0;
            m_strDevideName = "";
        }
        ~PlayParam() {};
    }PlayParam;
    typedef struct Task
    {
    private:
        TaskType m_eTaskType = TASK_NONE;

    public:
        Task(TaskType type);
        ~Task();

        union Param
        {
            PlayParam m_sPlayParam;
            Param(TaskType type);
            ~Param();
        }m_sParam;

        inline TaskType GetTaskType() { return m_eTaskType; };
    }Task;

public:
    XiheClient();
    ~XiheClient();
    int32_t Run();

private:
    int32_t ReleaseAll();
    int32_t RunThread();
    void OnRecvVideoPacket(std::shared_ptr<MediaPacket>& video);
    void OnVideoReady(const VideoInfo& info);
    int32_t Play(const PlayParam& param);
    int32_t StopPlay();
    void OnConnect(void* user);
    void OnExitPlay(void* user);
    int32_t ExitTaskThread();
    int32_t PutTaskToList(const std::shared_ptr<Task>& task);
    void RenderThread();

private:
    static GuiManager m_cGuiManager;
    std::thread* m_pRunThread;
    std::condition_variable m_cRunCv;
    std::mutex m_cRunCvLock;
    std::string m_strRemoteIp;
    uint16_t m_nRemotePort;
    RTSPClient* m_pRTSPClient;
    std::list<std::shared_ptr<Task>> m_cTaskList;
    std::mutex m_cTaskListLock;
    bool m_bStopRender;
    std::thread* m_pRenderThread;
    std::mutex m_pRenderVideoFrameLock;
    std::shared_ptr<MediaPacket> m_pRenderVideoFrame;
};