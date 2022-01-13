#ifndef NATIVEMESSAGING_H
#define NATIVEMESSAGING_H
#include "stdfx.h"

class IDM
{
public:
    /**
     * @brief Process IDMMsgHost and hold on stdin and stdout.
     * 
     * @param str the path of IDMMsgHost.
     * @return true Process success.
     * @return false Procss failed.
     */
    bool openProcss(std::string str)
    {
        securityAttribute.nLength = sizeof(securityAttribute);
        securityAttribute.lpSecurityDescriptor = nullptr;
        securityAttribute.bInheritHandle = true;
        if (!CreatePipe(&stdinRead, &stdinWrite, &securityAttribute, 0))
        {

            std::cerr << "CreatePipe failed" << std::endl;
            return false;
        }
        if (!CreatePipe(&stdoutRead, &stdoutWrite, &securityAttribute, 0))
        {

            std::cerr << "CreatePipe failed" << std::endl;
            return false;
        }
        startupInfo.cb = sizeof(startupInfo);
        startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        startupInfo.wShowWindow = SW_HIDE;
        startupInfo.hStdInput = stdinRead;
        startupInfo.hStdOutput = stdoutWrite;
        startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        if (!CreateProcess(str.c_str(), nullptr, nullptr, nullptr, true, 0, nullptr, nullptr, &startupInfo, &processInformation))
        {

            std::cerr << "CreateProcess failed" << std::endl;
            return false;
        }
        jobHandle = CreateJobObject(NULL, NULL);
        AssignProcessToJobObject(jobHandle, processInformation.hProcess);
        limitInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(jobHandle, JobObjectExtendedLimitInformation, &limitInfo, sizeof(limitInfo));
        return true;
    };
    /**
     * @brief Send Message to IDM.
     * 
     * @param str A Message.
     * @param stdin_write The handle of IDMMsgHost, that set in STARTUPINFO for CreateProcess.
     */
    void sendMsgToIDM(std::string str)
    {
        if (str.find("\"MSG#") != std::string::npos)
        {
            DWORD byte_write = 0;
            unsigned int len = str.length();
            std::string result;
            result += char(((len >> 0) & 0xFF));
            result += char(((len >> 8) & 0xFF));
            result += char(((len >> 16) & 0xFF));
            result += char(((len >> 24) & 0xFF));
            result += str;
            WriteFile(stdinWrite, result.data(), result.size(), &byte_write, nullptr);
        }
        else if (str.find("ef2://") != std::string::npos)
        {
            Code::ef2Protocol(str);
        }
        else if (str == "fin")
        {
            close();
            exit(0);
        }
    };
    /**
     * @brief Kill IDMMsgHost, and close handles.
     * 
     */
    void close()
    {
        ::CloseHandle(stdinWrite);
        ::TerminateProcess(processInformation.hProcess, 0);
        ::CloseHandle(processInformation.hThread);
        ::CloseHandle(processInformation.hProcess);
    };

private:
    SECURITY_ATTRIBUTES securityAttribute;
    HANDLE stdinRead, stdinWrite;
    HANDLE stdoutRead, stdoutWrite;
    STARTUPINFO startupInfo{};
    PROCESS_INFORMATION processInformation;
    HANDLE jobHandle;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limitInfo;
};

#endif // NATIVEMESSAGING_H