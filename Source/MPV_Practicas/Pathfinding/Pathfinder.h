// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FNode
{
  int32 Row;
  int32 Col;
  float GCost;
  float HCost;
  FNode* Parent;

  float FCost() const { return GCost + HCost; }

  FNode() : Row(0), Col(0), GCost(FLT_MAX), HCost(0), Parent(nullptr) {}
};

/**
 * 
 */
class MPV_PRACTICAS_API Pathfinder
{
public:
	Pathfinder(TMap<TCHAR, float> _tileCosts, TArray<FString> _gridMap);
	~Pathfinder();

  void FindPath();

	void DrawDebug();

  FIntPoint WorldToGrid(const FVector& WorldPos);

  void SetStartFromWorldPos(const FVector& WorldPos);

  void SetGoalFromWorldPos(const FVector& WorldPos);

private:

	TMap<TCHAR, float> m_tileCosts;
	TArray<FString> m_gridMap;
  TArray<FNode> CurrentPath;

  FIntPoint StartCell = FIntPoint(-1, -1);
  FIntPoint GoalCell = FIntPoint(-1, -1);
};
