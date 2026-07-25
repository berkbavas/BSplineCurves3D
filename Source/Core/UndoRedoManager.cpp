#include "UndoRedoManager.h"

BSplineRenderer::MoveKnotCommand::MoveKnotCommand(KnotPtr pKnot, const QVector3D& OldPosition, const QVector3D& NewPosition, SplinePtr pSpline)
    : pKnot(pKnot)
    , mOldPosition(OldPosition)
    , mNewPosition(NewPosition)
    , pSpline(pSpline)
{}

void BSplineRenderer::MoveKnotCommand::Execute()
{
    pKnot->SetPosition(mNewPosition);
    if (pSpline)
        pSpline->MakeDirty();
}

void BSplineRenderer::MoveKnotCommand::Undo()
{
    pKnot->SetPosition(mOldPosition);
    if (pSpline)
        pSpline->MakeDirty();
}

QString BSplineRenderer::MoveKnotCommand::GetDescription() const
{
    return "Move Knot";
}

BSplineRenderer::AddKnotCommand::AddKnotCommand(SplinePtr pSpline, KnotPtr pKnot)
    : pSpline(pSpline)
    , pKnot(pKnot)
{}

void BSplineRenderer::AddKnotCommand::Execute()
{
    pSpline->AddKnot(pKnot);
}

void BSplineRenderer::AddKnotCommand::Undo()
{
    pSpline->RemoveLastKnot();
}

QString BSplineRenderer::AddKnotCommand::GetDescription() const
{
    return "Add Knot";
}

BSplineRenderer::UndoRedoManager& BSplineRenderer::UndoRedoManager::Instance()
{
    static UndoRedoManager instance;
    return instance;
}

void BSplineRenderer::UndoRedoManager::ExecuteCommand(CommandPtr pCommand)
{
    pCommand->Execute();
    mUndoStack.push(pCommand);
    mRedoStack.clear();
}

void BSplineRenderer::UndoRedoManager::Undo()
{
    if (CanUndo())
    {
        auto pCommand = mUndoStack.pop();
        pCommand->Undo();
        mRedoStack.push(pCommand);
    }
}

void BSplineRenderer::UndoRedoManager::Redo()
{
    if (CanRedo())
    {
        auto pCommand = mRedoStack.pop();
        pCommand->Execute();
        mUndoStack.push(pCommand);
    }
}

void BSplineRenderer::UndoRedoManager::Clear()
{
    mUndoStack.clear();
    mRedoStack.clear();
}

QString BSplineRenderer::UndoRedoManager::GetUndoDescription() const
{
    return mUndoStack.isEmpty() ? "" : mUndoStack.top()->GetDescription();
}

QString BSplineRenderer::UndoRedoManager::GetRedoDescription() const
{
    return mRedoStack.isEmpty() ? "" : mRedoStack.top()->GetDescription();
}