#ifndef IDLE_PLATE_H
#define IDLE_PLATE_H

#include "plate.h"

namespace polaris
{
    class BasePlate : public Plate
    {
    public:
        void OnEnabled() override;
    };
}

#endif // IDLE_PLATE_H