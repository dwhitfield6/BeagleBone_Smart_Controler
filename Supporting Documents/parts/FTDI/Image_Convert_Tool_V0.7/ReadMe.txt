Introduction: 
This img_cvt.exe is the utility which runs on Windows to convert the PNG and JPEG image
to one of the FT8XX recognizable format. Note the size of image will remain. 

The usage is : 
    img_cvt  -i  inputfilename  -f  format

         format is as follow:
            0 : ARGB1555 [default]
            1 : L1
            2 : L4
            3 : L8
            4 : RGB332
            5 : ARGB2
            6 : ARGB4
            7 : RGB565
            8 : PALETTEED [FT80X only]    
            9 : L2 [FT81X only]

Example : 
    img_cvt  -i  lenaface40.png  -f   8

Output folder:  
All the files generated are under folder inputfilename_format of current command prompt working directory.

There are 4 types of files are generated under output folder: 
*.raw  : The binary format of converted file, which can be downloaded into FT8XX graphics memory directly. 
*.rawh : The header file of converted file, which is in text representation.  
    Programmer can include this file into their program and build it into final binary. 

*.bin  : The compressed binary format of converted file in ZLIB algorithm. Programmer needs download it 
   into graphics memory of FT8XX and use CMD_INFLATE to inflate them before using it. 
*.binh : The header file of compressed binary format,which is in text representation of *.bin. 
   Programmer can include this file into their program and build it into final binary. 
   
If it is palette format, one more folder is generated under output folder for the palette file(Lookup table), which shall be downloaded into RAM_PAL.

Release Note: 
V0.7: Fixed an issue where odd sized input images are not properly generated for the L2 format.
V0.6: Added L2 format support for FT81X
V0.5: Resize the image width to even number when L4 is converted format. 
      Add stride information in .Rawh file
V0.4: First external release	  
