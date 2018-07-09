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
    BUTTON_CRNT_DISABLE, // UP/DOWN ���� ���� �״�� DISABLE ��Ŵ. 
    BUTTON_VISIBLE_HIDE,
    BUTTON_STATE_MAX

}emPanelBtnType;

typedef enum enumPanelBtnColorType{
    COLOR_NORMAL = 0,   // UP : clBtnFace + clBlack,  DOWN : clRed + clWhite
    COLOR_RETURN,       // UP : clSkyBlue + clNavy,   DOWN : clRed + clWhite
    COLOR_BLUE,         // UP : clBtnFace + clBlue,   DOWN : clRed + clWhite
    COLOR_VALUE,        // UP : clPurple  + clYellow, DOWN : clPurple  + clYellow
                        // <-- �߰��Ǵ� ��ġ..
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
    * @brief : �־��� ���ڿ��� �־��� ���� ���� ���� ��� 10���� ���ڷ� �̷����
               ���ڿ����� Ȯ���Ѵ�.
    * @param char *str : ���ڿ� 
    * @param unsigned int nLen : ���ڿ� ����
    * @param bool bCareSign : true : +, - ǥ�⵵ ���ڷ� ����Ѵ�. 
    * @param bool bCarePoint : true : �Ҽ���(.)�� ���ڷ� ����Ѵ�. 
    * @return bool : 10���� ���ڷθ� �̷���� ��� true ��ȯ
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    bool __fastcall IsNumStr(char *str, unsigned int nLen, bool bCareSign, bool bCarePoint); // ��Ʈ���� ���ڿ����� Ȯ���Ͽ� ������ ��� true ��ȯ


    /** HexToStr()
    * @brief : Hex ���� String���� ��ȯ�� �ش�. 0x123 �̶�� "123" ���� ��ȯ �Ѵ�. 
    * @param : int n : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013�� 4�� 10�� ������][�۽ſ�] ���� �ۼ� 
    **/    
    AnsiString __fastcall HexToStr(int n, bool bUpper = true);


    /** Sec_To_HHMMSS()
    * @brief : �ʸ� ���ڿ� HH:MM:SS�� �����Ͽ� �����Ѵ�. 
    * @param : int nSec : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013�� 4�� 15�� ������][�۽ſ�] ���� �ۼ� 
    **/    
    AnsiString __fastcall Sec_To_HHMMSS(int nSec);


    /** CurrentTime_SS()
    * @brief : ���� �ð��� INT������ ��ȯ�Ͽ� �����Ѵ�.
    * @param : int nSec :
    * @return AnsiString :
    * @see
    - history : 1. [2013�� 10�� 23�� ������][�����] ���� �ۼ�
    **/    
    int __fastcall CurrentTime_SS();


    /** DivideByIndicator()
    * @brief : sSrc�� �־��� ���ڿ��� cIndicator�� �ش�Ǵ� ���ڷ� �����Ͽ� �߶� sList�� �־��ش�. 
               �߷��� ���ڿ��� ó���� ���κ��� ������ ������. 
    * @param AnsiString sSrc : 
    * @param TStringList * sList : 
    * @param char cIndicator : 
    * @return int : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    int  __fastcall DivideByIndicator(AnsiString sSrc, TStringList * sList, char cIndicator);


    int __fastcall StringToken(UnicodeString &sSrc, TStringList * sList, const wchar_t * sIndicator, bool bTrim);
    
    int __fastcall StringToken(AnsiString &sSrc, TStringList * sList, const char * sIndicator, bool bTrim);


    /** Panel_Down_ActionColor()
    * @brief : TPanel���� TLabel�� Caption�� �����Ͽ� ��ưó�� ���� ���� 
               ���õ� ���·� �׷��ִ� �Լ��̴�. 
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @param bool bEnabled : true : Enable, false : Disable
    * @param TColor DownBgColor : TPanel�� ���� ����. �Է��� ���� ��� default�� clRed �̴�.
    * @param TColor DownTxtColor : TLabel�� Font �����̴�. �Է��� ���� ��� default�� clWhite �̴�.
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
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
    * @param TColor UpBgColor : TPanel�� Enable���¿����� ���� ����. �Է��� ���� ��� default�� clBtnFace �̴�.
    * @param TColor UpTxtColor : TLabel�� Enable���� Font �����̴�. �Է��� ���� ��� default�� clBlue �̴�.
    * @return void : TPanel���� TLabel�� Caption�� �����Ͽ� ��ưó�� ���� ���� 
                    ������ �ȵ� ���·� �׷��ִ� �Լ��̴�. 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall Panel_Up_ActionColor(TObject *Sender, 
                                        TObject *pLable, 
                                        bool bEnabled, 
                                        TColor UpBgColor = clBtnFace, 
                                        TColor UpTxtColor = clBlack);


    /** Panel_CrntDisable()
    * @brief : TPanel���� TLabel�� Caption�� �����Ͽ� ��ưó�� ���� ���� ������ ���� �״�� Disable ���·� �����.  
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    void __fastcall Panel_CrntDisable(TObject *pPanel, TObject *pLable);


    /** PanelBtnState()
    * @brief : TPanel���� TLabel�� Caption�� �����Ͽ� ��ưó�� ���� ���� 
               Up / Down / Disable ���·� �׷��ִ� �Լ��̴�. 
    * @param TObject *pPanel : TPanel Pointer
    * @param TObject *pLable : TLabel Pointer
    * @param emPanelBtnType nState : ��ư�� ���¸� ���³��� enum type 
    * @param emPanelBtnColor nColor : ��ư�� ������ �����ϴ� enum type
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall PanelBtnState( TObject *pPanel, 
                                    TObject *pLable, 
                                    emPanelBtnType nState, 
                                    emPanelBtnColor nColor = COLOR_NORMAL);   


    /** GrideReSetting()
    * @brief : StringGrid �� Col, Row ���� ����� ��� ������ Width, Height �� ���� �ʴ� 
               �������� Cell�� ũ�⸦ �����ִ� �Լ��̴�. 
    * @param TObject *pSG : ������Ʈ ������ 
    * @param int col : ���ϴ� Col. �� 
    * @param int row : ���ϴ� Rwo �� 
    * @param int MaxWidth : ��� �ִ� Width �� (�ȼ�)
    * @param int MaxHeight : ��� �ִ� Height �� (�ȼ�) 
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    void __fastcall GrideReSetting(TObject *pSG, int col, int row, int MaxWidth, int MaxHeight);


    /**  DeleteDoubleSlash()
    * @brief : Error Code File���� Message�� ������ \r\n�� \\r\\n���� ����Ǿ� ��������. 
               �̰��� �ٽ� \r\n���� ����� �Լ��̴�.
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DeleteDoubleSlash(AnsiString &sMsg);
    void __fastcall DeleteDoubleSlash(UnicodeString &sMsg);

    /** DeleteCarriageReturn()
    * @brief : ���ڿ� �ֿ� \r \n ���ڸ� �����Ѵ�. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012�� 7�� 6�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DeleteCarriageReturn(AnsiString &sMsg);
    void __fastcall DeleteCarriageReturn(UnicodeString &sMsg);


    /** DeleteLastCarriageReturn()
    * @brief : ���ڿ��� �������� \r\n�� �����Ѵ�. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012�� 7�� 6�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DeleteLastCarriageReturn(AnsiString &sMsg);


    /** DeleteLastSlash()
    * @brief : ���丮 ��� ���� �������� "\\" ���ڸ� �����Ѵ�. 
    * @param : AnsiString &sMsg : 
    * @return void : 
    * @see 
    - history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DeleteLastSlash(AnsiString &sMsg);


    /** GetGCD_LCM()
    * @brief : �ִ� ������� �ִ� ������� ���ϴ� �Լ�.
    * @param int a : �Է� �� 1
    * @param int b : �԰� �� 2
    * @param int * pgcd : �ִ� ������� ���� 
    * @param int * plcm : �ִ� ������� ����. 
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    void __fastcall GetGCD_LCM(int a, int b, int * pgcd, int * plcm);

    //---------------------------------------------------------------------------
    // String Grid ���� ��. 

    /** StringGridSubCodeView()
    * @brief : String Grid Cell Draw �Լ����� �ش� Cell�� �־������ڷ� �ٽ� �׷��ִ� ������ �Ѵ�. 
     
    * @param : TObject *Sender : TStringGrid ������ 
    * @param : int ACol : ���� Col
    * @param : int ARow : ���� Row
    * @param : TRect &Rect : ���� cell�� Rect
    * @param : TColor clBackColor : B���� 
    * @param : TColor clFontColor : ��Ʈ �� 
    * @param : int nFontSize : ��Ʈ ũ�� 
    * @param : bool bTextAlignCenter : true : �¿� �߾� �迭, false : �� �迭 
    * @param : bool bTextBold : true ; ��Ʈ ����, false : �⺻ 

    * @return void : 
    * @see 
    - history : 1. [2012�� 7�� 9�� ������][�۽ſ�] ���� �ۼ� 
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
    // Directory �� File ���� ��. 

    /** GetLastDirName()
    * @brief : ���丮 ��ο��� ������ ���丮 �̸��� �߶� �������ش�. 
              ex) "c:\\Temp\\Test\\" ��� ��θ� ���ڷγѰ��ظ� "Test" �� ���ϵȴ�. 
    * @param : AnsiString sPath : ���丮 ��� (���� �̸��� ���ԵǸ� �ȵ�)
    * @return AnsiString : ����߿� ������ ���丮 �̸��� �����Ѵ�. 
    * @see 
    - history : 1. [2013�� 3�� 21�� �����][�۽ſ�] ���� �ۼ� 
    **/    
    AnsiString  __fastcall GetLastDirName(AnsiString sPath);


    /** GetFileExt()
    * @brief : ���� �̸����� Ȯ���ڸ� �����Ͽ� �����Ѵ�. "."�� ���Ե��� �ʴ´�. 
               ex1) "File.txt" �� ��� txt�� �����Ѵ�. 
               ex2) "c:\\Temp\\File.txt" �� ���  txt�� �����Ѵ�. 
    * @param : AnsiString sFileName : 
    * @return AnsiString : 
    * @see 
    - history : 1. [2013�� 3�� 21�� �����][�۽ſ�] ���� �ۼ� 
    **/
    AnsiString  __fastcall GetFileExt(AnsiString sFileName);

    /** GetFileSize()
    * @brief : �־��� ���� �̸��� �ش� ������ size�� �����Ѵ�. 
    * @param AnsiString &sFileName : ���� �̸�. (Full Path)
    * @param DWORD * pFileSizeHigh : ���� File Size�� DWORD ������ ��� ��� pFileSizeHigh�� ���� ���� ����. 
    * @return DWORD : File Size�� DWORD ���� ���� ���� �����Ѵ�. 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    DWORD       __fastcall GetFileSize(AnsiString &sFileName, DWORD * pFileSizeHigh);

    /** FindDir()
    * @brief : ������ Path�� ���� ���丮�� ��� ã�� TStringList* pList �� �Ѱ��ش�. 
    * @param AnsiString Path :  ã�� Root ���丮. 
    * @param TStringList * pList : ã�� ���丮 ����Ʈ�� ��ȯ�� ������.  
    * @return bool : �ϳ��� �߰ߵǸ� true ����, �ϳ��� ������ false.
    * @see 
    - history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    bool        __fastcall FindDir(AnsiString Path, __out TStringList * pList);



    /**
    * @fn      : int SetFileDateTime(char *szFilePath, FILETIME ftCreated, FILETIME ftModified, FILETIME ftAccessed)
    * @brief   : ���� �� ������ �ð��� �־��� ���ڷ� �����Ѵ�. 
    * @details : 
                 bool bIgnoreAttrAndApply;  // �� ������ true�� �����ϸ� �Ʒ� ������ ����ȴ�. 
                 bool bAttrHidden;          // Hidden �Ӽ� ���ϵ� ����. 
                 bool bAttrReadOnly;        // ReadOnly �Ӽ� ���ϵ� ����. 
                 bool bAttrSystem;          // System �Ӽ� ���ϵ� ����. 
                 bool bAttrCompressed;      // ���� ���ϵ� ����. 
                 bool bAttrEncrypted;       // Encrypted �Ӽ� ���ϵ� ����. 

    * @param   : char *szFilePath : 
    * @param   : FILETIME ftCreated : 
    * @param   : FILETIME ftModified : 
    * @param   : FILETIME ftAccessed : 
    * @return  : int : 
    * @see     : 
    - history  : 1. [2017�� 5�� 24�� ������][�۽ſ�] ���� �ۼ� 
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
    * @brief : �־��� ���� ���ο� ��� ���ϵ��� ã�� ����Ʈ�� �־��ش�. 
               bSubDir �� true�̸� ���� ���丮�� ã�´�. 
    * @param AnsiString Path : 
    * @param bool bSubDir : 
    * @param __out TStringList * pList : 
    * @return bool : 
    * @see 
    - history : 1. [2013�� 3�� 26�� ȭ����][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall FindFileList(AnsiString Path, bool bSubDir, __out TStringList * pList);



    /**
    * @fn      : TDateTime __fastcall GetFileMakeTime(AnsiString sFilePath);
    * @brief   : �ش� ������ ���� ���ڸ� ���Ѵ�. 
    * @details : 
    * @param   : AnsiString sFilePath : 
    * @return  : TDateTime : 
    * @see     : 
    - history  : 1. [2017�� 4�� 19�� ������][�۽ſ�] ���� �ۼ� 
    **/
    TDateTime __fastcall GetFileMakeTime(AnsiString sFilePath);



    /** SearchFileDate()
    * @brief : ���ڷ� �־��� ���� ���� ���ϵ� �߿� ���� �ֽ��� ������ �ð��� ���� �Ѵ�. 
    * @param AnsiString Mask : 
    * @return TDateTime : 
    * @see 
    - history : 1. [2013�� 3�� 27�� ������][�۽ſ�] ���� �ۼ� 
    **/    
    TDateTime   __fastcall SearchFileDate(AnsiString sDirPath);


    /** SearchLatestFile()
    * @brief : �־��� ��ο��� �־��� ���� �̸��� ������ ���� �߿� ���� �ֽ��� ���ϸ��� �����Ѵ�. 
    * @param AnsiString sDirPath  : ���� ��� 
    * @param AnsiString sFileName : ���Ͽ� ���Ե� ����, ""�� ��� ��� ���� �߿� ���� �ֽ��� ã�´�. 
    * @param AnsiString sExt      : ������ Ȯ����, ""�� ��� ��� Ȯ���ڿ��� ã�´�. 
    * @return AnsiString : ���� �ֽ��� ���� �̸� ����. 
    * @see 
    - history : 1. [2015�� 7�� 2�� �����][�۽ſ�] ���� �ۼ� 
    **/
    AnsiString  __fastcall SearchLatestFile(AnsiString sDirPath, AnsiString sFileName, AnsiString sExt);


    /** DeleteOldTimeFile()
    * @brief : �־��� ����� ���� �Ʒ��� �ִ� �����߿� ���� �ð� ���� ������ ������ �����Ѵ�.
    * @param AnsiString  sSrcPath : ���� ���, �ڿ� "\\"�� ���� �ʾƾ��Ѵ�.
    * @param TDateTime tmStandardTime : ������ ���� �ð�
    * @param int  nTimeType : ������ �ð� �߿� ������ �� Type�� ���Ѵ�.
                                0 : ������ ��¥ ����
                                1 : ������ ���� ��¥
                                2 : ������ Access ��¥
    * @param bool bSubDir : true : ���� ���丮�� �˻��Ͽ� �����Ѵ�.
    * @return bool : true : �ش� ������ ��������� true�� �����Ѵ�. (������ �����ϱ� ����)
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall DeleteOldTimeFile(  AnsiString  sSrcPath,
                                        TDateTime   tmStandardTime,
                                        int         nTimeType,
                                        bool        bSubDir);



    /**
    * @fn      : bool __fastcall TGlobalFuction::DeleteOldTimeDir(AnsiString  sSrcPath, TDateTime tmStandardTime)
    * @brief   : ���ڷ� �־��� �ð����� ������ ������ ã�� �����Ѵ�. 
    * @details : sSrcPath�� �־��� �������� �־��� tmStandardTime �ð� ���� ���� �ð��� ������ ������ ��� �����Ѵ�. 
    * @param   : AnsiString  sSrcPath :         ������ ���� Root ��� (Root ��δ� �������� �ʴ´�.)
    * @param   : TDateTime tmStandardTime :     ������ ���� �ð� (�� �ð����� ������ ������ �����Ѵ�.)
    * @return  : bool : 
    * @see     : 
    - history  : 1. [2017�� 5�� 24�� ������][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall DeleteOldTimeDir(AnsiString  sSrcPath,
                                            TDateTime   tmStandardTime);    

    /** DeleteDirectory()
    * @brief : ������ Path�� ���� ���丮�� ��� ã�� �����Ѵ�.
               bool bDeleteRoot �� true�� �����ϸ� ȣ���� �ڽ��� ���丮�� �����Ѵ�.
               �׷��� false�� �����ϸ� ȣ���� �ڽ��� ���丮�� �״�� �ΰ� �ڽ� ���丮�� ���ϵ鸸 �����Ѵ�.
    * @param AnsiString sSrcPath :  ������ Root ���丮.
    * @param Tbool bDeleteRoot : ȣ���� Root ���丮�� ������ ��� true�� �����Ѵ�.
    * @return void : ����.
    * @see
    - history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ�
    **/   
    void        __fastcall DeleteDirectory(AnsiString  sSrcPath, bool bDeleteRoot);

    /** DeleteDirectory()
    * @brief : ������ Path�� ���� ���丮�� ���� ���丮 �̸��� sDirName�� ���� ã�� ��� ã�� �����Ѵ�.
    * @param AnsiString sSrcPath :  ������ Root ���丮.
    * @param AnsiString sDirName :  ������ ���丮 �̸�.
    * @param  bool bDelete       :  true�̸� ã���� ������ ������ ��� �����Ѵ�. false�̸� ���丮 �̸��� sDirName�ΰ��� ã�� ���۸� �Ѵ�.
    * @return void : ����.
    * @see
    - history : 1. [2013�� 3�� 21�� �ݿ���][�۽ſ�] ���� �ۼ�
    **/    
    void        __fastcall DeleteDirectory(AnsiString  sSrcPath, AnsiString sDirName, bool bDelete = false);


    /** DeleteFileByExtension()
    * @brief : ������ Path�� ���� �߿� Ȯ���ڰ� ������ Ȯ���ڿ� �����ϸ� �����Ѵ�.
               bSubDir �� true�̸� ���� ���丮�� ã�Ƽ� �����Ѵ�.
    * @param AnsiString sSrcPath :  ������ Root ���丮.
    * @param AnsiString sExeName :  ������ ���� Ȯ����.
    * @param  bool bSubDir       :  true�̸� ���� ���丮�� ã�´�.
    * @return void : ����.
    * @see
    - history : 1. [2013�� 3�� 21�� �ݿ���][�۽ſ�] ���� �ۼ�
    **/    
    void        __fastcall DeleteFileByExtension(AnsiString  sSrcPath, AnsiString sExeName, bool bSubDir);


    /** ChangeExtension()
    * @brief : �־��� sFileName���� Ȯ���ڸ� sExt�� �������ش�. 
    * @param AnsiString sFileName : Ȯ���ڸ� �����ϰ����ϴ� ���� �̸����� ��ü ��ΰ� ���ԵǾ ��� ����. 
    * @param AnsiString sExt : Ȯ���ڿ��� ".Exe" ó�� '.'�� �����Ͽ��� �Ѵ�. 
    * @return AnsiString : Ȯ���ڸ� ������ ���� �̸��� �����Ѵ�. 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    AnsiString  __fastcall ChangeExtension(AnsiString sFileName, AnsiString sExt);

    /** CopyFiles()
    * @brief : Ư�� ������ ��� ������ ���� 
    * @param const char *srcfiles :  ���� �̸��� �ִ� Full Path ("c:\\Temp\\*.ini" )
    * @param const char *destdir :   ����� ��ǥ ����. 
    * @return int : 
    * @see 
    - history : 1. [2013�� 3�� 28�� �����][�۽ſ�] ���� �ۼ� 
    **/    
    int         __fastcall CopyFiles(const char *srcfiles, const char *destdir);


    /** DirectoryCopy()
    * @brief : Directory���� Directory�� sub ������ �����Ͽ� ���� �Ѵ�. 
    * @param AnsiString sSrcPath : Source Directory
    * @param AnsiString sTrgPath : Target Directory
    * @param TStringList * slSkipDirectory : sub Directory �߿��� �������� ���ƾ��� �̸��� ������ �� �ִ�. 
    * @return bool : ������ ��� true ����.
    * @see 
    - history : 1. [2015�� 11�� 24�� ȭ����][�۽ſ�] ���� �ۼ� 
    **/    
    bool __fastcall DirectoryCopy(AnsiString sSrcPath, AnsiString sTrgPath, TStringList * slSkipDirectory);

   
    bool __fastcall DirRename(const char * sOldname,const char * sNewname);

    bool __fastcall FileRename(const char * sOldname,const char * sNewname);

    /** ShowDirSelectDlg()
    * @brief : ���丮 ��θ� Ž���Ͽ� �ش� ��θ� �������ִ� ���̾�α� â�� ����Ѵ�. 
               ��� ��� 
               void __fastcall TForm1::Button3Click(TObject *Sender)
                {
                    _TCHAR szPath[MAX_PATH];

                    if( ShowDirSelectDlg( Handle, szPath ) ) {
                        Caption = szPath;
                    }
                }

    * @param HWND hWndOwner : �θ� Form �� �ڵ� 
    * @param _TCHAR *szDir :  ���õ� ��� 
    * @return bool :          ���õ� ��� true ����, ����� ��� false���� 
    * @see 
    - history : 1. [2013�� 3�� 21�� �����][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall ShowDirSelectDlg( HWND hWndOwner, _TCHAR *szDir );


    /** GetApplicationDir()
    * @brief : ���� ���� ������ Root ������ �����Ѵ�. �ڿ� "\\"�� �ٴ´�. 
    * @param : ����.
    * @return AnsiString : �������� Path
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    AnsiString  __fastcall GetApplicationDir() ; // SSY_13.10.10 [ ���������� �����ϴ� Root ���� ���� ���  ] 


    /**
    * @fn      : char __fastcall TGlobalFuction::GetApplicationDiskName()
    * @brief   : 
    * @details : ���� ���� ������ �ִ� Disk�� �̸��� �����ϴ� �Լ�. 
    * @param   : None
    * @return  : char : ã���� ã�� Disk Name�� �����ϰ� ��ã���� '\0' �� �����Ѵ�. 
    * @see     : 
    - history  : 1. [2018�� 1�� 25�� �����][Song Shin Young] First. 
    **/
    char __fastcall GetApplicationDiskName() ;


    /** GetMD5HashCodeFromFile()
    * @brief : File�� MD5 Hash Code�� �����ϴ� �Լ��̴�. 
    * @param __in LPCSTR filename : ���� �̸�(��ü ���)
    * @param __out LPSTR pRetHashCode : ������ MD5 Hash Code�� Hex Type String���� ��ȯ�Ͽ� �����Ѵ�. 
                                        Hash Code Len�� 16 byte�̹Ƿ� Hex Type String���� ��ȯ�ϸ� 32 Byte�� �ȴ�. 
    * @return bool : ����:true, ����:false
    * @see 
    - history : 1. [2015�� 10�� 22�� �����][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall GetMD5HashCodeFromFile(__in LPCSTR filename, __out LPSTR pRetHashCode);


    /**
    * @fn      : bool        __fastcall TGlobalFuction::DriveExists(char cDriveName, bool bOnlyHDD)
    * @brief   : ���ڷ� �־��� ����̺갡 ���� �����ϴ��� Ȯ���ϴ� �Լ�.
    * @details : 
    * @param   : char cDriveName : Ȯ���ϰ����ϴ� ����̺� �̸� 'c', 'd' ...
    * @param   : bool bOnlyHDD : true  : �� ���� true�̸� ���� HDD �� ��츸 true�� �����Ѵ�. 
                                 false : �� ���� false�̸� Removable, Remote, CD-Rom, RamDisk�� true�� �����Ѵ�. 
    * @return  : bool : ����̺� ���� �����ϸ� true�� �����Ѵ�. 
    * @see     : 
    - history  : 1. [2017�� 1�� 4�� ������][�۽ſ�] ���� �ۼ� 
    **/
    bool        __fastcall DriveExists(char cDriveName, bool bOnlyHDD = false);


    /**
    * @fn      : double __fastcall TGlobalFuction::GetDiskFreeSpace(char cDrive)
    * @brief   : 
    * @details : �־��� ��ũ�� ���� ������ MByte ������ �����ϴ� �Լ� 
    * @param   : char cDrive : 
    * @return  : double : ���� ���� MByte
    * @see     : 
    - history  : 1. [2018�� 1�� 25�� �����][Song Shin Young] First. 
    **/
    double      __fastcall GetDiskFreeSpace(char cDrive);








    //---------------------------------------------------------------------------
    // �׸��� ��

    /** DrawGraphBackLine()
    * @brief : Bitmap�� �־��� ũ��� ���� ���̸� �����Ѵ�. 
    * @param Graphics::TBitmap* Bitmap : �׷��� Bitmap ������ 
    * @param int nWidth : ���� 
    * @param int nHeight : ���� 
    * @param int nXpitch : X���� ���� (Width ����)
    * @param int nYpitch : Y���� ���� (Height ����)
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DrawGraphBackLine(  Graphics::TBitmap* Bitmap, 
                                        int nWidth, 
                                        int nHeight, 
                                        int nXpitch, 
                                        int nYpitch);   


    /** DrawImageTrans()
    * @brief : pDest�� srcBmp�� X,Y ��ġ�� ����ó���Ͽ� �׸���. 
    * @param Graphics::TBitmap* pDest : �׷��� ������ Bitmap
    * @param int X : pDest���� �ȼ� ��ǥ (Widht ���� ���� ��)
    * @param int Y : pDest���� �ȼ� ��ǥ (Height ���� ���� ��)
    * @param Graphics::TBitmap* srcBmp : �׸� �ҽ� Bitmap
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall DrawImageTrans(Graphics::TBitmap* pDest, int X, int Y, Graphics::TBitmap* srcBmp);



    /** DrawTextTrans()
    * @brief : pDest Bitmap�� sString ���ڿ��� X,Y ��ġ�� ����� ���� ó���Ͽ� �׸���. 
    * @param Graphics::TBitmap* pDest : �׷��� ������ Bitmap ������
    * @param Graphics::TBitmap* pTextBitmap : ���ڿ��� ���� ó���ϴµ� ����� �ӽ� Bitmap ������
    * @param int             X : �׷��� X ��ġ
    * @param int             Y : �׷��� Y ��ġ
    * @param AnsiString      sString : ����� ���ڿ�
    * @param TColor          cColor : ���� ����
    * @param unsigned int    Align : ������ ��ġ ���� ��� TEXT_ALIGN_BOTTOM, TEXT_ALIGN_V_CENTER, TEXT_ALIGN_RIGHT, TEXT_ALIGN_H_CENTER
    * @param int             nFontSize : ��Ʈ ũ��
    * @param bool            bBold : ��Ʈ ���� ����
    * @return void :
    * @see
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ�
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
    * @brief : TCanvas * �� �־��� �������� �׶��̼��� �׸���.
    * @param TCanvas *   pCanvas : �׷��� ��� Canvas ������
    * @param TColor      StartColor : �׶��̼� ���� ����
    * @param TColor      EndColor :  �׶��̼� �Ϸ� ����
    * @param int         ShadingLevel : ���� ����� �Ϸ� ������ �� ���� �ܰ� (���� Ŭ ���� �ε巯�� ǥ��)
    * @param TRect       &Rect : �׷��� ������ �����Ѵ�.
    * @return void :
    * @see
    - history : 1. [2015�� 11�� 10�� ȭ����][�۽ſ�] ���� �ۼ�
    **/
    void __fastcall DrawGradient(   TCanvas *   pCanvas,
                                    TColor      StartColor,
                                    TColor      EndColor,
                                    int         ShadingLevel,
                                    TRect       &Rect) ;




    /** Raw8Bit_to_Bmp24Bit()
    * @brief : 8 bit Raw ������ �̹����� 24Bit BMP ���Ϸ� �������ش�. 
    * @param BYTE * pRawData : 8 bit Raw Image ����Ÿ 
    * @param TMemoryStream * pStm : 24 Bit BMP �� ����� ������ ��ȯ TMemorySteam ������ 
    * @param int nWidth : �̹��� Width
    * @param int nHeight : �̹��� Height
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    void __fastcall Raw8Bit_to_Bmp24Bit(BYTE * pRawData, TMemoryStream * pStm, int nWidth, int nHeight);


    /** RawData_Bottom_Up()
    * @brief : Image raw data�� ���� ���� ��Ű�� �Լ� 
    * @param BYTE *data : Raw image data ������ 
    * @param int nPixelByte : �� Pixel�� �����ϴ� byte �� 
    * @param int biWidth : �̹��� ���� 
    * @param int biHeight : �̹��� ���� 
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    void __fastcall RawData_Bottom_Up(BYTE *data, int nPixelByte, int biWidth, int biHeight);


    
    //---------------------------------------------------------------------------
    // ���� ��� 

    /** DecimalRounding()
    * @brief : nPoint �� �Ҽ��� �ڸ���ŭ �ݿø��Ͽ� �����Ѵ�. 
               3.14159 �� nPoint �� 2 ��� 3.14�� �����Ѵ�. 
               ���� nPoint�� 3�̶�� 3.142 �� �����Ѵ�. 
    * @param double dSrc : �ݿø��� ������ �� 
    * @param int nPoint :  �ݿø��� �Ҽ��� �ڸ� 
    * @return double :     
    * @see 
    - history : 1. [2013�� 4�� 2�� ȭ����][�۽ſ�] ���� �ۼ� 
    **/    
    double __fastcall DecimalRounding(double dSrc, int nPoint);


    /** CompareDoubleUlps()
    * @brief : double Ÿ�� ���� ������ ������ ���ϱ�
    * @param double x : 
    * @param double y : 
    * @param int ulpsTolerance = 4 : 
    * @return int : 
    * @see 
    - history : 1. [2015�� 4�� 2�� �����][�۽ſ�] ���� �ۼ� 
    **/    
    int    __fastcall CompareDoubleUlps(double x, double y, int ulpsTolerance = 4);


    /** CompareDoubleAbsoulteAndUlps()
    * @brief :  ���� ��� ���� ������ ���ϱ� - ���� ���� + ULP ��� 
                ULP�� �̿��� �񱳰� ���� ���� ���� ������ ����ϴ� ��ó�� ��������,
                1 ULP�� ũ��� ������ ũ�⿡ ���� ���ϴ� ������� ���̱� ������ ��� ���� �񱳿� �ش�ȴٰ� �� �� �ֽ��ϴ�.
                �׷��Ƿ� ������ �����߾��� CompareDoubleRelative �Լ��� ���������� 0�� ����� ���� ���� ���� ���� ����
                (���� �߻��Ǵ� ��Ȳ�� �ƴ�����) ������ �߻��� ������ �����ֽ��ϴ�. 
                �ᱹ �̷��� ��Ȳ���� ����Ϸ��� �Ʒ��� ���� ���� ���� �񱳿� ULP �񱳸� �����Ͽ� �����ؾ� �մϴ�. 

    * @param double x : 
    * @param double y : 
    * @param double absTolerance = (1.0e-8) :
    * @parma int ulpsTolerance = 4 :
    * @return int :  0 : �� ���� ����.
                     1 : x�� y���� ŭ.
                    -1 : X�� Y���� ����.
    * @see 
    - history : 1. [2015�� 4�� 2�� �����][�۽ſ�] ���� �ۼ� 
    **/    
    int    __fastcall CompareDoubleAbsoulteAndUlps(double x, double y, double absTolerance = (1.0e-8), int ulpsTolerance = 4);


    //---------------------------------------------------------------------------
    // COM Port Check

    /** OpenCheckComport()
    * @brief : ComPort�� ���¸� Ȯ���Ͽ� ��� ������ Port���� Ȯ���Ѵ�. 
    * @param : char * szPort : ComPort Name ("COM4")
    * @return bool : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/    
    bool __fastcall OpenCheckComport(char * szPort);

    //---------------------------------------------------------------------------
    // Component ���� �� 

    /* * SetRadioGroupItemIndex()
    * @brief : TRadioGroup �� ItemIndex�� ����� OnClick Event�� �߻��ϴµ�
               �� �Լ��� ����ϸ� OnClick Event ���� ItemIndex�� ������ �� ����.
    * @param TRadioGroup *pRGroup :
    * @param int Index :
    * @return void :
    * @see
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ�
    * */

    void __fastcall SetRadioGroupItemIndex(TRadioGroup * pRGroup, int Index);


    /** ReplaceStr()
    * @brief : sStr ���� sS�� �ش�Ǵ� ���ڿ��� sT�� �ٲ۴�. 
    * @param AnsiString &sStr : ������ ��ü ���ڿ� (���������� ����� �� ���� �̰����� ���ڷ� �ѱ� ������ ���� ����ȴ�.)
    * @param AnsiString sS : ã�� ���ڿ� 
    * @param AnsiString sT : ��ġ�� ���ڿ� 
    * @return void : 
    * @see 
    - history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
    **/
    void __fastcall ReplaceStr(AnsiString &sStr, AnsiString sS, AnsiString sT);


    /**
    * @fn      : int __fastcall TGlobalFuction::StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos)
    * @brief   : 
    * @details : �־��� ���ڿ����� ���� ��ġ ���� �˻� ���ڿ��� ã������ ��ġ ���� ���� ��. (ã�� ��� 1 �̻��� ���� ����)
    * @param   : const AnsiString & sSrcStr : 
    * @param   : AnsiString sSearch : 
    * @param   : Cardinal InitPos : 
    * @return  : int : 0 : ��ã��, 1�̻� : ã�� ���ڿ� ��ġ 
    * @see     : 
    - history  : 1. [2018�� 1�� 17�� ������][Song Shin Young] First. 
    **/
    int __fastcall StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos);


    //---------------------------------------------------------------------------
    // TCP / IP

    /** 
    * @brief : ���� PC�� ��ġ�� LAN ī���� ��� IP LIST�� ���´�. 
    * @param : TStringList * pList : IP String�� ���Ե� List
    * @return bool : ���� ���� �� true ����. 
    * @see 
    - history : 1. [2015�� 7�� 2�� �����][�۽ſ�] ���� �ۼ� 
    **/    
    bool __fastcall LocalIPList(TStringList * pList);

    /**
    * @fn      : AnsiString __fastcall TGlobalFuction::GetFileInformation(AnsiString AFilename, AnsiString AQuery)
    * @brief   : ���������� ���� ������ ������ �Լ� 
    * @details : 
    * @param   : AnsiString AFilename : �������� �̸� (Full Path)
    * @param   : AnsiString AQuery : ����� �ϴ� �׸��� �Է��ϸ� �ȴ�. �Ʒ� ������ �׸��� Ȯ���϶�. 
                                    - CompanyName
                                    - FileDescription
                                    - FileVersion
                                    - InternalName
                                    - LegalCopyright
                                    - LegalTrademarks
                                    - ProductName
                                    - ProductVersion
    * @return  : AnsiString : �ش� ������ AQuery�� �ش��ϴ� ������ ���� �Ѵ�.  
    * @see     : 
    - history  : 1. [2017�� 4�� 19�� ������][�۽ſ�] ���� �ۼ� 
    **/
    AnsiString __fastcall GetFileInformation(AnsiString AFilename, AnsiString AQuery);


    
};

//---------------------------------------------------------------------------

extern TGlobalFuction GFun;

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
 * ���� �� Terminate�Ǹ� �޸� �ڵ� �����Ǵ� Thread �� ������ �� ����Ѵ�. 
 
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
