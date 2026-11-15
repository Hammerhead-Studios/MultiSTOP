//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Photo inventory
//
//=====================================================================================//

#include "cbase.h"
#include "player.h"
#include "portal_player.h"

#include "photo.h"
#include "inv_photos.h"
#include "portal_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//
// Photo inventory
//

int g_CurMaxInvPhotos = MAX_PHOTOS;	 //HACK: the max photos allowed by the player's current weapon_camera


bool CPhotoInventory::IsValidPhoto( int nIndex )
{
	// Make sure it's a valid index
	if ( nIndex < 0 || nIndex >= g_CurMaxInvPhotos || nIndex >= MAX_PHOTOS )
		return false;

	return ( m_Photos[nIndex].hCapturedEnt.Get() != NULL );
}

void CPhotoInventory::CyclePhotos( CPortal_Player *pPlayer, bool bForward )
{
    if ( !pPlayer || g_CurMaxInvPhotos <= 0 )
        return;

    const int nLimit = MIN( g_CurMaxInvPhotos, MAX_PHOTOS );
    const int nDirection = bForward ? 1 : -1;
    int nIndex = pPlayer->GetSelectedPhoto();
    for ( int i = 0; i < nLimit; ++i )
    {
        // Normalize negative indices too (GetSelectedPhoto can be -1).
        nIndex = (nIndex + nDirection + nLimit) % nLimit;
        if ( IsValidPhoto( nIndex ) )
        {
            pPlayer->SetSelectedPhoto( nIndex );
            return;
        }
    }
    // No valid photo remains; ClearPhotos is handled by the caller on purge.
}

void CPhotoInventory::ReportPhotos( void )
{
	for ( int i = 0; i < MAX_PHOTOS; i++ )
	{
		if ( !IsValidPhoto( i ) ) continue;
		Msg( "Photo %d: %s\n", i, m_Photos[i].hCapturedEnt->GetClassname() );
	}
}

int CPhotoInventory::FirstAvailablePhoto( int nStartIndex )
{
	if ( g_CurMaxInvPhotos <= 0 ) return -1;
	int nIndex = (nStartIndex % g_CurMaxInvPhotos + g_CurMaxInvPhotos) % g_CurMaxInvPhotos;

	// Find the first free slot
	for ( int i = 0; i < g_CurMaxInvPhotos; i++ )
	{
		// See if this is a free slot
		if ( !IsValidPhoto( nIndex % g_CurMaxInvPhotos ) )
			return nIndex % g_CurMaxInvPhotos;

		nIndex++;
	}

	return -1;
}

int	CPhotoInventory::AddPhoto( CaptureInfo_t *pInfo )
{
	// Must have room
	if ( NumPhotos() >= g_CurMaxInvPhotos )
		return -1;

	// Find the first free slot
	for ( int i = 0; i < g_CurMaxInvPhotos; i++ )
	{
		// See if this is a free slot
		if ( IsValidPhoto( i ) == false )
		{
			// Copy it
			m_Photos[i] = *pInfo;
			m_nNumPhotos++;
			return i;
		}
	}

	// No valid slot found
	return -1;
}

bool CPhotoInventory::GetPhoto( int nIndex, CaptureInfo_t *pOut )
{
	// Must be valid
	if ( IsValidPhoto( nIndex ) == false )
		return false;

	*pOut = m_Photos[nIndex];
	return true;
}

bool CPhotoInventory::RemovePhoto( int nIndex )
{
	// Make sure it's a valid index
	if ( IsValidPhoto( nIndex ) == false )
		return false;

	// Clear it
	Q_memset( &(m_Photos[nIndex]), NULL, sizeof( CaptureInfo_t ) );
	m_nNumPhotos--;

	return true;
}

void CPhotoInventory::PurgePhotos( void )
{
	m_nNumPhotos = 0;
	Q_memset( m_Photos, NULL, sizeof( m_Photos ) );
}

bool CPhotoInventory::UpdatePhoto( int nIndex, CaptureInfo_t &pIn )
{
	// Make sure it's a valid index
	if ( IsValidPhoto( nIndex ) == false )
		return false;

	m_Photos[nIndex] = pIn;
	return true;
}

void CPhotoInventory::OnRestore( void )
{
	// Fix up placement query pointers on restore.
	for ( int i = 0; i < MAX_PHOTOS; ++i )
	{
		CBaseAnimating* pEntAnimating = dynamic_cast<CBaseAnimating*>(m_Photos[i].hCapturedEnt.Get());
		if ( pEntAnimating )
		{
			m_Photos[i].pPlacementQuery = pEntAnimating->Get_CPhotoPlacementQuery();
		}
	}
}

