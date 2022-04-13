
// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	/*// Draw crosshairs, health, ammo etc
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if (HUDPackage.CrosshairsCenter)
		{
			// Draw to the left and up as it doesn't draw from center.
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			// Draw to the left and up as it doesn't draw from center.
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter);
		}
		if (HUDPackage.CrosshairsRight)
		{
			// Draw to the left and up as it doesn't draw from center.
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter);
		}
		if (HUDPackage.CrosshairsTop)
		{
			// Draw to the left and up as it doesn't draw from center.
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			// Draw to the left and up as it doesn't draw from center.
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter);
		}
	}*/
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	// Get tex width and height
	const float TexWidth = Texture->GetSizeX();
	const float TexHeight = Texture->GetSizeY();

	// center of screen, offset by tex width /2 and height / 2 to render it center (relative to texture)
	FVector2D TexDrawPoint(ViewportCenter.X - (TexWidth / 2.f), ViewportCenter.Y - (TexHeight / 2.f));


	// Draw texture based on our params
	DrawTexture(
		Texture,
		TexDrawPoint.X,
		TexDrawPoint.Y,
		TexWidth,
		TexHeight,
		0.f, // U
		0.f, // V
		1.f, // leave at 1.f
		1.f, // leave at 1.f
		// Change color when overlap with enemy or something 
		FLinearColor::White
	);
}
