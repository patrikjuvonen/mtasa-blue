/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.cpp
 *  PURPOSE:     Recording voice
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceRecorder.h"

CVoiceRecorder::CVoiceRecorder(void)
{
    m_bEnabled = false;

    m_VoiceState = VOICESTATE_AWAITING_INPUT;
    m_SampleRate = SAMPLERATE_WIDEBAND;
    m_Channel = CHANNEL_MONO;

    m_pAudioStream = nullptr;

    m_pOpusEncoderState = nullptr;

    m_pOutgoingBuffer = nullptr;
    m_iOpusOutgoingFrameSampleCount = 0;
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bIsSendingVoiceData = false;

    m_ulTimeOfLastSend = 0;
}

CVoiceRecorder::~CVoiceRecorder(void)
{
    DeInit();
}

int CVoiceRecorder::BASSCallback(HRECORD handle, const void *buffer, DWORD length, void *user)
{
    // This assumes that BASSCallback will only be called when user is a valid CVoiceRecorder pointer
    CVoiceRecorder* pVoiceRecorder = static_cast<CVoiceRecorder*>(user);
    pVoiceRecorder->m_CS.Lock();

    g_pCore->GetConsole()->Print("BASSCallback received");

    if (pVoiceRecorder->IsEnabled())
    {
        g_pCore->GetConsole()->Print("Calling CVoiceRecorder::SendFrame...");
        pVoiceRecorder->SendFrame(buffer, length);
    }

    pVoiceRecorder->m_CS.Unlock();
    return 0;
}

void CVoiceRecorder::Init(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucComplexity, unsigned int uiBitrate)
{
    m_bEnabled = bEnabled;

    if (!bEnabled)            // If we aren't enabled, don't bother continuing
        return;

    m_CS.Lock();

    // Convert the sample rate we received from the server (0-4) into an actual sample rate
    m_SampleRate = convertServerSampleRate(uiServerSampleRate);
    m_ucComplexity = ucComplexity;

    // State is awaiting input
    m_VoiceState = VOICESTATE_AWAITING_INPUT;

    // Time of last send, this is used to limit sending
    m_ulTimeOfLastSend = 0;

    m_pOpusEncoderState = opus_encoder_create(m_SampleRate, m_Channel, OPUS_APPLICATION_VOIP, nullptr);

    // Initialize our outgoing buffer
    opus_encoder_ctl(m_pOpusEncoderState, OPUS_SET_COMPLEXITY(m_ucComplexity));
    opus_encoder_ctl(m_pOpusEncoderState, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    opus_encoder_ctl(m_pOpusEncoderState, OPUS_SET_DTX(1));

    int iBitrate = (int)uiBitrate;
    if (iBitrate)
    {
        opus_encoder_ctl(m_pOpusEncoderState, OPUS_SET_BITRATE(iBitrate));
        g_pCore->GetConsole()->Printf("set to %d", iBitrate);
    }
    else
        opus_encoder_ctl(m_pOpusEncoderState, OPUS_GET_BITRATE(&iBitrate));

    opus_encoder_ctl(m_pOpusEncoderState, OPUS_GET_EXPERT_FRAME_DURATION(&m_iOpusOutgoingFrameSampleCount));

    int iSamplingRate = (int)m_SampleRate;
    if (iSamplingRate)
        opus_encoder_ctl(m_pOpusEncoderState, OPUS_GET_SAMPLE_RATE(&iSamplingRate));

    // Initialize recording with the default device
    BASS_RecordInit(-1);

    // Start recording
    BASS_RecordStart(m_SampleRate, m_Channel, BASS_SAMPLE_FLOAT, &BASSCallback, this);

    m_pOutgoingBuffer = (char*)malloc(2048 * FRAME_OUTGOING_BUFFER_COUNT);
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;

    g_pCore->GetConsole()->Printf("Server Voice Chat Complexity [%i]; Sample Rate: [%iHz]; Bitrate [%ibps]", m_ucComplexity, iSamplingRate, iBitrate);

    m_CS.Unlock();
}

void CVoiceRecorder::DeInit(void)
{
    if (m_bEnabled)
    {
        m_bEnabled = false;

        BASS_RecordFree();

        // Assumes now that BASSCallback will not be called in this context
        m_CS.Lock();
        m_CS.Unlock();
        // Assumes now that BASSCallback is not executing in this context

        m_pAudioStream = nullptr;

        m_iOpusOutgoingFrameSampleCount = 0;

        opus_encoder_destroy(m_pOpusEncoderState);
        m_pOpusEncoderState = nullptr;

        free(m_pOutgoingBuffer);
        m_pOutgoingBuffer = nullptr;

        m_VoiceState = VOICESTATE_AWAITING_INPUT;
        m_SampleRate = SAMPLERATE_WIDEBAND;

        m_pAudioStream = nullptr;

        m_uiOutgoingReadIndex = 0;
        m_uiOutgoingWriteIndex = 0;
        m_bIsSendingVoiceData = false;
        m_ulTimeOfLastSend = 0;
    }
}

const int CVoiceRecorder::getOpusBandwidthFromSampleRate(void)
{
    switch (m_SampleRate)
    {
        case SAMPLERATE_NARROWBAND:
            return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_NARROWBAND);
        case SAMPLERATE_MEDIUMBAND:
            return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_MEDIUMBAND);
        case SAMPLERATE_WIDEBAND:
            return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_WIDEBAND);
        case SAMPLERATE_SUPERWIDEBAND:
            return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_SUPERWIDEBAND);
        case SAMPLERATE_FULLBAND:
            return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_FULLBAND);
    }
    return opus_encoder_ctl(m_pOpusEncoderState, OPUS_BANDWIDTH_WIDEBAND);
}

