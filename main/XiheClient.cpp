#include "XiheClient.h"
#include "esp_log.h"
#include "Log.h"

extern "C"
{
#include "lcd.h"
}

GuiManager XiheClient::m_cGuiManager;

XiheClient::XiheClient()
{
    m_pRunThread = nullptr;
    m_strRemoteIp = "";
    m_nRemotePort = 0;
    m_pRTSPClient = nullptr;
    m_pRenderThread = nullptr;

    GuiManager::OnButtonClicked pOnConnectClicked = std::bind(&XiheClient::OnConnect, this, std::placeholders::_1);
    GuiManager::OnButtonClicked pOnExitPlayClicked = std::bind(&XiheClient::OnExitPlay, this, std::placeholders::_1);
    m_cGuiManager.SetBtnConnetClickedCallback(pOnConnectClicked);
    m_cGuiManager.SetBtnExitPlayClickedCallback(pOnExitPlayClicked);
}

XiheClient::~XiheClient()
{
    ReleaseAll();
    m_cGuiManager.SetBtnConnetClickedCallback(nullptr);
    m_cGuiManager.SetBtnExitPlayClickedCallback(nullptr);
}

int32_t XiheClient::ReleaseAll()
{
    Trace("[%p][XiheClient::ReleaseAll]", this);

    ExitTaskThread();
    {
        std::lock_guard<std::mutex> lock(m_cTaskListLock);
        m_cTaskList.clear();
    }
    
    m_bStopRender = true;
    if (m_pRenderThread != nullptr)
    {
        if (m_pRenderThread->joinable())
        {
            m_pRenderThread->join();
        }
        delete m_pRenderThread;
        m_pRenderThread = nullptr;
    }
    m_pRenderVideoFrame = nullptr;

    delete m_pRTSPClient;
    m_pRTSPClient = nullptr;

    return 0;
}

int32_t XiheClient::Run()
{
    Trace("[%p][XiheClient::Run] Run", this);

    ReleaseAll();
    int32_t ret = m_cGuiManager.InitUI();
    if (ret != 0)
    {
        Error("[%p][XiheClient::Run] InitUI fail,return %d", this, ret);
        ret = -1; goto fail;
    }

    m_pRunThread = new std::thread(&XiheClient::RunThread, this);
    return 0;

fail:
    ReleaseAll();
    return ret;
}

int32_t XiheClient::RunThread()
{
    Trace("[%p][XiheClient::RunThread] start RunThread", this);
    while (true)
    {
        while (true)
        {
            std::shared_ptr<Task> pTask = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_cTaskListLock);
                if (!m_cTaskList.empty())
                {
                    pTask = m_cTaskList.front();
                    m_cTaskList.pop_front();
                }
            }

            if (pTask == nullptr)
            {
                break;
            }
            if (pTask->GetTaskType() == TASK_EXIT)
            {
                Trace("[%p][XiheClient::RunThread] exit run thread", this);
                return 0;
            }

            int32_t ret =
                pTask->GetTaskType() == TASK_PLAY ? Play(pTask->m_sParam.m_sPlayParam) :
                pTask->GetTaskType() == TASK_STOP_PLAY ? StopPlay() : -1;

            if (ret >= 0)
            {
                Trace("[%p][XiheClient::RunThread] task:%d return:%d", this, pTask->GetTaskType(), ret);
            }
            else
            {
                Error("[%p][XiheClient::RunThread] task:%d return:%d", this, pTask->GetTaskType(), ret);
            }
        }

        {
            std::unique_lock<std::mutex> lock(m_cRunCvLock);
            m_cRunCv.wait(lock);
        }
    }
    return 0;
}

void XiheClient::OnRecvVideoPacket(std::shared_ptr<MediaPacket>& video)
{
    std::lock_guard<std::mutex> lock(m_pRenderVideoFrameLock);
    m_pRenderVideoFrame = video;
}

void XiheClient::OnVideoReady(const VideoInfo& info)
{
    Trace("[%p][XIheClient::OnVideoReady] video ready width:%d height:%d codec:%d",
        this, info.m_nWidth, info.m_nHight, info.m_nCodecID);
}

int32_t XiheClient::Play(const PlayParam& param)
{
    Trace("[%p][XiheClient::Play] Play ip:%s port:%d device:%s", this, param.m_strRemoteIp.c_str(), param.m_nRemotePort, param.m_strDevideName.c_str());

    if (m_pRTSPClient == nullptr)
    {
        m_pRTSPClient = new RTSPClient();
        RTSPClient::VideoReadyCallbaclk pVideoReadyCallbaclk = std::bind(&XiheClient::OnVideoReady, this, std::placeholders::_1);
        m_pRTSPClient->SetVideoReadyCallbaclk(pVideoReadyCallbaclk);
        RTPParser::MediaPacketCallbaclk pVideoCallback = std::bind(&XiheClient::OnRecvVideoPacket, this, std::placeholders::_1);
        m_pRTSPClient->SetVideoPacketCallbaclk(pVideoCallback);
    }

    char url[128];
    sprintf(url, "rtsp://%s:%d/device/%s?image=mpeg&resolution=480*272&fps=10", param.m_strRemoteIp.c_str(), param.m_nRemotePort, param.m_strDevideName.c_str());
    int ret = m_pRTSPClient->PlayUrl(url, RTSPClient::TransportType::TCP);
    if (ret != 0)
    {
        Error("[%p][XIheClient::PlayDevice] PlayUrl fail.return:%d", this, ret);
        return -1;
    }

    if (m_pRenderThread == nullptr)
    {
        m_pRenderThread = new std::thread(&XiheClient::RenderThread, this);
    }

    return 0;
}

