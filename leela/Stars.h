#pragma once

class StarsRenderer;

class Stars
{
    friend StarsRenderer;
public:
    Stars()
    {
    }

    ~Stars()
    {
    }

    void setCubeStarParameters(int maxCubeStars, float singlePixelStarsRatio)
    {
        this->maxCubeStars = maxCubeStars;
        this->singlePixelStarsRatio = singlePixelStarsRatio;
    }

private:
    int maxCubeStars;
    float singlePixelStarsRatio;        // radio of single pixel starts to the total number of stars
};