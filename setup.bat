if exist 1.reg (del 1.reg)
echo Windows Registry Editor Version 5.00 > 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\ef2] >> 1.reg
echo "URL Protocol"="" >> 1.reg
echo @="URL:IDM" >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\ef2\shell] >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\ef2\shell\open] >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\ef2\shell\open\command] >> 1.reg
set str=%~dp0
set fin=%str:\=\\%
echo @="\"%fin%ef2.exe\" --open-url -- \"%%1%\"" >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\IDM_ef2] >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\IDM_ef2\shell] >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\IDM_ef2\shell\open] >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\IDM_ef2\shell\open\command] >> 1.reg
echo @="\"%fin%ef2.exe\" \"%%1%\"" >> 1.reg
echo. >> 1.reg
echo [HKEY_CLASSES_ROOT\.ef2] >> 1.reg
echo @="IDM_ef2" >> 1.reg
echo. >> 1.reg
start 1.reg