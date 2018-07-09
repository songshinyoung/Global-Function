//---------------------------------------------------------------------------

#ifndef GlobalFunctionH
#define GlobalFunctionH
//---------------------------------------------------------------------------

#define GlobalFunctionVer 0x1002
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Graphics.hpp>
#include <TCHAR.H>

#include <Windows.h>

#pragma comment(lib,"version.lib")

typedef enum enumPanelBtnType{
    BUTTON_DOWN_ENABLE = 0,
    BUTTON_DOWN_DISABLE,
    BUTTON_UP_ENABLE,
    BUTTON_UP_DISABLE,
    BUTTON_CRNT_DISABLE, // UP/DOWN 현재 상태 그대로 DISABLE 시킴. 
    BUTTON_VISIBLE_HIDE,
    BUTTON_STATE_MAX

}emPanelBtnType;

typedef enum enumPanelBtnColorType{
    COLOR_NORMAL = 0,   // UP : clBtnFace + clBlack,  DOWN : clRed + clWhite
    COLOR_RETURN,       // UP : clSkyBlue + clNavy,   DOWN : clRed + clWhite
    COLOR_BLUE,         // UP : clBtnFace + clBlue,   DOWN : clRed + clWhite
    COLOR_VALUE,        // UP : clPurple  + clYellow, DOWN : clPurple  + clYellow
                        // <-- 추가되는 위치..
    COLOR_MAX

}emPanelBtnColor;


#define TEXT_ALIGN_TOP      0x01
#define TEXT_ALIGN_LEFT     0x02
#define TEXT_ALIGN_RIGHT    0x04
#define TEXT_ALIGN_BOTTOM   0x08
#define TEXT_ALIGN_H_CENTER 0x10
#define TEXT_ALIGN_V_CENTER 0x20


#define FILE_READ_BUFSIZE   1024
#define MD5LEN              16

class TGlobalFuction
{
public:
    __fastcall TGlobalFuction();
    virtual __fastcall ~TGlobalFuction();

    /** IsNumStr()
    * @brief : 주어진 문자열을 주어진 길이 까지 각각 모두 10진수 숫자로 이루어진
               문자열인지 확인한다.
    * @param char *str : 문자열 
    * @param unsigned int nLen : 문자열 길이
    * @param bool bCareSign : true : +, - 표기도 숫자로 취급한다. 
    * @param bool bCarePoint : true : 소수점(.)도 숫자로 취급한다. 
    * @return bool : 10진수 숫자로만 이루어진 경우 true 반환
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    bool __fastcall IsNumStr(char *str, unsigned int nLen, bool bCareSign, bool bCarePoint); // 스트링이 숫자열인지 확인하여 숫자일 경우 true 반환


    /** HexToStr()
    * @brief : Hex 값을 String으로 변환해 준다. 0x123 이라면 "123" 으로 변환 한다. 
    * @param : int n : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013년 4월 10일 수요일][송신영] 최초 작성 
    **/    
    AnsiString __fastcall HexToStr(int n, bool bUpper = true);


    /** Sec_To_HHMMSS()
    * @brief : 초를 문자열 HH:MM:SS로 변경하여 리턴한다. 
    * @param : int nSec : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013년 4월 15일 월요일][송신영] 최초 작성 
    **/    
    AnsiString __fastcall Sec_To_HHMMSS(int nSec);


    /** CurrentTime_SS()
    * @brief : 현재 시각을 INT형으로 변환하여 리턴한다.
    * @param : int nSec :
    * @return AnsiString :
    * @see
    - history : 1. [2013년 10월 23일 월요일][이재득] 최초 작성
    **/    
    int __fastcall CurrentTime_SS();


