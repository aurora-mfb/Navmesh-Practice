// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISteering.h"

class AAICharacter;

/**
 * 
 */
class MPV_PRACTICAS_API ArriveSteering : public ISteering
{
public:

	ArriveSteering(AAICharacter* _pCharacter, FVector _vTargetPos, float _fMaxVelocity, float _fMaxForce, float _fArriveRadious);
	virtual ~ArriveSteering() override = default;

	virtual void DoSteering() override;
	virtual FVector GetLinearAcceleration() const override;
	virtual float GetAngularAcceleration() const  override;
	virtual void DrawDebug() override;

private:

	AAICharacter* m_pCharacter;

	FVector m_vLinearAcceleration;
	float m_fAngularAcceleration;

	FVector m_vTargetPos;
	float m_fMaxVelocity;
	float m_fMaxAcceleration;
	FVector m_vDesiredVelocity;
	float m_fDestRadius;
};
