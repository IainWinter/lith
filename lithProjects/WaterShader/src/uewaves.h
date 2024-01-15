// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	GerstnerWaveFunctions.ush: Utility functions for Gerstner waves computation.
=============================================================================*/ 

#pragma once

#include "v2/math.h"

#define Gravity 980
#define SOLVE_NORMAL_Z 1
#define SteepnessThreshold 50
#define PER_WAVE_DATA_SIZE 2

struct FWaveParams
{
	vec2 Direction;
	float Wavelength;
	float Amplitude;
	float Steepness;
};

struct WaveOutput
{
	vec3 Normal;
	vec3 WPO;
};

struct GerstnerWaveRenderer
{
	vec2 WaveParamRTSize;
	int MaxWaves;
	vec2 WorldPos;
	float Time;
};

FWaveParams GetWaveRTData(GerstnerWaveRenderer inWaveRenderer, Texture2D inWaveParamsRT, SamplerState inWaveParamsRTSampler, int inWaveIndex, int inWaterBodyIndex)
{
	FWaveParams OutWaveParams;
		
	vec2 WaveUV;
	float WaveUV_b;
		
	inWaveIndex *= 2;
		
	WaveUV.x = inWaveIndex / inWaveRenderer.WaveParamRTSize.x;
	WaveUV.y = inWaterBodyIndex / inWaveRenderer.WaveParamRTSize.y;
	WaveUV += 0.5f / inWaveRenderer.WaveParamRTSize;
		
	WaveUV_b = WaveUV.x + (1.0f / inWaveRenderer.WaveParamRTSize.x);
		
	OutWaveParams.Direction = inWaveParamsRT.SampleLevel(inWaveParamsRTSampler, WaveUV, 0).rg;
		
	vec3 TempParams = inWaveParamsRT.SampleLevel(inWaveParamsRTSampler, vec2(WaveUV_b, WaveUV.y), 0).rgb;
		
	OutWaveParams.Wavelength = TempParams.x;
	OutWaveParams.Amplitude = TempParams.y;
	OutWaveParams.Steepness = TempParams.z;
		
	return OutWaveParams;
}
	
WaveOutput AddWaves(WaveOutput inWaveA, WaveOutput inWaveB)
{
	inWaveA.Normal += inWaveB.Normal;
	inWaveA.WPO += inWaveB.WPO;
		
	return inWaveA;
}
	
vec3 FinalizeNormal(vec3 inNormal)
{
	return normalize(vec3(inNormal.xy, 1 - inNormal.z));
}
	
vec3 PackNormalAndWPO(WaveOutput inWave)
{
	vec3 packedoutput = 0;
		
	packedoutput = floor(inWave.WPO * 100);
	packedoutput += ((inWave.Normal + 1.01) / 2.02);
	
	return packedoutput;
}
	
vec3 UnpackWaveNormal(float inPackedWave)
{
	vec3 outnormal = frac(inPackedWave);
	outnormal *= 2.02;
	outnormal -= 1.01;
	return outnormal;
}
	
vec3 UnpackWaveWPO(float inPackedWave)
{
	vec3 outWPO;
	outWPO = floor(inPackedWave) / 100;
	return outWPO;
}

vec2 GetWavePositionAndDispersion(GerstnerWaveRenderer inWaveRenderer, FWaveParams inWaveParams)
{
	float dispersion = 2 * PI / inWaveParams.Wavelength;
	vec2 wavevector = inWaveParams.Direction * dispersion;
	float wavespeed = sqrt(dispersion * Gravity);
	float wavetime = wavespeed * inWaveRenderer.Time;
		
	float wavepos = dot(inWaveRenderer.WorldPos, wavevector) - wavetime;
		
	return vec2(wavepos, dispersion);
}
	
