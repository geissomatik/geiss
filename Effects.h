
#ifndef GEISS_EFFECTS
#define GEISS_EFFECTS 1


bool LoadCustomMsg(int iCustomMsgNum)
{
    char section[] = "CUSTOM MESSAGES";
	char message[64];
    sprintf(message, "MESSAGE %d", iCustomMsgNum);
		
	GetPrivateProfileString(section,message,"NOT FOUND",g_song_title,256,inifile);

	if (!strcmp(g_song_title, "NOT FOUND"))
	{
		dumpmsg("Custom message not found:");
		dumpmsg(section);
		dumpmsg(message);
		g_song_title[0] = 0;
		return false;
	}

	return true;

}

void LoadPreset(int n)
{
    char section[64];
    sprintf(section, "PRESET %d", n);

    char temp[64];
	float fTemp;

    GetPrivateProfileString(section,"x_center","-999",temp,64,inifile); sscanf(temp, "%f", &fTemp);
    if (fTemp < -998) 
    {
        dumpmsg("preset not found!");
        return; // preset not present
    }
    new_gXC = fTemp*FXW;
    GetPrivateProfileString(section,"y_center","0.5",temp,64,inifile); sscanf(temp, "%f", &fTemp);
	new_gYC = fTemp*FXH;

	new_mode     = GetPrivateProfileInt(section,"mode",5,inifile);
    new_waveform = GetPrivateProfileInt(section,"wave",1,inifile);
    GetPrivateProfileString(section,"damping","1.0",temp,64,inifile); sscanf(temp, "%f", &new_damping);
    weightsum = GetPrivateProfileInt(section,"magic",1,inifile);
	g_bSlideShift  = GetPrivateProfileInt(section,"shift",0,inifile);
	gamma = GetPrivateProfileInt(section,"gamma",0,inifile);
    
    int t = GetPrivateProfileInt(section,"volpos",10,inifile);
    while (volpos < t) { volpos++; volscale *= 1.25f; };
    while (volpos > t) { volpos--; volscale *= 0.80f; };


    // these 8 are handled internally by FX_Apply_Mode()  (init part)
    //char temp[64];
    //GetPrivateProfileString(section,"t1","1",temp,64,inifile); sscanf(temp, "%f", &turn1);
    //GetPrivateProfileString(section,"t2","1",temp,64,inifile); sscanf(temp, "%f", &turn2);
    //GetPrivateProfileString(section,"s1","1",temp,64,inifile); sscanf(temp, "%f", &scale1);
    //GetPrivateProfileString(section,"s2","1",temp,64,inifile); sscanf(temp, "%f", &scale2);
    //GetPrivateProfileString(section,"f1","1",temp,64,inifile); sscanf(temp, "%f", &f1);
    //GetPrivateProfileString(section,"f2","1",temp,64,inifile); sscanf(temp, "%f", &f2);
    //GetPrivateProfileString(section,"f3","1",temp,64,inifile); sscanf(temp, "%f", &f3);
    //GetPrivateProfileString(section,"f4","1",temp,64,inifile); sscanf(temp, "%f", &f4);
    
    // these 8 are handled internally by FX_Apply_Mode()  (final part)
    //effect[CHASERS] = GetPrivateProfileInt(section,"effect_chasers",0,inifile);
    //effect[BAR]     = GetPrivateProfileInt(section,"effect_bar"    ,0,inifile);
    //effect[DOTS]    = GetPrivateProfileInt(section,"effect_dots"   ,0,inifile);
    //effect[SOLAR]   = GetPrivateProfileInt(section,"effect_solar"  ,0,inifile);
    //effect[GRID]    = GetPrivateProfileInt(section,"effect_grid"   ,0,inifile);
    //effect[NUCLIDE] = GetPrivateProfileInt(section,"effect_nuclide",0,inifile);
    //effect[SHADE]   = GetPrivateProfileInt(section,"effect_shadebobs",0,inifile);

    old_palette.lo_band       = GetPrivateProfileInt(section,"pal_lo_band",-1,inifile);
    old_palette.hi_band       = GetPrivateProfileInt(section,"pal_hi_band",-1,inifile);
    old_palette.bFXPalette    = (bool)GetPrivateProfileInt(section,"pal_bFX",-1,inifile);
    old_palette.iFXPaletteNum = GetPrivateProfileInt(section,"pal_FXpalnum",-1,inifile);
    old_palette.c1            = GetPrivateProfileInt(section,"pal_curve_id_1",-1,inifile);
    old_palette.c2            = GetPrivateProfileInt(section,"pal_curve_id_2",-1,inifile);
    old_palette.c3            = GetPrivateProfileInt(section,"pal_curve_id_3",-1,inifile);

    #if PLUGIN
        visMode = (GetPrivateProfileInt(section,"spectrum",0,inifile) == 1) ? spectrum : wave;
    #endif
    
    FX_Random_Palette(true);
    y_map_pos = -1;
    g_rush_map = true;
    FX_Apply_Mode(true, n);

    
    

}

