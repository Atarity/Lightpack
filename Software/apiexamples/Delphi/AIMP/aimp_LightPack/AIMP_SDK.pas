unit AIMP_SDK;

{************************************************}
{*                                              *}
{*            AIMP2: SDK (02.07.2009)           *}
{*              AIMP Version: 2.60              *}
{*              (c) Artem Izmaylov              *}
{*                 www.aimp.ru                  *}
{*            MAIL: support@aimp.ru             *}
{*              ICQ: 345-908-513                *}
{*                                              *}
{************************************************}

interface

uses
  Windows, Messages;   

const
  AIMP_PLUGIN_NAME   = 1;
  AIMP_PLUGIN_INFO   = 2;
  AIMP_PLUGIN_AUTHOR = 3;
  AIMP_PLUGIN_EXTS   = 4;

//==============================================================================
// R E M O T E    I N F O
//==============================================================================

const
  AIMP2_RemoteClass = 'AIMP2_RemoteInfo';
  AIMP2_RemoteFileSize = 2048;
  WM_AIMP_COMMAND = WM_USER + $75;
  // WParam = One of Command, LPARAM - Parameter

  // see parameters for AIMP_Status_Get/AIMP_Status_Set
  WM_AIMP_STATUS_GET    = 1;
  WM_AIMP_STATUS_SET    = 2; {HiWord of LParam - Command, LoWord of LParam - Parameter }
  WM_AIMP_CALLFUNC      = 3; { LPARAM - Func ID (see below Func ID for Addons) }
  WM_AIMP_GET_VERSION   = 4;
  WM_AIMP_STATUS_CHANGE = 5;

//==============================================================================
// A D D O N S
//==============================================================================

type
  PAIMP2FileInfo = ^TAIMP2FileInfo;
  TAIMP2FileInfo = packed record
    cbSelfSize: DWORD;
    //
    nActive: LongBool;
    nBitRate: DWORD;
    nChannels: DWORD;
    nDuration: DWORD;
    nFileSize: Int64;
    nRating: DWORD;
    nSampleRate: DWORD;
    nTrackID: DWORD;
    //
    nAlbumLen: DWORD;
    nArtistLen: DWORD;
    nDateLen: DWORD;
    nFileNameLen: DWORD;
    nGenreLen: DWORD;
    nTitleLen: DWORD;
    //
    sAlbum: PWideChar;
    sArtist: PWideChar;
    sDate: PWideChar;
    sFileName: PWideChar;
    sGenre: PWideChar;
    sTitle: PWideChar;
  end;

