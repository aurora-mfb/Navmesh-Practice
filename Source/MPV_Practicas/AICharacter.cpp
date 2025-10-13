// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "params/params.h"
#include "debug/debugdraw.h"
#include "XmlFile.h"
#include "PaperSpriteActor.h"

// Sets default values 
AAICharacter::AAICharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DesiredVelocityArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DesiredVelocityArrow"));
	DesiredVelocityArrow->SetupAttachment(RootComponent);
	DesiredVelocityArrow->SetHiddenInGame(false);

	LinearAccelerationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LinearAccelerationArrow"));
	LinearAccelerationArrow->SetupAttachment(RootComponent);
	LinearAccelerationArrow->SetHiddenInGame(false);
	LinearAccelerationArrow->SetArrowColor(FLinearColor::Green);

}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	ReadParams("params.xml", m_params);
	//ReadPath("path.xml", m_path);
	ReadObstaclesFromFile("obstacles.xml", m_obstacles);
	ReadGridAndCostsFromFile("grid_map.xml", m_tileCosts, m_gridMap);

	ReadNavMesh("hola.xml");

	// --- Nueva parte: cargar NavMesh ---
	m_pNavMesh = new Navmesh();
	if (m_pNavMesh && m_pNavMesh->InitializeNavMesh(TEXT("navmesh.xml")))
	{
		m_pNavMesh->DrawDebug(this);
	}

	m_vCurrentVelocity = FVector::ZeroVector;

	m_pSeekBehaviour = new SeekSteering(this, m_params.targetPosition, m_params.max_velocity, m_params.max_acceleration);
	m_pArriveBehaviour = new ArriveSteering(this, m_params.targetPosition, m_params.max_velocity, m_params.max_acceleration, m_params.dest_radius);
	m_pObstacleAvoidanceBehaviour = new ObstacleAvoidanceSteering(this, m_obstacles, m_params.char_rad, m_params.max_velocity, m_params.max_acceleration);
	m_pPathFollowBehaviour = new PathFollowingSteering(this, m_path, m_params.look_ahead, m_pSeekBehaviour);

	m_pPathfinder = new Pathfinder(m_tileCosts, m_gridMap);

}

void AAICharacter::ReadNavMesh(const FString& FileName)
{
	/*FString FullPath = FPaths::ProjectContentDir() + FileName;
	FString FileContent;

	if (!FFileHelper::LoadFileToString(FileContent, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("No se pudo leer el navmesh: %s"), *FullPath);
		return;
	}

	FXmlFile XmlFile(FileContent, EConstructMethod::ConstructFromBuffer);
	if (!XmlFile.IsValid()) return;

	const FXmlNode* RootNode = XmlFile.GetRootNode();
	if (!RootNode) return;

	const FXmlNode* PolygonsNode = RootNode->FindChildNode(TEXT("polygons"));
	if (PolygonsNode)
	{
		for (const FXmlNode* PolygonNode : PolygonsNode->GetChildrenNodes())
		{
			if (PolygonNode->GetTag() == TEXT("polygon"))
			{
				FNavPolygon Polygon;
				for (const FXmlNode* PointNode : PolygonNode->GetChildrenNodes())
				{
					if (PointNode->GetTag() == TEXT("point"))
					{
						float X = FCString::Atof(*PointNode->GetAttribute(TEXT("x")));
						float Y = FCString::Atof(*PointNode->GetAttribute(TEXT("y")));
						Polygon.Points.Add(FVector2D(X, Y));
					}
				}
				m_navPolygons.Add(Polygon);
			}
		}
	}

	const FXmlNode* LinksNode = RootNode->FindChildNode(TEXT("links"));
	if (LinksNode)
	{
		for (const FXmlNode* LinkNode : LinksNode->GetChildrenNodes())
		{
			if (LinkNode->GetTag() == TEXT("link"))
			{
				const FXmlNode* StartNode = LinkNode->FindChildNode(TEXT("start"));
				const FXmlNode* EndNode = LinkNode->FindChildNode(TEXT("end"));

				if (StartNode && EndNode)
				{
					FNavLink Link;
					Link.StartPolygonIndex = FCString::Atoi(*StartNode->GetAttribute(TEXT("polygon")));
					Link.StartEdgeStart = FCString::Atoi(*StartNode->GetAttribute(TEXT("edgestart")));
					Link.StartEdgeEnd = FCString::Atoi(*StartNode->GetAttribute(TEXT("edgeend")));
					Link.EndPolygonIndex = FCString::Atoi(*EndNode->GetAttribute(TEXT("polygon")));
					Link.EndEdgeStart = FCString::Atoi(*EndNode->GetAttribute(TEXT("edgestart")));
					Link.EndEdgeEnd = FCString::Atoi(*EndNode->GetAttribute(TEXT("edgeend")));

					m_navLinks.Add(Link);
				}
			}
		}
	}*/
}

