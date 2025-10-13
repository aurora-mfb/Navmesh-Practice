// Fill out your copyright notice in the Description page of Project Settings.


#include "Navmesh.h"


#include "DrawDebugHelpers.h"
#include "XmlFile.h"
#include <ProceduralMeshComponent.h>
#include <CompGeom/PolygonTriangulation.h>

Navmesh::Navmesh() {}

bool Navmesh::InitializeNavMesh(const FString& FilePath)
{
  FString FullPath = FPaths::ProjectContentDir() / FilePath;
  return LoadFromXML(FullPath);
}

bool Navmesh::LoadFromXML(const FString& FullPath)
{
  FString FileContent;
  if (!FFileHelper::LoadFileToString(FileContent, *FullPath))
  {
    UE_LOG(LogTemp, Error, TEXT("No se pudo leer el archivo: %s"), *FullPath);
    return false;
  }

  FXmlFile XmlFile(FileContent, EConstructMethod::ConstructFromBuffer);
  if (!XmlFile.IsValid())
  {
    UE_LOG(LogTemp, Error, TEXT("Archivo XML inválido."));
    return false;
  }

  const FXmlNode* RootNode = XmlFile.GetRootNode();
  const FXmlNode* PolygonsNode = RootNode->FindChildNode(TEXT("polygons"));
  if (!PolygonsNode) return false;

  // --- Leer polígonos ---
  int32 PolyID = 0;
  for (const FXmlNode* PolygonNode : PolygonsNode->GetChildrenNodes())
  {
    if (PolygonNode->GetTag() != TEXT("polygon")) continue;

    FNavPolygon Poly;
    Poly.ID = PolyID++;

    FVector Sum = FVector::ZeroVector;

    for (const FXmlNode* PointNode : PolygonNode->GetChildrenNodes())
    {
      float X = FCString::Atof(*PointNode->GetAttribute(TEXT("x")));
      float Y = FCString::Atof(*PointNode->GetAttribute(TEXT("y")));
      FVector P(X, 0, Y);
      Poly.Points.Add(P);
      Sum += P;
    }

    if (Poly.Points.Num() > 0)
      Poly.Center = Sum / Poly.Points.Num();

    Polygons.Add(Poly);
  }

  // --- Leer enlaces ---
  const FXmlNode* LinksNode = RootNode->FindChildNode(TEXT("links"));
  if (LinksNode)
  {
    for (const FXmlNode* LinkNode : LinksNode->GetChildrenNodes())
    {
      const FXmlNode* StartNode = LinkNode->FindChildNode(TEXT("start"));
      const FXmlNode* EndNode = LinkNode->FindChildNode(TEXT("end"));
      if (!StartNode || !EndNode) continue;

      int32 StartPoly = FCString::Atoi(*StartNode->GetAttribute(TEXT("polygon")));
      int32 EndPoly = FCString::Atoi(*EndNode->GetAttribute(TEXT("polygon")));

      if (Polygons.IsValidIndex(StartPoly) && Polygons.IsValidIndex(EndPoly))
      {
        Polygons[StartPoly].LinkedPolygons.AddUnique(EndPoly);
        Polygons[EndPoly].LinkedPolygons.AddUnique(StartPoly);
      }
    }
  }

  UE_LOG(LogTemp, Log, TEXT("NavMesh cargada: %d polígonos."), Polygons.Num());
  for (const FNavPolygon& Poly : Polygons)
  {
    UE_LOG(LogTemp, Warning, TEXT("Polígono %d con %d puntos."), Poly.ID, Poly.Points.Num());
    for (const FVector& P : Poly.Points)
    {
      UE_LOG(LogTemp, Warning, TEXT("   Punto: X=%.2f, Y=%.2f, Z=%.2f"), P.X, P.Y, P.Z);
    }
  }
  return true;
}

