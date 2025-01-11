#include <asmhack-base.h>
#include <asmhack-util.h>

const u16 *caption_str = u"Your free MKDS trial\nhas ended.\nPlease purchase\nthe full experience.";
const u16 *button1_str = u"Hell no";
const u16 *button2_str = u"Fuck off";

const u16 *progress1_str = u"Alright then.\nSince you wanted to\ncomplete the game...";
const u16 *progress2_str = u"You shall suffer\nthe wrath of\nmy minions then.";

const u16 *case2_str = u"Did you really\ntry to quit?\nYou fool...\nyou can't escape.";

u32 g_FrameCounter = 0;
u32 g_Status = 0;
u32 g_PressFrameCounter = 0;
int g_LastPress = -1;

void CustomScene_Initialize(SceneExecutionContext *ctx) {
    DebugPrintf("Custom scene init\n");

    g_FrameCounter = 0;

    ExecutionContext *ctx2 = GetActiveExecutionContext();
    Mem_HeapHead *heap = ExecutionContext_GetHeapHandle(ctx2);

    DisplayConfig *cfg = CreateDisplayConfig(heap, (void*)(0x21547c8)); // Use same init parameters as the Logo scene display config
    DisplayEngineConfig_SetBg1Config(&cfg->top_engine_cfg, 1, 0, 0, 0, 0, 1, 0);
    DisplayEngineConfig_SetBg1Config(&cfg->bottom_engine_cfg, 1, 0, 0, 0, 0, 2, 0);
    SetGlobalDisplayConfig(cfg);

    NavigationContextParams params;
    Mem_CpuFill8(&params, 0, sizeof(params));
    params.load_text_system = 1;
    params.text_system_font = &g_NftrMarioFont;
    params.text_system_load_bg_1_flag = 0; // All uses of the text system set these flags...
    params.text_system_load_bg_2_flag = 1;
    params.text_system_load_bg_3_flag = 0;

    PrepareNavigationContextFiles();
    InitializeNavigationContext(heap, &params);

    DebugPrintf("Custom scene init end\n");
}

void InjectAtMenu2();
void InjectAtTitle();

