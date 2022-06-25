#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//�v���C���[
class Player : public Character
{
public: //�֐�
    Player();
    ~Player() override;

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    //�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();


    //�v���C���[��stage�̓����蔻��
    void CollisionStage(DirectX::XMFLOAT3);

    //�f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    ////�W�����v����
    //void Jump(float speed);

    ////���͏����X�V
    //void UpdateVelocity(float elapsedTime);

    //�W�����v���͏���
    void InputJump();

private:    //�֐�
    //�X�e�B�b�N���͒l����ړ��x�N�g�����擾
    DirectX::XMFLOAT3 GetMoveVec() const;

    //�ړ����͏���
    void InputMove(float elapsedTime);


    ////���񏈗�
    //void Turn(float elapsedTime, float vx, float vz, float speed);

protected:
    //���n�������ɌĂ΂��
    void OnLanding()override;

private:    //�ϐ�
    Model* model = nullptr;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);

    //�W�����v
    float jumpSpeed = 20.0f;
    //float gravity = -1.0f;
    //DirectX::XMFLOAT3 velocity = { 0,0,0 }; //����

    int JumpCount = 0;
    int JumpLimit = 2;


    //ImGui
    bool VisibleDebugPrimitive = true;

    //�G�t�F�N�g
    Effect* hitEffect = nullptr;
};