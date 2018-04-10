//---------------------------------------------------------------------------
#define USE_BOOST_LIB 1     // boost�� ����� ��� 1, ������� ���� ��� 0���� �ϰ� ������


#include <vcl.h>
#include <Grids.hpp>        //
#include <shlobj.h>         // ShowDirSelectDlg()�� ��
#include <shellapi.h>       // SHFileOperation()�� ����.
#include <Wincrypt.h>
#include "GlobalFunction.h"
#include <stdio.h>
#include <string.h>
#include <map>

#if USE_BOOST_LIB
#include <boost/regex.hpp>  // Boost.Regex lib (���� ǥ���� ��� ����)
#endif

#pragma hdrstop

//---------------------------------------------------------------------------

#define DEL_LOG_DIR_EXPRESSION "(\\d{4}-\\d{2}-\\d{2})|(\\d{4}-\\d{2})" // 2014-03-12 or 2014-03 (���� ǥ���� ���)

#pragma package(smart_init)


TGlobalFuction GFun;

//---------------------------------------------------------------------------

TColor BtnColorType[COLOR_MAX][4] = {
//------------------------------------------
//                UP         |      DOWN
//------------------------------------------
//           BG      FRONT   |  BG       FONT
//------------------------------------------
        {clBtnFace, clBlack,  clRed,     clWhite},    	// COLOR_NORMAL
		{clSkyBlue, clNavy,   clRed,     clWhite},    	// COLOR_RETURN
		{clBtnFace, clBlue,   clRed,     clWhite},    	// COLOR_BLUE
		{clPurple,  clYellow, clPurple,  clYellow},    	// COLOR_BLUE
														// <-- �߰��Ǵ� ��ġ..
};

//---------------------------------------------------------------------------

__fastcall TGlobalFuction::TGlobalFuction()
{
    bIgnoreAttrAndApply = false;
    bAttrHidden = false;
    bAttrReadOnly = false;
    bAttrSystem = false;
    bAttrCompressed = false;
    bAttrEncrypted = false;
}

__fastcall TGlobalFuction::~TGlobalFuction()
{

}



//---------------------------------------------------------------------------
/** 
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
bool __fastcall TGlobalFuction::IsNumStr(char *str, unsigned int nLen, bool bCareSign, bool bCarePoint)
{
    bool bPointExist = false;

    // NULL �� ��� false
    if(str == NULL) return false;

    // ���ڿ��� ���� ���̺��� ���ڷ� ���� ���̰� �� ��� ó��.
    nLen = strlen(str) < nLen ? strlen(str) : nLen;

    // ���ڿ� ó���� ���� �Ҽ����� ��Ÿ���� ���� ���� ��� false
    if((str[0] == '.') || (str[nLen-1] == '.')) return false;

    // ���� �տ� -, + ��θ� ���ڷ� �ν��� ��� �ش� ��ȣ�� �ִ��� �Ǻ��Ͽ�
    // ��������� �۾��̴�. 
    if(bCareSign){
        if((str[0] == '-') || (str[0] == '+')){
            str++;
            nLen--;
            if(nLen == 0) return false;
        }
    }

    while(nLen){
        nLen--;
        // �Ҽ����� ������ ��� (�Ҽ����� �� �ϳ��� �����ؾ��Ѵ�.)
        if(bCarePoint && !bPointExist && (str[nLen] == '.')) {
            bPointExist = true;
        }
        else if((str[nLen] < '0') || (str[nLen] > '9') ) {
            return false;
        }
    }

    return true;
}


/** 
* @brief : Hex ���� String���� ��ȯ�� �ش�. 0x123 �̶�� "123" ���� ��ȯ �Ѵ�. 
* @param : int n : 
* @return AnsiString : 
* @see 
- history : 1. [2013�� 4�� 10�� ������][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::HexToStr(int n, bool bUpper)
{
    AnsiString sTemp;
    if(bUpper) sTemp.printf("%0X", n);
    else       sTemp.printf("%0x", n);
    return sTemp;
}

/**
* @brief : ���� �ð��� INT������ ��ȯ�Ͽ� �����Ѵ�.
* @param : int nSec :
* @return AnsiString :
* @see
- history : 1. [2013�� 10�� 23�� ������][�����] ���� �ۼ�
**/
int __fastcall TGlobalFuction::CurrentTime_SS()
{
    int sTime = 0;

    unsigned short Hour, Min, Sec, MSec;
    TDateTime dtPresent = Now(); // ���� �ð�.

    // ���� TDateTime�� ��,��,�� ��,��,�� �� ������.
    DecodeTime(dtPresent, Hour, Min, Sec, MSec);

    sTime += Hour*60*60;
    sTime += Min*60;
    sTime += Sec;

    return sTime;
}

/** 
* @brief : �ʸ� ���ڿ� HH:MM:SS�� �����Ͽ� �����Ѵ�. 
* @param : int nSec : 
* @return AnsiString : 
* @see 
- history : 1. [2013�� 4�� 15�� ������][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::Sec_To_HHMMSS(int nSec)
{
    AnsiString sTime;

    int nSecond = nSec % 60;
    int nMinute = (nSec % 3600) / 60;
    int nHour   = nSec / 3600;

    sTime.printf("%d:%02d:%02d", nHour, nMinute, nSecond);
    return sTime;
}
//---------------------------------------------------------------------------
/** 
* @brief : sSrc�� �־��� ���ڿ��� cIndicator�� �ش�Ǵ� ���ڷ� �����Ͽ� �߶� sList�� �־��ش�. 
           �߷��� ���ڿ��� ó���� ���κ��� ������ ������. 
* @param AnsiString sSrc : 
* @param TStringList * sList : 
* @param char cIndicator : 
* @return int : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
int __fastcall TGlobalFuction::DivideByIndicator(AnsiString sSrc, TStringList * sList, char cIndicator)
{
    int nDivideCnt = 0; // Divide Count
    int nSrcLen = 0;    // Source String Lenght
    int nTmpCnt = 0;
    char temp[500];
    
    AnsiString sBuf;

    if(sList == NULL) return 0;
    else if(sSrc == "") return 0;

    memset(temp, '\0', sizeof(temp));

    nSrcLen = sSrc.Length();

    nTmpCnt = 0;
    nDivideCnt = 0;
    for(int i=0; i<nSrcLen; i++){
        if(sSrc.c_str()[i] == cIndicator){
            nTmpCnt = 0;
            sBuf = temp;
            sBuf = sBuf.Trim();
            sList->Add(sBuf);
            memset(temp, '\0', sizeof(temp));
            nDivideCnt++;
            sBuf = "";
        }
        else{
            temp[nTmpCnt] = sSrc.c_str()[i];
            nTmpCnt++;

            if((i+1) == nSrcLen) {
                sBuf = temp;
                sBuf = sBuf.Trim();
                sList->Add(sBuf);
                nDivideCnt++;
                sBuf = "";
                break;
            }
        }
    }

    /* --------------------------------------------
    // SSY_081029 [ LOT ON ������ �ڸ��� �κ� ���� ] 
    int nComaPos = 0;
    sSrc = sSrc.Trim();

    while(sSrc.Length()){
        nComaPos = sSrc.AnsiPos(",");
        
        if(nComaPos == 1){
            sSrc = sSrc.Delete(1,1);
            sSrc = sSrc.Trim();
        }
        else if(nComaPos > 1){
            sBuf = sSrc.SubString(1, (nComaPos-1)); // �޸� �������� ��Ʈ�� ���ϱ� 
            sBuf = sBuf.Trim();

            nDivideCnt++;
            sList->Add(sBuf);                       // ����Ʈ�� ���ϱ� 

            sSrc = sSrc.Delete(1, nComaPos);        // �޸����� ���� 
            sSrc = sSrc.Trim();            
        }
        else if(nComaPos == 0){                     // ������ ��Ʈ���� ����. 
            nDivideCnt++;
            sList->Add(sSrc); 
        }
    }
    -------------------------------------------- */

    return nDivideCnt;
}
//---------------------------------------------------------------------------

/**
* @fn      : int __fastcall TGlobalFuction::StringToken(String sSrc, TStringList * sList, char cIndicator, bool bTrim)
* @brief : sSrc�� �־��� ���ڿ��� cIndicator�� �ش�Ǵ� ���ڷ� �����Ͽ� �߶� sList�� �־��ش�. 
           �߷��� ���ڿ��� ó���� ���κ��� ������ bTrim���ڰ� true�̸� �����ϰ� false�̸� �����Ѵ�. 
* @param String sSrc : 
* @param TStringList * sList : 
* @param wchar_t * sIndicator : �����ڷ� ����� �ɸ��͸� ���ڿ��� �����Ѵ�. (ex: L","�޸��� ����,   L";," �����ݷа� �޸��� �����ڷ� ���)
* @param bool bTrim : �߶��� ���ڿ��� Trim ���� ���� ����. 
* @return int : 
* @see 
- history  : 1. [2017�� 11�� 17�� �ݿ���][Song Shin Young] First. 
**/
int __fastcall TGlobalFuction::StringToken(String &sSrc, TStringList * sList, wchar_t *sIndicator, bool bTrim)
{
    if(sList == NULL) return 0;
    else if(sSrc.Length() == 0) return 0;

    int nDivideCnt = 0; // Divide Count
    int nStrCnt = sSrc.Length() + 1;

    // ������ ȸ������ �ʱ� ���� ���۸� ����� ����Ѵ�. 
    wchar_t *wstr = new wchar_t[nStrCnt];
    ZeroMemory(wstr, sizeof(wchar_t) * nStrCnt);
    memcpy(wstr, sSrc.w_str(), sizeof(wchar_t) * sSrc.Length());
    
    wchar_t *tk = wcstok(wstr, sIndicator);

    while (tk != NULL)
    {
        nDivideCnt += 1;    
        sList->Add(tk);
        if(bTrim) {
            sList->Strings[sList->Count-1] = sList->Strings[sList->Count-1].Trim();
        }
        tk = wcstok(NULL, sIndicator);
    }
    
    delete[] wstr;
    
    return nDivideCnt;
}
//---------------------------------------------------------------------------
/**
* @fn      : int __fastcall TGlobalFuction::StringToken(AnsiString sSrc, TStringList * sList, char *sIndicator, bool bTrim)
* @brief : sSrc�� �־��� ���ڿ��� sIndicator �ش�Ǵ� ���ڷ� �����Ͽ� �߶� sList�� �־��ش�. 
           �߷��� ���ڿ��� ó���� ���κ��� ������ bTrim���ڰ� true�̸� �����ϰ� false�̸� �����Ѵ�. 
* @param AnsiString sSrc : 
* @param TStringList * sList : 
* @param char * sIndicator : �����ڷ� ����� �ɸ��͸� ���ڿ��� �����Ѵ�. (ex: ","�޸��� ����,   ";," �����ݷа� �޸��� �����ڷ� ���)
* @param bool bTrim : �߶��� ���ڿ��� Trim ���� ���� ����. 
* @return int : 
* @see     : 
- history  : 1. [2017�� 11�� 17�� �ݿ���][Song Shin Young] First. 
**/
int __fastcall TGlobalFuction::StringToken(AnsiString &sSrc, TStringList * sList, char * sIndicator, bool bTrim)
{
    if(sList == NULL) return 0;
    else if(sSrc.Length() == 0) return 0;

    int nDivideCnt = 0; // Divide Count

    // ������ ȸ������ �ʱ� ���� ���۸� ����� ����Ѵ�. 
    char *str = new char[sSrc.Length() + 1];
    ZeroMemory(str, sSrc.Length() + 1);
    memcpy(str, sSrc.c_str(), sSrc.Length());
    
    char *tk = strtok(str, sIndicator);

    while (tk != NULL)
    {
        nDivideCnt += 1;
        sList->Add(tk);
        if(bTrim) {
            sList->Strings[sList->Count-1] = sList->Strings[sList->Count-1].Trim();
        }        
        tk = strtok(NULL, sIndicator);
    }

    delete[] str;

    return nDivideCnt;    
}
//---------------------------------------------------------------------------


