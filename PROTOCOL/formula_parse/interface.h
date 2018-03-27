#ifndef __FORMULA_PARSER_INTERFACE_H__
#define __FORMULA_PARSER_INTERFACE_H__



/*
***********************************************************************
** Function             : calculate
** Input                :
**		pcDataSource	: ����Դ��Ϊ����ָ��
**		nDataSourceLen	: ����Դ�ĳ���
**		pcFormula		: ��ʽ
**		nFormulaLen		: ��ʽ����
**		pStrFormat		: ��ʽ����ֵ���ַ������ʽ ���� "%d"
** Output               : �������
**		pOutStringData	: �����ַ������͵�����
** Return               :
** Description          :
**       --
	ASCII��HEX��DECIMAL��ʾ��ʽ�ǲ���Ҫ���㹫ʽ�ġ�

** Calls                :
**       -- ���õĺ���
***********************************************************************
*/
int calculate( const char* pcDataSource, const int nDataSourceLen,
               const char* pcFormula, const int nFormulaLen, char *pStrFormat, char* pOutStringData );




#endif