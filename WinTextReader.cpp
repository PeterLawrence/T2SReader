///////////////////////////////////////////////////////////////////////////////////
//
//  T2SReader Application
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
// Modification History
//
//  v0.1.0 (Beta) 17/02/07 First public Release
//
//  Added Scroll bar to text output window
//  v0.1.1 (Beta) 19/02/07 
//
//  Fixed problem with iconized Control Dialogue being closed and saving of registry (19/02/07)
//  v0.1.2 (Beta) 20/02/07 
//
//  Minor improvements to user interface
//  v0.1.3 (Beta) 10/05/07 
//
//  Updated hot key default for windows7
//  v0.1.4 (Beta) 24/10/11
//
///////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////
#define UWM_SYSTRAY (WM_USER + 1)
///////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
WinTextReader_Class g_WinTextReader;
///////////////////////////////////////////////////////////////////////////////////
#ifdef LOAD_SHELL_DLL
#undef Shell_NotifyIcon
typedef BOOL (CALLBACK *LPRShell_NotifyIcon)(DWORD,PNOTIFYICONDATAA);
LPRShell_NotifyIcon Shell_NotifyIcon;
#endif
///////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hwnd;
    WNDCLASSEX wc;
    MSG msg;
    NOTIFYICONDATA nid;
    char *classname = "T2SReader";
    
#ifdef LOAD_SHELL_DLL
    HMODULE hshell32;
    if (!(hshell32 = LoadLibrary("SHELL32"))) exit(1);
    if (!(Shell_NotifyIcon = (LPRShell_NotifyIcon) GetProcAddress(hshell32, "Shell_NotifyIconA")))
        exit(1);
#endif
    
    if (FAILED(CoInitialize(NULL)))
    {
        MessageBox( NULL, 
	                _T("Error initializing COM. Shutting down."), 
	                _T("Error"), MB_OK );
        return(0);
    }

    g_WinTextReader.SetWinInstance(hInstance);
    g_WinTextReader.LoadSpeechSettings();
    
    //SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
    
    //Create a window class for the window that receives systray notifications.
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = (LPCSTR)IDM_CONTEXTMAIN;
    wc.lpszClassName = classname;
    wc.hIconSm = LoadIcon(wc.hInstance, (LPCTSTR)IDI_SMALL);
    RegisterClassEx(&wc);
    
   
    hwnd = CreateWindow(classname, 0, WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    
    // Fill out NOTIFYICONDATA structure
    nid.cbSize = sizeof(NOTIFYICONDATA); // size
    nid.hWnd = hwnd; // window to receive notifications
    nid.uID = 1;     // application-defined ID for icon (can be any UINT value)
    nid.uFlags = NIF_MESSAGE |  // nid.uCallbackMessage is valid, use it
        NIF_ICON |     // nid.hIcon is valid, use it
        NIF_TIP;       // nid.szTip is valid, use it
    nid.uCallbackMessage = UWM_SYSTRAY; // message sent to nid.hWnd
    nid.hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON), 0); // 16x16 icon

    strcpy(nid.szTip, "T2SReader");
    
    // NIM_ADD: Add icon; NIM_DELETE: Remove icon; NIM_MODIFY: modify icon
    Shell_NotifyIcon(NIM_ADD, &nid); // This adds the icon

    g_WinTextReader.SetSystemWindow(hwnd);
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
#ifdef LOAD_SHELL_DLL
    FreeLibrary(hshell32);
