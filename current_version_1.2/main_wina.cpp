#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource2.h"
#include "keys.h"
#include "list.h"
#include "mainThread.h"

//VERSION 2

//SendMessage(hA, EM_SETREADONLY, (WPARAM) TRUE, 0);
//MessageBox(hwnd, "No Device Detected", "Warning!", MB_OK | MB_ICONINFORMATION);   

BOOL on2 = FALSE, buttons = FALSE, close = TRUE, xbox = FALSE;
BOOL focus = TRUE, axis1Check = FALSE, axis2Check = FALSE, start1 = TRUE;

char a[10], abe[2], sens = 3, aa[3];
char szClassName[ ] = "Hades";
char buf3[20] = "No Device Detected";
char buf2[][14] = {"Controller 1", "Controller 2", "Controller 3"};
char sens_char[][2] = {"1","2","3","4","5"};

unsigned char keybrdMap[24];
unsigned char mouseMap[10] = {0,0,0,0,0,0,0,0,0,0};

int len, sel = 0, i, iter, temp89 = -2, g, Zmax, Zmin;
int pressed = -1, axismark = 0, temp90 = -1, focusOn = -1;


FILE* file;
WNDPROC DefEditProc;
HANDLE hndlThread = NULL, hndlThreadMain = NULL;
HWND hwndButtonRecheck = NULL, hDialogKeybrd = NULL, hwndTemp = NULL, hwndButtonStop = NULL, mn = NULL;
HWND hwndTabControl = NULL, hwndCombo = NULL, hwndButtonReset = NULL, hTemp = NULL, hDialogHelp =NULL;
HWND hDialog = NULL, hDialog2 = NULL, hwndButtonApply = NULL, hwndButtonSave = NULL;

DWORD th_id, th_idMain;
RECT wndPosRec;

HFONT hFont2;
HBITMAP hBitmap = NULL;
HDC hdcStatic = NULL;
HBRUSH staticBackground = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH g_hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

JOYCAPS joyCaps;
JOYINFOEX joyInfo;

UINT joyID[] = {0, 1, 2};
UINT currControl = 5, list[3] = {5, 5 ,5};

