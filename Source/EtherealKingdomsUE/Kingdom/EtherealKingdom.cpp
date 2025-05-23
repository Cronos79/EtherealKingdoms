// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock


#include "EtherealKingdom.h"


UEtherealKingdom::UEtherealKingdom()
{
	//bIsReplicated = true;
}

void UEtherealKingdom::AdvanceKingdomDay()
{
    for (UEtherealBuilding* Building : Buildings)
    {
        if (IsValid(Building))
        {
            Building->AdvanceDay(this);
        }
    }
}
