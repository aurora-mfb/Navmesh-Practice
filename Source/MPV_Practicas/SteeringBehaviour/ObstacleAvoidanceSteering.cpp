// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleAvoidanceSteering.h"
#include "MPV_Practicas/AICharacter.h"

ObstacleAvoidanceSteering::ObstacleAvoidanceSteering(AAICharacter* _Character, TArray<FVector> _obstacles, float _charRad, float _MaxVelocity, float _maxAcceleration)
{
  m_pCharacter = _Character;
  m_obstacles = _obstacles;
  m_fCharRad = _charRad;
  m_fMaxVelocity = _MaxVelocity;
  m_fMaxAcceleration = _maxAcceleration;
}

void ObstacleAvoidanceSteering::DoSteering()
{
  FVector avoidanceForce = FVector::ZeroVector;

  FVector velocity = m_vCurrentVelocity;

  if (velocity.IsNearlyZero())
  {
    m_vLinearAcceleration = avoidanceForce;
    return;
  }

  FVector pp = m_pCharacter->GetActorLocation();

  FVector pathDir = (m_vTargetPos - pp).GetSafeNormal();

  FVector lp = pathDir * 800.f;

  float charRadius = m_fCharRad;
  float safetyMargin = 20.f;

  for (const FVector& obstacle : m_obstacles)
  {
    FVector po(obstacle.X, 0.f, obstacle.Y);
    FVector ro = po - pp;

    float proj = FVector::DotProduct(ro, pathDir);

    float maxProj = (m_vTargetPos - pp).Size() + 800.f;
    if (proj < 0 || proj > maxProj)
    {
      continue;
    }

    // Aquí está el cambio clave: calcular el punto más cercano desde la posición del personaje (pp)
    FVector cercano = pp + pathDir * proj;
    FVector diff = cercano - po;
    float dist = diff.Size();

    float combinedRadius = obstacle.Z + charRadius + safetyMargin;

    if (dist < combinedRadius)
    {
      float crossZ = FVector::CrossProduct(ro, lp).Z;

      FVector evadeDir;

      if (crossZ > 0)
      {
        evadeDir = FVector(ro.Z, 0.f, -ro.X); // 90 grados derecha
      }
      else if (crossZ < 0)
      {
        evadeDir = FVector(-ro.Z, 0.f, ro.X); // 90 grados izquierda
      }
      else
      {
        evadeDir = FVector(ro.Z, 0.f, -ro.X); // caso paralelo, girar derecha
      }

      evadeDir = evadeDir.GetSafeNormal();

      float penetration = combinedRadius - dist;
      float strength = FMath::Clamp(penetration / combinedRadius, 0.f, 1.f);

      avoidanceForce += evadeDir * strength * m_fMaxAcceleration;
    }
  }

  avoidanceForce = avoidanceForce.GetClampedToMaxSize(m_fMaxAcceleration);

  m_vLinearAcceleration = avoidanceForce;
}

FVector ObstacleAvoidanceSteering::GetLinearAcceleration() const
{
  return m_vLinearAcceleration;
}

float ObstacleAvoidanceSteering::GetAngularAcceleration() const
{
  return m_fAngularAcceleration;
}

void ObstacleAvoidanceSteering::DrawDebug()
{if (!m_pCharacter) return;

  UWorld* world = m_pCharacter->GetWorld();
  if (!world) return;

  FVector charPos = m_pCharacter->GetActorLocation();
  FVector vAhead = charPos + m_vCurrentVelocity.GetSafeNormal() * MAX_SEE_AHEAD;
  FVector vHalfAhead = charPos + m_vCurrentVelocity.GetSafeNormal() * MAX_SEE_AHEAD * 0.5;

  // 1. Dibujar línea desde el personaje hacia "ahead" y "halfAhead"
  DrawDebugLine(world, charPos, vAhead, FColor::Blue, false, -1, 0, 1.5f);
  DrawDebugLine(world, charPos, vHalfAhead, FColor::Cyan, false, -1, 0, 1.5f);

  // 2. Dibujar los obstáculos
  for (const FVector& obstacle : m_obstacles)
  {
    FVector pos = FVector(obstacle.X, 0.0f, obstacle.Y);  // Asumiendo posición en X,Z, radio en Y
    float radius = obstacle.Z;

    DrawDebugSphere(world, pos, radius, 16, FColor::Red, false, -1, 0, 1.5f);
  }

  // 3. (Opcional) Dibujar el obstáculo más amenazante (en morado)
  FVector mostThreateningObstacle = FVector::ZeroVector;
  float minDistance = FLT_MAX;

  for (const FVector& obstacle : m_obstacles)
  {
    float obstacleRadius = obstacle.Y;
    float combinedRadius = obstacleRadius + m_fCharRad;

    bool collision = FVector::Dist(vAhead, obstacle) <= combinedRadius || FVector::Dist(vHalfAhead, obstacle) <= combinedRadius;
    float distance = FVector::Dist(vAhead, obstacle);

    if (collision && distance < minDistance)
    {
      minDistance = distance;
      mostThreateningObstacle = obstacle;
    }
  }

  if (mostThreateningObstacle != FVector::ZeroVector)
  {
    DrawDebugSphere(world, mostThreateningObstacle, mostThreateningObstacle.Y, 16, FColor::Purple, false, -1, 0, 2.5f);
  }
}

void ObstacleAvoidanceSteering::SetCurrentVelocity(FVector _CurrentVel)
{
  m_vCurrentVelocity = _CurrentVel;
}

void ObstacleAvoidanceSteering::SetTargetPos(FVector _targetPos)
{
  m_vTargetPos = _targetPos;
}
