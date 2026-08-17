#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// 若有安裝 CMSIS 庫可取消註解；若無，程式碼已內建必要型態
// #include "stm32f4xx.h"

#define E_104 104

// 全局變數宣告
extern uint8_t SCPIChannel;
extern uint16_t SCPIIndexPara;
extern char SCPIParaValue[100];
extern uint8_t SCPICommaCount;

extern uint8_t SetupCompareElemAllValue;
extern uint8_t SetupCompareItemAllValue;
extern uint8_t SetupCompareValue[10];

extern uint8_t SetupCompareItemValue1[20][1];
extern uint8_t SetupCompareItemValue2[20][1];
extern uint8_t SetupCompareItemValue3[20][1];
extern uint8_t SetupCompareItemValue4[20][1];

// 函式原型
void SCPI_ChromaSetComparItemFun(void);
char *upper(char *str);
void yyerror(int err_code);
void DecideComparePage(void);
void ParaValueToFram(uint8_t a, uint8_t b, uint8_t c);
void SCPI_SetComparItemMaxMin(uint8_t mode, uint8_t channel, uint8_t item, uint8_t val);
void GetCompareSetupMaxMainValue(uint8_t channel, uint8_t item);
void DcideCompareElemItem(void);

#endif /* __MAIN_H */