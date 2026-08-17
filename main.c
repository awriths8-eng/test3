#include "main.h"

// ---------------------------------------------------------------------------
// 測試用全局變數定義（確保 AC5 編譯時 Symbol 可正確 Resolved）123456
// ---------------------------------------------------------------------------
uint8_t SCPIChannel = 1;
uint16_t SCPIIndexPara = 7;
char SCPIParaValue[100] = "V,I,W";
uint8_t SCPICommaCount = 2;

uint8_t SetupCompareElemAllValue = 0;
uint8_t SetupCompareItemAllValue = 0;
uint8_t SetupCompareValue[10] = {0};

uint8_t SetupCompareItemValue1[20][1] = {{0}};
uint8_t SetupCompareItemValue2[20][1] = {{0}};
uint8_t SetupCompareItemValue3[20][1] = {{0}};
uint8_t SetupCompareItemValue4[20][1] = {{0}};

// ---------------------------------------------------------------------------
// 輔助與樁函式（Stub Functions）
// ---------------------------------------------------------------------------
char *upper(char *str)
{
    char *p = str;
    while (*p)
    {
        *p = (char)toupper((unsigned char)*p);
        p++;
    }
    return str;
}

void yyerror(int err_code) { (void)err_code; }
void DecideComparePage(void) {}
void ParaValueToFram(uint8_t a, uint8_t b, uint8_t c) { (void)a; (void)b; (void)c; }
void SCPI_SetComparItemMaxMin(uint8_t mode, uint8_t channel, uint8_t item, uint8_t val) 
{ 
    (void)mode; (void)channel; (void)item; (void)val; 
}
void GetCompareSetupMaxMainValue(uint8_t channel, uint8_t item) { (void)channel; (void)item; }
void DcideCompareElemItem(void) {}

