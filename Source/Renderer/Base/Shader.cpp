#include "Shader.h"

#include "Util/Util.h"

#include <QDebug>
#include <QFile>

BSplineRenderer::Shader::Shader(const QString& Name)
    : pProgram(nullptr)
    , mName(Name)
{}

void BSplineRenderer::Shader::Initialize()
{
    LOG_INFO("Shader::Initialize: '{}' is being initializing.", mName.toStdString());

    initializeOpenGLFunctions();

    pProgram = QSharedPointer<QOpenGLShaderProgram>(new QOpenGLShaderProgram);

    for (const auto [ShaderType, Path] : mPaths)
    {
        const auto Bytes = Util::GetBytes(Path);
        if (!pProgram->addShaderFromSourceCode(ShaderType, Bytes.constData()))
        {
            BR_EXIT_FAILURE("Shader::Initialize: '{}' could not be loaded.", GetShaderTypeString(ShaderType).toStdString());
        }
    }

    if (!pProgram->link())
    {
        BR_EXIT_FAILURE("Shader::Initialize: Could not link shader program.");
    }

    if (!pProgram->bind())
    {
        BR_EXIT_FAILURE("Shader::Initialize: Could not bind shader program.");
    }

    LOG_INFO("Shader::Initialize: '{}' has been initialized.", mName.toStdString());
}

bool BSplineRenderer::Shader::Bind()
{
    return pProgram->bind();
}

void BSplineRenderer::Shader::Release()
{
    pProgram->release();
}

void BSplineRenderer::Shader::AddPath(QOpenGLShader::ShaderTypeBit Type, const QString& Path)
{
    mPaths.emplace(Type, Path);
}

QString BSplineRenderer::Shader::GetName() const
{
    return mName;
}

QString BSplineRenderer::Shader::GetShaderTypeString(QOpenGLShader::ShaderTypeBit Type)
{
    switch (Type)
    {
    case QOpenGLShader::Vertex:
        return "Vertex Shader";
    case QOpenGLShader::Fragment:
        return "Fragment Shader";
    case QOpenGLShader::Geometry:
        return "Geometry Shader";
    case QOpenGLShader::TessellationControl:
        return "Tessellation Control Shader";
    case QOpenGLShader::TessellationEvaluation:
        return "Tesselation Evaluation Shader";
    case QOpenGLShader::Compute:
        return "Compute Shader";
    default:
        return "Unknown Shader";
    }
}

void BSplineRenderer::Shader::SetSampler(const QString& Name, GLuint Unit, GLuint TextureId, GLuint Target)
{
    SetUniformValue(Name, Unit);
    glActiveTexture(GL_TEXTURE0 + Unit);
    glBindTexture(Target, TextureId);
}

void BSplineRenderer::Shader::SetUniformValueFloatArray(const QString& Name, const QVector<float>& Values)
{
    const auto location = pProgram->uniformLocation(Name);

    if (0 <= location)
    {
        pProgram->setUniformValueArray(location, Values.constData(), Values.size(), 1);
    }
    else
    {
        LOG_WARN("Shader::SetUniformValue: Uniform location '{}' could not be found.", Name.toStdString());
    }
}
