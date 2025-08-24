#pragma once

namespace Version {
    constexpr const char* GIT_HASH = "01016e3";
    constexpr const char* BUILD_TYPE = 
#ifdef WEB_BUILD
        "Web";
#else
        "Desktop";
#endif
    constexpr const char* VERSION = "1.0.0";
}
