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

#ifndef IOElectrify_h
#define IOElectrify_h

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>

#include "common.h"
#include "WMI.h"

#define INTEL_WMI_THUNDERBOLT_GUID "86ccfd48-205e-4a77-9c48-2021cbede341"

class IOElectrify : public IOService
{
    OSDeclareDefaultStructors(IOElectrify);
    typedef IOService super;

protected:
    IOPCIDevice* mProvider;
    WMI* mWMI;

public:
    virtual bool init(OSDictionary *propTable);
    virtual bool attach(IOService *provider);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
    virtual void free();
#ifdef DEBUG
    virtual void detach(IOService *provider);
#endif
    
    virtual IOReturn setPowerState(unsigned long powerState, IOService *service);
};


#endif
