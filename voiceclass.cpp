///////////////////////////////////////////////////////////////////////////////////
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
//
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <atlbase.h>		// ATL
#include <sapi.h>           // SAPI includes
#include <sphelper.h>

#include <string>
///////////////////////////////////////////////////////////////////////////////////
#include "voiceclass.h"
///////////////////////////////////////////////////////////////////////////////////
// Output formats
const SPSTREAMFORMAT g_aOutputFormat[NUM_OUTPUTFORMATS] = {SPSF_8kHz8BitMono,     
                                                SPSF_8kHz8BitStereo,
                                                SPSF_8kHz16BitMono,
                                                SPSF_8kHz16BitStereo,   
                                                SPSF_11kHz8BitMono,
                                                SPSF_11kHz8BitStereo,   
                                                SPSF_11kHz16BitMono,
                                                SPSF_11kHz16BitStereo,    
                                                SPSF_12kHz8BitMono,
                                                SPSF_12kHz8BitStereo,    
                                                SPSF_12kHz16BitMono,
                                                SPSF_12kHz16BitStereo,
                                                SPSF_16kHz8BitMono,   
                                                SPSF_16kHz8BitStereo, 
                                                SPSF_16kHz16BitMono,
                                                SPSF_16kHz16BitStereo,
                                                SPSF_22kHz8BitMono,
                                                SPSF_22kHz8BitStereo,
                                                SPSF_22kHz16BitMono,
                                                SPSF_22kHz16BitStereo,
                                                SPSF_24kHz8BitMono,
                                                SPSF_24kHz8BitStereo,
                                                SPSF_24kHz16BitMono,
                                                SPSF_24kHz16BitStereo,
                                                SPSF_32kHz8BitMono,
                                                SPSF_32kHz8BitStereo,
                                                SPSF_32kHz16BitMono,
                                                SPSF_32kHz16BitStereo,
                                                SPSF_44kHz8BitMono,
                                                SPSF_44kHz8BitStereo,
                                                SPSF_44kHz16BitMono,
                                                SPSF_44kHz16BitStereo,
                                                SPSF_48kHz8BitMono,
                                                SPSF_48kHz8BitStereo,
                                                SPSF_48kHz16BitMono,
                                                SPSF_48kHz16BitStereo};

