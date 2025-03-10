#pragma once

enum BodyDamageType : int
{
	BODY_PRISTINE,				///< unit should appear in pristine condition
	BODY_DAMAGED,					///< unit has been damaged
	BODY_REALLYDAMAGED,		///< unit is extremely damaged / nearly destroyed
	BODY_RUBBLE,					///< unit has been reduced to rubble/corpse/exploded-hulk, etc

	BODYDAMAGETYPE_COUNT
};