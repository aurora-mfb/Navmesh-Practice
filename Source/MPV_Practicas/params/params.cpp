#include "params.h"

#include "XmlFile.h"
#include "Misc/DefaultValueHelper.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"


bool ReadParams(const char* filename, Params& params)
{
	FString CurrentDirectory = FPlatformProcess::GetCurrentWorkingDirectory();

	// Log or use the current working directory
	UE_LOG(LogTemp, Log, TEXT("Current working directory: %s"), *CurrentDirectory);

	FString ContentFolderDir = FPaths::ProjectContentDir();

	//FString FilePath(TEXT("./params.xml"));
	FString params_path = filename;
	FString FilePath = FPaths::Combine(*ContentFolderDir, *params_path);
	UE_LOG(LogTemp, Log, TEXT("Params Path: %s"), *FilePath);

	
	FXmlFile MyXml(FilePath, EConstructMethod::ConstructFromFile);
	
	if (MyXml.GetRootNode())
	{
		const FXmlNode* RootNode = MyXml.GetRootNode();
	
		const FString MyChildTag("params");
		const FXmlNode* MyChildNode = RootNode->FindChildNode(MyChildTag);

		const FXmlNode* paramElem = MyChildNode->FindChildNode(TEXT("max_velocity"));
		FString value;
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.max_velocity);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("max_acceleration"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.max_acceleration);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("targetPosition"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("x");
			float x;
			FDefaultValueHelper::ParseFloat(value, x);
			params.targetPosition.X = x;
			value = paramElem->GetAttribute("z");
			float z;
			FDefaultValueHelper::ParseFloat(value, z);
			params.targetPosition.Z = z;
			params.targetPosition.Y = 0.0f;
		}

		paramElem = MyChildNode->FindChildNode(TEXT("arrive_radius"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.dest_radius);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("max_angular_velocity"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.max_angular_velocity);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("max_angular_acceleration"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.max_angular_acceleration);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("ngular_arrive_radius"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.angular_arrive_rad);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("targetRotation"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.targetRotation);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("look_ahead"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.look_ahead);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("time_ahead"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.time_ahead);

		}

		paramElem = MyChildNode->FindChildNode(TEXT("char_radius"));
		if (paramElem)
		{
			value = paramElem->GetAttribute("value");
			FDefaultValueHelper::ParseFloat(value, params.char_rad);

		}
	}
	return true;
}

void ReadPath(const FString& pathFile, TArray<FVector>& outPath)
{
	FString FullPath = FPaths::ProjectContentDir() + pathFile;
	FString FileContent;
	FFileHelper::LoadFileToString(FileContent, *FullPath);

	FXmlFile XmlFile(FileContent, EConstructMethod::ConstructFromBuffer);
	if (!XmlFile.IsValid()) return;

	const FXmlNode* RootNode = XmlFile.GetRootNode();
	const FXmlNode* PointsNode = RootNode->FindChildNode("points");
	if (!PointsNode) return;

	for (const FXmlNode* PointNode : PointsNode->GetChildrenNodes())
	{
		float x = FCString::Atof(*PointNode->GetAttribute("x"));
		float y = FCString::Atof(*PointNode->GetAttribute("y"));
		outPath.Add(FVector(x, 0, y));
	}
}

void ReadObstaclesFromFile(const FString& FilePath, TArray<FVector>& outObstacles)
{
	FString FullPath = FPaths::ProjectContentDir() + FilePath;
	FString FileContent;
	FFileHelper::LoadFileToString(FileContent, *FullPath);

	FXmlFile XmlFile(FileContent, EConstructMethod::ConstructFromBuffer);
	if (!XmlFile.IsValid()) return;

	const FXmlNode* RootNode = XmlFile.GetRootNode();

	const FXmlNode* ObstaclesNode = RootNode->FindChildNode(TEXT("obstacles"));
	if (!ObstaclesNode)
	{
		return;
	}

	const TArray<FXmlNode*> ObstacleNodes = ObstaclesNode->GetChildrenNodes();

	for (const FXmlNode* ObNode : ObstacleNodes)
	{
		if (ObNode->GetTag() == TEXT("obstacle"))
		{
			FString xStr = ObNode->GetAttribute(TEXT("x"));
			FString yStr = ObNode->GetAttribute(TEXT("y"));
			FString rStr = ObNode->GetAttribute(TEXT("r"));

			float x = FCString::Atof(*xStr);
			float y = FCString::Atof(*yStr);
			float r = FCString::Atof(*rStr);

			FVector Obstacle(x, y, r);
			outObstacles.Add(Obstacle);
		}
	}
}

