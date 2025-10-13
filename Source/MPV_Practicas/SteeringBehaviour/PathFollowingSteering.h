// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISteering.h"
#include "SeekSteering.h"

class AAICharacter;

/**
 * 
 */
class MPV_PRACTICAS_API PathFollowingSteering : public ISteering
{
public:

	PathFollowingSteering(AAICharacter* _Character, TArray<FVector> m_path, float _LookAhead, SeekSteering* _seekSteering);
	virtual ~PathFollowingSteering() override = default;

	virtual void DoSteering() override;
	virtual FVector GetLinearAcceleration() const override;
	virtual float GetAngularAcceleration() const  override;
	virtual void DrawDebug() override;

	void GetClosestPoint();

	FVector GetTargetPos();

	void SetPath(const TArray<FVector>& path);

private:

	AAICharacter* m_pCharacter;

	FVector m_vLinearAcceleration;
	float m_fAngularAcceleration;

	SeekSteering* m_pSeekBehaviour;
	TArray<FVector> m_path;
	FVector m_vCurrentTarget;
	int m_iClosestSegmentIndex;
	float m_fLookAhead;

};
