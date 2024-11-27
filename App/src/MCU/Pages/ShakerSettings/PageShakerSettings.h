#pragma once

#include <PageBasicSettings.h>

class TPageShakerSettings : public TPageBasicSettings
{
public:
    TPageShakerSettings(std::string Name);
private:
    void fillPageContainer(void) override;
};