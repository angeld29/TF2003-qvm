#include "g_local.h"

void dremove( gedict_t * te )
{
	if ( !te || te == world )
	{
		G_dprint( "***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n" );
		G_dprint( "WORLD has nearly been removed. Don't worry\n" );
		G_dprint( "***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n" );
		return;
	}
	if ( te->is_removed == 1 )
		return;
	te->is_removed = 1;
	ent_remove( te );
};

void display_location(  )
{
	G_sprint( self, 2, "Location : '%d %d %d'\n", self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] );
	G_sprint( self, 2, "Angles   : '%d %d %d'\n", self->s.v.angles[0], self->s.v.angles[1], self->s.v.angles[2] );
};
