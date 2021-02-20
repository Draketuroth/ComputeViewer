Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//Write the final color into the destination texture.
	float4 Color = float4(1.0, 0.0, 1.0, 1.0);
	DstTexture[DTid.xy] = SrcTexture[DTid.xy] * Color; // Simple tinting.
}