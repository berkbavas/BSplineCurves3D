#include "Util.h"

#include "Util/Logger.h"

#include <QFile>

QByteArray BSplineRenderer::Util::GetBytes(const QString& Path)
{
    QFile File(Path);

    if (File.open(QFile::ReadOnly))
    {
        return File.readAll();
    }
    else
    {
        LOG_WARN("Util::GetBytes: '{}' could not be opened", Path.toStdString());
        return QByteArray();
    }
}