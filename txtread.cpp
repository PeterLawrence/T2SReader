//////////////////////////////////////////////////////////////////////////////////
//
//  The main class for the text reader application
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
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include "resource.h"
///////////////////////////////////////////////////////////////////////////////////
#include <atlbase.h>		// ATL
#include <sapi.h>           // SAPI includes
#include <sphelper.h>
#include <spuihelp.h>
///////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "dlgtxtctrl.h"
#include "voiceclass.h"
#include "txtreader.h"
//////////////////////////////////////////////////////////////////////////////////
WinTextReader_Class::WinTextReader_Class()
{
    m_hInst=NULL;
    m_VoiceClass=NULL;
    m_nIDHotKey=0;
    m_MainControlDlg=NULL;
    m_SystemWindow=NULL;
    
    m_TxtWindow=NULL;
    
    m_CurrentTxt.assign("");
    m_AllTxt.assign("");
    
    m_AboutBox=NULL;
    m_HotKeyBox=NULL;
    
    LoadedUsersSetting=false;
    m_CurrentVoiceIndex=0;
    m_CurrentFormatIndex=18;
    m_UserVolume = 100;
    m_UserRate = 3;

    m_HotKeyCode = 'S'; // was T changed to S for windows 7
    m_HotKeyModifier = MOD_WIN;
    m_KeepText=true;

    LoadInitialSettings();
}
//////////////////////////////////////////////////////////////////////////////////
WinTextReader_Class::~WinTextReader_Class()
{
    SaveInitialSettings();
    ShutDownWindowInterface();
}
//////////////////////////////////////////////////////////////////////////////////
void WinTextReader_Class::SetMainControlDlg(HWND aWin)
{ 
    m_MainControlDlg = aWin;
    if (m_MainControlDlg==NULL)
    {
        if (m_TxtWindow)
        {
            delete m_TxtWindow;
            m_TxtWindow=NULL;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////
//
//  Store the current settings for the voice control
//
BOOL WinTextReader_Class::UpdateVoiceSettings()
{
    if (m_VoiceClass)
    {
        m_UserVolume=m_VoiceClass->GetVolume();
        m_UserRate = m_VoiceClass->GetRate();

        CComPtr<ISpStreamFormat> cpStream;
        HRESULT hrOutputStream = m_VoiceClass->GetcpVoice()->GetOutputStream(&cpStream);
        
        if (hrOutputStream == S_OK)
        {
            CSpStreamFormat Fmt;
            HRESULT  hr = Fmt.AssignFormat(cpStream);
            if (SUCCEEDED(hr))
            {
                SPSTREAMFORMAT eFmt = Fmt.ComputeFormatEnum();
                for(int i=0; i<NUM_OUTPUTFORMATS; i++ )
                {
                    if( g_aOutputFormat[i] == eFmt )
                    {
                        m_CurrentFormatIndex=i;
                    }
                }
            }
        }
        return (TRUE);
    }
    return (FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::SaveInitialSettings()
{
    LONG            lRetVal;
    HKEY            hkResult;
    lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                        _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &hkResult, NULL );

    if( lRetVal == ERROR_SUCCESS )
    {
        RegSetValueEx( hkResult, _T("HotKey"), NULL, REG_DWORD, (PBYTE) &m_HotKeyCode, sizeof(m_HotKeyCode) );
        RegSetValueEx( hkResult, _T("HotKeyModifier"), NULL, REG_DWORD, (PBYTE) &m_HotKeyModifier, sizeof(m_HotKeyModifier) );
        RegSetValueEx( hkResult, _T("KeepText"), NULL, REG_DWORD, (PBYTE) &m_KeepText, sizeof(m_KeepText) );
        return(TRUE);
    }
    return(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::LoadInitialSettings()
{
    LONG            lRetVal;
    HKEY            hkResult;
    DWORD size;

    LoadedUsersSetting=false;
    lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                        _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &hkResult, NULL );
    if( lRetVal == ERROR_SUCCESS )
    {
        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_HotKeyCode);
            lRetVal = RegQueryValueEx( hkResult, _T("HotKey"), NULL, NULL, (PBYTE) &m_HotKeyCode, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_HotKeyModifier);
            lRetVal = RegQueryValueEx( hkResult, _T("HotKeyModifier"), NULL, NULL, (PBYTE) &m_HotKeyModifier, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_KeepText);
            lRetVal = RegQueryValueEx( hkResult, _T("KeepText"), NULL, NULL, (PBYTE) &m_KeepText, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {
            return (TRUE);
        }
    }
    return (FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::SaveSpeechSettings()
{
    LONG            lRetVal;
    HKEY            hkResult;
    lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                        _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &hkResult, NULL );

    if( lRetVal == ERROR_SUCCESS )
    {
        RegSetValueEx( hkResult, _T("Voice"), NULL, REG_DWORD, (PBYTE) &m_CurrentVoiceIndex, sizeof(m_CurrentVoiceIndex) );
        RegSetValueEx( hkResult, _T("VoiceFormat"), NULL, REG_DWORD, (PBYTE) &m_CurrentFormatIndex, sizeof(m_CurrentFormatIndex) );

        // Ensure voice settings are up to date
        UpdateVoiceSettings();

        RegSetValueEx( hkResult, _T("Volume"), NULL, REG_DWORD, (PBYTE) &m_UserVolume, sizeof(m_UserVolume) );
        RegSetValueEx( hkResult, _T("Rate"), NULL, REG_DWORD, (PBYTE) &m_UserRate, sizeof(m_UserRate) );

        return(TRUE);
    }
    return(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::LoadSpeechSettings()
{
    LONG            lRetVal;
    HKEY            hkResult;
    DWORD size;

    LoadedUsersSetting=false;
    lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                        _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &hkResult, NULL );
    if( lRetVal == ERROR_SUCCESS )
    {
        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_CurrentVoiceIndex);
            lRetVal = RegQueryValueEx( hkResult, _T("Voice"), NULL, NULL, (PBYTE) &m_CurrentVoiceIndex, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_CurrentFormatIndex);
            lRetVal = RegQueryValueEx( hkResult, _T("VoiceFormat"), NULL, NULL, (PBYTE) &m_CurrentFormatIndex, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_UserVolume);
            lRetVal = RegQueryValueEx( hkResult, _T("Volume"), NULL, NULL, (PBYTE) &m_UserVolume, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {           
            size = sizeof(m_UserRate);
            lRetVal = RegQueryValueEx( hkResult, _T("Rate"), NULL, NULL, (PBYTE) &m_UserRate, &size );
        }

        if (lRetVal == ERROR_SUCCESS)
        {
            LoadedUsersSetting=true;
            return (TRUE);
        }
    }
    return (FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::SaveDialogueCurrentStatus()
{
    if (m_MainControlDlg)
    {
        RECT TheRect;
        if (IsIconic(m_MainControlDlg) || IsZoomed(m_MainControlDlg) || GetWindowRect(m_MainControlDlg,&TheRect)==0)
        {
            return (FALSE);
        }

        LONG            lRetVal;
        HKEY            hkResult;
        lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                            _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                            KEY_ALL_ACCESS, NULL, &hkResult, NULL );

        if( lRetVal == ERROR_SUCCESS )
        {
            TheRect.right-=TheRect.left;
            TheRect.bottom-=TheRect.top;
            RegSetValueEx( hkResult, _T("WinLeftPos"),NULL, REG_DWORD, (PBYTE) &TheRect.left, sizeof(TheRect.left) );
            RegSetValueEx( hkResult, _T("WinTopPos"), NULL, REG_DWORD, (PBYTE) &TheRect.top, sizeof(TheRect.top) );
            RegSetValueEx( hkResult, _T("WinWidth"),  NULL, REG_DWORD, (PBYTE) &TheRect.right, sizeof(TheRect.right) );
            RegSetValueEx( hkResult, _T("WinHeight"), NULL, REG_DWORD, (PBYTE) &TheRect.bottom, sizeof(TheRect.bottom) );

            return(TRUE);
        }
    }
    return(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::LoadDialogueCurrentStatus(bool RetoreWindowStatus)
{
    if (m_MainControlDlg)
    {
        LONG            lRetVal;
        HKEY            hkResult;
        DWORD size;

        lRetVal = RegCreateKeyEx( HKEY_CURRENT_USER, 
                            _T("SOFTWARE\\T2SReader"), 0, NULL, 0,
                            KEY_ALL_ACCESS, NULL, &hkResult, NULL );
        if( lRetVal == ERROR_SUCCESS )
        {
            RECT TheRect;
            GetWindowRect(m_MainControlDlg,&TheRect);
            TheRect.right-=TheRect.left;
            TheRect.bottom-=TheRect.top;

            size = sizeof(TheRect.left);
            lRetVal = RegQueryValueEx( hkResult, _T("WinLeftPos"),NULL, NULL, (PBYTE) &TheRect.left, &size );
            if (lRetVal == ERROR_SUCCESS)
                lRetVal = RegQueryValueEx( hkResult, _T("WinTopPos"), NULL, NULL, (PBYTE) &TheRect.top,  &size);
            if (lRetVal == ERROR_SUCCESS)
                lRetVal = RegQueryValueEx( hkResult, _T("WinWidth"),  NULL, NULL, (PBYTE) &TheRect.right, &size );
            if (lRetVal == ERROR_SUCCESS)
                lRetVal = RegQueryValueEx( hkResult, _T("WinHeight"), NULL, NULL, (PBYTE) &TheRect.bottom, &size );

            if (TheRect.left<0)
            {
                TheRect.left=0;
            }
            if (TheRect.top<0)
            {
                TheRect.top=0;
            }
            if (TheRect.right<DlgMinXVal)
            {
                TheRect.right=DlgMinXVal;
            }
            if (TheRect.bottom<DlgMinYVal)
            {
                TheRect.bottom=DlgMinYVal;
            }
            if (RetoreWindowStatus && lRetVal == ERROR_SUCCESS)
            {
                SetWindowPos(m_MainControlDlg,HWND_TOP,TheRect.left,TheRect.top,TheRect.right,TheRect.bottom,SWP_SHOWWINDOW);
            }

            if (lRetVal == ERROR_SUCCESS)
                return (TRUE);
        }
    }
    return (FALSE);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::InitTxtWindow()
{
    if (m_TxtWindow)
    {
        m_TxtWindow->Clear();
        m_TxtWindow->AppendText(m_AllTxt);
        return (true);
    }
    return (false);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::ClearHistory()
{
    m_AllTxt.erase();
    if (m_TxtWindow)
    {
        m_TxtWindow->Clear();
    }
    return (true);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::ShutDownWindowInterface()
{
    if (m_nIDHotKey>0)
    {
        if (UnregisterHotKey(m_SystemWindow, m_nIDHotKey)==0)
        {
            DWORD ErrCode=GetLastError();
            m_nIDHotKey=0;
        }
        else
        {   
            m_nIDHotKey=0;
        }
    }
    if (m_VoiceClass)
    {
        UpdateVoiceSettings();
        delete m_VoiceClass;
        m_VoiceClass=NULL;
    }
    if (m_TxtWindow)
    {
        delete m_TxtWindow;
        m_TxtWindow=NULL;
    }
    return (true);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::StartUpWindowInterface(HWND aWnd)
{
    m_SystemWindow=aWnd;
    if (m_nIDHotKey==0)
    {
        m_nIDHotKey = GlobalAddAtom("T2SReader");
        if (RegisterHotKey(m_SystemWindow, m_nIDHotKey, m_HotKeyModifier,m_HotKeyCode)==0)
        {
            // problem registering hot key
            m_nIDHotKey=0;

            MessageBox( NULL, 
                _T("Failed to register hot key.\r\nPlease select a new key.\r\nThe application could be already running or the currently\r\nset Hotkey could be in use by another program"), 
                _T("Error"), MB_OK );

            DialogBox(m_hInst, (LPCTSTR)IDD_HOTKEYDIALOG, m_SystemWindow, (DLGPROC)HotKeyDlg);

            if (m_nIDHotKey==0)
            {
                MessageBox( NULL, 
                    _T("Failed to register hot key. Shutting down."), 
                    _T("Error"), MB_OK );
                return (false);
            }
        }
    }
    if (!m_VoiceClass)
    {
        HRESULT hr = S_FALSE;
        m_VoiceClass = new VoiceClass();
        if (m_VoiceClass)
        {
            hr = m_VoiceClass->InitSapi();
        }
        if( SUCCEEDED( hr ) )
        {
            m_VoiceClass->OnInit(m_SystemWindow);
        }
        else
        {
            // Error - shut down
            MessageBox( NULL, 
                _T("Error initializing failed. Shutting down."), 
                _T("Error"), MB_OK );
            return (FALSE);
        } 
        if ( SUCCEEDED( hr ) )
        {
            m_VoiceClass->GetcpVoice()->SetNotifyWindowMessage(m_SystemWindow, WM_TalkingNoteAppCUSTOMEVENT, 0, 0 );
        }
    }

    if (m_VoiceClass)
    {
        if (LoadedUsersSetting)
        {
            SetVoiceToInitSettings();
        }
        else
        {
            m_CurrentVoiceIndex  = m_VoiceClass->GetDefaultVoiceIndex();
            m_CurrentFormatIndex = m_VoiceClass->GetDefaultFormatIndex();
            m_UserVolume         = m_VoiceClass->GetDefaultVolume();
            m_UserRate           = m_VoiceClass->GetDefaultRate();
        }
    }

    return (true);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::SetVoiceToInitSettings()
{
    if (m_VoiceClass)
    {
        // set voice to user's settings
        HRESULT hr = S_OK;
        CComPtr<IEnumSpObjectTokens> cpEnum;

        //Enumerate the available voices
        if(SUCCEEDED(hr))
            hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

        ULONG ulCount = 0;
        //Get the number of voices
        if(SUCCEEDED(hr))
            hr = cpEnum->GetCount(&ulCount);

        if (SUCCEEDED(hr) && m_CurrentVoiceIndex<ulCount)
        {
            CComPtr<ISpObjectToken> cpVoiceToken;
            ULONG i;
            for (i=0;(i<ulCount && SUCCEEDED(hr));i++)
            {
                cpVoiceToken.Release();
                if(SUCCEEDED(hr))
                    hr = cpEnum->Next( 1, &cpVoiceToken, NULL );

                if(SUCCEEDED(hr) && i == m_CurrentVoiceIndex)
                {
                    hr = m_VoiceClass->GetcpVoice()->Speak( NULL, SPF_PURGEBEFORESPEAK, 0);
                    if(SUCCEEDED(hr))
                    {
                        hr = m_VoiceClass->GetcpVoice()->SetVoice(cpVoiceToken);
                    }
                }
            }
        }

        m_VoiceClass->SetVolume(m_UserVolume);
        m_VoiceClass->SetRate(m_UserRate);

        if (m_VoiceClass->GetcpOutAudio() && m_CurrentFormatIndex<NUM_OUTPUTFORMATS)
        {
            SPSTREAMFORMAT eFmt = g_aOutputFormat[m_CurrentFormatIndex];
            CSpStreamFormat Fmt;
            Fmt.AssignFormat(eFmt);
    
            hr = m_VoiceClass->GetcpOutAudio()->SetFormat( Fmt.FormatId(), Fmt.WaveFormatExPtr() );
    
            if( SUCCEEDED( hr ) )
            {
                hr = m_VoiceClass->GetcpVoice()->SetOutput( m_VoiceClass->GetcpOutAudio(), FALSE );
            }
        }
        return (true);
    }
    return (false);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::SetVoiceToSystemDefaults()
{
    if (m_VoiceClass)
    {
        if (m_VoiceClass->SetToDefaults())
        {
            m_CurrentVoiceIndex  = m_VoiceClass->GetDefaultVoiceIndex();
            m_CurrentFormatIndex = m_VoiceClass->GetDefaultFormatIndex();
            m_UserVolume         = m_VoiceClass->GetDefaultVolume();
            m_UserRate           = m_VoiceClass->GetDefaultRate();
            return (true);
        }
    }
    return (false);
}
///////////////////////////////////////////////////////////////////////////////////
bool WinTextReader_Class::DoMenuCommand(int MenuCommand)
{
    switch (MenuCommand)
    {
    case IDM_EXIT:
        DestroyWindow(m_SystemWindow);
        return(true);

    case IDD_SHOW:
        if (m_MainControlDlg)
        {
            if (IsIconic(m_MainControlDlg))
            {
                OpenIcon(m_MainControlDlg);
            }
            BringWindowToTop(m_MainControlDlg);
        }
        else
        {
            DialogBox(m_hInst, (LPCTSTR)IDD_VOICEDIALOG, NULL, (DLGPROC)MainControlDlg);
        }
        return(true);

    case IDD_HIDE:
        if (m_MainControlDlg)
        {
            SendMessage(m_MainControlDlg,WM_CLOSE,0,0);
        }
        return(true);

    case IDD_ClearHistory:
        m_AllTxt.erase();
        m_CurrentTxt.erase();
        if (m_TxtWindow)
        {
            m_TxtWindow->Clear();
        }
        return (true);

    case IDD_KeepText:
        m_KeepText=!m_KeepText;

        if (m_MainControlDlg)
        {
            HMENU hmenu = GetMenu(m_MainControlDlg);
            if (hmenu)
            {
                HMENU hpopup = GetSubMenu(hmenu, 0);
                if (hpopup)
                {
                    if (m_KeepText)
                    {
                        CheckMenuItem(hmenu,IDD_KeepText,MF_CHECKED | MF_BYCOMMAND);
                    }
                    else
                    {
                        CheckMenuItem(hmenu,IDD_KeepText,MF_UNCHECKED | MF_BYCOMMAND);
                    }
                }
            }
        }
        return (true);

    case IDD_TEST:
        if (m_VoiceClass)
        {
            m_VoiceClass->TestVoice();
        }
        return(true);

    case IDD_REPEAT:
        m_VoiceClass->Speak(m_CurrentTxt);
        return (true);

    case IDD_Halt:
        m_VoiceClass->Stop();
        return (true);

    case IDD_RESETVOICEINIT:
        LoadSpeechSettings();
        if (LoadedUsersSetting)
        {
            SetVoiceToInitSettings();
        }
        else
        {
            SetVoiceToSystemDefaults();
        }
        if (m_MainControlDlg)
        {
            SendMessage(m_MainControlDlg,WM_COMMAND,ID_RESET,0);
        }
        break;

    case IDD_RESETVOICESYS:

        SetVoiceToSystemDefaults();
        if (m_MainControlDlg)
        {
            SendMessage(m_MainControlDlg,WM_COMMAND,ID_RESET,0);
        }
        break;

    case IDD_HOTKEY:
        if (m_HotKeyBox)
        {
            BringWindowToTop(m_HotKeyBox);
        }
        else
        {
            DialogBox(m_hInst, (LPCTSTR)IDD_HOTKEYDIALOG, m_SystemWindow, (DLGPROC)HotKeyDlg);
        }
        break;
    case IDD_ABOUTBOX:
        if (m_AboutBox)
        {
            BringWindowToTop(m_AboutBox);
        }
        else
        {
            DialogBox(m_hInst, (LPCTSTR)IDD_ABOUTBOX, m_SystemWindow, (DLGPROC)About);
        }
        return(true);
    }
    return (false);
}
///////////////////////////////////////////////////////////////////////////////////
BOOL WinTextReader_Class::TalkClipBoardData()
{ 
    HGLOBAL   hglb; 

    if (IsClipboardFormatAvailable(CF_TEXT))
    {
        if (!OpenClipboard(NULL)) 
                return(FALSE); 
 
        hglb = GetClipboardData(CF_TEXT);

        if (hglb != NULL) 
        { 
            char* lptstr = (char*) GlobalLock(hglb); 
            if (lptstr != NULL) 
            { 
                if (m_VoiceClass->IsProcessing())
                {
                    m_VoiceClass->Stop();
                }
                m_CurrentTxt.assign(lptstr);
                GlobalUnlock(hglb);

                if (m_KeepText)
                {
                    m_AllTxt.append(m_CurrentTxt);
                }
                else
                {
                    m_AllTxt.assign(m_CurrentTxt);
                }
                m_VoiceClass->Speak(m_CurrentTxt.c_str());
                if (m_TxtWindow)
                {
                    if (!m_KeepText)
                        m_TxtWindow->Clear();
                    m_TxtWindow->AppendText(m_CurrentTxt.c_str());
                }
            } 
            CloseClipboard();
 
            if (lptstr)
            {
                return(TRUE); 
            }
        }
    }
    else if (IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        hglb = GetClipboardData(CF_UNICODETEXT);

        if (hglb != NULL) 
        { 
            WCHAR *wStr = (WCHAR*) GlobalLock(hglb); 
            if (wStr != NULL) 
            { 
                m_VoiceClass->Speak(wStr);
                GlobalUnlock(hglb);
            } 
            CloseClipboard();
 
            if (wStr)
            {
                return(TRUE); 
            }
        }
    }
    return(FALSE); 
}
///////////////////////////////////////////////////////////////////////////////////
//
// Handles the WM_TalkingNoteAppCUSTOMEVENT application defined message and all
// of it's appropriate SAPI5 events.
//
void WinTextReader_Class::MainHandleSynthEvent()
{
    CSpEvent        event;  // helper class in sphelper.h for events that releases any 
                            // allocated memory in it's destructor - SAFER than SPEVENT
    SPVOICESTATUS   Stat;
    WPARAM          nStart;
    LPARAM          nEnd;
    int             i = 0;
    HRESULT 		hr = S_OK;

    if (!m_VoiceClass)
        return;

    while( event.GetFrom(m_VoiceClass->GetcpVoice()) == S_OK )
    {
        switch( event.eEventId )
        {
            case SPEI_START_INPUT_STREAM:

                break; 

            case SPEI_END_INPUT_STREAM:
                // Set global boolean stop to TRUE when finished speaking
                m_VoiceClass->SetStopFlag(TRUE);
                /*
                // Highlight entire text
                nStart=m_SelStart;
                nEnd=m_SelEnd;
                SendDlgItemMessage( m_hWnd, IDE_EDITBOX, EM_SETSEL, nStart, nEnd );
                */

                break;     
                
            case SPEI_VOICE_CHANGE:
                break;

            case SPEI_TTS_BOOKMARK:
                
                break;

            case SPEI_WORD_BOUNDARY:
                if (m_TxtWindow)
                {
                    hr = m_VoiceClass->GetcpVoice()->GetStatus( &Stat, NULL );

                    if (SUCCEEDED( hr ))
                    {
                        // Highlight word
                        nStart = (LPARAM)( Stat.ulInputWordPos / sizeof(char) ) + m_AllTxt.length()-m_CurrentTxt.length();
                        nEnd = nStart + Stat.ulInputWordLen;
                        m_TxtWindow->HighlightTxt(nStart,nEnd);
                    }
                }
                break;

            case SPEI_PHONEME:
                break;

            case SPEI_VISEME:
                break;

            case SPEI_SENTENCE_BOUNDARY:
                break;

            case SPEI_TTS_AUDIO_LEVEL:
                break;

            case SPEI_TTS_PRIVATE:
                break;

            default:
                // Unknown message
                break;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////
//
// This function handles the volume and rate controls
//
void WinTextReader_Class::HandleScroll(HWND hCtl)
{
    long hpos = 1;
    HWND hVolume, hRate;
    HRESULT	hr = S_OK;

    // Get the current position of the slider
    hpos = SendMessage( hCtl, TBM_GETPOS, 0, 0 );

    // Get the Handle for the scroll bar so it can be associated with an edit box
    hVolume = GetDlgItem( m_MainControlDlg, IDC_VOLUME_SLIDER );
    hRate = GetDlgItem( m_MainControlDlg, IDC_RATE_SLIDER );
    
    if( hCtl == hVolume )
    {
        m_VoiceClass->SetVolume((USHORT)hpos);
    }
    else if( hCtl == hRate )
    {
		m_VoiceClass->SetRate(hpos);
    }
    return;
}