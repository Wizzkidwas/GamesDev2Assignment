// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamesDev2AssignmentCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectileActor.h"
#include "MagicPotionItem.h"
#include "HealthPotionItem.h"

//////////////////////////////////////////////////////////////////////////
// AGamesDev2AssignmentCharacter

AGamesDev2AssignmentCharacter::AGamesDev2AssignmentCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	projectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	projectileSpawnPoint->SetupAttachment(RootComponent);
	projectileSpawnPoint->SetRelativeLocation(FVector(50.0f, 0.0f, 40.0f));
}

void AGamesDev2AssignmentCharacter::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	if (IsLocallyControlled() && HUDClass)
	{
		HUD = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDClass);
		HUD->AddToViewport();
		HUD->UpdateHUD(healthPoints, magicPoints, healthPotions, magicPotions, chargeStacks, speedStacks);
	}
	GetWorld()->GetTimerManager().SetTimer(PotionTimerHandle, [&]()
		{
			int random = FMath::RandRange(1, 3);
			switch (random)
			{
				case 1:
					AddHealthPotion();
					UE_LOG(LogTemp, Warning, TEXT("Health Potion"));
					break;
				case 2:
					AddMagicPotion();
					UE_LOG(LogTemp, Warning, TEXT("Magic Potion"));
					break;
				default:
					UE_LOG(LogTemp, Warning, TEXT("Nothing"));
					break;
			}
		}, 5, true, 5);
}

void AGamesDev2AssignmentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGamesDev2AssignmentCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGamesDev2AssignmentCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGamesDev2AssignmentCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGamesDev2AssignmentCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGamesDev2AssignmentCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGamesDev2AssignmentCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGamesDev2AssignmentCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGamesDev2AssignmentCharacter::OnResetVR);

	// Player actions
	PlayerInputComponent->BindAction("Health Potion", IE_Pressed, this, &AGamesDev2AssignmentCharacter::UseHealthPotion);
	PlayerInputComponent->BindAction("Magic Potion", IE_Pressed, this, &AGamesDev2AssignmentCharacter::UseMagicPotion);
	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &AGamesDev2AssignmentCharacter::Charge);
	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &AGamesDev2AssignmentCharacter::Heal);
	PlayerInputComponent->BindAction("Speed Up", IE_Pressed, this, &AGamesDev2AssignmentCharacter::SpeedUp);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AGamesDev2AssignmentCharacter::Shoot);
}

float AGamesDev2AssignmentCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	healthPoints -= DamageAmount;
	HUD->UpdateHPText(healthPoints);
	if (healthPoints <= 0)
	{
		Destroy();
		GetWorldTimerManager().ClearTimer(PotionTimerHandle);
		GetWorldTimerManager().ClearTimer(SpeedTimerHandle);
	}
	return DamageAmount;
}

void AGamesDev2AssignmentCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGamesDev2AssignmentCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AGamesDev2AssignmentCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AGamesDev2AssignmentCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGamesDev2AssignmentCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGamesDev2AssignmentCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * speedModifier);
	}
}

void AGamesDev2AssignmentCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * speedModifier);
	}
}

/// <summary>
/// End of UE4's pre-made character
/// </summary>

void AGamesDev2AssignmentCharacter::UseHealthPotion()
{
	if (healthPoints == 200)
	{
		return;
	}
	if (healthPotions > 0)
	{	
		healthPoints  += 50;
		healthPotions -= 1;
	}
	if (healthPoints > 200)
	{
		healthPoints = 200;
	}
	HUD->UpdateHPText(healthPoints);
	HUD->UpdateHPotionsText(healthPotions);
}

void AGamesDev2AssignmentCharacter::UseMagicPotion()
{
	if (magicPoints == 100)
	{
		return;
	}
	if (magicPotions > 0)
	{
		magicPoints  += 50;
		magicPotions -= 1;
	}
	if (magicPoints > 100)
	{	
		magicPoints = 100;
	}
	HUD->UpdateMPText(magicPoints);
	HUD->UpdateMPotionsText(magicPotions);
}

void AGamesDev2AssignmentCharacter::Charge()
{
	if (chargeStacks == 5)
	{
		return;
	}
	if (magicPoints >= 10)
	{
		chargeStacks += 1;
		magicPoints  -= 10;
		HUD->UpdateMPText(magicPoints);
		UpdateCharges();
	}
}

void AGamesDev2AssignmentCharacter::UpdateCharges()
{
	damage = baseDamage + (baseDamage * chargeStacks);
	HUD->UpdateCStacksText(chargeStacks);
}

void AGamesDev2AssignmentCharacter::Heal()
{
	if (healthPoints == 200)
	{
		return;
	}
	if (magicPoints >= 20)
	{
		magicPoints  -= 20;
		healthPoints += 80;
		if (healthPoints > 200)
		{
			healthPoints = 200;
		}
		HUD->UpdateHPText(healthPoints);
		HUD->UpdateMPText(magicPoints);
	}
}

void AGamesDev2AssignmentCharacter::SpeedUp()
{
	if (speedStacks == 5)
	{
		return;
	}
	if (magicPoints >= 25)
	{
		magicPoints -= 25;
		speedStacks += 1;
		HUD->UpdateMPText(magicPoints);
		UpdateSpeed();
	}
}

void AGamesDev2AssignmentCharacter::UpdateSpeed()
{
	speedModifier = 1 + (speedStacks * 0.5f);
	GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed * speedModifier;
	HUD->UpdateSStacksText(speedStacks);
	GetWorld()->GetTimerManager().SetTimer(SpeedTimerHandle, [&]()
		{
			if (speedStacks > 0)
			{
				speedStacks--;
				UpdateSpeed();
				UE_LOG(LogTemp, Warning, TEXT("Speed down"))
			}
		}, 10, true, 10);
}

void AGamesDev2AssignmentCharacter::Shoot()
{
	if (ProjectileClass)
	{
		FVector  SpawnLocation = projectileSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = projectileSpawnPoint->GetComponentRotation();
		AProjectileActor* TempProjectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SpawnLocation, SpawnRotation);
		TempProjectile->SetOwner(this);
		TempProjectile->SetDamage(damage);
		if (chargeStacks > 0)
		{
			chargeStacks--;
			UpdateCharges();
		}
	}
}

// Every 3 potions held grants an extra potion
void AGamesDev2AssignmentCharacter::AddHealthPotion()
{
	int bonus = 1 + FMath::FloorToInt(healthPotions / 3);
	healthPotions += bonus;
	HUD->UpdateHPotionsText(healthPotions);
}

void AGamesDev2AssignmentCharacter::AddMagicPotion()
{
	int bonus = 1 + FMath::FloorToInt(magicPotions / 3);
	magicPotions += bonus;
	HUD->UpdateMPotionsText(magicPotions);
}