/** 
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
void __fastcall TGlobalFuction::Panel_Down_ActionColor(TObject *pPanel, TObject *pLable, bool bEnabled, TColor DownBgColor, TColor DownTxtColor)
{
    TPanel * panPanel = NULL;
    TLabel * lblLabel = NULL;
    panPanel = dynamic_cast<TPanel *>(pPanel);
    lblLabel = dynamic_cast<TLabel *>(pLable);

    if(panPanel != NULL){
        panPanel->Enabled = bEnabled;
        panPanel->BevelOuter = bvLowered;
        if(bEnabled){
            panPanel->Color = DownBgColor;
            panPanel->Font->Color = DownTxtColor;
        }
        else {
            panPanel->Color = DownBgColor;
            panPanel->Font->Color = DownTxtColor;        
        }
    }

    if(lblLabel != NULL){
        lblLabel->Enabled = bEnabled;
        lblLabel->Font->Color = DownTxtColor;
    }      
}
//---------------------------------------------------------------------------

/** 
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
void __fastcall TGlobalFuction::Panel_Up_ActionColor(TObject *pPanel, TObject *pLable,bool bEnabled, TColor UpBgColor, TColor UpTxtColor)
{
    TPanel * panPanel = NULL;
    TLabel * lblLabel = NULL;
    panPanel = dynamic_cast<TPanel *>(pPanel);
    lblLabel = dynamic_cast<TLabel *>(pLable);

    if(panPanel != NULL){
        panPanel->Enabled = bEnabled;
        panPanel->BevelOuter = bvRaised;
        if(bEnabled){
            panPanel->Color = UpBgColor;
            panPanel->Font->Color = UpTxtColor;
        }
        else {
            panPanel->Color = clBtnFace;
            panPanel->Font->Color = clWhite;        
        }
    }

    if(lblLabel != NULL){
        lblLabel->Enabled = bEnabled;
        lblLabel->Font->Color = UpTxtColor;
    }      
}

//---------------------------------------------------------------------------

/** 
* @brief : TPanel���� TLabel�� Caption�� �����Ͽ� ��ưó�� ���� ���� ������ ���� �״�� Disable ���·� �����.  
* @param TObject *pPanel : TPanel Pointer
* @param TObject *pLable : TLabel Pointer
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::Panel_CrntDisable(TObject *pPanel, TObject *pLable)
{
    TPanel * panPanel = NULL;
    TLabel * lblLabel = NULL;
    panPanel = dynamic_cast<TPanel *>(pPanel);
    lblLabel = dynamic_cast<TLabel *>(pLable);

    if(panPanel != NULL){
        panPanel->Enabled = false;
        panPanel->Color = clBtnFace;
        panPanel->Font->Color = clWhite;        
    }

    if(lblLabel != NULL){
        lblLabel->Enabled = false;
    }      
}


/** 
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
void __fastcall TGlobalFuction::PanelBtnState(TObject *pPanel, TObject *pLable, emPanelBtnType nState, emPanelBtnColor nColor)
{
    TPanel * panPanel = dynamic_cast<TPanel *>(pPanel);
    TLabel * lblLabel = dynamic_cast<TLabel *>(pLable);

    TColor clUpBG, clUpFont, clDownBG, clDownFont;

    if(panPanel == NULL) return; // SSY_11.07.19 [   ] 

    clUpBG     = BtnColorType[nColor][0];
    clUpFont   = BtnColorType[nColor][1];
    clDownBG   = BtnColorType[nColor][2];
    clDownFont = BtnColorType[nColor][3];

    switch(nState){
        case BUTTON_DOWN_ENABLE :
            panPanel->Visible = true;
            Panel_Down_ActionColor(panPanel, lblLabel, true, clDownBG, clDownFont);
            break;
            
        case BUTTON_DOWN_DISABLE :
            panPanel->Visible = true;
            Panel_Down_ActionColor(panPanel, lblLabel, false, clDownBG, clDownFont);
            break;
            
        case BUTTON_UP_ENABLE :
            panPanel->Visible = true;
            Panel_Up_ActionColor(panPanel, lblLabel, true, clUpBG, clUpFont);
            break;
            
        case BUTTON_UP_DISABLE :
            panPanel->Visible = true;
            Panel_Up_ActionColor(panPanel, lblLabel, false, clUpBG, clUpFont);
            break;

        case BUTTON_CRNT_DISABLE:
            panPanel->Visible = true;
            Panel_CrntDisable(panPanel, lblLabel);
            break;

        case BUTTON_VISIBLE_HIDE:
            panPanel->Visible = false;
            break;
            
        default:
            break;
    }

}

/** 
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
void __fastcall TGlobalFuction::GrideReSetting(TObject *pSG, int col, int row, int MaxWidth, int MaxHeight)
{

    TStringGrid * StrGrid = dynamic_cast<TStringGrid *>(pSG);
    if(pSG == NULL) return;

    if(col <= 0 || row <= 0) return;

    StrGrid->ColCount = col;
    //int nTemp = MaxWidth / col;
    int nTemp = ((MaxWidth - 3) / col) - StrGrid->GridLineWidth;
    StrGrid->DefaultColWidth = nTemp;
    StrGrid->Width = (nTemp + StrGrid->GridLineWidth) * col + 3;


    StrGrid->RowCount = row;
    //nTemp = MaxHeight / row;
    nTemp = ((MaxHeight - 3) / row) - StrGrid->GridLineWidth;
    StrGrid->DefaultRowHeight = nTemp;
    StrGrid->Height = (nTemp + StrGrid->GridLineWidth) * row + 3;
}


/** 
* @brief : Error Code File���� Message�� ������ \r\n�� \\r\\n���� ����Ǿ� ��������. 
           �̰��� �ٽ� \r\n���� ����� �Լ��̴�.
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DeleteDoubleSlash(AnsiString &sMsg)
{
    int nPos = sMsg.Pos("\\r");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        sMsg.Insert("\r", nPos);
        nPos = sMsg.Pos("\\r");
    }

    nPos = sMsg.Pos("\\n");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        sMsg.Insert("\n", nPos);
        nPos = sMsg.Pos("\\n");
    }    
}

void __fastcall TGlobalFuction::DeleteDoubleSlash(UnicodeString &sMsg)
{
    int nPos = sMsg.Pos("\\r");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        sMsg.Insert(L"\r", nPos);
        nPos = sMsg.Pos(L"\\r");
    }

    nPos = sMsg.Pos(L"\\n");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        sMsg.Insert(L"\n", nPos);
        nPos = sMsg.Pos(L"\\n");
    }    
}


/** 
* @brief : ���丮 ��� ���� �������� "\\" ���ڸ� �����Ѵ�. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DeleteLastSlash(AnsiString &sMsg)
{
    //sMsg = ExcludeTrailingBackslash(sMsg);
    
    if(sMsg.Length() == 0) return;
    
    while(sMsg.SubString(sMsg.Length(), 1) == "\\") {
        sMsg.Delete(sMsg.Length(), 1);
    }
}

/** 
* @brief : ���ڿ� �ֿ� \r \n ���ڸ� �����Ѵ�. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012�� 7�� 6�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DeleteCarriageReturn(AnsiString &sMsg)
{
    int nPos = sMsg.Pos("\r\n");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        nPos = sMsg.Pos("\r\n");
    }
    
    nPos = sMsg.Pos("\r");
    while(nPos) {
        sMsg.Delete(nPos, 1);
        nPos = sMsg.Pos("\r");
    }

    nPos = sMsg.Pos("\n");
    while(nPos) {
        sMsg.Delete(nPos, 1);
        nPos = sMsg.Pos("\n");
    }    
}

void __fastcall TGlobalFuction::DeleteCarriageReturn(UnicodeString &sMsg)
{
    int nPos = sMsg.Pos(L"\r\n");
    while(nPos) {
        sMsg.Delete(nPos, 2);
        nPos = sMsg.Pos(L"\r\n");
    }
    
    nPos = sMsg.Pos(L"\r");
    while(nPos) {
        sMsg.Delete(nPos, 1);
        nPos = sMsg.Pos(L"\r");
    }

    nPos = sMsg.Pos(L"\n");
    while(nPos) {
        sMsg.Delete(nPos, 1);
        nPos = sMsg.Pos(L"\n");
    }    
}



/** 
* @brief : ���ڿ��� �������� \r\n�� �����Ѵ�. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012�� 7�� 6�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DeleteLastCarriageReturn(AnsiString &sMsg)
{
    int nLen = sMsg.Length();

    while(nLen) {
        if(sMsg.c_str()[nLen-1] == '\r' || sMsg.c_str()[nLen-1] == '\n') {
            sMsg.Delete(nLen, 1);
            nLen = sMsg.Length();
        }
        else {
            break;
        }
    }
}

/** 
* @brief : ���丮 ��ο��� ������ ���丮 �̸��� �߶� �������ش�. 
          ex) "c:\\Temp\\Test\\" ��� ��θ� ���ڷγѰ��ظ� "Test" �� ���ϵȴ�. 
* @param : AnsiString sPath : ���丮 ��� (���� �̸��� ���ԵǸ� �ȵ�)
* @return AnsiString : ����߿� ������ ���丮 �̸��� �����Ѵ�. 
* @see 
- history : 1. [2013�� 3�� 21�� �����][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::GetLastDirName(AnsiString sPath)
{
    sPath = ExcludeTrailingBackslash(sPath);
    int nPos = sPath.LastDelimiter("\\");

    if(nPos == 0) return "";
    if(nPos >= sPath.Length()) return "";

    sPath = sPath.SubString(nPos + 1, sPath.Length() - nPos);
    return sPath;
}


//---------------------------------------------------------------------------
/** 
* @brief : ���� �̸����� Ȯ���ڸ� �����Ͽ� �����Ѵ�. "."�� ���Ե��� �ʴ´�. 
           ex1) "File.txt" �� ��� txt�� �����Ѵ�. 
           ex2) "c:\\Temp\\File.txt" �� ���  txt�� �����Ѵ�. 
* @param : AnsiString sFileName : 
* @return AnsiString : 
* @see 
- history : 1. [2013�� 3�� 21�� �����][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::GetFileExt(AnsiString sFileName)
{
    sFileName = ExtractFileExt(sFileName);
    if((sFileName != "") && (sFileName.c_str()[0] == '.')) {
        sFileName = sFileName.SubString(2, sFileName.Length()-1);
    }

    return sFileName;
}




/** 
* @brief : �־��� ���� �̸��� �ش� ������ size�� �����Ѵ�. 
* @param AnsiString &sFileName : ���� �̸�. (Full Path)
* @param DWORD * pFileSizeHigh : ���� File Size�� DWORD ������ ��� ��� pFileSizeHigh�� ���� ���� ����. 
* @return DWORD : File Size�� DWORD ���� ���� ���� �����Ѵ�. 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
DWORD  __fastcall TGlobalFuction::GetFileSize(AnsiString &sFileName, DWORD * pFileSizeHigh)
{

    DWORD    FileSizeHigh, FileSizeLow; 
    HANDLE   hFindFile; 
    OFSTRUCT ReOpenBuff; 

	hFindFile   = (HANDLE) ::OpenFile(sFileName.c_str(), &ReOpenBuff, OF_READ);
	FileSizeLow =          ::GetFileSize(hFindFile, &FileSizeHigh);
	_lclose((HFILE)hFindFile);

    if(pFileSizeHigh != NULL) *pFileSizeHigh = FileSizeHigh;
    
    return FileSizeLow;
}

/** 
* @brief : �ִ� ������� �ִ� ������� ���ϴ� �Լ�.
* @param int a : �Է� �� 1
* @param int b : �԰� �� 2
* @param int * pgcd : �ִ� ������� ���� 
* @param int * plcm : �ִ� ������� ����. 
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::GetGCD_LCM(int a, int b, int * pgcd, int * plcm)
{
    int z;
    int x = a;
    int y = b;

    while(true) {
        ::Sleep(1);
                        
        z = x % y;

        if(0 == z) break;

        x = y;
        y = z;
    }

    *pgcd = y;
    *plcm = (a * b) / y;
}



//---------------------------------------------------------------------------

/** 
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
void __fastcall TGlobalFuction::StringGridSubCodeView(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TColor clBackColor,
          TColor clFontColor,
          int nFontSize,
          bool bTextAlignCenter,
          bool bTextBold)
{
    TStringGrid * pSG = dynamic_cast<TStringGrid *>(Sender);

    if(pSG == NULL) return;

    // ���� �� ���.
    TColor clOrgBrush  = pSG->Canvas->Brush->Color;
    TColor clOrgFontColor = pSG->Font->Color;
    int nOrgFontSize   = pSG->Font->Size;
    TFontStyles OrgFontStyle = pSG->Canvas->Font->Style;

    // ���ο� �� ����.
    pSG->Canvas->Brush->Color = clBackColor;
    pSG->Canvas->Font->Size = nFontSize;
    
    if(bTextBold)
        pSG->Canvas->Font->Style = TFontStyles() << fsBold;
    else
        pSG->Canvas->Font->Style = TFontStyles() >> fsBold;


    
    pSG->Canvas->FillRect(Rect);
    int nTextH = pSG->Canvas->TextHeight(pSG->Cells[ACol][ARow]);
    int nTextL = Rect.Left + 2;

    if(bTextAlignCenter) {
        nTextL = Rect.Left + (Rect.Width() - pSG->Canvas->TextWidth(pSG->Cells[ACol][ARow])) / 2;
    }

    int nTextT = Rect.Top + ((Rect.Bottom - Rect.Top) / 2) - (nTextH / 2);
    pSG->Canvas->Font->Color = clFontColor;
    
    pSG->Canvas->TextOutA(nTextL, nTextT, pSG->Cells[ACol][ARow]);


    // ���� �� ����.
    pSG->Canvas->Brush->Color = clOrgBrush;
    pSG->Canvas->Font->Size = nOrgFontSize;
    pSG->Canvas->Font->Color = clOrgFontColor;
    pSG->Canvas->Font->Style = OrgFontStyle;
}




/**
* @fn      : int SetFileDateTime(char *szFilePath, FILETIME ftCreated, FILETIME ftModified, FILETIME ftAccessed)
* @brief   : ���� �� ������ �ð��� �־��� ���ڷ� �����Ѵ�. 
* @details : 
* @param   : char *szFilePath : 
* @param   : FILETIME ftCreated : 
* @param   : FILETIME ftModified : 
* @param   : FILETIME ftAccessed : 
* @return  : int : 
* @see     : 
- history  : 1. [2017�� 5�� 24�� ������][�۽ſ�] ���� �ۼ� 
**/