void ReadGridAndCostsFromFile(const FString& FilePath, TMap<TCHAR, float>& outTileCosts, TArray<FString>& outGridRows)
{
	FString FullPath = FPaths::ProjectContentDir() + FilePath;
	FString FileContent;

	if (!FFileHelper::LoadFileToString(FileContent, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("No se pudo leer el archivo: %s"), *FullPath);
		return;
	}

	FXmlFile XmlFile(FileContent, EConstructMethod::ConstructFromBuffer);
	if (!XmlFile.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Archivo XML inválido"));
		return;
	}

	const FXmlNode* RootNode = XmlFile.GetRootNode();
	if (!RootNode) return;

	//-----------------------------------------------------
	// 1?? Intentar leer el formato clásico (<Costs> + <Grid>)
	//-----------------------------------------------------
	const FXmlNode* CostsNode = RootNode->FindChildNode(TEXT("Costs"));
	const FXmlNode* GridNode = RootNode->FindChildNode(TEXT("Grid"));

	if (CostsNode && GridNode)
	{
		// Leer costos
		const TArray<FXmlNode*>& TileNodes = CostsNode->GetChildrenNodes();
		for (const FXmlNode* TileNode : TileNodes)
		{
			if (TileNode->GetTag() == TEXT("Tile"))
			{
				FString TypeStr = TileNode->GetAttribute(TEXT("type"));
				FString CostStr = TileNode->GetAttribute(TEXT("cost"));

				if (!TypeStr.IsEmpty() && !CostStr.IsEmpty())
				{
					TCHAR Type = TypeStr[0];
					float Cost = FCString::Atof(*CostStr);
					outTileCosts.Add(Type, Cost);
				}
			}
		}

		// Leer grilla
		const TArray<FXmlNode*>& RowNodes = GridNode->GetChildrenNodes();
		for (const FXmlNode* RowNode : RowNodes)
		{
			if (RowNode->GetTag() == TEXT("Row"))
			{
				FString RowText = RowNode->GetContent();
				if (!RowText.IsEmpty())
				{
					outGridRows.Add(RowText);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Archivo leído como mapa de grilla."));
		return;
	}

	//-----------------------------------------------------
	// 2?? Si no hay <Costs>/<Grid>, intentar formato <navmesh>
	//-----------------------------------------------------
	const FXmlNode* PolygonsNode = RootNode->FindChildNode(TEXT("polygons"));
	if (!PolygonsNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se encontraron nodos <Costs>/<Grid> ni <polygons>."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Archivo detectado como NavMesh. Convirtiendo a formato simplificado..."));

	int PolygonIndex = 0;
	for (const FXmlNode* PolygonNode : PolygonsNode->GetChildrenNodes())
	{
		if (PolygonNode->GetTag() != TEXT("polygon"))
			continue;

		FString RowString;
		for (const FXmlNode* PointNode : PolygonNode->GetChildrenNodes())
		{
			if (PointNode->GetTag() == TEXT("point"))
			{
				float X = FCString::Atof(*PointNode->GetAttribute(TEXT("x")));
				float Y = FCString::Atof(*PointNode->GetAttribute(TEXT("y")));
				RowString += FString::Printf(TEXT("(%.0f,%.0f) "), X, Y);
			}
		}

		if (!RowString.IsEmpty())
		{
			outGridRows.Add(RowString);
			outTileCosts.Add('P', 1.0f); // Agregamos un tipo genérico 'P' con costo 1.0
		}

		PolygonIndex++;
	}

	UE_LOG(LogTemp, Log, TEXT("NavMesh convertido a %d filas de polígonos."), PolygonIndex);
}
