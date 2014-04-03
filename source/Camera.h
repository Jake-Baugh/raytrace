#ifndef CAMERA_H
#define CAMERA_H

#include "stdafx.h"
#include <vector>

class Camera
{
public:
	static Camera* GetCamera(int index);

	void setLens(float fovY, float aspect, float zn, float zf);
	void SetPosition(DirectX::XMFLOAT4 lPos);

	void strafe(float d);
	void walk(float d);
	void setYPosition(float y);
	void pitch(float angle);
	void rotateY(float angle);
	void rebuildView();

	void MoveY(float p_step);

	DirectX::XMFLOAT4 GetPosition()		const { return DirectX::XMFLOAT4(mPosition.x, mPosition.y, mPosition.z, 1); };
	DirectX::XMFLOAT4 GetLookAt()		const { return DirectX::XMFLOAT4(mLook.x, mLook.y, mLook.z, 1); };
	DirectX::XMFLOAT4 GetUp()			const { return DirectX::XMFLOAT4(mUp.x, mUp.y, mUp.z, 1); };
	DirectX::XMFLOAT4 GetRight()		const { return DirectX::XMFLOAT4(mRight.x, mRight.y, mRight.z, 1); };

//	XMMATRIX GetView()		const{ return mView; }
//	XMMATRIX GetProj()		const{ return mProj;}

	DirectX::XMFLOAT4X4 GetView()		const{ return m_view; }
	DirectX::XMFLOAT4X4 GetProj()		const{ return m_proj; }

private:
	static std::vector<Camera*> m_camera;

	Camera();
	~Camera();

	DirectX::XMFLOAT4 mPosition;
	DirectX::XMFLOAT4 mRight;
	DirectX::XMFLOAT4 mUp;
	DirectX::XMFLOAT4 mLook;

	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_proj;

};

#endif