//---------------------------------------------------------------------------
#define USE_BOOST_LIB 1     // boost를 사용할 경우 1, 사용하지 않을 경우 0으로 하고 컴파일


#include <vcl.h>
#include <Grids.hpp>        //
#include <shlobj.h>         // ShowDirSelectDlg()를 위
#include <shellapi.h>       // SHFileOperation()를 위해.
#include <Wincrypt.h>
#include "GlobalFunction.h"
#include <stdio.h>
#include <string.h>
#include <map>

#if USE_BOOST_LIB
#include <boost/regex.hpp>  // Boost.Regex lib (정규 표현식 사용 위해)
#endif

#pragma hdrstop

//---------------------------------------------------------------------------

#define DEL_LOG_DIR_EXPRESSION "(\\d{4}-\\d{2}-\\d{2})|(\\d{4}-\\d{2})" // 2014-03-12 or 2014-03 (정규 표현식 사용)

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
														// <-- 추가되는 위치..
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
bool __fastcall TGlobalFuction::IsNumStr(char *str, unsigned int nLen, bool bCareSign, bool bCarePoint)
{
    bool bPointExist = false;

    // NULL 일 경우 false
    if(str == NULL) return false;

    // 문자열의 실제 길이보다 인자로 받은 길이가 길 경우 처리.
    nLen = strlen(str) < nLen ? strlen(str) : nLen;

    // 문자열 처음과 끝에 소수점을 나타내는 점이 있을 경우 false
    if((str[0] == '.') || (str[nLen-1] == '.')) return false;

    // 숫자 앞에 -, + 기로를 숫자로 인식할 경우 해당 기호가 있는지 판별하여
    // 때어버리는 작업이다. 
    if(bCareSign){
        if((str[0] == '-') || (str[0] == '+')){
            str++;
            nLen--;
            if(nLen == 0) return false;
        }
    }

    while(nLen){
        nLen--;
        // 소수점도 감안할 경우 (소수점은 단 하나만 존재해야한다.)
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
* @brief : Hex 값을 String으로 변환해 준다. 0x123 이라면 "123" 으로 변환 한다. 
* @param : int n : 
* @return AnsiString : 
* @see 
- history : 1. [2013년 4월 10일 수요일][송신영] 최초 작성 
**/
AnsiString __fastcall TGlobalFuction::HexToStr(int n, bool bUpper)
{
    AnsiString sTemp;
    if(bUpper) sTemp.printf("%0X", n);
    else       sTemp.printf("%0x", n);
    return sTemp;
}

/**
* @brief : 현재 시각을 INT형으로 변환하여 리턴한다.
* @param : int nSec :
* @return AnsiString :
* @see
- history : 1. [2013년 10월 23일 월요일][이재득] 최초 작성
**/
int __fastcall TGlobalFuction::CurrentTime_SS()
{
    int sTime = 0;

    unsigned short Hour, Min, Sec, MSec;
    TDateTime dtPresent = Now(); // 현재 시간.

    // 현재 TDateTime을 년,월,일 시,분,초 로 나눈다.
    DecodeTime(dtPresent, Hour, Min, Sec, MSec);

    sTime += Hour*60*60;
    sTime += Min*60;
    sTime += Sec;

    return sTime;
}

/** 
* @brief : 초를 문자열 HH:MM:SS로 변경하여 리턴한다. 
* @param : int nSec : 
* @return AnsiString : 
* @see 
- history : 1. [2013년 4월 15일 월요일][송신영] 최초 작성 
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
* @brief : sSrc로 주어진 문자열을 cIndicator에 해당되는 문자로 구분하여 잘라 sList에 넣어준다. 
           잘려진 문자열의 처음과 끝부분의 공백은 없엔다. 
* @param AnsiString sSrc : 
* @param TStringList * sList : 
* @param char cIndicator : 
* @return int : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
    // SSY_081029 [ LOT ON 데이터 자르는 부분 수정 ] 
    int nComaPos = 0;
    sSrc = sSrc.Trim();

    while(sSrc.Length()){
        nComaPos = sSrc.AnsiPos(",");
        
        if(nComaPos == 1){
            sSrc = sSrc.Delete(1,1);
            sSrc = sSrc.Trim();
        }
        else if(nComaPos > 1){
            sBuf = sSrc.SubString(1, (nComaPos-1)); // 콤마 전까지의 스트링 구하기 
            sBuf = sBuf.Trim();

            nDivideCnt++;
            sList->Add(sBuf);                       // 리스트에 더하기 

            sSrc = sSrc.Delete(1, nComaPos);        // 콤마까지 삭제 
            sSrc = sSrc.Trim();            
        }
        else if(nComaPos == 0){                     // 마지막 스트링을 저장. 
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
* @brief : sSrc로 주어진 문자열을 cIndicator에 해당되는 문자로 구분하여 잘라 sList에 넣어준다. 
           잘려진 문자열의 처음과 끝부분의 공백은 bTrim인자가 true이면 삭제하고 false이면 유지한다. 
* @param String sSrc : 
* @param TStringList * sList : 
* @param wchar_t * sIndicator : 구분자로 사용할 케릭터를 문자열로 설정한다. (ex: L","콤마로 구분,   L";," 세미콜론과 콤마를 구분자로 사용)
* @param bool bTrim : 잘라진 문자열을 Trim 할지 여부 결정. 
* @return int : 
* @see 
- history  : 1. [2017년 11월 17일 금요일][Song Shin Young] First. 
**/
int __fastcall TGlobalFuction::StringToken(String &sSrc, TStringList * sList, wchar_t *sIndicator, bool bTrim)
{
    if(sList == NULL) return 0;
    else if(sSrc.Length() == 0) return 0;

    int nDivideCnt = 0; // Divide Count
    int nStrCnt = sSrc.Length() + 1;

    // 원본을 회손하지 않기 위해 버퍼를 만들어 사용한다. 
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
* @brief : sSrc로 주어진 문자열을 sIndicator 해당되는 문자로 구분하여 잘라 sList에 넣어준다. 
           잘려진 문자열의 처음과 끝부분의 공백은 bTrim인자가 true이면 삭제하고 false이면 유지한다. 
* @param AnsiString sSrc : 
* @param TStringList * sList : 
* @param char * sIndicator : 구분자로 사용할 케릭터를 문자열로 설정한다. (ex: ","콤마로 구분,   ";," 세미콜론과 콤마를 구분자로 사용)
* @param bool bTrim : 잘라진 문자열을 Trim 할지 여부 결정. 
* @return int : 
* @see     : 
- history  : 1. [2017년 11월 17일 금요일][Song Shin Young] First. 
**/
int __fastcall TGlobalFuction::StringToken(AnsiString &sSrc, TStringList * sList, char * sIndicator, bool bTrim)
{
    if(sList == NULL) return 0;
    else if(sSrc.Length() == 0) return 0;

    int nDivideCnt = 0; // Divide Count

    // 원본을 회손하지 않기 위해 버퍼를 만들어 사용한다. 
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
* @param TColor UpBgColor : TPanel의 Enable상태에서의 배경색 지정. 입력이 없을 경우 default로 clBtnFace 이다.
* @param TColor UpTxtColor : TLabel의 Enable상태 Font 색상이다. 입력이 없을 경우 default로 clBlue 이다.
* @return void : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 
                선택이 안된 상태로 그려주는 함수이다. 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : TPanel위에 TLabel로 Caption을 구현하여 버튼처럼 만든 것을 현재의 상태 그대로 Disable 상태로 만든다.  
* @param TObject *pPanel : TPanel Pointer
* @param TObject *pLable : TLabel Pointer
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : Error Code File에서 Message를 읽으면 \r\n이 \\r\\n으로 변경되어 읽혀진다. 
           이것을 다시 \r\n으로 만드는 함수이다.
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : 디렉토리 경로 에서 마지막에 "\\" 문자를 삭제한다. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성 
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
* @brief : 문자열 주에 \r \n 문자를 삭제한다. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012년 7월 6일 금요일][송신영] 최초 작성 
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
* @brief : 문자열의 마지막에 \r\n을 삭제한다. 
* @param : AnsiString &sMsg : 
* @return void : 
* @see 
- history : 1. [2012년 7월 6일 금요일][송신영] 최초 작성 
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
* @brief : 디렉토리 경로에서 마지막 디렉토리 이름을 잘라서 리턴해준다. 
          ex) "c:\\Temp\\Test\\" 라는 경로를 인자로넘겨준면 "Test" 가 리턴된다. 
* @param : AnsiString sPath : 디렉토리 경로 (파일 이름이 포함되면 안됨)
* @return AnsiString : 경로중에 마지막 디렉토리 이름을 리턴한다. 
* @see 
- history : 1. [2013년 3월 21일 목요일][송신영] 최초 작성 
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
* @brief : 파일 이름에서 확장자만 추출하여 리턴한다. "."은 포함되지 않는다. 
           ex1) "File.txt" 일 경우 txt를 리턴한다. 
           ex2) "c:\\Temp\\File.txt" 일 경우  txt를 리턴한다. 