#endif

    g_WinTextReader.SaveSpeechSettings();
    g_WinTextReader.ShutDownWindowInterface();
    // Unload COM
	CoUninitialize();

    return msg.wParam;
}
///////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
    switch (message)
    {
    case WM_CREATE:
        {
            if (!g_WinTextReader.StartUpWindowInterface(hwnd))
            {
                // failed to start
                SendMessage(hwnd,WM_CLOSE,0,0);
            }
        }
        return TRUE;
        
    case WM_DESTROY:
        {
            g_WinTextReader.ShutDownWindowInterface();
            NOTIFYICONDATA nid;
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = 1;
            nid.uFlags = NIF_TIP;
            Shell_NotifyIcon(NIM_DELETE, &nid); // removes the icon
            PostQuitMessage(0);
        }
        return TRUE;
        
    case WM_COMMAND:
        {
            int  wmId    = LOWORD(wParam); 
            int wmEvent = HIWORD(wParam); 
            // Parse the menu command:
            g_WinTextReader.DoMenuCommand(wmId);
        }
        break;

    case WM_HOTKEY:
            keybd_event( VK_CONTROL, 0, 0, 0);
            keybd_event( 0x43, 0, 0, 0);
            Sleep(10);
            keybd_event( 0x43, 0, KEYEVENTF_KEYUP, 0);
            keybd_event( VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
            g_WinTextReader.TalkClipBoardData();
            break;

    case WM_TalkingNoteAppCUSTOMEVENT:
            g_WinTextReader.MainHandleSynthEvent();
            return TRUE;
        
    case UWM_SYSTRAY: // Mouse activity over the icon
        switch (lParam)
        {
        case WM_RBUTTONUP:
            {
                POINT pt;
                GetCursorPos(&pt);
                HMENU hmenu = LoadMenu(g_WinTextReader.GetWinInstance(), MAKEINTRESOURCE(IDM_CONTEXTMAIN));
                HMENU hpopup = GetSubMenu(hmenu, 0);
                if (g_WinTextReader.DoKeepText())
                {
                    CheckMenuItem(hmenu,IDD_KeepText,MF_CHECKED | MF_BYCOMMAND);
                }
                else
                {
                    CheckMenuItem(hmenu,IDD_KeepText,MF_UNCHECKED | MF_BYCOMMAND);
                }
                SetForegroundWindow(hwnd);
                int aMenuCommand= TrackPopupMenu(hpopup,TPM_RETURNCMD | TPM_RIGHTBUTTON,pt.x, pt.y,0,hwnd,NULL);
                g_WinTextReader.DoMenuCommand(aMenuCommand);

                DestroyMenu(hmenu); // Delete loaded menu
            }
            break;

        case WM_LBUTTONDBLCLK:
            {
                if (g_WinTextReader.GetMainControlDlg())
                {
                    if (IsIconic(g_WinTextReader.GetMainControlDlg()))
                    {
                        OpenIcon(g_WinTextReader.GetMainControlDlg());
                    }
                    else
                    {
                        CloseWindow(g_WinTextReader.GetMainControlDlg());
                        OpenIcon(g_WinTextReader.GetMainControlDlg());
                    }
                    //BringWindowToTop(g_WinTextReader.GetMainControlDlg());
                }
                else
                {
                    DialogBox(g_WinTextReader.GetWinInstance(), (LPCTSTR)IDD_VOICEDIALOG, NULL, (DLGPROC)MainControlDlg);
                }
            }
            break;
        }
        return TRUE;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}
///////////////////////////////////////////////////////////////////////////////////
// Mesage handler for about box.
LRESULT CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    USES_CONVERSION;
    
    switch( message )
    {
    case WM_INITDIALOG:
        g_WinTextReader.SetAboutBox(hDlg);
        break;
        
    case WM_COMMAND:
        {
            WORD wId    = LOWORD(wParam); 
            WORD wEvent = HIWORD(wParam); 
            
            switch( wId )
            {
            case IDOK:
                EndDialog( hDlg, LOWORD(wParam) );
                g_WinTextReader.SetAboutBox(NULL);
                return TRUE;
            }
        }
        break;

    case WM_CLOSE:
        g_WinTextReader.SetAboutBox(NULL);
        EndDialog( hDlg, LOWORD(wParam) );
        return (TRUE);
    }
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////
#define NUM_HOTKEYS 48
TCHAR* g_HotKeyList[NUM_HOTKEYS] = {_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F"),_T("G"),_T("H"),_T("I"),
                                    _T("J"),_T("K"),_T("L"),_T("M"),_T("N"),_T("O"),_T("P"),_T("Q"),_T("R"),
                                    _T("S"),_T("T"),_T("U"),_T("V"),_T("W"),_T("X"),_T("Y"),_T("Z"),_T("0"),
                                    _T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),
                                    _T("F1"),_T("F2"),_T("F3"),_T("F4"),_T("F5"),_T("F6"),_T("F7"),_T("F8"),_T("F9"),
                                    _T("F10"),_T("F11"),_T("F12") };
