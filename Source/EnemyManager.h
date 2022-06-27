#pragma once

#include <vector>
#include "Enemy.h"
#include <set>

//�G�l�~�[�}�l�W���[
class EnemyManager
{
private:
    EnemyManager(){}
    ~EnemyManager(){}

    //�G�l�~�[���m�̏Փ˔���
    void CollsionEnemyVsEnemies();

public:
    //�B��̃C���X�^���X�擾
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    //�X�V����
    void Upadate(float elapsedTime);

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, ModelShader* shader);

    void DrawDebugPrimitive();

    void DrawDebugGUI();

    //�G�l�~�[���擾
    int GetEnemyCount()const { return static_cast<int>(enemies.size()); }

    //�G�l�~�[�擾
    Enemy* GetEnemy(int index) { return enemies.at(index); }

    //�G�l�~�[�o�^
    void Register(Enemy* enemy);

    //�G�l�~�[�폜
    void Remove(Enemy* enemy);

    //�G�l�~�[�S�폜
    void Clear();

    //�G�l�~�[�z�ubool
    bool enemyLeftClickSet = false;


private:
    std::vector<Enemy*> enemies;
    std::set<Enemy*>removes;

    //ImGui
    bool VisibleDebugPrimitive = true;

};