#include "shaderWater.hlsli"


//頂点シェーダ（水）
WATER_PS_IN main(VS_IN input)
{
	WATER_PS_IN output;

	// ピクセルシェーダーで光線を飛ばすために、ワールド座標も渡す
	float4 world = mul(input.Position, World);
	output.WorldPos = world.xyz;
	output.Position = mul(mul(world, View), Projection);

	return output;
}
