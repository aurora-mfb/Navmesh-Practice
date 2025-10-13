// Fill out your copyright notice in the Description page of Project Settings.


#include "ArriveSteering.h"
#include "MPV_Practicas/AICharacter.h"
#include "MPV_Practicas/debug/debugdraw.h"

ArriveSteering::ArriveSteering(AAICharacter* _pCharacter, FVector _vTargetPos, float _fMaxVelocity, float _fMaxForce, float _fDestRadius)
{
  m_pCharacter = _pCharacter;
  m_vTargetPos = _vTargetPos;
  m_fMaxVelocity = _fMaxVelocity;
  m_fMaxAcceleration = _fMaxForce;
  m_fDestRadius = _fDestRadius;
}

void ArriveSteering::DoSteering()
{
  FVector vDirToTarget = m_vTargetPos - m_pCharacter->GetActorLocation();
  FVector vNormalDirToTarget = vDirToTarget.GetSafeNormal();
  float fDistToTarget = vDirToTarget.Size();

  float fTargetRadius = 100.f;

  if (fDistToTarget < fTargetRadius) {
    // Si estoy suficientemente cerca, detenerme
    m_vDesiredVelocity = FVector::ZeroVector;
  }
  else if (fDistToTarget < m_fDestRadius) {
    // Desaceleración suave
    m_vDesiredVelocity = vNormalDirToTarget * m_fMaxVelocity * (fDistToTarget / m_fDestRadius);
  }
  else {
    // Velocidad máxima
    m_vDesiredVelocity = vNormalDirToTarget * m_fMaxVelocity;
  }

  m_vDesiredVelocity = FVector::ZeroVector;

  if (fDistToTarget > fTargetRadius)
  {
    float speed = m_fMaxVelocity;

    // Si estamos dentro del radio de desaceleración, reducimos la velocidad
    if (fDistToTarget < m_fDestRadius)
    {
      float factor = fDistToTarget / m_fDestRadius;  // Interpolación lineal [0..1]
      speed *= factor;
    }

    m_vDesiredVelocity = vNormalDirToTarget * speed;
  }

  FVector vSteering = m_vDesiredVelocity - m_pCharacter->GetCurrentVelocity();

  // 4. Añadimos fuerzas:
  float length = vSteering.Size();
  if (length > m_fMaxAcceleration)
  {
    vSteering = vSteering.GetSafeNormal() * m_fMaxAcceleration;
  }

  m_vLinearAcceleration = vSteering;
}

FVector ArriveSteering::GetLinearAcceleration() const
{
  return m_vLinearAcceleration;
}

float ArriveSteering::GetAngularAcceleration() const
{
  return 0.0f;
}

void ArriveSteering::DrawDebug()
{
  if (!m_pCharacter) return;

  UWorld* world = m_pCharacter->GetWorld();
  if (!world) return;

  // 1. Dibujar el punto de destino
  DrawDebugSphere(world, m_vTargetPos, 20.f, 12, FColor::Green, false, -1, 0, 2.0f);  // Target position

  // 2. Dibujar el radio de desaceleración
  DrawDebugCircle(world, m_vTargetPos, m_fDestRadius, 32, FColor::Yellow, false, -1, 0, 2.0f, FVector(1, 0, 0), FVector(0, 1, 0), false);

  // 3. Dibujar el radio de llegada (parada)
  float fTargetRadius = 100.f;
  DrawDebugCircle(world, m_vTargetPos, fTargetRadius, 32, FColor::Red, false, -1, 0, 1.5f, FVector(1, 0, 0), FVector(0, 1, 0), false);

  // 4. Línea del personaje al objetivo
  FVector charPos = m_pCharacter->GetActorLocation();
  DrawDebugLine(world, charPos, m_vTargetPos, FColor::Blue, false, -1, 0, 1.5f);
}