const
  WM_LANG = WM_USER + 101;

  // CallBack types
  AIMP_STATUS_CHANGE     = 1;
  AIMP_PLAY_FILE         = 2;
  AIMP_INFO_UPDATE       = 5;  // Update Info for current track
  AIMP_PLAYER_STATE      = 11; // Play/Pause/Stop
  AIMP_EFFECT_CHANGED    = 12; // Balance, Speed, Tempo, Pitch, Flanger and etc.
  AIMP_EQ_CHANGED        = 13; // Sliders changed
  AIMP_TRACK_POS_CHANGED = 14;

  // AIMP_Status_Set / AIMP_Status_Get
  AIMP_STS_VOLUME      = 1;
  AIMP_STS_BALANCE     = 2;
  AIMP_STS_SPEED       = 3;
  AIMP_STS_PLAYER      = 4;
  AIMP_STS_MUTE        = 5;
  AIMP_STS_REVERB      = 6;
  AIMP_STS_ECHO        = 7;
  AIMP_STS_CHORUS      = 8;
  AIMP_STS_Flanger     = 9;

  AIMP_STS_EQ_STS      = 10;
  AIMP_STS_EQ_SLDR01   = 11;
  AIMP_STS_EQ_SLDR02   = 12;
  AIMP_STS_EQ_SLDR03   = 13;
  AIMP_STS_EQ_SLDR04   = 14;
  AIMP_STS_EQ_SLDR05   = 15;
  AIMP_STS_EQ_SLDR06   = 16;
  AIMP_STS_EQ_SLDR07   = 17;
  AIMP_STS_EQ_SLDR08   = 18;
  AIMP_STS_EQ_SLDR09   = 19;
  AIMP_STS_EQ_SLDR10   = 20;
  AIMP_STS_EQ_SLDR11   = 21;
  AIMP_STS_EQ_SLDR12   = 22;
  AIMP_STS_EQ_SLDR13   = 23;
  AIMP_STS_EQ_SLDR14   = 24;
  AIMP_STS_EQ_SLDR15   = 25;
  AIMP_STS_EQ_SLDR16   = 26;
  AIMP_STS_EQ_SLDR17   = 27;
  AIMP_STS_EQ_SLDR18   = 28;

  AIMP_STS_REPEAT      = 29;
  AIMP_STS_ON_TOPMOST  = 30;
  AIMP_STS_POS         = 31;
  AIMP_STS_LENGTH      = 32;
  AIMP_STS_REPEATPLS   = 33;
  AIMP_STS_REP_PLS_1   = 34;
  AIMP_STS_KBPS        = 35;
  AIMP_STS_KHZ         = 36;
  AIMP_STS_MODE        = 37;
  AIMP_STS_RADIO_CAP   = 38;
  AIMP_STS_STREAM_TYPE = 39; // Music / CDA / Radio
  AIMP_STS_TIMER       = 40; // Reverse / Normal
  AIMP_STS_SHUFFLE     = 41;
  
  AIMP_STS_MAIN_HWND   = 42;
  AIMP_STS_TC_HWND     = 43;
  AIMP_STS_APP_HWND    = 44;
  AIMP_STS_PL_HWND     = 45;
  AIMP_STS_EQ_HWND     = 46;
  
  AIMP_STS_TRAY        = 47;

  // Support Exts Flags
  AIMP_PLS_EXTS    = 1;
  AIMP_AUDIO_EXTS  = 2;

  // Menu IDs
  AIMP_MAIN_MENU_OPN   = 0;
  AIMP_MAIN_MENU_FNC   = 2;
  AIMP_MAIN_MENU_CFG   = 3;
  AIMP_UTILS_MENU      = 4;
  AIMP_PLS_MENU_ADD    = 5;
  AIMP_PLS_MENU_JUMP   = 6;
  AIMP_PLS_MENU_FNC    = 7;
  AIMP_PLS_MENU_SEND   = 8;
  AIMP_PLS_MENU_DEL    = 9;
  AIMP_ADD_MENU        = 10;
  AIMP_DEL_MENU        = 11;
  AIMP_SRT_MENU        = 13;
  AIMP_MSC_MENU        = 14;
  AIMP_PLS_MENU        = 15;
  AIMP_TRAY            = 17;
  AIMP_EQ_LIB          = 18;
  // use AIMP_UTILS_MENU overthis:
  // + AIMP_MAIN_MENU_UTILS = 1;
  // + AIMP_TRAY_UTILS      = 16;

  // AIMP_CallFunction
  AIMP_OPEN_FILES      = 0;
  AIMP_OPEN_DIR        = 1;
  AIMP_ABOUT           = 2;
  AIMP_SLEEP_TIMER     = 3;
  AIMP_UTILS_AC        = 4;
  AIMP_UTILS_SR        = 5;
  AIMP_UTILS_TE        = 6;
  AIMP_UTILS_CDB       = 7;
  AIMP_OPTIONS         = 8;
  AIMP_PLUGINS         = 9;
  AIMP_QUIT            = 10;
  AIMP_NEXT_VIS        = 11;
  AIMP_PREV_VIS        = 12;
  AIMP_EQ_ANALOG       = 13;
  AIMP_TO_TRAY         = 14;
  AIMP_PLAY            = 15;
  AIMP_PAUSE           = 16;
  AIMP_STOP            = 17;
  AIMP_NEXT            = 18;
  AIMP_PREV            = 19;
  AIMP_ADD_FILES       = 20;
  AIMP_ADD_DIR         = 21;
  AIMP_ADD_PLS         = 22;
  AIMP_ADD_URL         = 23;
  AIMP_DEL_FILES       = 24;
  AIMP_DEL_BAD         = 25;
  AIMP_DEL_FROMHDD     = 26;
  AIMP_DEL_OFF         = 27;
  AIMP_DEL_OFF_HDD     = 28;
  AIMP_RESCAN_PLS      = 29;
  AIMP_SHOW_CURFILE    = 30;
  AIMP_SORT_INVERT     = 31;
  AIMP_SORT_RANDOM     = 32;
  AIMP_SORT_TITLE      = 33;
  AIMP_SORT_ARTIST     = 34;
  AIMP_SORT_FOLDER     = 35;
  AIMP_SORT_LENGTH     = 36;
  AIMP_SORT_RATING     = 37;
  AIMP_SEARCH          = 38;
  AIMP_OPEN_PLS        = 39;
  AIMP_SAVE_PLS        = 40;
  AIMP_PLAY_LAST       = 41;
  AIMP_OFF_SELECTED    = 42;
  AIMP_ON_SELECTED     = 43;
  AIMP_ADD2BOOKMARK    = 44;
  AIMP_EDITBOOKMARK    = 45;


