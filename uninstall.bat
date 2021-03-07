if exist 1.reg (del 1.reg)
echo Windows Registry Editor Version 5.00 > 1.reg
echo. >> 1.reg
echo [-HKEY_CLASSES_ROOT\ef2] >> 1.reg
echo. >> 1.reg
echo [-HKEY_CLASSES_ROOT\IDM_ef2] >> 1.reg
echo. >> 1.reg
echo [-HKEY_CLASSES_ROOT\.ef2] >> 1.reg
echo. >> 1.reg
start 1.reg