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

#include <IOKit/IOLib.h>
#include "IOElectrify.h"
#include "WMI.h"

#include <libkern/version.h>
extern kmod_info_t kmod_info;

// define & enumerate power states
enum
{
    kPowerStateSleep    = 0,
    kPowerStateDoze     = 1,
    kPowerStateNormal   = 2,
    kPowerStateCount
};

// Define usable power states
static IOPMPowerState powerStateArray[ kPowerStateCount ] =
{
    { 1,0,0,0,0,0,0,0,0,0,0,0 },
    { 1,kIOPMDeviceUsable, kIOPMDoze, kIOPMDoze, 0,0,0,0,0,0,0,0 },
    { 1,kIOPMDeviceUsable, IOPMPowerOn, IOPMPowerOn, 0,0,0,0,0,0,0,0 }
};

OSDefineMetaClassAndStructors(IOElectrify, IOService);

bool IOElectrify::init(OSDictionary *propTable)
{
    DebugLog("IOElectrify::init() %p\n", this);
    
    // announce version
    IOLog("IOElectrify: Version %s starting on OS X Darwin %d.%d.\n", kmod_info.version, version_major, version_minor);

    bool ret = super::init(propTable);
    if (!ret)
    {
        AlwaysLog("super::init returned false\n");
        return false;
    }

    // place version/build info in ioreg properties DV,Build and DV,Version
    char buf[128];
    snprintf(buf, sizeof(buf), "%s %s", kmod_info.name, kmod_info.version);
    setProperty("DV,Version", buf);
#ifdef DEBUG
    setProperty("DV,Build", "Debug-darkvoid");// LOGNAME);
#else
    setProperty("DV,Build", "Release-darkvoid");// LOGNAME);
#endif

    mProvider = NULL;
    
    return true;
}

bool IOElectrify::attach(IOService* provider)
{
    DebugLog("IOElectrify::attach() %s\n", provider->getName());

    return super::attach(provider);
}

bool IOElectrify::start(IOService *provider)
{
    DebugLog("IOElectrify::start() %s\n", provider->getName());
    
    if (!super::start(provider))
    {
        AlwaysLog("super::start returned false\n");
        return false;
    }
    
    bool result = false;
    
    mWMI = new WMI(provider);

    if (mWMI->initialize()) {

        if (mWMI->hasMethod(INTEL_WMI_THUNDERBOLT_GUID)) {
            result = true;
        }
    }

    if (result) {
        // init power state management & set state as PowerOn
        PMinit();
        registerPowerDriver(this, powerStateArray, kPowerStateCount);
        provider->joinPMtree(this);
    }
    
    return result;
}

void IOElectrify::stop(IOService *provider)
{
    DebugLog("IOElectrify::stop() %p\n", this);
    
    PMstop();
    
    if (mWMI != NULL) {
        delete mWMI;
        mWMI = NULL;
    }

    super::stop(provider);
}

void IOElectrify::free()
{
    DebugLog("IOElectrify::free() %p\n", this);

    super::free();
}

#ifdef DEBUG
void IOElectrify::detach(IOService *provider)
{
    DebugLog("IOElectrify::detach() %p\n", this);

    return super::detach(provider);
}
#endif

IOReturn IOElectrify::setPowerState(unsigned long powerState, IOService *service)
{
    DebugLog("setPowerState %ld\n", powerState);
    
    switch (powerState)
    {
        case kPowerStateSleep:
            DebugLog("--> sleep(%d)\n", (int)powerState);

            if (mWMI->hasMethod(INTEL_WMI_THUNDERBOLT_GUID)) {
                
                OSObject* params[3] = {
                    OSNumber::withNumber(0ULL, 32),
                    OSNumber::withNumber(0ULL, 32),
                    OSNumber::withNumber(0ULL, 32)
                };
                
                if (mWMI->executeMethod(INTEL_WMI_THUNDERBOLT_GUID, NULL, params, 3)) {
                    AlwaysLog("Thunderbolt force-power: OFF.\n");
                }
            }
            
            break;

        case kPowerStateDoze:
        case kPowerStateNormal:
            DebugLog("--> awake(%d)\n", (int)powerState);
            
            if (mWMI->hasMethod(INTEL_WMI_THUNDERBOLT_GUID)) {
                
                OSObject* params[3] = {
                    OSNumber::withNumber(0ULL, 32),
                    OSNumber::withNumber(0ULL, 32),
                    OSNumber::withNumber(1ULL, 32)
                };
                
                if (mWMI->executeMethod(INTEL_WMI_THUNDERBOLT_GUID, NULL, params, 3)) {
                    AlwaysLog("Thunderbolt force-power: ON.\n");
                }
            }
            break;
    }
    
    return IOPMAckImplied;
}
