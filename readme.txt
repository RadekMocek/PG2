.: PG2 :: OpenGL 3D app :.

## Dependencies ##

openCV not included, has to be installed separately:
    add system variable:
        Name:  OPENCV_DIR
        Value: ...\opencv\build
    add item to PATH:
        %OPENCV_DIR%\x64\vc16\bin

## Controls ##

* W A S D – movement
* Ctrl    – sprint     – toggle
* Space   – jetpack    – hold
* F       – flashlight – toggle
* F11     – fullscreen – toggle
* V       – vsync      – toggle

* mouselook
* scrollwheel to change FOV

## Sources ##

* CPP/OBJ/... – TUL ITE/PG2 (Z drive)
* Shaders     – Steve Jones
* Textures    – RPGMaker tileset
* Models      – https://free3d.com/3d-model/jukebox-bubbler-style-v2--835086.html
* Sounds      –
* Music       – The Synapse (Hong Kong Streets) by Alexander Brandon

## Assignment ##

ESSENTIALS:
* [X] 3D GL Core profile + shaders at least 4.5, GL debug
* [X] high performance => at least 60 FPS
* [X] allow VSync control, fullscreen vs. windowed switching (restore window position & size)
* [ ] event processing (camera, object, app behaviour...): mouse (both axes, wheel), keyboard
* [ ] multiple different independently moving 3D models, at leats two loaded from file
* [X] at least three different textures (or subtextures from texture atlas etc.)
* [ ] lighting model, at least 3 lights (ambient+directional+reflector: at least one is moving; + possible other lights)
* [X] correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )

EXTRAS:
* [X] height map textured by height & proper player height coords
* [ ] working collisions
* [ ] particles
* [ ] scripting (useful)
* [ ] audio (better than just background)

INSTAFAIL:
* using GLUT, GL compatible profile
