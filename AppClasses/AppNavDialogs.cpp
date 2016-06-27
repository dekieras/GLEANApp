/*
 *  AppNavDialogs.cpp
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/15/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppNavDialogs.h"

#include <iostream>
using std::cout;	using std::endl;
using std::string;


AppNavOpenDialog::AppNavOpenDialog(bool text_files_only_) : text_files_only(text_files_only_), success(false)
{
	initialize(this);
}

AppNavOpenDialog::~AppNavOpenDialog()
{
	DisposeNavEventUPP (NavOpenDialogUPP);
}

void AppNavOpenDialog::initialize(AppNavOpenDialog * derived_ptr)
{
    OSStatus err = noErr;
    NavDialogRef theOpenDialog;
    NavDialogCreationOptions dialogOptions;

    // Set up a dialog with default options
	err = NavGetDefaultDialogCreationOptions(&dialogOptions);
	assert(err == noErr);
	dialogOptions.modality = kWindowModalityAppModal;	
	NavOpenDialogUPP = NewNavEventUPP(AppNavOpenDialog::AppNavOpenDialogEventHandler);	

	err =  NavCreateGetFileDialog(&dialogOptions, NULL, NavOpenDialogUPP, 
				NULL, NULL, (void *)derived_ptr, &theOpenDialog);
	assert(err == noErr);
	assert(theOpenDialog);

	// specify to only allow text-type files
	CFMutableArrayRef identifiers = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	if(text_files_only) {
		CFStringRef uti_txt = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, CFSTR("txt"), kUTTypeData );
		CFArrayAppendValue(identifiers, uti_txt);
		CFStringRef uti_gomsl = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, CFSTR("gomsl"), kUTTypeData );
		CFArrayAppendValue(identifiers, uti_gomsl);
		CFRelease(uti_txt);
		CFRelease(uti_gomsl);
		}
	else {
		CFStringRef uti_dylib = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, CFSTR("dylib"), kUTTypeData );
		CFArrayAppendValue(identifiers, uti_dylib);
		CFRelease(uti_dylib);
		}
//	CFStringRef uti_TEXT = UTTypeCreatePreferredIdentifierForTag(kUTTagClassOSType, CFSTR("TEXT"), kUTTypeData );
//	CFArrayAppendValue(identifiers, uti_TEXT);

	err = NavDialogSetFilterTypeIdentifiers(theOpenDialog, identifiers);
	assert(err == noErr);
	
	err = NavDialogRun(theOpenDialog);
	if(err != noErr) 
		NavDialogDispose(theOpenDialog);	// dispose of it if something went wrong

	CFRelease(identifiers);
//	CFRelease(uti_TEXT);
}


void AppNavOpenDialog::AppNavOpenDialogEventHandler (NavEventCallbackMessage callBackSelector, 
                     NavCBRecPtr callBackParms, void* userData )
{
	OSStatus err = noErr;
	
	AppNavOpenDialog * derived_ptr = static_cast<AppNavOpenDialog *>(userData);
	
	// Determine which event message Nav Services and react accordingly
	switch (callBackSelector) {		
		case kNavCBUserAction: {
			NavReplyRecord reply;
			// Get the reply record
			err = NavDialogGetReply(callBackParms->context, &reply);
//			assert(err == noErr);
			// Determine the user action 
			NavUserAction userAction = NavDialogGetUserAction(callBackParms->context);	
			switch(userAction) {
				case kNavUserActionOpen: {
					derived_ptr->get_open_file_path_name(&reply);
					// if non-zero-length strings returned, set a success flag
//					derived_ptr->handle_file_open();
					break;
					}
//				case kNavUserActionSaveAs: {
//					   err = AppDoFSRefSave( &reply );
//					break;
//	  				}
				}
			    /* Dispose of the reply record when we're done */
			   NavDisposeReply(&reply);
		    break;
			}
		// The dialog terminated. Never assume termination
		//   before NS notifies your application 
		case kNavCBTerminate:
		{
		    // Dispose of the dialog record
		    NavDialogDispose (callBackParms->context);
			break;
		}
    }
}


