// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GeneralFunctionLibrary.generated.h"

#define GPrintDebug(x) UKismetSystemLibrary::PrintString(GetWorld(), TEXT(x), true, true, FColor::Yellow, 5.f);
#define GPrintDebugWithVar(x, ...) UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT(x), __VA_ARGS__), true, true, FColor::Yellow, 5.f);
#define GMyLog(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define GMyLogWithVar(x, ...) UE_LOG(LogTemp, Warning, TEXT(x), __VA_ARGS__);
#define GPrintError(x) UKismetSystemLibrary::PrintString(GetWorld(), TEXT(x), true, true, FColor::Red, 15.f);
#define GPrintErrorWithVar(x, ...) UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT(x), __VA_ARGS__), true, true, FColor::Red, 15.f); UE_LOG(LogTemp, Warning, TEXT(x), __VA_ARGS__);

#define RETURN_IF_NOT_OWNER() \
	if (ACharacter* CharacterRef = Cast<ACharacter>(GetOwner())) \
	{ \
		if (!CharacterRef->IsLocallyControlled()) \
		return; \
	}

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(Blueprintable)
struct FIntPair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int First = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Second = 0;

	FIntPair() = default;
	FIntPair(const int inFirst, const int inSecond) { First = inFirst; Second = inSecond; }
};

// Since TArray cannot be used in some contexts (i.e. maps) we encapsulate it inside struct
USTRUCT()
struct FStructActorArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<AActor*> EncapsulatedArray;

	FStructActorArray() = default;

	explicit FStructActorArray(AActor* SingleObj) {
		EncapsulatedArray.Add(SingleObj);
	 };
};

/**
 * 
 */
UCLASS()
class COREAS_API UGeneralFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UGameplayEffect> GetCostGameplayEffectClass(const TSubclassOf<UGameplayAbility>& AbilityClass);

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UGameplayEffect> GetCooldownGameplayEffectClass(const TSubclassOf<UGameplayAbility>& AbilityClass);
	
	// Given an index as row name and the data table, returns the row struct.
	template<typename T>
	static T GetDataTableRow(const int RowIndex, UDataTable* DTRef)
	{
		if (!DTRef)
		{
			if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Tried to get data table row but passed DT is nullptr."));}
			return T();
		}
		
		if (!UDataTableFunctionLibrary::DoesDataTableRowExist(DTRef, {*FString::FromInt(RowIndex)}))
		{
			if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Passed DT row does not exist."));}
			return T();
		}

		return *DTRef->FindRow<T>({*FString::FromInt(RowIndex)}, "");
	}

	static UActorComponent* CreateComponentOnInstance(AActor* ActorInstance,
		TSubclassOf<UActorComponent> ComponentClass,
		USceneComponent* OptionalParentForSceneComponent = nullptr,
		const FName& OptionalSocket = NAME_None)
	{
		if (!ActorInstance || !ComponentClass)
			return nullptr;

		// Don't create component on a template actor (CDO or Archetype)
		if (ActorInstance->IsTemplate())
			return nullptr;

		// For multiplayer games, create component only on server if component is replicating
		const UActorComponent* ComponentCDO = ComponentClass->GetDefaultObject<UActorComponent>();
		if (ComponentCDO->GetIsReplicated() && !ActorInstance->HasAuthority())
			return nullptr;

		UActorComponent* NewComp = NewObject<UActorComponent>(ActorInstance, ComponentClass);

		// Handles USceneComponent attachment
		if (USceneComponent* NewSceneComp = Cast<USceneComponent>(NewComp))
		{
			USceneComponent* ParentComponent = OptionalParentForSceneComponent;
			if (!ParentComponent)
				ParentComponent = ActorInstance->GetRootComponent();

			if (ParentComponent)
			{
				// Parent component should always be owned by the passed in actor instance!
				check(ParentComponent->GetOwner() == ActorInstance);
				NewSceneComp->SetupAttachment(ParentComponent, OptionalSocket);
			}
			else
			{
				// Set component directly as root if no root component on the actor
				ActorInstance->SetRootComponent(NewSceneComp);
			}
		}

		NewComp->RegisterComponent();
		return NewComp;
	}

	static UStaticMeshComponent* GetClosestComponentToLocation(TArray<UStaticMeshComponent*> ComponentArray, FVector Location);

	static int Fibonacci(const int n);

	static bool AmIServer(APawn* PawnRef) { return PawnRef->HasAuthority(); }

	static int GetLastNumberFromString(const FString& String)
	{
		const FString LastTwoChars = UKismetStringLibrary::GetSubstring(String, String.Len() - 2, 2);
		const FString LastChar = UKismetStringLibrary::GetSubstring(String, String.Len() - 1, 1);

		const int LastCharAsInt = UKismetStringLibrary::IsNumeric(LastChar) ? FCString::Atoi(*LastChar) : 0;
		
		return UKismetStringLibrary::IsNumeric(LastTwoChars) ? FCString::Atoi(*LastTwoChars) : LastCharAsInt;
	}

	// Currently O(n^2)
	template<typename T>
	static TArray<T> OrderObjectsByName(TArray<T> ArrayToOrder)
	{
		int CurrentIndex = 1;
		while (CurrentIndex < ArrayToOrder.Num())
		{
			const int CurrentNumber = GetLastNumberFromString(UKismetSystemLibrary::GetObjectName(ArrayToOrder[CurrentIndex]));
			const int PreviousNumber = GetLastNumberFromString(UKismetSystemLibrary::GetObjectName(ArrayToOrder[CurrentIndex-1]));

			if (PreviousNumber <= CurrentNumber)
			{
				CurrentIndex++;
				continue;
			}

			T CurrentComp = ArrayToOrder[CurrentIndex];
			T PreviousComp = ArrayToOrder[CurrentIndex - 1];

			ArrayToOrder[CurrentIndex] = PreviousComp;
			ArrayToOrder[CurrentIndex-1] = CurrentComp;
			CurrentIndex = 1;
		}
		return ArrayToOrder;
	}

	UFUNCTION(BlueprintPure)
	static FLinearColor HexSRGBToRGB(const FString& HexCode) {
		// Remove the '#' if present
		FString Hex = HexCode;
		if (Hex.StartsWith("#")) {
			Hex.MidInline(1, Hex.Len() - 1);
		}

		// Ensure the hex code is valid
		if (Hex.Len() != 6) {
			UE_LOG(LogTemp, Error, TEXT("Invalid hex color code: %s"), *HexCode);
			return FColor::Black; // Return black as a fallback
		}

		// Convert hex to RGB
		int32 R = FParse::HexNumber(*Hex.Mid(0, 2));
		int32 G = FParse::HexNumber(*Hex.Mid(2, 2));
		int32 B = FParse::HexNumber(*Hex.Mid(4, 2));

		// Create and return an FColor
		return FColor(R, G, B);
	}

	template <typename T>
	static void SuffleTArray(TArray<T*>* ArrayToShuffle) {
		for (int32 i = ArrayToShuffle->Num() - 1; i > 0; --i) {
			int32 j = FMath::Floor(FMath::Rand() * (i + 1)) % ArrayToShuffle->Num();
			T* temp = (*ArrayToShuffle)[i];
			(*ArrayToShuffle)[i] = (*ArrayToShuffle)[j];
			(*ArrayToShuffle)[j] = temp;
		}
	}
	
	static bool HasAnyObjectBetween(AActor* First, AActor* Second);

	static float DistanceBetween(AActor* First, AActor* Second);
};
