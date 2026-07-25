#pragma once

#include "Core/CurveContainer.h"
#include "Curve/Spline.h"

#include <cmath>
#include <map>

#include <QMatrix4x4>
#include <QVector3D>

namespace BSplineRenderer
{
    enum class AnimationType
    {
        None,
        Rotate,
        Pulse,
        Wave,
        Bounce,
        Spiral
    };

    class AnimationManager
    {
      public:
        static AnimationManager& Instance();

        void Update(float DeltaTime, CurveContainer* pContainer);

        void ApplyAnimation(SplinePtr pSpline);

        void SetEnabled(bool Enabled) { mEnabled = Enabled; }
        bool IsEnabled() const { return mEnabled; }

        void SetAnimationType(AnimationType AnimationType) { mAnimationType = AnimationType; }
        AnimationType GetAnimationType() const { return mAnimationType; }

        void SetSpeed(float Speed) { mSpeed = Speed; }
        float GetSpeed() const { return mSpeed; }

        void SetAmplitude(float Amplitude) { mAmplitude = Amplitude; }
        float GetAmplitude() const { return mAmplitude; }

        void Reset();
        void SaveOriginalPositions(CurveContainer* pContainer);
        const char* GetAnimationTypeName(AnimationType AnimationType) const;

      private:
        AnimationManager() = default;

        void ApplyRotation(SplinePtr pSpline);
        void ApplyPulse(SplinePtr pSpline);
        void ApplyWave(SplinePtr pSpline);
        void ApplyBounce(SplinePtr pSpline);
        void ApplySpiral(SplinePtr pSpline);

        bool mEnabled{ false };
        AnimationType mAnimationType{ AnimationType::None };
        float mSpeed{ 1.0f };
        float mAmplitude{ 1.0f };
        float mTime{ 0.0f };

        std::map<SplinePtr, QVector<QVector3D>> mOriginalPositions;
    };
}