void  AppNavOpenDialog::get_open_file_path_name(NavReplyRecord *reply) 
{                              
    OSStatus 		err;

	// believe it or not, we have to go through this AppleEvent thingy ...
    // first get the FSRef
	AEDesc actualDesc;
	err = AECoerceDesc(&reply->selection, typeFSRef, &actualDesc);
	assert(err == noErr);
	assert(&actualDesc);
    FSRef fileToOpen;
	err = AEGetDescData(&actualDesc, (void *)(&fileToOpen), sizeof(FSRef));
	assert(err == noErr);
	// now extract the file path and the file name as plain ASCII strings
	char buffer[PATH_MAX];
	err = FSRefMakePath(&fileToOpen, (UInt8 *)buffer, PATH_MAX-1);
	assert(err == noErr);
	file_path = buffer;		// save the complete file path string
    HFSUniStr255 theFileName;
	FSRef parentref;
	err = FSGetCatalogInfo(&fileToOpen, kFSCatInfoNone, NULL, &theFileName, NULL, &parentref);
	assert(err == noErr);
	// get the path for the parent directory
	err = FSRefMakePath(&parentref, (UInt8 *)buffer, PATH_MAX-1);
	assert(err == noErr);
	directory_path = buffer;		// save the complete file path string

	// get the file name string
    CFStringRef fileNameCFString;   
    fileNameCFString =  CFStringCreateWithCharacters (NULL, theFileName.unicode, theFileName.length);
	assert(fileNameCFString);
	CFStringGetCString(fileNameCFString, buffer, PATH_MAX-1, kCFStringEncodingUTF8);
	file_name = buffer;		// save the file name string
	CFRelease(fileNameCFString); // we created it ...
	AEDisposeDesc(&actualDesc);

	if(directory_path.length() > 0 && file_name.length() > 0 && file_path.length() > 0 && file_path.length() < PATH_MAX-1)
		success = true;
}

// if a non-empty file name is supplied, it becomes the initial file name
AppNavSaveAsDialog::AppNavSaveAsDialog(const string& file_name_) : file_name(file_name_), success(false)
{
	initialize(this);
}

AppNavSaveAsDialog::~AppNavSaveAsDialog()
{
	DisposeNavEventUPP (NavSaveAsDialogUPP);
}

void AppNavSaveAsDialog::initialize(AppNavSaveAsDialog * derived_ptr)
{
    OSStatus err = noErr;
    NavDialogRef theSaveAsDialog;
    NavDialogCreationOptions dialogOptions;

    // Set up a dialog with default options
	err = NavGetDefaultDialogCreationOptions(&dialogOptions);
	assert(err == noErr);

	// if a file_name was specified, make it show in the dialog initially
	// note that the file_name will be replaced by the one in the dialog, which might be modified by the user
	if(!file_name.empty())
		dialogOptions.saveFileName = CFStringCreateWithCString(kCFAllocatorDefault, file_name.c_str(), kCFStringEncodingMacRoman);

	dialogOptions.modality = kWindowModalityAppModal;	
	NavSaveAsDialogUPP = NewNavEventUPP(AppNavSaveAsDialog::AppNavSaveAsDialogEventHandler);	

	err =  NavCreatePutFileDialog(&dialogOptions, 'TEXT', kNavGenericSignature, 
				NavSaveAsDialogUPP, (void *)derived_ptr, &theSaveAsDialog);
	assert(err == noErr);
	assert(theSaveAsDialog);
	
	err = NavDialogRun(theSaveAsDialog);
	if(err != noErr) 
		NavDialogDispose(theSaveAsDialog);	// dispose of it if something went wrong ?

	// Release the CF string                     
	if (dialogOptions.saveFileName != NULL)     	
		CFRelease (dialogOptions.saveFileName);
}