void SavePreset(int n)
{
    char section[64];
    sprintf(section, "PRESET %d", n);

    char string[64];

	sprintf(string,"%f",gXC/(float)FXW); WritePrivateProfileString(section,"x_center",string,inifile);
	sprintf(string,"%f",gYC/(float)FXH); WritePrivateProfileString(section,"y_center",string,inifile);
	sprintf(string,"%d",mode); WritePrivateProfileString(section,"mode",string,inifile);
	sprintf(string,"%f",damping); WritePrivateProfileString(section,"damping",string,inifile);
    sprintf(string,"%d",waveform); WritePrivateProfileString(section,"wave",string,inifile);
	sprintf(string,"%d",old_weightsum); WritePrivateProfileString(section,"magic",string,inifile);
	sprintf(string,"%d",volpos); WritePrivateProfileString(section,"volpos",string,inifile);
	sprintf(string,"%d",g_bSlideShift); WritePrivateProfileString(section,"shift",string,inifile);
	sprintf(string,"%d",gamma); WritePrivateProfileString(section,"gamma",string,inifile);
	sprintf(string,"%d",g_bSlideShift  ); WritePrivateProfileString(section,"shift",string,inifile);

	sprintf(string,"%f",old_turn1); WritePrivateProfileString(section,"t1",string,inifile);
	sprintf(string,"%f",old_turn2); WritePrivateProfileString(section,"t2",string,inifile);
	sprintf(string,"%f",old_scale1); WritePrivateProfileString(section,"s1",string,inifile);
	sprintf(string,"%f",old_scale2); WritePrivateProfileString(section,"s2",string,inifile);

	sprintf(string,"%f",old_f1); WritePrivateProfileString(section,"f1",string,inifile);
	sprintf(string,"%f",old_f2); WritePrivateProfileString(section,"f2",string,inifile);
	sprintf(string,"%f",old_f3); WritePrivateProfileString(section,"f3",string,inifile);
	sprintf(string,"%f",old_f4); WritePrivateProfileString(section,"f4",string,inifile);

	sprintf(string,"%d",effect[CHASERS]); WritePrivateProfileString(section,"effect_chasers",string,inifile);
	sprintf(string,"%d",effect[BAR]    ); WritePrivateProfileString(section,"effect_bar",string,inifile);
	sprintf(string,"%d",effect[DOTS]   ); WritePrivateProfileString(section,"effect_dots",string,inifile);
	sprintf(string,"%d",effect[SOLAR]  ); WritePrivateProfileString(section,"effect_solar",string,inifile);
	sprintf(string,"%d",effect[GRID]   ); WritePrivateProfileString(section,"effect_grid",string,inifile);
	sprintf(string,"%d",effect[NUCLIDE]); WritePrivateProfileString(section,"effect_nuclide",string,inifile);
	sprintf(string,"%d",effect[SHADE]  ); WritePrivateProfileString(section,"effect_shadebobs",string,inifile);
	sprintf(string,"%d",effect[SPECTRAL]);WritePrivateProfileString(section,"effect_spectral",string,inifile);

	sprintf(string,"%d",old_palette.lo_band      ); WritePrivateProfileString(section,"pal_lo_band",string,inifile);
	sprintf(string,"%d",old_palette.hi_band      ); WritePrivateProfileString(section,"pal_hi_oband",string,inifile);
	sprintf(string,"%d",(int)old_palette.bFXPalette); WritePrivateProfileString(section,"pal_bFX",string,inifile);
	sprintf(string,"%d",old_palette.iFXPaletteNum); WritePrivateProfileString(section,"pal_FXpalnum",string,inifile);
	sprintf(string,"%d",old_palette.c1           ); WritePrivateProfileString(section,"pal_curve_id_1",string,inifile);
	sprintf(string,"%d",old_palette.c2           ); WritePrivateProfileString(section,"pal_curve_id_2",string,inifile);
	sprintf(string,"%d",old_palette.c3           ); WritePrivateProfileString(section,"pal_curve_id_3",string,inifile);

    #if PLUGIN
        if (visMode==spectrum) WritePrivateProfileString(section,"spectrum","1",inifile);
        else                   WritePrivateProfileString(section,"spectrum","0",inifile);
    #else
        WritePrivateProfileString(section,"spectrum","0",inifile);
    #endif

}






void ShadeBobs()
{
	for (int x=0; x<effect[SHADE]; x++)
	{
		int cr,cg,cb;
		int k, a, b;
		int D_offset;

		cr = (int)(1 + sinf(floatframe*micro_c1[x]));
		cg = (int)(1 + sinf(floatframe*micro_c2[x]));
		cb = (int)(1 + sinf(floatframe*micro_c3[x]));

		a = gXC + (int)(micro_rad[0][x]*cosf(floatframe*micro_f1[x]) +  micro_rad[2][x]*cosf(floatframe*micro_f2[x]));  
		b = gYC + (int)(micro_rad[1][x]*cosf(floatframe*micro_f3[x]) +  micro_rad[3][x]*cosf(floatframe*micro_f4[x]));  
		for ( k=0; k<4; k++ )
		{
			b += (rand()%5) - 2;
			a += (rand()%5) - 2;
		//return;  // no problems if we return here
			if (b>FX_YCUT && b<FXH-1-FX_YCUT)
			{
//return; // screws up if we return here!
				D_offset = b*FXW + a;
				if (iDispBits==8)
				{
					if (VS1[D_offset    ] < 250) VS1[D_offset    ]+=2;
					if (VS1[D_offset+  1] < 250) VS1[D_offset+  1]+=1;
					if (VS1[D_offset-  1] < 250) VS1[D_offset-  1]+=1;
					if (VS1[D_offset+FXW] < 250) VS1[D_offset+FXW]+=1;
					if (VS1[D_offset-FXW] < 250) VS1[D_offset-FXW]+=1;
				}
				else //if (iDispBits==32)
				{
					D_offset *= 4;
					if (cr==1) 
					{
						if (VS1[D_offset    ] < 250) VS1[D_offset    ]+=5;
						if (VS1[D_offset+  4] < 250) VS1[D_offset+  4]+=3;
						if (VS1[D_offset-  4] < 250) VS1[D_offset-  4]+=3;
						if (VS1[D_offset+FXW*4] < 250) VS1[D_offset+FXW*4]+=3;
						if (VS1[D_offset-FXW*4] < 250) VS1[D_offset-FXW*4]+=3;
					}
					D_offset++;
					if (cg==1)
					{
						if (VS1[D_offset    ] < 250) VS1[D_offset    ]+=5;
						if (VS1[D_offset+  4] < 250) VS1[D_offset+  4]+=3;
						if (VS1[D_offset-  4] < 250) VS1[D_offset-  4]+=3;
						if (VS1[D_offset+FXW*4] < 250) VS1[D_offset+FXW*4]+=3;
						if (VS1[D_offset-FXW*4] < 250) VS1[D_offset-FXW*4]+=3;
					}
					D_offset++;
					if (cb==1)
					{
						if (VS1[D_offset    ] < 250) VS1[D_offset    ]+=5;
						if (VS1[D_offset+  4] < 250) VS1[D_offset+  4]+=3;
						if (VS1[D_offset-  4] < 250) VS1[D_offset-  4]+=3;
						if (VS1[D_offset+FXW*4] < 250) VS1[D_offset+FXW*4]+=3;
						if (VS1[D_offset-FXW*4] < 250) VS1[D_offset-FXW*4]+=3;
					}
				}
			}
		}
	}
}

