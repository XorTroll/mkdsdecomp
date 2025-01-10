#include <asmhack-base.h>

void Inject_Start() {
    // Original functionality
    g_SceneOverlayConfig->maybe_ovl_id = 0;

    //////////////////////////////////////////////////////////////

    // Debug log to emulator
    DebugPrint("Hello World from mkds asmhack!\n");
}
ncp_set_call(0x0205DCA8, Inject_Start);
