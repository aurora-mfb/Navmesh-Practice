// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MPV_Practicas/params/params.h"
#include "CoreMinimal.h"

struct FNavPolygon
{
  int32 ID;
  TArray<FVector> Points;
  FVector Center;
  TArray<int32> LinkedPolygons;
};


struct FNavLink
{
  int32 StartPolygon;
  int32 EndPolygon;
};
/**
 * 
 */
class MPV_PRACTICAS_API Navmesh
{
public:
  Navmesh();

  // Carga el archivo XML y genera los polígonos
  bool InitializeNavMesh(const FString& FilePath);

  // Dibuja la malla en el mundo
  void DrawDebug(AActor* Owner, UMaterialInterface* Material = nullptr);

  // Retorna el índice del polígono donde cae un punto
  int32 GetPolygonIndexAtPoint(const FVector2D& Point) const;

  // Acceso directo a datos
  const TArray<FNavPolygon>& GetPolygons() const { return Polygons; }

  TArray<FVector> FindPath(const FVector& Start, const FVector& End);

  int32 FindContainingPolygon(const FVector& Point);

  bool IsPointInsidePolygon(const FVector& Point, const FNavPolygon& Poly);

private:
  TArray<FNavPolygon> Polygons;
  TArray<FNavLink> Links;

  bool LoadFromXML(const FString& FullPath);
  bool IsPointInPolygon(const FVector2D& Point, const TArray<FVector>& PolygonPoints) const;
};