* @param : AnsiString sFileName : 
* @return AnsiString : 
* @see 
- history : 1. [2013년 3월 21일 목요일][송신영] 최초 작성 
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
* @brief : 주어진 파일 이름을 해당 파일의 size를 리턴한다. 
* @param AnsiString &sFileName : 파일 이름. (Full Path)
* @param DWORD * pFileSizeHigh : 만약 File Size가 DWORD 범위를 벗어날 경우 pFileSizeHigh에 상위 값이 들어간다. 
* @return DWORD : File Size의 DWORD 범위 내의 값을 리턴한다. 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : 최대 공약수와 최대 공배수를 구하는 함수.
* @param int a : 입력 수 1
* @param int b : 입격 수 2
* @param int * pgcd : 최대 공약수를 리턴 
* @param int * plcm : 최대 공배수를 리턴. 
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
void __fastcall TGlobalFuction::StringGridSubCodeView(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TColor clBackColor,
          TColor clFontColor,
          int nFontSize,
          bool bTextAlignCenter,
          bool bTextBold)
{
    TStringGrid * pSG = dynamic_cast<TStringGrid *>(Sender);

    if(pSG == NULL) return;

    // 이전 값 기억.
    TColor clOrgBrush  = pSG->Canvas->Brush->Color;
    TColor clOrgFontColor = pSG->Font->Color;
    int nOrgFontSize   = pSG->Font->Size;
    TFontStyles OrgFontStyle = pSG->Canvas->Font->Style;

    // 새로운 값 설정.
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


    // 이전 값 복원.
    pSG->Canvas->Brush->Color = clOrgBrush;
    pSG->Canvas->Font->Size = nOrgFontSize;
    pSG->Canvas->Font->Color = clOrgFontColor;
    pSG->Canvas->Font->Style = OrgFontStyle;
}




