This project requires Visual Studio 2013.
Download Visual C++ 2013 Express for Windows Desktop for free at 
http://www.microsoft.com/en-us/download/details.aspx?id=43733

This project depends on the Autodesk FBX SDK.
Instructions to download and set up the FBX SDK with Visual Studio are available at
http://help.autodesk.com/view/FBX/2015/ENU/?guid=__files_GUID_6E7B1A2E_584C_49C6_999E_CD8367841B7C_htm

Load the solution into Visual Studio, then configure the DX11FBX project as directed in the link above.
(Remove the include and lib directories for the FBX SDK which I added, and replace them with your own.)

Then simply build and run. To move the camera, use the WASD keys. Hold LMB and move the mouse to rotate the camera.
Press Alt+Enter to go into fullscreen mode.

NOTE: I did NOT write the DirectXTK project. It's an open source library which I included in the repository 
for convenience. I wrote the other two projects though :P

ANOTHER NOTE: I did NOT create the shark model. I obtained it from http://tf3dm.com/3d-model/megalodon-battlefield-4-67390.html