void Diminish_Center(unsigned char *p1)
{
    bool bCenter = (mode != 12);    // center point, or vert line?

	if (center_dwindle < 0.999)
	{
        if (bCenter)
        {
            if (iDispBits>8)
            {
                for (int i=0; i<3; i++)
                {
		            if (p1[(gXC   +(gYC  )*FXW)*4] > 1) p1[(gXC   +(gYC  )*FXW)*4] = (unsigned char)(p1[(gXC   +(gYC  )*FXW)*4] * center_dwindle);
		            if (p1[(gXC-1 +(gYC  )*FXW)*4] > 1) p1[(gXC-1 +(gYC  )*FXW)*4] = (unsigned char)(p1[(gXC-1 +(gYC  )*FXW)*4] * center_dwindle);
		            if (p1[(gXC+1 +(gYC  )*FXW)*4] > 1) p1[(gXC+1 +(gYC  )*FXW)*4] = (unsigned char)(p1[(gXC+1 +(gYC  )*FXW)*4] * center_dwindle);
		            if (p1[(gXC   +(gYC+1)*FXW)*4] > 1) p1[(gXC   +(gYC+1)*FXW)*4] = (unsigned char)(p1[(gXC   +(gYC+1)*FXW)*4] * center_dwindle);
		            if (p1[(gXC   +(gYC-1)*FXW)*4] > 1) p1[(gXC   +(gYC-1)*FXW)*4] = (unsigned char)(p1[(gXC   +(gYC-1)*FXW)*4] * center_dwindle);
                    p1 = &p1[1];
                }
            }
            else
            {
		        if (p1[gXC   +(gYC  )*FXW] > 1) p1[gXC   +(gYC  )*FXW] = (unsigned char)(p1[gXC   +(gYC  )*FXW] * center_dwindle);
		        if (p1[gXC-1 +(gYC  )*FXW] > 1) p1[gXC-1 +(gYC  )*FXW] = (unsigned char)(p1[gXC-1 +(gYC  )*FXW] * center_dwindle);
		        if (p1[gXC+1 +(gYC  )*FXW] > 1) p1[gXC+1 +(gYC  )*FXW] = (unsigned char)(p1[gXC+1 +(gYC  )*FXW] * center_dwindle);
		        if (p1[gXC   +(gYC+1)*FXW] > 1) p1[gXC   +(gYC+1)*FXW] = (unsigned char)(p1[gXC   +(gYC+1)*FXW] * center_dwindle);
		        if (p1[gXC   +(gYC-1)*FXW] > 1) p1[gXC   +(gYC-1)*FXW] = (unsigned char)(p1[gXC   +(gYC-1)*FXW] * center_dwindle);
            }
        }
        else   // vertical line
        {
            int offset;
            if (iDispBits>8)
            {
                offset = (FX_YCUT*FXW + gXC) * 4;
                int o_inc = FXW*4;
                for (int i=FX_YCUT; i<FXH-FX_YCUT; i++)
                {
                    p1[offset-4] = (unsigned char)(p1[offset-4] * center_dwindle);
                    p1[offset-3] = (unsigned char)(p1[offset-3] * center_dwindle);
                    p1[offset-2] = (unsigned char)(p1[offset-2] * center_dwindle);
		            p1[offset  ] = (unsigned char)(p1[offset  ] * center_dwindle);
		            p1[offset+1] = (unsigned char)(p1[offset+1] * center_dwindle);
		            p1[offset+2] = (unsigned char)(p1[offset+2] * center_dwindle);
                    p1[offset+4] = (unsigned char)(p1[offset+4] * center_dwindle);
                    p1[offset+5] = (unsigned char)(p1[offset+5] * center_dwindle);
                    p1[offset+6] = (unsigned char)(p1[offset+6] * center_dwindle);
                    offset += o_inc;
                }
            }
            else
            {
                offset = (FX_YCUT*FXW + gXC);
                for (int i=FX_YCUT; i<FXH-FX_YCUT; i++)
                {
		            p1[offset-1] = (unsigned char)(p1[offset-1] * center_dwindle);
		            p1[offset  ] = (unsigned char)(p1[offset  ] * center_dwindle);
		            p1[offset+1] = (unsigned char)(p1[offset+1] * center_dwindle);
                    offset += FXW;
                }
            }
        }
	}
}


