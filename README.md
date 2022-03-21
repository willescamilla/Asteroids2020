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
	
	-Unreal Engine has an issue with C++ Structs working with packaged game? 
		https://answers.unrealengine.com/questions/214027/bug-475-struct-variables-fail-in-standalone-game.html
	-Some asteroids go past an iteration of 2 and continue to create children *(Possible because they are spawned and killed too fast)
	-No Main Menu screen *(I tried)
	-No thrust noise or rocket trail particle effect
	-When SpaceShip is hit, it is not repawned at the center of the screen
	-Asteroids seem to have a similar rotation/look, even when given random values
	-Asteroids sometimes spawn within eachother although they should be able to Overlap. 
		They get a Z offset to fix this, however, that asteroid can't be hit now...
		
3/20/2022:
	
	-The spaceship's movement & rotation is ungracefully implemented in the class' tick function. I think this is why the controls seem to break when playing the game as an executable, as the framerate then becomes variable.
	
