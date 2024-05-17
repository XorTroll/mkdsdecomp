#include <demo.h>

// Demo: start the game as a spectator race of shy guys in dry bombers
// This call is inserted right before the main scene loop, replacing a instruction related to setting the initial scene id in GlobalR

void InitialInject() {
    // Debug log to emulator
    DebugPrint("Hello!\n");

    g_GlobalR->cur_scene_id = SceneId_Race;

    g_GlobalCO->maybe_default_internal_course_id = InternalCourseId_pinball_course;

    for(int i = 0; i < 8; i++) {
        g_GlobalCO->racers2[i].character_id = CharacterId_ShyGuy;
        g_GlobalCO->racers2[i].kart_id = KartId_DryBomber;
        g_GlobalCO->racers[i].character_id = CharacterId_ShyGuy;
        g_GlobalCO->racers[i].kart_id = KartId_DryBomber;
        g_GlobalCO->racers[i].racer_type = RacerType_CPU;
    }

    DebugPrint("Goodbye!\n");
}
ncp_set_call(0x0205DCA0, InitialInject)