WaveOutput ComputeSingleWaveVectorized(float inWaveSin, float inWaveCos, float inDispersion, FWaveParams inWaveParams)
{
	WaveOutput OutWave;
		
	float wKA = inWaveParams.Amplitude * inDispersion;
	float q = inWaveParams.Steepness / wKA;
		
	OutWave.Normal.xy = inWaveSin * wKA * inWaveParams.Direction;
		
#if SOLVE_NORMAL_Z
	OutWave.Normal.z = inWaveCos * wKA * q;
#else
		OutWave.Normal.z = 0;
#endif

	OutWave.WPO.xy = -q * inWaveSin * inWaveParams.Direction * inWaveParams.Amplitude;
	OutWave.WPO.z = inWaveCos * inWaveParams.Amplitude;

	return OutWave;
}
	
WaveOutput GetSingleGerstnerWave(GerstnerWaveRenderer inWaveRenderer, Texture2D inWaveParamsRT, SamplerState inWaveParamsRTSampler, int inWaveIndex, int inWaterBodyIndex)
{
	WaveOutput OutWave;
		
	FWaveParams CurrentWave;
	CurrentWave = GetWaveRTData(inWaveRenderer, inWaveParamsRT, inWaveParamsRTSampler, inWaveIndex, inWaterBodyIndex);
		
	float dispersion = 2 * PI / CurrentWave.Wavelength;
	vec2 wavevector = CurrentWave.Direction * dispersion;
	float wavespeed = sqrt(dispersion * Gravity);
	float wavetime = wavespeed * inWaveRenderer.Time;

	float wavepos = dot(inWaveRenderer.WorldPos, wavevector) - wavetime;
		
	float wavesin = sin(wavepos);
	float wavecos = cos(wavepos);
		
	float wKA = CurrentWave.Amplitude * dispersion;
		
	float q = CurrentWave.Steepness / wKA;
		
	OutWave.Normal.xy = wavesin * wKA * CurrentWave.Direction;
		
#if SOLVE_NORMAL_Z
	OutWave.Normal.z = wavecos * CurrentWave.Steepness * saturate((CurrentWave.Amplitude * SteepnessThreshold) / CurrentWave.Wavelength);
		//OutWave.Normal.z = wavecos *  wKA * (q/MaxWaves);
#else
		OutWave.Normal.z = 0;
#endif

	OutWave.WPO.xy = -q * wavesin * CurrentWave.Direction * CurrentWave.Amplitude;
	OutWave.WPO.z = wavecos * CurrentWave.Amplitude;

	return OutWave;
		
}
	
WaveOutput GetAllGerstnerWaves(GerstnerWaveRenderer inWaveRenderer, Texture2D inWaveParamsRT, SamplerState inWaveParamsRTSampler, int inWaterBodyIndex)
{
		
	WaveOutput OutWaves = (WaveOutput)0;
	WaveOutput CurrentWave;
		
	int MaxWaveRange = inWaveRenderer.MaxWaves - 1;
		
	for (int i = 0; i <= MaxWaveRange; i++)
	{
		CurrentWave = GetSingleGerstnerWave(inWaveRenderer, inWaveParamsRT, inWaveParamsRTSampler, i, inWaterBodyIndex);
		OutWaves = AddWaves(OutWaves, CurrentWave);
	}
		
	//The Normal B channel must be inverted after combining waves
	OutWaves.Normal = FinalizeNormal(OutWaves.Normal);
	return OutWaves;
}
	
