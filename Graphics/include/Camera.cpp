#include <Graphics\include\Camera.h>
using namespace DirectX::SimpleMath;

namespace Graphics
{

	Camera::Camera(ID3D11Device* device, int width, int height, float drawDistance, float fieldOfView) :
		CameraBuffer(device),
		inverseBuffer(device)
	{
		this->fieldOfView = fieldOfView;
		this->drawDistance = drawDistance;

		this->projection = DirectX::XMMatrixPerspectiveFovRH(fieldOfView, float(width) / height, 0.1f, drawDistance);

		values.vP = this->projection * this->view;
		values.view = this->view;
		values.invP = this->projection.Invert();
	}

	Camera::~Camera()
	{

	}

	void Camera::update(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 forward, ID3D11DeviceContext* context)
	{
		forward.Normalize();

		this->view = DirectX::XMMatrixLookToRH(pos, forward, Vector3(0, 1, 0));

		this->pos = pos;

		values.view = this->view;
		values.vP = this->view * this->projection;
		values.invP = this->projection.Invert();
		values.camPos = Vector4(pos.x, pos.y, pos.z, 1);

		inverseMatrixes.invP = values.invP;
		inverseMatrixes.invView = view.Invert();

		CameraBuffer.write(context, &values, sizeof(ShaderValues));
		inverseBuffer.write(context, &inverseMatrixes, sizeof(InverseMatrixes));
	}

	//Depricated, use update instead.
	void Camera::updateLookAt(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 target, ID3D11DeviceContext * context)
	{
		Matrix newView = DirectX::XMMatrixLookAtRH(pos, target, Vector3(0, 1, 0));


		this->view = newView;
		this->pos = pos;

		values.vP = this->view * this->projection;
		values.invP = this->projection.Invert();
		values.view = this->view;
		values.camPos = Vector4(pos.x, pos.y, pos.z, 1);

		D3D11_MAPPED_SUBRESOURCE data;
		ZeroMemory(&data, sizeof(data));

		context->Map(this->CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

		memcpy(data.pData, &values, sizeof(ShaderValues));

		context->Unmap(this->CameraBuffer, 0);

	}


	void Camera::setPos(DirectX::SimpleMath::Vector3 pos)
	{
		this->pos = pos;
	}

	void Camera::setForward(DirectX::SimpleMath::Vector3 forward)
	{
		this->forward = forward;
	}

	void Camera::setUp(DirectX::SimpleMath::Vector3 up)
	{
		this->up = up;
	}

	void Camera::setRight(DirectX::SimpleMath::Vector3 right)
	{
		this->right = right;
	}

	DirectX::SimpleMath::Vector3 Camera::getPos() const
	{
		return this->pos;
	}

	DirectX::SimpleMath::Vector3 Camera::getForward() const
	{
		return this->view.Forward();
	}

	DirectX::SimpleMath::Vector3 Camera::getUp() const
	{
		return this->view.Up();
	}

	DirectX::SimpleMath::Vector3 Camera::getRight() const
	{
		return this->view.Right();
	}

	DirectX::SimpleMath::Matrix Camera::getView() const
	{
		return view;
	}

	DirectX::SimpleMath::Matrix Camera::getProj() const
	{
		return projection;
	}

	ConstantBuffer<Camera::ShaderValues>* Camera::getBuffer()
	{
		return &this->CameraBuffer;
	}

	ConstantBuffer<Camera::InverseMatrixes>* Camera::getInverseBuffer()
	{
		return &this->inverseBuffer;
	}
}