UINT g_HotKeyCode[NUM_HOTKEYS]  = { 'A','B','C','D','E','F','G','H','I',
                                    'J','K','L','M','N','O','P','Q','R',
                                    'S','T','U','V','W','X','Y','Z','0',
                                    '1','2','3','4','5','6','7','8','9',
                                    VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,
                                    VK_F10,VK_F11,VK_F12 };
///////////////////////////////////////////////////////////////////////////////////
int ModifierSelectionPos(const UINT &ModifierCode)
{
    switch (ModifierCode)
    {
    case MOD_ALT:
        return (0);
    case MOD_WIN:
        return (1);
    case (MOD_CONTROL | MOD_SHIFT):
        return (2);
    case (MOD_CONTROL | MOD_SHIFT | MOD_ALT):
        return (3);
    }
    return (1);
}
///////////////////////////////////////////////////////////////////////////////////
UINT SelectionPosToModifier(const int &SelectionPos)
{
    switch (SelectionPos)
    {
    case 0:
        return (MOD_ALT);
    case 1:
        return (MOD_WIN);
    case 2:
        return ((MOD_CONTROL | MOD_SHIFT));
    case 3:
        return ((MOD_CONTROL | MOD_SHIFT | MOD_ALT));
    }
    return (1);
}
///////////////////////////////////////////////////////////////////////////////////
// Mesage handler for hotkey control box
LRESULT CALLBACK HotKeyDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    USES_CONVERSION;
    
    switch( message )
    {
    case WM_INITDIALOG:
        {
            g_WinTextReader.SetHotKeyBox(hDlg);
            // Initialize the hotkey combo box
            int i,Selected = 0;
            for(i=0; i<NUM_HOTKEYS; i++ )
            {
                SendDlgItemMessage( hDlg, IDC_COMBHOTKEY, CB_ADDSTRING, 0,(LPARAM)g_HotKeyList[i] );
                if (g_HotKeyCode[i]==g_WinTextReader.GetHotKeyCode())
                {
                    Selected=i;
                }
            }
            SendDlgItemMessage( hDlg, IDC_COMBHOTKEY, CB_SETCURSEL, Selected, 0 );
            SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_ADDSTRING, 0,(LPARAM)_T("Alt Key"));
            SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_ADDSTRING, 0,(LPARAM)_T("Window's Key"));
            SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_ADDSTRING, 0,(LPARAM)_T("Ctrl + Shift"));
            SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_ADDSTRING, 0,(LPARAM)_T("Ctrl + Shift + Alt"));
            SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_SETCURSEL, ModifierSelectionPos(g_WinTextReader.GetHotKeyModifier()), 0 );

            EnableWindow(GetDlgItem(hDlg,IDAPPLY),FALSE);
        }
        break;
        
    case WM_COMMAND:
        {
            WORD wId    = LOWORD(wParam); 
            WORD wEvent = HIWORD(wParam); 
            
            switch( wId )
            {
            case IDAPPLY:
                EnableWindow(GetDlgItem(hDlg,IDAPPLY),FALSE);
            case IDOK:
                {
                    // read key code
                    int Selected = SendDlgItemMessage( hDlg, IDC_COMBHOTKEY, CB_GETCURSEL, 0, 0 );
                    if (Selected!=CB_ERR)
                    {
                        UINT NewKeyCode =  g_HotKeyCode[Selected];
                        int Selected = SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_GETCURSEL, 0, 0 );
                        if (Selected!=CB_ERR)
                        {
                            UINT NewModCode =  SelectionPosToModifier(Selected);
                            if (NewModCode!=g_WinTextReader.GetHotKeyModifier() || NewKeyCode!=g_WinTextReader.GetHotKeyCode() || g_WinTextReader.GetHotKeyID()==0)
                            {
                                UINT OldKeyCode=g_WinTextReader.GetHotKeyCode();
                                UINT OldModKey=g_WinTextReader.GetHotKeyModifier();
                                UINT OldKeyID=g_WinTextReader.GetHotKeyID();
                                if (OldKeyID!=0)
                                {
                                    if (UnregisterHotKey(g_WinTextReader.GetSystemWindow(), OldKeyID)==0)
                                    {
                                        DWORD ErrCode=GetLastError();
                                        MessageBox( hDlg, 
                                        _T("Can't assign new Hoykey. Please select an alternative key"), 
                                        _T("Error"), MB_OK );
                                        return(false);
                                    }
                                    else
                                    {
                                        g_WinTextReader.SetHotKeyID(0);
                                    }
                                }

                                UINT NewKeyCodeID= GlobalAddAtom("T2SReader");
                                if (RegisterHotKey(g_WinTextReader.GetSystemWindow(), NewKeyCodeID, NewModCode,NewKeyCode)==0)
                                {
                                    // try to set back to old key
                                    DWORD ErrCode=GetLastError();
                                    if (OldKeyID)
                                    {
                                        if (RegisterHotKey(g_WinTextReader.GetSystemWindow(), OldKeyID, OldModKey,OldKeyCode)==0)
                                        {
                                            ErrCode=GetLastError();
                                        }
                                        else
                                        {
                                            g_WinTextReader.SetHotKeyCode(OldKeyCode);
                                            g_WinTextReader.SetHotKeyModifier(OldModKey);
                                            g_WinTextReader.SetHotKeyID(OldKeyID);
                                        }
                                    }
                                    MessageBox( hDlg, 
                                        _T("Can't assign the new Hoykey.\r\nThe new Hotkey could be in use by another program.\r\nPlease select an alternative key"), 
                                        _T("Error"), MB_OK );
                                    return(false);
                                }
                                else
                                {
                                    g_WinTextReader.SetHotKeyCode(NewKeyCode);
                                    g_WinTextReader.SetHotKeyModifier(NewModCode);
                                    g_WinTextReader.SetHotKeyID(NewKeyCodeID);
                                }
                            }
                        }
                    }
                }
            }

            switch( wId )
            {
            case IDCANCEL:
            case IDOK:
                EndDialog( hDlg, LOWORD(wParam) );
                g_WinTextReader.SetHotKeyBox(NULL);
                return TRUE;

            case IDC_COMBHOTKEY:
            case IDC_COMBMODKEY:
                int Selected = SendDlgItemMessage( hDlg, IDC_COMBHOTKEY, CB_GETCURSEL, 0, 0 );
                if (Selected!=CB_ERR)
                {
                    UINT NewKeyCode =  g_HotKeyCode[Selected];
                    int Selected = SendDlgItemMessage( hDlg, IDC_COMBMODKEY, CB_GETCURSEL, 0, 0 );
                    if (Selected!=CB_ERR)
                    {
                        UINT NewModCode =  SelectionPosToModifier(Selected);
                        if (NewModCode!=g_WinTextReader.GetHotKeyModifier() || NewKeyCode!=g_WinTextReader.GetHotKeyCode() || g_WinTextReader.GetHotKeyID()==0)
                        {
                            EnableWindow(GetDlgItem(hDlg,IDAPPLY),TRUE);
                            return (true);
                        }
                    }
                }
                EnableWindow(GetDlgItem(hDlg,IDAPPLY),FALSE);
            }
        }
        break;

    case WM_CLOSE:
        g_WinTextReader.SetHotKeyBox(NULL);
        EndDialog( hDlg, LOWORD(wParam) );
        return (TRUE);
    }
    return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////
