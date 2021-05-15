#pragma once

#include "DXF.h"
#include "MyBaseShader.h"

class ColourShader : public MyBaseShader
{
public:
	struct ColourBufferType
	{
		XMFLOAT4 colour;
	};

public:
	ColourShader(ID3D11Device* device, HWND hwnd);
	~ColourShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 colour);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* colourBuffer;
};
