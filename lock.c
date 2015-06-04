#include "clock.h"
#include "home.h"
#include "lock.h"
#include "screenshot.h"
#include "settingsmenu.h"
#include "include/utils.h"

int lockscreen()
{	
	FILE * password;
	FILE * pin;
	char passwordData[20] = "";
	char pinData[4] = "";
	
	lockscreenBg = oslLoadImageFilePNG("system/lockscreen/lockscreenBg.png", OSL_IN_RAM, OSL_PF_8888);

	if (!lockscreenBg)
		debugDisplay();
		
	if (fileExists("system/settings/password.bin"))
	{
		passProtect = 1;	
	}
	else if (fileExists("system/settings/pin.bin"))
	{
		passProtect = 2;	
	}
	
	while (!osl_quit)
	{	
		LowMemExit();
	
		oslStartDrawing();
		oslClearScreen(RGB(0,0,0));
		
		controls();	

		oslDrawImage(background);	
		oslDrawImage(lockscreenBg);
		
		centerClock(1);
		
		oslIntraFontSetStyle(Roboto, 0.4f,WHITE,0,0);
		getDayOfWeek(180,90,1);
		getMonthOfYear(250,90);
		//oslDrawStringf(20,20,"%d",passProtect); //Used for debugging only

		oslIntraFontSetStyle(Roboto, 0.5f,WHITE,0,0);
		
		battery(370,2,1);
		digitaltime(420,4,0,hrTime);
		
		if (passProtect != 1)
			androidQuickSettings();
		
		oslDrawImage(cursor);

		if (passProtect == 1)
		{
			if (cursor->x >= 220 && cursor->x <= 260 && cursor->y >= 100 && cursor->y <= 272) 
			{	
				if (osl_pad.held.cross && osl_keys->analogY <= -50)
				{
					oslPlaySound(KeypressStandard, 1);
					openOSK("Enter Password", "", 20, -1);
					password = fopen("system/settings/password.bin", "r");
					fscanf(password,"%s",passwordData);
					fclose(password);

					if (strcmp(tempMessage, passwordData) == 0)
					{
						oslPlaySound(Unlock, 1); 
						oslDeleteImage(lockscreenBg);
						home();
					}
				}
			}
		}
		
		else if (passProtect == 2)
		{
			if (cursor->x >= 220 && cursor->x <= 260 && cursor->y >= 100 && cursor->y <= 272) 
			{	
				if (osl_pad.held.cross && osl_keys->analogY <= -50)
				{
					oslPlaySound(KeypressStandard, 1);
					openOSK("Enter Pin", "", 4, -1);
					pin = fopen("system/settings/pin.bin", "r");
					fscanf(pin,"%s", pinData);
					fclose(pin);

					if (strcmp(tempPin, pinData) == 0)
					{
						oslPlaySound(Unlock, 1); 
						oslDeleteImage(lockscreenBg);
						home();
					}
				}
			}
		}
		
		else if (passProtect == 0)
		{
			if (cursor->x >= 220 && cursor->x <= 260 && cursor->y >= 100 && cursor->y <= 272) 
			{
				if (osl_pad.held.cross && osl_keys->analogY <= -50)
				{
					oslPlaySound(Unlock, 1); 
					oslDeleteImage(lockscreenBg);
					home();
				}
			}
		}
		
		if (osl_pad.held.R && osl_keys->pressed.triangle)
		{
			screenshot();
		}
		
	oslEndDrawing(); 
    oslEndFrame(); 
	oslSyncFrame();
	}
	return 0;
}