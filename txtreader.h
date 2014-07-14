/////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////
#ifndef WinTextReader_Class_I
#define WinTextReader_Class_I
/////////////////////////////////////////////////////////////////////////
extern LRESULT CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
extern LRESULT CALLBACK HotKeyDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
extern LRESULT CALLBACK MainControlDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
/////////////////////////////////////////////////////////////////////////
#define DlgMinXVal 454
#define DlgMinYVal 210
/////////////////////////////////////////////////////////////////////////
class WinTextReader_Class
{
private:
    VoiceClass* m_VoiceClass;
    std::string m_CurrentTxt; // all processed text
    std::string m_AllTxt;     // text currently processing
    
    UINT m_nIDHotKey;
    UINT m_HotKeyCode;
    UINT m_HotKeyModifier;

    HINSTANCE m_hInst;
    HWND m_SystemWindow;   // The main controlling window
    HWND m_AboutBox;       // About box handle
    HWND m_HotKeyBox;      // Hotkey selection dialogue box
    HWND m_MainControlDlg; // Dialogue control window

    DlgTxtControl *m_TxtWindow; // The text output

    bool LoadedUsersSetting;
    int m_CurrentVoiceIndex,m_CurrentFormatIndex;
    USHORT m_UserVolume;
    long m_UserRate;

    bool m_KeepText;

public:
    WinTextReader_Class();

    ~WinTextReader_Class();

    UINT GetHotKeyCode() { return(m_HotKeyCode); }
    void SetHotKeyCode(UINT aCode) { m_HotKeyCode=aCode; }

    UINT GetHotKeyModifier() { return(m_HotKeyModifier); }
    void SetHotKeyModifier(UINT aCode) { m_HotKeyModifier=aCode; }

    UINT GetHotKeyID() { return(m_nIDHotKey); }
    void SetHotKeyID(UINT aCode) { m_nIDHotKey=aCode; }

    void SetKeepTextFlag(bool aStatus) { m_KeepText = aStatus; }
    bool DoKeepText() { return (m_KeepText); }

    bool ClearHistory();

    void SetWinInstance(HINSTANCE a_hInst) { m_hInst = a_hInst; }

    HINSTANCE GetWinInstance() { return (m_hInst); }

    void SetSystemWindow(HWND aWin) { m_SystemWindow = aWin; }
    HWND GetSystemWindow() { return (m_SystemWindow); }

    void SetMainControlDlg(HWND aWin);
    HWND GetMainControlDlg() { return (m_MainControlDlg); }

    void SetAboutBox(HWND aWnd) { m_AboutBox = aWnd; }
    HWND HasAboutBox() { return (m_AboutBox); }
    void SetHotKeyBox(HWND aWnd) { m_HotKeyBox = aWnd; }
    HWND HasHotKeyBox() { return (m_HotKeyBox); }

    bool StartUpWindowInterface(HWND aWnd);
    bool ShutDownWindowInterface();

    bool DoMenuCommand(int MenuCommand);
    void MainHandleSynthEvent();
    BOOL TalkClipBoardData();

    void SetCurrentVoiceIndex(int aVal) { m_CurrentVoiceIndex = aVal; }
    int GetCurrentVoiceIndex() { return (m_CurrentVoiceIndex); }
    int GetCurrentFormatIndex() { return (m_CurrentFormatIndex); }

    void SetTxtWindow(DlgTxtControl* aCtrl) { m_TxtWindow = aCtrl; }
    DlgTxtControl* GetTxtWindow() { return (m_TxtWindow); }
    bool InitTxtWindow();

    VoiceClass* GetVoiceClass() { return (m_VoiceClass); }

    void HandleScroll(HWND hCtl );

    BOOL SaveSpeechSettings();
    BOOL LoadSpeechSettings();
    BOOL UpdateVoiceSettings();

    BOOL SaveInitialSettings();
    BOOL LoadInitialSettings();

    BOOL SaveDialogueCurrentStatus();
    BOOL LoadDialogueCurrentStatus(bool RetoreWindowStatus=true);

    bool SetVoiceToSystemDefaults();
    bool SetVoiceToInitSettings();
};
#endif