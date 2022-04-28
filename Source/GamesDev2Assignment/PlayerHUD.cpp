// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"

UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayerHUD::UpdateHUD(int HP, int MP, int HPotions, int MPotions, int CStacks, int SStacks)
{
	UpdateHPText(HP);
	UpdateMPText(MP);
	UpdateHPotionsText(HPotions);
	UpdateMPotionsText(MPotions);
	UpdateCStacksText(CStacks);
	UpdateSStacksText(SStacks);
}

void UPlayerHUD::UpdateHPText(int HP)
{
	if (TB_HPText)
	{
		TB_HPText->SetText(FText::FromString(FString("Health: " + FString::FromInt(HP) + " / 200")));
	}
}

void UPlayerHUD::UpdateMPText(int MP)
{
	if (TB_MPText)
	{
		TB_MPText->SetText(FText::FromString(FString("Magic: " + FString::FromInt(MP) + " / 100")));
	}
}

void UPlayerHUD::UpdateHPotionsText(int HPotions)
{
	if (TB_HPotionsText)
	{
		TB_HPotionsText->SetText(FText::FromString(FString("Health Potions: " + FString::FromInt(HPotions))));
	}
}

void UPlayerHUD::UpdateMPotionsText(int MPotions)
{
	if (TB_MPotionsText)
	{
		TB_MPotionsText->SetText(FText::FromString(FString("Magic Potions: " + FString::FromInt(MPotions))));
	}
}

void UPlayerHUD::UpdateCStacksText(int CStacks)
{
	if (TB_CStacksText)
	{
		TB_CStacksText->SetText(FText::FromString(FString("Charge Stacks: " + FString::FromInt(CStacks) + " / 5")));
	}
}

void UPlayerHUD::UpdateSStacksText(int SStacks)
{
	if (TB_SStacksText)
	{
		TB_SStacksText->SetText(FText::FromString(FString("Speed Stacks: " + FString::FromInt(SStacks) + " / 5")));
	}
}