void Navmesh::DrawDebug(AActor* Owner, UMaterialInterface* Material)
{
  if (!Owner) return;

  for (const FNavPolygon& Poly : Polygons)
  {
    for (int32 i = 0; i < Poly.Points.Num(); ++i)
    {
      const FVector& A = Poly.Points[i];
      const FVector& B = Poly.Points[(i + 1) % Poly.Points.Num()];
      DrawDebugLine(Owner->GetWorld(), A, B, FColor::Green, false, -1, 0, 2.f);
    }

    DrawDebugSphere(Owner->GetWorld(), Poly.Center, 10.f, 8, FColor::Yellow);
  }

  UE_LOG(LogTemp, Log, TEXT("NavMesh dibujada en escena."));
}

int32 Navmesh::GetPolygonIndexAtPoint(const FVector2D& Point) const
{
  for (int32 i = 0; i < Polygons.Num(); ++i)
  {
    if (IsPointInPolygon(Point, Polygons[i].Points))
      return i;
  }
  return -1;
}

bool Navmesh::IsPointInPolygon(const FVector2D& P, const TArray<FVector>& Poly) const
{
  int32 Crossings = 0;
  int32 Count = Poly.Num();
  for (int32 i = 0; i < Count; ++i)
  {
    FVector2D A(Poly[i].X, Poly[i].Z);
    FVector2D B(Poly[(i + 1) % Count].X, Poly[(i + 1) % Count].Z);

    if (((A.Y > P.Y) != (B.Y > P.Y)) &&
      (P.X < (B.X - A.X) * (P.Y - A.Y) / (B.Y - A.Y) + A.X))
    {
      Crossings++;
    }
  }
  return (Crossings % 2 == 1);
}

TArray<FVector> Navmesh::FindPath(const FVector& Start, const FVector& End)
{
  TArray<FVector> Path;

  int32 StartPoly = FindContainingPolygon(Start);
  int32 EndPoly = FindContainingPolygon(End);

  if (StartPoly == INDEX_NONE || EndPoly == INDEX_NONE)
    return Path;

  // BFS simple entre polígonos conectados
  TMap<int32, int32> CameFrom;
  TQueue<int32> Queue;
  Queue.Enqueue(StartPoly);
  CameFrom.Add(StartPoly, -1);

  bool bFound = false;

  while (!Queue.IsEmpty())
  {
    int32 Current;
    Queue.Dequeue(Current);

    if (Current == EndPoly)
    {
      bFound = true;
      break;
    }

    for (int32 Neighbor : Polygons[Current].LinkedPolygons)
    {
      if (!CameFrom.Contains(Neighbor))
      {
        CameFrom.Add(Neighbor, Current);
        Queue.Enqueue(Neighbor);
      }
    }
  }

  if (!bFound)
    return Path;

  // Reconstruir camino
  TArray<int32> PolyPath;
  int32 Current = EndPoly;
  while (Current != -1)
  {
    PolyPath.Insert(Current, 0);
    Current = CameFrom[Current];
  }

  // Convertir a puntos (centros de los polígonos)
  for (int32 PolyIndex : PolyPath)
  {
    Path.Add(Polygons[PolyIndex].Center);
  }

  return Path;
}

int32 Navmesh::FindContainingPolygon(const FVector& Point)
{
  for (int32 i = 0; i < Polygons.Num(); ++i)
  {
    if (IsPointInsidePolygon(Point, Polygons[i]))
      return i;
  }
  return INDEX_NONE;
}

bool Navmesh::IsPointInsidePolygon(const FVector& Point, const FNavPolygon& Poly)
{
  int32 Crossings = 0;
  int32 NumPoints = Poly.Points.Num();

  for (int32 i = 0; i < NumPoints; ++i)
  {
    const FVector& A = Poly.Points[i];
    const FVector& B = Poly.Points[(i + 1) % NumPoints];

    // Verifica si el segmento cruza la línea horizontal desde Point
    if (((A.Z > Point.Z) != (B.Z > Point.Z)) &&
      (Point.X < (B.X - A.X) * (Point.Z - A.Z) / (B.Z - A.Z) + A.X))
    {
      Crossings++;
    }
  }

  return (Crossings % 2 == 1);
}