#include "shaderWater.hlsli"

//==============================================================================
//	水のピクセルシェーダー（レイトレーシング）
//
//	水面の1ピクセルごとに光線（レイ）を飛ばして色を決める。
//	  ・反射：水面で跳ね返った光線が、地形・立方体・空のどれに当たるかを調べる
//	  ・屈折：水の中に曲がって入った光線が湖底に当たるまでの距離で、水の色を付ける
//	  ・フレネル：水面を浅い角度で見るほど反射が強く、真上から見るほど底が見える
//
//	※ DirectX 11 にはハードウェアのレイトレーシング（DXR）がないので、
//	   シェーダーの計算だけで光線と物体の当たり判定をしている。
//	   ここで知っている物体（地形と立方体）だけが水に映る。
//==============================================================================

// 地形の形（field.cpp と同じ値・同じ式にすること）
static const float LAKE_RADIUS = 12.0;
static const float LAKE_BOTTOM = -3.0;
static const float SHORE_HEIGHT = 0.6;
static const float TERRAIN_MAX_HEIGHT = 4.5;	// 地形のいちばん高いところより少し上

static const float WATER_IOR = 1.33;			// 水の屈折率
static const float3 WATER_ABSORB = float3(0.45, 0.12, 0.08);	// 水が色を吸う強さ（赤が一番吸われるので青緑に見える）
static const float3 WATER_DEEP_COLOR = float3(0.02, 0.12, 0.16);	// 深いところの水の色

// 軽くするための設定（光線を飛ばす回数・距離を必要なぶんだけにする）
static const float REFLECTION_MIN_FRESNEL = 0.04;	// 反射がこれより弱いとき（真上近くから見たとき）は、反射の光線を飛ばさず空の色で代わりにする
static const float REFRACTION_MAX_FRESNEL = 0.97;	// 反射がこれより強いとき（真横近くから見たとき）は、屈折の光線を飛ばさない
static const int   REFLECTION_STEPS = 96;			// 反射の光線を進める最大回数
static const int   REFRACTION_STEPS = 48;			// 屈折の光線を進める最大回数（水の中は短い距離で湖底に当たる）
static const float REFRACTION_MAX_DISTANCE = 12.0;	// 屈折の光線を追う距離（湖の深さは最大でも約3.6）


// 地形の高さ（field.cpp の FieldGetHeight と同じ式）
float TerrainHeight(float2 p)
{
	float d = length(p) + 1.2 * sin(p.x * 0.35) * sin(p.y * 0.3);

	float basin = lerp(LAKE_BOTTOM, SHORE_HEIGHT, smoothstep(0.0, 1.0, saturate(d / LAKE_RADIUS)));

	float hillMask = saturate((d - LAKE_RADIUS) / LAKE_RADIUS);
	float hills = 1.2 * (sin(p.x * 0.15) * cos(p.y * 0.13) + 1.0)
				+ 0.6 * (sin(p.x * 0.31 + 1.7) * sin(p.y * 0.27 + 0.5) + 1.0);

	return basin + hills * hillMask;
}

// 地形の色（field.cpp の FieldGetColor と同じ式）
float3 TerrainColor(float height)
{
	const float3 sand = float3(0.78, 0.72, 0.52);
	const float3 grass = float3(0.30, 0.62, 0.25);
	const float3 darkGrass = float3(0.20, 0.45, 0.18);

	float t1 = saturate((height - 0.5) / 0.6);
	float t2 = saturate((height - 2.5) / 1.5);
	return lerp(lerp(sand, grass, t1), darkGrass, t2);
}

// 地形の法線（まわりの高さの差から求める）
float3 TerrainNormal(float2 p)
{
	const float e = 0.1;
	return normalize(float3(
		TerrainHeight(p - float2(e, 0.0)) - TerrainHeight(p + float2(e, 0.0)),
		2.0 * e,
		TerrainHeight(p - float2(0.0, e)) - TerrainHeight(p + float2(0.0, e))));
}