void CustomScene_Update(SceneExecutionContext *ctx, u32 frame_count) {
    DebugPrintf("Custom scene update status %d counter %d\n", g_Status, g_FrameCounter);

    if(g_Status == 0) {
        g_FrameCounter++;

        if(g_FrameCounter > 40) {
            int blend_flag_top = true; // Flags for color blending in top/sub screens respectively (won't be really noticed unless the back screen is enabled in the nav context params)
            int blend_flag_sub = true;
            int start_button_2_focused = true; // Self-explanatory
            int fade_in_start_y = TextSystem_FadeY_80; // Controls the fade-in direction
            int fade_out_end_y = TextSystem_FadeY_160; // Controls the fade-out direction
            TextSystem_SetFadeInSseqId(110); // Just throw a boulder to the user
            TextSystem_DisplayDoubleButtonDialog(caption_str, button1_str, button2_str, start_button_2_focused, blend_flag_top, blend_flag_sub, fade_in_start_y, fade_out_end_y);

            g_Status = 1;
            g_FrameCounter = 0;
        }
    }
    else if(g_Status == 1) {
        if(TextSystem_WasAnyDialogButtonPressed()) {
            // User pressed a button in the dialog
            // Take advantage of this system, only acknowledge the input (and finish the dialog) after 240 frames (~4 seconds) of holding the SAME button

            int cur_press = TextSystem_IsButton2Focused();
            if(g_LastPress == -1) {
                g_LastPress = cur_press;
            }
            else if(g_LastPress != cur_press) {
                g_LastPress = -1;
                g_PressFrameCounter = 0;
            }

            g_PressFrameCounter++;
            if(g_PressFrameCounter >= 240) {
                g_LastPress = -1;
                g_PressFrameCounter = 0;
                
                // Fine, close the dialog
                TextSystem_CloseDialog();
            }
        }
        else if(TextSystem_IsDialogFinished()) {
            // The dialog already faded out and closed
            // Check the final button press (this is internally kept as the button pressed by the user before we finished the dialog)
            if(TextSystem_IsButton2Focused()) {
                // Progress dialog on the sub screen
                int blend_flag_top = true; // Same as above
                int blend_flag_sub = true;
                int fade_in_start_y = TextSystem_FadeY_80;
                int fade_out_end_y = TextSystem_FadeY_160;
                int enable_progress_anim = true; // Whether to show the rolling-wheel progress sprite/animation
                TextSystem_DisplayProgressDialog(progress2_str, blend_flag_top, blend_flag_sub, fade_in_start_y, fade_out_end_y, enable_progress_anim);
                g_Status = 2;
            }
            else {
                // Progress dialog on the top screen
                int blend_flag_top = true; // Same as above
                int blend_flag_sub = true;
                int fade_in_start_y = TextSystem_FadeY_80;
                int fade_out_end_y = TextSystem_FadeY_160;
                int enable_top_progress_anim = true; // Now we could show the wheel animation on BOTH screens at the same time (nice, MKDS devs)
                int enable_sub_progress_anim = true;
                TextSystem_DisplayProgressDialog_TopScreen(progress1_str, blend_flag_top, blend_flag_sub, fade_in_start_y, fade_out_end_y, enable_top_progress_anim, enable_sub_progress_anim);
                g_Status = 3;
            }
        }
    }
    else if(g_Status == 2) {
        g_FrameCounter++;
        if(g_FrameCounter > 200) {
            g_FrameCounter = 0;
            g_Status = 4;
        }
    }
    else if(g_Status == 3) {
        g_FrameCounter++;
        if(g_FrameCounter > 200) {
            g_FrameCounter = 0;
            g_Status = 5;
        }
    }
    else if(g_Status == 4) {
        if(ctx != NULL) {
            // setup race
            InjectAtTitle();

            g_GlobalCO->next_race.cc_type = CcType_150cc_Mirror;
            g_GlobalCO->next_race.internal_course_id = InternalCourseId_rainbow_course;

            g_GlobalCO->next_race.racer_entries[0].character_id = CharacterId_Waluigi;
            g_GlobalCO->next_race.racer_entries[0].kart_id = KartId_GoldMantis;
            g_GlobalCO->next_race.racer_entries[0].type = RacerType_Player;

            for(u32 i = 1; i < 8; i++) {
                g_GlobalCO->next_race.racer_entries[i].character_id = CharacterId_ShyGuy;
                g_GlobalCO->next_race.racer_entries[i].kart_id = KartId_4WheelCradle;
            }

            g_Status = 999;
            g_FrameCounter = 0;
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
            g_FrameCounter = 0;
            ctx->next_scene_id = SceneId_Race;
        }
    }
    else if(g_Status == 6) {
        // I'm not sure if these fade in/out values are intended, or are a non-reachable limit (they make the dialog violently shake :P)
        TextSystem_DisplayProgressDialog_TopScreen(case2_str, true, true, TextSystem_FadeY_255, TextSystem_FadeY_255, true, false);
        g_FrameCounter = 0;
        g_Status = 7;
    }
    else if(g_Status == 7) {
        g_FrameCounter++;

        if(g_FrameCounter > 150) {
            g_FrameCounter = 0;
            g_Status = 4;
        }
    }

    NavigationContext_OnSceneUpdate();
}

void CustomScene_Vblank(SceneExecutionContext *ctx, u32 frame_count) {
    DebugPrintf("Custom scene vblank\n");

    NavigationContext_OnSceneVblank();
}

void CustomScene_Finalize(SceneExecutionContext *ctx) {
    DebugPrintf("Custom scene finalize\n");

    if(g_Status == 999) {
        g_Status = 6;
    }

    DisposeNavigationContext();
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
