#include "peh_table.h"
#include "SDK.hpp"
#include "sdk_utils.h"
#include "peh_plate.h"
#include "error_utils.h"

#include <windows.h>
#include <MinHook.h>
#pragma comment(lib, "libMinHook.x64.lib")

polaris::tables::PehTable* gpPehTable;

namespace polaris
{
    namespace tables
    {
        PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;
        PVOID ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
        {
            if (pObject != nullptr && pFunction != nullptr)
            {
                if (gpPehTable->m_lpPlates.size() > 0)
                {
                    tables::plates::PehPlate* primaryTable = dynamic_cast<tables::plates::PehPlate*>(gpPehTable->m_lpPlates.front());

                    primaryTable->Update();
                    primaryTable->ProcessEventHook(pObject, pFunction, pParams);
                }
            }

            return ProcessEvent(pObject, pFunction, pParams);
        }

        PVOID(*CollectGarbageInternal)(uint32_t, bool) = nullptr;
        PVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
        {
            return NULL;
        }

        PehTable::PehTable()
        {
            if (gpPehTable != nullptr)
                utilities::ErrorUtils::ThrowException(L"gpPehTable is not null.");

            gpPehTable = this;

            auto pProcessEventAddress = utilities::SDKUtils::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
            if (!pProcessEventAddress)
                utilities::ErrorUtils::ThrowException(L"ProcessEvent address was not found.");

            auto pCollectGarbageInternalAddress = utilities::SDKUtils::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");
            if (!pCollectGarbageInternalAddress)
                utilities::ErrorUtils::ThrowException(L"Finding pattern for CollectGarbageInternal has failed. Please relaunch Fortnite and try again!");

            MH_CreateHook(static_cast<LPVOID>(pCollectGarbageInternalAddress), CollectGarbageInternalHook, reinterpret_cast<LPVOID*>(&CollectGarbageInternal));
            MH_EnableHook(static_cast<LPVOID>(pCollectGarbageInternalAddress));

            MH_CreateHook(static_cast<LPVOID>(pProcessEventAddress), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
            MH_EnableHook(static_cast<LPVOID>(pProcessEventAddress));
        }
    }
}