void Drop_Solar_Particles_320(int n)
{
	unsigned char p1, p2, p3;
	int x, y;
	int D_offset, k;
	float newx;
	int z, z0, z1, i0, i1, i2;
	int FXW4 = FXW * 4;


	if (iDispBits==8)
	{
		n *= 4;
	}

	for (k=n; k>0; k--)
	{
		newx = 100;
		while (newx >= 17)
		{
			y = rand() % 48 - 24;
			x = rand() % 64 - 32;
			newx = sqrtf(x*x + y*y);
		}
		y += gYC;
		x += gXC;
		if (y>FX_YCUT && y<FXH-1-FX_YCUT)
		{
			D_offset = y*320 + x;
		
			p1 = rand()%256;
			p2 = rand()%256;
			p3 = rand()%256;

			if (iDispBits==8)
			{
				i0 = ((17-newx)*12.0)*(p1/256.0);
				i1 = ((17-newx)* 3.0)*(p1/256.0);
				i2 = ((17-newx)* 0.8)*(p1/256.0);

				i0 = 2 + rand()%3;
				i1 = i0/2;
				i2 = i1/2;

				if (VS1[D_offset] < 207-i0) 
				{
						VS1[D_offset] += i0;
						VS1[D_offset+1] += i1;
						VS1[D_offset-1] += i1;
						VS1[D_offset+320] += i1;
						VS1[D_offset-320] += i1;
						VS1[D_offset-321] += i2;
						VS1[D_offset-319] += i2;
						VS1[D_offset+319] += i2;
						VS1[D_offset+321] += i2;
				}
			}
			else //if (iDispBits > 8)
			{
				D_offset *= 4;

				i0 = ((17-newx)*12.0)*(p1/256.0);
				i1 = ((17-newx)* 3.0)*(p1/256.0);
				i2 = ((17-newx)* 0.8)*(p1/256.0);
				if (VS1[D_offset] < 207-i0) 
				{
					VS1[D_offset] += i0;
					VS1[D_offset+4] += i1;
					VS1[D_offset-4] += i1;
					VS1[D_offset+FXW4] += i1;
					VS1[D_offset-FXW4] += i1;
					VS1[D_offset-FXW4-4] += i2;
					VS1[D_offset-FXW4+4] += i2;
					VS1[D_offset+FXW4-4] += i2;
					VS1[D_offset+FXW4+4] += i2;
				}
				D_offset ++;

				i0 = ((17-newx)*12.0)*(p2/256.0);
				i1 = ((17-newx)* 3.0)*(p2/256.0);
				i2 = ((17-newx)* 0.8)*(p2/256.0);
				if (VS1[D_offset] < 207-i0) 		
				{
						VS1[D_offset] += i0;
						VS1[D_offset+4] += i1;
						VS1[D_offset-4] += i1;
						VS1[D_offset+FXW4] += i1;
						VS1[D_offset-FXW4] += i1;
						VS1[D_offset-FXW4-4] += i2;
						VS1[D_offset-FXW4+4] += i2;
						VS1[D_offset+FXW4-4] += i2;
						VS1[D_offset+FXW4+4] += i2;
				}
				D_offset ++;

				i0 = ((17-newx)*12.0)*(p3/256.0);
				i1 = ((17-newx)* 3.0)*(p3/256.0);
				i2 = ((17-newx)* 0.8)*(p3/256.0);
				if (VS1[D_offset] < 207-i0)
				{
						VS1[D_offset] += i0;
						VS1[D_offset+4] += i1;
						VS1[D_offset-4] += i1;
						VS1[D_offset+FXW4] += i1;
						VS1[D_offset-FXW4] += i1;
						VS1[D_offset-FXW4-4] += i2;
						VS1[D_offset-FXW4+4] += i2;
						VS1[D_offset+FXW4-4] += i2;
						VS1[D_offset+FXW4+4] += i2;
				}
			}
		}
	}
}


