// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "params/params.h"
#include "GameFramework/PlayerController.h"
#include "util.h"
#include "Components/ArrowComponent.h"
#include "PaperSpriteActor.h"

#include "SteeringBehaviour/SeekSteering.h"
#include "SteeringBehaviour/ArriveSteering.h"
#include "SteeringBehaviour/PathFollowingSteering.h"
#include "SteeringBehaviour/ObstacleAvoidanceSteering.h"

#include "Pathfinding/Pathfinder.h"

#include "Navmesh/Navmesh.h"

#include "AICharacter.generated.h"

//struct FNavPolygon
//{
//	TArray<FVector2D> Points;
//};
//
//struct FNavLink
//{
//	int32 StartPolygonIndex;
//	int32 StartEdgeStart;
//	int32 StartEdgeEnd;
//	int32 EndPolygonIndex;
//	int32 EndEdgeStart;
//	int32 EndEdgeEnd;
//};

UCLASS()
class MPV_PRACTICAS_API AAICharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAICharacter();

	void ReadNavMesh(const FString& FileName);
	void DrawNavMesh();

	//TArray<FNavPolygon> m_navPolygons;
	//TArray<FNavLink> m_navLinks;


	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIChar)
	uint32 bDoMovement : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIChar)
		float current_angle;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* PathMaterial;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* NavmeshMaterial;

	UPROPERTY()
	UArrowComponent* DesiredVelocityArrow;

	UPROPERTY()
	UArrowComponent* LinearAccelerationArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	APaperSpriteActor* StopRadiusActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	Params m_params;

	TArray<FVector> m_path;

	TArray<FVector> m_obstacles;

	TMap<TCHAR, float> m_tileCosts;

	TArray<FString> m_gridMap;

	Navmesh* m_pNavMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AIFunctions")
	void OnClickedLeft(const FVector& mousePosition);
	UFUNCTION(BlueprintCallable, Category = "AIFunctions")
	void OnClickedRight(const FVector& mousePosition);
	
	const Params& GetParams() const { return m_params; }

	float GetActorAngle() const
	{
		FQuat currQuat = GetActorQuat();
		FVector axis;
		float axisAngle;
		currQuat.ToAxisAndAngle(axis, axisAngle);
		axisAngle = FMath::RadiansToDegrees(axisAngle);
		if (axis.Y > 0.0f)
			axisAngle = -axisAngle;

		axisAngle = convertTo360(axisAngle);
		return axisAngle;
	}
	void SetActorAngle(float angle) { FRotator newRot(angle, 0.0f, 0.0f); SetActorRotation(newRot); }

	void DrawDebug();

	FVector GetCurrentVelocity();

private:

	SeekSteering* m_pSeekBehaviour;
	ArriveSteering* m_pArriveBehaviour;
	PathFollowingSteering* m_pPathFollowBehaviour;
	ObstacleAvoidanceSteering* m_pObstacleAvoidanceBehaviour;

	Pathfinder* m_pPathfinder;

	TArray<FVector> m_currentPath;

	FVector m_vCurrentVelocity;

	FVector m_startPoint;

	FVector m_endPoint;

	bool bShouldFollowPath = false;
};
