# Worms Game (with audio!)

### Gameplay
Since the assignment had some room for interpritation, I've implemented a couple of mechanics that can be enabled by uncommenting next lines in Game.cpp:26:

		Game->GameRules =
			EGameRules_CollidingIntoWormsKillsYou
			//| EGameRules_CollidingIntoWormCutsIt
			//| EGameRules_SelfCollisionCuts
			//| EGameRules_SelfCollisionKills
			;
      
Game restarts every time your worm dies after 8s or after a player presses any mouse button

### Audio
I'm using audio to represent different states of the game:
Countdown at the beggining and two (actually one but slightly altered) themes for playbale state and game over state.
Collecting food and damaging/killings worms adds procedural sounds to the mix

### AI
Worms store quantinized direction attractiveness for making decision. This approach let's me work with a limited amount of raycasts per frame (and per worm) and still have a good result.
  
    #define NUM_RAYCASTS_PER_FRAME 8
  
 Is set to limit every worm to have maximum of 8 raycasts per frame. But if I got the assignment wrong, and 8 is a total maximum amount of raycasts, that wouldn't require a big code changes. Worm's AI was tested at 1 raycast per frame and I was happy with a result.
 
    #define RAYCAST_DISTANCE 150.0f
    
I ended up with a pretty slow turning speed, and I find 150 units works best for this game. But feel free to change it to required 100.


### Debug
Also, during development I had to implement a couple of debug tools to make sure my optimizations and AI decisions works properly. I left EDebugDisplay_AI on since it's just looks really good, but you can adjust it to your needs:

		Game->DebugDisplay = 
			EDebugDisplay_AI
			//| EDebugDisplay_BoundingBoxes
			//| EDebugDisplay_Breadcrumbs
			//| EDebugDisplay_FoodBroadphase
			//| EDebugDisplay_MoveTargets
			;

Talking about necessary optimization I decided that the game needs bounding box checks for Worms (EDebugDisplay_BoundingBoxes) and Food Broadphasing (EDebugDisplay_FoodBroadphase) to limit the number of overlap checks.

### Side notes about codestyle
Usually I'm really strict about the code style, but got a little confused here and ended up with a mix of common cpp and Unreal styles. 
