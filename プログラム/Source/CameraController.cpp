#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

//�X�V����
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    //�J�����̉�]���x
    float speed = rollSpeed * elapsedTime;

    //�X�e�B�b�N�̓��͒l�����킹��X����Y������]
    if (ay > 0)
    {
        angle.x += speed;
    }
    if (ay < 0)
    {
        angle.x -= speed;
    }
    if (ax > 0)
    {
        angle.y += speed;
    }
    if (ax < 0)
    {
        angle.y -= speed;
    }

    //X���̃J������]�𐧌�
    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }
    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }

    //Y���̃J������]�l��-3.14�`3.14�Ɏ��܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }

    //�J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    //��]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    //�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
    DirectX::XMFLOAT3 eye;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //�J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DirectX::XMFLOAT3 a;
        a.x = DirectX::XMConvertToDegrees(angle.x);
        a.y = DirectX::XMConvertToDegrees(angle.y);
        a.z = DirectX::XMConvertToDegrees(angle.z);
        ImGui::InputFloat3("Angle", &a.x);
        angle.x = DirectX::XMConvertToRadians(a.x);
        angle.y = DirectX::XMConvertToRadians(a.y);
        angle.z = DirectX::XMConvertToRadians(a.z);

        ImGui::SliderFloat("RollSpeed", &rollSpeed,0,100);
        ImGui::SliderFloat("Range", &range,1,100);

        float max= DirectX::XMConvertToDegrees(maxAngleX);
        float min = DirectX::XMConvertToDegrees(minAngleX);
        ImGui::InputFloat("MaxAngleX", &max);
        ImGui::InputFloat("MinAngleX", &min);
        maxAngleX = DirectX::XMConvertToRadians(max);
        minAngleX = DirectX::XMConvertToRadians(min);

    }
}