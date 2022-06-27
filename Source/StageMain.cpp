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
    map[1][2][6] = {};  //����item������
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


    model->UpdateTransform(transform);

}

Model::Node* StageMain::SearchMap(int index)
{
    for (int y = 0; y < MAP_Y; ++y)
    {
        for (int z = 0; z < MAP_Z; ++z)
        {
            for (int x = 0; x < MAP_X; ++x)
            {
                if (map[y][z][x].Index == index)
                    return &map[y][z][x];
            }
        }
    }
    return {};
}

void StageMain::MoveBlock(Model::Node* block,DirectX::XMFLOAT3 move)
{
    //���肤������
    if (block->name) {
        Model::Node* Block = SearchMap(block->Index);
        Block->translate = { Block->translate.x + move.x,Block->translate.y + move.y,Block->translate.z + move.z };
        model->GetNodes()[Block->Index] = *Block;   //����������node�ɏ㏑��
        BlockDropJudge(Block);
    }
}

void StageMain::BlockDropJudge(Model::Node* mapB)
{
    //�����Ƃ�
    if (bottomSearch(mapB)) //�X�e�[�W�̉�����(y0)
    {
        mapB->translate.y -= 1; //�X�e�[�W���Ȃ��ꍇ���Ƃ�
        model->GetNodes()[mapB->Index] = *mapB;
    }

    GravityBlock(mapB); //y-1��艺����
}

void StageMain::GravityBlock(Model::Node* mapB)
{
    int mx = mapB->translate.x;
    int mz = mapB->translate.z;
    int my = mapB->translate.y - 1;
    for (Model::Node mapNode : mapBottom)  //�}�b�vy0��艺��������
    {
        if (mapNode.translate.y != my)continue;    //�����T��
        if (mapNode.translate.x == mx && mapNode.translate.z == mz)
        {
            mx = mapB->translate.x + mapLeft;
            mz = mapB->translate.z - mapForward;
            my = mapB->translate.y;
            map[my][-mz][mx] = *mapB;
            *mapB = {};
            model->GetNodes()[map[my][-mz][mx].Index] = map[my][-mz][mx];
            model->UpdateTransform(transform);

        }
    }
}

bool StageMain::bottomSearch(Model::Node* mapB)
{
    int mx = mapB->translate.x+mapLeft;
    int mz = mapB->translate.z-mapForward;
    int my = mapB->translate.y - 1;  //������w��
    //�}�b�v-y��T��
    if (my >= 0) {
        if (map[my][-mz][mx].name)return false;
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