TCHAR* g_aszOutputFormat[NUM_OUTPUTFORMATS] = {_T("8kHz 8 Bit Mono"),     
                                                _T("8kHz 8 Bit Stereo"),
                                                _T("8kHz 16 Bit Mono"),
                                                _T("8kHz 16 Bit Stereo"),   
                                                _T("11kHz 8 Bit Mono"),
                                                _T("11kHz 8 Bit Stereo"),   
                                                _T("11kHz 16 Bit Mono"),
                                                _T("11kHz 16 Bit Stereo"),    
                                                _T("12kHz 8 Bit Mono"),
                                                _T("12kHz 8 Bit Stereo"),    
                                                _T("12kHz 16 Bit Mono"),
                                                _T("12kHz 16 Bit Stereo"),
                                                _T("16kHz 8 Bit Mono"),   
                                                _T("16kHz 8 Bit Stereo"), 
                                                _T("16kHz 16 Bit Mono"),
                                                _T("16kHz 16 Bit Stereo"),
                                                _T("22kHz 8 Bit Mono"),
                                                _T("22kHz 8 Bit Stereo"),
                                                _T("22kHz 16 Bit Mono"),
                                                _T("22kHz 16 Bit Stereo"),
                                                _T("24kHz 8 Bit Mono"),
                                                _T("24kHz 8 Bit Stereo"),
                                                _T("24kHz 16 Bit Mono"),
                                                _T("24kHz 16 Bit Stereo"),
                                                _T("32kHz 8 Bit Mono"),
                                                _T("32kHz 8 Bit Stereo"),
                                                _T("32kHz 16 Bit Mono"),
                                                _T("32kHz 16 Bit Stereo"),
                                                _T("44kHz 8 Bit Mono"),
                                                _T("44kHz 8 Bit Stereo"),
                                                _T("44kHz 16 Bit Mono"),
                                                _T("44kHz 16 Bit Stereo"),
                                                _T("48kHz 8 Bit Mono"),
                                                _T("48kHz 8 Bit Stereo"),
                                                _T("48kHz 16 Bit Mono"),
                                                _T("48kHz 16 Bit Stereo")};
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
VoiceClass::VoiceClass()
{
    m_bPause = false; // pause audio?
    m_bStop  = true;  // stop audio?
    m_DefaultRate=0;
    m_DefaultVolume=0;
    m_DefaultFormatIndex=0;
    m_DefaultVoiceIndex=0;
}
///////////////////////////////////////////////////////////////////////////////////
void TextCheck(const char *aStr)
{
    int i=0;
}
///////////////////////////////////////////////////////////////////////////////////
bool VoiceClass::OnInit(HWND hWnd)
{
    HRESULT hr = S_OK;

    if ( !m_cpVoice )
    {
        hr = E_FAIL;
    }

    if ( SUCCEEDED( hr ) )
    {
        SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOOUT,&m_cpOutAudio  );
    }

	// Set the notification message for the voice
	if ( SUCCEEDED( hr ) )
	{
		m_cpVoice->SetNotifyWindowMessage( hWnd, WM_TalkingNoteAppCUSTOMEVENT, 0, 0 );
	}

    // We're interested in all TTS events
    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->SetInterest( SPFEI_ALL_TTS_EVENTS, SPFEI_ALL_TTS_EVENTS );
    }

    // Get defult rate and volume
    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->GetRate( &m_DefaultRate );
    }
    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->GetVolume( &m_DefaultVolume );
    }

    // If any SAPI initialization failed, shut down!
    if( FAILED( hr ) )
    {
        MessageBox( NULL, _T("Error initializing speech objects. Shutting down."), _T("Error"), MB_OK );
		return(FALSE);
    }

    CComPtr<ISpStreamFormat> cpStream;
    HRESULT hrOutputStream = m_cpVoice->GetOutputStream(&cpStream);
    
    if (hrOutputStream == S_OK)
    {
        CSpStreamFormat Fmt;
        hr = Fmt.AssignFormat(cpStream);
        if (SUCCEEDED(hr))
        {
            SPSTREAMFORMAT eFmt = Fmt.ComputeFormatEnum();
            for(int i=0; i<NUM_OUTPUTFORMATS; i++ )
            {
                if( g_aOutputFormat[i] == eFmt )
                {
                    m_DefaultFormatIndex=i;
                }
            }
        }
    }   

    // find current voice
    CComPtr<IEnumSpObjectTokens> cpEnum;

    //Enumerate the available voices
    if(SUCCEEDED(hr))
        hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

    ULONG ulCount = 0;
    //Get the number of voices
    if(SUCCEEDED(hr))
        hr = cpEnum->GetCount(&ulCount);

    if (SUCCEEDED(hr))
    {
        CComPtr<ISpObjectToken> cpCurVoiceToken;
        hrOutputStream = m_cpVoice->GetVoice(&cpCurVoiceToken);

        CSpDynamicString CurDesc;
        hr = SpGetDescription(cpCurVoiceToken, &CurDesc);
        char *aText1 = CurDesc.CopyToChar();

        if (aText1)
        {
            ULONG i;
            CComPtr<ISpObjectToken> cpVoiceToken;
            for (i=0;(i<ulCount && SUCCEEDED(hr));i++)
            {
                cpVoiceToken.Release();
                if(SUCCEEDED(hr))
                    hr = cpEnum->Next( 1, &cpVoiceToken, NULL );

                if(SUCCEEDED(hr))
                {
                    CSpDynamicString dstrDesc;
                    hr = SpGetDescription(cpVoiceToken, &dstrDesc);

                    char *aText = dstrDesc.CopyToChar();
                    if (aText)
                    {
                        if (strcmp(aText1,aText)==0)
                        {
                            m_DefaultVoiceIndex=i;
                        }
                    }
                }
            }
        }
    }
    return(TRUE);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::InitSapi()
{
    HRESULT  hr;
    
    hr = m_cpVoice.CoCreateInstance( CLSID_SpVoice );

    return hr;
}
///////////////////////////////////////////////////////////////////////////////////
//
// This function is called during initialization and whenever the 
// selection for the voice  changes. 
// If the new voice is different from the one that's currently 
// selected, it first stops any synthesis that is going on and
// sets the new voice on the global voice object. 
//
HRESULT VoiceClass::VoiceChange(ISpObjectToken* pToken)
{
    HRESULT         hr = S_OK;
    GUID*           pguidAudioFormat = NULL;
    int             iFormat = 0;
    
    //Determine if it is the current voice
    CComPtr<ISpObjectToken> pOldToken;
    hr = m_cpVoice->GetVoice( &pOldToken );

    if (SUCCEEDED(hr))
    {
        if (pOldToken != pToken)
        {        
            // Stop speaking. This is not necesary, for the next call to work,
            // but just to show that we are changing voices.
            hr = m_cpVoice->Speak( NULL, SPF_PURGEBEFORESPEAK, 0);
            // And set the new voice on the global voice object
            if (SUCCEEDED (hr) )
            {
                hr = m_cpVoice->SetVoice( pToken );
            }
        }
    }

    return hr;
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::Speak(const char* TheText)
{
    HRESULT hr = S_FALSE;
    if(!m_bPause) // if not paused output string
    {
        if (TheText)
        {
            size_t TheLen=strlen(TheText);
            if (TheLen>0)
            {
                wchar_t *TheWideText = new wchar_t[TheLen+1];
                if (TheWideText)
                {
                    memset(TheWideText,0,sizeof(wchar_t)*(TheLen+1));
                    mbstowcs(TheWideText,TheText,TheLen);
                    hr=Speak(TheWideText);
                    delete [] TheWideText;
                }
            }
        }
    }
    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::Speak(const std::string &TheText)
{
    HRESULT hr = S_FALSE;
    if(!m_bPause) // if not paused output string
    {
        if (TheText.length()>0)
        {
            int TheLen=TheText.length();
            if (TheLen>0)
            {
                wchar_t *TheWideText = new wchar_t[TheLen+1];
                if (TheWideText)
                {
                    memset(TheWideText,0,sizeof(wchar_t)*(TheLen+1));
                    mbstowcs(TheWideText,TheText.c_str(),TheLen);
                    hr=Speak(TheWideText);
                    delete [] TheWideText;
                }
            }
        }
    }
    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::Speak(const wchar_t *TheText)
{
    HRESULT hr = S_FALSE;
    if(!m_bPause) // if not paused output string
    {
        if (TheText)
        {
            hr = m_cpVoice->Speak(TheText, SPF_ASYNC|SPF_IS_NOT_XML,0);

            m_bStop = false;
        }
    }
    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::Stop()
{
    HRESULT hr = m_cpVoice->Speak( NULL, SPF_PURGEBEFORESPEAK, 0 );
    m_bPause = false;
    m_bStop = true;   
    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::Pause()
{
    HRESULT hr = S_FALSE;
    if( !m_bStop )
    {
        if( !m_bPause )
        {
            // Pause the voice...
            hr=m_cpVoice->Pause();
            m_bPause = true;
        }
        else
        {
            // start voice
            m_cpVoice->Resume();
            m_bPause = false;
        }
    }
    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
HRESULT VoiceClass::SetAudio(SPSTREAMFORMAT eFmt)
{
    HRESULT hr = S_FALSE;
    CSpStreamFormat Fmt;
    Fmt.AssignFormat(eFmt);

	if ( m_cpOutAudio )
	{
		hr = m_cpOutAudio->SetFormat( Fmt.FormatId(), Fmt.WaveFormatExPtr() );
	}
	else
	{
		hr = E_FAIL;
	}

    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->SetOutput( m_cpOutAudio, FALSE );
    }

    return (hr);
}
///////////////////////////////////////////////////////////////////////////////////
USHORT VoiceClass::GetVolume()
{
    USHORT aVol= (USHORT) 0;
    m_cpVoice->GetVolume(&aVol);
    return (aVol);
}
///////////////////////////////////////////////////////////////////////////////////
long  VoiceClass::GetRate()
{
    long aRate=(long) 0;
    m_cpVoice->GetRate(&aRate);
    return (aRate);
}
///////////////////////////////////////////////////////////////////////////////////
void VoiceClass::SetVolume(USHORT aVal)
{ 
    m_cpVoice->SetVolume(aVal);
}
///////////////////////////////////////////////////////////////////////////////////
void VoiceClass::SetRate(long aVal)
{ 
    m_cpVoice->SetRate(aVal);
}
///////////////////////////////////////////////////////////////////////////////////
bool VoiceClass::TestVoice()
{
    if (m_cpVoice)
    {
        if (IsProcessing())
        {
            Stop();
        }
        CComPtr<ISpStreamFormat> cpStream;
        HRESULT hr = m_cpVoice->GetOutputStream(&cpStream);

        CComPtr<ISpObjectToken> cpCurVoiceToken;
        if(SUCCEEDED(hr))
            hr = m_cpVoice->GetVoice(&cpCurVoiceToken);

        CSpDynamicString CurDesc;
        char *aText1=NULL;
        if(SUCCEEDED(hr))
        {
            hr = SpGetDescription(cpCurVoiceToken, &CurDesc);
            aText1 = CurDesc.CopyToChar();
        }

        if (aText1 && strlen(aText1)>0 && strlen(aText1)<100)
        {
            char TestStringIs[150];
            _snprintf(TestStringIs,149,"The current voice is %1s",aText1);
            Speak(TestStringIs);
        }
        else
        {
            Speak("This is a test");
        }
        return (true);
    }
    return (false);
}
///////////////////////////////////////////////////////////////////////////////////
bool VoiceClass::SetToDefaults()
{
    if (m_cpVoice)
    {
        if (IsProcessing())
            Stop(); // need to stop voice

        // set voice to default settings
        HRESULT hr = S_OK;
        CComPtr<IEnumSpObjectTokens> cpEnum;

        //Enumerate the available voices
        if(SUCCEEDED(hr))
            hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

        ULONG ulCount = 0;
        //Get the number of voices
        if(SUCCEEDED(hr))
            hr = cpEnum->GetCount(&ulCount);

        if (SUCCEEDED(hr) && m_DefaultVoiceIndex<ulCount)
        {
            CComPtr<ISpObjectToken> cpVoiceToken;
            ULONG i;
            for (i=0;(i<ulCount && SUCCEEDED(hr));i++)
            {
                cpVoiceToken.Release();
                if(SUCCEEDED(hr))
                    hr = cpEnum->Next( 1, &cpVoiceToken, NULL );

                if(SUCCEEDED(hr) && i == m_DefaultVoiceIndex)
                {
                    hr = m_cpVoice->Speak( NULL, SPF_PURGEBEFORESPEAK, 0);
                    if(SUCCEEDED(hr))
                    {
                        hr = m_cpVoice->SetVoice(cpVoiceToken);
                    }
                }
            }
        }
        else
        {
            return (false);
        }

        SetVolume(m_DefaultVolume);
        SetRate(m_DefaultRate);

        if (m_cpOutAudio && m_DefaultFormatIndex<NUM_OUTPUTFORMATS)
        {
            SPSTREAMFORMAT eFmt = g_aOutputFormat[m_DefaultFormatIndex];
            CSpStreamFormat Fmt;
            Fmt.AssignFormat(eFmt);
    
            hr = m_cpOutAudio->SetFormat( Fmt.FormatId(), Fmt.WaveFormatExPtr() );
    
            if( SUCCEEDED( hr ) )
            {
                hr = m_cpVoice->SetOutput( m_cpOutAudio, FALSE );
            }
        }
        return (true);
    }
    return (false);
}