void AppNavSaveAsDialog::AppNavSaveAsDialogEventHandler (NavEventCallbackMessage callBackSelector, 
                     NavCBRecPtr callBackParms, void* userData )
{
	OSStatus err = noErr;
	
	AppNavSaveAsDialog * derived_ptr = static_cast<AppNavSaveAsDialog *>(userData);
	
	// Determine which event message Nav Services and react accordingly
	switch (callBackSelector) {		
		case kNavCBUserAction: {
			NavReplyRecord reply;
			// Get the reply record
			err = NavDialogGetReply(callBackParms->context, &reply);
//			assert(err == noErr);
			// Determine the user action 
			NavUserAction userAction = NavDialogGetUserAction(callBackParms->context);	
			switch(userAction) {
//				case kNavUserActionOpen: {
//					derived_ptr->get_open_file_path_name(&reply);
					// if non-zero-length strings returned, set a success flag
//					derived_ptr->handle_file_open();
//					break;
//					}
				case kNavUserActionSaveAs: {
					derived_ptr->get_save_file_path_name(&reply);
					derived_ptr->handle_file_save();
//					   err = AppDoFSRefSave( &reply );
					break;
	  				}
				}
			    /* Dispose of the reply record when we're done */
			   NavDisposeReply(&reply);
		    break;
			}
		// The dialog terminated. Never assume termination
		//   before NS notifies your application 
		case kNavCBTerminate:
		{
		    // Dispose of the dialog record
		    NavDialogDispose (callBackParms->context);
			break;
		}
    }
}


void  AppNavSaveAsDialog::get_save_file_path_name(NavReplyRecord *reply) 
{                              
    OSErr 		err = noErr;
    FSRef 		fileRefParent;
    AEDesc		actualDesc;
	char buffer[PATH_MAX];

	CFStringGetCString(reply->saveFileName, buffer, PATH_MAX-1, kCFStringEncodingUTF8);
	file_name = buffer;
    //Check for valid FSRef by coercing Apple event descriptor
    err = AECoerceDesc(&reply->selection, typeFSRef, &actualDesc);
	assert(err == noErr);
	err = AEGetDescData(&actualDesc, (void *) &fileRefParent, sizeof (FSRef));
	assert(err == noErr);
	err = FSRefMakePath(&fileRefParent, (UInt8 *)buffer, PATH_MAX-1);
	assert(err == noErr);
	directory_path = buffer;
//	cout << "filename is " << file_name << endl;
//	cout << "directory path is " << directory_path << endl;
	file_path = directory_path;
	file_path += "/";
	file_path += file_name;
//	cout << "Full file path is " << file_path << endl;
	AEDisposeDesc(&actualDesc);
/*
    //Check for valid FSRef by coercing Apple event descriptor
    if ((err = AECoerceDesc (&reply->selection, typeFSRef, &actualDesc )) == noErr) 
 	{
            //Check descriptor data 
            if ((err = AEGetDescData (&actualDesc, (void *) &fileRefParent, sizeof (FSRef))) == noErr ) 
                {
					err = FSRefMakePath(&fileRefParent, (UInt8 *)buffer, PATH_MAX-1);
					file_path = buffer;
					cout << "Path is " << file_path << endl;
					CFStringGetCString(reply->saveFileName, buffer, PATH_MAX-1, kCFStringEncodingUTF8);
					file_name = buffer;
					cout << "Filename is " << file_name << endl;
					file_path += "/";
					file_path += file_name;
					cout << "Full save path is " << file_path << endl;
                   }
                AEDisposeDesc (&actualDesc);
	}	
	*/

	if(file_name.length() > 0 && file_path.length() > 0 && file_path.length() < PATH_MAX-1)
		success = true;
}