void AAICharacter::DrawNavMesh()
{
	//UWorld* World = GetWorld();
	//if (!World) return;

	//float Height = GetActorLocation().Z;

	//for (const FNavPolygon& Polygon : m_navPolygons)
	//{
	//	for (const FVector2D& Pt : Polygon.Points)
	//	{
	//		FVector P(Pt.X, Pt.Y, Height); // ? CORREGIDO
	//		DrawDebugPoint(World, P, 10.f, FColor::Yellow, true);
	//	}
	//}

	//// Dibuja polígonos
	//for (const FNavPolygon& Polygon : m_navPolygons)
	//{
	//	int32 NumPoints = Polygon.Points.Num();
	//	for (int32 i = 0; i < NumPoints; ++i)
	//	{
	//		FVector2D Start2D = Polygon.Points[i];
	//		FVector2D End2D = Polygon.Points[(i + 1) % NumPoints];

	//		FVector Start(Start2D.X, Height, Start2D.Y);
	//		FVector End(End2D.X, Height, End2D.Y);

	//		DrawDebugLine(World, Start, End, FColor::Green, true, -1.f, 0, 5.f);
	//	}
	//}

	//// Dibuja enlaces (links)
	//for (const FNavLink& Link : m_navLinks)
	//{
	//	if (m_navPolygons.IsValidIndex(Link.StartPolygonIndex) && m_navPolygons.IsValidIndex(Link.EndPolygonIndex))
	//	{
	//		const FNavPolygon& StartPoly = m_navPolygons[Link.StartPolygonIndex];
	//		const FNavPolygon& EndPoly = m_navPolygons[Link.EndPolygonIndex];

	//		FVector2D StartEdgeStart = StartPoly.Points[Link.StartEdgeStart];
	//		FVector2D StartEdgeEnd = StartPoly.Points[Link.StartEdgeEnd];
	//		FVector StartMid(
	//			(StartEdgeStart.X + StartEdgeEnd.X) * 0.5f,
	//			Height,
	//			(StartEdgeStart.Y + StartEdgeEnd.Y) * 0.5f);

	//		FVector2D EndEdgeStart = EndPoly.Points[Link.EndEdgeStart];
	//		FVector2D EndEdgeEnd = EndPoly.Points[Link.EndEdgeEnd];
	//		FVector EndMid(
	//			(EndEdgeStart.X + EndEdgeEnd.X) * 0.5f,
	//			Height,
	//			(EndEdgeStart.Y + EndEdgeEnd.Y) * 0.5f);

	//		DrawDebugLine(World, StartMid, EndMid, FColor::Red, true, -1.f, 0, 3.f);
	//	}
	//}
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pNavMesh)
	{
		m_pNavMesh->DrawDebug(this, NavmeshMaterial);
	}

	// Solo moverse si el path está activo
	if (bShouldFollowPath && m_pPathFollowBehaviour)
	{
		m_pPathFollowBehaviour->DoSteering();
		FVector steering = m_pPathFollowBehaviour->GetLinearAcceleration();

		m_vCurrentVelocity += steering * DeltaTime;
		m_vCurrentVelocity = m_vCurrentVelocity.GetClampedToMaxSize(m_params.max_velocity);

		SetActorLocation(GetActorLocation() + m_vCurrentVelocity * DeltaTime);
	}
}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacter::OnClickedLeft(const FVector& mousePosition)
{
	// Teletransporta al personaje al punto clicado
	SetActorLocation(mousePosition);
	m_vCurrentVelocity = FVector::ZeroVector;

	// Guarda el punto de inicio
	m_startPoint = mousePosition;

	// Apaga movimiento hasta que haya un path nuevo
	bShouldFollowPath = false;

	DrawDebugSphere(GetWorld(), mousePosition, 10, 8, FColor::Blue, false, 5);
}

void AAICharacter::OnClickedRight(const FVector& mousePosition)
{
	if (!m_pNavMesh) return;

	m_endPoint = mousePosition;
	DrawDebugSphere(GetWorld(), mousePosition, 10, 8, FColor::Red, false, 5);

	// Calcula path sobre el navmesh
	TArray<FVector> Path = m_pNavMesh->FindPath(m_startPoint, m_endPoint);

	// Actualiza PathFollowing
	if (m_pPathFollowBehaviour)
	{
		m_pPathFollowBehaviour->SetPath(Path);
	}

	// Activa el movimiento
	bShouldFollowPath = true;

	// Debug del path
	for (int i = 0; i < Path.Num() - 1; ++i)
	{
		DrawDebugLine(GetWorld(), Path[i], Path[i + 1], FColor::Cyan, true, 10, 0, 3.f);
	}
}

void AAICharacter::DrawDebug()
{
	TArray<FVector> Points =
	{
		FVector(0.f, 0.f, 0.f),
		FVector(100.f, 0.f, 0.f),
		FVector(100.f, 0.f, 100.f),
		FVector(100.f, 0.f, 100.f),
		FVector(0.f, 0.f, 100.f)
	};

	SetPath(this, TEXT("follow_path"), TEXT("path"), Points, 5.0f, PathMaterial);

	SetCircle(this, TEXT("targetPosition"), m_params.targetPosition, 20.0f);
	FVector dir(cos(FMath::DegreesToRadians(m_params.targetRotation)), 0.0f, sin(FMath::DegreesToRadians(m_params.targetRotation)));
	SetArrow(this, TEXT("targetRotation"), dir, 80.0f);

	TArray<TArray<FVector>> Polygons = {
		{ FVector(0.f, 0.f, 0.f), FVector(100.f, 0.f, 0.f), FVector(100.f, 0.f, 100.0f), FVector(0.f, 0.f, 100.0f) },
		{ FVector(100.f, 0.f, 0.f), FVector(200.f, 0.f, 0.f), FVector(200.f, 0.f, 100.0f) }
	};
	SetPolygons(this, TEXT("navmesh"), TEXT("mesh"), Polygons, NavmeshMaterial);
}

FVector AAICharacter::GetCurrentVelocity()
{
	return m_vCurrentVelocity;
}