int32_t XiheClient::StopPlay()
{
    Trace("[%p][XiheClient::StopPlay] StopPlay", this);
    if (m_pRTSPClient == nullptr)
    {
        return 0;
    }

    int32_t ret = m_pRTSPClient->CloseClient();
    if (ret != 0)
    {
        Trace("[%p][XiheClient::StopPlay] close rtsp client fail,return:%d", this, ret);
        return -1;
    }

    m_bStopRender = true;
    if (m_pRenderThread != nullptr)
    {
        if (m_pRenderThread->joinable())
        {
            m_pRenderThread->join();
        }
        delete m_pRenderThread;
        m_pRenderThread = nullptr;
    }

    return 0;
}

int32_t XiheClient::ExitTaskThread()
{
    Trace("[%p][XiheClient::ExitTaskThread] ExitTaskThread", this);

    std::shared_ptr<Task> task = std::make_shared<Task>(TASK_EXIT);
    int32_t ret = PutTaskToList(task);
    if (ret != 0)
    {
        Error("[%p][XiheClient::ExitTaskThread] put exie task to list fail,return:%d", this, ret);
        return -1;
    }

    if (m_pRunThread != nullptr)
    {
        if (m_pRunThread->joinable())
        {
            m_pRunThread->join();
        }

        delete m_pRunThread;
        m_pRunThread = nullptr;
    }

    return 0;
}

int32_t  XiheClient::PutTaskToList(const std::shared_ptr<Task>& task)
{
    if (task == nullptr)
    {
        Error("[%p][XiheClient::PutTaskToList] task is null", this);
        return -1;
    }

    {
        std::lock_guard<std::mutex> lock(m_cTaskListLock);
        m_cTaskList.push_back(task);
    }
    m_cRunCv.notify_one();

    return 0;
}

void XiheClient::OnConnect(void* user)
{
    Trace("[%p][XiheClient::OnConnect] OnConnect", this);

    std::string ip(m_cGuiManager.GetConnectIp());
    int port = atoi(m_cGuiManager.GetConnectPort());
    if (ip == "" || port == 0)
    {
        Error("[%p][XiheClient::OnConnect] input error ip:%s port:%d", this, ip, port);
        return;
    }

    std::shared_ptr<Task> task = std::make_shared<Task>(TASK_PLAY);
    task->m_sParam.m_sPlayParam.m_strRemoteIp = ip;
    task->m_sParam.m_sPlayParam.m_nRemotePort = port;
    task->m_sParam.m_sPlayParam.m_strDevideName = "video0";
    int32_t ret = PutTaskToList(task);
    if (ret != 0)
    {
        Error("[%p][XiheClient::ExitTaskThread] put play task to list fail,return:%d", this, ret);
        return;
    }

    m_cGuiManager.ShowPlayWin();
}

void XiheClient::OnExitPlay(void* user)
{
    Trace("[%p][XiheClient::OnConnect] OnExitPlay", this);

    std::shared_ptr<Task> task = std::make_shared<Task>(TASK_STOP_PLAY);
    int32_t ret = PutTaskToList(task);
    if (ret != 0)
    {
        Error("[%p][XiheClient::ExitTaskThread] put play task to list fail,return:%d", this, ret);
        return;
    }

    m_cGuiManager.ShowLoginWin();
}

void XiheClient::RenderThread()
{
    m_bStopRender = false;
    while (!m_bStopRender)
    {
        std::shared_ptr<MediaPacket> pVideoFrame = nullptr;
        {
            std::lock_guard<std::mutex> lock(m_pRenderVideoFrameLock);
            if (m_pRenderVideoFrame != nullptr)
            {
                pVideoFrame = m_pRenderVideoFrame;
                m_pRenderVideoFrame = nullptr;
            }
        }

        if (pVideoFrame == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        m_cGuiManager.RenderVideoFrame(pVideoFrame);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    m_cGuiManager.ShowLoginWin();
    //RefreshScreen();
}

XiheClient::Task::Task(TaskType type) : m_sParam(type)
{
    m_eTaskType = type;
}

XiheClient::Task::~Task()
{
    switch (m_eTaskType)
    {
    case XiheClient::TASK_NONE:
        break;
    case XiheClient::TASK_PLAY:
        m_sParam.m_sPlayParam.~PlayParam();
        break;
    case XiheClient::TASK_STOP_PLAY:
        break;
    case XiheClient::TASK_EXIT:
        break;
    default:
        break;
    }
}

XiheClient::Task::Param::Param(XiheClient::TaskType type)
{
    switch (type)
    {
    case XiheClient::TASK_NONE:
        break;
    case XiheClient::TASK_PLAY:
        new(&m_sPlayParam) PlayParam;
        break;
    case XiheClient::TASK_STOP_PLAY:
        break;
    case XiheClient::TASK_EXIT:
        break;
    default:
        break;
    }
}

XiheClient::Task::Param::~Param()
{
}