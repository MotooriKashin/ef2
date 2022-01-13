#define _WIN32_WINNT 0x0600
#define HEADSOCKET_IMPLEMENTATION
#include "src\headsocket.h"
#include "src\code.h"
#include "src\privilege.h"
#include "src\ui.h"
#include "src\websocket.h"

#pragma comment(linker, "/SUBSYSTEM:windows \"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma execution_character_set("utf-8")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // 命令行调用
  for (int i = 0; i < __argc; i++)
  {
    if (strstr(__argv[i], "ef2://"))
    {
      if (strstr(__argv[i], "ef2://websocket"))
      {
        Websocket websocker(atol(__argv[i] + 16));
      }
      else
        Code::ef2Protocol(__argv[i]);
      return 0;
    }
    else if (strstr(__argv[i], ".ef2"))
    {
      Code::ef2File(__argv[i]);
      return 0;
    }
  }

  // UAC权限提升
  if (!Privilege::QueryPrivilege())
    return Privilege::UpgradePrivilege();

  UI ui(hInstance, nCmdShow);
  return 0;
}