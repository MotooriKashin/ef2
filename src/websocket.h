#include "headsocket.h"
#include "nativemessaging.h"
#include <iostream>

static IDM idm;

namespace headsocket
{
    class client : public web_socket_client
    {
        HEADSOCKET_CLIENT(client, web_socket_client)

    public:
        bool async_received_data(const data_block &db, uint8_t *ptr, size_t length) override
        {
            if (db.op == opcode::text)
            {
                // Handle text message (null-terminated string is in 'ptr')
                // ...

                // Send text response back to client
                push("ok");
                idm.sendMsgToIDM((char *)ptr);
            }
            else
            {
                // Handle 'length' bytes of binary data in 'ptr'
                // ...

                // Send binary response back to client
                push(&length, sizeof(size_t));
            }

            // Consume this data block
            return true;
        };
    };
}
class Websocket
{
public:
    Websocket(int port)
    {
        if (!port || port > 65535)
            port = 9186;
        typedef headsocket::web_socket_server<headsocket::client> server_t;
        static auto server = server_t::create(port);

        char pvData[MAX_PATH] = {0};
        DWORD pcbData = sizeof(pvData);
        MSG Msg;
        LSTATUS ret = RegGetValue(HKEY_LOCAL_MACHINE,
                                  TEXT("SOFTWARE\\Google\\Chrome\\NativeMessagingHosts\\com.tonec.idm\\"),
                                  TEXT(""),
                                  RRF_RT_REG_SZ,
                                  NULL,
                                  (PVOID)&pvData,
                                  &pcbData);
        if (ret != ERROR_SUCCESS)
        {
            MessageBox(NULL, TEXT("未能读取到注册表中IDM的Native Messaging信息！"), NULL, MB_ICONWARNING);
            exit(-1);
        }
        pvData[strlen(pvData) - 15] = '\0';
        if (!idm.openProcss(strcat(pvData, "IDMMsgHost.exe")))
            exit(-1);
        /* 消息循环 */
        while (GetMessage(&Msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        idm.close();
        exit(Msg.wParam);
    };
};