#include "app.h"

#define _ATL_NO_COM_SUPPORT
#include <atlbase.h>

class AtlExeModule : public CAtlExeModuleT<AtlExeModule>
{
};

void App::Run()
{
    AtlExeModule{}.RunMessageLoop();
}