void Drop_Solar_Particles(int n)
{
	int x, y;
	int D_offset, k;
	float newx, xf, yf;
	int z, z0, z1, i0, i1, i2;
	int FXW4 = FXW * 4;

	if (iDispBits==8)
	{
		n *= 4;
	}

	for (k=n; k>0; k--)
	{
		newx = 100;
		int its=0;
		while (newx >= 35)
		{
			y = (rand() % 72 - 36);
			x = (rand() % 96 - 48);
			newx = sqrtf(x*x + y*y);   // max. here is 80, but we clip at 48.
			//newx *= newx;
			//newx *= newx;
			//newx *= (1.0/(80.0*80.0));
			//its++;
			//if (its > 9999) exit(9999);
		}

		y += gYC;
		x += gXC;
		if (y>FX_YCUT && y<FXH-1-FX_YCUT)
		{
			D_offset = y*FXW + x;
		
			if (iDispBits==8)
			{
				i0 = 2 + rand()%2 + (35-newx)/9;
				i1 = i0-1;
				i2 = i1-1;
				if (VS1[D_offset] < 207-i0) 
				{
					VS1[D_offset] += i0;
					VS1[D_offset+1] += i1;
					VS1[D_offset-1] += i1;
					VS1[D_offset+FXW] += i1;
					VS1[D_offset-FXW] += i1;
					VS1[D_offset-FXW-1] += i2;
					VS1[D_offset-FXW+1] += i2;
					VS1[D_offset+FXW-1] += i2;
					VS1[D_offset+FXW+1] += i2;
				}
			}
			else //if (iDispBits > 8)
			{
				D_offset *= 4;

				i0 = 4 + rand()%30 * (35-newx)/25;
				i1 = i0-3;
				i2 = i1/2;
				if (VS1[D_offset] < 207-i0) 
				{
					VS1[D_offset] += i0;
					VS1[D_offset+4] += i1;
					VS1[D_offset-4] += i1;
					VS1[D_offset+FXW4] += i1;
					VS1[D_offset-FXW4] += i1;
					VS1[D_offset-FXW4-4] += i2;
					VS1[D_offset-FXW4+4] += i2;
					VS1[D_offset+FXW4-4] += i2;
					VS1[D_offset+FXW4+4] += i2;
				}
				D_offset ++;

				i0 = 4 + rand()%30 * (35-newx)/25;
				i1 = i0-3;
				i2 = i1/2;
				if (VS1[D_offset] < 207-i0) 		
				{
						VS1[D_offset] += i0;
						VS1[D_offset+4] += i1;
						VS1[D_offset-4] += i1;
						VS1[D_offset+FXW4] += i1;
						VS1[D_offset-FXW4] += i1;
						VS1[D_offset-FXW4-4] += i2;
						VS1[D_offset-FXW4+4] += i2;
						VS1[D_offset+FXW4-4] += i2;
						VS1[D_offset+FXW4+4] += i2;
				}
				D_offset ++;

				i0 = 4 + rand()%30 * (35-newx)/25;
				i1 = i0-3;
				i2 = i1/2;
				if (VS1[D_offset] < 207-i0)
				{
						VS1[D_offset] += i0;
						VS1[D_offset+4] += i1;
						VS1[D_offset-4] += i1;
						VS1[D_offset+FXW4] += i1;
						VS1[D_offset-FXW4] += i1;
						VS1[D_offset-FXW4-4] += i2;
						VS1[D_offset-FXW4+4] += i2;
						VS1[D_offset+FXW4-4] += i2;
						VS1[D_offset+FXW4+4] += i2;
				}
			}
		}
	}
}


void Solid_Line(float frame, unsigned char far *VS2)
{
	if (FXW==320)
	{
		int k, a, b, k2;
		float newx, newy, newx2, newy2;

		frame = frame*0.55/(0.08*20);
		newx = gXC + (12*cosf(frame*0.1102+10) +  15*cosf(frame*0.1312+20));  // DO NOT
		newy = gYC + (11*cosf(frame*0.1204+40) +  10*cosf(frame*0.1715+30));  // ERASE!
		newx2 = gXC + (10*cosf(frame*0.1213+33) +  13*cosf(frame*0.1408+15));  // DO NOT
		newy2 = gYC + ( 9*cosf(frame*0.1304+12) +  11*cosf(frame*0.1103+21));  // ERASE!

		for (k=0; k<43; k++)
		{
			a = newx*(k/43.0) + newx2*(1-k/43.0);
			b = newy*(k/43.0) + newy2*(1-k/43.0);
			if (b>FX_YCUT && b<FXH-1-FX_YCUT)
			{
				k2 = b*320+a;
				if (iDispBits>8) k2 *= 4;
				if (VS2[k2] < 223) VS2[k2] += 16;
			}
		}
	}
	else
	{
		int k, a, b, offset;
		float newx, newy, newx2, newy2;
		float s = FXW/640.0;

		frame = frame*0.55/(0.08*20);
		newx = gXC + (s*16*cosf(frame*0.1102+10) +  s*15*cosf(frame*0.1312+20));  // DO NOT
		newy = gYC + (s*15*cosf(frame*0.1204+40) +  s*10*cosf(frame*0.1715+30));  // ERASE!
		newx2 = gXC + (s*14*cosf(frame*0.1213+33) +  s*13*cosf(frame*0.1408+15));  // DO NOT
		newy2 = gYC + (s*13*cosf(frame*0.1304+12) +  s*11*cosf(frame*0.1103+21));  // ERASE!

		int k2 = s*50;
		float inv_s = 1.0f/(float)k2;
		for (k=0; k<k2; k++)
		{
			a = newx*(k*inv_s) + newx2*(1-k*inv_s);
			b = newy*(k*inv_s) + newy2*(1-k*inv_s);
			if (b>FX_YCUT && b<FXH-1-FX_YCUT)
			{
				offset = b*FXW+a;
				if (iDispBits>8) offset *= 4;
				if (VS2[offset] < 223) VS2[offset] += 16;
			}
		}
	}
}