BEGIN_DATADESC_NO_BASE( CPhotoInventory )

	DEFINE_FIELD( m_nNumPhotos, FIELD_INTEGER ),
	DEFINE_EMBEDDED_ARRAY( m_Photos, MAX_PHOTOS ),

END_DATADESC()




// Explicit-player inventory functions: do not resolve ownership using UTIL_GetLocalPlayer().
bool Photo_CanAdd( CPortal_Player *pPlayer )
{
    return pPlayer && pPlayer->m_PhotoInventory.CanAddPhoto();
}

int Photo_Add( CPortal_Player *pPlayer, CaptureInfo_t *pInfo )
{
    if ( !Photo_CanAdd( pPlayer ) || !pInfo ) return -1;
    const int nIndex = pPlayer->m_PhotoInventory.AddPhoto( pInfo );
    if ( nIndex < 0 ) return -1;
    pPlayer->OnPhotoAdded( nIndex );
    CSingleUserRecipientFilter user( pPlayer );
    user.MakeReliable();
    UserMessageBegin( user, "TakePhoto" );
    WRITE_EHANDLE( pInfo->hCapturedEnt );
    WRITE_BYTE( nIndex );
    MessageEnd();
    return nIndex;
}

bool Photo_Get( CPortal_Player *pPlayer, int nIndex, CaptureInfo_t *pOut )
{
    return pPlayer && pOut && pPlayer->m_PhotoInventory.GetPhoto( nIndex, pOut );
}

bool Photo_Remove( CPortal_Player *pPlayer, int nIndex )
{
    if ( !pPlayer || !pPlayer->m_PhotoInventory.RemovePhoto( nIndex ) ) return false;
    pPlayer->OnPhotoRemoved( nIndex );
    if ( pPlayer->m_PhotoInventory.NumPhotos() > 0 )
        pPlayer->m_PhotoInventory.CyclePhotos( pPlayer, true );
    else
        pPlayer->SetSelectedPhoto( 0 ); // ClearPhotos() is called during a full strip.
    return true;
}

unsigned int Photo_Count( CPortal_Player *pPlayer )
{
    return pPlayer ? pPlayer->m_PhotoInventory.NumPhotos() : 0;
}

void Photo_Purge( CPortal_Player *pPlayer )
{
    if ( pPlayer ) pPlayer->m_PhotoInventory.PurgePhotos();
}

void Photo_Cycle( CPortal_Player *pPlayer, bool bForward )
{
    if ( pPlayer ) pPlayer->m_PhotoInventory.CyclePhotos( pPlayer, bForward );
}

bool Photo_IsValid( CPortal_Player *pPlayer, int nIndex )
{
    return pPlayer && pPlayer->m_PhotoInventory.IsValidPhoto( nIndex );
}

bool Photo_Update( CPortal_Player *pPlayer, int nIndex, CaptureInfo_t &pIn )
{
    return pPlayer && pPlayer->m_PhotoInventory.UpdatePhoto( nIndex, pIn );
}

// Legacy compatibility for untouched single-player call sites.
bool Photo_CanAdd( void ) { return Photo_CanAdd( ToPortalPlayer( UTIL_GetLocalPlayer() ) ); }
int Photo_Add( CaptureInfo_t *pInfo ) { return Photo_Add( ToPortalPlayer( UTIL_GetLocalPlayer() ), pInfo ); }
bool Photo_Get( int nIndex, CaptureInfo_t *pOut ) { return Photo_Get( ToPortalPlayer( UTIL_GetLocalPlayer() ), nIndex, pOut ); }
bool Photo_Remove( int nIndex ) { return Photo_Remove( ToPortalPlayer( UTIL_GetLocalPlayer() ), nIndex ); }
unsigned int Photo_Count( void ) { return Photo_Count( ToPortalPlayer( UTIL_GetLocalPlayer() ) ); }
void Photo_Purge( void ) { Photo_Purge( ToPortalPlayer( UTIL_GetLocalPlayer() ) ); }
void Photo_Cycle( bool bForward ) { Photo_Cycle( ToPortalPlayer( UTIL_GetLocalPlayer() ), bForward ); }
bool Photo_IsValid( int nIndex ) { return Photo_IsValid( ToPortalPlayer( UTIL_GetLocalPlayer() ), nIndex ); }
bool Photo_Update( int nIndex, CaptureInfo_t &pIn ) { return Photo_Update( ToPortalPlayer( UTIL_GetLocalPlayer() ), nIndex, pIn ); }