// note: For AIMP_GetPath
//  AIMP_CFG_DATA     = 0;
//  AIMP_CFG_PLS      = 1;
//  AIMP_CFG_LNG      = 2;
//  AIMP_CFG_SKINS    = 3;
//  AIMP_CFG_PLUGINS  = 4;
//  AIMP_CFG_ICONS    = 5;
//  AIMP_CFG_ML       = 6;
//  AIMP_CFG_ENC      = 7;
//  AIMP_CFG_MODULES  = 8;

  // For AIMP_QueryObject
  IAIMP2PlayerID             = $0001;
  IAIMP2PlaylistManagerID    = $0003;
  IAIMP2ExtendedID           = $0004;
  IAIMP2CoverArtManagerID    = $0005;
  IAIMP2PlaylistManager2ID   = $0006;
  IAIMPConfigFileID          = $0010;
  IAIMPLanguageFileID        = $0011;

  // For AIMP_ObjectClass
  AIMP_EXT_LC_MESSAGE = 100;

  // Option Frame Position Flags
  AIMP_FRAME_POS_PLAY     = 1;
  AIMP_FRAME_POS_PLAYLIST = 2;
  AIMP_FRAME_POS_PLAYER   = 3;
  AIMP_FRAME_POS_TEMPLATE = 4;
  AIMP_FRAME_POS_SYSTEM   = 5;
  AIMP_FRAME_POS_SKINS    = 6;
  AIMP_FRAME_POS_LANGS    = 7;

  // AIMP_PLS_SORT_TYPE_XXX
  AIMP_PLS_SORT_TYPE_TITLE      = 1;
  AIMP_PLS_SORT_TYPE_FILENAME   = 2;
  AIMP_PLS_SORT_TYPE_DURATION   = 3;
  AIMP_PLS_SORT_TYPE_ARTIST     = 4;
  AIMP_PLS_SORT_TYPE_INVERSE    = 5;
  AIMP_PLS_SORT_TYPE_RANDOMIZE  = 6;

