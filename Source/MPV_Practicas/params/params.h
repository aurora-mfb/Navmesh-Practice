#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <CoreMinimal.h>

struct Params
{
    FVector targetPosition;
    float max_velocity;
    float max_acceleration;
    float dest_radius;

    float targetRotation;
    float max_angular_velocity;
    float max_angular_acceleration;
    float angular_arrive_rad;

    float look_ahead;
    float time_ahead;

    float char_rad;
};

bool ReadParams(const char* filename, Params& params);

void ReadPath(const FString& pathFile, TArray<FVector>& outPath);

void ReadObstaclesFromFile(const FString& FilePath, TArray<FVector>& outObstacles);

void ReadGridAndCostsFromFile(const FString& FilePath, TMap<TCHAR, float>& outTileCosts, TArray<FString>& outGridRows);

#endif