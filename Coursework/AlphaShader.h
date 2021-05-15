#pragma once

#include "DXF.h"
#include "MyBaseShader.h"

using namespace std;
using namespace DirectX;

class AlphaShader : public MyBaseShader
{
public:
	AlphaShader(ID3D11Device* device, HWND hwnd);
	~AlphaShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

};