// its an independent function, will set the date and time
///   its a core function in this tool
int __fastcall TGlobalFuction::SetFileDateTime(char *szFilePath, TDateTime dtCreated, TDateTime dtModified, TDateTime dtAccessed)
{
  DWORD dwAttr;
  dwAttr = GetFileAttributes(szFilePath);
    
  if(bIgnoreAttrAndApply)
  {//to ignore mean resets the file/folder attributes to normal
   //         and again it will be set back

      if(FILE_ATTRIBUTE_HIDDEN&dwAttr)
          if(!bAttrHidden)
              return 0;
      if(FILE_ATTRIBUTE_READONLY&dwAttr)
          if(!bAttrReadOnly)
              return 0;
      if(FILE_ATTRIBUTE_SYSTEM&dwAttr)
          if(!bAttrSystem)
              return 0;
      if(FILE_ATTRIBUTE_COMPRESSED&dwAttr)
          if(!bAttrCompressed)
              return 0;
      if(FILE_ATTRIBUTE_ENCRYPTED&dwAttr)
          if(!bAttrEncrypted)
              return 0;

      SetFileAttributes(szFilePath,FILE_ATTRIBUTE_NORMAL);
  }
  else
  {//if not to ignore, then the following 
   //attributed file/folder can not be set to new file x date time

      if(FILE_ATTRIBUTE_HIDDEN&dwAttr)
          return 0;
      if(FILE_ATTRIBUTE_READONLY&dwAttr)
          return 0;
      if(FILE_ATTRIBUTE_SYSTEM&dwAttr)
          return 0;
      if(FILE_ATTRIBUTE_COMPRESSED&dwAttr)
          return 0;
      if(FILE_ATTRIBUTE_ENCRYPTED&dwAttr)
          return 0;
  }

  // open the file/folder
  HANDLE hFile = CreateFile(szFilePath,
        GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

  if(hFile == INVALID_HANDLE_VALUE)
      return 1;

    SYSTEMTIME      SystemTime;
    FILETIME        ftCreated;
    FILETIME        ftAccessed;
    FILETIME        ftModified;

    DateTimeToSystemTime(dtCreated,    SystemTime);
    SystemTimeToFileTime(&SystemTime, &ftCreated);
    DateTimeToSystemTime(dtModified,   SystemTime);
    SystemTimeToFileTime(&SystemTime, &ftAccessed);
    DateTimeToSystemTime(dtAccessed,   SystemTime);
    SystemTimeToFileTime(&SystemTime, &ftModified);

    //set date time
    BOOL bVal = SetFileTime(hFile,&ftCreated,&ftAccessed,&ftModified);
    
    if(!bVal)
    {
      char *szBuff = (char*)malloc(sizeof(char)*1024);
      LPVOID lpMsgBuf;
      int err = GetLastError();

      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
              NULL, 
              err, 
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
              (LPTSTR) &lpMsgBuf, 
              0, 
              NULL);

      sprintf(szBuff,"Unable to set date to \n  %s \n\nReason:%s \nError No:%d",
              szFilePath,lpMsgBuf,err),

      MessageBox(NULL,(LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
              LocalFree(lpMsgBuf);
    }

  // close the handle
      CloseHandle(hFile);
    
  // if it is ignore then it should be set back to its original attribute
  if(bIgnoreAttrAndApply)
      SetFileAttributes(szFilePath,dwAttr);
    
  if(!bVal)
      return 2;
  return 0;
}


//---------------------------------------------------------------------------

/** 
* @brief : ������ Path�� ���� ���丮�� ��� ã�� TStringList* pList �� �Ѱ��ش�. 
* @param AnsiString Path :  ã�� Root ���丮. 
* @param TStringList * pList : ã�� ���丮 ����Ʈ�� ��ȯ�� ������.  
* @return bool : �ϳ��� �߰ߵǸ� true ����, �ϳ��� ������ false.
* @see 
- history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
bool __fastcall TGlobalFuction::FindDir(AnsiString Path, __out TStringList * pList)
{
    if(pList == NULL) return false;

    pList->Clear();

    AnsiString sFindPath = Path;

    DeleteLastSlash(sFindPath);

    sFindPath = sFindPath + "\\*.*";
    
    TSearchRec sr;
    //�Ķ���ͷ� ������ Ƽ���丮���� ���� ���丮�� ã�´�.
    if(FindFirst(sFindPath, faDirectory, sr) == 0){
        while(FindNext(sr)==0) {
            if(sr.Attr == faDirectory) {
                if((sr.Name != "." && sr.Name != "..")){
                    pList->Add(sr.Name);
                }
            }
        }
    }
    else {
        return false;
    }
    
    FindClose(sr);

    if(pList->Count > 0) return true;
    
    return false;
}


/** 
* @brief : �־��� ���� ���ο� ��� ���ϵ��� ã�� ����Ʈ�� �־��ش�. 
           bSubDir �� true�̸� ���� ���丮�� ã�´�. 
* @param AnsiString Path : 
* @param bool bSubDir : 
* @param __out TStringList * pList : 
* @return bool : 
* @see 
- history : 1. [2013�� 3�� 26�� ȭ����][�۽ſ�] ���� �ۼ� 
**/
bool __fastcall TGlobalFuction::FindFileList(AnsiString Path, bool bSubDir, __out TStringList * pList)
{
    if(pList == NULL) return false;

    //pList->Clear();

    AnsiString sFindPath = Path;

    DeleteLastSlash(sFindPath);

    sFindPath = sFindPath + "\\*.*";
    
    TSearchRec sr;
    //�Ķ���ͷ� ������ Ƽ���丮���� ���� ���丮�� ã�´�.
    if(FindFirst(sFindPath, faAnyFile, sr) == 0){
        while(FindNext(sr)==0) {
            if(sr.Attr == faDirectory) {
                if((sr.Name != "." && sr.Name != "..")){
                    if(bSubDir) {
                        FindFileList(Path + "\\" + sr.Name, bSubDir, pList);
                    }
                }
            }
            else {
                pList->Add(sr.Name);
            }
        }
    }
    else {
        return false;
    }
    
    FindClose(sr);

    if(pList->Count > 0) return true;
    
    return false;
}


/**
* @fn      : TDateTime __fastcall GetFileMakeTime(AnsiString sFilePath);
* @brief   : �ش� ������ ���� ���ڸ� ���Ѵ�. 
* @details : 
* @param   : AnsiString sFilePath : 
* @return  : TDateTime : 
* @see     : 
- history  : 1. [2017�� 4�� 19�� ������][�۽ſ�] ���� �ۼ� 
**/
TDateTime __fastcall TGlobalFuction::GetFileMakeTime(AnsiString sFilePath)
{
    TSearchRec sr;

    TDateTime DateTime;
    TDateTime DateTimeTmp;

    if (FindFirst(sFilePath, faAnyFile, sr) == 0) {
        do {
            if ((sr.Attr & faDirectory) != sr.Attr) {
                DateTimeTmp = sr.TimeStamp;

                FILETIME    TempTime;
                SYSTEMTIME  SystemTime;
                
                FileTimeToLocalFileTime( &sr.FindData.ftLastWriteTime, &TempTime );
                FileTimeToSystemTime( &TempTime, &SystemTime );

                DateTime = SystemTimeToDateTime(SystemTime);                    

            }
        } while(FindNext(sr) == 0);
    }

    FindClose(sr);

    //Return.
    return DateTime;
}

//---------------------------------------------------------------------------
/** 
* @brief : ���ڷ� �־��� ���� ���� ���ϵ� �߿� ���� �ֽ��� ������ �ð��� ���� �Ѵ�. 
* @param AnsiString Mask : 
* @return TDateTime : 
* @see 
- history : 1. [2013�� 3�� 27�� ������][�۽ſ�] ���� �ۼ� 
**/
TDateTime __fastcall TGlobalFuction::SearchFileDate(AnsiString sDirPath)
{
    TSearchRec sr;
    DeleteLastSlash(sDirPath);
    AnsiString SerchFile = sDirPath + "\\*.*";
    //int nTime = 0;
    TDateTime DateTime;
    TDateTime DateTimeTmp;

    if (FindFirst(SerchFile, faAnyFile, sr) == 0) {
        do {
            if ((sr.Attr & faDirectory) != sr.Attr) {
                if (sr.TimeStamp > DateTimeTmp) {
                    DateTimeTmp = sr.TimeStamp;

                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;
                    
                    FileTimeToLocalFileTime( &sr.FindData.ftLastWriteTime, &TempTime );
                    FileTimeToSystemTime( &TempTime, &SystemTime );

                    DateTime = SystemTimeToDateTime(SystemTime);                    
                }

            }
        } while(FindNext(sr) == 0);
    }

    FindClose(sr);

    //Return.
    return DateTime;
}


/** 
* @brief : �־��� ��ο��� �־��� ���� �̸��� ������ ���� �߿� ���� �ֽ��� ���ϸ��� �����Ѵ�. 
* @param AnsiString sDirPath  : ���� ��� 
* @param AnsiString sFileName : ���Ͽ� ���Ե� ����, ""�� ��� ��� ���� �߿� ���� �ֽ��� ã�´�. 
* @param AnsiString sExt      : ������ Ȯ����, ""�� ��� ��� Ȯ���ڿ��� ã�´�. 
* @return AnsiString : ���� �ֽ��� ���� �̸� ����. 
* @see 
- history : 1. [2015�� 7�� 2�� �����][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::SearchLatestFile(AnsiString sDirPath, AnsiString sFileName, AnsiString sExt)
{
    TSearchRec sr;
    DeleteLastSlash(sDirPath);
    AnsiString SerchFile = sDirPath + "\\*.*";
    AnsiString sFileNameTemp;
    AnsiString sRetFile;
    TDateTime DateTime;
    TDateTime DateTimeTmp;

    if(sExt != "") sExt = sExt.UpperCase();

    if (FindFirst(SerchFile, faAnyFile, sr) == 0) {
        do {
            if ((sr.Attr & faDirectory) != sr.Attr) {

                bool bMatchingFileName  = false;
                bool bMatchingExt       = false;

                sFileNameTemp = sr.Name;
                if(sFileName != "") {
                    if(sFileNameTemp.AnsiPos(sFileName) > 0) {
                        bMatchingFileName = true;
                    }
                }
                else {
                    bMatchingFileName = true;
                }

                if(sExt != "") {
                    AnsiString sExtTemp = GetFileExt(sFileNameTemp);
                    sExtTemp = sExtTemp.UpperCase();

                    if(sExtTemp == sExt) {
                        bMatchingExt = true;
                    }
                }
                else {
                    bMatchingExt = true;
                }

                if(bMatchingFileName && bMatchingExt) {
                    if (sr.TimeStamp > DateTimeTmp) {

                        sRetFile    = sr.Name;
                        DateTimeTmp = sr.TimeStamp;

                        FILETIME TempTime;
                        SYSTEMTIME SystemTime;
                        
                        FileTimeToLocalFileTime( &sr.FindData.ftLastWriteTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        DateTime = SystemTimeToDateTime(SystemTime);                    
                    }
                }
            }
        } while(FindNext(sr) == 0);
    }

    FindClose(sr);

    //Return.
    return sRetFile;
}

/** 
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
bool __fastcall TGlobalFuction::DeleteOldTimeFile(AnsiString  sSrcPath, TDateTime tmStandardTime, int  nTimeType, bool bSubDir)
{

    bool bAllFileDelete = true;
    DeleteLastSlash(sSrcPath);

    if(sSrcPath == "") return false;

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;
    TDateTime  tmFileDateTime;

    // ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // ���丮.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    // ���� ���丮 �ؿ� ���� Ȯ���Ͽ� ������ ���
                    if(sr.Name != ".." && sr.Name != ".") {

                        bAllFileDelete = false;
                        
                        if(bSubDir) {
                            
                            sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                            // ��� ȣ�� �Ѵ�.
                            if(DeleteOldTimeFile(sSrcPathTemp, tmStandardTime, nTimeType, bSubDir)) {

#if USE_BOOST_LIB
//---------------------------------------------------------------------------------------

                                // ������ �̸��� "��-��-��" �̰ų� "��-��" �� ��� ������ ��������� �����Ѵ�.
                                std::string sPath = AnsiString(sr.Name).c_str();
								boost::regex re;

								try{
                                    // Set up the regular expression for case-insensitivity
                                    re.assign(DEL_LOG_DIR_EXPRESSION, boost::regex_constants::icase);
                                }
                                catch (boost::regex_error& e) {
                                    continue;
                                }

                                // ���� ǥ���İ� ��ġ�� �Ǹ� �ش� ������ �����Ѵ�.
                                if (boost::regex_match(sPath, re)) {
                                    RemoveDir(sSrcPathTemp);
                                }

//---------------------------------------------------------------------------------------

#else

//---------------------------------------------------------------------------------------


                                AnsiString sDirName = AnsiString(sr.Name);

                                if(sDirName.Length() == 7) {
                                    AnsiString sDirTemp1 = sDirName.SubString(1,4);
                                    AnsiString sDirTemp2 = sDirName.SubString(6,2);
                                    
                                    if(sDirName.c_str()[4] == '-') {
                                        if(GFun.IsNumStr(sDirTemp1.c_str(), 4, false, false)) {
                                            if(GFun.IsNumStr(sDirTemp2.c_str(), 2, false, false)) {
                                                RemoveDir(sSrcPathTemp);
                                            }                                        
                                        }
                                    }
                                }
                                else if(sDirName.Length() == 10) {
                                    AnsiString sDirTemp1 = sDirName.SubString(1,4);
                                    AnsiString sDirTemp2 = sDirName.SubString(6,2);
                                    AnsiString sDirTemp3 = sDirName.SubString(9,2);
                                    
                                    if(sDirName.c_str()[4] == '-' && sDirName.c_str()[7] == '-') {
                                        if(GFun.IsNumStr(sDirTemp1.c_str(), 4, false, false)) {
                                            if(GFun.IsNumStr(sDirTemp2.c_str(), 2, false, false)) {
                                                if(GFun.IsNumStr(sDirTemp3.c_str(), 2, false, false)) {
                                                    RemoveDir(sSrcPathTemp);              
                                                }                                        
                                            }                                        
                                        }
                                    }
                                }    
//---------------------------------------------------------------------------------------
                                
#endif                                
                            }
                        }
                    }
                }
                // ����.------------------------------
                else {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                    // ������ Write Time ����
                    if(nTimeType == 1){
                        FileTimeToLocalFileTime( &sr.FindData.ftLastWriteTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);
                    }
                    // ������ Access Time ����
                    else if(nTimeType == 2){
                        FileTimeToLocalFileTime( &sr.FindData.ftLastAccessTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);
                    }
                    // ������ ��¥ ����
                    else{
                        FileTimeToLocalFileTime( &sr.FindData.ftCreationTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);
                    }

                    if(tmFileDateTime < tmStandardTime){
                        DeleteFile(sSrcPathTemp.c_str());
                    }
                    else {
                        bAllFileDelete = false;
                    }
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

    }

    return bAllFileDelete;
}

//---------------------------------------------------------------------------
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
bool __fastcall TGlobalFuction::DeleteOldTimeDir(AnsiString  sSrcPath, TDateTime tmStandardTime)
{

    bool bAllFileDelete = true;
    DeleteLastSlash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;
    TDateTime  tmFileDateTime;

    // ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // ���丮.--------------------------
                if((sr.Attr & faDirectory) == faDirectory) {

                    // ���� ���丮 �ؿ� ���� Ȯ���Ͽ� ������ ���
                    if(sr.Name != ".." && sr.Name != ".") {
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        FILETIME TempTime;
                        SYSTEMTIME SystemTime;
                        
                        FileTimeToLocalFileTime( &sr.FindData.ftCreationTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);

                        if(tmFileDateTime < tmStandardTime){
                            DeleteDirectory(sSrcPathTemp, true);
                        }                        
                    }
                }
                // ����.------------------------------
                else {
                    // ������ �Ű� �Ⱦ�. 
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

    }

    return bAllFileDelete;
}

//---------------------------------------------------------------------------

/**
* @brief : ������ Path�� ���� ���丮�� ��� ã�� �����Ѵ�.
           bool bDeleteRoot �� true�� �����ϸ� ȣ���� �ڽ��� ���丮�� �����Ѵ�.
           �׷��� false�� �����ϸ� ȣ���� �ڽ��� ���丮�� �״�� �ΰ� �ڽ� ���丮�� ���ϵ鸸 �����Ѵ�.
* @param AnsiString sSrcPath :  ������ Root ���丮.
* @param Tbool bDeleteRoot : ȣ���� Root ���丮�� ������ ��� true�� �����Ѵ�.
* @return void : ����.
* @see
- history : 1. [2012�� 9�� 28�� �ݿ���][�۽ſ�] ���� �ۼ�
**/
void __fastcall TGlobalFuction::DeleteDirectory(AnsiString  sSrcPath, bool bDeleteRoot)
{
    DeleteLastSlash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // ���丮.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        // ��� ȣ�� �Ѵ�.
                        DeleteDirectory(sSrcPathTemp, true);

                        // �ش� ���丮�� ���� ���丮�� ���ϵ��� ��� �����Ͽ����� �ڽ��� ���� ��Ų��.
                        //RemoveDir(sSrcPathTemp);
                    }
                }
                // ����.------------------------------
                else {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                    DeleteFile(sSrcPathTemp.c_str());
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

        if(bDeleteRoot) {
            // �ش� ���丮�� ���� ���丮�� ���ϵ��� ��� �����Ͽ����� �ڽ��� ���� ��Ų��.
            RemoveDir(sSrcPath);
        }

    }
}

