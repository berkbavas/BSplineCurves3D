#include "ImGuiWindow.h"

#include "Core/AnimationManager.h"
#include "Core/CurveSerializer.h"
#include "Core/PresetShapes.h"
#include "Core/UndoRedoManager.h"
#include "Renderer/RendererManager.h"
#include "Util/Logger.h"

#include <imgui.h>

#include <QFileDialog>
#include <QtImGui.h>

BSplineRenderer::ImGuiWindow::ImGuiWindow(QObject* pParent)
    : QObject(pParent)
{}

void BSplineRenderer::ImGuiWindow::Draw()
{
    mNumberOfSegments = mRendererManager->GetNumberOfSegments();
    mNumberOfSectors = mRendererManager->GetNumberOfSectors();

    // Main control panel
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_MenuBar);

    // Menü çubuğu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Curves", "Ctrl+S"))
            {
                CurveSerializer::SaveToFile(QString(mFilePath), mCurveContainer);
            }
            if (ImGui::MenuItem("Load Curves", "Ctrl+O"))
            {
                CurveSerializer::LoadFromFile(QString(mFilePath), mCurveContainer);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            auto& UndoManager = UndoRedoManager::Instance();
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, UndoManager.CanUndo()))
            {
                UndoManager.Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, UndoManager.CanRedo()))
            {
                UndoManager.Redo();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Statistics", nullptr, &mShowStatistics);
            ImGui::MenuItem("Help", "F1", &mShowHelp);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    DrawRenderSettings();
    DrawCurvePanel();
    DrawPresetShapesPanel();
    DrawAnimationPanel();
    DrawFileOperationsPanel();
    DrawThemePanel();
    DrawCameraPanel();

    ImGui::Separator();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    // Optional panels
    if (mShowStatistics)
    {
        DrawStatisticsPanel();
    }

    if (mShowHelp)
    {
        DrawHelpPanel();
    }
}

