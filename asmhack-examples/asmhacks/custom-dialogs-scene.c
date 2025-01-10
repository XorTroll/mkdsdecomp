#include <asmhack-base.h>
#include <asmhack-util.h>

const u16 *caption_str = u"Your free MKDS trial\nhas ended.\nPlease purchase\nthe full experience.";
const u16 *button1_str = u"Hell no";
const u16 *button2_str = u"Fuck off";

const u16 *progress1_str = u"Alright then.\nSince you wanted to\ncomplete the game...";
const u16 *progress2_str = u"You shall suffer\nthe wrath of\nmy minions then.";

const u16 *case2_str = u"Did you really\ntry to quit?\nYou fool...\nyou can't escape.";

u32 g_Counter = 0;
u32 g_Status = 0;

void CustomScene_Initialize(SceneExecutionContext *ctx) {
    DebugPrintf("Custom scene init\n");

    g_Counter = 0;

    ExecutionContext *ctx2 = GetActiveExecutionContext();
    Mem_HeapHead *heap = ExecutionContext_GetHeapHandle(ctx2);

    DisplayConfig *cfg = CreateDisplayConfig(heap, (void*)(0x21547c8));
    DisplayEngineConfig_SetBg1Config(&cfg->top_engine_cfg, 1, 0, 0, 0, 0, 1, 0);
    DisplayEngineConfig_SetBg1Config(&cfg->bottom_engine_cfg, 1, 0, 0, 0, 0, 2, 0);
    SetGlobalDisplayConfig(cfg);

    StructJT jt;
    Mem_CpuFill8(&jt, 0, sizeof(jt));
    jt.load_dialog = 1;
    jt.dialog_font = &g_NftrMarioFont;
    jt.dialog_flag_1 = 0;
    jt.dialog_flag_2 = 1;

    VerifySomeTextSystemFiles_from_arm();
    InitializeGlobalQ_from_arm(heap, &jt);

    DebugPrintf("Custom scene init end\n");
}

void InjectAtMenu2();
void InjectAtTitle();

void CustomScene_Update(SceneExecutionContext *ctx, u32 frame_count) {
    DebugPrintf("Custom scene update status %d counter %d\n", g_Status, g_Counter);

    if(g_Status == 0) {
        g_Counter++;

        if(g_Counter > 40) {
            ShowYesNoDialog_from_arm(caption_str, button1_str, button2_str, 0);
            g_Status = 1;
            g_Counter = 0;
        }
    }
    else if(g_Status == 1) {
        if(sub_2125F54_from_arm()) {
            sub_2125FC0_from_arm();
        }
        else if(DialogMaybeAnyButtonPressed_from_arm()) {
            if(DialogContext_Button2Pressed_from_arm()) {
                ShowProgressDialogImpl_from_arm(progress2_str, 1, 1, 1, 0, 1);
                g_Status = 2;
            }
            else {
                ShowProgressDialogImpl_from_arm(progress1_str, 1, 1, 1, 0, 1);
                g_Status = 3;
            }
        }
    }
    else if(g_Status == 2) {
        g_Counter++;
        if(g_Counter > 200) {
            g_Counter = 0;
            g_Status = 4;
        }
    }
    else if(g_Status == 3) {
        g_Counter++;
        if(g_Counter > 200) {
            g_Counter = 0;
            g_Status = 5;
        }
    }
    else if(g_Status == 4) {
        if(ctx != NULL) {
            // setup race
            InjectAtTitle();

            g_GlobalCO->next_race.cc_type = CcType_150cc_Mirror;
            g_GlobalCO->next_race.internal_course_id = InternalCourseId_Award;

            g_GlobalCO->next_race.racer_entries[0].character_id = CharacterId_Waluigi;
            g_GlobalCO->next_race.racer_entries[0].kart_id = KartId_GoldMantis;
            g_GlobalCO->next_race.racer_entries[0].type = RacerType_Player;

            for(u32 i = 1; i < 8; i++) {
                g_GlobalCO->next_race.racer_entries[i].character_id = CharacterId_ShyGuy;
                g_GlobalCO->next_race.racer_entries[i].kart_id = KartId_4WheelCradle;
            }

            g_Status = 999;
            g_Counter = 0;
            ctx->next_scene_id = SceneId_Race;
        }
    }
    else if(g_Status == 5) {
        if(ctx != NULL) {
            g_GlobalCO->next_race.display_mode = DisplayMode_StaffRoll;
            g_GlobalCO->next_race.internal_course_id = InternalCourseId_StaffRollTrue;

            g_GlobalCO->next_race.racer_entries[0].character_id = CharacterId_DryBones;
            g_GlobalCO->next_race.racer_entries[0].kart_id = KartId_DryBomber;
            g_GlobalCO->next_race.racer_entries[0].type = RacerType_Player;

            g_Status = 0;
            g_Counter = 0;
            ctx->next_scene_id = SceneId_Race;
        }
    }
    else if(g_Status == 6) {
        ShowProgressDialogImpl_from_arm(case2_str, 1, 1, 1, 0, 1);
        g_Counter = 0;
        g_Status = 7;
    }
    else if(g_Status == 7) {
        g_Counter++;

        if(g_Counter > 150) {
            g_Counter = 0;
            g_Status = 4;
        }
    }

    sub_2125928_from_arm();
}

void CustomScene_Vblank(SceneExecutionContext *ctx, u32 frame_count) {
    DebugPrintf("Custom scene vblank\n");

    sub_21258F0_from_arm();
}

void CustomScene_Finalize(SceneExecutionContext *ctx) {
    DebugPrintf("Custom scene finalize\n");

    if(g_Status == 999) {
        g_Status = 6;
    }

    ResetGlobalQ_from_arm();
}

static const SceneInfo g_CustomSceneInfo = {
    .init_fn = CustomScene_Initialize,
    .update_fn = CustomScene_Update,
    .fini_fn = CustomScene_Finalize,
    .vblank_fn = CustomScene_Vblank,
    .pre_sleep_fn = NULL,
    .post_sleep_fn = NULL,
    .fade_in_frame_count = 30,
    .fade_out_frame_count = 30,
    .unk = 0
};

int SceneFn_Custom(SceneExecutionContext *ctx) {
    DebugPrintf("Got to scene custom!!!\n");
    const SceneInfo info = g_CustomSceneInfo;
    OnSceneLoop(ctx, &info);
    return 0;
}

void InjectAtMenu2() {
    *(void**)(0x0215490C) = (void*)SceneFn_Title;
    *(void**)(0x0215491C) = (void*)SceneFn_Custom;
}

void InjectAtTitle() {
    *(void**)(0x0215490C) = (void*)SceneFn_Custom;
    *(void**)(0x0215491C) = (void*)SceneFn_Menu2;
}

/////////////////////////////////////////////////////////////////////////////////////

void Inject_Start() {
    // Original functionality
    g_SceneOverlayConfig->maybe_ovl_id = 0;

    //////////////////////////////////////////////////////////////

    // Debug log to emulator
    DebugPrint("Hello World from mkds asmhack!\n");
    
    InjectAtMenu2();
    g_SceneOverlayConfig->cur_scene_id = SceneId_Menu2;
}
ncp_set_call(0x0205DCA8, Inject_Start);

void Inject_SceneUpdate(int bit) {
    // Original functionality
    LoopFn_Sound();

    //////////////////////////////////////////////////////////////

    DebugPrintf("Scene update (%s)!\n", SceneNames[g_SceneOverlayConfig->cur_scene_id]);
}
ncp_set_call(0x02048D68, Inject_SceneUpdate);