#define LEFTSTICKY         0xFF000000UL
#define TOPSTICKY          0x00FF0000UL
#define RIGHTSTICKY        0x0000FF00UL
#define BOTTOMSTICKY       0x000000FFUL
#define SAMESTICKY         0xF0F0F0F0UL
#define ALLSTICKY          0xFFFFFFFFUL
#define TOPLEFTSTICKY      0xFFFF0000UL
#define TOPRIGHTSTICKY     0x00FFFF00UL
#define BOTTOMLEFTSTICKY   0xFF0000FFUL
#define BOTTOMRIGHTSTICKY  0x0000FFFFUL
#define NULLSTICKY         0x00000000UL
//////////////////////////////////////////////////////////////////////////////////
void MoveDlgItem(HWND DlgWin,unsigned long TheID,int XOffset,int YOffset, unsigned int GlueType)
{
    RECT TheRect;
    HWND aWin=GetDlgItem(DlgWin,TheID);
    GetWindowRect(aWin,&TheRect);
    POINT TheLocation;

    TheLocation.x=TheRect.left;
    TheLocation.y=TheRect.top;
    ScreenToClient(DlgWin,&TheLocation);
    TheRect.left=TheLocation.x;
    TheRect.top=TheLocation.y;
    TheLocation.x=TheRect.right;
    TheLocation.y=TheRect.bottom;
    ScreenToClient(DlgWin,&TheLocation);
    TheRect.right=TheLocation.x;
    TheRect.bottom=TheLocation.y;

    if (GlueType==TOPLEFTSTICKY)
    {
        TheRect.bottom=TheRect.bottom+YOffset;
    }
    else if (GlueType==BOTTOMRIGHTSTICKY)
    {
        TheRect.right=TheRect.right+XOffset;
        TheRect.top=TheRect.top+YOffset;
        TheRect.bottom=TheRect.bottom+YOffset;
    }
    else if (GlueType==ALLSTICKY)
    {
        TheRect.right=TheRect.right+XOffset;
        TheRect.bottom=TheRect.bottom+YOffset;
    }
    else
    {
        if ((GlueType & RIGHTSTICKY)==RIGHTSTICKY)
        {
            TheRect.right=TheRect.right+XOffset;
            TheRect.left=TheRect.left+XOffset;
        }
        if ((GlueType & BOTTOMSTICKY)==BOTTOMSTICKY)
        {
            TheRect.top=TheRect.top+YOffset;
            TheRect.bottom=TheRect.bottom+YOffset;
        }
        if ((GlueType & LEFTSTICKY)==LEFTSTICKY)
        {
        }
        if ((GlueType & TOPSTICKY)==TOPSTICKY)
        {
        }
    }
    MoveWindow(aWin,TheRect.left,TheRect.top,TheRect.right-TheRect.left,TheRect.bottom-TheRect.top,TRUE);
}
//////////////////////////////////////////////////////////////////////////////////
bool HandleSize(HWND DlgWin,int theWidth, int theHeight, int &oldWidth, int &oldHeight)
{ 
    if (IsIconic(DlgWin)) return(true);

    RECT ClientRect;
    GetClientRect(DlgWin,&ClientRect);
  
    // difference in size
    int XOffset=theWidth-oldWidth;
    int YOffset=theHeight-oldHeight;

    // ratios
    float ExpandX=theWidth/float(oldWidth);
    float ExpandY=theHeight/float(oldHeight);

    MoveDlgItem(DlgWin,IDE_EDITBOX,XOffset,YOffset,ALLSTICKY);
    
    MoveDlgItem(DlgWin,IDC_TXTVOICE,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDC_COMBO_VOICES,XOffset,YOffset,BOTTOMSTICKY);

    MoveDlgItem(DlgWin,IDC_TXTRATE,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDC_RATE_SLIDER,XOffset,YOffset,BOTTOMSTICKY);

    MoveDlgItem(DlgWin,IDC_TXTVOLUME,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDC_VOLUME_SLIDER,XOffset,YOffset,BOTTOMSTICKY);

    MoveDlgItem(DlgWin,IDC_TXTFORMAT,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDC_COMBO_OUTPUT,XOffset,YOffset,BOTTOMSTICKY);

    MoveDlgItem(DlgWin,IDSTOP,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDD_REPEAT,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDD_ClearHistory,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDSHUTDOWN,XOffset,YOffset,BOTTOMSTICKY);
    MoveDlgItem(DlgWin,IDCLOSE,XOffset,YOffset,BOTTOMSTICKY);

    oldWidth=ClientRect.right;
    oldHeight=ClientRect.bottom;
 
    InvalidateRect( DlgWin, NULL, FALSE );
    return(true);
}
///////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainControlDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int oldWidth=0,oldHeight=0;
    
    switch (message)
    {
    case WM_INITDIALOG:
        {
            g_WinTextReader.SetTxtWindow(new DlgTxtControl(hDlg,IDE_EDITBOX));
            g_WinTextReader.SetMainControlDlg(hDlg);
            
            HRESULT hr=S_FALSE;
            
            // Initialize the Output Format combo box
            for( int i=0; i<NUM_OUTPUTFORMATS; i++ )
            {
                SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_ADDSTRING, 0,
                    (LPARAM)g_aszOutputFormat[i] );
                
                SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_SETITEMDATA, i, 
                    (LPARAM)g_aOutputFormat[i] );
            }
            
            // Set the default output format as the current selection.
            if( SUCCEEDED( hr ) )
            {
                CComPtr<ISpStreamFormat> cpStream;
                HRESULT hrOutputStream = g_WinTextReader.GetVoiceClass()->GetcpVoice()->GetOutputStream(&cpStream);
                
                if (hrOutputStream == S_OK)
                {
                    CSpStreamFormat Fmt;
                    hr = Fmt.AssignFormat(cpStream);
                    if (SUCCEEDED(hr))
                    {
                        SPSTREAMFORMAT eFmt = Fmt.ComputeFormatEnum();
                        for( i=0; i<NUM_OUTPUTFORMATS; i++ )
                        {
                            if( g_aOutputFormat[i] == eFmt )
                            {
                                SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_SETCURSEL, i, 0 );
                            }
                        }
                    }
                }
                else
                {
                    SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_SETCURSEL, 0, 0 );
                }
            }
            
            // Use the SAPI5 helper function in sphelper.h to initialize the Voice combo box.
            if ( SUCCEEDED( hr ) )
            {
                hr = SpInitTokenComboBox( GetDlgItem( hDlg, IDC_COMBO_VOICES ), SPCAT_VOICES );
                if (SUCCEEDED(hr))
                {
                    //Determine if it is the current voice
                    CComPtr<ISpObjectToken> pCurToken;
                    hr = g_WinTextReader.GetVoiceClass()->GetcpVoice()->GetVoice( &pCurToken );
                    if (SUCCEEDED(hr))
                    {
                        long NumberOfItems = SendDlgItemMessage( hDlg, IDC_COMBO_VOICES, CB_GETCOUNT, 0, 0);
                        if (NumberOfItems!=CB_ERR)
                        {
                            for( i=0; i<NumberOfItems; i++ )
                            {
                                ISpObjectToken* pToken = SpGetComboBoxToken( GetDlgItem( hDlg, IDC_COMBO_VOICES ),i );
                                if( pToken == pCurToken )
                                {
                                    SendDlgItemMessage( hDlg, IDC_COMBO_VOICES, CB_SETCURSEL, i, 0 );
                                }
                            }
                        }
                    }
                    SendDlgItemMessage( hDlg, IDC_COMBO_VOICES, CB_SETCURSEL, g_WinTextReader.GetCurrentVoiceIndex(), 0 );
                }
            }           
            
            // Get default rate and volume
            if( SUCCEEDED( hr ) )
            {
                // initialize sliders and edit boxes with default rate
                SendDlgItemMessage( hDlg, IDC_RATE_SLIDER, TBM_SETRANGE, TRUE, MAKELONG( SPMIN_RATE, SPMAX_RATE ) );
                SendDlgItemMessage( hDlg, IDC_RATE_SLIDER, TBM_SETPOS, TRUE, g_WinTextReader.GetVoiceClass()->GetRate() );
                SendDlgItemMessage( hDlg, IDC_RATE_SLIDER, TBM_SETPAGESIZE, TRUE, 5 );
            }
            
            if( SUCCEEDED( hr ) )
            {
                SendDlgItemMessage( hDlg, IDC_VOLUME_SLIDER, TBM_SETRANGE, TRUE, MAKELONG( SPMIN_VOLUME, SPMAX_VOLUME ) );
                SendDlgItemMessage( hDlg, IDC_VOLUME_SLIDER, TBM_SETPOS, TRUE, g_WinTextReader.GetVoiceClass()->GetVolume() );
                SendDlgItemMessage( hDlg, IDC_VOLUME_SLIDER, TBM_SETPAGESIZE, TRUE, 10 );
            }
            
            HINSTANCE hInstance=(HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE);
            HICON hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_WINTEXTREADER));
            HICON hIconSmall = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_SMALL));
            PostMessage (hDlg, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
            PostMessage (hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);
            DestroyIcon(hIcon);
            DestroyIcon(hIconSmall);
            
            g_WinTextReader.InitTxtWindow();
            
            RECT ClientRect;
            GetClientRect(hDlg,&ClientRect);
            oldWidth=ClientRect.right;
            oldHeight=ClientRect.bottom;
            
            g_WinTextReader.LoadDialogueCurrentStatus(true);

            HMENU hmenu = GetMenu(hDlg);
            if (hmenu)
            {
                HMENU hpopup = GetSubMenu(hmenu, 0);
                if (hpopup)
                {
                    if (g_WinTextReader.DoKeepText())
                    {
                        CheckMenuItem(hmenu,IDD_KeepText,MF_CHECKED | MF_BYCOMMAND);
                    }
                    else
                    {
                        CheckMenuItem(hmenu,IDD_KeepText,MF_UNCHECKED | MF_BYCOMMAND);
                    }
                    RemoveMenu(hpopup,IDD_SHOW,MF_BYCOMMAND);
                }
            }
        }
        return(TRUE);
        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCLOSE:
            {
                g_WinTextReader.SaveDialogueCurrentStatus();
                g_WinTextReader.SetMainControlDlg(NULL);
                EndDialog(hDlg, LOWORD(wParam));
            }
            return (TRUE);
            
        case IDSHUTDOWN:
            {
                g_WinTextReader.SaveDialogueCurrentStatus();
                g_WinTextReader.SetMainControlDlg(NULL);
                EndDialog(hDlg, LOWORD(wParam));
                SendMessage(g_WinTextReader.GetSystemWindow(),WM_CLOSE,0,0);
            }
            return (TRUE);
            
        case IDC_COMBO_VOICES:
            if( HIWORD(wParam) == CBN_SELCHANGE )
            {
                HRESULT         hr = S_OK;
                GUID*           pguidAudioFormat = NULL;
                
                // Get the token associated with the selected voice
                ISpObjectToken* pToken = SpGetCurSelComboBoxToken( GetDlgItem( hDlg, IDC_COMBO_VOICES ) );
                
                LONG TheSelPos=SendDlgItemMessage( hDlg, IDC_COMBO_VOICES, CB_GETCURSEL, g_WinTextReader.GetCurrentVoiceIndex(), 0 );
                if (TheSelPos != CB_ERR)
                {
                    g_WinTextReader.SetCurrentVoiceIndex(TheSelPos);
                }
                
                //Determine if it is the current voice
                CComPtr<ISpObjectToken> pOldToken;
                hr = g_WinTextReader.GetVoiceClass()->GetcpVoice()->GetVoice( &pOldToken );
                
                if (SUCCEEDED(hr))
                {
                    if (pOldToken != pToken)
                    {        
                        // Stop speaking. This is not necesary, for the next call to work,
                        // but just to show that we are changing voices.
                        hr = g_WinTextReader.GetVoiceClass()->GetcpVoice()->Speak( NULL, SPF_PURGEBEFORESPEAK, 0);
                        // And set the new voice on the global voice object
                        if (SUCCEEDED (hr) )
                        {
                            hr = g_WinTextReader.GetVoiceClass()->GetcpVoice()->SetVoice( pToken );
                        }
                    }
                }
            }
            break;
            
        case IDC_COMBO_OUTPUT:
            if( HIWORD(wParam) == CBN_SELCHANGE )
            {
                HRESULT         hr = S_OK;
                // Get the audio output format and set it's GUID
                long iFormat  = SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_GETCURSEL, 0, 0 );
                SPSTREAMFORMAT eFmt = (SPSTREAMFORMAT)SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT,
                    CB_GETITEMDATA, iFormat, 0 );
                CSpStreamFormat Fmt;
                Fmt.AssignFormat(eFmt);
                if ( g_WinTextReader.GetVoiceClass()->GetcpOutAudio() )
                {
                    hr = g_WinTextReader.GetVoiceClass()->GetcpOutAudio()->SetFormat( Fmt.FormatId(), Fmt.WaveFormatExPtr() );
                }
                else
                {
                    hr = E_FAIL;
                }
                
                if( SUCCEEDED( hr ) )
                {
                    hr = g_WinTextReader.GetVoiceClass()->GetcpVoice()->SetOutput( g_WinTextReader.GetVoiceClass()->GetcpOutAudio(), FALSE );
                }
            }
            break;
            
        case IDSTOP:
            g_WinTextReader.GetVoiceClass()->Stop();
            break;
            
       case ID_RESET:
            SendDlgItemMessage( hDlg, IDC_VOLUME_SLIDER, TBM_SETPOS, TRUE, g_WinTextReader.GetVoiceClass()->GetVolume() );
            SendDlgItemMessage( hDlg, IDC_RATE_SLIDER, TBM_SETPOS, TRUE, g_WinTextReader.GetVoiceClass()->GetRate() );
            
            // reset output format
            SendDlgItemMessage( hDlg, IDC_COMBO_OUTPUT, CB_SETCURSEL, g_WinTextReader.GetCurrentFormatIndex(), 0 );
            SendDlgItemMessage( hDlg, IDC_COMBO_VOICES, CB_SETCURSEL, g_WinTextReader.GetCurrentVoiceIndex(), 0 );

            // Change the volume and the rate to reflect what the UI says
            g_WinTextReader.HandleScroll(GetDlgItem( hDlg, IDC_VOLUME_SLIDER ) );
            g_WinTextReader.HandleScroll(GetDlgItem( hDlg, IDC_RATE_SLIDER ) );
            break;

        default: 
            // Parse the menu command
            g_WinTextReader.DoMenuCommand(LOWORD(wParam));
            break;
        }
        break;
        
    case WM_HSCROLL:
        g_WinTextReader.HandleScroll((HWND) lParam );
        return TRUE;
        
    case WM_SIZING:
        {
            RECT* ClientRect = (LPRECT) lParam;
            if ( (ClientRect->right-ClientRect->left)<DlgMinXVal ||
                (ClientRect->bottom-ClientRect->top)<DlgMinYVal )
            {
                if ((ClientRect->right-ClientRect->left)<DlgMinXVal)
                {
                    ClientRect->right=ClientRect->left+DlgMinXVal;
                }
                if ((ClientRect->bottom-ClientRect->top)<DlgMinYVal)
                {
                    ClientRect->bottom=ClientRect->top+DlgMinYVal;
                }
                return (TRUE);
            }
        }
        break;
        
    case WM_CLOSE:
        g_WinTextReader.SaveDialogueCurrentStatus();
        g_WinTextReader.SetMainControlDlg(NULL);
        EndDialog(hDlg, LOWORD(wParam));
        break;
        
    case WM_SIZE:
        {
            if (wParam==SIZE_MINIMIZED)
            {
                //DestroyWindow(hDlg);
            }
            else
            {
                int nWidth = LOWORD(lParam);  // width of client area 
                int nHeight = HIWORD(lParam); // height of client area
                return (HandleSize(hDlg,nWidth,nHeight,oldWidth,oldHeight));
            }
        }
        break;
    }
    return FALSE;
}
