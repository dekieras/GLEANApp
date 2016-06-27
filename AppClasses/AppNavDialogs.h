/*
 *  AppNavDialogs.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/15/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#ifndef APPNAVDIALOGS_H
#define APPNAVDIALOGS_H

#include <Carbon/Carbon.h>
#include <string>

class AppNavOpenDialog {
public:
	AppNavOpenDialog(bool text_files_only_ = true);
	virtual ~AppNavOpenDialog();

	bool successful() const {return success;}
	const std::string& get_file_name() const {return file_name;}
	const std::string& get_directory_path() const {return directory_path;}
	const std::string& get_file_path() const {return file_path;}

protected:
	// called from the derived class constructors to provide the appropriate "this" pointer.
	void initialize(AppNavOpenDialog * derived_pointer);

	// the static event handler function - all events are routed to this handler and dispatched to specific
	// handlers for the event class - protected to allow derived classes to install it for other events
	static void AppNavOpenDialogEventHandler(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void* userData );

	virtual void handle_file_open() {}	// override to handle open using file_path and file_name

private:
	NavEventUPP NavOpenDialogUPP;			// the associated UPP
	bool text_files_only;
	std::string file_name;			// the file name
	std::string directory_path;		// the path for the enclosing directory
	std::string file_path;			// the complete path for the file
	bool success;
	
	// helpers
	void get_open_file_path_name(NavReplyRecord *reply);
};

class AppNavSaveAsDialog {
public:
	AppNavSaveAsDialog(const std::string& file_name_ = "");
	virtual ~AppNavSaveAsDialog();

	bool successful() const {return success;}
	const std::string& get_file_name() const {return file_name;}
	const std::string& get_directory_path() const {return directory_path;}
	const std::string& get_file_path() const {return file_path;}

protected:
	// called from the derived class constructors to provide the appropriate "this" pointer.
	void initialize(AppNavSaveAsDialog * derived_pointer);

	// the static event handler function - all events are routed to this handler and dispatched to specific
	// handlers for the event class - protected to allow derived classes to install it for other events
	static void AppNavSaveAsDialogEventHandler(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void* userData );

	virtual void handle_file_save() {}	// override to handle save using file_path and file_name
	
private:
	NavEventUPP NavSaveAsDialogUPP;			// the associated UPP
	std::string file_name;			// the file name
	std::string directory_path;		// the path for the enclosing directory
	std::string file_path;			// the complete path for the file
	bool success;
	
	// helpers
	void get_save_file_path_name(NavReplyRecord *reply);
};

#endif
