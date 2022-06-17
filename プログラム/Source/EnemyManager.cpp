#include <imgui.h>
#include "EnemyManager.h"
#include "Collision.h"
#include "EnemySlime.h"

//�X�V����
void EnemyManager::Upadate(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);

    }

    //�j������(�X�V�������I�������Ɉ�C�ɍ폜
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        //�G�̔j������
        delete enemy;
    }
    //�j�����X�g���N���A
    removes.clear();

    //�G���m�̓����蔻��
    CollsionEnemyVsEnemies();
}

//�G�l�~�[���m�̏Փ˔���
void EnemyManager::CollsionEnemyVsEnemies()
{

    int enemyCount = GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        for (int e = 0; e < enemyCount; ++e)//2�̂̃G�l�~�[���񂷂���2�dfor��
        {
            if (i == e)continue;    //�����G�l�~�[�Ȃ�R���e�j���[
            Enemy* enemyI = GetEnemy(i);
            Enemy* enemyE = GetEnemy(e);    //�����蔻�������G�l�~�[2�̎擾
                
                    //�Փˏ���
            DirectX::XMFLOAT3 outPosition;
            //if (Collision::IntersectSphereVsSphere(
            //    enemyI->GetPosition(), enemyI->GetRadius(),
            //    enemyE->GetPosition(), enemyE->GetRadius(),
            //    outPosition))
            //{
            //    //�����o����̈ʒu�ݒ�
            //    enemyE->SetPosition(outPosition);
            //}

            if (Collision::IntersectCylinder(
                enemyI->GetPosition(), enemyI->GetRadius(), enemyI->GetHeight(),
                enemyE->GetPosition(), enemyE->GetRadius(), enemyE->GetHeight(),
                outPosition
            ))
            {
                //�����o����̈ʒu�ݒ�
                enemyE->SetPosition(outPosition);
            }

        }
    }
}

//�`�揈��
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(dc, shader);
    }
}

void EnemyManager::DrawDebugPrimitive()
{
    if (!VisibleDebugPrimitive)return;

    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

void EnemyManager::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("EnemyManager", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("enemyLeftClickSet", &enemyLeftClickSet);

        if (ImGui::Button("Add"))
        {
            EnemySlime* enemy = new EnemySlime();
            enemy->SetPosition({ (float)(rand() % 100 - 50) / 10.0f,0,(float)(rand() % 100 - 50) / 10.0f });
            Register(enemy);
        }
        if (ImGui::Button("Clear"))
        {
            for (Enemy* enemy : enemies)
            {
                Remove(enemy);
            }
        }
        ImGui::Checkbox("VisibleDebugPrimitiveEnemy", &VisibleDebugPrimitive);

        //for (Enemy* enemy : enemies)
        //{
        //    if (ImGui::CollapsingHeader("a", ImGuiTreeNodeFlags_OpenOnArrow))
        //    {
        //        DirectX::XMFLOAT3 p = enemy->GetPosition();
        //        ImGui::InputFloat3("Position", &p.x);
        //        enemy->SetPosition(p);
        //    }

        //}
        
        for (int i=0;i<enemies.size();++i)
        {
            char number[] = { (char)i ,'\0'};
            if(ImGui::TreeNode(number))
            {
                enemies.at(i)->GuiEnemy();
                
                ImGui::TreePop();
            }
        }

    }

}

//�G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

//�G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
    //�j�����X�g�ɒǉ�
    removes.insert(enemy);
}

//�G�l�~�[�S�폜
void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
}
