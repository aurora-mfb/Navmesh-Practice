// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowingSteering.h"
#include "MPV_Practicas/AICharacter.h"

PathFollowingSteering::PathFollowingSteering(AAICharacter* _Character, TArray<FVector> _Path, float _LookAhead, SeekSteering* _seekSteering)
{
  m_pCharacter = _Character;
  m_path = _Path;
  m_fLookAhead = _LookAhead;
  m_pSeekBehaviour = _seekSteering;
}

void PathFollowingSteering::DoSteering()
{
  DrawDebug();
  GetClosestPoint();

  m_pSeekBehaviour->SetTargetPos(m_vCurrentTarget);

  m_pSeekBehaviour->DoSteering();

  m_vLinearAcceleration = m_pSeekBehaviour->GetLinearAcceleration();
}

void PathFollowingSteering::GetClosestPoint()
{
  FVector currentPos = m_pCharacter->GetActorLocation();

  float minDistSquared = FLT_MAX;
  FVector closestPoint = FVector::ZeroVector;
  int closestSegmentIndex = -1;
  float tClosest = 0.f;

  for (int i = 0; i < m_path.Num() - 1; ++i)
  {
    FVector A = m_path[i];
    FVector B = m_path[i + 1];
    FVector AB = B - A;
    FVector AP = currentPos - A;

    float abLengthSquared = AB.SizeSquared(); // Esto sería ||AB||2, la longitud al cuadrado.
    if (abLengthSquared == 0)
      continue; // Es invalido el segmento.

    // Proyección escalar t
    float t = FVector::DotProduct(AP, AB) / abLengthSquared;

    // Clamp t entre 0 y 1 para limitar al segmento
    t = FMath::Clamp(t, 0.f, 1.f);

    // Punto más cercano en el segmento
    FVector pointOnSegment = A + t * AB;

    // Distancia al personaje (en 3D o 2D plano XZ)
    float dx = currentPos.X - pointOnSegment.X;
    float dz = currentPos.Z - pointOnSegment.Z;
    float distSquared = dx * dx + dz * dz;

    if (distSquared < minDistSquared)
    {
      minDistSquared = distSquared;
      closestPoint = pointOnSegment;
      closestSegmentIndex = i;
      tClosest = t;
    }
  }

  // Paso 2: Avanzar el lookAhead desde el punto más cercano
  FVector target = closestPoint;
  float remaining = m_fLookAhead;

  int segIdx = closestSegmentIndex;

  // Calcula cuánto queda de distancia en el segmento actual desde closestPoint hasta B
  while (remaining > 0.f && segIdx < m_path.Num() - 1)
  {
    FVector A = m_path[segIdx];
    FVector B = m_path[segIdx + 1];
    FVector AB = B - A;
    float segmentLength = AB.Size();

    // Desde closestPoint hasta B en este segmento
    float segmentRemaining;

    if (segIdx == closestSegmentIndex)
    {
      segmentRemaining = segmentLength * (1.f - tClosest);
      A = closestPoint; // empezamos desde el punto más cercano
      AB = B - A;
    }
    else
    {
      segmentRemaining = segmentLength;
      A = m_path[segIdx];
      AB = B - A;
    }

    if (remaining < segmentRemaining)
    {
      FVector ABdir = AB.GetSafeNormal();
      target = A + ABdir * remaining;
      remaining = 0.f;
    }
    else
    {
      remaining -= segmentRemaining;
      segIdx++;
      target = B;
    }
  }

  m_vCurrentTarget = target;
  m_iClosestSegmentIndex = closestSegmentIndex;
}

FVector PathFollowingSteering::GetTargetPos()
{
    return m_vCurrentTarget;
}

FVector PathFollowingSteering::GetLinearAcceleration() const
{
  return m_vLinearAcceleration;
}

float PathFollowingSteering::GetAngularAcceleration() const
{
  return 0.0f;
}

void PathFollowingSteering::DrawDebug()
{
  UWorld* World = m_pCharacter->GetWorld();
  if (!World) return;

  // 1. Dibuja líneas del path
  for (int i = 0; i < m_path.Num() - 1; ++i)
  {
    DrawDebugLine(
      World,
      m_path[i],
      m_path[i + 1],
      FColor::Blue,
      false,
      -1.f,
      0,
      5.f
    );
  }

  // 2. Dibuja una esfera en el punto más cercano
  DrawDebugSphere(
    World,
    m_vCurrentTarget,
    25.f,
    12,
    FColor::Red,
    false,
    -1.f,
    0,
    2.f
  );
}

void PathFollowingSteering::SetPath(const TArray<FVector>& path)
{
  m_path = path;
  m_iClosestSegmentIndex = 0;
}
