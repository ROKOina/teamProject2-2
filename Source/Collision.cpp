#include "Collision.h"
#include <cmath>

//���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    //B->A�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionB, PositionA);  //�����Z�ŕ��������߂�a->b
    DirectX::XMVECTOR LengthSq  = DirectX::XMVector3LengthSq(Vec);  //sq = �X�N�G�A(������)    2�悵�Ă���ƍl���ėǂ�
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq); 

    //��������
    float range = radiusA + radiusB;
    if (range*range < lengthSq)
    {
        return false;
    }

    Vec = DirectX::XMVector3Normalize(Vec);     //���K�����ĒP�ʃx�N�g����
    Vec = DirectX::XMVectorScale(Vec, range);   //A��B�̋������̃x�N�g���쐬

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->B�ɉ����o���̂ŁA
                                                        //�|�W�V����A�𑫂��AVec�𑫂�

    //A��B�������o��
   // PositionB = DirectX::XMVectorAdd(Vec, LengthSq );
    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    return true;

}

//�~���Ɖ~���̌�������
bool Collision::IntersectCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB
)
{
    //A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }

    //A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
//    //�J�C�g�E�@
////XZ���ʂł͈̔̓`�F�b�N
//    float vx = positionB.x - positionA.x;
//    float vz = positionB.z - positionA.z;
//    float rangeA = radiusA + radiusB;
//    float distXZ = sqrtf(vx * vx + vz * vz);
//    if (distXZ > rangeA)
//    {
//        return false;
//    }
//    //A��B�������o��
//    vx /= distXZ;
//    vz /= distXZ;
//    outPositionB.x = positionA.x + (vx * rangeA);
//    outPositionB.y = positionA.y;
//    outPositionB.z = positionA.z + (vz * rangeA);
//
//    return true;

    //����
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //�����Z�ŕ��������߂�a->b
    Vec = DirectX::XMVectorSetY(Vec,0); //y����0��
                                                                              //DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = �X�N�G�A(������)    2�悵�Ă���ƍl���ėǂ�

    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = �X�N�G�A(������)    2�悵�Ă���ƍl���ėǂ�
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //��������
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }


    Vec = DirectX::XMVector3Normalize(Vec);     //���K�����ĒP�ʃx�N�g����
    Vec = DirectX::XMVectorScale(Vec, range);   //A��B�̋������̃x�N�g���쐬

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->B�ɉ����o���̂ŁA

    outPositionB.x = DirectX::XMVectorGetX(PositionB);
    outPositionB.y = 0;
    outPositionB.z = DirectX::XMVectorGetZ(PositionB);

    return true;


}

//���Ɖ~���̌�������
bool Collision::IntersectSphereVsCylider(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition
)
{   
    /////////////////////////////////////////////////
    //�����`�Ƌ������_�Ɏ����Ă���
    DirectX::XMFLOAT3 spherePos0 = { 
        spherePosition.x - cylinderPosition.x,
         spherePosition.y - cylinderPosition.y,
          spherePosition.z - cylinderPosition.z
    };                              
        if (fabsf(spherePos0.y) > cylinderHeight)return false;//y����
        if (fabsf(spherePos0.x) > cylinderRadius)return false;//x����
        if (fabsf(spherePos0.z) > cylinderRadius)return false;//z����

    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&cylinderPosition);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //�����Z�ŕ��������߂�a->b
                                                                              //DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = �X�N�G�A(������)    2�悵�Ă���ƍl���ėǂ�
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = �X�N�G�A(������)    2�悵�Ă���ƍl���ėǂ�
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->B�ɉ����o���̂ŁA
                                                        //�|�W�V����A�𑫂��AVec�𑫂�
    DirectX::XMStoreFloat3(&outCylinderPosition, PositionB);

    


    return true;

}

//���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    //���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        //���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //�O�p�`�i�ʁj�Ƃ̌�������
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                //�O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                //�O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                //�O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, BC));

                //���ς̌��ʂ��v���X�Ȃ�Η�����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dotLen = DirectX::XMVectorGetX(Dot);
                if (dotLen >= 0) continue;   //�������̏ꍇcontinue

                //���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(A, S), N);

                float x= DirectX::XMVectorGetX(SA) / DirectX::XMVectorGetX(Dot);
                if (x<0.0f || x>neart)continue; //��_�܂ł̋��������܂łɌv�Z�����ŋߋ������
                                                //�傫�����̓X�L�b�v
                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                //��_���O�p�`�̓����ɂ��邩����
                //���
                DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB); //�O�p�`�̊O��
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);  //�@���ƊO�ς̓��ς����߂�
                float dot1;
                dot1 = DirectX::XMVectorGetX(Dot1);
                if (dot1 < 0)continue;  //�O�p�`�̊O���Ȃ�continue

                //���
                DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC); //�O�p�`�̊O��
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);  //�@���ƊO�ς̓��ς����߂�
                float dot2;
                dot2 = DirectX::XMVectorGetX(Dot2);
                if (dot2 < 0)continue;  //�O�p�`�̊O���Ȃ�continue

                //�O��
                DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA); //�O�p�`�̊O��
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);  //�@���ƊO�ς̓��ς����߂�
                float dot3;
                dot3 = DirectX::XMVectorGetX(Dot3);
                if (dot3 < 0)continue;  //�O�p�`�̊O���Ȃ�continue

                //�ŋߋ������X�V
                neart = x;

                //��_��@�����X�V
                HitPosition = P;
                HitNormal = N;
                materialIndex = subset.materialIndex;

            }
        }
        if (materialIndex >= 0)
        {
            //���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition =
                DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            //�q�b�g���ۑ�
            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = 
                    DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, WorldNormal);
                //DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }

        }

    }

    ////�ȑO�̏����������������悤�ɉ��̎���
    //if (end.y < 0.0f)
    //{
    //    result.position.x = end.x;
    //    result.position.y = 0.0f;
    //    result.position.z = end.z;
    //    result.normal.x = 0.0f;
    //    result.normal.y = 1.0f;
    //    result.normal.z = 0.0f;
    //    return true;
    //}
    return hit;
}