/**
* @brief : ������ Path�� ���� ���丮�� ���� ���丮 �̸��� sDirName�� ���� ã�� ��� ã�� �����Ѵ�.
* @param AnsiString sSrcPath :  ������ Root ���丮.
* @param AnsiString sDirName :  ������ ���丮 �̸�.
* @param  bool bDelete       :  true�̸� ã���� ������ ������ ��� �����Ѵ�. false�̸� ���丮 �̸��� sDirName�ΰ��� ã�� ���۸� �Ѵ�.
* @return void : ����.
* @see
- history : 1. [2013�� 3�� 21�� �ݿ���][�۽ſ�] ���� �ۼ�
**/
void __fastcall TGlobalFuction::DeleteDirectory(AnsiString  sSrcPath, AnsiString sDirName, bool bDelete)
{
    //static int count = 0;

    sSrcPath = ExcludeTrailingBackslash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // ���丮.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        bool bSubDelete = (sr.Name == sDirName) ? true : bDelete;

                        // ��� ȣ�� �Ѵ�.
                        DeleteDirectory(sSrcPathTemp, sDirName, bSubDelete);

                        // �ش� ���丮�� ���� ���丮�� ���ϵ��� ��� �����Ͽ����� �ڽ��� ���� ��Ų��.
                        //RemoveDir(sSrcPathTemp);
                    }
                }
                // ����.------------------------------
                else if(bDelete) {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                    DeleteFile(sSrcPathTemp.c_str());

                    //TraceUpdate("���� ���� : " + sSrcPathTemp);
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

        if(bDelete) {
            // �ش� ���丮�� ���� ���丮�� ���ϵ��� ��� �����Ͽ����� �ڽ��� ���� ��Ų��.
            RemoveDir(sSrcPath);
            //TraceUpdate("���� ���� : " + sSrcPath);
        }
    }
}