    /** DivideByIndicator()
    * @brief : sSrc로 주어진 문자열을 cIndicator에 해당되는 문자로 구분하여 잘라 sList에 넣어준다. 
               잘려진 문자열의 처음과 끝부분의 공백은 없엔다. 
    * @param AnsiString sSrc : 
    * @param TStringList * sList : 
    * @param char cIndicator : 
    * @return int : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    int  __fastcall DivideByIndicator(AnsiString sSrc, TStringList * sList, char cIndicator);


    int __fastcall StringToken(UnicodeString &sSrc, TStringList * sList, const wchar_t * sIndicator, bool bTrim);
    
    int __fastcall StringToken(AnsiString &sSrc, TStringList * sList, const char * sIndicator, bool bTrim);


    /** Panel_Down_ActionColor()
    * @brief : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 
               선택된 상태로 그려주는 함수이다. 
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @param bool bEnabled : true : Enable, false : Disable
    * @param TColor DownBgColor : TPanel의 배경색 지정. 입력이 없을 경우 default로 clRed 이다.
    * @param TColor DownTxtColor : TLabel의 Font 색상이다. 입력이 없을 경우 default로 clWhite 이다.
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall Panel_Down_ActionColor(TObject *Sender, 
                                        TObject *pLable, 
                                        bool bEnabled,
                                        TColor UpBgColor = clRed, 
                                        TColor UpTxtColor = clWhite);



    /** Panel_Up_ActionColor()
    * @brief : 
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @param bool bEnabled : true : Enable, false : Disable 
    * @param TColor UpBgColor : TPanel의 Enable상태에서의 배경색 지정. 입력이 없을 경우 default로 clBtnFace 이다.
    * @param TColor UpTxtColor : TLabel의 Enable상태 Font 색상이다. 입력이 없을 경우 default로 clBlue 이다.
    * @return void : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 
                    선택이 안된 상태로 그려주는 함수이다. 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall Panel_Up_ActionColor(TObject *Sender, 
                                        TObject *pLable, 
                                        bool bEnabled, 
                                        TColor UpBgColor = clBtnFace, 
                                        TColor UpTxtColor = clBlack);


    /** Panel_CrntDisable()
    * @brief : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 현재의 상태 그대로 Disable 상태로 만든다.  
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    void __fastcall Panel_CrntDisable(TObject *pPanel, TObject *pLable);


    /** PanelBtnState()
    * @brief : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 
               Up / Down / Disable 상태로 그려주는 함수이다. 
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @param emPanelBtnType nState : 버튼의 상태를 나태내는 enum type 
    * @param emPanelBtnColor nColor : 버튼의 색상을 지정하는 enum type
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall PanelBtnState( TObject *pPanel, 
                                    TObject *pLable, 
                                    emPanelBtnType nState, 
                                    emPanelBtnColor nColor = COLOR_NORMAL);   


    /** GrideReSetting()
    * @brief : StringGrid 의 Col, Row 수가 변경될 경우 정해진 Width, Height 를 넘지 않는 
               범위에서 Cell의 크기를 정해주는 함수이다. 
    * @param TObject *pSG : 컴포넌트 포인터 
    * @param int col : 원하는 Col. 수 
    * @param int row : 원하는 Rwo 수 
    * @param int MaxWidth : 허용 최대 Width 값 (픽셀)
    * @param int MaxHeight : 허용 최대 Height 값 (픽셀) 
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    void __fastcall GrideReSetting(TObject *pSG, int col, int row, int MaxWidth, int MaxHeight);


    /**  DeleteDoubleSlash()
    * @brief : Error Code File에서 Message를 읽으면 \r\n이 \\r\\n으로 변경되어 읽혀진다. 
               이것을 다시 \r\n으로 만드는 함수이다.
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DeleteDoubleSlash(AnsiString &sMsg);
    void __fastcall DeleteDoubleSlash(UnicodeString &sMsg);

    /** DeleteCarriageReturn()
    * @brief : 문자열 주에 \r \n 문자를 삭제한다. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012년 7월 6일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DeleteCarriageReturn(AnsiString &sMsg);
    void __fastcall DeleteCarriageReturn(UnicodeString &sMsg);


    /** DeleteLastCarriageReturn()
    * @brief : 문자열의 마지막에 \r\n을 삭제한다. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012년 7월 6일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DeleteLastCarriageReturn(AnsiString &sMsg);


    /** DeleteLastSlash()
    * @brief : 디렉토리 경로 에서 마지막에 "\\" 문자를 삭제한다. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DeleteLastSlash(AnsiString &sMsg);


    /** GetGCD_LCM()
    * @brief : 최대 공약수와 최대 공배수를 구하는 함수.
    * @param int a : 입력 수 1
    * @param int b : 입격 수 2
    * @param int * pgcd : 최대 공약수를 리턴 
    * @param int * plcm : 최대 공배수를 리턴. 
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    void __fastcall GetGCD_LCM(int a, int b, int * pgcd, int * plcm);

    //---------------------------------------------------------------------------
    // String Grid 관련 툴. 

    /** StringGridSubCodeView()
    * @brief : String Grid Cell Draw 함수에서 해당 Cell을 주어진인자로 다시 그려주는 역할을 한다. 
     
    * @param : TObject *Sender : TStringGrid 포인터 
    * @param : int ACol : 현재 Col
    * @param : int ARow : 현재 Row
    * @param : TRect &Rect : 현재 cell의 Rect
    * @param : TColor clBackColor : B배경색 
    * @param : TColor clFontColor : 폰트 색 
    * @param : int nFontSize : 폰트 크기 
    * @param : bool bTextAlignCenter : true : 좌우 중앙 배열, false : 좌 배열 
    * @param : bool bTextBold : true ; 폰트 굵게, false : 기본 

    * @return void : 
    * @see 
    - history : 1. [2012년 7월 9일 월요일][송신영] 최초 작성 
    **/    
    void __fastcall StringGridSubCodeView(TObject   *Sender, 
                                          int       ACol,
                                          int       ARow,
                                          TRect     &Rect,
                                          TColor    clBackColor,
                                          TColor    clFontColor,
                                          int       nFontSize,
                                          bool      bTextAlignCenter,
                                          bool      bTextBold);

    //---------------------------------------------------------------------------
    // Directory 및 File 과련 툴. 

