#include "StageMain.h"

#include "Camera.h"
#include "imgui.h"

static StageMain* instance = nullptr;

DirectX::XMFLOAT4X4 transform = {
1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 1, 0,
0, 0, 0, 1
};

//�C���X�^���X�擾
StageMain& StageMain::Instance()
{
    return *instance;
}

void Stage::DrawDebugGUI()
{
    const ModelResource* resource = model->GetResource();
    const std::vector<Model::Node>& nodes = model->GetNodes();
    if (ImGui::CollapsingHeader("Stage", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::TreeNode("Material"))
        {
            for (const ModelResource::Mesh& mesh : resource->GetMeshes())
            {
                for (const ModelResource::Subset& subset : mesh.subsets)
                {
                    ImGui::SliderFloat("smooth", &subset.material->pbr.adjustSmoothness, -1.0f, 1.0f);
                    ImGui::SliderFloat("Metalic", &subset.material->pbr.adjustMetalness, -1.0f, 1.0f);
                }
            }
            ImGui::TreePop();
        }
    }
   
}
Model::Node Barel;

//�R���X�g���N�^
StageMain::StageMain()
{
    instance = this;

    //�X�e�[�W���f����ǂݍ���
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/blockMap1/map1.mdl");
    model = new Model("Data/Model/blockMap1/map2/map2.mdl");
    //model = new Model("Data/Model/blockMap/blockMap.mdl");
    
    model->nodeSearch("map2", map,&mapBottom);   //map�Ƀm�[�h������(map�̂��������)
    map[1][2][6] = {};
}

StageMain::~StageMain()
{
    //�X�e�[�W���f����j��
    delete model;
}

void StageMain::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    //��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&model->GetNodes().at(0).rotate));
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(model->GetNodes().at(0).translate.x, model->GetNodes().at(0).translate.y, model->GetNodes().at(0).translate.z);
    //3�̍s���g�ݍ��킹�āA���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

#include "Input/Input.h"
//�X�V����
void StageMain::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {   //���肤������
        map[1][4][6].translate.z += 1;
    }

    if (!map[1][4][6].name)
    {
        return;
    }
    //�����Ƃ�
    if (bottomSearch())
    {
        map[1][4][6].translate.y -= 1;
        int mx = map[1][4][6].translate.x + mapLeft;
        int mz = map[1][4][6].translate.z - mapForward;
        int my = map[1][4][6].translate.y; 

        if (my == 0)
        {
            map[my][-mz][mx] = map[1][4][6];
            map[1][4][6] = {};
                model->GetNodes()[map[my][-mz][mx].Index] = map[my][-mz][mx];
                model->UpdateTransform(transform);


        }
    }
    if (!map[1][4][6].name)
    {
        return;
    }

    model->GetNodes()[map[1][4][6].Index] = map[1][4][6];

    model->UpdateTransform(transform);

}

bool StageMain::bottomSearch()
{
    int mx = map[1][4][6].translate.x+mapLeft;
    int mz = map[1][4][6].translate.z-mapForward;
    int my = map[1][4][6].translate.y - 1;  //������w��
    //�}�b�v-y��T��
    if (my >= 0) {
        if (map[my][-mz][mx].name)return false;
    }


    mx = map[1][4][6].translate.x;
    mz = map[1][4][6].translate.z;

    for (Model::Node mapB : mapBottom)  //�}�b�vy0��艺��������
    {
        if (mapB.translate.y != my)continue;    //�����T��
        if (mapB.translate.x == mx && mapB.translate.z == mz)
        {
            return false;
        }
    }
    return true;    //�����Ȃ��ꍇ
}

//���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}


//�`�揈��
void Stage::Render(ID3D11DeviceContext* dc,RenderContext rc, ModelShader* shader)
{
    //�V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, rc, model);
}