//---------------------------------------------------------------------------

/**
* @brief : ������ Path�� ���� �߿� Ȯ���ڰ� ������ Ȯ���ڿ� �����ϸ� �����Ѵ�.
           bSubDir �� true�̸� ���� ���丮�� ã�Ƽ� �����Ѵ�.
* @param AnsiString sSrcPath :  ������ Root ���丮.
* @param AnsiString sExeName :  ������ ���� Ȯ����.
* @param  bool bSubDir       :  true�̸� ���� ���丮�� ã�´�.
* @return void : ����.
* @see
- history : 1. [2013�� 3�� 21�� �ݿ���][�۽ſ�] ���� �ۼ�
**/
void __fastcall TGlobalFuction::DeleteFileByExtension(AnsiString  sSrcPath, AnsiString sExeName, bool bSubDir)
{
    sSrcPath = ExcludeTrailingBackslash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // ���丮.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        // ��� ȣ�� �Ѵ�.
                        if(bSubDir) DeleteFileByExtension(sSrcPathTemp, sExeName, bSubDir);
                    }
                }
                // ����.------------------------------
                else {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    AnsiString sFileName = GetFileExt(sr.Name);

                    if(sFileName == sExeName) {
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                        DeleteFile(sSrcPathTemp.c_str());
                        //TraceUpdate("���� ���� : " + sSrcPathTemp);
                    }
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);
    }
}

/** 
* @brief : �־��� sFileName���� Ȯ���ڸ� sExt�� �������ش�. 
* @param AnsiString sFileName : Ȯ���ڸ� �����ϰ����ϴ� ���� �̸����� ��ü ��ΰ� ���ԵǾ ��� ����. 
* @param AnsiString sExt : Ȯ���ڿ��� ".Exe" ó�� '.'�� �����Ͽ��� �Ѵ�. 
* @return AnsiString : Ȯ���ڸ� ������ ���� �̸��� �����Ѵ�. 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
AnsiString  __fastcall TGlobalFuction::ChangeExtension(AnsiString sFileName, AnsiString sExt)
{
    AnsiString sRet  = ChangeFileExt(sFileName, sExt);
    return sRet;
}



// Ư�� ������ ��� ������ ���� 
/** 
* @brief : 
* @param const char *srcfiles :  ���� �̸��� �ִ� Full Path ("c:\\Temp\\*.ini" )
* @param const char *destdir :   ����� ��ǥ ����. 
* @return int : 
* @see 
- history : 1. [2013�� 3�� 28�� �����][�۽ſ�] ���� �ۼ� 
**/
int __fastcall TGlobalFuction::CopyFiles(const char *srcfiles, const char *destdir) 
{ 
#if 1

    int nRet = 0;
    AnsiString sSrcPath = ExtractFileDir(srcfiles);     // ��ο��� ���� �̸��� �����ϰ� Path�� �����Ѵ�.
    AnsiString sTargetPaht = destdir;

    sSrcPath = sSrcPath + "\\";

    DeleteLastSlash(sTargetPaht);
    sTargetPaht = sTargetPaht + "\\";

    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // Target ���� ������ ���� ����. 
    if(!DirectoryExists(sTargetPaht)) {
        ForceDirectories(sTargetPaht);
    }

    // Source ���丮 �����ϴ��� Ȯ��
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(srcfiles, faAnyFile, sr) == 0){
            do{
                // ���ϸ� �˻�.------------------------------
                if((sr.Attr & faDirectory) != faDirectory) {
                    AnsiString sSrcFile = sSrcPath + sr.Name;
                    AnsiString sTrgFile = sTargetPaht + sr.Name;

                    try {
                        bool bRet = CopyFile(sSrcFile.c_str(), sTrgFile.c_str(), false);   // false�̸� ����� ���.
                        nRet = !bRet ? -1 : nRet;
                    }
                    __finally {

                    }   
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);
    }
    else {
        nRet = -2;
    }

    return nRet;

#else

    SHFILEOPSTRUCT sfop; 
    ZeroMemory(&sfop, sizeof(sfop)); 

    //wFunc
    //Type: UINT
    //        A value that indicates which operation to perform. One of the following values:
    //FO_COPY
    //        Copy the files specified in the pFrom member to the location specified in the pTo member.
    //FO_DELETE
    //        Delete the files specified in pFrom.
    //FO_MOVE
    //        Move the files specified in pFrom to the location specified in pTo.
    //FO_RENAME
    //        Rename the file specified in pFrom. You cannot use this flag to rename multiple files with a single function call. Use FO_MOVE instead.
    //    FO_MOVE   = 0x0001
    //    FO_COPY   = 0x0002
    //    FO_DELETE = 0x0003
    //    FO_RENAME = 0x0004    
    sfop.wFunc = FO_COPY; 

    sfop.pFrom = srcfiles; 
    sfop.pTo = destdir; 

    //fFlags
    //        Type: FILEOP_FLAGS
    //        Flags that control the file operation. This member can take a combination of the following flags.
    //FOF_ALLOWUNDO
    //        Preserve undo information, if possible.
    //        Prior to Windows Vista, operations could be undone only from the same process that performed the original operation.
    //        In Windows Vista and later systems, the scope of the undo is a user session. Any process running in the user session can undo another operation. The undo state is held in the Explorer.exe process, and as long as that process is running, it can coordinate the undo functions.
    //        If the source file parameter does not contain fully qualified path and file names, this flag is ignored.
    //FOF_CONFIRMMOUSE
    //        Not used.
    //FOF_FILESONLY
    //        Perform the operation only on files (not on folders) if a wildcard file name (*.*) is specified.
    //FOF_MULTIDESTFILES
    //        The pTo member specifies multiple destination files (one for each source file in pFrom) rather than one directory where all source files are to be deposited.
    //FOF_NOCONFIRMATION
    //        Respond with Yes to All for any dialog box that is displayed.
    //FOF_NOCONFIRMMKDIR
    //        Do not ask the user to confirm the creation of a new directory if the operation requires one to be created.
    //FOF_NO_CONNECTED_ELEMENTS
    //        Version 5.0. Do not move connected files as a group. Only move the specified files.
    //FOF_NOCOPYSECURITYATTRIBS
    //        Version 4.71. Do not copy the security attributes of the file. The destination file receives the security attributes of its new folder.
    //FOF_NOERRORUI
    //        Do not display a dialog to the user if an error occurs.
    //FOF_NORECURSEREPARSE
    //        Not used.
    //FOF_NORECURSION
    //        Only perform the operation in the local directory. Do not operate recursively into subdirectories, which is the default behavior.
    //FOF_NO_UI
    //        Version 6.0.6060 (Windows Vista). Perform the operation silently, presenting no UI to the user. This is equivalent to FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR.
    //FOF_RENAMEONCOLLISION
    //        Give the file being operated on a new name in a move, copy, or rename operation if a file with the target name already exists at the destination.
    //FOF_SILENT
    //        Do not display a progress dialog box.
    //FOF_SIMPLEPROGRESS
    //        Display a progress dialog box but do not show individual file names as they are operated on.
    //FOF_WANTMAPPINGHANDLE
    //        If FOF_RENAMEONCOLLISION is specified and any files were renamed, assign a name mapping object that contains their old and new names to the hNameMappings member. This object must be freed using SHFreeNameMappings when it is no longer needed.
    //FOF_WANTNUKEWARNING
    //        Version 5.0. Send a warning if a file is being permanently destroyed during a delete operation rather than recycled. This flag partially overrides FOF_NOCONFIRMATION.
    //
    //    FOF_MULTIDESTFILES = 0x0001
    //    FOF_CONFIRMMOUSE = 0x0002
    //    FOF_SILENT = 0x0004
    //    FOF_RENAMEONCOLLISION = 0x0008
    //    FOF_NOCONFIRMATION = 0x0010
    //    FOF_WANTMAPPINGHANDLE = 0x0020
    //    FOF_ALLOWUNDO = 0x0040
    //    FOF_FILESONLY = 0x0080
    //    FOF_SIMPLEPROGRESS = 0x0100
    //    FOF_NOCONFIRMMKDIR = 0x0200
    //    FOF_NOERRORUI = 0x0400
    //    FOF_NOCOPYSECURITYATTRIBS = 0x0800
    //    FOF_NORECURSION = 0x1000
    //    FOF_NO_CONNECTED_ELEMENTS = 0x2000
    //    FOF_WANTNUKEWARNING = 0x4000
    //    FOF_NORECURSEREPARSE = 0x8000
    sfop.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT; 

    
    return SHFileOperation(&sfop);    
#endif     
} 