void Two_Chasers(float frame)
{
	int k, a, b;

	float s = FXW/640.0;

	float t = frame;
	float time_scale = 1.0f;
	if (fps_at_last_mode_switch >= 10.0f && fps_at_last_mode_switch < 120.0f)
		time_scale = 30.0f/fps_at_last_mode_switch;
	t *= time_scale;

	// temp work: draw line
    /*
	{
		#include "draw.h"
		pt2d p, q;
		color_td color;
		color.r = 255;
		color.a = 255;//128 + 127*sinf(frame*0.07);
		p.x = gXC + (s*84*cosf(frame*0.1102+10) +  s*75*cosf(frame*0.1312+20));  
		p.y = gYC + (s*64*cosf(frame*0.1204+40) +  s*65*cosf(frame*0.1715+30)); 
		q.x = gXC + (s*54*cosf(frame*0.1213+33) +  s*65*cosf(frame*0.1408+15));  
		q.y = gYC + (s*62*cosf(frame*0.1304+12) +  s*61*cosf(frame*0.1103+21));  
		Smooth_Line(p, q, color, VS1, iDispBits, FXW);
	}
    */

	int passes = (effect[CHASERS]==2) ? 2 : 1;
	int k2 = (int)(20*s);
	for (k=0; k<k2; k++)
	{
		t = t + 0.08f*time_scale*20/(float)k2;
		for (int pass=0; pass<passes; pass++)
		{
			if (pass==0)
			{
				a = gXC + (int)(s*74*cosf(t*0.1102f+10.0f) +  s*65*cosf(t*0.1312f+20.0f));  
				b = gYC + (int)(s*54*cosf(t*0.1204f+40.0f) +  s*55*cosf(t*0.1715f+30.0f)); 
			}
			else
			{
				a = gXC + (int)(s*64*cosf(t*0.1213f+33.0f) +  s*55*cosf(t*0.1408f+15.0f));  
				b = gYC + (int)(s*52*cosf(t*0.1304f+12.0f) +  s*51*cosf(t*0.1103f+21.0f));  
			}

			if (b>FX_YCUT && b<FXH-1-FX_YCUT)
			{
				int offset = (b*FXW+a) * ((iDispBits==8) ? 1 : 4);
				if (iDispBits>8)
				{
					VS1[offset  ] = 255-(255-VS1[offset  ])*0.6f;
					VS1[offset+1] = 255-(255-VS1[offset+1])*0.6f;
					VS1[offset+2] = 255-(255-VS1[offset+2])*0.6f;
				}
				else //if (iDispBits==8)  //3.40
				{
					VS1[offset] = 255-(255-VS1[offset])*0.6f;
				}
			}
		}
	}
}


void Nuclide()
{
	// also see RenderWave()

	if (!SoundActive || SoundEmpty)// && iDispBits==8)
	{
		//if ((iDispBits==8 && rand()%28==0) || (iDispBits>8 && rand()%12==0))
		if (rand()%12==0)
		{
			int nodes = 3 + rand()%5;
			int n;
			int phase = rand()%1000;
			int rad = (20 + rand()%30);
			int x,y,cx,cy,str,val;
			int r;// = rand()%8 + 3;  // MAX is **10**

            if (FXW==320)
                r = 2 + rand()%6;
            else
                r = 3 + rand()%8;

			if (FXW==320)
				rad = 22 + rand()%6; //min(35, 14 + 0.7*vol);//(20 + rand()%30);
			else
				rad = 34 + rand()%8; //min(50, 20 + vol);//(20 + rand()%30);

			float cr=1.0, cg=1.0, cb=1.0;
			if (iDispBits>8)
			{
				int intfram = intframe + chaser_offset;
				float f = 7*sinf(intfram*0.007+29) + 5*cosf(intfram*0.0057+27);
				cr = 0.5 + 0.25*sinf(intfram*gF[0] +20-f) + 0.25*cosf(intfram*gF[3] +17+f);
				cg = 0.5 + 0.25*sinf(intfram*gF[1] +42+f) + 0.25*cosf(intfram*gF[4] +26-f);
				cb = 0.5 + 0.25*sinf(intfram*gF[2] +57-f) + 0.25*cosf(intfram*gF[5] +35+f);
			}

			for (n=0; n<nodes; n++)
			{
				cx = gXC + rad*cosf(n/(float)nodes * 6.28 + phase);
				cy = gYC + rad*sinf(n/(float)nodes * 6.28 + phase);
				if (cy-10>FX_YCUT && cy+10<FXH-1-FX_YCUT)
				{
					long D_offset = cy*FXW + cx;
					if (iDispBits>8) D_offset *= 4; 
					long T_offset;

					for (x=-10; x<10; x++)
					for (y=-10; y<10; y++)
					{
						T_offset = y*FXW + x;
						val = (r-sqrtf(x*x + y*y))*25;
						if (val>0)
						{
							if (iDispBits==8)
							{
								str = VS1[D_offset+T_offset] + val;
								VS1[D_offset+T_offset] = min(255, str);
							}
                            /*
							else if (iDispBits==16)
							{
								str = VS1[D_offset+T_offset] + val*cr;
								VS1[D_offset+T_offset] = min(255, str);
								str = VS3[D_offset+T_offset] + val*cg;
								VS3[D_offset+T_offset] = min(255, str);
								str = VS5[D_offset+T_offset] + val*cb;
								VS5[D_offset+T_offset] = min(255, str);
							}
                            */
							else //if (iDispBits==32)
							{
								T_offset *= 4;
								str = VS1[D_offset+T_offset] + val*cr;
								VS1[D_offset+T_offset] = min(255, str);
								str = VS1[D_offset+T_offset+1] + val*cg;
								VS1[D_offset+T_offset+1] = min(255, str);
								str = VS1[D_offset+T_offset+2] + val*cb;
								VS1[D_offset+T_offset+2] = min(255, str);
							}
						}
					}
				}
			}
		}
	}
}


/*
void Neutrons()
{
	if (iDispBits==8)
	{
		if (intframe%3==0 && rand()%3==0)
		{
			int x,y,cx,cy,str,val;
			cx = rand() % (FXW - 40) + 20;
			cy = rand() % (FXH - (FX_YCUT+20)*2 ) + (FX_YCUT+20);

			long D_offset = cy*FXW + cx;
			long T_offset;

			for (x=-10; x<10; x++)
			for (y=-10; y<10; y++)
			{
				T_offset = y*FXW + x;
				val = max(0, (10-sqrtf(x*x + y*y))*25);
    			str = VS1[D_offset+T_offset] + val;
				VS1[D_offset+T_offset] = min(255, str);
			}
		}
	}
}
*/

