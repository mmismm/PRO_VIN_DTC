@echo off

:: �ж��Ƿ���copy_config�����Ѷ��壬�������ֶ����벿��
if defined copy_config goto define_original_file

:: ��δ����copy_config����ͨ���ֶ������ж�
echo **********��ʾ***********
echo ���롰1�� ����DebugĿ¼��dll�ļ�
echo ���롰2�� ����ReleaseĿ¼��dll�ļ�
echo ���롰3�� ����SO�ļ�
echo *************************
set /p Copycopy_config=��ѡ���������:
if %Copycopy_config%==1 (
	SET copy_config=Debug
)
if %Copycopy_config%==2 (
	SET copy_config=Release
)
if %Copycopy_config%==3 (
	SET copy_config=Android
)

:define_original_file
if %copy_config%==Android (
	set cmd_copy_original_file=.\libs\armeabi\*.so
)
if %copy_config%==Release (
	set cmd_copy_original_file=.\release\*.dll
)
if %copy_config%==Debug (
	set cmd_copy_original_file=.\debug\*.dll
)

::*******�����ļ�************************************************************************************************************
::*******************************************************************************************************************************
:: Ӧ�������·�������ⲻͬ�˿�������ͬĿ¼��ʱ�������⣻
set cmd_copy_direction_path=..\..\Windows��\Diagnostics\DiagnosticSysRes\BAIC_X35_VINDTC

:: ���������л�˫�����д��������ļ���·����VC���ɺ��¼����õ�·����һ������VC����ʱ��·������һ����
set VC_copy_original_file=..\%cmd_copy_original_file%
set VC_copy_direction_path=..\%cmd_copy_direction_path%

:: ͨ���ж�cmd_copy_direction_path�ļ��Ƿ���ڣ�ȷ����ֱ�����д��������ļ���VC���ɺ��¼�����
if exist "%cmd_copy_direction_path%" (
	:: ����cmd_copy_direction_path·����˵�������������У����ж�Դ�ļ��Ƿ����
	if exist "%cmd_copy_original_file%" (
		xcopy /e/s/y "%cmd_copy_original_file%" "%cmd_copy_direction_path%"
	) else (
		echo Դ�ļ�������: "%cmd_copy_original_file%"
		start /wait "��ʾ"  cmd /c "mode con  cols=50 lines=10 &echo.Դ�ļ�Ŀ¼�����ڣ���"%cmd_copy_original_file%"��,�����������...&pause>nul"
		pause
	)
) else (
	:: cmd_copy_direction_path�����ڣ�������VC���ã�����·����һ�£�Ҳ��������Ĳ�����
	if exist "%VC_copy_direction_path%" ( 
		xcopy /e/s/y "%VC_copy_original_file%" "%VC_copy_direction_path%"
	) else (
		:: �������ڣ�Ӧ�ֶ�������������ж�̬�⣬������������ļ���
		echo Ŀ���ļ��в�����: "%VC_copy_direction_path%"
		start /wait "��ʾ"  cmd /c "mode con  cols=50 lines=10 &echo.Ŀ���ļ�Ŀ¼�����ڣ���"%VC_copy_direction_path%"��,�����������...&pause>nul"
		pause
	)
)
::*******************************************************************************************************************************
::*******************************************************************************************************************************

::*******************************************************************************************************************************
pause




