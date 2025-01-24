#include <asmhack-base.h>
#include <asmhack-util.h>

void Inject_Start() {
    // Original functionality
    g_SceneOverlayConfig->maybe_ovl_id = 0;

    // Inject Scene_Test over Scene_Emblem (by overriding the scene function)
    *(void**)(0x02154918) = (void*)(0x21804E1);

    //////////////////////////////////////////////////////////////

    DebugPrint("Hello World from mkds asmhack! Test injected!\n");
    
    // Regular start
    g_SceneOverlayConfig->cur_scene_id = SceneId_Logo;
}
ncp_set_call(0x0205DCA8, Inject_Start);

void Inject_SceneUpdate(int bit) {
    // Original functionality
    LoopFn_Sound();

    //////////////////////////////////////////////////////////////

    DebugPrintf("Scene update (%s)\n", SceneNames[g_SceneOverlayConfig->cur_scene_id]);
}
ncp_set_call(0x02048D68, Inject_SceneUpdate);
