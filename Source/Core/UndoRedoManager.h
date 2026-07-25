#pragma once

#include "Curve/Spline.h"

#include <memory>

#include <QStack>
#include <QVector3D>

namespace BSplineRenderer
{
    // Base class for Undo/Redo commands
    class Command
    {
      public:
        virtual ~Command() = default;
        virtual void Execute() = 0;
        virtual void Undo() = 0;
        virtual QString GetDescription() const = 0;
    };

    using CommandPtr = std::shared_ptr<Command>;

    // Knot position change command
    class MoveKnotCommand : public Command
    {
      public:
        MoveKnotCommand(KnotPtr pKnot, const QVector3D& OldPosition, const QVector3D& NewPosition, SplinePtr pSpline);

        void Execute() override;
        void Undo() override;
        QString GetDescription() const override;

      private:
        KnotPtr pKnot;
        QVector3D mOldPosition;
        QVector3D mNewPosition;
        SplinePtr pSpline;
    };

    // Add knot command
    class AddKnotCommand : public Command
    {
      public:
        AddKnotCommand(SplinePtr pSpline, KnotPtr pKnot);

        void Execute() override;
        void Undo() override;
        QString GetDescription() const override;

      private:
        SplinePtr pSpline;
        KnotPtr pKnot;
    };

    // Undo/Redo manager
    class UndoRedoManager
    {
      public:
        static UndoRedoManager& Instance();

        void ExecuteCommand(CommandPtr pCommand);

        void Undo();
        void Redo();

        bool CanUndo() const { return !mUndoStack.isEmpty(); }
        bool CanRedo() const { return !mRedoStack.isEmpty(); }

        int UndoCount() const { return mUndoStack.size(); }
        int RedoCount() const { return mRedoStack.size(); }

        void Clear();

        QString GetUndoDescription() const;
        QString GetRedoDescription() const;

      private:
        UndoRedoManager() = default;

        QStack<CommandPtr> mUndoStack;
        QStack<CommandPtr> mRedoStack;
    };
}
