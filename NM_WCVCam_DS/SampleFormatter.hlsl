
HLSL_EXTERNAL_INCLUDE(

Texture2D<float4> offscreenTexture : register(t0);
RWByteAddressBuffer outputBuffer: register(u0);

[numthreads(CS_THREADS_NUM_IN_CS, CS_THREADS_NUM_IN_CS, 1)]
void formatterMain(uint3 dispatchThreadId: SV_DispatchThreadID)
{
    float4 pixel0 = offscreenTexture.Load(int3(4 * dispatchThreadId.x, dispatchThreadId.y, 0));
    float4 pixel1 = offscreenTexture.Load(int3(4 * dispatchThreadId.x + 1, dispatchThreadId.y, 0));
    float4 pixel2 = offscreenTexture.Load(int3(4 * dispatchThreadId.x + 2, dispatchThreadId.y, 0));
    float4 pixel3 = offscreenTexture.Load(int3(4 * dispatchThreadId.x + 3, dispatchThreadId.y, 0));
    
    uint3 bgr24_3;
    bgr24_3.x = (uint(pixel0.b * 255.0) & 0xFF) | ((uint(pixel0.g * 255.0) & 0xFF) << 8)
        | ((uint(pixel0.r * 255.0) & 0xFF) << 16) | ((uint(pixel1.b * 255.0) & 0xFF) << 24);
    bgr24_3.y = (uint(pixel1.g * 255.0) & 0xFF) | ((uint(pixel1.r * 255.0) & 0xFF) << 8)
        | ((uint(pixel2.b * 255.0) & 0xFF) << 16) | ((uint(pixel2.g * 255.0) & 0xFF) << 24);
    bgr24_3.z = (uint(pixel2.r * 255.0) & 0xFF) | ((uint(pixel3.b * 255.0) & 0xFF) << 8)
        | ((uint(pixel3.g * 255.0) & 0xFF) << 16) | ((uint(pixel3.r * 255.0) & 0xFF) << 24);
    
    uint index = ((VCAM_VIDEO_HEIGHT_IN_CS - dispatchThreadId.y - 1) * VCAM_VIDEO_WIDTH_IN_CS 
        + 4 * dispatchThreadId.x) * 3;
    outputBuffer.Store3(index, bgr24_3);
}

)