    /** GetLastDirName()
    * @brief : 디렉토리 경로에서 마지막 디렉토리 이름을 잘라서 리턴해준다. 
              ex) "c:\\Temp\\Test\\" 라는 경로를 인자로넘겨준면 "Test" 가 리턴된다. 
    * @param : AnsiString sPath : 디렉토리 경로 (파일 이름이 포함되면 안됨)
    * @return AnsiString : 경로중에 마지막 디렉토리 이름을 리턴한다. 
    * @see 
    - history : 1. [2013년 3월 21일 목요일][송신영] 최초 작성 
    **/    
    AnsiString  __fastcall GetLastDirName(AnsiString sPath);


    /** GetFileExt()
    * @brief : 파일 이름에서 확장자만 추출하여 리턴한다. "."은 포함되지 않는다. 
               ex1) "File.txt" 일 경우 txt를 리턴한다. 
               ex2) "c:\\Temp\\File.txt" 일 경우  txt를 리턴한다. 
    * @param : AnsiString sFileName : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013년 3월 21일 목요일][송신영] 최초 작성 
    **/
    AnsiString  __fastcall GetFileExt(AnsiString sFileName);

    /** GetFileSize()
    * @brief : 주어진 파일 이름을 해당 파일의 size를 리턴한다. 
    * @param AnsiString &sFileName : 파일 이름. (Full Path)
    * @param DWORD * pFileSizeHigh : 만약 File Size가 DWORD 범위를 벗어날 경우 pFileSizeHigh에 상위 값이 들어간다. 
    * @return DWORD : File Size의 DWORD 범위 내의 값을 리턴한다. 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    DWORD       __fastcall GetFileSize(AnsiString &sFileName, DWORD * pFileSizeHigh);

    /** FindDir()
    * @brief : 지정된 Path의 하위 디렉토리를 모두 찾아 TStringList* pList 에 넘겨준다. 
    * @param AnsiString Path :  찾을 Root 디렉토리. 
    * @param TStringList * pList : 찾은 디렉토리 리스트로 반환할 포인터.  
    * @return bool : 하나라도 발견되면 true 리턴, 하나도 없으면 false.
    * @see 
    - history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성 
    **/    
    bool        __fastcall FindDir(AnsiString Path, __out TStringList * pList);



    /**
    * @fn      : int SetFileDateTime(char *szFilePath, FILETIME ftCreated, FILETIME ftModified, FILETIME ftAccessed)
    * @brief   : 파일 및 폴더의 시간을 주어진 인자로 변경한다. 
    * @details : 
                 bool bIgnoreAttrAndApply;  // 이 변수를 true로 설정하만 아래 변수가 적용된다. 
                 bool bAttrHidden;          // Hidden 속성 파일도 변경. 
                 bool bAttrReadOnly;        // ReadOnly 속성 파일도 변경. 
                 bool bAttrSystem;          // System 속성 파일도 변경. 
                 bool bAttrCompressed;      // 압축 파일도 변경. 
                 bool bAttrEncrypted;       // Encrypted 속성 파일도 변경. 

    * @param   : char *szFilePath : 
    * @param   : FILETIME ftCreated : 
    * @param   : FILETIME ftModified : 
    * @param   : FILETIME ftAccessed : 
    * @return  : int : 
    * @see     : 
    - history  : 1. [2017년 5월 24일 수요일][송신영] 최초 작성 
    **/

    bool bIgnoreAttrAndApply;
    bool bAttrHidden;
    bool bAttrReadOnly;
    bool bAttrSystem;
    bool bAttrCompressed;
    bool bAttrEncrypted;

    int __fastcall SetFileDateTime(char *szFilePath, 
                                    TDateTime ftCreated, 
                                    TDateTime ftModified, 
                                    TDateTime ftAccessed);


    /** FindFileList()
    * @brief : 주어진 폴더 내부에 모든 파일들을 찾아 리스트에 넣어준다. 
               bSubDir 가 true이면 하위 디렉토리도 찾는다. 
    * @param AnsiString Path : 
    * @param bool bSubDir : 
    * @param __out TStringList * pList : 
    * @return bool : 
    * @see 
    - history : 1. [2013년 3월 26일 화요일][송신영] 최초 작성 
    **/
    bool        __fastcall FindFileList(AnsiString Path, bool bSubDir, __out TStringList * pList);



    /**
    * @fn      : TDateTime __fastcall GetFileMakeTime(AnsiString sFilePath);
    * @brief   : 해당 파일의 생성 일자를 구한다. 
    * @details : 
    * @param   : AnsiString sFilePath : 
    * @return  : TDateTime : 
    * @see     : 
    - history  : 1. [2017년 4월 19일 수요일][송신영] 최초 작성 
    **/
    TDateTime __fastcall GetFileMakeTime(AnsiString sFilePath);



    /** SearchFileDate()
    * @brief : 인자로 주어진 폴더 내의 파일들 중에 가장 최신의 파일의 시간을 리턴 한다. 
    * @param AnsiString Mask : 
    * @return TDateTime : 
    * @see 
    - history : 1. [2013년 3월 27일 수요일][송신영] 최초 작성 
    **/    
    TDateTime   __fastcall SearchFileDate(AnsiString sDirPath);


