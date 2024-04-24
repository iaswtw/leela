#pragma once

// Each shader program is used in each of these stages.
// Renderers can choose which stage they want to draw.
typedef enum
{
    RenderStage_Pre,
    RenderStage_Main,
    RenderStage_Post,

    RenderStage_Translucent_Main,

    RenderStage_Final,

} RenderStage;