void reset();
void checkControl();
void loadSettings();
void saveSettings( HWND hwnd );
int getButton( JOYINFOEX );
DWORD WINAPI test(LPVOID);
DWORD WINAPI testMain(LPVOID);

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ToolDlgProc2(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ToolDlgProc3(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ToolDlgProcHelp(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               // MAin Window
    MSG messages;            
    WNDCLASSEX wincl;        
    
    //Win7 and XP style controls
    
    INITCOMMONCONTROLSEX InitCtrlEx;
    InitCtrlEx.dwSize = sizeof(InitCtrlEx);
    InitCtrlEx.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&InitCtrlEx);
    
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;  
    wincl.style = CS_DBLCLKS;                 // Catch double-clicks 
    wincl.cbSize = sizeof (WNDCLASSEX);

    
    wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL),
MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wincl.cbClsExtra = 0;                      
    wincl.cbWndExtra = 0;                      

    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;  

    for( int y=0; y<28;y++)
         keybrdMap[y] = 0;
    
    if ( !RegisterClassEx (&wincl))
         return 0;
    
    hwnd = CreateWindowEx (
           0,                   
           szClassName,         
           "Hades 1.2",       // Title
           WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX & ~WS_THICKFRAME, //Thickframe - No resize
           60,       // X - pos, CW_USEDEFAULT
           30,
           700,                 // Width
           680,                 
           HWND_DESKTOP,        // The window is a child-window to desktop 
           NULL,                // No menu
           hThisInstance,       // Program Instance
           NULL );
    
    ShowWindow (hwnd, nFunsterStil);
    mn = hwnd;
    // Tab Control Window
    hwndTabControl = CreateWindow (
                 WC_TABCONTROL, "", 
                 WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                 2, 0, 690, 630, 
                 hwnd, NULL, hThisInstance, NULL );
    
    hFont2 = CreateFont(-14, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0, ("Trebuchet MS"));
    SendMessage(hwndTabControl, WM_SETFONT, (WPARAM)hFont2, FALSE);
    
    hwndCombo = CreateWindow("COMBOBOX",
             "No Device Selected",
             WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | SS_CENTER,  // The SS_CENTER does not work...it only changes the style..no idea
             25, 53, 250, 60,
             hwnd,
             (HMENU)IDD_SIZE_CBO,
             hThisInstance,
             NULL);
    
    hwndButtonRecheck = CreateWindow( "BUTTON", "Refresh", 
                 WS_CHILD|WS_VISIBLE, 
                 285, 53, 80, 25, 
                 hwnd, (HMENU)IDC_BUTTON_RECHECK, hThisInstance, NULL);
    
    // Tabs
    TCITEM tci1,tci2;
    tci1.mask = TCIF_TEXT;
    tci1.pszText = " Mouse ";
    tci1.cchTextMax = strlen(" Mouse ") + 1;
    tci1.iImage = -1;
    tci1.lParam = 0;
    tci1.dwState = 2; //TCIS_BUTTONPRESSED;
 
    tci2.mask = TCIF_TEXT;
    tci2.pszText = " KeyBoard ";
    tci2.cchTextMax = strlen(" KeyBoard ") + 1;
    tci2.iImage = -1;
    tci2.lParam = 0;
    tci2.dwState = 2; //TCIS_BUTTONPRESSED;
    
    TabCtrl_InsertItem(hwndTabControl, 0, & tci1);
    TabCtrl_InsertItem(hwndTabControl, 1, & tci2);
                  
    
    ShowWindow ( hwndTabControl, nFunsterStil );
    ShowWindow ( hwndCombo, nFunsterStil );
    ShowWindow ( hwndButtonRecheck, nFunsterStil );
    
    hwndButtonSave = CreateWindow( "BUTTON", "Save Settings", 
                 WS_CHILD|WS_VISIBLE, 
                 265-73, 555+28, 110, 25, 
                 hwnd, (HMENU)IDC_BUTTON_SAVE, hThisInstance, NULL);
    
    SendMessage(hwndButtonSave, WM_SETFONT, (WPARAM)hFont2, FALSE);
    
    hwndButtonApply = CreateWindow( "BUTTON", "Apply", 
                 WS_CHILD|WS_VISIBLE, 
                 334, 555+28, 80, 25, 
                 hwnd, (HMENU)IDC_BUTTON_APPLY, hThisInstance, NULL);
    
    SendMessage(hwndButtonApply, WM_SETFONT, (WPARAM)hFont2, FALSE);
    
    hwndButtonStop = CreateWindow( "BUTTON", "Stop", 
                 WS_CHILD|WS_VISIBLE, 
                 510+40, 555+28, 80, 25, 
                 hwnd, (HMENU)IDC_BUTTON_STOP, hThisInstance, NULL);
    
    SendMessage(hwndButtonStop, WM_SETFONT, (WPARAM)hFont2, FALSE);
    
    hwndButtonReset = CreateWindow("BUTTON","Reset", 
                 WS_CHILD|WS_VISIBLE, 
                 380+63, 555+28, 80, 25, 
                 hwnd, (HMENU)IDC_BUTTON_RESET, hThisInstance, NULL);
    
    SendMessage(hwndButtonReset, WM_SETFONT, (WPARAM)hFont2, FALSE);
    
    ShowWindow (hwndButtonApply, nFunsterStil);
    ShowWindow (hwndButtonReset, nFunsterStil);
    ShowWindow (hwndButtonSave, nFunsterStil);
    ShowWindow (hwndButtonStop, nFunsterStil);
    
    checkControl();
    reset();
    loadSettings();
    joyInfo.dwFlags = JOY_RETURNALL;
    hndlThread = CreateThread(NULL, 0 , test, hwnd, 0, &th_id);
    
    while ( GetMessage (&messages, NULL, 0, 0) ){
            
            TranslateMessage(&messages); // Translate virtual-key messages into character messages
            DispatchMessage(&messages); // Send message to WindowProcedure
            
    }
    
    return messages.wParam; // The program return-value is 0 - The value that PostQuitMessage() gave
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{       
     
    UINT wEvent;
    switch (message)                  
    {
           
        case WM_CREATE:
//             IDD_DIALOG_HELP
             hDialog = CreateDialog( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, ToolDlgProc);
             ShowWindow ( hDialog, SW_HIDE );
             
             hDialogHelp = CreateDialog( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_HELP), hwnd, ToolDlgProcHelp);
             ShowWindow ( hDialogHelp, SW_HIDE );
             
             hDialog2 = CreateDialog( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, ToolDlgProc2);
             ShowWindow ( hDialog2, SW_SHOW);
             
             hDialogKeybrd = CreateDialog( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG3), hwnd, ToolDlgProc3);
             ShowWindow ( hDialogKeybrd, SW_HIDE );
                  
             if( hDialog == NULL || hDialog2 == NULL || hDialogKeybrd == NULL)
                 MessageBox(hwnd, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);   
             
             for( int y=0; y<24; y++){
                  hTemp = GetDlgItem( hDialogKeybrd, IDC_EDIT11+y );
                  DefEditProc = (WNDPROC)SetWindowLong(hTemp, GWL_WNDPROC, (long)EditProc);
             }

             break;
             
        case WM_COMMAND:
              
              
              switch( HIWORD( wParam ) ) {
		              case CBN_SELCHANGE:
                            {
                             currControl = list[ SendMessage( hwndCombo, CB_GETCURSEL, 0, 0) ];
                              if ( currControl != 5 ){
                                    joyGetDevCaps( currControl, &joyCaps, sizeof(joyCaps));
                                    if( joyCaps.wNumAxes == 5)
                                        xbox = TRUE;      
                                    else
                                        xbox = FALSE;
                             }
                             
                                    
                             reset();
                             loadSettings();
                        }
                        break;          
                }
                
             switch (LOWORD(wParam)){
                         
                    case IDM_SHOW1:
                         ShowWindow(hDialog, SW_SHOW);
                         break;
                         
                    case IDM_ABOUT:
                         ShowWindow(hDialogHelp, SW_SHOW);
                         break;
                         
                    case IDM_USAGE:
                         MessageBox(hwnd, "Checkout the Readme File included with the Program Folder", "Notification", MB_OK | MB_ICONINFORMATION);   
                         break;
                    
                    case IDC_BUTTON_SAVE:
                         saveSettings( hwnd );
                         break;
                    
                    case IDC_BUTTON_APPLY:
                         checkControl();
                         if( currControl == 5 )
                             MessageBox(hwnd, "No Device Detected", "Warning", MB_OK | MB_ICONINFORMATION);   
                             
                         else{
                             close = FALSE;
                        
                             if( close2 == FALSE){
                                    close2 = TRUE;
                                //WaitForSingleObject(hndlThread, 200);
                
                                    hndlThreadMain = CreateThread(NULL, 0 , testMain, hwnd, 0, &th_idMain);
                            }
                            
                            else
                            MessageBox(hwnd, "Already Running", "Warning", MB_OK | MB_ICONINFORMATION);
                        }
                        break;
                         
                    case IDC_BUTTON_STOP:
                         checkControl();
                         if( currControl == 5 )
                             MessageBox(hwnd, "No Device Detected", "Warning", MB_OK | MB_ICONINFORMATION);   
                         
                         else{
                             if( close2 == TRUE)
                                 close2 = FALSE;
                          
                             if( close == FALSE ){
                                    close = TRUE;
                                     
                                    //WaitForSingleObject(hndlThreadMain, 0); 
                                    hndlThread = CreateThread(NULL, 0 , test, hwnd, 0, &th_id);
                            }
                        }
                        break;
                         
                    case IDC_BUTTON_RESET:
                         reset();
                         break;
                    
                    case IDC_BUTTON_RECHECK:
                         checkControl();
                         loadSettings();
                         if( close2 == FALSE && close == FALSE ){
                                close = TRUE;
                                 
                                //WaitForSingleObject(hndlThreadMain, 0); 
                                hndlThread = CreateThread(NULL, 0 , test, hwnd, 0, &th_id);
                         }
                         break;
                    
                }
             break;
             
        case WM_MOVE:
             
             GetWindowRect(hwnd, &wndPosRec);
             SetWindowPos(hDialog2, HWND_TOP, wndPosRec.left + 10, wndPosRec.top + 138, 0, 0, SWP_NOSIZE);
             SetWindowPos(hDialogKeybrd, HWND_TOP, wndPosRec.left + 10, wndPosRec.top + 138, 0, 0, SWP_NOSIZE);
             break;
        
        case WM_NOTIFY:
             
                 wEvent = ((LPNMHDR)lParam)->code; 
                 switch( wEvent){
                         
                         case TCN_SELCHANGE:
                              sel = (++sel)% 2;
                              
                              if( sel == 0){
                                  ShowWindow(hDialogKeybrd, SW_HIDE);
                                  ShowWindow(hDialog2, SW_SHOW);
                              }
                              
                              else{
                                  ShowWindow(hDialog2, SW_HIDE);
                                  ShowWindow(hDialogKeybrd, SW_SHOW);
                              }
                              break;
                 
                 }
                 break;
                 
        case WM_DESTROY:
                 
                 if( hndlThread != NULL )
                     WaitForSingleObject(hndlThread, 0);
        
                 if( hndlThreadMain != NULL ){
                     close2 = FALSE;
                     WaitForSingleObject(hndlThread, 0);
                }
                 
                 DestroyWindow(hDialog);
                 DestroyWindow(hDialogHelp);
                 DestroyWindow(hDialog2);
                 DestroyWindow(hDialogKeybrd);
                 DestroyWindow(hwndTabControl);
                 
                 SetWindowLong(hTemp, GWL_WNDPROC, (long)DefEditProc);
                 
                 DeleteObject(hFont2);
                 DeleteObject(g_hbrBackground);
                 DeleteObject(staticBackground);
                 
                 PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
                 break;
        
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    
    }

    return 0;
}


BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
     
     switch( Message ){
                case WM_COMMAND:
              
                    switch (LOWORD(wParam)){
                            case IDC_UP:
                                if( sens < 5 ){
                                    sens++;
                                    SetDlgItemText( hwnd, IDC_STATIC_LAST, sens_char[sens-1]);
                                }
                                break;
                            
                            case IDC_DOWN:
                                if( sens > 1 ){
                                    sens--;
                                    SetDlgItemText( hwnd, IDC_STATIC_LAST, sens_char[sens-1]);
                                }
                                break;    
                    }
                    break;
                    
                 case WM_CLOSE:
                    ShowWindow( hwnd, SW_HIDE);
                    
                 default:
                        return FALSE;
             
    }
     
     return TRUE;
}

BOOL CALLBACK ToolDlgProcHelp(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
        switch( Message ){
              case WM_CLOSE:
                    ShowWindow( hwnd, SW_HIDE);
              
              default :
                    return FALSE;
             }
     
     return TRUE;
}
BOOL CALLBACK ToolDlgProc2(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
      
     switch( Message ){
             
             case WM_COMMAND:
                  switch( LOWORD(wParam)){
                         
                         case IDC_CHECKBOX1:
                               if ( HIWORD( wParam ) == BN_CLICKED ){
                                    if( joyCaps.wNumAxes > 1 ){
                                    if( axis1Check == TRUE )
                                        axis1Check = FALSE;
                                        
                                    else{
                                        axis1Check = TRUE;
                                        mouseMap[9] = 1;
                                        for( int i=0; i<4; i++){
                                             SetDlgItemText( hDialogKeybrd, IDC_EDIT27+i, "");
                                             keybrdMap[i+16] = 0;
                                        }
                                    }
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX2, BM_SETCHECK, BST_UNCHECKED, 0);
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX3, BM_SETCHECK, BST_UNCHECKED, 0);
                               }
                               else
                                   SendDlgItemMessage(hwnd, IDC_CHECKBOX1, BM_SETCHECK, BST_UNCHECKED, 0);
                               }
                          break;
                          
                          case IDC_CHECKBOX2:
                               if ( HIWORD( wParam ) == BN_CLICKED ){
                                    if( joyCaps.wNumAxes > 3 ){
                                    if( axis2Check == TRUE )
                                        axis2Check = FALSE;
                                    else{
                                        axis2Check = TRUE;
                                        mouseMap[9] = 2;
                                        for( int i=0; i<4; i++){
                                             SetDlgItemText( hDialogKeybrd, IDC_EDIT31+i, "");
                                             keybrdMap[i+20] = 0;
                                        }
                                    }
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX1, BM_SETCHECK, BST_UNCHECKED, 0);
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX3, BM_SETCHECK, BST_UNCHECKED, 0);
                                    }
                                    else
                                        SendDlgItemMessage(hwnd, IDC_CHECKBOX2, BM_SETCHECK, BST_UNCHECKED, 0);
                               }
                          break;
                          
                          case IDC_CHECKBOX3:
                               if ( HIWORD( wParam ) == BN_CLICKED ){
                                    if( buttons == TRUE ){
                                        buttons = FALSE;
                                        for( int i=0; i<4; i++){
                                             SetDlgItemText( hDialog2, IDC_EDIT2+i, "");
                                             mouseMap[i] = 0;
                                        }
                                    }
                                    else{
                                        buttons = TRUE;
                                        mouseMap[9] = 3;
                                    }
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX2, BM_SETCHECK, BST_UNCHECKED, 0);
                                    SendDlgItemMessage(hwnd, IDC_CHECKBOX1, BM_SETCHECK, BST_UNCHECKED, 0);
                               }
                          break;
                          
                          case IDC_EDIT2:
                          case IDC_EDIT3:
                          case IDC_EDIT4:
                          case IDC_EDIT5:
                          case IDC_EDIT6:
                          case IDC_EDIT7:
                          case IDC_EDIT8:
                          case IDC_EDIT9: 
                          case IDC_EDIT10:                    
                               
                               if( LOWORD(wParam)<40013 && buttons == FALSE ){
                                   focusOn = -1;
                                   break;
                               }    
                               if ( HIWORD( wParam ) == EN_SETFOCUS )
                                    focusOn = LOWORD(wParam);
                               
                               else if( HIWORD( wParam ) == EN_KILLFOCUS ){
                                    focusOn = -1;
                                    temp89 = -2;
                               }
                               break;
                        
                                    
                  }
                  break;
                              
            
             case WM_CTLCOLORDLG:
                         return (LONG)g_hbrBackground;
     
             case WM_CTLCOLORSTATIC:
                           {
                            hdcStatic = (HDC)wParam;
                            if( IDC_STATIC54 == GetDlgCtrlID((HWND)lParam) && on2 == TRUE && axismark == 1){
                                SetBkColor(hdcStatic, RGB(255,0,0));
                                return (LONG)staticBackground;
                            } //if
                            
                            else if( IDC_STATIC55 == GetDlgCtrlID((HWND)lParam) && on2 == TRUE && axismark == 2 ){
                                 SetBkColor(hdcStatic, RGB(255,0,0));
                                 return (LONG)staticBackground;
                            } //else if
                            
                            else{
                                     SetBkMode(hdcStatic, TRANSPARENT);
                                     return (LONG)g_hbrBackground;
                            } //else
                           }                        
     } //switch
     
     return FALSE;
}

