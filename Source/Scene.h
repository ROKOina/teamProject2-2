#pragma once
#include "Graphics/DepthStencil.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Light.h"
#include "Graphics/LightManager.h"
#include <memory>
//�V�[��
class Scene
{
public:
    Scene(){}
    virtual ~Scene(){}

    //������
    virtual void Initialize() = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render() = 0;

    //�����������Ă��邩
    bool IsReady()const { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }

private:
    bool ready = false;

    


};