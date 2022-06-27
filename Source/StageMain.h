#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Collision.h"

//�X�e�[�W
class Stage
{
public:
    Stage();
    ~Stage();

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, RenderContext rc,ModelShader* shader);

    void UpdateTransform();

    //���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //�C���X�^���X�擾
    static Stage& Instance();

    //���f���Q�b�^�[
    Model* GetModel()const { return model; }

    //ImGui�f�o�b�O�\��
    void DrawDebugGUI();

private:
    Model* model = nullptr;


};