eSampleRate CVoiceRecorder::convertServerSampleRate(unsigned int uiServerSampleRate)
{
    switch (uiServerSampleRate)
    {
        case SERVERSAMPLERATE_NARROWBAND:
            return SAMPLERATE_NARROWBAND;
        case SERVERSAMPLERATE_MEDIUMBAND:
            return SAMPLERATE_MEDIUMBAND;
        case SERVERSAMPLERATE_WIDEBAND:
            return SAMPLERATE_WIDEBAND;
        case SERVERSAMPLERATE_SUPERWIDEBAND:
            return SAMPLERATE_SUPERWIDEBAND;
        case SERVERSAMPLERATE_FULLBAND:
            return SAMPLERATE_FULLBAND;
    }
    return SAMPLERATE_WIDEBAND;
}

void CVoiceRecorder::UpdatePTTState(unsigned int uiState)
{
    if (!m_bEnabled)
        return;

    m_CS.Lock();

    if (uiState == 1)
    {
        if (m_VoiceState == VOICESTATE_AWAITING_INPUT)
        {
            // Call event on the local player for starting to talk
            if (g_pClientGame->GetLocalPlayer())
            {
                m_CS.Unlock();
                CLuaArguments Arguments;
                bool          bEventTriggered = g_pClientGame->GetLocalPlayer()->CallEvent("onClientPlayerVoiceStart", Arguments, true);

                if (!bEventTriggered)
                {
                    return;
                }
                m_CS.Lock();
                m_VoiceState = VOICESTATE_RECORDING;
            }
        }
    }
    else if (uiState == 0)
    {
        if (m_VoiceState == VOICESTATE_RECORDING)
        {
            m_VoiceState = VOICESTATE_RECORDING_LAST_PACKET;

            // Call event on the local player for stopping to talk
            if (g_pClientGame->GetLocalPlayer())
            {
                m_CS.Unlock();
                CLuaArguments Arguments;
                g_pClientGame->GetLocalPlayer()->CallEvent("onClientPlayerVoiceStop", Arguments, true);
                m_CS.Lock();
            }
        }
    }
    m_CS.Unlock();
}