    /** SearchLatestFile()
    * @brief : 주어진 경로에서 주어진 파일 이름을 포함한 파일 중에 가장 최신의 파일명을 리턴한다. 
    * @param AnsiString sDirPath  : 폴더 경로 
    * @param AnsiString sFileName : 파일에 포함된 문자, ""일 경우 모든 파일 중에 가장 최신을 찾는다. 
    * @param AnsiString sExt      : 파일의 확장자, ""일 경우 모든 확장자에서 찾는다. 
    * @return AnsiString : 가장 최신의 파일 이름 리턴. 
    * @see 
    - history : 1. [2015년 7월 2일 목요일][송신영] 최초 작성 
    **/
    AnsiString  __fastcall SearchLatestFile(AnsiString sDirPath, AnsiString sFileName, AnsiString sExt);


    /** DeleteOldTimeFile()
    * @brief : 주어진 경로의 폴더 아래에 있는 파일중에 기준 시간 보다 오래된 파일을 삭제한다.
    * @param AnsiString  sSrcPath : 폴더 경로, 뒤에 "\\"가 붙지 않아야한다.
    * @param TDateTime tmStandardTime : 삭제될 기준 시간
    * @param int  nTimeType : 파일의 시간 중에 기준이 될 Type을 정한다.
                                0 : 생성한 날짜 기준
                                1 : 마지막 저장 날짜
                                2 : 마지막 Access 날짜
    * @param bool bSubDir : true : 하위 디렉토리도 검색하여 삭제한다.
    * @return bool : true : 해당 포더가 비어있으면 true를 리턴한다. (폴더를 삭제하기 위해)
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    bool        __fastcall DeleteOldTimeFile(  AnsiString  sSrcPath,
                                        TDateTime   tmStandardTime,
                                        int         nTimeType,
                                        bool        bSubDir);



    /**
    * @fn      : bool __fastcall TGlobalFuction::DeleteOldTimeDir(AnsiString  sSrcPath, TDateTime tmStandardTime)
    * @brief   : 인자로 주어진 시간보다 오래된 폴더를 찾아 삭제한다. 
    * @details : sSrcPath에 주어진 폴더에서 주어진 tmStandardTime 시간 보다 생성 시간이 오래된 폴더는 모두 삭제한다. 
    * @param   : AnsiString  sSrcPath :         삭제할 폴더 Root 경로 (Root 경로는 삭제되지 않는다.)
    * @param   : TDateTime tmStandardTime :     삭제할 기준 시간 (이 시간보다 오래된 폴더는 삭제한다.)
    * @return  : bool : 
    * @see     : 
    - history  : 1. [2017년 5월 24일 수요일][송신영] 최초 작성 
    **/
    bool        __fastcall DeleteOldTimeDir(AnsiString  sSrcPath,
                                            TDateTime   tmStandardTime);    

    /** DeleteDirectory()
    * @brief : 지정된 Path의 하위 디렉토리를 모두 찾아 삭제한다.
               bool bDeleteRoot 를 true로 설정하면 호출한 자신의 디렉토리도 삭제한다.
               그러나 false로 지정하면 호출한 자신의 디렉토리는 그대로 두고 자식 디렉토리와 파일들만 삭제한다.
    * @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
    * @param Tbool bDeleteRoot : 호출한 Root 디렉토리도 삭제할 경우 true로 설정한다.
    * @return void : 없음.
    * @see
    - history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성
    **/   
    void        __fastcall DeleteDirectory(AnsiString  sSrcPath, bool bDeleteRoot);

    /** DeleteDirectory()
    * @brief : 지정된 Path의 하위 디렉토리를 에서 디렉토리 이름이 sDirName인 것을 찾아 모두 찾아 삭제한다.
    * @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
    * @param AnsiString sDirName :  삭제할 디렉토리 이름.
    * @param  bool bDelete       :  true이면 찾아진 폴더나 파일을 모두 삭제한다. false이면 디렉토리 이름이 sDirName인것을 찾는 동작만 한다.
    * @return void : 없음.
    * @see
    - history : 1. [2013년 3월 21일 금요일][송신영] 최초 작성
    **/    
    void        __fastcall DeleteDirectory(AnsiString  sSrcPath, AnsiString sDirName, bool bDelete = false);


    /** DeleteFileByExtension()
    * @brief : 지정된 Path의 파일 중에 확장자가 지정된 확장자와 동일하면 삭제한다.
               bSubDir 가 true이면 하위 디렉토리도 찾아서 삭제한다.
    * @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
    * @param AnsiString sExeName :  삭제할 파일 확장자.
    * @param  bool bSubDir       :  true이면 하위 디렉토리도 찾는다.
    * @return void : 없음.
    * @see
    - history : 1. [2013년 3월 21일 금요일][송신영] 최초 작성
    **/    
    void        __fastcall DeleteFileByExtension(AnsiString  sSrcPath, AnsiString sExeName, bool bSubDir);


