#include "Core/Controller.h"

#include <QApplication>

int main(int Argc, char* Argv[])
{
    QApplication Application(Argc, Argv);

    BSplineRenderer::Controller Controller;

    Controller.Run();

    return Application.exec();
}
