#include "g_local.h"

void TeamFortress_CTF_FlagInfo(  )
{
	gedict_t *te;

	te = Finditem( 1 );
	if ( te->goal_state == 1 )
	{
		if ( self == PROG_TO_EDICT( te->s.v.owner ) )
			G_sprint( self, 2, "You have the enemy flag. " );
		else
		{
			G_sprint( self, 2, "%s has", PROG_TO_EDICT( te->s.v.owner )->s.v.netname );
			if ( self->team_no == 1 )
				G_sprint( self, 2, " your flag. " );
			else
				G_sprint( self, 2, " the enemy flag. " );
		}
	} else
	{
		if ( !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
		{
			if ( self->team_no == 1 )
				G_sprint( self, 2, "Your flag is lying about. " );
			else
				G_sprint( self, 2, "The enemy flag is lying about. " );
		} else
		{
			if ( self->team_no == 1 )
				G_sprint( self, 2, "Your flag is in your base. " );
			else
				G_sprint( self, 2, "The enemy flag is in their base. " );
		}
	}
	te = Finditem( 2 );
	if ( te->goal_state == 1 )
	{
		if ( self == PROG_TO_EDICT( te->s.v.owner ) )
			G_sprint( self, 2, "You have the enemy flag.\n" );
		else
		{
			G_sprint( self, 2, "%s has", PROG_TO_EDICT( te->s.v.owner )->s.v.netname );
			if ( self->team_no == 2 )
				G_sprint( self, 2, " your flag.\n" );
			else
				G_sprint( self, 2, " the enemy flag.\n" );
		}
	} else
	{
		if ( !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
		{
			if ( self->team_no == 2 )
				G_sprint( self, 2, "Your flag is lying about.\n" );
			else
				G_sprint( self, 2, "The enemy flag is lying about.\n" );
		} else
		{
			if ( self->team_no == 2 )
				G_sprint( self, 2, "Your flag is in your base.\n" );
			else
				G_sprint( self, 2, "The enemy flag is in their base.\n" );
		}
	}
};
