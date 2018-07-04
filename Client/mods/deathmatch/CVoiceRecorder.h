/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
 *  PURPOSE:     Header for voice class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CVOICE_H
#define __CVOICE_H

#define VOICE_SAMPLE_SIZE 2

#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

#include <libopus/include/opus.h>
#include <bass/bass.h>

enum eVoiceState
{
    VOICESTATE_AWAITING_INPUT = 0,
    VOICESTATE_RECORDING,
    VOICESTATE_RECORDING_LAST_PACKET,
};

enum eSampleRate
{
    SAMPLERATE_NARROWBAND = 8000,
    SAMPLERATE_MEDIUMBAND = 12000,
    SAMPLERATE_WIDEBAND = 16000,
    SAMPLERATE_SUPERWIDEBAND = 24000,
    SAMPLERATE_FULLBAND = 48000
};

enum eServerSampleRate
{
    SERVERSAMPLERATE_NARROWBAND = 0,
    SERVERSAMPLERATE_MEDIUMBAND,
    SERVERSAMPLERATE_WIDEBAND,
    SERVERSAMPLERATE_SUPERWIDEBAND,
    SERVERSAMPLERATE_FULLBAND
};

enum eChannel
{
    CHANNEL_MONO = 1,
    CHANNEL_STEREO = 2
};

class CVoiceRecorder
{
public:
    CVoiceRecorder(void);
    ~CVoiceRecorder(void);

    void Init(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucComplexity, unsigned int uiBitrate);

    bool IsEnabled(void) { return m_bEnabled; }

    void DoPulse(void);

    void UpdatePTTState(unsigned int uiState);

    unsigned int  GetSampleRate(void) { return m_SampleRate; }
    unsigned char GetComputationalComplexity(void) { return m_ucComplexity; }

    const int     getOpusBandwidthFromSampleRate(void);

private:
    void DeInit(void);

    void SendFrame(const void* inputBuffer, DWORD length);

    static int __stdcall BASSCallback(HRECORD handle, const void *buffer, DWORD length, void *user);

    bool        m_bEnabled;
    eVoiceState m_VoiceState;

    HRECORD m_pAudioStream;

    OpusEncoder* m_pOpusEncoderState;

    char*        m_pOutgoingBuffer;
    int          m_iOpusOutgoingFrameSampleCount;
    unsigned int m_uiOutgoingReadIndex;
    unsigned int m_uiOutgoingWriteIndex;
    bool         m_bIsSendingVoiceData;

    unsigned long m_ulTimeOfLastSend;

    unsigned int m_uiAudioBufferSize = 2048 * FRAME_OUTGOING_BUFFER_COUNT;
    eSampleRate  convertServerSampleRate(unsigned int uiServerSampleRate);

    eSampleRate   m_SampleRate;
    unsigned char m_ucComplexity;

    eChannel m_Channel;

    std::list<SString> m_EventQueue;
    CCriticalSection   m_CS;
};
#endif
