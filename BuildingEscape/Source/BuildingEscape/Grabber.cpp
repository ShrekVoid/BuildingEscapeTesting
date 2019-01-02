// Copyright Ben Tristem 2016.

#include "BuildingEscape.h"
#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();

	FindAndBindInput();
}

void UGrabber::FindAndBindInput()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();

	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("No input component was found!"));
	}
	else
	{
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("No physics handle was found!"));
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector playerViewPosition;
	FRotator playerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPosition, playerViewRotation);

	//UE_LOG(LogTemp, Warning, TEXT("Location: %s, Rotation: %s"), *playerViewPosition.ToString(), *playerViewRotation.ToString());


	FVector LineTraceEnd = playerViewPosition + playerViewRotation.Vector() * Reach;

	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

FHitResult UGrabber::GetFirstPhysicsObjectInReach()
{

	FVector playerViewPosition;
	FRotator playerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPosition, playerViewRotation);

	//UE_LOG(LogTemp, Warning, TEXT("Location: %s, Rotation: %s"), *playerViewPosition.ToString(), *playerViewRotation.ToString());


	FVector LineTraceEnd = playerViewPosition + playerViewRotation.Vector() * Reach;

	/*DrawDebugLine(
	GetWorld(),
	playerViewPosition,
	LineTraceEnd,
	FColor(255, 0, 0),
	false,
	0.f,
	0.f,
	10.f
	);
	*/


	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType(hit,
		playerViewPosition,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	AActor* actorHit = hit.GetActor();

	if (actorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Hit! Object: %s"), *(actorHit->GetName()));
	}

	return hit;
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber pressed"));

	auto hit = GetFirstPhysicsObjectInReach();
	auto ComponentToGrab = hit.GetComponent();
	auto ActorHit = hit.GetActor();

	if (ActorHit) {
		// Grab it
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true
		);
	}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber released"));

	PhysicsHandle->ReleaseComponent();
}


