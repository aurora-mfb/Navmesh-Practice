// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MPV_PRACTICAS_API ISteering
{
public:

	virtual ~ISteering() = default;

	virtual void DoSteering() = 0;
	virtual FVector GetLinearAcceleration() const = 0;
	virtual float GetAngularAcceleration() const = 0;
	virtual void DrawDebug() = 0;
};