BOOL CALLBACK ToolDlgProc3(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
      
     switch( Message ){
     
             case WM_CTLCOLORDLG:
                         return (LONG)g_hbrBackground;
     
             case JOYBUTTONPRESS1:
                  
                  pressed = (int)wParam -1;
                  SendDlgItemMessage(hDialogKeybrd, IDC_STATIC2+pressed, WM_SETFONT, (WPARAM)hFont2, TRUE);
                  break;
             
              case JOYBUTTONPRESS:
                  
                  pressed = -1;
                  SendDlgItemMessage(hDialogKeybrd, IDC_STATIC2+(int)wParam-1, WM_SETFONT, (WPARAM)hFont2, TRUE);
                  break;      
             
             case WM_CTLCOLORSTATIC:
                            {
                            hdcStatic = (HDC)wParam;
                            if( pressed == -1 ){
                                SetBkMode(hdcStatic, TRANSPARENT);
                                return (LONG)g_hbrBackground;
                            }
                            
                            else if( (IDC_STATIC2+pressed) == GetDlgCtrlID((HWND)lParam) ){
                                     SetBkColor(hdcStatic, RGB(255,0,0));
                                     return (LONG)staticBackground;
                            }
                            }
     }
     
     return FALSE;
}

void checkControl() {
     
     iter = 0;
     i = 0;
     joyInfo.dwSize = sizeof( joyInfo );
     list[0] = list[2] = list[2] = 5; 
     
     SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0); // Selective delete - CB_DELETESTRING
     while( iter < 3 ){
                    
                    if( joyGetPosEx( joyID[iter], &joyInfo ) )
                        iter++;
                    
                    else{
                         SendMessage( hwndCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>( (LPCTSTR)buf2[iter]) );
                         SendMessage( hwndCombo, CB_SETCURSEL, 0, i);  
                         currControl = list[i] = joyID[iter];
                         i++;  iter++;
                    }    
             }
             
     if( iter == 3 && i ==0 ){
         currControl = 5;
         SendMessage( hwndCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)buf3) );
         SendMessage( hwndCombo, CB_SETCURSEL, 0, 0 ); // set current selection
     }
     
    
    if ( currControl != 5 ){
       if(start1 == TRUE )
            currControl = list[0];
       
       start1 = FALSE;
       joyGetDevCaps( currControl, &joyCaps, sizeof(joyCaps));
       if( joyCaps.wNumAxes == 5)
            xbox = TRUE;  
       else
           xbox = FALSE;    
    }  
}               