WaveOutput GetAllGerstnerWavesVectorized(GerstnerWaveRenderer inWaveRenderer, int inWaterBodyIndex)
{
		
	WaveOutput OutWaves = (WaveOutput)0;
	WaveOutput CurrentWave;
		
	int MaxWaveRange = (inWaveRenderer.MaxWaves / 4) - 1;
	for (int i = 0; i <= MaxWaveRange; i++)
	{
		FWaveParams WaveA;
		FWaveParams WaveB;
		FWaveParams WaveC;
		FWaveParams WaveD;
			
		vec2 WaveAPosAndDispersion = GetWavePositionAndDispersion(inWaveRenderer, WaveA);
		vec2 WaveBPosAndDispersion = GetWavePositionAndDispersion(inWaveRenderer, WaveB);
		vec2 WaveCPosAndDispersion = GetWavePositionAndDispersion(inWaveRenderer, WaveC);
		vec2 WaveDPosAndDispersion = GetWavePositionAndDispersion(inWaveRenderer, WaveD);
			
		float4 WaveSines = 0;
		float4 WaveCosines = 0;
			
		WaveSines = sin(float4(WaveAPosAndDispersion.x, WaveBPosAndDispersion.x, WaveCPosAndDispersion.x, WaveDPosAndDispersion.x));
		WaveCosines = cos(float4(WaveAPosAndDispersion.x, WaveBPosAndDispersion.x, WaveCPosAndDispersion.x, WaveDPosAndDispersion.x));
			
		//sincos( float4( WaveAPosAndDispersion.x, WaveBPosAndDispersion.x, WaveCPosAndDispersion.x, WaveDPosAndDispersion.x), WaveSines, WaveCosines);
			
		CurrentWave = ComputeSingleWaveVectorized(WaveSines.x, WaveCosines.x, WaveAPosAndDispersion.y, WaveA);
		OutWaves = AddWaves(OutWaves, CurrentWave);
			
		CurrentWave = ComputeSingleWaveVectorized(WaveSines.y, WaveCosines.y, WaveBPosAndDispersion.y, WaveB);
		OutWaves = AddWaves(OutWaves, CurrentWave);
			
		CurrentWave = ComputeSingleWaveVectorized(WaveSines.z, WaveCosines.z, WaveCPosAndDispersion.y, WaveC);
		OutWaves = AddWaves(OutWaves, CurrentWave);
			
		CurrentWave = ComputeSingleWaveVectorized(WaveSines.a, WaveCosines.a, WaveDPosAndDispersion.y, WaveD);
		OutWaves = AddWaves(OutWaves, CurrentWave);
			
	}

	//The Normal B channel must be inverted after combining waves
	OutWaves.Normal = FinalizeNormal(OutWaves.Normal);
	return OutWaves;
}
	
WaveOutput GetRangeGerstnerWaves(GerstnerWaveRenderer inWaveRenderer, Texture2D inWaveParamsRT, SamplerState inWaveParamsRTSampler, int inMinWaveIndex, int inMaxWaveIndex, int inWaterBodyIndex)
{
	WaveOutput OutWaves;
	WaveOutput CurrentWave;
		
	int MaxWaveRange = min(inMaxWaveIndex, inWaveRenderer.MaxWaves - 1);
		
	for (int i = inMinWaveIndex; i <= MaxWaveRange; i++)
	{
		CurrentWave = GetSingleGerstnerWave(inWaveRenderer, inWaveParamsRT, inWaveParamsRTSampler, i, inWaterBodyIndex);
		OutWaves = AddWaves(OutWaves, CurrentWave);
	}
		
	//The Normal B channel must be inverted after combining waves
	OutWaves.Normal = FinalizeNormal(OutWaves.Normal);
		
	return OutWaves;
}
	
// TODO[ jbard] placeholder : Remove ASAP
float ComputeWaveDepthAttenuationFactor(int InWaterBodyIndex, float InWaterDepth)
{
	return 1.0f;
}

GerstnerWaveRenderer InitializeGerstnerWaveRenderer(vec2 inWaveParamsRTSize, int inMaxwaves, vec2 inWorldPos, float inTime)
{
	GerstnerWaveRenderer OutWaveRenderer;
	
	OutWaveRenderer.WaveParamRTSize = inWaveParamsRTSize;
	OutWaveRenderer.MaxWaves = inMaxwaves;
	OutWaveRenderer.WorldPos = inWorldPos;
	OutWaveRenderer.Time = inTime;
		
	return OutWaveRenderer;
}

WaveOutput AddWavesNew(WaveOutput inWaveA, WaveOutput inWaveB)
{
	inWaveA.Normal += inWaveB.Normal;
	inWaveA.WPO += inWaveB.WPO;
		
	return inWaveA;
}
	
vec3 FinalizeNormalNew(vec3 inNormal)
{
	return normalize(vec3(inNormal.xy, 1.0f - inNormal.z));
}

