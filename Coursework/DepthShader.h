#pragma once

#include "DXF.h"
#include "MyBaseShader.h"

class DepthShader : public MyBaseShader
{

public:
	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjectionMatrix);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* colourBuffer;
};