    /** ChangeExtension()
    * @brief : 주어진 sFileName에서 확장자를 sExt로 변경해준다. 
    * @param AnsiString sFileName : 확장자를 변경하고자하는 파일 이름으로 전체 경로가 포함되어도 상관 없다. 
    * @param AnsiString sExt : 확장자에는 ".Exe" 처럼 '.'을 포함하여야 한다. 
    * @return AnsiString : 확장자를 변경한 파일 이름을 리턴한다. 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    AnsiString  __fastcall ChangeExtension(AnsiString sFileName, AnsiString sExt);

    /** CopyFiles()
    * @brief : 특정 폴더로 모든 파일을 복사 
    * @param const char *srcfiles :  파일 이름이 있는 Full Path ("c:\\Temp\\*.ini" )
    * @param const char *destdir :   복사될 목표 폴더. 
    * @return int : 
    * @see 
    - history : 1. [2013년 3월 28일 목요일][송신영] 최초 작성 
    **/    
    int         __fastcall CopyFiles(const char *srcfiles, const char *destdir);


    /** DirectoryCopy()
    * @brief : Directory에서 Directory로 sub 폴더를 포함하여 복사 한다. 
    * @param AnsiString sSrcPath : Source Directory
    * @param AnsiString sTrgPath : Target Directory
    * @param TStringList * slSkipDirectory : sub Directory 중에서 복사하지 말아야할 이름을 지정할 수 있다. 
    * @return bool : 정상일 경우 true 리턴.
    * @see 
    - history : 1. [2015년 11월 24일 화요일][송신영] 최초 작성 
    **/    
    bool __fastcall DirectoryCopy(AnsiString sSrcPath, AnsiString sTrgPath, TStringList * slSkipDirectory);

   
    bool __fastcall DirRename(const char * sOldname,const char * sNewname);

    bool __fastcall FileRename(const char * sOldname,const char * sNewname);

    /** ShowDirSelectDlg()
    * @brief : 디렉토리 경로를 탐색하여 해당 경로를 지정해주는 다이얼로그 창을 출력한다. 
               사용 방법 
               void __fastcall TForm1::Button3Click(TObject *Sender)
                {
                    _TCHAR szPath[MAX_PATH];

                    if( ShowDirSelectDlg( Handle, szPath ) ) {
                        Caption = szPath;
                    }
                }

    * @param HWND hWndOwner : 부모 Form 의 핸들 
    * @param _TCHAR *szDir :  선택된 경로 
    * @return bool :          선택된 경우 true 리턴, 취소한 경우 false리턴 
    * @see 
    - history : 1. [2013년 3월 21일 목요일][송신영] 최초 작성 
    **/
    bool        __fastcall ShowDirSelectDlg( HWND hWndOwner, _TCHAR *szDir );


    /** GetApplicationDir()
    * @brief : 현재 실행 파일의 Root 폴더를 리턴한다. 뒤에 "\\"가 붙는다. 
    * @param : 없음.
    * @return AnsiString : 실행파일 Path
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    AnsiString  __fastcall GetApplicationDir() ; // SSY_13.10.10 [ 실행파일이 존재하는 Root 폴더 절대 경로  ] 


    /**
    * @fn      : char __fastcall TGlobalFuction::GetApplicationDiskName()
    * @brief   : 
    * @details : 현재 실행 파일이 있는 Disk의 이름을 리턴하는 함수. 
    * @param   : None
    * @return  : char : 찾으면 찾은 Disk Name을 리턴하고 모찾으면 '\0' 를 리턴한다. 
    * @see     : 
    - history  : 1. [2018년 1월 25일 목요일][Song Shin Young] First. 
    **/
    char __fastcall GetApplicationDiskName() ;


    /** GetMD5HashCodeFromFile()
    * @brief : File의 MD5 Hash Code를 생성하는 함수이다. 
    * @param __in LPCSTR filename : 파일 이름(전체 경로)
    * @param __out LPSTR pRetHashCode : 생성된 MD5 Hash Code를 Hex Type String으로 변환하여 리턴한다. 
                                        Hash Code Len가 16 byte이므로 Hex Type String으로 변환하면 32 Byte가 된다. 
    * @return bool : 정상:true, 실패:false
    * @see 
    - history : 1. [2015년 10월 22일 목요일][송신영] 최초 작성 
    **/
    bool        __fastcall GetMD5HashCodeFromFile(__in LPCSTR filename, __out LPSTR pRetHashCode);


    /**
    * @fn      : bool        __fastcall TGlobalFuction::DriveExists(char cDriveName, bool bOnlyHDD)
    * @brief   : 인자로 주어진 드라이브가 현재 존재하는지 확인하는 함수.
    * @details : 
    * @param   : char cDriveName : 확인하고자하는 드라이브 이름 'c', 'd' ...
    * @param   : bool bOnlyHDD : true  : 이 값이 true이면 오직 HDD 인 경우만 true를 리턴한다. 
                                 false : 이 값이 false이면 Removable, Remote, CD-Rom, RamDisk도 true로 리턴한다. 
    * @return  : bool : 드라이브 명이 존재하면 true를 리턴한다. 
    * @see     : 
    - history  : 1. [2017년 1월 4일 수요일][송신영] 최초 작성 
    **/
    bool        __fastcall DriveExists(char cDriveName, bool bOnlyHDD = false);


