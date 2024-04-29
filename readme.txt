# PG2 OpenGL 3D app

## Dependencies

openCV not included

add system variable:
    Name:  OPENCV_DIR
    Value: ...\opencv\build
add item to PATH:
    %OPENCV_DIR%\x64\vc16\bin

## Controls

* WASD – movement
* F    – flashlight on/off
* F11  – fullscreen on/off
* V    – vsync on/off

* hold RMB to mouselook
* scrollwheel to change FOV

## Sources

* CPP/OBJ/... – TUL ITE/PG2 (Z drive)
* Shaders     – Steve Jones
* Textures    – RPGMaker tileset
* Sounds      –
* Music       – The Synapse (Hong Kong Streets) by Alexander Brandon

## Assignment

ESSENTIALS:
* [X] 3D GL Core profile + shaders at least 4.5, GL debug
* [X] high performance => at least 60 FPS
* [X] allow VSync control, fullscreen vs. windowed switching (restore window position & size)
* [ ] event processing (camera, object, app behaviour...): mouse (both axes, wheel), keyboard
* [ ] multiple different independently moving 3D models, at leats two loaded from file
* [X] at least three different textures (or subtextures from texture atlas etc.)
* [ ] lighting model, at least 3 lights (1x ambient, 1x directional, 1x reflector: at least one is moving; +possible other lights)
* [ ] correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )

EXTRAS:
* [ ] height map textured by height & proper player height coords
* [ ] working collisions
* [ ] particles
* [ ] scripting (useful)
* [ ] audio (better than just background)

INSTAFAIL:
* using GLUT, GL compatible profile