/** DirectoryCopy()
* @brief : Directory���� Directory�� sub ������ �����Ͽ� ���� �Ѵ�.
* @param AnsiString sSrcPath : Source Directory
* @param AnsiString sTrgPath : Target Directory
* @param TStringList * slSkipDirectory : sub Directory �߿��� �������� ���ƾ��� �̸��� ������ �� �ִ�.
* @return bool : ������ ��� true ����.
* @see
- history : 1. [2015�� 11�� 24�� ȭ����][�۽ſ�] ���� �ۼ�
**/
bool __fastcall TGlobalFuction::DirectoryCopy(AnsiString sSrcPath, AnsiString sTrgPath, TStringList * slSkipDirectory)
{
    bool bRetValue = true;
    bool bCopySkip = false;
    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp, sTrgPathTemp;

    // ����� ����� ���丮�� ���� ��� �����Ѵ�.
    if(!DirectoryExists(sTrgPath)) {
        if(ForceDirectories(sTrgPath) != true) {
            return false;
        }
    }

    if(FindFirst(sSrc, faAnyFile, sr) == 0){
        do{
            // ���丮.
            if((sr.Attr & faDirectory) == faDirectory){
                if(sr.Name != ".." && sr.Name != "."){
                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                    sTrgPathTemp = sTrgPath + "\\" + sr.Name;

                    bCopySkip = false;

                    if(slSkipDirectory != NULL) {
                        for(int i=0; i<slSkipDirectory->Count; i++){
                            // �������� \ ��ȣ�� ������ �����Ѵ�.
                            if(slSkipDirectory->Strings[i].c_str()[slSkipDirectory->Strings[i].Length()-1] == '\\'){
                                slSkipDirectory->Strings[i].Delete(slSkipDirectory->Strings[i].Length(), 1);
                            }

                            // Copy�� Skip�ؾ��ϴ� Directory�� ��� �������� �ʴ´�.
                            AnsiString sSkipDir = slSkipDirectory->Strings[i];
                            if(stricmp(sSrcPathTemp.c_str(), sSkipDir.c_str()) == 0){
                                bCopySkip = true;
                                break;
                            }
                        }
                    }

                    // Copy�� Skip�ؾ��ϴ� Directory�� �ƴϸ� ����ȣ��� ����.
                    if(!bCopySkip) DirectoryCopy(sSrcPathTemp, sTrgPathTemp, slSkipDirectory);
                }
            }
            // ����.
            else {
                sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                sTrgPathTemp = sTrgPath + "\\" + sr.Name;

                bCopySkip = false;

                if(slSkipDirectory != NULL) {
                    for(int i=0; i<slSkipDirectory->Count; i++){

                        // Copy�� Skip�ؾ��ϴ� ������ ��� �������� �ʴ´�.
                        AnsiString sSkipFile = slSkipDirectory->Strings[i];
                        if(stricmp(sSrcPathTemp.c_str(), sSkipFile.c_str()) == 0){
                            bCopySkip = true;
                            break;
                        }
                    }
                }

                if(!bCopySkip) CopyFile(sSrcPathTemp.c_str(), sTrgPathTemp.c_str(), false); //�����
                //CopyFile(sSrc.w_str(), sTrg.w_str(), false); //�����
            }
        }while(FindNext(sr)==0);

        bRetValue = true;
    }
    else {
        bRetValue = false;
    }
    FindClose(sr);

    return bRetValue;
}



bool __fastcall TGlobalFuction::DirRename(const char * sOldname,const char * sNewname)
{
    assert(sOldname != NULL);
    assert(sNewname != NULL);

    if(!DirectoryExists(sOldname)) return false;
    
	//�̸��� �ٲٰ� ����� ���� �޴´�.
	int nResult = rename( sOldname, sNewname );

	if( nResult == 0 ) {
		return true;
	}
	else {
		return false;
	}

	return 0;

}

bool __fastcall TGlobalFuction::FileRename(const char * sOldname,const char * sNewname)
{
    assert(sOldname != NULL);
    assert(sNewname != NULL);

    if(!FileExists(sOldname)) return false;
    
	//�̸��� �ٲٰ� ����� ���� �޴´�.
	int nResult = rename( sOldname, sNewname );

	if( nResult == 0 ) {
		return true;
	}
	else {
		return false;
	}

	return 0;

}


//---------------------------------------------------------------------------

/** 
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
bool __fastcall TGlobalFuction::ShowDirSelectDlg( HWND hWndOwner, _TCHAR *szDir )
{

    LPMALLOC ShellMalloc;
    LPITEMIDLIST idRoot, idBrowse;
    BROWSEINFO BrowseInfo;
    _TCHAR* shBuff;
    _TCHAR szPath[MAX_PATH];

    szPath[0] = '\0' ;
    if (SHGetMalloc(&ShellMalloc) == NOERROR) {
        SHGetSpecialFolderLocation(0, CSIDL_DRIVES, &idRoot);
        shBuff = (_TCHAR*)ShellMalloc->Alloc(MAX_PATH);

        if (shBuff){
            BrowseInfo.hwndOwner      = hWndOwner;
            BrowseInfo.pidlRoot       = idRoot;
            BrowseInfo.pszDisplayName = shBuff;
            BrowseInfo.lpszTitle      = _T("�Ʒ� ��Ͽ��� ���ϴ� ������ �����ϼ���.") ;
            BrowseInfo.ulFlags        = BIF_DONTGOBELOWDOMAIN;
            BrowseInfo.lpfn           = NULL;
            BrowseInfo.lParam         = 0;

            idBrowse = SHBrowseForFolder(&BrowseInfo);

            if (idBrowse) SHGetPathFromIDList(idBrowse, szPath);

        }
        ShellMalloc->Free(shBuff);
        ShellMalloc->Free(idBrowse);
        ShellMalloc->Free(idRoot);
    }

    ShellMalloc->Release();

#ifdef UNICODE
    int nLen = wcslen( szPath );

    if( nLen > 0 ){
        wcscpy( szDir, szPath );
        if( szPath[nLen-1] != '\\' ) wcscat( szDir, L"\\");
        return true;
    }else{
        return false;
    }
#else
    int nLen = strlen( szPath );

    if( nLen > 0 ){
        strcpy( szDir, szPath );
        if( szPath[nLen-1] != '\\' ) strcat( szDir, _T("\\"));
        return true;
    }else{
        return false;
    }
#endif
}


/** 
* @brief : ���� ���� ������ Root ������ �����Ѵ�. �ڿ� "\\"�� �ٴ´�. 
* @param : ����.
* @return AnsiString : �������� Path
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
AnsiString __fastcall TGlobalFuction::GetApplicationDir() 
{
    // ���� ��� ����. --------------------------------
    AnsiString sPath = ExtractFilePath(Application->ExeName);
    DeleteLastSlash(sPath);
    if(GetLastDirName(sPath) == ".") {
        sPath  = sPath.SubString(1, sPath.Length() - 1);
        DeleteLastSlash(sPath);
    }

    sPath = sPath + "\\";

    return sPath;
}

/**
* @fn      : char __fastcall TGlobalFuction::GetApplicationDiskName()
* @brief   : 
* @details : ���� ���� ������ �ִ� Disk�� �̸��� �����ϴ� �Լ�. 
* @param   : None
* @return  : char : ã���� ã�� Disk Name�� �����ϰ� ��ã���� '\0' �� �����Ѵ�. 
* @see     : 
- history  : 1. [2018�� 1�� 25�� �����][Song Shin Young] First. 
**/
char __fastcall TGlobalFuction::GetApplicationDiskName() 
{
    // ���� ��� ����. --------------------------------
    AnsiString sDrive = ExtractFileDrive(Application->ExeName);
    if(sDrive == "") return '\0';
    
    return sDrive.c_str()[0];
}


/** 
* @brief : File�� MD5 Hash Code�� �����ϴ� �Լ��̴�. 
* @param __in LPCSTR filename : ���� �̸�(��ü ���)
* @param __out LPSTR pRetHashCode : ������ MD5 Hash Code�� Hex Type String���� ��ȯ�Ͽ� �����Ѵ�. 
                                    Hash Code Len�� 16 byte�̹Ƿ� Hex Type String���� ��ȯ�ϸ� 32 Byte�� �ȴ�. 
* @return bool : ����:true, ����:false
* @see 
- history : 1. [2015�� 10�� 22�� �����][�۽ſ�] ���� �ۼ� 
**/
bool __fastcall TGlobalFuction::GetMD5HashCodeFromFile(__in LPCSTR filename, __out LPSTR pRetHashCode)
{
    DWORD       dwStatus		= 0;
    BOOL        bResult         = FALSE;
    HCRYPTPROV  hProv           = 0;
    HCRYPTHASH  hHash           = 0;
    HANDLE      hFile           = NULL;
    BYTE        rgbFile[FILE_READ_BUFSIZE];
    DWORD       cbRead          = 0;
    BYTE        rgbHash[MD5LEN];
    DWORD       cbHash          = 0;
    const CHAR  rgbDigits[]     = "0123456789abcdef";
    
    // Logic to check usage goes here.

    if(filename == NULL)        return false;
    if(pRetHashCode == NULL)    return false;

    hFile = CreateFile( filename,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_SEQUENTIAL_SCAN,
                        NULL);



    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
        //printf("Error opening file %s\nError: %d\n", filename, dwStatus);

        return false;
    }



    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        //printf("CryptAcquireContext failed: %d\n", dwStatus);
        CloseHandle(hFile);

        return false;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        //printf("CryptAcquireContext failed: %d\n", dwStatus);
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);

        return false;
    }

    while (1)
    {
        bResult = ReadFile(hFile, rgbFile, FILE_READ_BUFSIZE, &cbRead, NULL);

        if (!bResult || 0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
            //printf("CryptHashData failed: %d\n", dwStatus);
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);

            return false;
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        //printf("ReadFile failed: %d\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);

        return false;
    }

    cbHash = MD5LEN;
    int nIdx = 0;
    
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        //printf("MD5 hash of file %s is: ", filename);
        for (DWORD i = 0; i < cbHash; i++)
        {
            pRetHashCode[nIdx++] = rgbDigits[rgbHash[i] >> 4];
            pRetHashCode[nIdx++] = rgbDigits[rgbHash[i] & 0xf];

            //printf("%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
        }
        //printf("\n");

    }
    else
    {
        dwStatus = GetLastError();
		//printf("CryptGetHashParam failed: %d\n", dwStatus);

		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		CloseHandle(hFile);

		return false;
	}

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return true;
}




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
bool        __fastcall TGlobalFuction::DriveExists(char cDriveName, bool bOnlyHDD)
{
    UINT result;
    AnsiString sDrive;
    sDrive.printf("%c:\\", cDriveName);

    result = ::GetDriveType(sDrive.c_str());   

    switch(result)   
    {   
        case DRIVE_UNKNOWN :        return false;
        case DRIVE_NO_ROOT_DIR :    return false;
        case DRIVE_REMOVABLE :      return (bOnlyHDD ? false : true);
        case DRIVE_FIXED :          return true;
        case DRIVE_REMOTE :         return (bOnlyHDD ? false : true);
        case DRIVE_CDROM :          return (bOnlyHDD ? false : true);
        case DRIVE_RAMDISK :        return (bOnlyHDD ? false : true);
        default:                    return false;
    }

    return false;

    
//    String drive, msg;
//    _ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;
//  
//        msg = drive + " - " + msg;   
//        if(true == ::GetDiskFreeSpaceEx(drive.c_str(), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes)){
//
//            msg = msg + " - FreeBytesAvailable : " + String(FreeBytesAvailable.QuadPart)
//                      + " / TotalNumberOfBytes : " + String(TotalNumberOfBytes.QuadPart)
//                      + " / TotalNumberOfFreeBytes : " + String(TotalNumberOfFreeBytes.QuadPart);
//        }

}








