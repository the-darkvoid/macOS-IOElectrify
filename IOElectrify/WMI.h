/*
 *  Released under "The GNU General Public License (GPL-2.0)"
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef WMI_h
#define WMI_h

#include "common.h"

#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

class WMI
{
    IOACPIPlatformDevice* mDevice = NULL;
    OSArray* mData = NULL;

public:
    // Constructor
    WMI(IOService *provider);
    // Destructor
    ~WMI();
    
    bool initialize();
    bool hasMethod(const char * guid);
    bool executeMethod(const char * guid, OSObject ** result = NULL, OSObject * params[] = NULL, IOItemCount paramCount = NULL);
    
    inline IOACPIPlatformDevice* getACPIDevice() { return mDevice; }
    
private:
    bool extractData();
    void parseWDGEntry(struct WMI_DATA * block);
    
    OSDictionary* getMethod(const char * guid);
};


#endif /* WMI_h */
