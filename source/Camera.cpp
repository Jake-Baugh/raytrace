#include "Camera.h"

std::vector<Camera*>* Camera::m_camera = new std::vector<Camera*>(10);

Camera* Camera::GetCamera(int index)
{
	if(m_camera->at(index) == nullptr)
		m_camera->at(index) = new Camera();
	return m_camera->at(index);
}

Camera::Camera()
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRight    = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mUp       = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mLook     = XMFLOAT3(0.0f, 0.0f, 1.0f);

	mView = XMMatrixIdentity();
	mProj = XMMatrixIdentity();
}

Camera::~Camera()
{
}

void Camera::SetPosition(XMFLOAT3 lPos)
{
	mPosition = lPos;
}

void Camera::setYPosition(float y)
{
	mPosition.y = y;
}

void Camera::setLens(float fovY, float aspect, float zn, float zf)
{
	mProj = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
}

void Camera::strafe(float d)
{
	mPosition += d * mRight;
}

void Camera::walk(float d)
{
	mPosition += d * mLook;
}

void Camera::pitch(float angle)
{
	XMMATRIX R;
	
	R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal( XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal( XMLoadFloat3(&mLook), R));
}

void Camera::rotateY(float angle)
{
	XMMATRIX R;
	R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal( XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal( XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal( XMLoadFloat3(&mLook), R));
}

void Camera::rebuildView()
{
	mView = XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mLook), XMLoadFloat3(&mUp)); 
}

void Camera::MoveY(float p_step)
{
	mPosition.y += p_step;
}