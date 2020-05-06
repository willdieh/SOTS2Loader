# SOTS2ModelLoader
Converts Sword of the Stars 2 Models and Textures to OBJ files. 

SOTS2 has some of the most beautiful models I've even seen. Unfortunately, Kerberos never released their pipeline software for modders to have access to modeling. This program doesn't allow the use of custom models but at least is a step in the right direction in decyphering the proprietary pipeline model format.

I've at least got basic geometry and texture information which can be output to OBJ files and viewed in blender. There's a lot left to do. The native model format contains translation data and (what is probably) animation data that I just haven't finished decodding. As a result the model loads with all components at the 0,0 point rather than translated into their proper places. Also, I'm pretty sure not all the textures are being loaded (SOTS2 has many different types of texture per model). 

If you're any good with 3D model formats and think you can add to the data field descriptions, please submit a pull request or contact me!


