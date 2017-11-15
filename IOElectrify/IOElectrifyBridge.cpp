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
#include "IOElectrifyBridge.h"

#include <libkern/version.h>
extern kmod_info_t kmod_info;

OSDefineMetaClassAndStructors(IOElectrifyBridge, IOService);

bool IOElectrifyBridge::init(OSDictionary *propTable)
{
    DebugLog("IOElectrifyBridge::init() %p\n", this);
    
    // announce version
    IOLog("IOElectrifyBridge: Version %s starting on OS X Darwin %d.%d.\n", kmod_info.version, version_major, version_minor);
    
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

bool IOElectrifyBridge::attach(IOService* provider)
{
    DebugLog("IOElectrifyBridge::attach() %s\n", provider->getName());
    
    mProvider = OSDynamicCast(IOPCI2PCIBridge, provider);
    
    if (mProvider == NULL) {
        return false;
    }

    DebugLog("Provider -> Provider %s\n", mProvider->getProvider()->getName());

    if (strcmp(mProvider->getProvider()->getName(), "PEG2")) {
        DebugLog("Mismatch %s vs PEG2\n", mProvider->getProvider()->getName());
        return false;
    }

    return super::attach(provider);
}

bool IOElectrifyBridge::start(IOService *provider)
{
    DebugLog("IOElectrifyBridge::start() %s\n", provider->getName());
    
    if (!super::start(provider))
    {
        AlwaysLog("super::start returned false\n");
        return false;
    }
    
    SInt32 score = 0;
    
    //mProvider->probe(mProvider, &score);
    
    //DebugLog("Probe score: %d", score);
    
    IOOptionBits options = 0;
    
    mProvider->requestProbe(options);
    
    return true;
}

void IOElectrifyBridge::stop(IOService *provider)
{
    DebugLog("IOElectrifyBridge::stop() %p\n", this);
    
    super::stop(provider);
}

void IOElectrifyBridge::free()
{
    DebugLog("IOElectrifyBridge::free() %p\n", this);
    
    super::free();
}

#ifdef DEBUG
void IOElectrifyBridge::detach(IOService *provider)
{
    DebugLog("IOElectrifyBridge::detach() %p\n", this);
    
    return super::detach(provider);
}
#endif

