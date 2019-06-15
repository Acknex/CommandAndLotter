void unit_setTarget(ENTITY* ent, VECTOR* pos)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_UNIT)
		{
			vec_set(ent->UNIT_TARGET, pos);
		}
	}
	
}
