#ifndef CAMERA_H
#define CAMERA_H

#include "stdafx.h"
#include <vector>

class Camera
{
public:
	static Camera* GetCamera(int index);

	void setLens(float fovY, float aspect, float zn, float zf);
	void SetPosition(XMFLOAT4 lPos);

	void strafe(float d);
	void walk(float d);
	void setYPosition(float y);
	void pitch(float angle);
	void rotateY(float angle);
	void rebuildView();

	void MoveY(float p_step);

	XMFLOAT4 GetPosition()		const { return XMFLOAT4(mPosition.x, mPosition.y, mPosition.z, 1);};
	XMFLOAT4 GetLookAt()		const { return XMFLOAT4(mLook.x, mLook.y, mLook.z, 1);};
	XMFLOAT4 GetUp()			const { return XMFLOAT4(mUp.x, mUp.y, mUp.z, 1);};
	XMFLOAT4 GetRight()			const { return XMFLOAT4(mRight.x, mRight.y, mRight.z, 1);};

//	XMMATRIX GetView()		const{ return mView; }
//	XMMATRIX GetProj()		const{ return mProj;}

	XMFLOAT4X4 GetView()		const{ return m_view; }
	XMFLOAT4X4 GetProj()		const{ return m_proj; }

private:
	static std::vector<Camera*>* m_camera;

	Camera();
	~Camera();

	XMFLOAT4 mPosition;
	XMFLOAT4 mRight;
	XMFLOAT4 mUp;
	XMFLOAT4 mLook;

	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;

};

#endif