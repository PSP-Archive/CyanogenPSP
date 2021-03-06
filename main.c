#include "appDrawer.h"
#include "boot.h"
#include "gameLauncher.h"
#include "homehook.h"
#include "homeMenu.h"
#include "include/utils.h"
#include "lockScreen.h"
#include "main.h"
#include "musicPlayer.h"
#include "powerMenu.h"
#include "settingsMenu.h"

PSP_MODULE_INFO("CyanogenPSP",  1, 6, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU); 
PSP_HEAP_SIZE_KB(20 * 1024); //This line will be altered for slims. Right now us (20480) when building for PSP 1000s, and (53248) for testing on PPSSPP. Using heap_max breaks the browser so don't use it.

int initOSLib() //Initialize OsLib
{
    oslInit(1);
    oslInitGfx(OSL_PF_8888, 1);
	oslSetBilinearFilter(1);
    oslInitAudio();
	oslNetInit(); 
    oslSetKeyAutorepeatInit(40);
    oslSetKeyAutorepeatInterval(10);
    return 0;
}

u32 homeHook()
{
	SceCtrlData ctrl;
	u32 buttons = 0;
	u32 homeButton = 0;
	sceCtrlPeekBufferPositive(&ctrl, 1);
	homeButton = readHomeButton();
	if (homeButton != 0)
		if (buttons & PSP_CTRL_HOME)
			powermenu();
	return 0;
}

int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}

