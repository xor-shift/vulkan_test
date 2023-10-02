#pragma once

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)
