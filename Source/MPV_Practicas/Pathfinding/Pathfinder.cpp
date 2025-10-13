// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinder.h"

Pathfinder::Pathfinder(TMap<TCHAR, float> _tileCosts, TArray<FString> _gridMap)
{
  m_tileCosts = _tileCosts;
  m_gridMap = _gridMap;
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::FindPath()
{
  int32 NumRows = m_gridMap.Num();
  int32 NumCols = m_gridMap[0].Len();

  // 1. Crear matriz de nodos
  TArray<TArray<FNode>> nodes;
  nodes.SetNum(NumRows);
  for (int32 r = 0; r < NumRows; r++)
  {
    nodes[r].SetNum(NumCols);
    for (int32 c = 0; c < NumCols; c++)
    {
      nodes[r][c].Row = r;
      nodes[r][c].Col = c;
      nodes[r][c].GCost = FLT_MAX;
      nodes[r][c].HCost = FMath::Abs(NumRows - 1 - r) + FMath::Abs(NumCols - 1 - c);
      nodes[r][c].Parent = nullptr;
    }
  }

  if (StartCell.X < 0 || GoalCell.X < 0)
  {
    UE_LOG(LogTemp, Error, TEXT("Start or Goal cell not set!"));
    return;
  }

  FNode* startNode = &nodes[StartCell.X][StartCell.Y];
  FNode* goalNode = &nodes[GoalCell.X][GoalCell.Y];

  startNode->GCost = 0;

  TArray<FNode*> openList;
  TSet<FNode*> closedSet;

  openList.Add(startNode);

  while (openList.Num() > 0)
  {
    openList.Sort([](const FNode& A, const FNode& B) {
      return A.FCost() < B.FCost();
      });

    FNode* currentNode = openList[0];
    openList.RemoveAt(0);
    closedSet.Add(currentNode);

    if (currentNode == goalNode)
    {
      TArray<FNode> path;
      FNode* pathNode = currentNode;
      while (pathNode)
      {
        path.Insert(*pathNode, 0);
        pathNode = pathNode->Parent;
      }
      CurrentPath = path;

      FString pathStr = TEXT("Camino encontrado: ");
      for (FNode node : path)
      {
        pathStr += FString::Printf(TEXT("(%d,%d) "), node.Row, node.Col);
      }
      UE_LOG(LogTemp, Warning, TEXT("%s"), *pathStr);

      return;
    }

    TArray<FIntPoint> directions = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    for (const FIntPoint& dir : directions)
    {
      int32 newRow = currentNode->Row + dir.X;
      int32 newCol = currentNode->Col + dir.Y;

      if (newRow < 0 || newRow >= NumRows || newCol < 0 || newCol >= NumCols)
        continue;

      FNode* neighbor = &nodes[newRow][newCol];
      TCHAR tileChar = m_gridMap[newRow][newCol];

      float tileCost = m_tileCosts.Contains(tileChar) ? m_tileCosts[tileChar] : 9999.f;
      if (tileCost >= 9999.f) continue;

      if (closedSet.Contains(neighbor)) continue;

      float tentativeG = currentNode->GCost + tileCost;

      if (tentativeG < neighbor->GCost)
      {
        neighbor->Parent = currentNode;
        neighbor->GCost = tentativeG;
        neighbor->HCost = FMath::Abs(goalNode->Row - newRow) + FMath::Abs(goalNode->Col - newCol);

        if (!openList.Contains(neighbor))
        {
          openList.Add(neighbor);
        }
      }
    }
  }

  CurrentPath.Empty();
  return;
}

void Pathfinder::DrawDebug()
{
  if (!GWorld || m_gridMap.Num() == 0) return;

  float MinX = -855.f;
  float MaxX = 855.f;
  float MinZ = -455.f;
  float MaxZ = 455.f;

  float WidthX = MaxX - MinX;
  float HeightZ = MaxZ - MinZ;

  int32 NumRows = m_gridMap.Num();
  int32 NumCols = m_gridMap[0].Len();

  float TileSizeX = WidthX / NumCols;
  float TileSizeZ = HeightZ / NumRows;

  for (int32 row = 0; row < NumRows; ++row)
  {
    const FString& RowStr = m_gridMap[row];
    for (int32 col = 0; col < NumCols; ++col)
    {
      TCHAR TileChar = RowStr[col];
      float Cost = m_tileCosts.Contains(TileChar) ? m_tileCosts[TileChar] : 9999.f;

      float PosX = MinX + TileSizeX * col + TileSizeX / 2.f;
      float PosZ = MaxZ - (TileSizeZ * row + TileSizeZ / 2.f);

      FVector Pos = FVector(PosX, 0.f, PosZ);

      FVector Extent = FVector(TileSizeX / 2.f, 5.f, TileSizeZ / 2.f);

      FColor Color = FColor::Green;
      if (Cost >= 9999.f) Color = FColor::Black; // Obstáculo
      else if (Cost > 4.f) Color = FColor::Red;
      else if (Cost > 2.f) Color = FColor::Orange;
      else if (Cost > 1.f) Color = FColor::Yellow;

      DrawDebugSolidBox(GWorld, Pos, Extent, Color, false, -1, 0);

      DrawDebugString(GWorld, Pos + FVector(0, 15.f, 0), FString::Printf(TEXT("%c"), TileChar), nullptr, FColor::White, 0.f, false);
    }
  }

  // Dibuja el camino encontrado si existe
  if (CurrentPath.Num() > 0)
  {
    for (int32 i = 0; i < CurrentPath.Num(); i++)
    {
      FNode node = CurrentPath[i];

      // Convierte fila y columna a posición física
      float PosX = MinX + TileSizeX * node.Col + TileSizeX / 2.f;
      float PosZ = MaxZ - (TileSizeZ * node.Row + TileSizeZ / 2.f);
      FVector Pos = FVector(PosX, 0.f, PosZ);

      // Dibuja una caja o esfera azul en cada nodo del path
      DrawDebugSolidBox(GWorld, Pos, FVector(TileSizeX / 2.f, 10.f, TileSizeZ / 2.f), FColor::Blue, false, -1.f, 0);

      // Opcional: dibuja líneas entre nodos consecutivos
      if (i > 0)
      {
        FNode prevNode = CurrentPath[i - 1];
        FVector PrevPos = FVector(
          MinX + TileSizeX * prevNode.Col + TileSizeX / 2.f,
          0.f,
          MaxZ - (TileSizeZ * prevNode.Row + TileSizeZ / 2.f));

        DrawDebugLine(GWorld, PrevPos, Pos, FColor::Blue, false, -1.f, 0, 5.f);
      }
    }
  }
}

FIntPoint Pathfinder::WorldToGrid(const FVector& WorldPos)
{
  float MinX = -855.f;
  float MaxX = 855.f;
  float MinZ = -455.f;
  float MaxZ = 455.f;

  int32 NumRows = m_gridMap.Num();
  int32 NumCols = m_gridMap[0].Len();

  float TileSizeX = (MaxX - MinX) / NumCols;
  float TileSizeZ = (MaxZ - MinZ) / NumRows;

  int32 col = FMath::Clamp(int32((WorldPos.X - MinX) / TileSizeX), 0, NumCols - 1);
  int32 row = FMath::Clamp(int32((MaxZ - WorldPos.Z) / TileSizeZ), 0, NumRows - 1);

  return FIntPoint(row, col);
}

void Pathfinder::SetStartFromWorldPos(const FVector& WorldPos)
{
  StartCell = WorldToGrid(WorldPos);
  UE_LOG(LogTemp, Warning, TEXT("Start cell set to (%d, %d)"), StartCell.X, StartCell.Y);
}

void Pathfinder::SetGoalFromWorldPos(const FVector& WorldPos)
{
  GoalCell = WorldToGrid(WorldPos);
  UE_LOG(LogTemp, Warning, TEXT("Goal cell set to (%d, %d)"), GoalCell.X, GoalCell.Y);
}