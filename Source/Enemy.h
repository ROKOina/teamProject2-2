#pragma once

#include "Graphics//shader.h"
#include "Character.h"

//�G�l�~�[
class Enemy :public Character
{
public:
    Enemy(){}
    ~Enemy()override{}

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(ID3D11DeviceContext* dc, ModelShader* shader) = 0;

    //�j��
    void Destroy();

    //�f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();

    virtual void GuiEnemy() {}
protected:

};