void CVoiceRecorder::DoPulse(void)
{
    m_CS.Lock();

    char* pInputBuffer;
    char  bufTempOutput[2048];

    // Only send every 100 ms
    if (CClientTime::GetTime() - m_ulTimeOfLastSend > 100 && m_VoiceState != VOICESTATE_AWAITING_INPUT)
    {
        m_bIsSendingVoiceData = false;
        unsigned int uiBytesAvailable = 0;

        if (m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex)
            uiBytesAvailable = m_uiOutgoingWriteIndex - m_uiOutgoingReadIndex;
        else
            uiBytesAvailable = m_uiOutgoingWriteIndex + (m_uiAudioBufferSize - m_uiOutgoingReadIndex);

        unsigned int uiOpusBlockSize = m_iOpusOutgoingFrameSampleCount * m_Channel * VOICE_SAMPLE_SIZE;

        unsigned int uiOpusFramesAvailable = uiBytesAvailable / uiOpusBlockSize;

        g_pCore->GetConsole()->Printf("CVoiceRecorder::DoPulse - tick: %d, \
m_iOpusOutgoingFrameSampleCount: %d, \
m_uiOutgoingWriteIndex: %d, \
m_uiOutgoingReadIndex: %d, \
m_uiAudioBufferSize: %d, \
m_Channel: %d, \
uiBytesAvailable: %d, \
uiOpusBlockSize: %d, \
uiOpusFramesAvailable: %d, \
VOICE_SAMPLE_SIZE: %d",
            CClientTime::GetTime(),
            m_iOpusOutgoingFrameSampleCount,
            m_uiOutgoingWriteIndex,
            m_uiOutgoingReadIndex,
            m_uiAudioBufferSize,
            m_Channel,
            uiBytesAvailable,
            uiOpusBlockSize,
            uiOpusFramesAvailable,
            VOICE_SAMPLE_SIZE
        );

        if (uiOpusFramesAvailable > 0)
        {
            unsigned char* ucEncodedOutput = new unsigned char[2048];

            while (uiOpusFramesAvailable-- > 0)
            {
                ucEncodedOutput = nullptr;
                
                // Does the input data wrap around the buffer? Copy it first then
                if (m_uiOutgoingReadIndex + uiOpusBlockSize >= m_uiAudioBufferSize)
                {
                    unsigned t;
                    for (t = 0; t < uiOpusBlockSize; t++)
                        bufTempOutput[t] = m_pOutgoingBuffer[t % m_uiAudioBufferSize];
                    pInputBuffer = bufTempOutput;
                }
                else
                    pInputBuffer = m_pOutgoingBuffer + m_uiOutgoingReadIndex;

                m_bIsSendingVoiceData = true;

                // Encode our audio stream with opus
                opus_int32 uiBytesWritten = opus_encode_float(m_pOpusEncoderState, (float*)pInputBuffer, uiOpusBlockSize, ucEncodedOutput, 2048);

                m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + uiOpusBlockSize) % m_uiAudioBufferSize;

                g_pClientGame->GetLocalPlayer()->GetVoice()->DecodeAndBuffer(bufTempOutput, uiBytesWritten);

                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
                if (pBitStream)
                {
                    CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

                    if (pLocalPlayer)
                    {
                        pBitStream->Write((unsigned short)uiBytesWritten);                  // size of buffer / voice data
                        pBitStream->Write((char*)bufTempOutput, uiBytesWritten);            // voice data

                        g_pNet->SendPacket(PACKET_ID_VOICE_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
                                           PACKET_ORDERING_VOICE);
                        g_pNet->DeallocateNetBitStream(pBitStream);
                    }
                }
            }
            free(ucEncodedOutput);

            m_ulTimeOfLastSend = CClientTime::GetTime();
        }
    }

    if (m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET)            // End of voice data (for events)
    {
        m_VoiceState = VOICESTATE_AWAITING_INPUT;

        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

            if (pLocalPlayer)
            {
                g_pNet->SendPacket(PACKET_ID_VOICE_END, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, PACKET_ORDERING_VOICE);
                g_pNet->DeallocateNetBitStream(pBitStream);
            }
        }
    }
    m_CS.Unlock();
}