void One_Dotty_Chaser(float frame)
{
	int a, b, k;
	float s = FXW/640.0;

	float t = frame;
	if (fps_at_last_mode_switch >= 10.0f && fps_at_last_mode_switch < 120.0f)
		t *= 30.0f/fps_at_last_mode_switch;

	a = gXC + (int)(s*64*cosf(t*0.0613+33) +  s*55*cosf(t*0.0708+15));
	b = gYC + (int)(s*52*cosf(t*0.0704+12) +  s*51*cosf(t*0.0503+21));

	if (b>FX_YCUT && b<FXH-1-FX_YCUT)
	{
		chaser_ptr++;
		chaser_ptr %= 20;
		chaser_x[chaser_ptr] = a;
		chaser_y[chaser_ptr] = b;
		chaser_r[chaser_ptr] = 127+126*sinf(t*0.0613+33);
		chaser_g[chaser_ptr] = 127+126*sinf(t*0.0713+30);
		chaser_b[chaser_ptr] = 127+126*sinf(t*0.0513+27);

		for (k=0; k<20; k++)
		{
			if (iDispBits==8)
			{
				int offset = chaser_y[k]*FXW+chaser_x[k];
    			VS1[offset] = chaser_r[k];
				if (FXW >= 1050) //the palletized ones don't look good fat until later...
				{
    				VS1[offset+1] = chaser_r[k];
    				VS1[offset+FXW] = chaser_r[k];
    				VS1[offset+FXW+1] = chaser_r[k];
				}
			}
			else //if (iDispBits == 32)
			{
				int offset = (chaser_y[k]*FXW+chaser_x[k])*4;
    			VS1[offset  ] = chaser_r[k];
				VS1[offset+1] = chaser_g[k];
				VS1[offset+2] = chaser_b[k];
				if (FXW >= 880) //the truecolor ones look better FAT even at pretty low resolutions.
				{
    				VS1[offset+4] = chaser_r[k];
					VS1[offset+5] = chaser_g[k];
					VS1[offset+6] = chaser_b[k];
    				VS1[offset+FXW*4  ] = chaser_r[k];
					VS1[offset+FXW*4+1] = chaser_g[k];
					VS1[offset+FXW*4+2] = chaser_b[k];
    				VS1[offset+FXW*4+4] = chaser_r[k];
					VS1[offset+FXW*4+5] = chaser_g[k];
					VS1[offset+FXW*4+6] = chaser_b[k];
				}
			}
			chaser_x[k]++;
		}
	}
}



void Mode6Edges()
{
    int D_offset_1;
    int D_offset_2;
	int x;

    if (iDispBits>8)
    {
        D_offset_1 = (FXH-FX_YCUT-1)*FXW*4;
        D_offset_2 = (    FX_YCUT-1)*FXW*4;
        int c1,c2,c3,c4,c5,c6;

		for (x=0; x<FXW; x++)
		{
			c1 = 99 + 99*sinf(intframe*0.040 + x*0.031);    //red
			c2 = 99 + 99*sinf(intframe*-0.030 + x*0.024);
			c3 = 99 + 99*sinf(intframe*0.037 + x*0.035);    //green
			c4 = 99 + 99*sinf(intframe*-0.032 + x*0.028);
			c5 = 99 + 99*sinf(intframe*0.031 + x*0.033);    //blue
			c6 = 99 + 99*sinf(intframe*-0.035 + x*0.029);

			VS1[D_offset_1 + FXW*4] = rand()%40 + c1; 
			VS1[D_offset_1        ] = rand()%40 + c1; 
			VS1[D_offset_1 - FXW*4] = rand()%40 + c1; 
			VS1[D_offset_2 + FXW*4] = rand()%40 + c2; 
			VS1[D_offset_2        ] = rand()%40 + c2; 
			VS1[D_offset_2 - FXW*4] = rand()%40 + c2; 

            D_offset_1++;
            D_offset_2++;

			VS1[D_offset_1 + FXW*4] = rand()%40 + c3; 
			VS1[D_offset_1        ] = rand()%40 + c3; 
			VS1[D_offset_1 - FXW*4] = rand()%40 + c3; 
			VS1[D_offset_2 + FXW*4] = rand()%40 + c4; 
			VS1[D_offset_2        ] = rand()%40 + c4; 
			VS1[D_offset_2 - FXW*4] = rand()%40 + c4; 

            D_offset_1++;
            D_offset_2++;

			VS1[D_offset_1 + FXW*4] = rand()%40 + c5; 
			VS1[D_offset_1        ] = rand()%40 + c5; 
			VS1[D_offset_1 - FXW*4] = rand()%40 + c5; 
			VS1[D_offset_2 + FXW*4] = rand()%40 + c6; 
			VS1[D_offset_2        ] = rand()%40 + c6; 
			VS1[D_offset_2 - FXW*4] = rand()%40 + c6; 
            
            D_offset_1 += 2;
            D_offset_2 += 2;
		}
    }
    else
    {
        D_offset_1 = (FXH-FX_YCUT-1)*FXW;
        D_offset_2 = (    FX_YCUT-1)*FXW;
		for (x=0; x<FXW; x++)
		{
			int c1 = 40 + 40*sinf(intframe*0.040 + x*0.031);
			int c2 = 40 + 40*sinf(intframe*-0.030 + x*0.024);
			VS1[D_offset_1 + FXW] = rand()%40 + c1; 
			VS1[D_offset_1      ] = rand()%40 + c1; 
			VS1[D_offset_1 - FXW] = rand()%40 + c1; 
			VS1[D_offset_2 + FXW] = rand()%40 + c2; 
			VS1[D_offset_2      ] = rand()%40 + c2; 
			VS1[D_offset_2 - FXW] = rand()%40 + c2; 
            D_offset_1++;
            D_offset_2++;
		}
    
        D_offset_1 = (FXH-FX_YCUT-1)*FXW;
        D_offset_2 = (    FX_YCUT-1)*FXW;
        /*
		if (iDispBits == 16) 
            for (x=0; x<FXW; x++)
		    {
			    int c1 = 40 + 40*sinf(intframe*0.037 + x*0.035);
			    int c2 = 40 + 40*sinf(intframe*-0.032 + x*0.028);
			    VS3[D_offset_1 + FXW] = rand()%40 + c1; 
			    VS3[D_offset_1      ] = rand()%40 + c1; 
			    VS3[D_offset_1 - FXW] = rand()%40 + c1; 
			    VS3[D_offset_2 + FXW] = rand()%40 + c2; 
			    VS3[D_offset_2      ] = rand()%40 + c2; 
			    VS3[D_offset_2 - FXW] = rand()%40 + c2; 

			    c1 = 40 + 40*sinf(intframe*0.031 + x*0.033);
			    c2 = 40 + 40*sinf(intframe*-0.035 + x*0.029);
			    VS5[D_offset_1 + FXW] = rand()%40 + c1; 
			    VS5[D_offset_1      ] = rand()%40 + c1; 
			    VS5[D_offset_1 - FXW] = rand()%40 + c1; 
			    VS5[D_offset_2 + FXW] = rand()%40 + c2; 
			    VS5[D_offset_2      ] = rand()%40 + c2; 
			    VS5[D_offset_2 - FXW] = rand()%40 + c2; 

                D_offset_1++;
                D_offset_2++;
		    }
        */
    }
}


