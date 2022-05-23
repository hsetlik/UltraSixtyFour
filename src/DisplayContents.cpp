#include "DisplayContents.h"

using namespace DisplayContents;

Content::Content(uint16_t w, uint16_t h) : width(w),
                                           height(h),
                                           messages({})
{
}

Content::~Content()
{
}