int getButton(JOYINFOEX joy){
    int inp = joy.dwButtons;
    int povinp;
    
    JOYCAPS joy23;
    joyGetDevCaps(currControl, &joy23, sizeof(joy23));

    if( joy23.wCaps & JOYCAPS_HASPOV)
        povinp = joy.dwPOV;
    else
        povinp = 40000;
    
    if( xbox == TRUE ){
        if( joy.dwZpos > Zmax )
            return 9;
             
        else if( joy.dwZpos < Zmin ){
            return 10;
        }
    }
    
    if ( inp == 0 ){
         if( povinp <32000 ){
             return (13 + (int)(povinp/9000));     
         }
    }
    else{
        for( int j = 1; j<=12; j++){
         if( inp & 0x01 )
             return j;
         else 
              inp = inp >> 1;             
         }
    }

    return 0;
}

void reset(){
     for( int i=0; i<24; i++)
          SetDlgItemText( hDialogKeybrd, IDC_EDIT11+i, "");     
     
     for( int i=0; i<8; i++)
          SetDlgItemText( hDialog2, IDC_EDIT2+i, "");
          
     SendDlgItemMessage(hDialog2, IDC_CHECKBOX1, BM_SETCHECK, BST_UNCHECKED, 0);     
     SendDlgItemMessage(hDialog2, IDC_CHECKBOX2, BM_SETCHECK, BST_UNCHECKED, 0);
     SendDlgItemMessage(hDialog2, IDC_CHECKBOX3, BM_SETCHECK, BST_UNCHECKED, 0);
     
     for( int i=0; i<24; i++)
          keybrdMap[i] = 0;
          
     for( int i=0; i<10; i++)
          mouseMap[i] = 0;
}

