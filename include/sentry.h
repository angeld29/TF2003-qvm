
void    lvl1_sentry_stand();
void    lvl1_sentry_atk1();
void    lvl1_sentry_atk2();
void    lvl1_sentry_atk3();
void    lvl2_sentry_stand();
void    lvl2_sentry_atk1();
void    lvl2_sentry_atk2();
void    lvl2_sentry_atk3();
void    lvl3_sentry_stand();
void    lvl3_sentry_atk1();
void    lvl3_sentry_atk2();
void    lvl3_sentry_atk3();

void    Sentry_Rotate();
int     Sentry_FindTarget();
int     Sentry_FindTarget_Angel();
void    Sentry_FoundTarget();
void    Sentry_HuntTarget();
void    Sentry_Pain( struct gedict_s *attacker, float take );
void    Sentry_Die();
int     Sentry_Fire();
void    Sentry_MuzzleFlash();
void    CheckSentry( gedict_t * gunhead );