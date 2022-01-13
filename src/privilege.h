#ifndef PRIVILEGE_H
#define PRIVILEGE_H
#include "stdfx.h"

class Privilege
{
public:
    static BOOL QueryPrivilege()
    {
        BOOL b;
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID AdministratorsGroup;
        b = AllocateAndInitializeSid(
            &NtAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &AdministratorsGroup);
        if (b)
        {
            if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
            {
                b = FALSE;
            }
            FreeSid(AdministratorsGroup);
        }
        return b;
    };
    static BOOL UpgradePrivilege()
    {
        TCHAR Path[MAX_PATH];
        SHELLEXECUTEINFO ShExecInfo = {0};

        ZeroMemory(Path, MAX_PATH);
        GetModuleFileName(NULL, Path, MAX_PATH);
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.lpVerb = TEXT("runas");
        ShExecInfo.lpFile = Path;
        ShExecInfo.nShow = SW_SHOWDEFAULT;
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        exit(1);
    }
};

#endif // PRIVILEGE_H