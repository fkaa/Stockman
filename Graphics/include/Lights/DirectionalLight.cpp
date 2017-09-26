#include "DirectionalLight.h"
#include <algorithm>
using namespace DirectX::SimpleMath;
#define PI 3.14159265
#define SUNSET_TIME 0.4f
#define DAY_NIGHT_ON false

DirectionalLight::DirectionalLight(ID3D11Device * device, int width, int height)
{
	pos = Vector4(0, 50, 0.5, 1);
	dayColor = Vector3(1, 1, 0.8);
	sunDownColor = Vector3(2, 0.5, -1);
	nightColor = Vector3(0.1, 0.1, 0.3);
	isNight = false;
	

	projection = DirectX::XMMatrixOrthographicRH(100.f, 100.f, 1, 200);
	view = DirectX::XMMatrixLookAtRH(pos, Vector3(0, 0, 0), Vector3(0, 1, 0));

	matrixData.vp = view * projection;
	
	shaderData.color = dayColor;
	shaderData.shadowFade = 1;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(ShaderMatrix);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &matrixData;

	device->CreateBuffer(&desc, &data, &this->matrixBuffer);

	desc.ByteWidth = sizeof(ShaderValues);
	data.pSysMem = &shaderData;

	device->CreateBuffer(&desc, &data, &this->shaderBuffer);


	viewPort = { 0 };
	viewPort.Height = height;
	viewPort.Width = width;
	viewPort.MaxDepth = 1.f;
}

DirectionalLight::~DirectionalLight()
{
	SAFE_RELEASE(matrixBuffer);
	SAFE_RELEASE(shaderBuffer);
}

void DirectionalLight::update(ID3D11DeviceContext * context, Vector3 offset)
{
	//a little bit temp, might be final
	static float rotationDeg = 0;

	//Enable to get the day night cycle
#if DAY_NIGHT_ON
	rotationDeg += 0.01745 * 0.25;
#else
	rotationDeg = 3.14 * 0.25;
#endif

	if (rotationDeg >= PI * 0.5)
	{
		rotationDeg = -PI * 0.5;
		isNight = !isNight;
	}

	Matrix rotation = Matrix::CreateRotationZ(rotationDeg);

	this->shaderData.pos = Vector4::Transform(pos, rotation);
	
	//If its nighttime the shadows fade out
	Vector3 lightDir = -shaderData.pos;
	lightDir.Normalize();

	Vector3 groundDir(1, 0, 0);

	float fade = snap(1.0 - abs(lightDir.Dot(groundDir)), 0, SUNSET_TIME);
	shaderData.shadowFade = fade / SUNSET_TIME;

	//Interpolates between colors
	float dayAmount = min(shaderData.shadowFade, 1);
	float sundownAmount = 1 - min(shaderData.shadowFade, 1);

	if (!isNight)
		shaderData.color = (dayColor * dayAmount) + (sunDownColor * sundownAmount);
	
	else
		shaderData.color = (nightColor * dayAmount);
	
	shaderData.color.x = snap(shaderData.color.x, 0, 1);
	shaderData.color.y = snap(shaderData.color.y, 0, 1);
	shaderData.color.z = snap(shaderData.color.z, 0, 1);
	
	this->shaderData.pos = shaderData.pos + offset;
	view = DirectX::XMMatrixLookAtRH(shaderData.pos, offset, Vector3(0, 1, 0));
	matrixData.vp = view * projection;


	D3D11_MAPPED_SUBRESOURCE data = {};
	
	context->Map(shaderBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &shaderData, sizeof(shaderData));

	context->Unmap(shaderBuffer, 0);

	context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &matrixData, sizeof(matrixData));

	context->Unmap(matrixBuffer, 0);

}

float DirectionalLight::snap(float value, float minVal, float maxVal)
{
	value = max(min(value, maxVal), minVal);
	return value;
}