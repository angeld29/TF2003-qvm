/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id: g_tf_utils.c,v 1.7 2005-05-28 19:03:46 AngelD Exp $
 */
#include "g_local.h"

qboolean GetInfokeyString( gedict_t * pl, const char *key, const char *key2, char *value, int size, const char *defaultval )
{
	infokey( pl, key, value, size );
	if ( value[0] )
		return true;
	if ( key2 )
	{
		infokey( pl, key2, value, size );
		if ( value[0] )
			return true;
	}
	if ( defaultval )
		Q_strncpyz( value, defaultval, size );

	return false;
}

int GetInfokeyInt( gedict_t * pl, const char *key, const char *key2, int defaultval )
{
	char    buff[32];

	infokey( pl, key, buff, sizeof( buff ) );
	if ( buff[0] )
		return atoi( buff );

	if ( key2 )
	{
		infokey( pl, key2, buff, sizeof( buff ) );
		if ( buff[0] )
			return atoi( buff );
	}

	return defaultval;
}

qboolean GetSVInfokeyString( const char *key, const char *key2, char *value, int size, const  char *defaultval )
{
	infokey( world, key, value, size );
	if ( value[0] )
		return true;
	if ( key2 )
	{
		infokey( world, key2, value, size );
		if ( value[0] )
			return true;
	}
	if ( defaultval )
		Q_strncpyz( value, defaultval, size );

	return false;
}

int GetSVInfokeyInt( const char *key, const char *key2, int defaultval )
{
	char    buff[32];

	infokey( world, key, buff, sizeof( buff ) );
	if ( buff[0] )
		return atoi( buff );

	if ( key2 )
	{
		infokey( world, key2, buff, sizeof( buff ) );
		if ( buff[0] )
			return atoi( buff );
	}

	return defaultval;
}

float GetSVInfokeyFloat( const char *key, const char *key2, float defaultval )
{
	char    buff[32];

	infokey( world, key, buff, sizeof( buff ) );
	if ( buff[0] )
		return atof( buff );

	if ( key2 )
	{
		infokey( world, key2, buff, sizeof( buff ) );
		if ( buff[0] )
			return atof( buff );
	}

	return defaultval;
}


void KickPlayer( int psize, gedict_t * p )
{
	g_globalvars.msg_entity = EDICT_TO_PROG( p );
	if ( psize > -3 )
		trap_WriteByte( MSG_ONE, SVC_SMALLKICK );
	else
		trap_WriteByte( MSG_ONE, SVC_BIGKICK );
}

void muzzleflash(  )
{
	trap_WriteByte( MSG_MULTICAST, SVC_MUZZLEFLASH );
	WriteEntity( MSG_MULTICAST, self );
	trap_multicast( PASSVEC3( self->s.v.origin ), MULTICAST_PVS );
}

/////////////////////////
//
// Visibility
//
/////////////////////////
int visible( gedict_t * targ )
{
	vec3_t  spot1;
	vec3_t  spot2;

	VectorAdd( self->s.v.origin, self->s.v.view_ofs, spot1 );
	VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, spot2 );

	traceline( PASSVEC3( spot1 ), PASSVEC3( spot2 ), 1, self );
    	if( GetSVInfokeyInt( "watervis", NULL, 0) == 0)
	{
	 if ( g_globalvars.trace_inopen && g_globalvars.trace_inwater )
		return 0;
	}

	if ( g_globalvars.trace_fraction == 1 )
		return 1;

	return 0;
}

int range( gedict_t * targ )
{
	vec3_t  spot1;
	vec3_t  spot2;
	float   r;

	VectorAdd( self->s.v.origin, self->s.v.view_ofs, spot1 );
	VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, spot2 );
	VectorSubtract( spot1, spot2, spot2 );
	r = vlen( spot2 );
	if ( r < 120 )
		return 0;
	if ( r < 500 )
		return 1;
	if ( r < 1000 )
		return 2;
	return 3;
}

int infront( gedict_t * targ )
{
	vec3_t  vec;
	float   dot;

	makevectors( self->s.v.angles );
	VectorSubtract( targ->s.v.origin, self->s.v.origin, vec );
	VectorNormalize( vec );

	dot = DotProduct( vec, g_globalvars.v_forward );
	if ( dot > 0.3 )
		return 1;
	return 0;
}
