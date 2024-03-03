#include "MJPEGRTPParser.h"
#include "Log/Log.h"

MJPEGRTPParser::MJPEGRTPParser()
{
    m_pMediaPacketCallbaclk = nullptr;
    m_nPaylodaType = 97;
    m_nSSRC = 0;
    m_nLastPackTime = 0;
}

MJPEGRTPParser::~MJPEGRTPParser()
{
    ReleaseAll();
}

int32_t MJPEGRTPParser::ReleaseAll()
{
    m_PacketBuff.ClearBuff(0);
    m_pMediaPacketCallbaclk = nullptr;
    m_nPaylodaType = 97;
    m_nSSRC = 0;
    m_nLastPackTime = 0;

    return 0;
}

void MJPEGRTPParser::GetPaylodaType(uint8_t& type)
{
    type = m_nPaylodaType;
}

bool MJPEGRTPParser::SetPacketCallbaclk(RTPParser::MediaPacketCallbaclk callback)
{
    m_pMediaPacketCallbaclk = callback;
    return true;
}

void MJPEGRTPParser::GetSSRC(uint32_t& ssrc)
{
    ssrc = m_nSSRC;
}

int32_t MJPEGRTPParser::OutputMediaPacket()
{
    if (m_pMediaPacketCallbaclk == nullptr)
    {
        return 0;
    }

    uint8_t* data = nullptr;
    uint32_t size = 0;

    {
        std::lock_guard<std::mutex> lock(m_PacketBuffLock);
        uint8_t* pRawData = nullptr;
        uint32_t nRawSize = 0;
        m_PacketBuff.GetRawData(pRawData, nRawSize);

        if (nRawSize == 0 || pRawData == nullptr)
        {
            return 0;
        }

        data = (uint8_t*)malloc(nRawSize);
        if (data == nullptr)
        {
            Error("[%p][H264RTPParser::OutputMediaPacket] malloc data fail", this);
            return -1;
        }
        memcpy(data, pRawData, nRawSize);
        size = nRawSize;
    }

    std::shared_ptr<MediaPacket> pMediaPacket = std::make_shared<MediaPacket>();
    pMediaPacket->m_lDTS = m_nLastPackTime;
    pMediaPacket->m_lPTS = m_nLastPackTime;
    pMediaPacket->m_nFrameType = 0;
    pMediaPacket->m_nLength = size;
    pMediaPacket->m_pData = data;

    {
        //std::string path = "/usr/Video1/" + std::to_string(pMediaPacket->m_lDTS) + ".jpg";
        //FILE* fp = fopen(path.c_str(), "wb+");
        //fwrite(pMediaPacket->m_pData, 1, pMediaPacket->m_nLength, fp);
        //fclose(fp);
    }

    m_pMediaPacketCallbaclk(pMediaPacket);
    pMediaPacket = nullptr;

    return 0;
}

int32_t MJPEGRTPParser::RecvPacket(const std::shared_ptr<Packet>& packet)
{
    uint8_t* data = packet->m_pData;
    uint32_t size = packet->m_nLength;

    if (data == nullptr || size < 12)
    {
        Error("[%p][H264RTPParser::RecvPacket] data:%p or size:%d error", this, data, size);
        return -1;
    }

    if ((data[0] & 0xc0) != 0x80)
    {
        Error("[%p][H264RTPParser::RecvPacket] rtp version != 2", this);
        return -2;
    }

    bool bMarke = data[1] >> 7;
    m_nPaylodaType = data[1] & 0x7f;
    uint32_t time = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    m_nSSRC = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];

    uint32_t nNaluType = data[12] & 0x1f;
    if (time != m_nLastPackTime)
    {
        OutputMediaPacket();
        m_PacketBuff.ClearBuff();
        m_nLastPackTime = time;
    }

    if (nNaluType == 0x0a)
    {
        m_PacketBuff.Append(&data[13], size - 13);
        OutputMediaPacket();
        m_PacketBuff.ClearBuff();
    }
    else if (nNaluType == 28)
    {
        bool bIsStart = ((data[13] & 0x80) == 0x80);

        if (bIsStart || m_PacketBuff.GetDataSize() == 0)
        {
            uint8_t NRI = data[12] & 0x60;
            uint8_t type = data[13] & 0x1f;
            uint8_t nalutype = NRI | type;
            nalutype |= 0x80;
            m_PacketBuff.Append(&nalutype, 1);
        }

        m_PacketBuff.Append(&data[14], size - 14);
        if (bMarke)
        {
            OutputMediaPacket();
            m_PacketBuff.ClearBuff();
        }
    }
    else
    {
        Error("[%p][H264RTPParser::RecvPacket] not supported nalu type:%d ", this, nNaluType);
        return -3;
    }

    return 0;
}
