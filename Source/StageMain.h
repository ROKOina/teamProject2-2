#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision.h"
#include <memory>
#include <vector>
using namespace std;

//�}�b�v�̑傫���i�}�b�v�`�b�v�j
#define  MAP_X (13)
#define  MAP_Z (7)
#define  MAP_Y (2)
#define mapLeft  ((MAP_X - 1) / 2)  //�X�e�[�W�����̒l
#define mapForward  ((MAP_Z - 1) / 2)


//�X�e�[�W
class StageMain
{
public:
    StageMain();
    ~StageMain();

    //�X�V����
    void Update(float elapsedTime);

    void MoveBlock(Model::Node* block, DirectX::XMFLOAT3 move);   //�u���b�N������


    void BlockDropJudge(Model::Node* map);  //���Ƀu���b�N�������ꍇ���Ƃ�

    Model::Node* SearchMap(int index);   //Index����node��T��


    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    void UpdateTransform();

    //���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //�C���X�^���X�擾
    static StageMain& Instance();

private:
    bool bottomSearch(Model::Node* map);    //���u���b�N�Ƀu���b�N�������ꍇtrue
    void GravityBlock(Model::Node* mapB);   //�������u���b�N�̉��Ƀu���b�N������Α������

public:
    //Y,Z,X�Ȃ̂Œ��ӁI�I�I�I�I�I�I
    Model::Node map[MAP_Y][MAP_Z][MAP_X] = {};
    vector<Model::Node> mapBottom;

private:
    //vector<vector<Model::Node>> map[MAP_X][MAP_Y]; //�}�b�v�`�b�v�����ꕨ
    
    Model* model = nullptr;


};