/** Struct containing the decoded water body header data  */	
struct FWaterBodyHeader
{
	// Index into the actual wave data buffer
	int DataIndex;

	// Number of waves to be read from DataIndex
	int NumWaves;
	
	// Target Wave Mask Depth
	float TargetWaveMaskDepth;
};

/** Function to decode the raw data from the water indirection buffer */
FWaterBodyHeader DecodeWaterBodyHeader(float4 InDataToDecode)
{
	FWaterBodyHeader OutWaterBodyHeader = (FWaterBodyHeader)0;
	OutWaterBodyHeader.DataIndex = InDataToDecode.x; 
	OutWaterBodyHeader.NumWaves = InDataToDecode.y; 
	OutWaterBodyHeader.TargetWaveMaskDepth = InDataToDecode.z;
	
	return OutWaterBodyHeader;
}

/** Function to decode the data for an individual gerstner wave from the water data buffer */
FWaveParams DecodeWaveParams(float4 InDataToDecode0, float4 InDataToDecode1)
{
	FWaveParams OutWaveParams = (FWaveParams)0;
	OutWaveParams.Direction = InDataToDecode0.xy;
	OutWaveParams.Wavelength = InDataToDecode0.z;
	OutWaveParams.Amplitude = InDataToDecode0.w;
	OutWaveParams.Steepness = InDataToDecode1.x;

	return OutWaveParams;
}

FWaveParams GetWaveDataNew(int InWaveIndex, FWaterBodyHeader InWaterBodyHeader)
{
	const int AbsoluteWaveDataIndex = InWaterBodyHeader.DataIndex + (InWaveIndex * PER_WAVE_DATA_SIZE);

	float4 Data0 = View.WaterData[AbsoluteWaveDataIndex];
	float4 Data1 = View.WaterData[AbsoluteWaveDataIndex + 1];
		
	return DecodeWaveParams(Data0, Data1);
}

FWaveParams GetWaveDataByWaveIndexNew(int InWaterBodyIndex, int InWaveIndex)
{
	const FWaterBodyHeader WaterBodyHeader = DecodeWaterBodyHeader(View.WaterIndirection[InWaterBodyIndex]);
	return GetWaveDataNew(InWaveIndex, WaterBodyHeader);
}

vec3 PackNormalAndWPONew(WaveOutput inWave)
{
	vec3 packedoutput = (vec3)0;
		
	packedoutput = floor(inWave.WPO * 100);
	packedoutput += ((inWave.Normal + 1.01) / 2.02);
	
	return packedoutput;
}

WaveOutput GetSingleGerstnerWaveNew(int InWaveIndex, FWaterBodyHeader InWaterBodyHeader, float InTime, FLWCVector2 InWorldPos)
{
	WaveOutput OutWave = (WaveOutput)0;
		
	FWaveParams CurrentWave = GetWaveDataNew(InWaveIndex, InWaterBodyHeader);
		
	float Dispersion = 2 * PI / CurrentWave.Wavelength;
	vec2 WaveVector = CurrentWave.Direction * Dispersion;
	float WaveSpeed = sqrt(Dispersion * Gravity);
	float WaveTime = WaveSpeed * InTime;

	// Use the offset of the normalized tile as world position.
	// Doing proper LWC math is both more expensive and leads to less smooth/precise results.
	InWorldPos = LWCNormalizeTile(InWorldPos);
	vec2 WorldPos = InWorldPos.Offset;

	float WavePos = dot(WorldPos, WaveVector) - WaveTime;
	float WaveSin = sin(WavePos);
	float WaveCos = cos(WavePos);

	// Fix up tile borders by blending between them
	const vec2 TileBorderDist = (UE_LWC_RENDER_TILE_SIZE * 0.5f) - abs(WorldPos);
	const float BlendZoneWidth = 400.0f; // Blend over a range of 4 meters on each side of the tile
	if (any(TileBorderDist < BlendZoneWidth))
	{
		const vec2 BlendWorldPos = TileBorderDist;

		const vec2 BlendAlpha2 = saturate(TileBorderDist / BlendZoneWidth);
		const float BlendAlpha = BlendAlpha2.x * BlendAlpha2.y;

		const float BlendWavePos = dot(BlendWorldPos, WaveVector) - WaveTime;
		WaveSin = lerp(sin(BlendWavePos), WaveSin, BlendAlpha);
		WaveCos = lerp(cos(BlendWavePos), WaveCos, BlendAlpha);
	}
		
	float wKA = CurrentWave.Amplitude * Dispersion;
		
	float q = CurrentWave.Steepness / wKA;
		
	OutWave.Normal.xy = WaveSin * wKA * CurrentWave.Direction;
		
#if SOLVE_NORMAL_Z
		OutWave.Normal.z = WaveCos *  CurrentWave.Steepness * saturate( (CurrentWave.Amplitude * SteepnessThreshold) / CurrentWave.Wavelength );
		//OutWave.Normal.z = WaveCos *  wKA * (q/MaxWaves);
#else
		OutWave.Normal.z = 0;
#endif

	OutWave.WPO.xy = -q * WaveSin * CurrentWave.Direction *  CurrentWave.Amplitude;
	OutWave.WPO.z = WaveCos * CurrentWave.Amplitude;

	return OutWave;
		
}

