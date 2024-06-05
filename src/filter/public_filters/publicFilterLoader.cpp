#include "publicFilterLoader.hpp"
#include "FramePixelValueProcess.hpp"
#include "HdrMergeProcess.hpp"
#include "SequenceIdProcess.hpp"

namespace libobsensor {
publicFilterCreator::publicFilterCreator(std::function<std::shared_ptr<IFilter>()> creatorFunc) : creatorFunc_(creatorFunc) {}

std::shared_ptr<IFilter> publicFilterCreator::create() {
    return creatorFunc_();
}


namespace PublicFilterCreatorLoader {

#define ADD_FILTER_CREATOR(name)                                                                     \
    {                                                                                                \
        #name, std::make_shared<publicFilterCreator>([]() { return std::make_shared<name>(#name); }) \
    }

std::map<std::string, std::shared_ptr<IFilterCreator>> getCreators() {
    static std::map<std::string, std::shared_ptr<IFilterCreator>> filterCreators = {
        ADD_FILTER_CREATOR(PixelValueScaler),
        ADD_FILTER_CREATOR(PixelValueCutOff),
        ADD_FILTER_CREATOR(HdrMerge),
        ADD_FILTER_CREATOR(SequenceIdFilter),

    };

    return filterCreators;
}

}  // namespace publicFilterLoader
}  // namespace libobsensor