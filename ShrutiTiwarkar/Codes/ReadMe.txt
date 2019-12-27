ReadMe :	Win32 SDK Project 2019 Batch
		belongs To Shruti Anant Tiwarkar
		https://github.com/ShrutiTiwarkar/WinRT19_Project
			

Project Description :
		
			- The project defines to create a form for storing details of a person.
			- This project used Win32 SDK and COM (Class Factory).
			- In this project the data entered by user is a part of client where as storing in File is Server part.
			- The validation of details are before saving in file.
			
		
		
	Files Included in Project
	
		Codes
			1. OptionSelection.abc -> OptionSelection.exe
			2. AddReadFillDll.abc1 -> AddReadFillDll.dll
			3. FileRecord.abc2 	   -> FileRecord.reg
			4. RecordFile.abc3	   -> RecordFile.h
			5. SetIcon.abc4		   -> SetIcon.h
			6. SetIcon.abc5		   -> SetIcon.rc
			7. Asset			   
									->	bitmap.bmp
									->	bitmap1.bmp
									->	icon.ico
			
		ScreenShots
		
			1. ShrutiTiwarkar_SplashScreen.jpg
			2. MainMenu
			3. Validation
			4. Display
			

	Project Navigation steps
		
		- Add AddReadFillDll.dll path in FileRecord.reg
		- Register it.
		
		1. OptionSelection.exe
				- Run the .exe	ShrutiTiwarkar_SplashScreen will display.

		2. Press 'S' or 's' to Start
		
		3. Menu will get display
		
			a. Add Member
			b. Display All
			c. Exit
			
		4. ( 'a' or 'A' Selected)
			
			- Dialog Box will get Dsiplay with some fields and two buttons 'OK' & 'CANCLE'
				OK : will validate and save.
				CANCLE : Discard the entry.
				
			Fields in Dialog Box
					i. Name , Middle name, Surname
						- EditText is used, values are taken from user.
						- This string must not contain any symbol or number.
						- Field should not be empty.
						- This Name validation "3_ValidateName.jpg"
					ii. Day, Month, Year
						- The DropDown list displays 
							Days  : 1 - 31
							Month : 1 - 12
							Year  :	1990 - 2001
						- Validation 
							Month ,year and date .
							"4_ValidateDate.jpg"
							"5_ValidateDate.jpg"
					iii. Gender
						- select the gender for the particular person.
						- default selection is Male.
						
					iv. Email-id
						- enter Email-id.	
						  example "NAMEOrNumber@gmail.com".
						- Field should not be empty.
					v. Phone Number
						- Must be Numbers.
						- Not Symbols nor alphabets.
						- Must be 10 digits.
					vi. Press OK to submit.
						"9_FinalSubmit.jpg"	
					
		5. ('B' or 'b' Selected)
		
				- DialogBox will get Display with the list view with all the fields entered.
				- The every field will display rows wise with each member entry according to Register Number.
				- 10_Display.jpg
				
		6. ('C' or 'c' Selected)
				- Exit 
				- Successfully terminate the program.
				
				
		7. All this process creates two Files in the Directory
			- Usese must view "Record.txt"
			- This File displays all the entries done uptill Now.
			
		8. Temp.txt
			- used for programming purpose.