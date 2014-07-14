/////////////////////////////////////////////////////////////////////////
//
//  The class used for controlling the speech output
//  
//  Author P.J.Lawrence Feb 2007 Email P.J.Lawrence@gre.ac.uk
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose
// and to alter it and redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////
#ifndef VoiceClass_I
#define VoiceClass_I
/////////////////////////////////////////////////////////////////////////
#define WM_TalkingNoteAppCUSTOMEVENT       WM_APP          // Window message used for systhesis events
#define NUM_OUTPUTFORMATS       36
extern const SPSTREAMFORMAT g_aOutputFormat[NUM_OUTPUTFORMATS];
extern TCHAR* g_aszOutputFormat[NUM_OUTPUTFORMATS];
/////////////////////////////////////////////////////////////////////////
class VoiceClass
{
private:
    CComPtr<ISpVoice>   m_cpVoice;
    CComPtr<ISpAudio>   m_cpOutAudio;
    bool                m_bPause;
    bool                m_bStop;
    USHORT              m_DefaultVolume;
    long                m_DefaultRate;
    int                 m_DefaultFormatIndex;
    int                 m_DefaultVoiceIndex;

public:
    const CComPtr<ISpVoice> GetcpVoice() { return(m_cpVoice); }
    const CComPtr<ISpAudio> GetcpOutAudio() { return(m_cpOutAudio); }

    
    USHORT GetDefaultVolume() { return(m_DefaultVolume); }
    long   GetDefaultRate() { return(m_DefaultRate); }
    int    GetDefaultFormatIndex() { return(m_DefaultFormatIndex); }
    int    GetDefaultVoiceIndex() { return(m_DefaultVoiceIndex); }
    bool   SetToDefaults();

    USHORT  GetVolume();
    long  GetRate();

    void SetVolume(USHORT aVal);
    void SetRate(long aVal);
    void SetStopFlag(bool aStatus) {  m_bStop=aStatus; }
    bool TestVoice();

    VoiceClass();
    HRESULT InitSapi();
    bool OnInit(HWND hWnd);
    HRESULT VoiceChange(ISpObjectToken* pToken);

    HRESULT Speak(const char*TheText);
    HRESULT Speak(const wchar_t *TheText);
    HRESULT Speak(const std::string &TheText);
    HRESULT Stop();
    HRESULT Pause();

    BOOL IsProcessing() { return (!m_bStop); }
    HRESULT SetAudio(SPSTREAMFORMAT eFmt);
};
#endif