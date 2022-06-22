#include "Graphics/Graphics.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "Input\Input.h"

#include "SceneGame.h"

// ������
void SceneGame::Initialize()
{

	//�X�e�[�W������
	stage = new StageMain();
	//�v���C���[������
	player = new Player();
	//�J�����R���g���[���[������
	cameraController = new CameraController();

	//�G�l�~�[������
	//EnemyManager::Instance().Register(new EnemySlime());

	EnemyManager& enemyManager = EnemyManager::Instance();
	//EnemySlime* slime = new EnemySlime();
	//slime->SetPosition(DirectX::XMFLOAT3(0, 0, 5));
	//enemyManager.Register(slime);

	for (int i = 0; i < 2; ++i)	//�����蔻��̂��߃G�l�~�[2�̐���
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f, 1000.0f
	);

	//�Q�[�W�X�v���C�g
	gauge = new Sprite();
}

// �I����
void SceneGame::Finalize()
{
	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();
	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	//�v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	//�Q�[�W�X�v���C�g�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	//DirectX::XMFLOAT3 target = player->GetPosition();
	DirectX::XMFLOAT3 target{0,0,0};

	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	//�v���C���[�X�V����
	player->Update(elapsedTime);
	//�G�l�~�[�X�V����
	EnemyManager::Instance().Upadate(elapsedTime);
	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;	//�`�悷�邽�߂ɕK�v�ȍ\����
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	//�J�����p�����[�^�[�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//// �r���[�s��
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// �J�����̎��_�i�ʒu�j
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// �J�����̒����_�i�^�[�Q�b�g�j
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// �J�����̏����

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// �v���W�F�N�V�����s��
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// ����p
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// ��ʏc���䗦
	//	float nearZ = 0.1f;	// �J�������f���o���̍ŋߋ���
	//	float farZ = 1000.0f;	// �J�������f���o���̍ŉ�����
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);	//�v���W�F�N�V�����s��쐬
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);	//rc�ɓn��
	//}

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc,rc);	//�V�F�[�_�[�ɃJ�����̏���n��

		//�X�e�[�W�`��
		stage->Render(dc, shader);
		//�v���C���[�`��
		player->Render(dc, shader);
		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
			//�v���C���[�f�o�b�O�v���~�e�B�u�`��
			player->DrawDebugPrimitive();


				//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
				EnemyManager::Instance().DrawDebugPrimitive();

				// ���C�������_���`����s
				graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

				// �f�o�b�O�����_���`����s
				graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("DebugMenu", nullptr, ImGuiWindowFlags_None))
		{

			//�v���C���[�f�o�b�O�`��
			player->DrawDebugGUI();

			//�J�����@
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				DirectX::XMFLOAT3 eye = camera.Instance().GetEye();
				DirectX::XMFLOAT3 focus = camera.Instance().GetFocus();
				ImGui::InputFloat3("Eye", &eye.x);
				ImGui::InputFloat3("Focus", &focus.x);
			}

			//�J�����R���g���[���[
			cameraController->DrawDebugGUI();

			//�G�l�~�[�}�l�[�W���[
			EnemyManager& enemyManager = EnemyManager::Instance();
			enemyManager.Instance().DrawDebugGUI();

		}
		ImGui::End();

	}

	////3D���f���`��
	//{
	//	Shader* shader = graphics.GetShader();
	//	shader->Begin(dc, rc);

	//	//�X�e�[�W�`��
	//	stage->Render(dc, shader);
	//	//�v���C���[�`��
	//	player->Render(dc, shader);

	//	shader->End(dc);
	//}
}

//�G�l�~�[HP�Q�[�W�`��
void SceneGame::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	//�r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	
	//�S�Ă̓G�̓����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemuCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemuCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();
		enemyPos.y += 1;	//�G�l�~�[�̓��������
		DirectX::XMVECTOR EnemyPos = DirectX::XMLoadFloat3(&enemyPos);

		//���[���h���W����X�N���[�����W�ɕϊ�
		EnemyPos = DirectX::XMVector3Project(
			EnemyPos,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			Projection, View, World
		);
		DirectX::XMStoreFloat3(&enemyPos, EnemyPos);

		//2d�X�v���C�g�`��
		gauge->Render(dc, enemyPos.x-20, enemyPos.y, 50/enemy->GetMaxHealth()*enemy->GetHealth(), 10,
			0, 0, 0, 0,
			0,
			1, 0, 0, 1);

		EnemyPos = DirectX::XMLoadFloat3(&enemyPos);

		//�X�N���[�����W���烏�[���h���W�ɕϊ�
		EnemyPos = DirectX::XMVector3Unproject(
			EnemyPos,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			Projection, View, World
		);

	}


	//�G�l�~�[�z�u����
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		//���C�X�^�[�g
		screenPosition.z = viewport.MinDepth;
		DirectX::XMVECTOR ScreenPosS = DirectX::XMLoadFloat3(&screenPosition);
		//���C�G���h
		screenPosition.z = viewport.MaxDepth;
		DirectX::XMVECTOR ScreenPosE = DirectX::XMLoadFloat3(&screenPosition);
		//�X�N���[�����W���烏�[���h���W�ɕϊ�
		ScreenPosS = DirectX::XMVector3Unproject(
			ScreenPosS,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			Projection, View, World
		);
		ScreenPosE = DirectX::XMVector3Unproject(
			ScreenPosE,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			Projection, View, World
		);
		DirectX::XMFLOAT3 screenPositionS;
		DirectX::XMStoreFloat3(&screenPositionS, ScreenPosS);
		DirectX::XMFLOAT3 screenPositionE;
		DirectX::XMStoreFloat3(&screenPositionE, ScreenPosE);

		//�n�`�Ƃ̓�����
		HitResult hit;
		if (StageMain::Instance().RayCast(screenPositionS, screenPositionE, hit))
		{
			if (EnemyManager::Instance().enemyLeftClickSet) {
				EnemySlime* enemy = new EnemySlime();
				enemy->SetPosition(hit.position);
				EnemyManager::Instance().Register(enemy);
			}
		}

	}

}
