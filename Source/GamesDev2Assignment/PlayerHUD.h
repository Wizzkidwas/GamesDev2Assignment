// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GAMESDEV2ASSIGNMENT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TB_HPText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TB_MPText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TB_HPotionsText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TB_MPotionsText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TB_CStacksText;

public:
	void UpdateHUD(int HP, int MP, int HPotions, int MPotions, int CStacks);

	void UpdateHPText(int HP);

	void UpdateMPText(int MP);

	void UpdateHPotionsText(int HPotions);

	void UpdateMPotionsText(int MPotions);

	void UpdateCStacksText(int CStacks);
};