/**
* @fn      : int SetFileDateTime(char *szFilePath, FILETIME ftCreated, FILETIME ftModified, FILETIME ftAccessed)
* @brief   : 파일 및 폴더의 시간을 주어진 인자로 변경한다. 
* @details : 
* @param   : char *szFilePath : 
* @param   : FILETIME ftCreated : 
* @param   : FILETIME ftModified : 
* @param   : FILETIME ftAccessed : 
* @return  : int : 
* @see     : 
- history  : 1. [2017년 5월 24일 수요일][송신영] 최초 작성 
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
* @brief : 지정된 Path의 하위 디렉토리를 모두 찾아 TStringList* pList 에 넘겨준다. 
* @param AnsiString Path :  찾을 Root 디렉토리. 
* @param TStringList * pList : 찾은 디렉토리 리스트로 반환할 포인터.  
* @return bool : 하나라도 발견되면 true 리턴, 하나도 없으면 false.
* @see 
- history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성 
**/
bool __fastcall TGlobalFuction::FindDir(AnsiString Path, __out TStringList * pList)
{
    if(pList == NULL) return false;

    pList->Clear();

    AnsiString sFindPath = Path;

    DeleteLastSlash(sFindPath);

    sFindPath = sFindPath + "\\*.*";
    
    TSearchRec sr;
    //파라미터로 지정한 티렉토리에서 하위 디렉토리를 찾는다.
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
* @brief : 주어진 폴더 내부에 모든 파일들을 찾아 리스트에 넣어준다. 
           bSubDir 가 true이면 하위 디렉토리도 찾는다. 
* @param AnsiString Path : 
* @param bool bSubDir : 
* @param __out TStringList * pList : 
* @return bool : 
* @see 
- history : 1. [2013년 3월 26일 화요일][송신영] 최초 작성 
**/
bool __fastcall TGlobalFuction::FindFileList(AnsiString Path, bool bSubDir, __out TStringList * pList)
{
    if(pList == NULL) return false;

    //pList->Clear();

    AnsiString sFindPath = Path;

    DeleteLastSlash(sFindPath);

    sFindPath = sFindPath + "\\*.*";
    
    TSearchRec sr;
    //파라미터로 지정한 티렉토리에서 하위 디렉토리를 찾는다.
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
* @brief   : 해당 파일의 생성 일자를 구한다. 
* @details : 
* @param   : AnsiString sFilePath : 
* @return  : TDateTime : 
* @see     : 
- history  : 1. [2017년 4월 19일 수요일][송신영] 최초 작성 
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
* @brief : 인자로 주어진 폴더 내의 파일들 중에 가장 최신의 파일의 시간을 리턴 한다. 
* @param AnsiString Mask : 
* @return TDateTime : 
* @see 
- history : 1. [2013년 3월 27일 수요일][송신영] 최초 작성 
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
* @brief : 주어진 경로에서 주어진 파일 이름을 포함한 파일 중에 가장 최신의 파일명을 리턴한다. 
* @param AnsiString sDirPath  : 폴더 경로 
* @param AnsiString sFileName : 파일에 포함된 문자, ""일 경우 모든 파일 중에 가장 최신을 찾는다. 
* @param AnsiString sExt      : 파일의 확장자, ""일 경우 모든 확장자에서 찾는다. 
* @return AnsiString : 가장 최신의 파일 이름 리턴. 
* @see 
- history : 1. [2015년 7월 2일 목요일][송신영] 최초 작성 
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
bool __fastcall TGlobalFuction::DeleteOldTimeFile(AnsiString  sSrcPath, TDateTime tmStandardTime, int  nTimeType, bool bSubDir)
{

    bool bAllFileDelete = true;
    DeleteLastSlash(sSrcPath);

    if(sSrcPath == "") return false;

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;
    TDateTime  tmFileDateTime;

    // 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // 디렉토리.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    // 서브 디렉토리 밑에 까지 확인하여 삭제할 경우
                    if(sr.Name != ".." && sr.Name != ".") {

                        bAllFileDelete = false;
                        
                        if(bSubDir) {
                            
                            sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                            // 재귀 호출 한다.
                            if(DeleteOldTimeFile(sSrcPathTemp, tmStandardTime, nTimeType, bSubDir)) {

#if USE_BOOST_LIB
//---------------------------------------------------------------------------------------

                                // 폴더의 이름이 "년-월-일" 이거나 "년-월" 일 경우 폴더가 비어있으면 삭제한다.
                                std::string sPath = AnsiString(sr.Name).c_str();
								boost::regex re;

								try{
                                    // Set up the regular expression for case-insensitivity
                                    re.assign(DEL_LOG_DIR_EXPRESSION, boost::regex_constants::icase);
                                }
                                catch (boost::regex_error& e) {
                                    continue;
                                }

                                // 정규 표현식과 매치가 되면 해당 폴더를 삭제한다.
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
                // 파일.------------------------------
                else {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                    // 마지막 Write Time 기준
                    if(nTimeType == 1){
                        FileTimeToLocalFileTime( &sr.FindData.ftLastWriteTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);
                    }
                    // 마지막 Access Time 기준
                    else if(nTimeType == 2){
                        FileTimeToLocalFileTime( &sr.FindData.ftLastAccessTime, &TempTime );
                        FileTimeToSystemTime( &TempTime, &SystemTime );

                        tmFileDateTime = SystemTimeToDateTime(SystemTime);
                    }
                    // 생성한 날짜 기준
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
* @brief   : 인자로 주어진 시간보다 오래된 폴더를 찾아 삭제한다. 
* @details : sSrcPath에 주어진 폴더에서 주어진 tmStandardTime 시간 보다 생성 시간이 오래된 폴더는 모두 삭제한다. 
* @param   : AnsiString  sSrcPath :         삭제할 폴더 Root 경로 (Root 경로는 삭제되지 않는다.)
* @param   : TDateTime tmStandardTime :     삭제할 기준 시간 (이 시간보다 오래된 폴더는 삭제한다.)
* @return  : bool : 
* @see     : 
- history  : 1. [2017년 5월 24일 수요일][송신영] 최초 작성 
**/
bool __fastcall TGlobalFuction::DeleteOldTimeDir(AnsiString  sSrcPath, TDateTime tmStandardTime)
{

    bool bAllFileDelete = true;
    DeleteLastSlash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;
    TDateTime  tmFileDateTime;

    // 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // 디렉토리.--------------------------
                if((sr.Attr & faDirectory) == faDirectory) {

                    // 서브 디렉토리 밑에 까지 확인하여 삭제할 경우
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
                // 파일.------------------------------
                else {
                    // 파일은 신경 안씀. 
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

    }

    return bAllFileDelete;
}

//---------------------------------------------------------------------------

/**
* @brief : 지정된 Path의 하위 디렉토리를 모두 찾아 삭제한다.
           bool bDeleteRoot 를 true로 설정하면 호출한 자신의 디렉토리도 삭제한다.
           그러나 false로 지정하면 호출한 자신의 디렉토리는 그대로 두고 자식 디렉토리와 파일들만 삭제한다.
* @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
* @param Tbool bDeleteRoot : 호출한 Root 디렉토리도 삭제할 경우 true로 설정한다.
* @return void : 없음.
* @see
- history : 1. [2012년 9월 28일 금요일][송신영] 최초 작성
**/
void __fastcall TGlobalFuction::DeleteDirectory(AnsiString  sSrcPath, bool bDeleteRoot)
{
    DeleteLastSlash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // 디렉토리.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        // 재귀 호출 한다.
                        DeleteDirectory(sSrcPathTemp, true);

                        // 해당 디렉토리의 하위 디렉토리와 파일들을 모두 삭제하였으면 자신을 삭제 시킨다.
                        //RemoveDir(sSrcPathTemp);
                    }
                }
                // 파일.------------------------------
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
            // 해당 디렉토리의 하위 디렉토리와 파일들을 모두 삭제하였으면 자신을 삭제 시킨다.
            RemoveDir(sSrcPath);
        }

    }
}

/**
* @brief : 지정된 Path의 하위 디렉토리를 에서 디렉토리 이름이 sDirName인 것을 찾아 모두 찾아 삭제한다.
* @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
* @param AnsiString sDirName :  삭제할 디렉토리 이름.
* @param  bool bDelete       :  true이면 찾아진 폴더나 파일을 모두 삭제한다. false이면 디렉토리 이름이 sDirName인것을 찾는 동작만 한다.
* @return void : 없음.
* @see
- history : 1. [2013년 3월 21일 금요일][송신영] 최초 작성
**/
void __fastcall TGlobalFuction::DeleteDirectory(AnsiString  sSrcPath, AnsiString sDirName, bool bDelete)
{
    //static int count = 0;

    sSrcPath = ExcludeTrailingBackslash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // 디렉토리.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        bool bSubDelete = (sr.Name == sDirName) ? true : bDelete;

                        // 재귀 호출 한다.
                        DeleteDirectory(sSrcPathTemp, sDirName, bSubDelete);

                        // 해당 디렉토리의 하위 디렉토리와 파일들을 모두 삭제하였으면 자신을 삭제 시킨다.
                        //RemoveDir(sSrcPathTemp);
                    }
                }
                // 파일.------------------------------
                else if(bDelete) {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                    DeleteFile(sSrcPathTemp.c_str());

                    //TraceUpdate("파일 삭제 : " + sSrcPathTemp);
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);

        if(bDelete) {
            // 해당 디렉토리의 하위 디렉토리와 파일들을 모두 삭제하였으면 자신을 삭제 시킨다.
            RemoveDir(sSrcPath);
            //TraceUpdate("폴더 삭제 : " + sSrcPath);
        }
    }
}


//---------------------------------------------------------------------------

/**
* @brief : 지정된 Path의 파일 중에 확장자가 지정된 확장자와 동일하면 삭제한다.
           bSubDir 가 true이면 하위 디렉토리도 찾아서 삭제한다.
* @param AnsiString sSrcPath :  삭제할 Root 디렉토리.
* @param AnsiString sExeName :  삭제할 파일 확장자.
* @param  bool bSubDir       :  true이면 하위 디렉토리도 찾는다.
* @return void : 없음.
* @see
- history : 1. [2013년 3월 21일 금요일][송신영] 최초 작성
**/
void __fastcall TGlobalFuction::DeleteFileByExtension(AnsiString  sSrcPath, AnsiString sExeName, bool bSubDir)
{
    sSrcPath = ExcludeTrailingBackslash(sSrcPath);

    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(sSrc, faAnyFile, sr) == 0){
            do{
                // 디렉토리.--------------------------
                if((sr.Attr & faDirectory) == faDirectory){

                    if(sr.Name != ".." && sr.Name != "."){
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;

                        // 재귀 호출 한다.
                        if(bSubDir) DeleteFileByExtension(sSrcPathTemp, sExeName, bSubDir);
                    }
                }
                // 파일.------------------------------
                else {
                    FILETIME TempTime;
                    SYSTEMTIME SystemTime;

                    AnsiString sFileName = GetFileExt(sr.Name);

                    if(sFileName == sExeName) {
                        sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                        DeleteFile(sSrcPathTemp.c_str());
                        //TraceUpdate("파일 삭제 : " + sSrcPathTemp);
                    }
                }
            }while(FindNext(sr)==0);
        }

        FindClose(sr);
    }
}