void BSplineRenderer::ImGuiWindow::DrawRenderSettings()
{
    if (ImGui::CollapsingHeader("Render Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::SliderInt("Number of Segments", &mNumberOfSegments, 1, 64))
        {
            mRendererManager->SetNumberOfSegments(mNumberOfSegments);
        }

        if (ImGui::SliderInt("Number of Sectors", &mNumberOfSectors, 1, 64))
        {
            mRendererManager->SetNumberOfSectors(mNumberOfSectors);
        }

        ImGui::Checkbox("Wireframe", mRendererManager->GetWireframe());

        ImGui::Separator();
        ImGui::Text("Glow Effects:");

        auto& Glow = mRendererManager->GetGlowSettings();
        ImGui::SliderFloat("Glow Intensity", &Glow.Intensity, 0.0f, 3.0f, "%.2f");
        ImGui::SliderFloat("Pulse Speed", &Glow.Pulse, 0.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("Shimmer", &Glow.Shimmer, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Core Boost", &Glow.CoreBoost, 0.0f, 2.0f, "%.2f");

        if (ImGui::Button("Reset Glow"))
        {
            Glow.Intensity = 1.0f;
            Glow.Pulse = 0.0f;
            Glow.Shimmer = 0.0f;
            Glow.CoreBoost = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Neon Preset"))
        {
            Glow.Intensity = 2.0f;
            Glow.Pulse = 1.5f;
            Glow.Shimmer = 0.6f;
            Glow.CoreBoost = 1.2f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Subtle Preset"))
        {
            Glow.Intensity = 1.2f;
            Glow.Pulse = 0.5f;
            Glow.Shimmer = 0.2f;
            Glow.CoreBoost = 0.3f;
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawCurvePanel()
{
    ImGui::BeginDisabled(!mSelectedCurve);
    if (ImGui::CollapsingHeader("Curve", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (mSelectedCurve)
        {
            ImGui::Text("Pointer: 0x%p", mSelectedCurve.get());
            ImGui::Text("# of Knots: %zu", mSelectedCurve->GetKnots().size());

            ImGui::Separator();
            ImGui::Text("Geometry:");
            ImGui::SliderFloat("Radius", &mSelectedCurve->GetRadius_NonConst(), 0.10f, 0.50f);

            ImGui::Separator();
            ImGui::Text("Material:");
            ImGui::SliderFloat("Ambient", &mSelectedCurve->GetAmbient_NonConst(), 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse", &mSelectedCurve->GetDiffuse_NonConst(), 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &mSelectedCurve->GetSpecular_NonConst(), 0.0f, 1.0f);
            ImGui::SliderFloat("Shininess", &mSelectedCurve->GetShininess_NonConst(), 1.0f, 128.0f);
            ImGui::ColorEdit3("Color", &mSelectedCurve->GetColor_NonConst()[0]);

            ImGui::Separator();
            if (mSelectedKnot)
            {
                ImGui::Text("Selected Knot: (%.2f, %.2f, %.2f)", mSelectedKnot->GetPosition().x(), mSelectedKnot->GetPosition().y(), mSelectedKnot->GetPosition().z());
            }

            ImGui::Separator();
            if (ImGui::Button("Remove Last Knot") && mSelectedCurve->GetKnotCount() > 0)
            {
                mSelectedCurve->RemoveLastKnot();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All Knots"))
            {
                mSelectedCurve->ClearKnots();
            }
        }
    }
    ImGui::EndDisabled();
}

void BSplineRenderer::ImGuiWindow::DrawPresetShapesPanel()
{
    if (ImGui::CollapsingHeader("Preset Shapes"))
    {
        ImGui::Text("Shape Parameters:");
        ImGui::SliderFloat("Radius##preset", &mPresetRadius, 1.0f, 20.0f);
        ImGui::SliderFloat("Height##preset", &mPresetHeight, 1.0f, 30.0f);
        ImGui::SliderInt("Points##preset", &mPresetPoints, 6, 128);
        ImGui::SliderInt("Turns##preset", &mPresetTurns, 1, 10);

        ImGui::Separator();
        ImGui::Text("2D Shapes:");

        if (ImGui::Button("Circle"))
        {
            auto pSpline = PresetShapes::CreateCircle(mPresetRadius, mPresetPoints);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Star"))
        {
            auto pSpline = PresetShapes::CreateStar(5, mPresetRadius, mPresetRadius * 0.5f);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Heart"))
        {
            auto pSpline = PresetShapes::CreateHeart(mPresetRadius * 0.6f, mPresetPoints);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }

        if (ImGui::Button("Wave"))
        {
            auto pSpline = PresetShapes::CreateWave(mPresetRadius * 0.4f, 2.0f, mPresetRadius * 4.0f, mPresetPoints);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Lissajous"))
        {
            auto pSpline = PresetShapes::CreateLissajous(3.0f, 2.0f, mPresetRadius, mPresetRadius);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Butterfly"))
        {
            auto pSpline = PresetShapes::CreateButterfly(mPresetRadius * 0.4f, 128);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }

        ImGui::Separator();
        ImGui::Text("3D Shapes:");

        if (ImGui::Button("Spiral"))
        {
            auto pSpline = PresetShapes::CreateSpiral(mPresetRadius, mPresetHeight, mPresetTurns);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Helix"))
        {
            auto pSpline = PresetShapes::CreateHelix(mPresetRadius, 2.0f, mPresetTurns);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Trefoil Knot"))
        {
            auto pSpline = PresetShapes::CreateTrefoilKnot(mPresetRadius * 0.6f, 96);
            mCurveContainer->AddCurve(pSpline);
            emit CurveAdded(pSpline);
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawAnimationPanel()
{
    if (ImGui::CollapsingHeader("Animation"))
    {
        auto& AnimationManager = AnimationManager::Instance();

        bool Enabled = AnimationManager.IsEnabled();
        if (ImGui::Checkbox("Enable Animation", &Enabled))
        {
            AnimationManager.SetEnabled(Enabled);
            if (Enabled)
            {
                AnimationManager.SaveOriginalPositions(mCurveContainer);
            }
        }

        const char* AnimationTypes[] = { "None", "Rotate", "Pulse", "Wave", "Bounce", "Spiral" };
        if (ImGui::Combo("Animation Type", &mSelectedAnimationType, AnimationTypes, IM_ARRAYSIZE(AnimationTypes)))
        {
            AnimationManager.SetAnimationType(static_cast<AnimationType>(mSelectedAnimationType));
            AnimationManager.SaveOriginalPositions(mCurveContainer);
        }

        if (ImGui::SliderFloat("Speed", &mAnimationSpeed, 0.1f, 5.0f))
        {
            AnimationManager.SetSpeed(mAnimationSpeed);
        }

        if (ImGui::SliderFloat("Amplitude", &mAnimationAmplitude, 0.1f, 5.0f))
        {
            AnimationManager.SetAmplitude(mAnimationAmplitude);
        }

        if (ImGui::Button("Reset Animation"))
        {
            AnimationManager.Reset();
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawStatisticsPanel()
{
    ImGui::Begin("Statistics", &mShowStatistics);

    const auto TotalCurves = mCurveContainer->GetCurves().size();
    auto TotalKnots = 0;
    auto TotalLength = 0.0f;

    for (const auto& pCurve : mCurveContainer->GetCurves())
    {
        TotalKnots += pCurve->GetKnotCount();
        TotalLength += pCurve->GetTotalLength();
    }

    ImGui::Text("Total Curves: %d", TotalCurves);
    ImGui::Text("Total Knots: %d", TotalKnots);
    ImGui::Text("Total Length: %.2f units", TotalLength);

    ImGui::Separator();

    if (mSelectedCurve)
    {
        ImGui::Text("Selected Curve Statistics:");
        ImGui::Text("  Knot Count: %d", mSelectedCurve->GetKnotCount());
        ImGui::Text("  Length: %.2f units", mSelectedCurve->GetTotalLength());

        QVector3D Centroid = mSelectedCurve->GetCentroid();
        ImGui::Text("  Centroid: (%.2f, %.2f, %.2f)", Centroid.x(), Centroid.y(), Centroid.z());

        const auto& [MinPos, MaxPos] = mSelectedCurve->GetBoundingBox();
        ImGui::Text("  Bounding Box:");
        ImGui::Text("    Min: (%.2f, %.2f, %.2f)", MinPos.x(), MinPos.y(), MinPos.z());
        ImGui::Text("    Max: (%.2f, %.2f, %.2f)", MaxPos.x(), MaxPos.y(), MaxPos.z());
    }

    ImGui::Separator();
    auto& UndoManager = UndoRedoManager::Instance();
    ImGui::Text("Undo Stack: %d", UndoManager.UndoCount());
    ImGui::Text("Redo Stack: %d", UndoManager.RedoCount());

    ImGui::End();
}

void BSplineRenderer::ImGuiWindow::DrawFileOperationsPanel()
{
    if (ImGui::CollapsingHeader("File Operations"))
    {
        ImGui::InputText("File Path", mFilePath, sizeof(mFilePath));

        if (ImGui::Button("Save Curves"))
        {
            if (CurveSerializer::SaveToFile(QString(mFilePath), mCurveContainer))
            {
                LOG_INFO("Curves saved to: {}", mFilePath);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Curves"))
        {
            if (CurveSerializer::LoadFromFile(QString(mFilePath), mCurveContainer))
            {
                LOG_INFO("Curves loaded from: {}", mFilePath);
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Clear All Curves"))
        {
            while (!mCurveContainer->GetCurves().isEmpty())
            {
                mCurveContainer->RemoveCurve(mCurveContainer->GetCurves().first());
            }
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawThemePanel()
{
    if (ImGui::CollapsingHeader("Theme"))
    {
        if (ImGui::Button("Dark"))
        {
            ApplyTheme(ThemeStyle::Dark);
        }
        ImGui::SameLine();
        if (ImGui::Button("Light"))
        {
            ApplyTheme(ThemeStyle::Light);
        }
        ImGui::SameLine();
        if (ImGui::Button("Classic"))
        {
            ApplyTheme(ThemeStyle::Classic);
        }
        ImGui::SameLine();
        if (ImGui::Button("Blue"))
        {
            ApplyTheme(ThemeStyle::Blue);
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawCameraPanel()
{
    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Camera Presets:");
        if (ImGui::Button("Front"))
        {
            emit RequestCameraPreset(0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Back"))
        {
            emit RequestCameraPreset(1);
        }
        ImGui::SameLine();
        if (ImGui::Button("Left"))
        {
            emit RequestCameraPreset(2);
        }
        ImGui::SameLine();
        if (ImGui::Button("Right"))
        {
            emit RequestCameraPreset(3);
        }

        if (ImGui::Button("Top"))
        {
            emit RequestCameraPreset(4);
        }
        ImGui::SameLine();
        if (ImGui::Button("Bottom"))
        {
            emit RequestCameraPreset(5);
        }
        ImGui::SameLine();
        if (ImGui::Button("Isometric"))
        {
            emit RequestCameraPreset(6);
        }

        ImGui::Separator();
        if (ImGui::Button("Reset Camera"))
        {
            emit RequestCameraReset();
        }
    }
}

void BSplineRenderer::ImGuiWindow::DrawHelpPanel()
{
    ImGui::Begin("Help", &mShowHelp);

    ImGui::Text("B-Spline Renderer - Controls Guide");
    ImGui::Separator();

    ImGui::Text("Mouse Controls:");
    ImGui::BulletText("Left Click: Select curve/knot");
    ImGui::BulletText("Left Drag: Move selected knot");
    ImGui::BulletText("Right Click: Add new knot/curve");
    ImGui::BulletText("Middle Drag: Rotate camera");
    ImGui::BulletText("Scroll: Zoom in/out");

    ImGui::Separator();
    ImGui::Text("Keyboard Shortcuts:");
    ImGui::BulletText("W/A/S/D: Move camera");
    ImGui::BulletText("Ctrl+Z: Undo");
    ImGui::BulletText("Ctrl+Y: Redo");
    ImGui::BulletText("Ctrl+S: Save curves");
    ImGui::BulletText("Ctrl+O: Load curves");
    ImGui::BulletText("F1: Toggle help");

    ImGui::Separator();
    ImGui::Text("Features:");
    ImGui::BulletText("Preset Shapes: Create ready-made curves");
    ImGui::BulletText("Animation: Animate your curves");
    ImGui::BulletText("Save/Load: Export/import curves as JSON");
    ImGui::BulletText("Statistics: View curve information");
    ImGui::BulletText("Themes: Change UI appearance");

    ImGui::End();
}

void BSplineRenderer::ImGuiWindow::ApplyTheme(ThemeStyle Style)
{
    mCurrentTheme = Style;

    switch (Style)
    {
    case ThemeStyle::Dark:
        ImGui::StyleColorsDark();
        break;
    case ThemeStyle::Light:
        ImGui::StyleColorsLight();
        break;
    case ThemeStyle::Classic:
        ImGui::StyleColorsClassic();
        break;
    case ThemeStyle::Blue:
    {
        ImGui::StyleColorsDark();
        ImGuiStyle& s = ImGui::GetStyle();
        s.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.15f, 0.95f);
        s.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.3f, 0.6f, 0.8f);
        s.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.4f, 0.7f, 0.9f);
        s.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.35f, 0.65f, 1.0f);
        s.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.3f, 0.6f, 0.8f);
        s.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.4f, 0.7f, 0.9f);
        s.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.35f, 0.65f, 1.0f);
        s.Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.15f, 0.3f, 0.8f);
        s.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.15f, 0.3f, 1.0f);
        s.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.2f, 0.4f, 1.0f);
        break;
    }
    }
}

void BSplineRenderer::ImGuiWindow::SetRendererManager(RendererManager* pManager)
{
    mRendererManager = pManager;
}

void BSplineRenderer::ImGuiWindow::SetCurveContainer(CurveContainer* pContainer)
{
    mCurveContainer = pContainer;
}
