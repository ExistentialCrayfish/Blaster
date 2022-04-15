
// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw crosshairs, health, ammo etc
	FVector2D ViewportSize;
	if (GEngine)
	{
		if (GEngine->GameViewport != nullptr)
		{

			// REF - 0,0 is top left of screen

			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

			float SpreadScaled = CrosshairsSpreadMax * HUDPackage.CrosshairsSpread;

			if (HUDPackage.CrosshairsCenter)
			{
				// Draw to the left and up as it doesn't draw from center.
				DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, FVector2D(0.f, 0.f), HUDPackage.CrosshairsColor);
			}
			if (HUDPackage.CrosshairsLeft)
			{
				// Draw to the left and up as it doesn't draw from center.
				DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
			}
			if (HUDPackage.CrosshairsRight)
			{
				// Draw to the left and up as it doesn't draw from center.
				DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
			}
			if (HUDPackage.CrosshairsTop)
			{
				// Draw to the left and up as it doesn't draw from center.
				DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), HUDPackage.CrosshairsColor);
			}
			if (HUDPackage.CrosshairsBottom)
			{
				// Draw to the left and up as it doesn't draw from center.
				DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), HUDPackage.CrosshairsColor);
			}
		}
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D CrosshairsSpread, FLinearColor CrosshairColor)
{
	// Get tex width and height
	const float TexWidth = Texture->GetSizeX();
	const float TexHeight = Texture->GetSizeY();

	// center of screen, offset by tex width /2 and height / 2 to render it center (relative to texture)
	FVector2D TexDrawPoint(ViewportCenter.X - (TexWidth / 2.f) + CrosshairsSpread.X, ViewportCenter.Y - (TexHeight / 2.f) + CrosshairsSpread.Y);


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
		CrosshairColor
	);
}