    /**
    * @fn      : double __fastcall TGlobalFuction::GetDiskFreeSpace(char cDrive)
    * @brief   : 
    * @details : 주어진 디스크의 여유 공간을 MByte 단위로 리턴하는 함수 
    * @param   : char cDrive : 
    * @return  : double : 여유 공간 MByte
    * @see     : 
    - history  : 1. [2018년 1월 25일 목요일][Song Shin Young] First. 
    **/
    double      __fastcall GetDiskFreeSpace(char cDrive);








    //---------------------------------------------------------------------------
    // 그리기 툴

    /** DrawGraphBackLine()
    * @brief : Bitmap에 주어진 크기로 격자 무늬를 생성한다. 
    * @param Graphics::TBitmap* Bitmap : 그려질 Bitmap 포인터 
    * @param int nWidth : 넓이 
    * @param int nHeight : 높이 
    * @param int nXpitch : X방향 간격 (Width 방향)
    * @param int nYpitch : Y방향 간격 (Height 방향)
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DrawGraphBackLine(  Graphics::TBitmap* Bitmap, 
                                        int nWidth, 
                                        int nHeight, 
                                        int nXpitch, 
                                        int nYpitch);   


    /** DrawImageTrans()
    * @brief : pDest에 srcBmp를 X,Y 위치에 투명처리하여 그린다. 
    * @param Graphics::TBitmap* pDest : 그려질 목적지 Bitmap
    * @param int X : pDest위의 픽셀 좌표 (Widht 방향 시작 점)
    * @param int Y : pDest위의 픽셀 좌표 (Height 방향 시작 점)
    * @param Graphics::TBitmap* srcBmp : 그릴 소스 Bitmap
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall DrawImageTrans(Graphics::TBitmap* pDest, int X, int Y, Graphics::TBitmap* srcBmp);



    /** DrawTextTrans()
    * @brief : pDest Bitmap에 sString 문자열을 X,Y 위치에 배경을 투명 처리하여 그린다. 
    * @param Graphics::TBitmap* pDest : 그려질 목적지 Bitmap 포인터
    * @param Graphics::TBitmap* pTextBitmap : 문자열을 투명 처리하는데 사용할 임시 Bitmap 포인터
    * @param int             X : 그려질 X 위치
    * @param int             Y : 그려질 Y 위치
    * @param AnsiString      sString : 출력할 문자열
    * @param TColor          cColor : 글자 색상
    * @param unsigned int    Align : 글자의 위치 정렬 방식 TEXT_ALIGN_BOTTOM, TEXT_ALIGN_V_CENTER, TEXT_ALIGN_RIGHT, TEXT_ALIGN_H_CENTER
    * @param int             nFontSize : 폰트 크기
    * @param bool            bBold : 폰트 굵기 설정
    * @return void :
    * @see
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성
    **/
    void __fastcall DrawTextTrans(  Graphics::TBitmap* pDest,
                                    Graphics::TBitmap* pTextBitmap,
                                    int             X,
                                    int             Y,
                                    AnsiString      sString,
                                    TColor          cColor,
                                    unsigned int    Align,
                                    int             nFontSize = 10,
                                    bool            bBold = true);


    /** DrawGradient()
    * @brief : TCanvas * 에 주어진 색상으로 그라데이션을 그린다.
    * @param TCanvas *   pCanvas : 그려질 대상 Canvas 포인터
    * @param TColor      StartColor : 그라데이션 시작 색상
    * @param TColor      EndColor :  그라데이션 완료 색상
    * @param int         ShadingLevel : 시작 색상과 완료 색상간의 색 구분 단계 (값으 클 수록 부드러운 표현)
    * @param TRect       &Rect : 그려질 구역을 설정한다.
    * @return void :
    * @see
    - history : 1. [2015년 11월 10일 화요일][송신영] 최초 작성
    **/
    void __fastcall DrawGradient(   TCanvas *   pCanvas,
                                    TColor      StartColor,
                                    TColor      EndColor,
                                    int         ShadingLevel,
                                    TRect       &Rect) ;




    /** Raw8Bit_to_Bmp24Bit()
    * @brief : 8 bit Raw 데이터 이미지를 24Bit BMP 파일로 생성해준다. 
    * @param BYTE * pRawData : 8 bit Raw Image 데이타 
    * @param TMemoryStream * pStm : 24 Bit BMP 로 변경된 데이터 반환 TMemorySteam 포인터 
    * @param int nWidth : 이미지 Width
    * @param int nHeight : 이미지 Height
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    void __fastcall Raw8Bit_to_Bmp24Bit(BYTE * pRawData, TMemoryStream * pStm, int nWidth, int nHeight);


    /** RawData_Bottom_Up()
    * @brief : Image raw data를 상하 반전 시키는 함수 
    * @param BYTE *data : Raw image data 포인터 
    * @param int nPixelByte : 한 Pixel당 차지하는 byte 수 
    * @param int biWidth : 이미지 넓이 
    * @param int biHeight : 이미지 높이 
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    void __fastcall RawData_Bottom_Up(BYTE *data, int nPixelByte, int biWidth, int biHeight);


    
    //---------------------------------------------------------------------------
    // 수학 계산 

    /** DecimalRounding()
    * @brief : nPoint 의 소수점 자리만큼 반올림하여 리턴한다. 
               3.14159 를 nPoint 가 2 라면 3.14를 리턴한다. 
               만약 nPoint가 3이라면 3.142 를 리턴한다. 
    * @param double dSrc : 반올림할 원래의 값 
    * @param int nPoint :  반올림할 소수점 자리 
    * @return double :     
    * @see 
    - history : 1. [2013년 4월 2일 화요일][송신영] 최초 작성 
    **/    
    double __fastcall DecimalRounding(double dSrc, int nPoint);