void Grid()
{
  int xInc = FXW/30;//(FXW >= 800) ? FXW/40 : FXW/30;//FXW/30;
	int yInc = xInc;//FXH/40;
    unsigned char s = max(0, 65 + 45*sinf(intframe*0.06033f*30.0f/fps_at_last_mode_switch  )  
                                + 35*cosf(intframe*0.04710f*30.0f/fps_at_last_mode_switch+1)
                                + 25*cosf(intframe*0.00523f*30.0f/fps_at_last_mode_switch-1)
                           );
	int x, dir, y;
	int k1, k;

	dir = (intframe%xInc);
	if (grid_dir==1)        // grid_dir is either 1 or -1
		dir *= -1;

  
	for (y=FX_YCUT_HIDE; y<FXH-FX_YCUT_HIDE; y+=yInc)
	{
		k1 = y*FXW;
		if (iDispBits>8) k1 *= 4;

		if (iDispBits==8)
		{
			if (FXW >= 1700)  // FAT PIXELS
			{
				for (x=0; x<FXW-1; x+=xInc)
				{
    				k = k1 + x + dir;
					if (VS1[k]<s) VS1[k] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+FXW]<s) VS1[k+FXW] = s; //150;
					if (VS1[k+FXW+1]<s) VS1[k+FXW+1] = s; //150;
				}
			}
			else //skinny pixels
			{ 
				for (x=0; x<FXW; x+=xInc)
				{
    				k = k1 + x + dir;
					if (VS1[k]<s) VS1[k] = s; //150;
				}
			}
		}
		else //32-bit
		{
			if (FXW >= 1800)  // FAT PIXELS
			{
				for (x=0; x<FXW-1; x+=xInc)
				{
    				k = k1 + (x + dir)*4;
					if (VS1[k  ]<s) VS1[k  ] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+2]<s) VS1[k+2] = s; //150;
					k += 4;
					if (VS1[k  ]<s) VS1[k  ] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+2]<s) VS1[k+2] = s; //150;
					k += FXW*4-4;
					if (VS1[k  ]<s) VS1[k  ] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+2]<s) VS1[k+2] = s; //150;
					k += 4;
					if (VS1[k  ]<s) VS1[k  ] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+2]<s) VS1[k+2] = s; //150;
				}
			}
			else //skinny pixels
			{
				for (x=0; x<FXW-1; x+=xInc)
				{
    				k = k1 + (x + dir)*4;
					if (VS1[k  ]<s) VS1[k  ] = s; //150;
					if (VS1[k+1]<s) VS1[k+1] = s; //150;
					if (VS1[k+2]<s) VS1[k+2] = s; //150;
				}
			}
		}
	}
    

}
/*
void DoCrystals(int level, int xc, int yc)
{
    int spokes = 2 + rand()%(6);
    int length = 6 + rand()%(20);
    int phase = rand()%100;

    float t, dx, dy;
    int x, y, n, D_offset;

    for (n=0; n<spokes; n++)
    {
        dx = cosf(n/(float)spokes*6.28 + phase);
        dy = sinf(n/(float)spokes*6.28 + phase);
        for (t=0; t<length; t += 0.5)
        {
            x = xc + dx*t;
            y = yc + dy*t;
            D_offset = y*FXW + x;
            if (iDispBits==8)
            {
                if (VS1[D_offset] < 150) VS1[D_offset] = 150;
            }
            else
            {
                D_offset *= 4;
                if (VS1[D_offset  ] < 150) VS1[D_offset  ] = 150;
                if (VS1[D_offset+1] < 150) VS1[D_offset+1] = 150;
                if (VS1[D_offset+2] < 150) VS1[D_offset+2] = 150;
            }
        }
        //if (rand()%8==0)
        //{
        //    DoCrystals(level+1, x, y);
        //}
    }   
}
*/

#endif

