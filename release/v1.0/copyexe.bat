@echo 创建目录
mkdir Debug
mkdir Release

@echo 小工具
copy ..\..\v1.0\module\filemanagers\test\viewcalcumatfile\nmake\x64\Debug\viewcalcumatfile.exe Debug\viewcalcumatfile.exe
copy ..\..\v1.0\module\filemanagers\test\viewacquisitionfile\nmake\x64\Release\viewacquisitionfile.exe Release\viewacquisitionfile.exe
copy ..\..\v1.0\module\lz_master_main\test\calcu_noslave\nmake\x64\Release\calcu_noslave.exe Release\calcu_noslave.exe

@echo 标定程序
copy ..\..\v1.0\module\acquisition\test\LzGrasp\nmake\x64\Release\LzGrasp.exe Release\LzGrasp.exe
copy ..\..\v1.0\module\lz_calibration\test\x64\Release\Lz_Calibration.exe Release\Lz_Calibration.exe

@echo 客户端程序
copy ..\..\v1.0\module\lz_client_main\test\gui\nmake\x64\Debug\lanzhouclient2013.exe Debug\lanzhouclient2013.exe

@echo 办公室服务器程序
copy ..\..\v1.0\module\lz_office_server_main\test\nmake\x64\Release\lzservertest.exe Release\lzservertest.exe

@echo 主控程序
copy ..\..\v1.0\module\lz_master_main\test\gui\nmake\x64\Release\lz_master_gui.exe Release\lz_master_gui.exe
copy ..\..\v1.0\module\lz_master_main\test\monitor\nmake\x64\Release\lz_master_monitor.exe Release\lz_master_monitor.exe

@echo 从控程序
copy ..\..\v1.0\module\lz_slave_main\test\nmake\x64\Release\lz_slave.exe Release\lz_slave.exe


Sub bat 
echo off & cls 
echo create_shortcut 
start wscript -e:vbs "%~f0" 
Exit Sub 
End Sub 

Set WshShell = WScript.CreateObject("WScript.Shell") 
strDesktop = WshShell.SpecialFolders("Desktop") 
set oShellLink = WshShell.CreateShortcut(strDesktop & "\客户端.lnk") 
oShellLink.TargetPath = "Debug\lanzhouclient2013.exe" 
oShellLink.WindowStyle = 3 
oShellLink.Hotkey = "Ctrl+Alt+e" 
oShellLink.IconLocation = "icon\cali_icon.ico, 0" 
oShellLink.Description = "快捷方式" 
oShellLink.WorkingDirectory = "" 
oShellLink.Save