    /** CompareDoubleUlps()
    * @brief : double 타입 오차 허용범위 내에서 비교하기
    * @param double x : 
    * @param double y : 
    * @param int ulpsTolerance = 4 : 
    * @return int : 
    * @see 
    - history : 1. [2015년 4월 2일 목요일][송신영] 최초 작성 
    **/    
    int    __fastcall CompareDoubleUlps(double x, double y, int ulpsTolerance = 4);


    /** CompareDoubleAbsoulteAndUlps()
    * @brief :  오차 허용 범위 내에서 비교하기 - 절대 범위 + ULP 사용 
                ULP를 이용한 비교가 얼핏 보면 절대 범위를 사용하는 것처럼 보이지만,
                1 ULP의 크기는 숫자의 크기에 따라 변하는 상대적인 값이기 때문에 상대 범위 비교에 해당된다고 볼 수 있습니다.
                그러므로 이전에 설명했었던 CompareDoubleRelative 함수와 마찬가지로 0에 가까운 아주 작은 값을 비교할 때에
                (흔히 발생되는 상황은 아니지만) 문제가 발생할 여지가 남아있습니다. 
                결국 이러한 상황까지 고려하려면 아래와 같이 절대 범위 비교와 ULP 비교를 조합하여 구현해야 합니다. 

    * @param double x : 
    * @param double y : 
    * @param double absTolerance = (1.0e-8) :
    * @parma int ulpsTolerance = 4 :
    * @return int :  0 : 두 수가 같음.
                     1 : x가 y보다 큼.
                    -1 : X가 Y보다 작음.
    * @see 
    - history : 1. [2015년 4월 2일 목요일][송신영] 최초 작성 
    **/    
    int    __fastcall CompareDoubleAbsoulteAndUlps(double x, double y, double absTolerance = (1.0e-8), int ulpsTolerance = 4);


    //---------------------------------------------------------------------------
    // COM Port Check

    /** OpenCheckComport()
    * @brief : ComPort의 상태를 확인하여 사용 가능한 Port인지 확인한다. 
    * @param : char * szPort : ComPort Name ("COM4")
    * @return bool : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/    
    bool __fastcall OpenCheckComport(char * szPort);

    //---------------------------------------------------------------------------
    // Component 관리 툴 

    /* * SetRadioGroupItemIndex()
    * @brief : TRadioGroup 의 ItemIndex를 변경시 OnClick Event가 발생하는데
               이 함수를 사용하면 OnClick Event 없이 ItemIndex를 변경할 수 있음.
    * @param TRadioGroup *pRGroup :
    * @param int Index :
    * @return void :
    * @see
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성
    * */

    void __fastcall SetRadioGroupItemIndex(TRadioGroup * pRGroup, int Index);


    /** ReplaceStr()
    * @brief : sStr 에서 sS에 해당되는 문자열을 sT로 바꾼다. 
    * @param AnsiString &sStr : 변경할 전체 문자열 (참조형으로 변경된 후 값이 이곳으로 인자로 넘긴 변수의 값이 변경된다.)
    * @param AnsiString sS : 찾을 문자열 
    * @param AnsiString sT : 대치할 문자열 
    * @return void : 
    * @see 
    - history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
    **/
    void __fastcall ReplaceStr(AnsiString &sStr, AnsiString sS, AnsiString sT);


    /**
    * @fn      : int __fastcall TGlobalFuction::StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos)
    * @brief   : 
    * @details : 주어진 문자열에서 시작 위치 부터 검색 문자열이 찾아지는 위치 값을 리턴 함. (찾은 경우 1 이상의 값을 리턴)
    * @param   : const AnsiString & sSrcStr : 
    * @param   : AnsiString sSearch : 
    * @param   : Cardinal InitPos : 
    * @return  : int : 0 : 못찾음, 1이상 : 찾은 문자열 위치 
    * @see     : 
    - history  : 1. [2018년 1월 17일 수요일][Song Shin Young] First. 
    **/
    int __fastcall StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos);


    //---------------------------------------------------------------------------
    // TCP / IP

    /** 
    * @brief : 현재 PC에 설치된 LAN 카드의 모든 IP LIST를 얻어온다. 
    * @param : TStringList * pList : IP String이 포함된 List
    * @return bool : 정상 동작 시 true 리턴. 
    * @see 
    - history : 1. [2015년 7월 2일 목요일][송신영] 최초 작성 
    **/    
    bool __fastcall LocalIPList(TStringList * pList);

    /**
    * @fn      : AnsiString __fastcall TGlobalFuction::GetFileInformation(AnsiString AFilename, AnsiString AQuery)
    * @brief   : 실행파일의 파일 정보를 얻어오는 함수 
    * @details : 
    * @param   : AnsiString AFilename : 실행파일 이름 (Full Path)
    * @param   : AnsiString AQuery : 얻고자 하는 항목을 입력하면 된다. 아래 가능한 항목을 확인하라. 
                                    - CompanyName
                                    - FileDescription
                                    - FileVersion
                                    - InternalName
                                    - LegalCopyright
                                    - LegalTrademarks
                                    - ProductName
                                    - ProductVersion
    * @return  : AnsiString : 해당 파일의 AQuery에 해당하는 정보를 리턴 한다.  
    * @see     : 
    - history  : 1. [2017년 4월 19일 수요일][송신영] 최초 작성 
    **/
    AnsiString __fastcall GetFileInformation(AnsiString AFilename, AnsiString AQuery);


    
};