type
  HPLS = Integer;
  HPLSTREAM = DWORD;
  HAIMPMENU = Pointer;

  // for Playlist Processing
  TAIMPPlaylistDeleteProc = function (AFileInfo: PAIMP2FileInfo; AUserData: DWORD): Boolean; stdcall;
  TAIMPPlaylistSortProc = function (AFileInfo1, AFileInfo2: PAIMP2FileInfo; AUserData: DWORD): Integer; stdcall;
  // for Menu CallBack
  TAIMPMenuProc = procedure (User: DWORD; Handle: Pointer); stdcall;
  // for Other CallBack Types
  TAIMPStatusChange = procedure (User, CallBackType: DWORD); stdcall;

  TAIMPPlaylistInfo = packed record
    PLSName: PAnsiChar;
    FileCount: DWORD;
    PLSDuration: DWORD;
    PLSSize: Int64;
    PlaylistID: HPLS;
  end;
  TPLSInfo = TAIMPPlaylistInfo;

  PAIMPMenuInfo = ^TAIMPMenuInfo;
  TAIMPMenuInfo = packed record
    CheckBox: Boolean;
    RadioItem: Boolean;
    Checked: Boolean;
    Enabled: Boolean;
    Proc: Pointer; // TAIMPMenuProc;
    Bitmap: HBITMAP; // 0 - no bmp
    Caption: PWideChar;
    User: DWORD;
  end;

  IPLSStrings = interface
    function AddFile(FileName: PWideChar; FileInfo: PAIMP2FileInfo): Boolean; stdcall;
    function DelFile(ID: Integer): Boolean; stdcall;
    function GetFileName(ID: Integer): PWideChar; stdcall;
    function GetFileInfo(ID: Integer; AInfo: PAIMP2FileInfo): Boolean; stdcall;
    function GetFileObj(ID: Integer): DWORD; stdcall;
    function GetCount: Integer; stdcall;
  end;

  { IAIMP2OptionFrame }

  // User option frame
  IAIMP2OptionFrame = interface
    function FrameCreate(AParent: HWND): HWND; stdcall;
    function FrameData: Pointer; stdcall; // reserved
    function FrameFlags: Integer; stdcall; // See FramePositionFlags
    function FrameName: PWideChar; stdcall;
    procedure FrameFree(AWindow: HWND); stdcall;
    procedure FrameLoadConfigNotify; stdcall;
    procedure FrameSaveConfigNotify; stdcall;
  end;

  { IAIMP2Player }

  // See IAIMPPlayerID
  IAIMP2Player = interface
    function Version: Integer; stdcall;
    function PlayTrack(ID: HPLS; ATrackIndex: Integer): Boolean; stdcall;
    procedure PlayOrResume; stdcall;
    procedure Pause; stdcall;
    procedure Stop; stdcall;
    procedure NextTrack; stdcall;
    procedure PrevTrack; stdcall;
  end;

  { IAIMP2PlaylistManager }

  // See IAIMP2PlaylistManagerID
  IAIMP2PlaylistManager = interface
    function AIMP_PLS_CreateFromFile(AFile: PWideChar; AActivate, AStartPlay: Boolean): HPLS; stdcall;
    function AIMP_PLS_ID_ActiveGet: HPLS; stdcall;
    function AIMP_PLS_ID_ActiveSet(ID: HPLS): Boolean; stdcall;
    function AIMP_PLS_ID_PlayingGet: HPLS; stdcall;
    function AIMP_PLS_ID_PlayingGetTrackIndex(ID: HPLS): Integer; stdcall;
    function AIMP_PLS_NewEx(AName: PWideChar; AActivate: Boolean): HPLS; stdcall;
    function AIMP_PLS_PlayFile(AFileName: PWideChar; AFailIfNotExists: Boolean): Boolean; stdcall;
    // Playlist Processing
    function AIMP_PLS_DeleteByFilter(ID: HPLS; AFilterProc: TAIMPPlaylistDeleteProc; AUserData: DWORD): Boolean; stdcall;
    function AIMP_PLS_SortByFilter(ID: HPLS; AFilterProc: TAIMPPlaylistSortProc; AUserData: DWORD): Boolean; stdcall;
    // Entries
    function AIMP_PLS_Entry_Delete(ID: HPLS; AEntryIndex: Integer): Boolean; stdcall;
    function AIMP_PLS_Entry_DeleteAll(ID: HPLS): Boolean; stdcall;
    function AIMP_PLS_Entry_FileNameGet(ID: HPLS; AEntryIndex: Integer; ABuf: PWideChar; ABufferSizeInChars: DWORD): Boolean; stdcall;
    function AIMP_PLS_Entry_FileNameSet(ID: HPLS; AEntryIndex: Integer; ABuf: PWideChar): Boolean; stdcall;
    function AIMP_PLS_Entry_FocusedGet(ID: HPLS): Integer; stdcall;
    function AIMP_PLS_Entry_FocusedSet(ID: HPLS; AEntryIndex: Integer): Boolean; stdcall;
    function AIMP_PLS_Entry_InfoGet(ID: HPLS; AEntryIndex: Integer; PFileInfo: PAIMP2FileInfo): Boolean; stdcall;
    function AIMP_PLS_Entry_InfoSet(ID: HPLS; AEntryIndex: Integer; PFileInfo: PAIMP2FileInfo): Boolean; stdcall;
    function AIMP_PLS_Entry_PlayingSwitchGet(ID: HPLS; AEntryIndex: Integer): Boolean; stdcall;
    function AIMP_PLS_Entry_PlayingSwitchSet(ID: HPLS; AEntryIndex: Integer; ASwitch: Boolean): Boolean; stdcall;
    function AIMP_PLS_Entry_ReloadInfo(ID: HPLS; AEntryIndex: Integer): Boolean; stdcall;
    // Load/Save Playlists
    function AIMP_PM_DestroyStream(AHandle: HPLSTREAM): Boolean; stdcall;
    function AIMP_PM_ReadItem(AHandle: HPLSTREAM; PItem: PAIMP2FileInfo): DWORD; stdcall;
    function AIMP_PM_ReadStream(AFileName: PWideChar; var Count: Integer): HPLSTREAM; stdcall;
    function AIMP_PM_SaveStream(AFileName: PWideChar): HPLSTREAM; stdcall;
    function AIMP_PM_WriteItem(AHandle: HPLSTREAM; PItem: PAIMP2FileInfo): DWORD; stdcall;
    // added in 2.50 B295
    function AIMP_PLS_ID_PlayingSetTrackIndex(ID: HPLS; AEntryIndex: Integer): Boolean; stdcall;
  end;

  { IAIMP2PlaylistManager2 }

  // See IAIMP2PlaylistManager2ID
  IAIMP2PlaylistManager2 = interface(IAIMP2PlaylistManager)
    // Count of loaded playlists
    function AIMP_PLS_Count: Word; stdcall;
    // Return = -1 - ID is not valid, otherthis - count of files in playlist 
    function AIMP_PLS_GetFilesCount(ID: HPLS): Integer; stdcall;
    function AIMP_PLS_GetInfo(ID: HPLS; out ADuration, ASize: Int64): HRESULT; stdcall;
    function AIMP_PLS_GetName(ID: HPLS; ABuffer: PWideChar; ABufferSizeInChars: Integer): HRESULT; stdcall;
    // Custom Sorting, see AIMP_PLS_SORT_TYPE_XXX
    function AIMP_PLS_Sort(ID: HPLS; ASortType: Integer): HRESULT; stdcall;
    function AIMP_PLS_SortByTemplate(ID: HPLS; ABuffer: PWideChar; ABufferSizeInChars: Integer): HRESULT; stdcall;
    // if Index = -1 returned ID of current playlist.
    function AIMP_PLS_ID_By_Index(Index: Integer; out ID: HPLS): HRESULT; stdcall;
    // Get Formated title for Entry
    function AIMP_PLS_Entry_GetTitle(ID: HPLS; AEntryIndex: Integer;
      ABuffer: PWideChar; ABufferSizeInChars: Integer): HRESULT; stdcall;
    // Set Entry to playback queue
    function AIMP_PLS_Entry_QueueRemove(ID: HPLS; AEntryIndex: Integer): HRESULT; stdcall;
    function AIMP_PLS_Entry_QueueSet(ID: HPLS; AEntryIndex: Integer; AInsertAtQueueBegining: LongBool): HRESULT; stdcall;
    // Moving Entry
    function AIMP_PLS_Entry_SetPosition(ID: HPLS; AEntryIndex, ANewEntryIndex: Integer): HRESULT; stdcall;
  end;

  { IAIMPConfigFile }

  // See IAIMPConfigFileID
  IAIMPConfigFile = interface
    // functions return null value, if value don't exists in configuration file
    function AIMP_Config_ReadString(ASectionName, AItemName, AValueBuffer: PWideChar;
      ASectionNameSizeInChars, AItemNameSizeInChars, AValueBufferSizeInChars: Integer): HRESULT; stdcall;
    function AIMP_Config_ReadInteger(ASectionName, AItemName: PWideChar;
      ASectionNameSizeInChars, AItemNameSizeInChars: Integer; out AValue: Integer): HRESULT; stdcall;
    //
    function AIMP_Config_WriteString(ASectionName, AItemName, AValueBuffer: PWideChar;
      ASectionNameSizeInChars, AItemNameSizeInChars, AValueBufferSizeInChars: Integer): HRESULT; stdcall;
    function AIMP_Config_WriteInteger(ASectionName, AItemName: PWideChar;
      ASectionNameSizeInChars, AItemNameSizeInChars: Integer; AValue: Integer): HRESULT; stdcall;
    //
    function AIMP_Config_IsSectionExists(ASectionName: PWideChar; ASectionNameSizeInChars: Integer): HRESULT; stdcall;
    function AIMP_Config_RemoveSection(ASectionName: PWideChar; ASectionNameSizeInChars: Integer): HRESULT; stdcall;
  end;

  { IAIMPLanguageFile }

  // See IAIMPLanguageFileID
  IAIMPLanguageFile = interface
    function AIMP_Lang_Version: Integer;
    function AIMP_Lang_CurrentFile(ABuffer: PWideChar; ABufferSizeInChars: Integer): Integer; stdcall;
    function AIMP_Lang_IsSectionExists(ASectionName: PWideChar; ASectionNameSizeInChars: Integer): HRESULT; stdcall;
    function AIMP_Lang_ReadString(ASectionName, AItemName, AValueBuffer: PWideChar;
      ASectionNameSizeInChars, AItemNameSizeInChars, AValueBufferSizeInChars: Integer): HRESULT; stdcall;
    // When Language changed AIMP will send to window handle "WM_LANG" message
    function AIMP_Lang_Notification(AWndHandle: HWND; ARegister: LongBool): HRESULT; stdcall;
  end;

  { IAIMP2Extended }

  // See IAIMP2ExtendedID
  IAIMP2Extended = interface
    function AIMP_GetPath(ID: Integer; ABuffer: PWideChar; ABufferSizeInChars: Integer): Integer; stdcall;
    function AIMP_ObjectClass(ID: DWORD; AData: Pointer; ARegister: Boolean): Boolean; stdcall;
    // User Option Dialogs
    function AIMP_Options_FrameAdd(AFrame: IAIMP2OptionFrame): DWORD; stdcall;
    function AIMP_Options_FrameRemove(AFrame: IAIMP2OptionFrame): DWORD; stdcall;
    function AIMP_Options_ModifiedChanged(AFrame: IAIMP2OptionFrame): DWORD; stdcall;
  end;

  { IAIMP2CoverArtManager }

  // See IAIMP2CoverArtManagerID
  IAIMP2CoverArtManager = interface
    { Return picture will be proportional stretched to ADisplaySize value }
    function GetCoverArtForFile(AFile: PWideChar; const ADisplaySize: TSize): HBITMAP; stdcall;
    { Draw CoverArt of playing file, Return - cover art drawing successfuly }
    { CoverArt will be proportional stretched to R value }
    function CurrentCoverArtDraw(DC: HDC; const R: TRect): HRESULT; stdcall;
    { Return <> S_OK, CoverArt is empty or file are not playing }
    function CurrentCoverArtGetSize(out ASize: TSize): HRESULT; stdcall;
    { W, H - destination display sizes, function will correct sizes for proportional drawing }
    { Return <> S_OK, CoverArt is empty or file are not playing }
    function CurrentCoverArtCorrectSizes(var W, H: Integer): HRESULT; stdcall;
  end;

  { IAIMP2Controller }
 
  IAIMP2Controller = interface
    function IsUnicodeVersion: Boolean; stdcall; // Must be True
    // CallBack
    function AIMP_CallBack_Set(dwCBType: DWORD; Proc: Pointer; User: DWORD): Boolean; stdcall;
    function AIMP_CallBack_Remove(dwCBType: DWORD; Proc: Pointer): Boolean; stdcall;
    // Status
    function AIMP_Status_Get(StatusType: DWORD): DWORD; stdcall;
    function AIMP_Status_Set(StatusType, Value: DWORD): boolean; stdcall;
    // Playlist
    // For Compatibility only! Please, use PlaylistManager Interface!! 
    function AIMP_PLS_Clear(ID: HPLS): Boolean; stdcall;
    function AIMP_PLS_Delete(ID: HPLS): Boolean; stdcall;
    function AIMP_PLS_New(Name: PWideChar): HPLS; stdcall;
    function AIMP_PLS_Info(Index: Integer; out PlaylistInfo: TAIMPPlaylistInfo): Boolean; stdcall;
    function AIMP_PLS_Count: Word; stdcall;
    function AIMP_PLS_GetFiles(ID: HPLS; out Strings: IPLSStrings): Boolean; stdcall;
    function AIMP_PLS_GetSelFiles(ID: HPLS; out Strings: IPLSStrings): Boolean; stdcall;
    function AIMP_PLS_AddFiles(ID: HPLS; Strings: IPLSStrings): Boolean; stdcall;
    function AIMP_PLS_SetPLS(ID: HPLS): Boolean; stdcall;
    // System
    function AIMP_NewStrings(out Strings: IPLSStrings): Boolean; stdcall;
    function AIMP_GetCurrentTrack(AInfo: PAIMP2FileInfo): Boolean; stdcall;
    function AIMP_QueryInfo(Filename: PWideChar; AInfo: PAIMP2FileInfo): Boolean; stdcall;
    function AIMP_GetSystemVersion: DWORD; stdcall;
    function AIMP_CallFunction(FuncID: DWORD): Boolean; stdcall;
    function AIMP_GetLanguage(Str: PWideChar; ABufferSizeInChars: Integer): Integer; stdcall;
    function AIMP_GetCfgPath(Str: PWideChar; ABufferSizeInChars: Integer): Integer; stdcall;
    function AIMP_GetSupportExts(Flags: DWORD; Str: PWideChar; ABufferSizeInChars: Integer): Integer; stdcall;
    // Menu
    function AIMP_Menu_CreateEx(Parent: HAIMPMENU; MenuInfo: PAIMPMenuInfo): HAIMPMENU; stdcall;
    function AIMP_Menu_Create(MenuID: DWORD; MenuInfo: PAIMPMenuInfo): HAIMPMENU; stdcall;
    function AIMP_Menu_Update(Handle: HAIMPMENU; MenuInfo: PAIMPMenuInfo): Boolean; stdcall;
    function AIMP_Menu_Remove(Handle: HAIMPMENU): Boolean; stdcall;
    // extantion
    function AIMP_QueryObject(ObjectID: Integer; var Obj): Boolean; stdcall;
  end;

  { IAIMPAddonHeader }

  IAIMPAddonHeader = interface
    function GetHasSettingsDialog: LongBool; stdcall;
    function GetPluginAuthor: PWideChar; stdcall;
    function GetPluginName: PWideChar; stdcall;
    procedure Finalize; stdcall;
    procedure Initialize(AController: IAIMP2Controller); stdcall;
    procedure ShowSettingsDialog(AParentWindow: HWND); stdcall;
  end;

  TAIMPAddonHeaderProc = function (out AHeader: IAIMPAddonHeader): LongBool; stdcall; 
  // Export function name: AIMP_QueryAddonEx

