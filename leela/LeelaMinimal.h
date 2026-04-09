#pragma once

// Each shader program is used in each of these stages.
// Renderers can choose which stage they want to draw.
enum class RenderStage
{
    Pre,
    Main,
    Post,
    TranslucentMain,        // Objects with alpha < 1.0 have to be rendered after all objects with alpha = 1.0
                            // It still doesn't solve all problems.
    Final,
} ;

enum class ViewportType
{
    Primary,
    Minimap,
    AlternateObserver
};