//---------------------------------------------------------------------------

extern TGlobalFuction GFun;

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
 * 실행 후 Terminate되면 메모리 자동 삭제되는 Thread 를 생성할 때 사용한다. 
 
 * example : 
        void __fastcall TForm1::Button1Click(TObject *Sender)
        {
            CRunThread  thread(&Thread_Run, Form1);
        }

        void __fastcall TForm1::Thread_Run(TObject *Sender)
        {
            int nCount = 1000;
            
            while(nCount--) {

            }
        }
*/
//---------------------------------------------------------------------------
class CRunThread
{
    class TRunThread : public TThread
    {
    private:
        TNotifyEvent     OnEvent;
        TNotifyEvent     OnEnter;
        TNotifyEvent     OnExit;
        TObject*         Sender;

    public:
        __fastcall TRunThread(TNotifyEvent event, TObject* sender, TNotifyEvent Enter, TNotifyEvent exit)
            : TThread(false)
        {
            FreeOnTerminate = true;
            OnEvent = event;
            OnExit  = exit;
            OnEnter = Enter;
            Sender  = sender;
        }
        void __fastcall Execute()
        {
            try {
                if(OnEnter != NULL) OnEnter(Sender);

                if(OnEvent != NULL) OnEvent(Sender);
            }
            __finally {
                if(OnExit != NULL) OnExit(Sender);
            }
        }
    };
public:
    CRunThread(TNotifyEvent event, TObject* sender, TNotifyEvent Enter, TNotifyEvent exit)
    {
        TRunThread*  thread = new TRunThread(event, sender, Enter, exit);
    };

    CRunThread(TNotifyEvent event, TObject* sender)
    {
        TRunThread*  thread = new TRunThread(event, sender, (TNotifyEvent)NULL, (TNotifyEvent)NULL);
    };
};

//---------------------------------------------------------------------------

typedef void __fastcall (__closure *THomingThreadEvent)(System::TObject* Sender, int Axis, int * pReturn, bool * pCancel);

class CHomingThread
{
    class THomingThread : public TThread
    {
    private:
        THomingThreadEvent  OnEvent;
        TObject*            m_Sender;
        int                 m_nAxis;
        int *               m_pReturn;
        bool *              m_pCancel;

    public:
        __fastcall THomingThread(System::TObject* Sender, THomingThreadEvent event, int Axis, int * pReturn, bool * pCancel)
            : TThread(false)
        {
            FreeOnTerminate = true;
            OnEvent     = event;
            m_nAxis     = Axis;
            m_pReturn   = pReturn;
            m_Sender    = Sender;
            m_pCancel   = pCancel;
        }

        void __fastcall Execute()
        {
            try {
                if(OnEvent != NULL) OnEvent(m_Sender, m_nAxis, m_pReturn, m_pCancel);
            }
            __finally {

            }
        }
    };

public:
    CHomingThread(System::TObject* Sender, THomingThreadEvent event, int Axis, int * pReturn, bool * pCancel)
    {
        THomingThread*  thread = new THomingThread(Sender, event, Axis, pReturn, pCancel);
    }
};

//---------------------------------------------------------------------------

typedef void __fastcall (__closure *TClientSocketThreadEvent)(System::TObject* Sender, int Index);

class CClientSocketThread
{
    class TClientSocketThread : public TThread
    {
    private:
        TClientSocketThreadEvent    OnEvent;
        TObject*                    m_Sender;
        int                         m_Index;

    public:
        __fastcall TClientSocketThread(System::TObject* Sender, TClientSocketThreadEvent event, int idx)
            : TThread(false)
        {
            FreeOnTerminate = true;
            OnEvent     = event;
            m_Sender    = Sender;
            m_Index     = idx;
        }   

        void __fastcall Execute()
        {
            try {
                if(OnEvent != NULL) OnEvent(m_Sender, m_Index);
            }
            __finally {

            }
        }
    };

public:
    CClientSocketThread(System::TObject* Sender, TClientSocketThreadEvent event, int idx)
    {
        TClientSocketThread*  thread = new TClientSocketThread(Sender, event, idx);
    }
};

#endif
