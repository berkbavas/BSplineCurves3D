#include "Model.h"

BSplineRenderer::Model::Model(MeshPtr pMesh)
    : mMesh(pMesh)
{}

BSplineRenderer::MeshPtr BSplineRenderer::Model::GetMesh() const
{
    return mMesh;
}