/**
* @fn      : double __fastcall TGlobalFuction::GetDiskFreeSpace(char cDrive)
* @brief   : 
* @details : �־��� ��ũ�� ���� ������ MByte ������ �����ϴ� �Լ� 
* @param   : char cDrive : 
* @return  : double : ���� ���� MByte
* @see     : 
- history  : 1. [2018�� 1�� 25�� �����][Song Shin Young] First. 
**/
double __fastcall TGlobalFuction::GetDiskFreeSpace(char cDrive)
{   
    int nDriveIndex = cDrive == '\0' ? 0 : cDrive - 'A' + 1;
    
    //__int64 ��ü�뷮 = DiskSize(nDriveIndex);   // 0: ���� ��ũ, 1:A, 2:B, 3:C, 4:D ...... 
    __int64 nFreeSpace = DiskFree(nDriveIndex);

    double dFreeSpaceMByte = (nFreeSpace >> 10) / 1024.0;

    return dFreeSpaceMByte;
}















//---------------------------------------------------------------------------

/** 
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
void __fastcall TGlobalFuction::DrawGraphBackLine(Graphics::TBitmap* Bitmap, int nWidth, int nHeight, int nXpitch, int nYpitch)
{
    int nCountX = 0;
    int nCountY = 0;
    int nStartX = 0, nStartY = 0;
    int nEndX   = 0, nEndY   = 0;
    int nTimeLineCnt = 0 ;

    Bitmap->Canvas->Pen->Color = (TColor)RGB(0, 57, 0);
    Bitmap->Canvas->Pen->Width = 1;

    // X�� ���м� �׸��� ------------
    nStartY = 0; 
    nEndY   = nHeight;

    while(nEndX < nWidth){
        nStartX = nCountX * nXpitch;
        nEndX   = nStartX;
        
        //if(nCountX == (8 + (10 * nTimeLineCnt))) {
        if(nCountX == (0 + (10 * nTimeLineCnt))) {
            nTimeLineCnt++;
            Bitmap->Canvas->Pen->Width = 2;
        }
        else {
            Bitmap->Canvas->Pen->Width = 1;
        }
        
        nCountX++;
        
        Bitmap->Canvas->MoveTo(nStartX, nStartY);
        Bitmap->Canvas->LineTo(nEndX, nEndY);
    }

    // Y �� ���м� �׸��� ----------
    nStartX = 0;
    nEndX   = nWidth;
    nStartY = 0;
    nEndY   = 0;

    while(nEndY < nHeight){
        nStartY = nCountY * nYpitch;
        nEndY   = nStartY;
        nCountY++;
        Bitmap->Canvas->MoveTo(nStartX, nStartY);
        Bitmap->Canvas->LineTo(nEndX, nEndY);
    }    
}

//---------------------------------------------------------------------------
/** 
* @brief : pDest�� srcBmp�� X,Y ��ġ�� ����ó���Ͽ� �׸���. 
* @param Graphics::TBitmap* pDest : �׷��� ������ Bitmap
* @param int X : pDest���� �ȼ� ��ǥ (Widht ���� ���� ��)
* @param int Y : pDest���� �ȼ� ��ǥ (Height ���� ���� ��)
* @param Graphics::TBitmap* srcBmp : �׸� �ҽ� Bitmap
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DrawImageTrans(Graphics::TBitmap* pDest, int X, int Y, Graphics::TBitmap* srcBmp)
{
    srcBmp->Transparent = true;
    srcBmp->TransparentColor = clFuchsia;
    pDest->Canvas->Draw(X, Y, srcBmp);
}
//---------------------------------------------------------------------------

/** 
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

void __fastcall TGlobalFuction::DrawTextTrans(
    Graphics::TBitmap* pDest,
    Graphics::TBitmap* pTextBitmap,
    int             X,
    int             Y,
    AnsiString      sString,
    TColor          cColor,
    unsigned int    Align,
    int             nFontSize,
    bool            bBold)
{
    TRect rc;


    pTextBitmap->FreeImage();
    pTextBitmap->Canvas->Font->Name = "����";
    pTextBitmap->Canvas->Font->Size = nFontSize;    
    pTextBitmap->Canvas->Font->Style = bBold ? TFontStyles() << fsBold : TFontStyles() >> fsBold;
    pTextBitmap->Width = pTextBitmap->Canvas->TextWidth(sString) + 1;
    pTextBitmap->Height = pTextBitmap->Canvas->TextHeight(sString);
    pTextBitmap->Canvas->Brush->Color = clFuchsia;
    pTextBitmap->Canvas->Font->Color = cColor;

    rc.Top      = 0;
    rc.Left     = 0;
    rc.Right    = pTextBitmap->Width;
    rc.Bottom   = pTextBitmap->Height;
    
    pTextBitmap->Canvas->FillRect(rc);
    pTextBitmap->Canvas->TextOutA(1 , 0, sString);
    pTextBitmap->Transparent = true;
    pTextBitmap->TransparentColor = clFuchsia;

    if(Align & TEXT_ALIGN_BOTTOM){
        Y = Y - pTextBitmap->Height;
    }
    else if(Align & TEXT_ALIGN_V_CENTER){
        Y = Y - (pTextBitmap->Height/2);
    }

    if(Align & TEXT_ALIGN_RIGHT){
        X = X - pTextBitmap->Width;
    }
    else if(Align & TEXT_ALIGN_H_CENTER){
        X = X - (pTextBitmap->Width/2);
    }    

    DrawImageTrans(pDest, X, Y, pTextBitmap);
}



/**
* @fn      : void __fastcall TGlobalFuction::DrawGradient(TCanvas *   pCanvas, TColor StartColor, TColor  EndColor, int  ShadingLevel, TRect  &Rect)
* @brief   : �־��� Canvas�� �ش� �������� ���� �������� ����Ǵ� �׶���Ʈ�� �׸���. (���� StringGrid���� ���õ� Item�� �׸� �� ����Ѵ�.)
* @details : 
* @param   : TCanvas *   pCanvas :      �׷��� ��� Canvas
* @param   : TColor      StartColor :   �׶��̼� ������ ���� ���� 
* @param   : TColor      EndColor :     �׶��̼� ������ ���� ���� 
* @param   : int         ShadingLevel : ���� ���� ������� ���� �ܰ� �� (Ŭ ���� �ε巴��)
* @param   : TRect       &Rect :        �׶��̼��� �׸� ���� 
* @return  : void : 
* @see     : 
- history  : 1. [2016�� 9�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::DrawGradient(TCanvas *   pCanvas,TColor      StartColor,TColor      EndColor,int         ShadingLevel,TRect       &Rect)
{
    if(pCanvas == NULL) return;

    int GradientHeight = (Rect.Bottom - Rect.Top) / ShadingLevel;

    if(GradientHeight <= 1) {
        ShadingLevel = Rect.Bottom - Rect.Top;
        GradientHeight = 1;
    }

   // calculate individual RGB intensities of the starting color.
   int sRed       = GetRValue(StartColor);
   int sGreen     = GetGValue(StartColor);
   int sBlue      = GetBValue(StartColor);

   int eRed       = GetRValue(EndColor);
   int eGreen     = GetGValue(EndColor);
   int eBlue      = GetBValue(EndColor);

   eRed     = eRed   - sRed;
   eGreen   = eGreen - sGreen ;
   eBlue    = eBlue  - sBlue;

   int RedIncrement     = (eRed   + 1)  / ShadingLevel; // calculate
   int GreenIncrement   = (eGreen + 1)  / ShadingLevel; // increment of
   int BlueIncrement    = (eBlue  + 1)  / ShadingLevel; // each gradient

   TColor FillColor;
   // the for loop draws each gradient section. the loop stops one pass
   // away from black because black was drawn by FillRect above

   TRect rect = Rect;      // calculate size of backgnd


   for (int j=0;j< ShadingLevel;j++)
   {
     rect.Bottom = (j == (ShadingLevel-1)) ? Rect.Bottom : (rect.Top + GradientHeight);
     FillColor = (TColor) RGB( sRed   + (RedIncrement * j ),
                               sGreen + (GreenIncrement* j),
                               sBlue  + (BlueIncrement * j));
     pCanvas->Brush->Color = FillColor;
     pCanvas->FillRect(rect);
     rect.Top += GradientHeight;
   }

}

#define COLOR 256

/** 
* @brief : 8 bit Raw ������ �̹����� 24Bit BMP ���Ϸ� �������ش�. 
* @param BYTE * pRawData : 8 bit Raw Image ����Ÿ 
* @param TMemoryStream * pStm : 24 Bit BMP �� ����� ������ ��ȯ TMemorySteam ������ 
* @param int nWidth : �̹��� Width
* @param int nHeight : �̹��� Height
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::Raw8Bit_to_Bmp24Bit(BYTE * pRawData, TMemoryStream * pStm, int nWidth, int nHeight)
{
    if(pRawData == NULL || pStm == NULL) return;

//    int COLOR = 256;

    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    RGBQUAD bmp_pal[COLOR];

    //BITMAPINFOHEADER ����
    bmih.biSize             = sizeof(BITMAPINFOHEADER);
    bmih.biWidth            = nWidth;
    bmih.biHeight           = nHeight;
    bmih.biPlanes           = 1;
    bmih.biBitCount         = 8;
    bmih.biCompression      = BI_RGB;
    bmih.biSizeImage        = nWidth*nHeight;
    bmih.biXPelsPerMeter    = 0;
    bmih.biYPelsPerMeter    = 0;
    bmih.biClrUsed          = COLOR;
    bmih.biClrImportant     = 0;

    //BITMAPFILEHEADER ����
    bmfh.bfType             =    0x4d42;  //BM

    bmfh.bfSize             = sizeof(BITMAPFILEHEADER)+((COLOR)*sizeof(RGBQUAD))+sizeof(BITMAPINFOHEADER)
                             + bmih.biSizeImage -2;
    bmfh.bfReserved1        = 0;
    bmfh.bfReserved2        = 0;
    bmfh.bfOffBits          = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
                              +(COLOR*sizeof(RGBQUAD))-2;

    //PALETTE ����
    for(int i=0;i<COLOR;i++)           //8bit-> 256 monochrome ����
    {
        bmp_pal[i].rgbRed       = i;
        bmp_pal[i].rgbGreen     = i;
        bmp_pal[i].rgbBlue      = i;
        bmp_pal[i].rgbReserved  = 0;
    }

    pStm->WriteBuffer(&bmfh, sizeof(BITMAPFILEHEADER));
    pStm->WriteBuffer(&bmih, sizeof(BITMAPINFOHEADER));
    pStm->WriteBuffer(bmp_pal, sizeof(RGBQUAD) * COLOR);
    pStm->WriteBuffer(pRawData, nHeight * nWidth);

    pStm->Position = 0;
}


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
void __fastcall TGlobalFuction::RawData_Bottom_Up(BYTE *data, int nPixelByte, int biWidth, int biHeight)
{

    int nOneLineSize = nPixelByte * biWidth;
    BYTE * pTemp = new BYTE[nOneLineSize];

    int nBottomLine = biHeight - 1;
    int nUpLine     = 0;
    for(int i=0; i<biHeight/2; i++) {
        memcpy(pTemp, &data[nUpLine * nOneLineSize], nOneLineSize);
        memcpy(&data[nUpLine * nOneLineSize], &data[nBottomLine * nOneLineSize], nOneLineSize);
        memcpy(&data[nBottomLine * nOneLineSize], pTemp, nOneLineSize);
        nBottomLine -= 1;
        nUpLine     += 1;
    }

    delete[] pTemp;
}

//---------------------------------------------------------------------------
/** 
* @brief : nPoint �� �Ҽ��� �ڸ���ŭ �ݿø��Ͽ� �����Ѵ�. 
           3.14159 �� nPoint �� 2 ��� 3.14�� �����Ѵ�. 
           ���� nPoint�� 3�̶�� 3.142 �� �����Ѵ�. 
* @param double dSrc : �ݿø��� ������ �� 
* @param int nPoint :  �ݿø��� �Ҽ��� �ڸ� 
* @return double :     
* @see 
- history : 1. [2013�� 4�� 2�� ȭ����][�۽ſ�] ���� �ۼ� 
**/
double __fastcall TGlobalFuction::DecimalRounding(double dSrc, int nPoint)
{
    int k=pow(10,nPoint);
    double Result=floor(dSrc * k + 0.5) / k;
    return Result;
}