int callbackThread(SceSize args, void *argp)
{
	int id;
	
	id = sceKernelCreateCallback("Exit Callback", (void *)exit_callback, NULL);
	sceKernelRegisterExitCallback(id);

	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks()
{
	int callback_thread_id = 0;

	callback_thread_id = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if (callback_thread_id >= 0)
	{
		sceKernelStartThread(callback_thread_id, 0, 0);
	}

	return callback_thread_id;
}

int main()
{
	initOSLib(); //Initializes OsLib
	
	OSL_MEMSTATUS ram = oslGetRamStatus();
	totalRam = (ram.maxAvailable/1000000);//Get total RAM during bootup
	
	initHomeButton(sceKernelDevkitVersion());
	
	oslIntraFontInit(INTRAFONT_CACHE_ALL | INTRAFONT_STRING_UTF8); //Initializes OSL fonts

	//Loads our audio tones
	camera_click = oslLoadSoundFile("system/media/audio/ui/camera_click.wav", OSL_FMT_NONE);
	KeypressStandard = oslLoadSoundFile("system/media/audio/ui/KeypressStandard.wav", OSL_FMT_NONE);
	Lock = oslLoadSoundFile("system/media/audio/ui/Lock.wav", OSL_FMT_NONE);
	Unlock = oslLoadSoundFile("system/media/audio/ui/Unlock.wav", OSL_FMT_NONE);
	
	strcpy(backgroundPath, setFileDefaultsChar("system/settings/background.bin", "system/framework/framework-res/res/ventana.png", backgroundPath));
	strcpy(fontPath, setFileDefaultsChar("system/settings/font.bin", "system/fonts/Roboto.pgf", fontPath));
	
	hrTime = setFileDefaultsInt("system/app/clock/timeSet.bin", 0, hrTime);
	language = setFileDefaultsInt("system/settings/language.bin", 0, language);
	batteryM = setFileDefaultsInt("system/settings/battery.bin", 1, batteryM);
	experimentalF = setFileDefaultsInt("system/settings/experimentalFeatures.bin", 0, experimentalF);
	DARK = setFileDefaultsInt("system/settings/darkTheme.bin", 0, DARK);
	fontSize = setFileDefaultsFloat("system/settings/fontSize.bin", 0.5, fontSize);
	lsCoverArt = setFileDefaultsInt("system/app/musicplayer/lsCoverArt.bin", 1, lsCoverArt);
	
	checkGBootActivation();
	themesLoad();
	iconPackLoad();
	installRequiredFiles();
	
	//Loads our images into memory
	background = oslLoadImageFile(backgroundPath, OSL_IN_RAM, OSL_PF_8888);
	cursor = oslLoadImageFilePNG(cursorPath, OSL_IN_VRAM, OSL_PF_8888);
	navbar = oslLoadImageFile(navbarPath, OSL_IN_RAM, OSL_PF_8888);
	navbar2 = oslLoadImageFile(navbar2Path, OSL_IN_RAM, OSL_PF_8888);
	ic_allapps = oslLoadImageFilePNG(allappsPath, OSL_IN_RAM, OSL_PF_8888);
	ic_allapps_pressed = oslLoadImageFile(allapps_pressedPath, OSL_IN_RAM, OSL_PF_8888);
	//notif = oslLoadImageFile("system/home/menu/notif.png", OSL_IN_RAM, OSL_PF_8888);
	welcome = oslLoadImageFilePNG("system/home/icons/welcome.png", OSL_IN_RAM, OSL_PF_8888);
	transbackground = oslLoadImageFilePNG("system/home/icons/transbackground.png", OSL_IN_RAM, OSL_PF_8888);
	control = oslLoadImageFilePNG(controlsPath, OSL_IN_VRAM, OSL_PF_8888);
	ic_launcher_apollo = oslLoadImageFilePNG(apolloPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_browser = oslLoadImageFile(browserPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_calculator = oslLoadImageFilePNG(calcPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_clock = oslLoadImageFilePNG(clockPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_filemanager = oslLoadImageFilePNG(fmPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_gallery = oslLoadImageFilePNG(galleryPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_game = oslLoadImageFilePNG(gamePath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_messenger = oslLoadImageFilePNG(messagesPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_settings = oslLoadImageFilePNG(settingsPath, OSL_IN_RAM, OSL_PF_8888);
	ic_launcher_umd = oslLoadImageFilePNG(umdPath, OSL_IN_RAM, OSL_PF_8888);
	usbdebug = oslLoadImageFilePNG("system/home/icons/usbdebug.png", OSL_IN_RAM, OSL_PF_8888);
	music = oslLoadImageFilePNG("system/home/icons/music.png", OSL_IN_RAM, OSL_PF_8888);
	quickSettings = oslLoadImageFile(quickSettingsBgPath, OSL_IN_VRAM, OSL_PF_8888);
	batt100 = oslLoadImageFile("system/home/icons/100.png", OSL_IN_VRAM, OSL_PF_8888);
	batt80 = oslLoadImageFile("system/home/icons/80.png", OSL_IN_VRAM, OSL_PF_8888);
	batt60 = oslLoadImageFile("system/home/icons/60.png", OSL_IN_VRAM, OSL_PF_8888);
	batt40 = oslLoadImageFile("system/home/icons/40.png", OSL_IN_VRAM, OSL_PF_8888);
	batt20 = oslLoadImageFile("system/home/icons/20.png", OSL_IN_VRAM, OSL_PF_8888);
	batt10 = oslLoadImageFile("system/home/icons/10.png", OSL_IN_VRAM, OSL_PF_8888);
	batt0 = oslLoadImageFile("system/home/icons/0.png", OSL_IN_VRAM, OSL_PF_8888);
	battcharge = oslLoadImageFile("system/home/icons/charge.png", OSL_IN_VRAM, OSL_PF_8888);
	volumeBar = oslLoadImageFilePNG("system/home/menu/volumeBar.png", OSL_IN_RAM, OSL_PF_8888);
	volumeControl = oslLoadImageFile(volumeControlPath, OSL_IN_RAM, OSL_PF_8888);
	layerA = oslLoadImageFilePNG("system/home/icons/layerA.png", OSL_IN_RAM, OSL_PF_8888);
	layerB = oslLoadImageFilePNG("system/home/icons/layerB.png", OSL_IN_RAM, OSL_PF_8888);
	navbarHighlight = oslLoadImageFilePNG(navbarHighlightPath, OSL_IN_RAM, OSL_PF_8888);
	navbarHighlight2 = oslLoadImageFilePNG(navbarHighlight2Path, OSL_IN_RAM, OSL_PF_8888);
	power = oslLoadImageFilePNG("system/home/menu/powerMenu.png", OSL_IN_RAM, OSL_PF_8888);
	powerSelection = oslLoadImageFilePNG("system/home/menu/powerSelection.png", OSL_IN_RAM, OSL_PF_8888);
	recoverySelection = oslLoadImageFilePNG("system/home/menu/recoverySelection.png", OSL_IN_RAM, OSL_PF_8888);
	lsMusic = oslLoadImageFilePNG("system/home/menu/lsMusic.png", OSL_IN_RAM, OSL_PF_8888);
	lsMusicBg = oslLoadImageFilePNG("system/home/menu/lsMusicBg.png", OSL_IN_RAM, OSL_PF_8888);
	mp3Play = oslLoadImageFilePNG("system/app/musicplayer/play.png", OSL_IN_RAM, OSL_PF_8888);
	mp3Pause = oslLoadImageFilePNG("system/app/musicplayer/pause.png", OSL_IN_RAM, OSL_PF_8888);
	
	//Debugger - Displays an error message if the following resources are missing.
	if (!background || !cursor || !navbar || !navbar2 || !ic_allapps || !ic_allapps_pressed || !ic_launcher_apollo || !ic_launcher_browser || !ic_launcher_calculator || !ic_launcher_filemanager || !ic_launcher_gallery || !ic_launcher_game || !ic_launcher_messenger || !ic_launcher_settings || !ic_launcher_umd || !usbdebug  || !music || !quickSettings || !batt100 || !batt80 || !batt60 || !batt40 || !batt20 || !batt10 || !batt0 || !battcharge || !volumeBar || !volumeControl || !layerA || !layerA || !navbarHighlight || !navbarHighlight2 || !power || !powerSelection || !recoverySelection || !lsMusic || !lsMusicBg || !mp3Play || !mp3Pause)
		debugDisplay();
	
	Roboto = oslLoadIntraFontFile(fontPath, INTRAFONT_CACHE_ALL | INTRAFONT_STRING_UTF8);
	oslSetFont(Roboto); //Load and set font
	
	SceUID kModule[4];
	
	kModule[0] = pspSdkLoadStartModule("modules/display.prx", PSP_MEMORY_PARTITION_KERNEL);
	kModule[1] = pspSdkLoadStartModule("modules/control.prx", PSP_MEMORY_PARTITION_KERNEL);
	kModule[2] = pspSdkLoadStartModule("modules/impose.prx", PSP_MEMORY_PARTITION_KERNEL);
	kModule[2] = pspSdkLoadStartModule("modules/homehook.prx", PSP_MEMORY_PARTITION_KERNEL);
	
	int i;
	
	for (i = 0; i < 3; i++)
	{
		if (kModule[i] < 0)
		{
			pspDebugScreenPrintf("Error 0x%08X starting module.\n", kModule[i]);
			break;
		}
	}
	
	setupCallbacks();
	
	//removeUpdateContents(); //Delete update.zip
	
	setCpuBoot(); //Set default CPU or load pre-existing value
	setPowerManagement(); //Set default power save settings or load pre-existing values.
	
	getPSPNickname(); //Get PSP name before hand
	strcpy (pspname, nickname);
	
	//Sets the cursor's original position on the screen
	cursor->x = 240;
	cursor->y = 136;	
	
	sceImposeSetHomePopup(0);
	
	//Main loop to run the program
	while (!osl_quit)
	{		
		bootAnimation();
		
		//Ends Printing and Drawing
		oslEndDrawing(); 

		//End's Frame
        oslEndFrame(); 
		
		//Synchronizes the screen 
		oslSyncFrame();	
	}
	
	//sceImposeSetHomePopup(1);
	oslNetTerm();
	oslQuit(); //Terminates/Ends the program
	return 0;
}