Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);
SamplerState BilinearClamp : register(s0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//Write the final color into the destination texture.
	DstTexture[DTid.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}