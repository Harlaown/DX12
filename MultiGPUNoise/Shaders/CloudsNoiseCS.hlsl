
float random(float n)
{
    return frac(sin(n) * 43758.5453);
}

float rand(float2 c)
{
    return frac(sin(dot(c.xy, float2(12.9898, 78.233))) * 43758.5453);
}

#define PI 3.14159265358979323846

#define NUM_NOISE_OCTAVES 5

// Precision-adjusted variations of https://www.shadertoy.com/view/4djSRW
// Precision-adjusted variations of https://www.shadertoy.com/view/4tdSWr



static const float cloudscale = 1.1;
static const float speed = 0.03;
static const float clouddark = 0.5;
static const float cloudlight = 0.3;
static const float cloudcover = 0.2;
static const float cloudalpha = 8.0;
static const float skytint = 0.5;
static const float3 skycolour1 = float3(0.2, 0.4, 0.6);
static const float3 skycolour2 = float3(0.4, 0.7, 1.0);
static const float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
static const float2 RenderTargetSize = float2(4096, 4096);
static const float TotalTime = 0.0f;

float2 hash(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float noise(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //float2 of = 0.5 + 0.5*float2(sign(a.x-a.y), sign(a.y-a.x));
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, float3(70.0, 70.0, 70.0));
}

float fbm(float2 n)
{
    float total = 0.0, amplitude = 0.1;
    for (int i = 0; i < 7; i++)
    {
        total += noise(n) * amplitude;
        n = mul(m, n);
        amplitude *= 0.4;
    }
    return total;
}

RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
    float2 p = DTid.xy / RenderTargetSize.xy;
	
    float2 uv = p * float2(RenderTargetSize.x / RenderTargetSize.y, 1.0);

	
    float currentSpeed = TotalTime * speed;
    float q = fbm(uv * cloudscale * 0.5);

	
    //ridged noise shape
    float r = 0.0;
    uv *= cloudscale;
    uv -= q - currentSpeed;
    float weight = 0.8;
    for (int i = 0; i < 8; i++)
    {
        r += abs(weight * noise(uv));
        uv = mul(m, uv) + currentSpeed;
        weight *= 0.7;
    }
    
    //noise shape
    float f = 0.0;
    uv = p * float2(RenderTargetSize.x / RenderTargetSize.y, 1.0);
    uv *= cloudscale;
    uv -= q - currentSpeed;
    weight = 0.7;
    for (int i = 0; i < 8; i++)
    {
        f += weight * noise(uv);
        uv = mul(m, uv) + currentSpeed;
        weight *= 0.6;
    }
    
    f *= r + f;
    
    //noise colour
    float c = 0.0;
    currentSpeed = TotalTime * speed * 2.0;
    uv = p * float2(RenderTargetSize.x / RenderTargetSize.y, 1.0);
    uv *= cloudscale * 2.0;
    uv -= q - currentSpeed;
    weight = 0.4;
    for (int i = 0; i < 7; i++)
    {
        c += weight * noise(uv);
        uv = mul(m, uv) + currentSpeed;
        weight *= 0.6;
    }
    
    //noise ridge colour
    float c1 = 0.0;
    currentSpeed = TotalTime * speed * 3.0;
    uv = p * float2(RenderTargetSize.x / RenderTargetSize.y, 1.0);
    uv *= cloudscale * 3.0;
    uv -= q - currentSpeed;
    weight = 0.4;
    for (int i = 0; i < 7; i++)
    {
        c1 += abs(weight * noise(uv));
        uv = mul(m, uv) + currentSpeed;
        weight *= 0.6;
    }
	
    c += c1;
    
    float3 skycolour = lerp(skycolour2, skycolour1, p.y);
    float3 cloudcolour = float3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight * c), 0.0, 1.0);
   
    f = cloudcover + cloudalpha * f * r;
    
    float3 result = lerp(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));

    output[DTid.xy] = float4(result, 1.0);
}