//--------------------------------------------------------------

/** 
* @brief : double Ÿ�� ���� ������ ������ ���ϱ�
* @param double x : 
* @param double y : 
* @param int ulpsTolerance = 4 : 
* @return int : 
* @see 
- history : 1. [2015�� 4�� 2�� �����][�۽ſ�] ���� �ۼ� 
**/
#pragma warn -8056 // W8056 Integer arithmetic overflow OFF

int __fastcall TGlobalFuction::CompareDoubleUlps(double x, double y, int ulpsTolerance)
{
    double diff = x - y;

    __int64 nx = *((__int64*)&x);
    __int64 ny = *((__int64*)&y);

    if ((nx & 0x8000000000000000) != (ny & 0x8000000000000000))
    {
        if (x == y)
            return 0;

        return (diff > 0) ? 1 : -1;
    }

    __int64 ulpsDiff = nx - ny;
    if ((ulpsDiff >= 0 ? ulpsDiff : -ulpsDiff) <= ulpsTolerance)
        return 0;

    return (diff > 0) ? 1 : -1;
}


/** 
* @brief :  ���� ��� ���� ������ ���ϱ� - ���� ���� + ULP ��� 
            ULP�� �̿��� �񱳰� ���� ���� ���� ������ ����ϴ� ��ó�� ��������,
            1 ULP�� ũ��� ������ ũ�⿡ ���� ���ϴ� ������� ���̱� ������ ��� ���� �񱳿� �ش�ȴٰ� �� �� �ֽ��ϴ�.
            �׷��Ƿ� ������ �����߾��� CompareDoubleRelative �Լ��� ���������� 0�� ����� ���� ���� ���� ���� ����
            (���� �߻��Ǵ� ��Ȳ�� �ƴ�����) ������ �߻��� ������ �����ֽ��ϴ�. 
            �ᱹ �̷��� ��Ȳ���� ����Ϸ��� �Ʒ��� ���� ���� ���� �񱳿� ULP �񱳸� �����Ͽ� �����ؾ� �մϴ�. 

* @param double x : 
* @param double y : 
* @param double absTolerance = (1.0e-8 : 
* @return int : 
* @see 
- history : 1. [2015�� 4�� 2�� �����][�۽ſ�] ���� �ۼ� 
**/
int __fastcall TGlobalFuction::CompareDoubleAbsoulteAndUlps(double x, 
                                                            double y,
                                                            double absTolerance,
                                                            int ulpsTolerance)
{
    double diff = x - y;
    if (fabs(diff) <= absTolerance)
        return 0;

    __int64 nx = *((__int64*)&x);
    __int64 ny = *((__int64*)&y);

    if ((nx & 0x8000000000000000) != (ny & 0x8000000000000000))
        return (diff > 0) ? 1 : -1;

    __int64 ulpsDiff = nx - ny;
    if ((ulpsDiff >= 0 ? ulpsDiff : -ulpsDiff) <= ulpsTolerance)
        return 0;

    return (diff > 0) ? 1 : -1;
}
#pragma warn +8056 // W8056 Integer arithmetic overflow ON
//---------------------------------------------------------------------------

/** 
* @brief : ComPort�� ���¸� Ȯ���Ͽ� ��� ������ Port���� Ȯ���Ѵ�. 
* @param : char * szPort : ComPort Name ("COM4")
* @return bool : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
bool __fastcall TGlobalFuction::OpenCheckComport(char * szPort) 
{ 
    DWORD dwError; 
    COMMTIMEOUTS CommTimeOuts; 
    HANDLE hCommPort = CreateFile(  szPort, 
                                    GENERIC_READ | GENERIC_WRITE, 
                                    0, 
                                    NULL, 
                                    OPEN_EXISTING, 
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                                    NULL ); 

    if(hCommPort == INVALID_HANDLE_VALUE) //Port��ȣ�� ������ �ְų� ������ΰ���.. 
    { 
        return false; 
    } 

    CloseHandle(hCommPort); 
    return true; 
    
} 
//---------------------------------------------------------------------------

/** 
* @brief : TRadioGroup �� ItemIndex�� ����� OnClick Event�� �߻��ϴµ� 
           �� �Լ��� �Ͽ��ϸ� OnClick Event ���� ItemIndex�� ������ �� ����. 
* @param TRadioGroup *pRGroup : 
* @param int Index : 
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::SetRadioGroupItemIndex(TRadioGroup *pRGroup, int Index) 
{
    if(pRGroup == NULL) return; 
    TNotifyEvent OnClickEvent   = pRGroup->OnClick;
    pRGroup->OnClick            = NULL;                    
    pRGroup->ItemIndex          = Index;
    pRGroup->OnClick            = OnClickEvent;
}


/** 
* @brief : sStr ���� sS�� �ش�Ǵ� ���ڿ��� sT�� �ٲ۴�. 
* @param AnsiString &sStr : ������ ��ü ���ڿ� (���������� ����� �� ���� �̰����� ���ڷ� �ѱ� ������ ���� ����ȴ�.)
* @param AnsiString sS : ã�� ���ڿ� 
* @param AnsiString sT : ��ġ�� ���ڿ� 
* @return void : 
* @see 
- history : 1. [2015�� 10�� 23�� �ݿ���][�۽ſ�] ���� �ۼ� 
**/
void __fastcall TGlobalFuction::ReplaceStr(AnsiString &sStr, AnsiString sS, AnsiString sT)
{
    sStr = StringReplace(sStr, sS, sT, TReplaceFlags()<<rfReplaceAll);
}


//---------------------------------------------------------------------------
/**
* @fn      : int __fastcall TGlobalFuction::StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos)
* @brief   : 
* @details : �־��� ���ڿ����� ���� ��ġ ���� �˻� ���ڿ��� ã������ ��ġ ���� ���� ��. (ã�� ��� 1 �̻��� ���� ����)
* @param   : const AnsiString & sSrcStr : 
* @param   : AnsiString sSearch : 
* @param   : Cardinal InitPos : 
* @return  : int : 0 : ��ã��, 1�̻� : ã�� ���ڿ� ��ġ 
* @see     : ���� ����Ʈ : http://cbuilder.borlandforum.com/impboard/impboard.dll?action=read&db=bcb_tip&no=684
- history  : 1. [2018�� 1�� 17�� ������][Song Shin Young] First. 
**/
int __fastcall TGlobalFuction::StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos)
{
    if( (InitPos) >= (unsigned int)sSrcStr.Length()) return 0;
    char *ret = NULL;
    ret = strstr(sSrcStr.c_str() + InitPos, sSearch.c_str());
    if( ret == NULL) return 0;
    return  ret-sSrcStr.c_str()+1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

/** 
* @brief : ���� PC�� ��ġ�� LAN ī���� ��� IP LIST�� ���´�. 
* @param : TStringList * pList : IP String�� ���Ե� List
* @return bool : ���� ���� �� true ����. 
* @see 
- history : 1. [2015�� 7�� 2�� �����][�۽ſ�] ���� �ۼ� 
**/
bool __fastcall TGlobalFuction::LocalIPList(TStringList * pList)
{
    if(pList == NULL) return false;

  char Buffer[64];
  int I;
  WSADATA wsaData;
  PHOSTENT lpHostent = NULL;
  struct sockaddr_in addr;

  pList->Clear();
  int nRet = gethostname(Buffer, sizeof(Buffer));

  int  uLastError      = 0;
  bool bLoacalStartup  = false;

  if(nRet != 0) {
    //uLastError = GetLastError();
    uLastError = WSAGetLastError();

    if(uLastError == 10093) {
        WSAStartup(MAKEWORD( 2, 0 ), &wsaData);
        bLoacalStartup = true;

        nRet = gethostname(Buffer, sizeof(Buffer));

        if(nRet != 0) {
            WSACleanup();
            return false;
        }
    }
  }

  lpHostent = gethostbyname(Buffer);
  if( lpHostent == NULL )return false;
  I = 0;
  while(lpHostent->h_addr_list[I] != NULL)
  {
     memcpy( &(addr.sin_addr), lpHostent->h_addr_list[I], lpHostent->h_length );
     pList->Add( inet_ntoa(addr.sin_addr) );
     I++; 
  }

  if(bLoacalStartup) {
    WSACleanup();
  }

  return true;
}


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

AnsiString __fastcall TGlobalFuction::GetFileInformation(AnsiString AFilename, AnsiString AQuery)
{
/*    �� AQuery List
    - CompanyName
    - FileDescription
    - FileVersion
    - InternalName
    - LegalCopyright
    - LegalTrademarks
    - ProductName
    - ProductVersion
*/

    if(AQuery.IsEmpty()) return "";

    DWORD dwHandle;
    LPVOID ptr;
    UINT uLength;

    DWORD dwVersionInfoSize = GetFileVersionInfoSize(AFilename.c_str(), &dwHandle);
    if(dwVersionInfoSize == 0) return "";

    LPVOID pFileInfo = (LPVOID) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwVersionInfoSize);

    GetFileVersionInfo(AFilename.c_str(), dwHandle, dwVersionInfoSize, pFileInfo);
    VerQueryValue(pFileInfo, TEXT("\\VarFileInfo\\Translation"), &ptr, &uLength);

    WORD *id = (WORD*) ptr;
    AnsiString szString = "\\StringFileInfo\\" + IntToHex(id[0], 4) + IntToHex(id[1], 4) + "\\" + AQuery;

    VerQueryValue(pFileInfo, szString.c_str(), &ptr, &uLength);
    if(uLength == 0) return "";

    AnsiString sOut = AnsiString((char *) ptr);
    HeapFree(GetProcessHeap(), 0, pFileInfo);

    return sOut;
}


