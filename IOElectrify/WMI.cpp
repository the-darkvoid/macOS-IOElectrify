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
 * Based on Dolnor's IOWMIFamily (https://github.com/Dolnor/IOWMIFamily/)
 */

#include "WMI.h"
#include <uuid/uuid.h>

#define kWMIMethod "_WDG"

#define kWMIGuid "guid"
#define kWMIObjectId "object-id"
#define kWMINotifyId "notify-id"
#define kWMIInstanceCount "instance-count"
#define kWMIFlags "flags"
#define kWMIFlagsText "flags-text"

/*
 * If the GUID data block is marked as expensive, we must enable and
 * explicitily disable data collection.
 */
#define ACPI_WMI_EXPENSIVE   0x1
#define ACPI_WMI_METHOD      0x2    /* GUID is a method */
#define ACPI_WMI_STRING      0x4    /* GUID takes & returns a string */
#define ACPI_WMI_EVENT       0x8    /* GUID is an event */

struct __attribute__((packed)) WMI_DATA
{
    uuid_t guid;
    union {
        char object_id[2];
        struct {
            unsigned char notify_id;
            unsigned char reserved;
        };
    };
    UInt8 instance_count;
    UInt8 flags;
};

#define WMI_DATA_SIZE sizeof(WMI_DATA)

// Convert UUID to little endian
void le_uuid_dec(uuid_t *in, uuid_t *out)
{
    *((uint32_t *)out + 0) = OSSwapInt32(*((uint32_t *)in + 0));
    *((uint16_t *)out + 2) = OSSwapInt16(*((uint16_t *)in + 2));
    *((uint16_t *)out + 3) = OSSwapInt16(*((uint16_t *)in + 3));
    *((uint64_t *)out + 1) =            (*((uint64_t *)in + 1));
}

// parseWMIFlags - Parse WMI flags to a string
OSString * parseWMIFlags(UInt8 flags)
{
    char output[80];

    memset(&output, 0, sizeof(output));

    if (flags != 0)
    {
        if (flags & ACPI_WMI_EXPENSIVE)
        {
            strcat(output, "ACPI_WMI_EXPENSIVE ");
        }
        
        if (flags & ACPI_WMI_METHOD)
        {
            strcat(output, "ACPI_WMI_METHOD ");
        }
        
        if (flags & ACPI_WMI_STRING)
        {
            strcat(output, "ACPI_WMI_STRING ");
        }
        
        if (flags & ACPI_WMI_EVENT)
        {
            strcat(output, "ACPI_WMI_EVENT ");
        }
    }
    else
    {
        strcat(output, "NONE");
    }

    return (OSString::withCString(output));
}

WMI::WMI(IOService* provider)
{
    mDevice = OSDynamicCast(IOACPIPlatformDevice, provider);
}

bool WMI::initialize()
{
    if (mDevice != NULL) {
        mData = OSArray::withCapacity(0);
            
        if (extractData()) {
            return true;
        }

        AlwaysLog("WMI method %s not found on object %s\n", kWMIMethod, mDevice->getName());
    }
}

WMI::~WMI()
{
    if (mData != NULL) {
        mData->release();
    }
}

// Parse the _WDG method output for WMI data blocks
bool WMI::extractData()
{
    OSObject *wdg;
    OSData *data;

    if (mDevice->evaluateObject(kWMIMethod, &wdg) != kIOReturnSuccess)
    {
        AlwaysLog("ACPI object %s does not export _WDG data\n", mDevice->getName());
        return false;
    }
    
    data = OSDynamicCast(OSData, wdg);
    
    if (data == NULL) {
        AlwaysLog("%s:_WDG did not return a data blob\n", mDevice->getName());
        return false;
    }

    int count = data->getLength() / WMI_DATA_SIZE;
    
    for (int i = 0; i < count; i++) {
        parseWDGEntry(
          (struct WMI_DATA*)data->getBytesNoCopy(i * WMI_DATA_SIZE, WMI_DATA_SIZE));
    }
    
    mDevice->setProperty("WDG", mData);
    
    data->release();
    
    return true;
}

// Parse WDG datablock into an OSArray
void WMI::parseWDGEntry(struct WMI_DATA* block)
{
    char guid_string[37];
    char object_id_string[3];
    OSDictionary *dict = OSDictionary::withCapacity(6);
    
    uuid_t hostUUID;
    
    le_uuid_dec(&block->guid, &hostUUID);
    uuid_unparse_lower(hostUUID, guid_string);

    dict->setObject(kWMIGuid, OSString::withCString(guid_string));

    if (block->flags & ACPI_WMI_EVENT)
        dict->setObject(kWMINotifyId, OSNumber::withNumber(block->notify_id, 8));
    else
    {
        snprintf(object_id_string, 3, "%c%c", block->object_id[0], block->object_id[1]);
        dict->setObject(kWMIObjectId, OSString::withCString(object_id_string));
    }
    
    dict->setObject(kWMIInstanceCount, OSNumber::withNumber(block->instance_count, 8));
    dict->setObject(kWMIFlags, OSNumber::withNumber(block->flags, 8));
    
#ifdef DEBUG
    dict->setObject(kWMIFlagsText, parseWMIFlags(block->flags));
#endif
    
    mData->setObject(dict);
}

OSDictionary* WMI::getMethod(const char * guid)
{
    if (mData && mData->getCount() > 0)
    {
        if (OSCollectionIterator* iterator = OSCollectionIterator::withCollection(mData))
        {
            OSDictionary* entry;
            
            while ((entry = OSDynamicCast(OSDictionary, iterator->getNextObject())))
            {
                OSString * methodGUID = OSDynamicCast(OSString, entry->getObject(kWMIGuid));
                OSNumber * flags = OSDynamicCast(OSNumber, entry->getObject(kWMIFlags));

                if (methodGUID && flags
                    && flags->unsigned32BitValue() & ACPI_WMI_METHOD
                    && strncmp(methodGUID->getCStringNoCopy(), guid, strlen(guid)) == 0) {
                    return entry;
                }
            }
            iterator->release();
        }
    }
    
    return NULL;
}

bool WMI::hasMethod(const char * guid)
{
    OSDictionary* method = getMethod(guid);
    
    if (method != NULL) {
        OSString * methodId = OSDynamicCast(OSString, method->getObject(kWMIObjectId));
            
        if (methodId != NULL) {
            DebugLog("found method %s with guid %s\n", methodId->getCStringNoCopy(), guid);
            return true;
        }
    }

    return false;
}

bool WMI::executeMethod(const char * guid, OSObject ** result, OSObject * params[], IOItemCount paramCount)
{
    char methodName[16];
    OSDictionary* method = getMethod(guid);
    
    if (method != NULL)
    {
        OSString * methodId = OSDynamicCast(OSString, method->getObject(kWMIObjectId));
        
        if (methodId != NULL) {
            memset(&methodName, 0, sizeof(methodName));

            strcat(methodName, "WM");
            strcat(methodName, methodId->getCStringNoCopy());
            
            DebugLog("Calling method %s\n", methodName);
            mDevice->evaluateObject(methodName, result, params, paramCount);
            
            return true;
        }
    }
    
    return false;
}
