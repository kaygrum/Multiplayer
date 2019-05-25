#include <RoboCatServerPCH.h>


MouseServer::MouseServer()
{
}

void MouseServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject( this );
}


bool MouseServer::HandleCollisionWithCat( RoboCat* inCat )
{
	if (!picked)
	{
		if (inCat->GetTrash() <= 15)
			inCat->GetTrash()++;
		picked = true;
		
		// Hacked in here.
		int ECRS_Trash = 1 << 3;
		NetworkManagerServer::sInstance->SetStateDirty(inCat->GetNetworkId(), ECRS_Trash);
	}
	//kill yourself!
	SetDoesWantToDie( true );
	return false;
}


