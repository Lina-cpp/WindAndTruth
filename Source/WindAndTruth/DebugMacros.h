#pragma once



//Macro to DrawSphere if getworld is valid
#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, true);
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, -1.f, 0, 1.f);
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 20, FColor::Red, true);
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
{	\
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, -1.f, 0, 1.f); \
DrawDebugPoint(GetWorld(), EndLocation, 20, FColor::Red, true); \
}
#define DRAW_SPHERE2(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 50, 12, Color, true);