void saveSettings( HWND hwnd ){
     BOOL overwrite = FALSE;
     joyGetDevCaps( currControl, &joyCaps, sizeof(joyCaps));
     if( currControl != 5 ){
         file = fopen("settings.gol", "rb");
         if ( file !=NULL ){
              fread(&temp45, sizeof(temp45), 1, file);
              if( temp45.id == joyCaps.wPid ){
                  
                  for( int n=0; n<24; n++)
                       temp45.keybrd[n] = keybrdMap[n];
                  for( int n=0; n<10; n++)
                       temp45.mouse[n] = mouseMap[n];
              }
              lList( 1 );
              overwrite = TRUE;
     
              while( fread(&temp45, sizeof(temp45), 1, file) ) {
                     if( temp45.id == joyCaps.wPid ){
                  
                         for( int n=0; n<24; n++)
                              temp45.keybrd[n] = keybrdMap[n];
                         for( int n=0; n<10; n++)
                              temp45.mouse[n] = mouseMap[n];
                     }
                     lList();
              }
              fclose(file);     
         }
         
         file = fopen( "settings.gol", "wb+");
         if( overwrite == TRUE ){
             curr = &start;
             file = fopen( "settings.gol", "wb");
             while( curr != NULL ){
                    temp45 = curr->inst;
                    fwrite( &temp45, sizeof(temp45), 1, file);
                    curr = curr->next;
             }
             fclose(file);
         }
         else{
              file = fopen( "settings.gol", "ab+");
              
              temp45.id = joyCaps.wPid;
              for( int n=0; n<24; n++)
                   temp45.keybrd[n] = keybrdMap[n];
              for( int n=0; n<10; n++)
                   temp45.mouse[n] = mouseMap[n];
                   
              fwrite( &temp45, sizeof(temp45), 1, file);
              fclose( file );
         }
     }
     else
         MessageBox(hwnd, "No Device Detected", "Warning!", MB_OK | MB_ICONINFORMATION);
}


