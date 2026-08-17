#include "main.h"

// 1234567890123
// ---------------------------------------------------------------------------
// 測試用全局變數定義（確保 AC5 編譯時 Symbol 可正確 Resolved）
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

    for (i = 0; i < SCPIIndexPara; i++)
    {
        if (SCPIParaValue[i] == ',')
        {
            c[0] += 1;
            if (c[0] == 1)
            {
                s1 = 0;
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
                FunNamePara[c[2]] = SCPIParaValue[j];
                c[2]++;
            }
            for (j = 0; j < 29; j++)
            {
                S = strcmp(MeasView_S[j], upper(FunNamePara));
                if (S == 0)
                {
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
        for (j = 0; j < 20; j++)
        {
            switch (i)
            {
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