// ライトの当たり方（ふつうの3Dの頂点シェーダーと同じ計算）
float3 Shade(float3 albedo, float3 normal)
{
	float light = saturate(dot(normal, -normalize(Light.Direction.xyz)));
	return albedo * (Light.Diffuse.rgb * light + Light.Ambient.rgb);
}

// 空の色（地平線ほど白っぽく）
float3 SkyColor(float3 dir)
{
	return lerp(float3(0.75, 0.85, 0.95), float3(0.4, 0.6, 0.9), saturate(dir.y * 2.0));
}


// 光線と地形の当たり判定（レイマーチング：少しずつ進めて、地面の下に入ったところを探す）
bool TraceTerrain(float3 origin, float3 dir, float maxDistance, int maxSteps, out float hitDistance)
{
	bool hit = false;
	float t = 0.02;
	float prevT = 0.0;

	[loop]
	for (int i = 0; i < maxSteps; i++)
	{
		float3 p = origin + dir * t;
		float aboveGround = p.y - TerrainHeight(p.xz);

		// 地面の下に入った
		if (aboveGround < 0.0) { hit = true; break; }

		// 上に向かう光線が地形より高く出たら、もう当たらない
		if (dir.y > 0.0 && p.y > TERRAIN_MAX_HEIGHT) break;

		prevT = t;
		t += max(0.05, aboveGround * 0.4);	// 地面から離れているほど大きく進む
		if (t > maxDistance) break;
	}

	// 当たったら、1つ前の位置との間を半分ずつ狭めて、当たった場所を細かく求める
	float lo = prevT;
	float hi = t;
	[unroll]
	for (int j = 0; j < 6; j++)
	{
		float mid = (lo + hi) * 0.5;
		float3 q = origin + dir * mid;
		if (q.y - TerrainHeight(q.xz) < 0.0) hi = mid;
		else lo = mid;
	}

	hitDistance = hit ? hi : 0.0;
	return hit;
}

// 光線と立方体の当たり判定（立方体の座標に直して、-0.5〜0.5 の箱と判定する）
bool TraceCube(float3 origin, float3 dir, out float hitDistance, out float3 hitNormal)
{
	float3 o = mul(float4(origin, 1.0), CubeInvWorld).xyz;
	float3 d = mul(float4(dir, 0.0), CubeInvWorld).xyz;

	// スラブ法：x・y・z それぞれの面の間に光線が入っている区間の重なりを求める
	float3 invD = 1.0 / d;
	float3 t0 = (-0.5 - o) * invD;
	float3 t1 = ( 0.5 - o) * invD;
	float3 tMin = min(t0, t1);
	float3 tMax = max(t0, t1);
	float tNear = max(max(tMin.x, tMin.y), tMin.z);
	float tFar = min(min(tMax.x, tMax.y), tMax.z);

	bool hit = (tNear <= tFar && tNear >= 0.0);

	// 当たった面の法線（立方体の座標）→ ワールド座標に直す
	float3 n = float3(0.0, 0.0, -sign(d.z));
	if (tNear == tMin.x)      n = float3(-sign(d.x), 0.0, 0.0);
	else if (tNear == tMin.y) n = float3(0.0, -sign(d.y), 0.0);

	hitDistance = hit ? tNear : 0.0;
	hitNormal = normalize(mul((float3x3)CubeInvWorld, n));
	return hit;
}

// 空と太陽のきらめき（光の向きと反射の向きがそろうと明るく光る）
float3 SkyAndSun(float3 dir)
{
	float sun = pow(saturate(dot(dir, -normalize(Light.Direction.xyz))), 400.0);
	return SkyColor(dir) + Light.Diffuse.rgb * sun * 4.0;
}

