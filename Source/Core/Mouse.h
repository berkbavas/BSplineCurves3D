#pragma once

#include <QObject>

namespace BSplineRenderer
{
    struct Mouse
    {
        float X{ 0 };
        float Y{ 0 };
        float DX{ 0 };
        float DY{ 0 };

        Qt::MouseButton Button{ Qt::NoButton };

        void Reset()
        {
            X = 0;
            Y = 0;
            DX = 0;
            DY = 0;
            Button = Qt::NoButton;
        }
    };
}