// Called from other thread. Critical section is already locked.
void CVoiceRecorder::SendFrame(const void* inputBuffer, DWORD uiLength)
{
    g_pCore->GetConsole()->Print("Call to CVoiceRecorder::SendFrame received...");

    if (m_VoiceState != VOICESTATE_AWAITING_INPUT && m_bEnabled && inputBuffer)
    {
        g_pCore->GetConsole()->Print("CVoiceRecorder::SendFrame is now going to do some math...");
        
        unsigned int remainingBufferSize = 0;
        unsigned int uiTotalBufferSize = uiLength * FRAME_OUTGOING_BUFFER_COUNT;

        // Calculate how much of our buffer is remaining
        if (m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex)
            remainingBufferSize = uiTotalBufferSize - (m_uiOutgoingWriteIndex - m_uiOutgoingReadIndex);
        else
            remainingBufferSize = m_uiOutgoingReadIndex - m_uiOutgoingWriteIndex;

        // Copy from our input buffer to our outgoing buffer at write index
        memcpy(m_pOutgoingBuffer + m_uiOutgoingWriteIndex, inputBuffer, uiLength);

        g_pCore->GetConsole()->Printf("CVoiceRecorder::SendFrame [1] - tick: %d, \
remainingBufferSize: %d, \
uiLength: %d, \
FRAME_OUTGOING_BUFFER_COUNT: %d, \
uiTotalBufferSize: %d, \
remainingBufferSize: %d, \
m_uiOutgoingWriteIndex: %d, \
m_uiOutgoingReadIndex: %d",
CClientTime::GetTime(),
remainingBufferSize,
uiLength,
FRAME_OUTGOING_BUFFER_COUNT,
uiTotalBufferSize,
remainingBufferSize,
m_uiOutgoingWriteIndex,
m_uiOutgoingReadIndex
);

        // Re-align our write index
        m_uiOutgoingWriteIndex += uiLength;

        g_pCore->GetConsole()->Printf("CVoiceRecorder::SendFrame [2] - tick: %d, \
remainingBufferSize: %d, \
uiLength: %d, \
FRAME_OUTGOING_BUFFER_COUNT: %d, \
uiTotalBufferSize: %d, \
remainingBufferSize: %d, \
m_uiOutgoingWriteIndex: %d, \
m_uiOutgoingReadIndex: %d",
CClientTime::GetTime(),
remainingBufferSize,
uiLength,
FRAME_OUTGOING_BUFFER_COUNT,
uiTotalBufferSize,
remainingBufferSize,
m_uiOutgoingWriteIndex,
m_uiOutgoingReadIndex
);

        // If we have reached the end of the buffer, go back to the start
        if (m_uiOutgoingWriteIndex == uiTotalBufferSize)
            m_uiOutgoingWriteIndex = 0;

        g_pCore->GetConsole()->Printf("CVoiceRecorder::SendFrame [3] - tick: %d, \
remainingBufferSize: %d, \
uiLength: %d, \
FRAME_OUTGOING_BUFFER_COUNT: %d, \
uiTotalBufferSize: %d, \
remainingBufferSize: %d, \
m_uiOutgoingWriteIndex: %d, \
m_uiOutgoingReadIndex: %d",
CClientTime::GetTime(),
remainingBufferSize,
uiLength,
FRAME_OUTGOING_BUFFER_COUNT,
uiTotalBufferSize,
remainingBufferSize,
m_uiOutgoingWriteIndex,
m_uiOutgoingReadIndex
);

        // Wrap around the buffer?
        if (uiLength >= remainingBufferSize)
            m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + m_iOpusOutgoingFrameSampleCount * m_Channel * VOICE_SAMPLE_SIZE) % uiTotalBufferSize;

        g_pCore->GetConsole()->Printf("CVoiceRecorder::SendFrame [4] - tick: %d, \
remainingBufferSize: %d, \
uiLength: %d, \
FRAME_OUTGOING_BUFFER_COUNT: %d, \
uiTotalBufferSize: %d, \
remainingBufferSize: %d, \
m_uiOutgoingWriteIndex: %d, \
m_uiOutgoingReadIndex: %d",
CClientTime::GetTime(),
remainingBufferSize,
uiLength,
FRAME_OUTGOING_BUFFER_COUNT,
uiTotalBufferSize,
remainingBufferSize,
m_uiOutgoingWriteIndex,
m_uiOutgoingReadIndex
);
    }
}