void loadSettings(){
     if( currControl !=5){
         file = fopen("settings.gol", "rb");
         if( file != NULL ){
             while( fread(&temp45, sizeof(temp45), 1, file) ){
                     if( temp45.id == joyCaps.wPid ){
                         for( int y=0; y<24; y++)
                              keybrdMap[y] = temp45.keybrd[y];
                              
                         for( int y=0; y<10; y++)
                              mouseMap[y] = temp45.mouse[y];   
                         break;
                     }
             }//while
             fclose(file);
         
             //mouse
             for( int f=0; f<9; f++){
                   if( mouseMap[f] != 0){
                       if( mouseMap[f] < 13)
                           sprintf(a, "Button %d", mouseMap[f] );
                       else if( mouseMap[f] == 13)
                           sprintf(a, "POV UP");
                       else if( mouseMap[f] == 14)
                           sprintf(a, "POV RIGHT");
                       else if( mouseMap[f] == 15)
                           sprintf(a, "POV DOWN");
                       else if( mouseMap[f] == 16)
                           sprintf(a, "POV LEFT");
                       
                       const char* tree = a;   
                       SetDlgItemText( hDialog2, IDC_EDIT2+f, tree);
                   }     
                   else
                       SetDlgItemText( hDialogKeybrd, IDC_EDIT2+f, ""); 
             }
             
             if( mouseMap[9] == 1 )
                SendDlgItemMessage(hDialog2, IDC_CHECKBOX1, BM_SETCHECK, BST_CHECKED, 0);
             
             else if( mouseMap[9] == 2 )
                SendDlgItemMessage(hDialog2, IDC_CHECKBOX2, BM_SETCHECK, BST_CHECKED, 0);   
             
             else if( mouseMap[9] == 3 )
                SendDlgItemMessage(hDialog2, IDC_CHECKBOX3, BM_SETCHECK, BST_CHECKED, 0);
             
             //keyboard
             for( int f=0; f<24; f++){
                  for( g=0; g<128; g++){
                          if( keybrdMap[f] == keysInst[g].vkCode){
                              SetDlgItemText( hDialogKeybrd, IDC_EDIT11+f, keysInst[g].name );
                              break;
                          }
                     }
                     if( g == 128){
                         sprintf(abe, "%c", keybrdMap[f]);
                         const char* l = abe;
                         SetDlgItemText( hDialogKeybrd, IDC_EDIT11+f, l );
                     }
             }
              
         } //file if
     } //control if
}

DWORD WINAPI test(LPVOID param){
    
    int temp = 0 , on =1, xmax=0, xmin=0;
    axismark = 0, on2 = TRUE;
    int but, prevControl = currControl;
    BOOL ok, keybrdOK, axisOK;
    
    while( close == TRUE ){
           if( currControl != 5){
               prevControl = currControl;
        joyGetDevCaps( currControl, &joyCaps, sizeof(joyCaps));
        xmax = ( (joyCaps.wXmax+joyCaps.wXmin)/2 ) + 3000;
        xmin = xmax - 6000;
        
        if ( joyCaps.wNumAxes > 3 ){
            
             Zmax = ( (joyCaps.wRmax+joyCaps.wRmin)/2 ) + 3000;
             Zmin = Zmax - 6000;
            }  
        
    
        
        while( currControl == prevControl && close == TRUE){
            //prevControl = currControl;   
            joyGetPosEx( currControl, &joyInfo );
            but = getButton( joyInfo);
                   
            if( sel==0 ){
                
                if( joyInfo.dwXpos > xmax || joyInfo.dwXpos < xmin){
                    if( on2 == TRUE ){
                        axismark = 1;
                        SendDlgItemMessage(hDialog2, IDC_STATIC54, WM_SETFONT, (WPARAM)hFont2, TRUE);
                        on2 = FALSE;
                    }
                                    
                }
                
                else if ( xbox == FALSE && (joyInfo.dwZpos > Zmax || joyInfo.dwZpos < Zmin) ){
                     if( on2 == TRUE ){
                        axismark = 2;
                        SendDlgItemMessage(hDialog2, IDC_STATIC55, WM_SETFONT, (WPARAM)hFont2, TRUE);
                        on2 = FALSE;
                    }
                }
                
                else if( xbox == TRUE && (joyInfo.dwRpos > Zmax || joyInfo.dwRpos < Zmin) ){
                     if( on2 == TRUE ){
                        axismark = 2;
                        SendDlgItemMessage(hDialog2, IDC_STATIC55, WM_SETFONT, (WPARAM)hFont2, TRUE);
                        on2 = FALSE;
                    }
                }
                
                else{
                     if( on2 == FALSE ){
                         if( axismark == 1)
                             SendDlgItemMessage(hDialog2, IDC_STATIC54, WM_SETFONT, (WPARAM)hFont2, TRUE);
                         else if( axismark ==2 )
                                  SendDlgItemMessage(hDialog2, IDC_STATIC55, WM_SETFONT, (WPARAM)hFont2, TRUE);
                     }
                     on2 = TRUE;
                     axismark = 0;
                     
                }
            }
        
            if( but ){
                if( sel==1 ){
                    if( on == 1 ){
                        temp = but;
                        SendMessage( hDialogKeybrd, JOYBUTTONPRESS1, (WPARAM)temp, 0 );
                    }
                    on=0;
                }
                else {
                     if( keybrdMap[but - 1] != 0 )
                         keybrdOK = FALSE;
                     else
                         keybrdOK = TRUE ;
                       
                     temp90 = focusOn;
                     if( temp90 != -1 && keybrdOK == TRUE){
                         
                         if( temp89 != but || temp89 == -2 ){
                             for( int g=0; g<9; g++){
                                  if( mouseMap[g] == but ){
                                      ok = FALSE;
                                      break;
                                  }
                                  else
                                      ok = TRUE;
                             }
                             
                             if( ok == TRUE ){
                                 if( but< 13 )
                                     sprintf(a, "Button %d", but );
                                 else{
                                      if( but == 13 )
                                          sprintf(a, "POV UP");
                                      else if( but == 14 )
                                          sprintf(a, "POV RIGHT");
                                      else if( but == 15 )
                                          sprintf(a, "POV DOWN");
                                      else if( but == 16 )
                                          sprintf(a, "POV LEFT");
                                 }
                                 const char* ag = a;
                                 SetDlgItemText( hDialog2, temp90, ag);
                                 mouseMap[temp90 - 40009] = but;
                             }
                             else{
                                  SetDlgItemText( hDialog2, temp90, "");
                                  mouseMap[temp90 - 40009] = 0;
                             }     
                         }
                         temp89 = but;
                     }//temp90 if
                }//else
                
            }//if
          
            else if( temp!=0){
                 if(sel==1){
                 SendMessage( hDialogKeybrd, JOYBUTTONPRESS, (WPARAM)temp, 0 );
                 temp=0; on=1;
                 }
            }
        }//inner while
    } //if
    
}//outer while  

}//function


LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int ID = GetDlgCtrlID( hwnd );
    BOOL keyok = TRUE;
    
    switch (uMsg)
    {
        
        case WM_KEYDOWN:{
             
             for( int m=0; m<9; m++){
                 if( mouseMap[m] == ID-40017 ){
                     keyok = FALSE;
                     break;
                 }
             }
             if( keyok==FALSE)
                 break;            
             
             for( int f=0; f<24; f++){
                  if( keybrdMap[f] == (unsigned char)wParam ){
                      keyok = FALSE;
                      if( ID == 40018 + f){
                          SetDlgItemText( hDialogKeybrd, ID, "" );
                          keybrdMap[ID - 40018] = 0;
                      }
                      break;    
                  }
             }
             if( keyok == TRUE ){
                 if( (ID>IDC_EDIT26 && ID<IDC_EDIT31 && joyCaps.wNumAxes<2) || (ID>IDC_EDIT26 && ID<IDC_EDIT31 && axis1Check == TRUE) )
                     break;   
                 
                 else if( (ID>IDC_EDIT30 && joyCaps.wNumAxes<4) || (ID>IDC_EDIT30 && axis2Check == TRUE) )
                      break;
                      
                 else{
                     for( g=0; g<128; g++){
                          if( (unsigned char)wParam == keysInst[g].vkCode){
                              SetDlgItemText( hDialogKeybrd, ID, keysInst[g].name );
                              break;
                          }
                     }
                     if( g == 128){
                         sprintf(abe, "%c", (char)wParam);
                         const char* f = abe;
                         SetDlgItemText( hDialogKeybrd, ID, f );
                     }
                     keybrdMap[ID - 40018] = (unsigned char)wParam;
                 }
             }
             break;
                       }//case
                       
        default:
                return CallWindowProc(DefEditProc, hwnd, uMsg, wParam, lParam);
 
    }
 
    return FALSE;
}

DWORD WINAPI testMain(LPVOID param){
    
    joyThread joyThreadObj(currControl, keybrdMap, mouseMap, sens, xbox);
    joyThreadObj.mainThread();
    close2 = FALSE;
}

