# Asteroids2020
Asteroids clone using Unreal Engine

**MOVEMENT**
Thrust:		W or KeyBoard_Up
Rotate right:	D or KeyBoard_Right
Rotate left:	A or KeyBoard_Left

**ACTIONS**
Fire:		LeftClick or SpaceBar
Invincibility:	S or KeyBoard_Down


**KNOWN ISSUES**
4/13/2020:
	-Some asteroids go past an iteration of 2 and continue to create children *(Possible because they are spawned and killed to fast)
	-No Main Menu screen *(I tried)
	-No thrust noise or rocket trail particle effect
	-When SpaceShip is hit, it is not repawned at the center of the screen
	-Asteroids seem to have a similar rotation/look, even when given random values
	-Asteroids rarely spawn within eachother before they recognize they can Overlap. 
		They get a Z offset to fix this, however that asteroid can't be hit now
	