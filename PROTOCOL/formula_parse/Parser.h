#ifndef __FORMULA_PARSER_PARSER_H__
#define __FORMULA_PARSER_PARSER_H__

#include "public.h"

#define MAX_SIZE 512

enum BOOL
{
    FALSE,
    TRUE
};

typedef struct tagOPERATE
{
	double Operand;//������
	char Operator;//������
} OPERATE, * P_OPERATE;

int IsDigit( char ch, int bHex );//�ж��Ƿ�Ϊ����

//����ʽ�е�A-Z�û�Ϊ��Ч����
void PutDataToFormula( const char* pcDataSource, const int nDataSourceLen, const char* pSrcFormula, int nFormulaLen, char* pDestFormula );

//����ʽ�еĸ���
void HandleMinus( char* pcSrc );

//�õ�����ĸ�滻��ѧ���ʽ
void FormulaProc( char* pSrc );

//��������������ȼ���ʽ����ʽ
void FormatFormula( char* pSrc, P_OPERATE pOperate );

//��ò�����
int GetOperand( char* pSrc, int * nLen, double *pOperand );

//
int Locate( char ch );

//��ò�������ջ�����ȼ�
int GetPushStackPriority( char ch );

//��ò�������ջ�����ȼ�
int GetPopStackPriority( char ch );

//������ֵ
double CalculateValue( P_OPERATE pOperator, double x );

//��ֹ�ʽ��Switch
void ParseFormulaAndSwitch( const char* pstrFormula, int nLen, char* pOutStrFormula, char* pOutSwitch );

//SwitchFunction
void SwitchFunction( int nValue, char* pSwitchFunction, int nLen, char* pOutValue );


/************************************************************************/
/* ���Format������
0��ASCII  ����Ҫ����
1��HEX ʮ��������ʾ	����Ҫ����
2��DECIMAL ʮ������ʾ	����Ҫ����
3��INTEGER ������ʽ����ʾ	��Ҫ����
4��DOUBLE ���������ݸ�ʽ����ʾ	��Ҫ����
5��STRING ������ֵ�ҵ���Ӧ���ַ���ID                                                                     */
/************************************************************************/
int GetFormatType( char *pStrFormat );

#endif