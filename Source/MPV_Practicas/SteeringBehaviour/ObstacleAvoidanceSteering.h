// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISteering.h"
#include "SeekSteering.h"

class AAICharacter;

#define MAX_SEE_AHEAD 800.0f 
#define MAX_AVOID_FORCE 850.0f 
/**
 * 
 */
class MPV_PRACTICAS_API ObstacleAvoidanceSteering : public ISteering
{
public:

	ObstacleAvoidanceSteering(AAICharacter* _Character, TArray<FVector> _obstacles, float _charRad, float _MaxVelocity, float _maxAcceleration);
	virtual ~ObstacleAvoidanceSteering() override = default;

	virtual void DoSteering() override;
	virtual FVector GetLinearAcceleration() const override;
	virtual float GetAngularAcceleration() const  override;
	virtual void DrawDebug() override;

	void SetCurrentVelocity(FVector _CurrentVel);
	void SetTargetPos(FVector _targetPos);

private:

	AAICharacter* m_pCharacter;

	FVector m_vLinearAcceleration;
	float m_fAngularAcceleration;

	SeekSteering* m_pSeekBehaviour;
	TArray<FVector> m_obstacles;
	FVector m_vCurrentVelocity;
	float m_fMaxVelocity;
	float m_fCharRad;
	FVector m_vTargetPos;
	float m_fMaxAcceleration;
};
