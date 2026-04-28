
#include "BufAttributes.hpp"

#include <vector>
#include <string>

static const char *labelsVec[] LABELS_ATTRIBUTE =
{
    "person",
};

bool GetLabelsVector(std::vector<std::string> &labels)
{
    constexpr size_t labelsSz = 1;
    labels.clear();

    if (!labelsSz)
    {
        return false;
    }

    labels.reserve(labelsSz);

    for (size_t i = 0; i < labelsSz; ++i)
    {
        labels.emplace_back(labelsVec[i]);
    }

    return true;
}

