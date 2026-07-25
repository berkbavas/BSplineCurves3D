#pragma once

#include "Util/Logger.h"

#include <map>

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace BSplineRenderer
{
    class Shader : protected QOpenGLExtraFunctions
    {
      public:
        Shader(const QString& Name);

        void Initialize();
        bool Bind();
        void Release();

        void AddPath(QOpenGLShader::ShaderTypeBit Type, const QString& Path);

        QString GetName() const;

        static QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit Type);

        template<typename T>
        void SetUniformValue(const QString& Name, T Value)
        {
            const auto location = pProgram->uniformLocation(Name);

            if (0 <= location)
            {
                pProgram->setUniformValue(location, Value);
            }
            else
            {
                LOG_WARN("Shader::SetUniformValue: Uniform location '{}' could not be found.", Name.toStdString());
            }
        }

        template<typename T>
        void SetUniformValueArray(const QString& Name, const QVector<T>& Values)
        {
            const auto location = pProgram->uniformLocation(Name);

            if (0 <= location)
            {
                pProgram->setUniformValueArray(location, Values.constData(), Values.size());
            }
            else
            {
                LOG_WARN("Shader::SetUniformValue: Uniform location '{}' could not be found.", Name.toStdString());
            }
        }

        void SetUniformValueFloatArray(const QString& Name, const QVector<float>& Values);

        void SetSampler(const QString& Name, GLuint Unit, GLuint TextureId, GLuint Target = GL_TEXTURE_2D);

      private:
        QSharedPointer<QOpenGLShaderProgram> pProgram;
        std::map<QOpenGLShader::ShaderTypeBit, QString> mPaths;
        QString mName;
    };
}