/*
#define kMTPDocType			'iTin'	// Chap. 11
#define kMTPApplicationSignature	'MTPP'	// Chap. 6

    
OSStatus ShowMyNavSaveAsDialog ()
{
    OSStatus	 	err = noErr;
    NavDialogRef 	theSaveDialog;
    NavDialogCreationOptions dialogOptions;

    // Set up a dialog with default options 
    if ((err = NavGetDefaultDialogCreationOptions (&dialogOptions )) == noErr) 
    {
        // Set document window title as the default filename  
//        err = CopyWindowTitleAsCFString (theItineraryWindow, &dialogOptions.saveFileName);

//		CFStringRef save_name = CFStringCreateWithCString(kCFAllocatorDefault, g_file_read_name.c_str(), kCFStringEncodingMacRoman);
//		dialogOptions.saveFileName = save_name;
		
		dialogOptions.saveFileName = CFStringCreateWithCString(kCFAllocatorDefault, g_file_read_name.c_str(), kCFStringEncodingMacRoman);
		
        dialogOptions.modality = kWindowModalityAppModal;
        // Obtain a UPP for our event-handling function 
        gEventProc = NewNavEventUPP (AppNavEventCallback);
        //Create the Save dialog
        if ((err = NavCreatePutFileDialog (&dialogOptions,  kMTPDocType,
                                kMTPApplicationSignature, gEventProc, 
                                NULL,  &theSaveDialog)) == noErr) 
            {
                // Check for valid dialog reference 
                if (theSaveDialog != NULL) 
                    {
                        // If dialog fails to run, dispose of dialog ref and UPP 		
                        if (( err = NavDialogRun (theSaveDialog)) != noErr)
                          {
                                NavDialogDispose (theSaveDialog);
                                DisposeNavEventUPP (gEventProc);
                            }
                    }
            } 
//		CFRelease(save_name);
        // Check for valid save filename        	
        if (dialogOptions.saveFileName != NULL)     	
        // Release the CF string                     
			CFRelease (dialogOptions.saveFileName);
    }
    return err;
}

OSErr get_save_file_path_name (NavReplyRecord* reply)
{
    OSErr 		err = noErr;
    FSRef 		fileRefParent;
    AEDesc		actualDesc;
	char buffer[PATH_MAX];

    //Check for valid FSRef by coercing Apple event descriptor
    if ((err = AECoerceDesc (&reply->selection, typeFSRef, &actualDesc )) == noErr) 
 	{
            //Check descriptor data 
            if ((err = AEGetDescData (&actualDesc, (void *) &fileRefParent, sizeof (FSRef))) == noErr ) 
                {
					err = FSRefMakePath(&fileRefParent, (UInt8 *)buffer, PATH_MAX-1);
					file_path = buffer;
					cout << "Path is " << file_path << endl;
					CFStringGetCString(reply->saveFileName, buffer, PATH_MAX-1, kCFStringEncodingUTF8);
					file_name = buffer;
					cout << "Filename is " << file_name << endl;
					file_path += "/";
					file_path += file_name;
					cout << "Full save path is " << file_path << endl;
                   }
                AEDisposeDesc (&actualDesc);
	}		
    return err;
}



string g_file_read_path;
string g_file_read_name;
string g_save_file_path;
string g_my_file_text;
		case kMyReadCommand:
			g_file_read_path.clear();
			result = ShowMyNavOpenDialog();
			// open and read the file
			read_my_file();
			break;
		case kMySpewCommand:
			result = ShowMyNavSaveAsDialog();
			write_my_file();
			break;

void read_my_file()
{
	if(g_file_read_path.length() < PATH_MAX-1) {	// check for complete path
		ifstream infile(g_file_read_path.c_str());
		string line;
		g_my_file_text.clear();
		while(getline(infile, line)) {
			cout << line << endl;
			g_my_file_text += line;
			g_my_file_text += "\n";
			}
		infile.close();
		}
}


void write_my_file()
{
	if(g_save_file_path.length() < PATH_MAX-1 && g_save_file_path.length() > 0) {	// check for complete path
		ofstream outfile(g_save_file_path.c_str());
		if(outfile.good()) {
			g_my_file_text = g_my_file_text + g_my_file_text;
			outfile << g_my_file_text;
		outfile.close();
		}
	}
}

*/
