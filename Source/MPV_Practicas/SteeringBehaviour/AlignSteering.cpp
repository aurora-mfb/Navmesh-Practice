// Fill out your copyright notice in the Description page of Project Settings.


#include "AlignSteering.h"
#include "MPV_Practicas/AICharacter.h"

AlignSteering::AlignSteering(AAICharacter* _pCharacter, FVector _vTargetPos, float _fMaxVelocity, float _fMaxForce, float _fArriveRadious)
{
}

void AlignSteering::DoSteering()
{
  //// 1. Obtener la rotación actual del actor
  //float CurrentRotation = m_pCharacter->GetActorRotation().Yaw;

  //// 2. Calcular la velocidad angular actual (grados/seg)
  //float DeltaAngle = FMath::FindDeltaAngleDegrees(PreviousRotation, CurrentRotation);
  //float CurrentAngularVelocity = DeltaAngle / DeltaTime;

  //// 3. Calcular diferencia con el ángulo objetivo
  //float AngleToTarget = FMath::FindDeltaAngleDegrees(CurrentRotation, TargetRotation); // [-180, 180]

  //// 4. Si estamos dentro del radio de llegada, desaceleramos a 0
  //float DesiredAngularVelocity = 0.f;
  //if (FMath::Abs(AngleToTarget) >= AngularArrivalRadius)
  //{
  //  float AngleRatio = FMath::Abs(AngleToTarget) / 180.f;
  //  DesiredAngularVelocity = FMath::Clamp(AngleRatio * MaxAngularVelocity, 0.f, MaxAngularVelocity);
  //  DesiredAngularVelocity *= FMath::Sign(AngleToTarget);
  //}

  //// 5. Steering angular = velocidad deseada - actual
  //float AngularSteering = DesiredAngularVelocity - CurrentAngularVelocity;

  //// 6. Limitar aceleración angular
  //if (FMath::Abs(AngularSteering) > MaxAngularAcceleration)
  //{
  //  AngularSteering = FMath::Sign(AngularSteering) * MaxAngularAcceleration;
  //}

  //// 7. Aplicar resultado
  //m_fAngularAcceleration = AngularSteering;

  //// 8. Guardar rotación actual para el próximo frame
  //PreviousRotation = CurrentRotation;
}

FVector AlignSteering::GetLinearAcceleration() const
{
  return FVector();
}

float AlignSteering::GetAngularAcceleration() const
{
  return m_fAngularAcceleration;
}

void AlignSteering::DrawDebug()
{
}