/** 
* @brief : 주어진 sFileName에서 확장자를 sExt로 변경해준다. 
* @param AnsiString sFileName : 확장자를 변경하고자하는 파일 이름으로 전체 경로가 포함되어도 상관 없다. 
* @param AnsiString sExt : 확장자에는 ".Exe" 처럼 '.'을 포함하여야 한다. 
* @return AnsiString : 확장자를 변경한 파일 이름을 리턴한다. 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
**/
AnsiString  __fastcall TGlobalFuction::ChangeExtension(AnsiString sFileName, AnsiString sExt)
{
    AnsiString sRet  = ChangeFileExt(sFileName, sExt);
    return sRet;
}



// 특정 폴더로 모든 파일을 복사 
/** 
* @brief : 
* @param const char *srcfiles :  파일 이름이 있는 Full Path ("c:\\Temp\\*.ini" )
* @param const char *destdir :   복사될 목표 폴더. 
* @return int : 
* @see 
- history : 1. [2013년 3월 28일 목요일][송신영] 최초 작성 
**/
int __fastcall TGlobalFuction::CopyFiles(const char *srcfiles, const char *destdir) 
{ 
#if 1

    int nRet = 0;
    AnsiString sSrcPath = ExtractFileDir(srcfiles);     // 경로에서 파일 이름을 삭제하고 Path만 추출한다.
    AnsiString sTargetPaht = destdir;

    sSrcPath = sSrcPath + "\\";

    DeleteLastSlash(sTargetPaht);
    sTargetPaht = sTargetPaht + "\\";

    TSearchRec sr;
    AnsiString sSrcPathTemp;

    // Target 폴더 없으면 새로 생성. 
    if(!DirectoryExists(sTargetPaht)) {
        ForceDirectories(sTargetPaht);
    }

    // Source 디렉토리 존재하는지 확인
    if(DirectoryExists(sSrcPath)) {

        if(FindFirst(srcfiles, faAnyFile, sr) == 0){
            do{
                // 파일만 검색.------------------------------
                if((sr.Attr & faDirectory) != faDirectory) {
                    AnsiString sSrcFile = sSrcPath + sr.Name;
                    AnsiString sTrgFile = sTargetPaht + sr.Name;

                    try {
                        bool bRet = CopyFile(sSrcFile.c_str(), sTrgFile.c_str(), false);   // false이면 덮어쓰기 허용.
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
* @brief : Directory에서 Directory로 sub 폴더를 포함하여 복사 한다.
* @param AnsiString sSrcPath : Source Directory
* @param AnsiString sTrgPath : Target Directory
* @param TStringList * slSkipDirectory : sub Directory 중에서 복사하지 말아야할 이름을 지정할 수 있다.
* @return bool : 정상일 경우 true 리턴.
* @see
- history : 1. [2015년 11월 24일 화요일][송신영] 최초 작성
**/
bool __fastcall TGlobalFuction::DirectoryCopy(AnsiString sSrcPath, AnsiString sTrgPath, TStringList * slSkipDirectory)
{
    bool bRetValue = true;
    bool bCopySkip = false;
    AnsiString sSrc = sSrcPath + "\\*.*";
    TSearchRec sr;
    AnsiString sSrcPathTemp, sTrgPathTemp;

    // 복사될 장소의 디렉토리가 없을 경우 생성한다.
    if(!DirectoryExists(sTrgPath)) {
        if(ForceDirectories(sTrgPath) != true) {
            return false;
        }
    }

    if(FindFirst(sSrc, faAnyFile, sr) == 0){
        do{
            // 디렉토리.
            if((sr.Attr & faDirectory) == faDirectory){
                if(sr.Name != ".." && sr.Name != "."){
                    sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                    sTrgPathTemp = sTrgPath + "\\" + sr.Name;

                    bCopySkip = false;

                    if(slSkipDirectory != NULL) {
                        for(int i=0; i<slSkipDirectory->Count; i++){
                            // 마지막에 \ 기호가 있으면 삭제한다.
                            if(slSkipDirectory->Strings[i].c_str()[slSkipDirectory->Strings[i].Length()-1] == '\\'){
                                slSkipDirectory->Strings[i].Delete(slSkipDirectory->Strings[i].Length(), 1);
                            }

                            // Copy를 Skip해야하는 Directory일 경우 복사하지 않는다.
                            AnsiString sSkipDir = slSkipDirectory->Strings[i];
                            if(stricmp(sSrcPathTemp.c_str(), sSkipDir.c_str()) == 0){
                                bCopySkip = true;
                                break;
                            }
                        }
                    }

                    // Copy를 Skip해야하는 Directory가 아니면 제귀호출로 들어간다.
                    if(!bCopySkip) DirectoryCopy(sSrcPathTemp, sTrgPathTemp, slSkipDirectory);
                }
            }
            // 파일.
            else {
                sSrcPathTemp = sSrcPath + "\\" + sr.Name;
                sTrgPathTemp = sTrgPath + "\\" + sr.Name;

                bCopySkip = false;

                if(slSkipDirectory != NULL) {
                    for(int i=0; i<slSkipDirectory->Count; i++){

                        // Copy를 Skip해야하는 파일일 경우 복사하지 않는다.
                        AnsiString sSkipFile = slSkipDirectory->Strings[i];
                        if(stricmp(sSrcPathTemp.c_str(), sSkipFile.c_str()) == 0){
                            bCopySkip = true;
                            break;
                        }
                    }
                }

                if(!bCopySkip) CopyFile(sSrcPathTemp.c_str(), sTrgPathTemp.c_str(), false); //덮어쓰기
                //CopyFile(sSrc.w_str(), sTrg.w_str(), false); //덮어쓰기
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
    
	//이름을 바꾸고 결과를 리턴 받는다.
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
    
	//이름을 바꾸고 결과를 리턴 받는다.
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
            BrowseInfo.lpszTitle      = _T("아래 목록에서 원하는 폴더를 선택하세요.") ;
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
* @brief : 현재 실행 파일의 Root 폴더를 리턴한다. 뒤에 "\\"가 붙는다. 
* @param : 없음.
* @return AnsiString : 실행파일 Path
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
**/
AnsiString __fastcall TGlobalFuction::GetApplicationDir() 
{
    // 각종 경로 설정. --------------------------------
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
* @details : 현재 실행 파일이 있는 Disk의 이름을 리턴하는 함수. 
* @param   : None
* @return  : char : 찾으면 찾은 Disk Name을 리턴하고 모찾으면 '\0' 를 리턴한다. 
* @see     : 
- history  : 1. [2018년 1월 25일 목요일][Song Shin Young] First. 
**/
char __fastcall TGlobalFuction::GetApplicationDiskName() 
{
    // 각종 경로 설정. --------------------------------
    AnsiString sDrive = ExtractFileDrive(Application->ExeName);
    if(sDrive == "") return '\0';
    
    return sDrive.c_str()[0];
}


/** 
* @brief : File의 MD5 Hash Code를 생성하는 함수이다. 
* @param __in LPCSTR filename : 파일 이름(전체 경로)
* @param __out LPSTR pRetHashCode : 생성된 MD5 Hash Code를 Hex Type String으로 변환하여 리턴한다. 
                                    Hash Code Len가 16 byte이므로 Hex Type String으로 변환하면 32 Byte가 된다. 
* @return bool : 정상:true, 실패:false
* @see 
- history : 1. [2015년 10월 22일 목요일][송신영] 최초 작성 
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
* @brief   : 인자로 주어진 드라이브가 현재 존재하는지 확인하는 함수.
* @details : 
* @param   : char cDriveName : 확인하고자하는 드라이브 이름 'c', 'd' ...
* @param   : bool bOnlyHDD : true  : 이 값이 true이면 오직 HDD 인 경우만 true를 리턴한다. 
                             false : 이 값이 false이면 Removable, Remote, CD-Rom, RamDisk도 true로 리턴한다. 
* @return  : bool : 드라이브 명이 존재하면 true를 리턴한다. 
* @see     : 
- history  : 1. [2017년 1월 4일 수요일][송신영] 최초 작성 
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
* @details : 주어진 디스크의 여유 공간을 MByte 단위로 리턴하는 함수 
* @param   : char cDrive : 
* @return  : double : 여유 공간 MByte
* @see     : 
- history  : 1. [2018년 1월 25일 목요일][Song Shin Young] First. 
**/
double __fastcall TGlobalFuction::GetDiskFreeSpace(char cDrive)
{   
    int nDriveIndex = cDrive == '\0' ? 0 : cDrive - 'A' + 1;
    
    //__int64 전체용량 = DiskSize(nDriveIndex);   // 0: 현재 디스크, 1:A, 2:B, 3:C, 4:D ...... 
    __int64 nFreeSpace = DiskFree(nDriveIndex);

    double dFreeSpaceMByte = (nFreeSpace >> 10) / 1024.0;

    return dFreeSpaceMByte;
}















//---------------------------------------------------------------------------

/** 
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
void __fastcall TGlobalFuction::DrawGraphBackLine(Graphics::TBitmap* Bitmap, int nWidth, int nHeight, int nXpitch, int nYpitch)
{
    int nCountX = 0;
    int nCountY = 0;
    int nStartX = 0, nStartY = 0;
    int nEndX   = 0, nEndY   = 0;
    int nTimeLineCnt = 0 ;

    Bitmap->Canvas->Pen->Color = (TColor)RGB(0, 57, 0);
    Bitmap->Canvas->Pen->Width = 1;

    // X축 구분선 그리기 ------------
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

    // Y 축 구분선 그리기 ----------
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
* @brief : pDest에 srcBmp를 X,Y 위치에 투명처리하여 그린다. 
* @param Graphics::TBitmap* pDest : 그려질 목적지 Bitmap
* @param int X : pDest위의 픽셀 좌표 (Widht 방향 시작 점)
* @param int Y : pDest위의 픽셀 좌표 (Height 방향 시작 점)
* @param Graphics::TBitmap* srcBmp : 그릴 소스 Bitmap
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
**/
void __fastcall TGlobalFuction::DrawImageTrans(Graphics::TBitmap* pDest, int X, int Y, Graphics::TBitmap* srcBmp)
{
    srcBmp->Transparent = true;
    srcBmp->TransparentColor = clFuchsia;
    pDest->Canvas->Draw(X, Y, srcBmp);
}
//---------------------------------------------------------------------------

/** 
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
    pTextBitmap->Canvas->Font->Name = "굴림";
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
* @brief   : 주어진 Canvas에 해당 색상으로 수직 방향으로 진행되는 그라디언트를 그린다. (보통 StringGrid에서 선택된 Item을 그릴 때 사용한다.)
* @details : 
* @param   : TCanvas *   pCanvas :      그려질 대상 Canvas
* @param   : TColor      StartColor :   그라데이션 시작할 기준 색상 
* @param   : TColor      EndColor :     그라데이션 종료할 기준 색상 
* @param   : int         ShadingLevel : 시작 부터 종료까지 색상 단계 값 (클 수록 부드럽다)
* @param   : TRect       &Rect :        그라데이션을 그릴 범위 
* @return  : void : 
* @see     : 
- history  : 1. [2016년 9월 23일 금요일][송신영] 최초 작성 
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
* @brief : 8 bit Raw 데이터 이미지를 24Bit BMP 파일로 생성해준다. 
* @param BYTE * pRawData : 8 bit Raw Image 데이타 
* @param TMemoryStream * pStm : 24 Bit BMP 로 변경된 데이터 반환 TMemorySteam 포인터 
* @param int nWidth : 이미지 Width
* @param int nHeight : 이미지 Height
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
**/
void __fastcall TGlobalFuction::Raw8Bit_to_Bmp24Bit(BYTE * pRawData, TMemoryStream * pStm, int nWidth, int nHeight)
{
    if(pRawData == NULL || pStm == NULL) return;

//    int COLOR = 256;

    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    RGBQUAD bmp_pal[COLOR];

    //BITMAPINFOHEADER 설정
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

    //BITMAPFILEHEADER 설정
    bmfh.bfType             =    0x4d42;  //BM

    bmfh.bfSize             = sizeof(BITMAPFILEHEADER)+((COLOR)*sizeof(RGBQUAD))+sizeof(BITMAPINFOHEADER)
                             + bmih.biSizeImage -2;
    bmfh.bfReserved1        = 0;
    bmfh.bfReserved2        = 0;
    bmfh.bfOffBits          = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
                              +(COLOR*sizeof(RGBQUAD))-2;

    //PALETTE 설정
    for(int i=0;i<COLOR;i++)           //8bit-> 256 monochrome 설정
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
* @brief : Image raw data를 상하 반전 시키는 함수 
* @param BYTE *data : Raw image data 포인터 
* @param int nPixelByte : 한 Pixel당 차지하는 byte 수 
* @param int biWidth : 이미지 넓이 
* @param int biHeight : 이미지 높이 
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : nPoint 의 소수점 자리만큼 반올림하여 리턴한다. 
           3.14159 를 nPoint 가 2 라면 3.14를 리턴한다. 
           만약 nPoint가 3이라면 3.142 를 리턴한다. 
* @param double dSrc : 반올림할 원래의 값 
* @param int nPoint :  반올림할 소수점 자리 
* @return double :     
* @see 
- history : 1. [2013년 4월 2일 화요일][송신영] 최초 작성 
**/
double __fastcall TGlobalFuction::DecimalRounding(double dSrc, int nPoint)
{
    int k=pow(10,nPoint);
    double Result=floor(dSrc * k + 0.5) / k;
    return Result;
}


//--------------------------------------------------------------

/** 
* @brief : double 타입 오차 허용범위 내에서 비교하기
* @param double x : 
* @param double y : 
* @param int ulpsTolerance = 4 : 
* @return int : 
* @see 
- history : 1. [2015년 4월 2일 목요일][송신영] 최초 작성 
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
* @brief :  오차 허용 범위 내에서 비교하기 - 절대 범위 + ULP 사용 
            ULP를 이용한 비교가 얼핏 보면 절대 범위를 사용하는 것처럼 보이지만,
            1 ULP의 크기는 숫자의 크기에 따라 변하는 상대적인 값이기 때문에 상대 범위 비교에 해당된다고 볼 수 있습니다.
            그러므로 이전에 설명했었던 CompareDoubleRelative 함수와 마찬가지로 0에 가까운 아주 작은 값을 비교할 때에
            (흔히 발생되는 상황은 아니지만) 문제가 발생할 여지가 남아있습니다. 
            결국 이러한 상황까지 고려하려면 아래와 같이 절대 범위 비교와 ULP 비교를 조합하여 구현해야 합니다. 

* @param double x : 
* @param double y : 
* @param double absTolerance = (1.0e-8 : 
* @return int : 
* @see 
- history : 1. [2015년 4월 2일 목요일][송신영] 최초 작성 
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
* @brief : ComPort의 상태를 확인하여 사용 가능한 Port인지 확인한다. 
* @param : char * szPort : ComPort Name ("COM4")
* @return bool : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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

    if(hCommPort == INVALID_HANDLE_VALUE) //Port번호에 문제가 있거나 사용중인경우등.. 
    { 
        return false; 
    } 

    CloseHandle(hCommPort); 
    return true; 
    
} 
//---------------------------------------------------------------------------

/** 
* @brief : TRadioGroup 의 ItemIndex를 변경시 OnClick Event가 발생하는데 
           이 함수를 하용하면 OnClick Event 없이 ItemIndex를 변경할 수 있음. 
* @param TRadioGroup *pRGroup : 
* @param int Index : 
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
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
* @brief : sStr 에서 sS에 해당되는 문자열을 sT로 바꾼다. 
* @param AnsiString &sStr : 변경할 전체 문자열 (참조형으로 변경된 후 값이 이곳으로 인자로 넘긴 변수의 값이 변경된다.)
* @param AnsiString sS : 찾을 문자열 
* @param AnsiString sT : 대치할 문자열 
* @return void : 
* @see 
- history : 1. [2015년 10월 23일 금요일][송신영] 최초 작성 
**/
void __fastcall TGlobalFuction::ReplaceStr(AnsiString &sStr, AnsiString sS, AnsiString sT)
{
    sStr = StringReplace(sStr, sS, sT, TReplaceFlags()<<rfReplaceAll);
}


//---------------------------------------------------------------------------
/**
* @fn      : int __fastcall TGlobalFuction::StringPos(const AnsiString & sSrcStr, AnsiString sSearch, Cardinal InitPos)
* @brief   : 
* @details : 주어진 문자열에서 시작 위치 부터 검색 문자열이 찾아지는 위치 값을 리턴 함. (찾은 경우 1 이상의 값을 리턴)
* @param   : const AnsiString & sSrcStr : 
* @param   : AnsiString sSearch : 
* @param   : Cardinal InitPos : 
* @return  : int : 0 : 못찾음, 1이상 : 찾은 문자열 위치 
* @see     : 참고 싸이트 : http://cbuilder.borlandforum.com/impboard/impboard.dll?action=read&db=bcb_tip&no=684
- history  : 1. [2018년 1월 17일 수요일][Song Shin Young] First. 
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
* @brief : 현재 PC에 설치된 LAN 카드의 모든 IP LIST를 얻어온다. 
* @param : TStringList * pList : IP String이 포함된 List
* @return bool : 정상 동작 시 true 리턴. 
* @see 
- history : 1. [2015년 7월 2일 목요일][송신영] 최초 작성 
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

AnsiString __fastcall TGlobalFuction::GetFileInformation(AnsiString AFilename, AnsiString AQuery)
{
/*    ※ AQuery List
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