// 反射の光線が見るもの（いちばん近くに当たった物の色。何にも当たらなければ空と太陽）
float3 TraceReflection(float3 origin, float3 dir)
{
	float cubeT = 0.0;
	float terrainT = 0.0;
	float3 cubeNormal = float3(0.0, 1.0, 0.0);
	bool hitCube = TraceCube(origin, dir, cubeT, cubeNormal);
	bool hitTerrain = TraceTerrain(origin, dir, 80.0, REFLECTION_STEPS, terrainT);

	// 何にも当たらなければ空
	float3 color = SkyAndSun(dir);

	if (hitCube && (!hitTerrain || cubeT < terrainT))
	{
		color = Shade(CubeColor.rgb, cubeNormal);
	}
	else if (hitTerrain)
	{
		float3 p = origin + dir * terrainT;
		color = Shade(TerrainColor(p.y), TerrainNormal(p.xz));
	}
	return color;
}

// 屈折の光線が見るもの（湖底の色に、水の中を進んだ距離のぶん水の色を混ぜる）
float3 TraceRefraction(float3 origin, float3 dir)
{
	float3 color = WATER_DEEP_COLOR;
	float t = 0.0;
	if (TraceTerrain(origin, dir, REFRACTION_MAX_DISTANCE, REFRACTION_STEPS, t))
	{
		float3 p = origin + dir * t;
		float3 bottom = Shade(TerrainColor(p.y), TerrainNormal(p.xz)) * 0.8;
		float3 absorb = exp(-WATER_ABSORB * t);	// 進んだ距離が長いほど色が吸われる
		color = bottom * absorb + WATER_DEEP_COLOR * (1.0 - absorb);
	}
	return color;
}

// 波（向き・細かさ・速さの違う波を足して、水面の傾き＝法線を作る）
float3 WaveNormal(float2 p, float time)
{
	const float4 waves[4] = {
		// xy：向き  z：細かさ  w：高さ
		float4(normalize(float2( 1.0,  0.2)), 1.3, 0.030),
		float4(normalize(float2(-0.4,  1.0)), 2.1, 0.020),
		float4(normalize(float2( 0.7, -0.7)), 3.7, 0.010),
		float4(normalize(float2(-0.9, -0.3)), 6.0, 0.006),
	};
	const float speeds[4] = { 1.6, 2.3, 3.1, 4.0 };

	float2 slope = float2(0.0, 0.0);
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float phase = dot(waves[i].xy, p) * waves[i].z + time * speeds[i];
		slope += waves[i].xy * waves[i].z * waves[i].w * cos(phase);
	}
	return normalize(float3(-slope.x, 1.0, -slope.y));
}


//ピクセルシェーダ（水）
float4 main(WATER_PS_IN input) : SV_TARGET
{
	float3 p = input.WorldPos;
	float3 view = normalize(p - CameraPosition);	// カメラからこのピクセルへ向かう光線
	float3 normal = WaveNormal(p.xz, Time);

	// フレネル（シュリックの近似）：浅い角度で見るほど反射が強くなる
	// 先に求めておいて、ほとんど見えない方の光線は飛ばさない（いちばん重いのが光線の計算なので）
	float cosTheta = saturate(dot(-view, normal));
	float fresnel = 0.02 + 0.98 * pow(1.0 - cosTheta, 5.0);

	// 反射（波で下向きに跳ね返らないよう、少しだけ上向きにする）
	float3 reflectDir = reflect(view, normal);
	reflectDir.y = max(reflectDir.y, 0.01);
	reflectDir = normalize(reflectDir);
	float3 reflection;
	[branch]
	if (fresnel > REFLECTION_MIN_FRESNEL)
		reflection = TraceReflection(p + float3(0.0, 0.01, 0.0), reflectDir);
	else
		reflection = SkyAndSun(reflectDir);	// 反射は数%しか効かないので、空の色で代わりにする

	// 屈折
	float3 refraction;
	[branch]
	if (fresnel < REFRACTION_MAX_FRESNEL)
		refraction = TraceRefraction(p, refract(view, normal, 1.0 / WATER_IOR));
	else
		refraction = WATER_DEEP_COLOR;		// 屈折は数%しか効かないので、深い水の色で代わりにする

	return float4(lerp(refraction, reflection, fresnel), 1.0);
}