// ---------------------------------------------------------------------------
// 待掃描目標函式
// ---------------------------------------------------------------------------
void SCPI_ChromaSetComparItemFun(void)
{
    char MeasView_S[29][7] = {"V","VPK+","VPK-","I","IPK+","IPK-","W","P+pk","P-pk","VA",
                              "VAR","PF","CFV","CFI","DEG","FREQ","IHz","THDV","THDI","IS",
                              "ENEG","WP+","WP-","P(avg)","q","q+","q-","q(avg)","NONE"};
    char FunNamePara[7] = {0};
    uint8_t R[17] = {0}, S = 0, s1 = 0, e1 = 0, i = 0, j = 0, c[3] = {0}, Elem = 0;
    bool Status = false;
    Elem = SCPIChannel;
    uint8_t test_check1 = 1;
    uint8_t test_check2 = 2;

    // [異常/錯誤] i 為 uint8_t，但 SCPIIndexPara 是 uint16_t，最大可到 65535。
    // 若 SCPIIndexPara 介於 100~255，i 會超過 SCPIParaValue[100] 的合法範圍造成越界讀取；
    // 若 SCPIIndexPara > 255，i（uint8_t）永遠追不上，會在 255 溢位歸零，造成無窮迴圈（DoS 風險）。
    for (i = 0; i < SCPIIndexPara; i++)
    {
        if (SCPIParaValue[i] == ',')
        {
            c[0] += 1;
            if (c[0] == 1)
            {
                s1 = 0;
                // [嚴重異常] 整數下溢：若第一個逗號出現在 i = 0（字串開頭就是逗號），
                // e1 = (uint8_t)(0 - 1) = 255，而非預期的 -1。
                // 會導致下方 for (j = s1; j <= e1; j++) 從 0 跑到 255（跑 256 次），
                // 造成 FunNamePara[7]（stack buffer）嚴重溢位寫入，
                // 以及 SCPIParaValue[j] 讀取超出其 100 bytes 範圍。可被輸入資料直接觸發。
                e1 = i - 1;
            }
            else
            {
                s1 = e1 + 2;
                e1 = i - 1;
            }
            Status = true;
        }
        else if (i == SCPIIndexPara - 1)
        {
            if (c[0] == 0)
            {
                s1 = 0;
            }
            else
            {
                s1 = e1 + 2;
            }
            e1 = SCPIIndexPara - 1;
            Status = true;
        }

        if (Status == true)
        {
            for (j = s1; j <= e1; j++)
            {
                // [異常] FunNamePara[7] 寫入沒有邊界檢查。
                // 若逗號分段字串長度 >= 7（或上方 e1 下溢時），c[2] 會超過 6，
                // 造成堆疊緩衝區溢位寫入（stack buffer overflow）。
                FunNamePara[c[2]] = SCPIParaValue[j];
                c[2]++;
            }
            for (j = 0; j < 29; j++)
            {
                S = strcmp(MeasView_S[j], upper(FunNamePara));
                if (S == 0)
                {
                    // [異常] R[17] 寫入沒有邊界檢查。
                    // c[1] 完全由輸入字串中有效逗號分段數量決定，若超過 17 筆，
                    // 這裡會先於下方 SetupCompareValue[7] = R[j] 讀取端更早發生寫入越界，是問題根源。
                    R[c[1]] = j + 1;
                    c[1]++;
                    break;
                }
            }
            memset(FunNamePara, 0, sizeof(FunNamePara));
            Status = false;
            c[2] = 0;
        }
    }

    if (c[1] != SCPICommaCount + 1)
    {
        yyerror(E_104);
        return;
    }

    for (i = Elem; i < Elem + 1; i++)
    {
        SetupCompareElemAllValue = i;
        // [異常] for (j = 0; j < 20; j++) 上界寫死為 20，
        // 但下方 R[17] 只宣告 17 個元素；j 到 17、18、19 時 R[j] 會越界讀取（CodeQL: cpp/constant-array-overflow）。
        for (j = 0; j < 20; j++)
        {
            switch (i)
            {
            // [異常] switch (i) 沒有 default 分支：若 Elem（即 SCPIChannel）不是 1~4，
            // 會靜默不做任何事而不報錯，容易埋藏隱藏 bug。
            case 1:
                SetupCompareItemValue1[j][0] = 29;
                break;
            case 2:
                SetupCompareItemValue2[j][0] = 29;
                break;
            case 3:
                SetupCompareItemValue3[j][0] = 29;
                break;
            case 4:
                SetupCompareItemValue4[j][0] = 29;
                break;
            }
            SetupCompareItemAllValue = j + 1;
            SetupCompareValue[6] = j + 1;
            DecideComparePage();
            ParaValueToFram(29, 6, 8);
            // [異常] R[17] 陣列越界讀取：當 j = 17, 18, 19 時，R[j] 超出宣告範圍。
            // CodeQL 掃描規則 cpp/constant-array-overflow 已標記此行（off-by-3）。
            SetupCompareValue[7] = R[j];
            ParaValueToFram(29, 7, 8);
            SCPI_SetComparItemMaxMin(1, i, j + 1, 0);
            SCPI_SetComparItemMaxMin(2, i, j + 1, 0);
            GetCompareSetupMaxMainValue(i, j + 1);
        }
    }

    DcideCompareElemItem();

    if (R[0] == 29 && c[1] == 1)
    {
        return;
    }

    for (i = Elem; i < Elem + 1; i++)
    {
        SetupCompareElemAllValue = i;
        for (j = 0; j < c[1]; j++)
        {
            switch (i)
            {
            // [異常] 同上，switch (i) 缺少 default 分支。
            case 1:
                SetupCompareItemValue1[j][0] = R[j];
                break;
            case 2:
                SetupCompareItemValue2[j][0] = R[j];
                break;
            case 3:
                SetupCompareItemValue3[j][0] = R[j];
                break;
            case 4:
                SetupCompareItemValue4[j][0] = R[j];
                break;
            }
            SetupCompareItemAllValue = j + 1;
            SetupCompareValue[6] = j + 1;
            DecideComparePage();
            ParaValueToFram(29, 6, 8);
            SetupCompareValue[7] = R[j];
            ParaValueToFram(29, 7, 8);
            SCPI_SetComparItemMaxMin(1, i, j + 1, 0);
            SCPI_SetComparItemMaxMin(2, i, j + 1, 0);
            GetCompareSetupMaxMainValue(i, j + 1);
        }
    }

    DcideCompareElemItem();

    // [異常] 條件恆為 false：test_check1 不可能同時 >= 8 且 <= 4，
    // 這段程式碼永遠不會被執行到（CodeQL 已標記為「比較結果始終相同」）。
    if (test_check1 >= 8 && test_check1 <= 4)
    {
        test_check2 = 99;
    }
}

int main(void)
{
    SCPI_ChromaSetComparItemFun();
    while (1)
    {
        // STM32 Main Loop
    }
}