//==============================================================================
// Old Style Addon strunct - don't use for new plugins
//==============================================================================

  PAIMPAddonHeader = ^TAIMPAddonHeader;
  TAIMPAddonHeader = packed record
    Version: DWORD; // Old Value: Byte - default alignment - 4 bytes
    DllInstance: DWORD;
    GetPlgName: function: PAnsiChar;  stdcall; 
    GetAuthor: function: PAnsiChar;  stdcall;
    Init: procedure (AIMP: IAIMP2Controller); stdcall;
    Config: procedure (Handle: DWORD; const Win: DWORD); stdcall;
    Free: procedure; stdcall;
  end;
 
  TAddonProc = function: PAIMPAddonHeader; stdcall;
  // Export function name: AIMP_QueryAddon

//==============================================================================
// V I S U A L S
//==============================================================================

const
  // PluginFlags
  VIS_RQD_DATA_WAVE       = 1;
  VIS_RQD_DATA_SPECTRUM   = 2;
  VIS_RQD_NOT_SUSPEND     = 4;

type
  TWaveForm = array[0..1, 0..511] of Shortint;
  TSpectrum = array[0..1, 0..255] of Byte;

  TAIMPVisualData = packed record
    LevelR, LevelL: Integer;
    Spectrum: TSpectrum;
    WaveForm: TWaveForm;
  end;
  PAIMPVisualData = ^TAIMPVisualData;

  IAIMP2VisualPlugin = interface
    function AuthorName: PWideChar; stdcall;
    function PluginName: PWideChar; stdcall;
    function PluginInfo: PWideChar; stdcall;
    function PluginFlags: DWORD; stdcall;
    function Initialize: BOOL; stdcall;
    procedure Deinitialize; stdcall;
    procedure DisplayClick(X, Y: Integer); stdcall;
    procedure DisplayRender(DC: HDC; AData: PAIMPVisualData); stdcall;
    procedure DisplayResize(AWidth, AHeight: Integer); stdcall;
  end;

  TAIMPVisualProc = function: IAIMP2VisualPlugin; stdcall;
  // Export function name: AIMP_QueryVisual

implementation

end.
