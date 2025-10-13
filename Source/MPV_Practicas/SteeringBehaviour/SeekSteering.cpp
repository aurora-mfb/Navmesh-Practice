// Fill out your copyright notice in the Description page of Project Settings.


#include "SeekSteering.h"
#include "MPV_Practicas/AICharacter.h"
#include "MPV_Practicas/debug/debugdraw.h"

SeekSteering::SeekSteering(AAICharacter* _pCharacter, FVector _vTargetPos, float _fMaxVelocity, float _fMaxForce)
{
  m_pCharacter = _pCharacter;
  m_vTargetPos = _vTargetPos;
  m_fMaxVelocity = _fMaxVelocity;
  m_fMaxAcceleration = _fMaxForce;
}

void SeekSteering::DoSteering()
{
  // 1. Calculamos la direccion:
  FVector vDirToTarget = m_vTargetPos - m_pCharacter->GetActorLocation();
  FVector vNormalDirToTarget = (vDirToTarget).GetSafeNormal();

  // 2. Dirreccion y distancia del target? si esta cerca velocidad es 0.
  float ArrivalRadius = 10.f;
  float fDistToTarget = vDirToTarget.Size();

  if (fDistToTarget < ArrivalRadius)
  {
    m_vDesiredVelocity = FVector::ZeroVector;
  }
  else
  {
    m_vDesiredVelocity = vNormalDirToTarget * m_fMaxVelocity;
  }

  // 3. Aceleracion con esa velocidad:
  FVector vSteering = m_vDesiredVelocity - m_pCharacter->GetCurrentVelocity();

  // 4. Añadimos fuerzas:
  float length = vSteering.Size();
  if (length > m_fMaxAcceleration)
  {
    vSteering = vSteering.GetSafeNormal() * m_fMaxAcceleration;
  }

  // 5. Devolvemos la nueva aceleracion:
  m_vLinearAcceleration = vSteering;
}

FVector SeekSteering::GetLinearAcceleration() const
{
  return m_vLinearAcceleration;
}

float SeekSteering::GetAngularAcceleration() const
{
  return 0.0f;
}

void SeekSteering::DrawDebug()
{
  SetArrow(m_pCharacter, TEXT("DesiredVelocityArrow"), m_vDesiredVelocity, m_vDesiredVelocity.Size());
  SetArrow(m_pCharacter, TEXT("LinearAccelerationArrow"), m_vLinearAcceleration, m_vLinearAcceleration.Size());
}

void SeekSteering::SetTargetPos(FVector _targetPos)
{
  m_vTargetPos = _targetPos;
}


