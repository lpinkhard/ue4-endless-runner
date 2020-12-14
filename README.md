# ue4-endless-runner
Template for an endless runner in UE4

This template serves as a starting point for endless runner games, reducing the workload for creating such a game without compromising quality. The template features procedural level generation by spawning random tiles, which you can create yourself using the Blueprints provided.

You will start with a project that is already optimized for desktop, mobile and console gameplay. Keyboard, controller and touch input is already configured. In its default configuration, the character will follow splines within the tiles which designate the lanes, but you can also opt for free movement or no lateral movement at all.

Side scrolling is also possible by moving the camera and disabling lateral movement.

[Demo video](https://youtu.be/a2cYvfdjt1o)

Features:
 - Procedural level generation using tile system
 - Character can jump and move left or right (optional)
 - Three movement modes - fixed, spline and free movement
 - Lanes follow splines defined in tile Blueprints
 - Spawning of tiles and objects controlled by Blueprint arrays
 - Multiple items and blockers through inheritance
 - Moving platforms
 - Ragdoll death physics implementation
 - Third person or side scrolling perspective
 - Mobile device support with portrait mode
 - Supports touch gestures for movement and jumping
 - Basic audio support using sound cues or Wwise events
 - Basic HUD for score and distance
 - Clean code with detailed documentation
 - Number of Blueprints: 21
 - Number of C++ Classes: 8
Input: Gamepad, Keyboard, Touch
Network Replicated: No
Supported Development Platforms:
Windows: Yes
Mac: Yes

Important/Additional Notes:
 - Ragdoll death requires a physics asset for the character
 - Project uses a combination of C++ and Blueprint for best results