WaveOutput GetAllGerstnerWavesNew(int InWaterBodyIndex, float InTime, FLWCVector2 InWorldPos)
{		
	WaveOutput OutWaves = (WaveOutput)0;
	WaveOutput CurrentWave = (WaveOutput)0;
		
	const FWaterBodyHeader WaterBodyHeader = DecodeWaterBodyHeader(View.WaterIndirection[InWaterBodyIndex]);

	for (int i = 0; i < WaterBodyHeader.NumWaves; i++)
	{
		CurrentWave = GetSingleGerstnerWaveNew(i, WaterBodyHeader, InTime, InWorldPos);
		OutWaves = AddWavesNew(OutWaves, CurrentWave);
	}
		
	//The Normal B channel must be inverted after combining waves
	OutWaves.Normal = FinalizeNormalNew(OutWaves.Normal);
	return OutWaves;
}
	
WaveOutput GetRangeGerstnerWavesNew(int InMinWaveIndex, int InMaxWaveIndex, int InWaterBodyIndex, float InTime, FLWCVector2 InWorldPos)
{
	WaveOutput OutWaves = (WaveOutput)0;
	WaveOutput CurrentWave = (WaveOutput)0;
		
	const FWaterBodyHeader WaterBodyHeader = DecodeWaterBodyHeader(View.WaterIndirection[InWaterBodyIndex]);

	const int MaxWaveRange = min(InMaxWaveIndex + 1, WaterBodyHeader.NumWaves);
	const int MinWaveRange = max(0, InMinWaveIndex);

	for (int i = MinWaveRange; i < MaxWaveRange; i++)
	{
		CurrentWave = GetSingleGerstnerWaveNew(i, WaterBodyHeader, InTime, InWorldPos);
		OutWaves = AddWavesNew(OutWaves, CurrentWave);
	}
		
	//The Normal B channel must be inverted after combining waves
	OutWaves.Normal = FinalizeNormalNew(OutWaves.Normal);
		
	return OutWaves;
}

/** Returns the wave attenuation factor according to a given water body and a given water depth. Should match the CPU version (GetWaveAttenuationFactor) */
float ComputeWaveDepthAttenuationFactorNew(int InWaterBodyIndex, float InWaterDepth)
{
	const FWaterBodyHeader WaterBodyHeader = DecodeWaterBodyHeader(View.WaterIndirection[InWaterBodyIndex]);

	// Completely erase waves when there are no waves at all (in case waves are computed on a water body with no waves, like a swimming pool) :
	const float TotalErasureFactor = saturate((float) WaterBodyHeader.NumWaves);
	const float StrengthCoefficient = exp(-max(InWaterDepth, 0.0f) / (WaterBodyHeader.TargetWaveMaskDepth / 2.0f));
	return saturate(1.0f - StrengthCoefficient) * TotalErasureFactor;
}