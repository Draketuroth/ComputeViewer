Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//Write the final color into the destination texture.
	float4 pixel = SrcTexture[DTid.xy];
	DstTexture[DTid.xy] = float4(pixel.x + 0.5, pixel.y, pixel.z, pixel.a);
}